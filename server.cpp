#include <iostream>
#include <boost/asio.hpp>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <hiredis/hiredis.h>
#include <unordered_map>
#include <string>
#include <sstream>
#include <mutex>
#include <thread>
#include <memory>
#include <bcrypt/BCrypt.hpp>

using boost::asio::ip::tcp;
using namespace std;

sql::mysql::MySQL_Driver* driver;
std::unique_ptr<sql::Connection> connection;
redisContext* redis_context;
std::mutex user_mutex;
unordered_map<string, std::shared_ptr<tcp::socket>> online_users;

void init_database() {
    driver = sql::mysql::get_mysql_driver_instance();
    connection.reset(driver->connect("tcp://127.0.0.1:3306", "wang", "123456"));
    connection->setSchema("chat_db");
}

void init_redis() {
    redis_context = redisConnect("127.0.0.1", 6379);
    if (redis_context == nullptr || redis_context->err) {
        if (redis_context) {
            std::cerr << "Redis连接错误: " << redis_context->errstr << "\n";
            redisFree(redis_context);
        } else {
            std::cerr << "Redis连接错误: 无法分配上下文\n";
        }
        exit(1);
    }
}

void handle_request(std::shared_ptr<tcp::socket> socket) {
    try {
        while (true) {
            boost::system::error_code error;
            char data[1024] = {0};
            size_t length = socket->read_some(boost::asio::buffer(data), error);

            if (error == boost::asio::error::eof) {
                break; // 连接关闭
            } else if (error) {
                throw boost::system::system_error(error);
            }

            std::istringstream request_stream(std::string(data, length));
            std::string request_type, request_content;
            std::getline(request_stream, request_type);
            std::getline(request_stream, request_content);

            std::string response;
            if (request_type == "REGISTER") {
                std::istringstream content_stream(request_content);
                std::string username, password;
                std::getline(content_stream, username, ':');
                std::getline(content_stream, password);

                std::string hashed_password = BCrypt::generateHash(password);

                std::lock_guard<std::mutex> lock(user_mutex);
                std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("INSERT INTO users (username, password) VALUES (?, ?)"));
                pstmt->setString(1, username);
                pstmt->setString(2, hashed_password);
                
                try {
                    pstmt->execute();
                    response = "注册成功";
                } catch (sql::SQLException &e) {
                    response = "用户名已存在";
                }
            } else if (request_type == "LOGIN") {
                std::istringstream content_stream(request_content);
                std::string username, password;
                std::getline(content_stream, username, ':');
                std::getline(content_stream, password);

                std::lock_guard<std::mutex> lock(user_mutex);
                std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("SELECT password FROM users WHERE username=?"));
                pstmt->setString(1, username);
                std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

                if (res->next()) {
                    std::string hashed_password = res->getString("password");
                    if (BCrypt::validatePassword(password, hashed_password)) {
                        online_users[username] = socket;
                        response = "登录成功";
                    } else {
                        response = "用户名或密码错误";
                    }
                } else {
                    response = "用户名或密码错误";
                }
            } else if (request_type == "SEND_PRIVATE") {
                std::istringstream content_stream(request_content);
                std::string to_user, private_message;
                std::getline(content_stream, to_user, ':');
                std::getline(content_stream, private_message);

                std::lock_guard<std::mutex> lock(user_mutex);
                if (online_users.find(to_user) != online_users.end()) {
                    boost::asio::write(*online_users[to_user], boost::asio::buffer(private_message + "\n"));
                    response = "私聊消息已发送";
                } else {
                    response = "用户不在线";
                }
            } else if (request_type == "SEND_ALL") {
                std::lock_guard<std::mutex> lock(user_mutex);
                for (auto& [_, user_socket] : online_users) {
                    boost::asio::write(*user_socket, boost::asio::buffer(request_content + "\n"));
                }
                response = "群发消息已发送";
            }

            boost::asio::write(*socket, boost::asio::buffer(response + "\n"));
        }
    } catch (std::exception& e) {
        std::cerr << "处理请求时异常: " << e.what() << "\n";
    }
}

int main() {
    try {
        init_database();
        init_redis();
        
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        while (true) {
            auto socket = std::make_shared<tcp::socket>(io_context);
            acceptor.accept(*socket);

            std::thread(handle_request, socket).detach();
        }
    } catch (std::exception& e) {
        std::cerr << "服务器异常: " << e.what() << "\n";
    }

    return 0;
}
