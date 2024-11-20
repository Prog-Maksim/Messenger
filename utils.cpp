#include "utils.h"

int load_port() {
    std::ifstream file("config.txt");
    int port = 8080;

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

std::unordered_map<std::string, Contact> load_contacts() {
    std::unordered_map<std::string, Contact> contacts;
    std::ifstream file("contacts.txt");

    std::string name, ip;
    int port;
    while (file >> name >> ip >> port) {
        contacts[name] = {ip, port};
    }
    return contacts;
}

void save_contacts(const std::unordered_map<std::string, Contact> &contacts) {
    std::ofstream file("contacts.txt");
    for (const auto &[name, contact] : contacts) {
        file << name << " " << contact.ip << " " << contact.port << "\n";
    }
}

std::string get_contact_name(const std::unordered_map<std::string, Contact>& contacts, const std::string& ip) {
    for (const auto& [name, contact] : contacts) {
        if (contact.ip == ip) {
            return name;
        }
    }
    return "";
}
