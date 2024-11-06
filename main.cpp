#include <asio.hpp>
#include <iostream>
#include <array>
#include <thread>
#include <string>
#include <sstream>
#include <fstream>

using asio::ip::tcp;


int load_port() {
    std::ifstream file("config.txt");
    int port = 8080; // порт по умолчанию

    if (file.is_open()) {
        file >> port;
        file.close();
    }
    return port;
}

void save_port(int port) {
    std::ofstream file("config.txt");
    if (file.is_open()) {
        file << port;
        file.close();
    }
}

void handle_client(tcp::socket socket) {
    std::string address;

    try {
        tcp::endpoint client_endpoint = socket.remote_endpoint();
        address = client_endpoint.address().to_string();

        std::cout << "User " << address << " online " << std::endl;

        std::array<char, 1024> recv_buffer;

        while (true) {
            size_t len = socket.read_some(asio::buffer(recv_buffer));
            std::cout << address << " -> ";
            std::cout.write(recv_buffer.data(), len);
        }
    } catch (std::exception& e) {
        std::cout << "User " << address << " offline" << std::endl;
    }
}

void start_server(asio::io_context& io_context, int& port) {
    try {
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
        std::cout << "Listening for TCP messages on port " << port << "..." << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::thread(handle_client, std::move(socket)).detach();
        }
    } catch (std::exception& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
    }
}

void start_client(asio::io_context& io_context, int& port) {
    tcp::socket socket(io_context);

    asio::error_code ec;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(asio::ip::host_name(), "");
    std::string server_ip;
    for (auto& endpoint : endpoints) {
        if (endpoint.endpoint().address().is_v4()) {
            server_ip = endpoint.endpoint().address().to_string();
            break;
        }
    }

    while (true) {
        std::cout << "Enter command (type 'exit' to quit): ";
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        if (command == "/info") {
            std::cout << "Server IP: " << server_ip << " listening on port " << port << std::endl;
        } else if (command.substr(0, 10) == "/set_port ") {
            int new_port;
            std::istringstream iss(command.substr(10));
            if (!(iss >> new_port) || new_port <= 0 || new_port > 65535) {
                std::cout << "Invalid port number. Please enter a port between 1 and 65535." << std::endl;
                continue;
            }

            port = new_port;
            save_port(port);
            std::cout << "Port updated to " << port << ". Restart the server to apply changes." << std::endl;
            break;  // Останавливаем клиент для перезапуска
        } else if (command.substr(0, 6) == "/send ") {
    std::istringstream iss(command.substr(6));
    std::string user_ip;
    int user_port;
    std::string message;

    // Проверяем, если команда в формате "/send -a <ip> <port>"
    iss >> user_ip;
    if (user_ip == "-a") {
        if (!(iss >> user_ip >> user_port)) {  // Ожидаем IP и порт
            std::cout << "Usage: /send -a <ip> <port>" << std::endl;
            continue;
        }

        try {
            tcp::endpoint endpoint(asio::ip::make_address(user_ip), user_port);
            if (!socket.is_open()) {
                socket.connect(endpoint, ec);
            }

            if (ec) {
                std::cout << "Error connecting to " << user_ip << ":" << user_port << ": " << ec.message() << std::endl;
                socket.close();
                continue;
            }

            std::cout << "Connected to " << user_ip << ":" << user_port << ". Type 'exit' or '-e' to quit.\n";
            while (true) {
                std::cout << "Enter message: ";
                std::getline(std::cin, message);

                if (message == "exit" || message == "-e") {
                    std::cout << "Exiting chat with " << user_ip << ":" << user_port << std::endl;
                    socket.close();
                    break;
                }

                message += '\n';
                asio::write(socket, asio::buffer(message));
            }
        } catch (std::exception& e) {
            std::cout << "Client Error: " << e.what() << std::endl;
        }
    } else {  // Если команда в формате "/send <ip> <port> <message>"
        if (!(iss >> user_port)) {
            std::cout << "Invalid command format. Usage: /send <ip> <port> <message>" << std::endl;
            continue;
        }

        std::getline(iss, message);

        if (message.empty()) {
            std::cout << "Message cannot be empty." << std::endl;
            continue;
        }

        try {
            tcp::endpoint endpoint(asio::ip::make_address(user_ip), user_port);

            if (!socket.is_open()) {
                socket.connect(endpoint, ec);
            }

            if (ec) {
                std::cout << "Error connecting to " << user_ip << ":" << user_port << ": " << ec.message() << std::endl;
                socket.close();
                continue;
            }

            message += '\n';
            asio::write(socket, asio::buffer(message));

            std::cout << "Message sent to " << user_ip << ":" << user_port << "!" << std::endl;

            socket.close();
        } catch (std::exception& e) {
            std::cout << "Client Error: " << e.what() << std::endl;
        }
    }
} else {
            std::cout << "Invalid command. Use /send <ip> <port> <message>, /info, or /set_port <new port>" << std::endl;
        }
    }
}

int main() {
    setlocale(LC_ALL, "");
    asio::io_context io_context;

    int port = load_port();  // Загружаем порт из конфигурационного файла

    std::thread server_thread([&io_context, &port]() {
        start_server(io_context, port);
    });

    start_client(io_context, port);

    // Перезапуск программы, если был изменен порт
    server_thread.detach();  // Отключаем серверный поток перед завершением
    std::cout << "Restarting with new port..." << std::endl;
    main();  // Рекурсивный вызов main() для перезапуска программы
    return 0;
}