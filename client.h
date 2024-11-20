#ifndef CLIENT_H
#define CLIENT_H

#include "utils.h"
#include <asio.hpp>

void start_client(asio::io_context &io_context, int &port);

#endif // CLIENT_H
