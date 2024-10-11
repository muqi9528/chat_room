# chat_room
wangxiang
当然，这里是更新后的README文档，包含使用Makefile的说明：

```
# Chat_Room

这是一个使用C++构建的聊天应用程序，包含服务器和客户端，支持多用户消息传递功能。

## 目录

- [简介](#简介)
- [功能](#功能)
- [技术栈](#技术栈)
- [安装](#安装)
- [使用方法](#使用方法)
- [编译](#编译)
- [注意事项](#注意事项)

## 简介

该项目实现了一个简单的聊天应用程序，使用C++进行开发，采用MySQL和Redis作为数据存储，使用Boost库进行增强功能，并实现了多线程同步。

## 功能

- 用户注册和登录
- 消息发送和接收
- 支持多客户端连接
- 数据库存储用户信息
- 缓存消息
- Session和Cookie管理

## 技术栈

- C++
- 套接字编程
- MySQL
- Redis
- Boost库
- 互斥锁（mutex）用于多线程同步
- Session和Cookie管理

## 安装

1. 克隆仓库：
   ```
   git clone https://github.com/yourusername/chatapp.git
   ```
2. 安装依赖：
   - 确保已安装MySQL和Redis
   - 安装Boost库

## 使用方法

1. 使用Makefile编译程序：
   ```
   make
   ```
2. 启动服务器：
   ```
   ./server
   ```
3. 启动客户端：
   ```
   ./client
   ```
4. 注册并登录用户，开始聊天。

## 编译

使用Makefile来编译项目：

- 编译所有目标：
  ```
  make
  ```

- 清理生成的文件：
  ```
  make clean
  ```

## 注意事项

- 确保MySQL和Redis服务正在运行。
- 配置文件中正确设置数据库连接信息。
- 适当配置防火墙以允许必要的端口通信。

```
在Ubuntu上安装和配置这些库的教程如下：

### 1. 安装MySQL Connector/C++（`-lmysqlcppconn`）

1. **更新包列表**：
   ```bash
   sudo apt update
   ```

2. **安装MySQL Connector/C++**：
   ```bash
   sudo apt install libmysqlcppconn-dev
   ```

### 2. 安装hiredis（`-lhiredis`）

1. **安装hiredis库**：
   ```bash
   sudo apt install libhiredis-dev
   ```

### 3. 安装Boost System（`-lboost_system`）

1. **安装Boost库**：
   ```bash
   sudo apt install libboost-system-dev
   ```

### 4. 安装BCrypt（`-lbcrypt`）

1. **安装BCrypt库**：
   ```bash
   sudo apt install libbcrypt-dev
   ```

### 编译和链接

安装这些库后，你可以通过在编译时添加相应的链接选项来使用它们。例如：

```bash
g++ your_code.cpp -o your_program -lmysqlcppconn -lhiredis -lboost_system -lbcrypt
```

这将在编译时链接你所需要的库。确保你的代码中包含相应的头文件。
