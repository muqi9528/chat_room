#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

std::string send_request(tcp::socket& socket, const std::string& request_type, const std::string& request_content) {
    boost::system::error_code error;

    // 发送请求类型和内容
    boost::asio::write(socket, boost::asio::buffer(request_type + "\n"), error);
    if (error) {
        return "发送请求类型时发生错误: " + error.message();
    }
    
    boost::asio::write(socket, boost::asio::buffer(request_content + "\n"), error);
    if (error) {
        return "发送请求内容时发生错误: " + error.message();
    }

    // 接收服务器响应
    char reply[1024] = {0};
    size_t reply_length = socket.read_some(boost::asio::buffer(reply), error);
    
    if (error && error != boost::asio::error::eof) {
        return "接收响应时发生错误: " + error.message();
    }

    return std::string(reply, reply_length);
}

void print_menu() {
    std::cout << "\n命令菜单:\n"
              << "1. 注册\n"
              << "2. 登录\n"
              << "3. 私聊\n"
              << "4. 群发\n"
              << "5. 退出\n"
              << "请选择操作(1-5): ";
}

bool connect_to_server(tcp::socket& socket) {
    try {
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));
        return true;
    } catch (std::exception& e) {
        std::cerr << "连接服务器时异常: " << e.what() << "\n";
        return false;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);

        while (!connect_to_server(socket)) {
            std::cerr << "重试连接...\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

        int choice;
        while (true) {
            print_menu();
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除输入缓冲

            std::string response;
            switch (choice) {
                case 1: {
                    std::string username, password;
                    std::cout << "用户名: ";
                    std::cin >> username;
                    std::cout << "密码: ";
                    std::cin >> password;

                    response = send_request(socket, "REGISTER", username + ":" + password);
                    break;
                }
                case 2: {
                    std::string username, password;
                    std::cout << "用户名: ";
                    std::cin >> username;
                    std::cout << "密码: ";
                    std::cin >> password;

                    response = send_request(socket, "LOGIN", username + ":" + password);
                    break;
                }
                case 3: {
                    std::string to_user, private_message;
                    std::cout << "发送给（用户名）: ";
                    std::cin >> to_user;
                    std::cout << "消息内容: ";
                    std::cin.ignore();
                    std::getline(std::cin, private_message);

                    response = send_request(socket, "SEND_PRIVATE", to_user + ":" + private_message);
                    break;
                }
                case 4: {
                    std::string message;
                    std::cout << "消息内容: ";
                    std::cin.ignore();
                    std::getline(std::cin, message);

                    response = send_request(socket, "SEND_ALL", message);
                    break;
                }
                case 5:
                    std::cout << "退出程序。\n";
                    return 0;
                default:
                    std::cout << "无效选择，请输入1-5。\n";
                    continue;
            }

            std::cout << "服务器回复: " << response << "\n";

            if (response.find("错误") != std::string::npos) {
                std::cerr << "检测到错误，尝试重新连接...\n";
                socket.close();
                while (!connect_to_server(socket)) {
                    std::cerr << "重试连接...\n";
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        }
    } catch (std::exception& e) {
        std::cerr << "异常: " << e.what() << "\n";
    }

    return 0;
}
