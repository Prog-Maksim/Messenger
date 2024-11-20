#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include <thread>
#include <unordered_map>
#include <asio.hpp>

void start_server(asio::io_context &io_context, int &port);

#endif //SERVER_H
