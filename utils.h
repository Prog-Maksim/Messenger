#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

struct Contact {
    std::string ip;
    int port;
};

int load_port();
void save_port(int port);
std::unordered_map<std::string, Contact> load_contacts();
void save_contacts(const std::unordered_map<std::string, Contact> &contacts);
std::string get_contact_name(const std::unordered_map<std::string, Contact>& contacts, const std::string& ip);

#endif // UTILS_H
