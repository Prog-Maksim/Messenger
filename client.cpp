﻿#include "client.h"
#include <asio.hpp>


void start_client(asio::io_context &io_context, int &port) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    auto contacts = load_contacts();
    asio::ip::tcp::socket socket(io_context);

    asio::error_code ec;
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(asio::ip::host_name(), "");
    std::string server_ip;
    for (auto &endpoint : endpoints) {
        if (endpoint.endpoint().address().is_v4()) {
            server_ip = endpoint.endpoint().address().to_string();
            break;
        }
    }

    while (true) {
        SetConsoleTextAttribute(hConsole, 10);
        std::wcout << L"Введите команду:";
        SetConsoleTextAttribute(hConsole, 15);
        std::string command;
        std::getline(std::cin, command);

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (command == "/info") {
            std::wcout << L"Ваш IP адрес: ";
            std::cout << server_ip << std::endl;
            std::wcout << L"Порт по которому вы принимаете сообщения: ";
            std::cout << port << std::endl;
        }
        else if (command == "/help") {
            std::wcout << L"1) /info - узнать информацию о сервере" << std::endl;
            std::wcout << L"2) /help - выводит информацию о командах" << std::endl;
            std::wcout << L"3) /add_contact - добавляет человека в контакты" << std::endl;
            std::wcout << L"4) /edit_contact - изменяет контакт" << std::endl;
            std::wcout << L"5) /delete_contact - удаляет контакт" << std::endl;
            std::wcout << L"6) /show_contacts - выводит все контакты" << std::endl;
            std::wcout << L"7) /set_port - изменяет порт для прослушивания сообщений" << std::endl;
            std::wcout << L"8) /send - отправляет сообщение пользователю, для работы указывается ip адрес пользователя" << std::endl;
            std::wcout << L"9) /send_name - отправляет сообщение пользователю по имени из записной книжки" << std::endl;
            std::wcout << L"|| Для каждой команды начиная с 3 добавьте приставку '?' чтобы узнать о ней подробнее ||" << std::endl;
        }
        else if (command == "/set_port ?") {
            SetConsoleTextAttribute(hConsole, 6);
            std::wcout << L"Изменяет порт для прослушивания сообщений\n"
                          L"-----------------------------------------\n"
                          L"Пример: /set_port <порт>\n"
                          L"- порт: новое число на котором вы будет принимать сообщения" << std::endl;
            SetConsoleTextAttribute(hConsole, 15);
        }
        else if (command.substr(0, 10) == "/set_port ") {
            int new_port;
            std::istringstream iss(command.substr(10));
            if (!(iss >> new_port) || new_port <= 0 || new_port > 65535) {
                std::cout << "Invalid port number. Please enter a port between 1 and 65535." << std::endl;
                continue;
            }

            port = new_port;
            save_port(port);
            std::cout << "Port updated to " << port << ". Restart the server to apply changes." << std::endl;
            break;
        }

        else if (command == "/add_contact ?") {
            std::wcout << L"Добавляет нового пользователя в записную книжку\n"
                          L"-----------------------------------------------\n"
                          L"Пример: /add_contact <название> <ip адрес> <порт>\n"
                          L"- название: имя пользователя которое будет использоваться для отправки сообщения\n"
                          L"- ip адрес: адресс ПК пользователя, например: 192.168.х.х\n"
                          L"- порт: число, по которому пользователь принимает сообщения" << std::endl;
        }
        else if (cmd == "/add_contact") {
            std::string name, ip;
            int port;
            iss >> name >> ip >> port;

            if (contacts.find(name) != contacts.end()) {
                std::cout << "Contact with this name already exists.\n";
            } else {
                contacts[name] = {ip, port};
                save_contacts(contacts);
                std::cout << "Contact " << name << " added.\n";
            }
        }

        else if (command == "/edit_contact ?") {
            std::wcout << L"Изменяет данные пользователя в записной книжке\n"
                          L"----------------------------------------------\n"
                          L"Пример: /edit_contact <название> <ip адрес> <порт>\n"
                          L"- название: имя пользователя по которому будет произведен поиск в записной книжке\n"
                          L"- ip адрес: новый адресс ПК пользователя, например: 192.168.х.х\n"
                          L"- порт: новое число, по которому пользователь принимает сообщения" << std::endl;
        }
        else if (cmd == "/edit_contact") {
            std::string name, ip;
            int port;
            iss >> name >> ip >> port;

            if (contacts.find(name) == contacts.end()) {
                std::cout << "Contact not found.\n";
            } else {
                contacts[name] = {ip, port};
                save_contacts(contacts);
                std::cout << "Contact " << name << " updated.\n";
            }
        }

        else if (command == "/delete_contact ?") {
            std::wcout << L"Удаляет данные пользователя в записной книжке\n"
                          L"----------------------------------------------\n"
                          L"Пример: /delete_contact <название>\n"
                          L"- название: имя пользователя по которому будет произведен поиск в записной книжке" << std::endl;
        }
        else if (cmd == "/delete_contact") {
            std::string name;
            iss >> name;

            if (contacts.erase(name)) {
                save_contacts(contacts);
                std::cout << "Contact " << name << " deleted.\n";
            } else {
                std::cout << "Contact not found.\n";
            }
        }

        else if (command == "/show_contacts ?") {
            std::wcout << L"Выводит всех пользователей в записной книжке\n"
                          L"--------------------------------------------\n"
                          "Пример: /show_contacts" << std::endl;
        }
        else if (command == "/show_contacts") {
            for (const auto &[name, contact]: contacts) {
                std::cout << "- " << name << ": " << contact.ip << ":" << contact.port << std::endl;
            }
        }

        else if (command == "/send ?") {
            std::wcout << L"Отправляет сообщение пользователю\n"
                          L"---------------------------------\n"
                          L"Пример: /send <ip адрес> <порт>\n"
                          L"- ip адрес: адрес ПК пользователя, например: 192.168.х.х\n"
                          L"- порт: число, по которому пользователь принимает сообщения" << std::endl;
        }
        else if (cmd == "/send") {
            std::string user_ip;
            int user_port;
            std::string message;

            iss >> user_ip >> user_port;
            std::getline(iss, message);

            if (message.empty()) {
                std::cout << "Message cannot be empty." << std::endl;
                continue;
            }

            try {
                asio::ip::tcp::endpoint endpoint(asio::ip::make_address(user_ip), user_port);

                if (!socket.is_open()) {
                    socket.connect(endpoint);
                }

                message += '\n';
                asio::write(socket, asio::buffer(message));

                std::cout << "Message sent to " << user_ip << ":" << user_port << "!" << std::endl;

                socket.close();
            } catch (std::exception &e) {
                std::cout << "Client Error: " << e.what() << std::endl;
                socket.close();
            }
        }

        else if (command == "/send_name ?") {
            std::wcout << L"Отправляет сообщение пользователю из записной книжки\n"
                          L"----------------------------------------------------\n"
                          L"Пример: /send_name <имя пользователя>" << std::endl;
        }
        else if (cmd == "/send_name") {
            std::string name;
            iss >> name;

            if (contacts.find(name) == contacts.end()) {
                std::wcout << L"Данный контакт не найден!" << std::endl;
                continue;
            }

            auto &contact = contacts[name];
            try {
                asio::ip::tcp::endpoint endpoint(asio::ip::make_address(contact.ip), contact.port);

                if (!socket.is_open()) {
                    socket.connect(endpoint);
                }

                std::cout << "Connected to " << name << " (" << contact.ip << ":" << contact.port << "). Type 'exit' or '-e' to quit.\n";

                while (true) {
                    std::wcout << "Введите сообщение:";
                    std::string message;
                    std::getline(std::cin, message);

                    if (message == "exit" || message == "-e") {
                        std::cout << "Exiting chat with " << name << std::endl;
                        socket.close();
                        break;
                    }

                    message += '\n';
                    asio::write(socket, asio::buffer(message));
                }
            } catch (std::exception &e) {
                std::cout << "Client Error: " << e.what() << std::endl;
                socket.close();
            }
        }

        else {
            SetConsoleTextAttribute(hConsole, 11);
            std::wcout << L"Неверная команда. Используйте /help чтобы узнать доступные команды" << std::endl;
            SetConsoleTextAttribute(hConsole, 15);
        }
    }
}