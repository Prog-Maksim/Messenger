#include "server.h"

void handle_client(asio::ip::tcp::socket socket, const std::unordered_map<std::string, Contact>& contacts) {
    std::string address;

    try {
        asio::ip::tcp::endpoint client_endpoint = socket.remote_endpoint();
        address = client_endpoint.address().to_string();

        std::string contact_name = get_contact_name(contacts, address);
        if (contact_name.empty()) {
            contact_name = address;
        }

        std::array<char, 1024> recv_buffer;

        while (true) {
            size_t len = socket.read_some(asio::buffer(recv_buffer));

            std::cout << contact_name << " -> ";
            std::cout.write(recv_buffer.data(), len);
            std::cout << std::endl;
        }

    } catch (std::exception& e) {
        std::string contact_name = get_contact_name(contacts, address);
        if (contact_name.empty()) {
            contact_name = address;
        }
    }
}

void start_server(asio::io_context &io_context, int &port) {
    std::unordered_map<std::string, Contact> contacts = load_contacts();

    try {
        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

        while (true) {
            asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::thread(handle_client, std::move(socket), std::cref(contacts)).detach();
        }
    } catch (std::exception &e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
    }
}
