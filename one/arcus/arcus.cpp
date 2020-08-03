#include <one/arcus/arcus.h>

namespace one {

Server::Server(/* args */) {}

Server::~Server() {}

int Server::listen(unsigned int port) { return -1; }

int Server::update() { return -1; }

int Server::status() const { return -1; }

Client::Client(/* args */) {}

Client::~Client() {}

int Client::connect(const char* ip, unsigned int port) { return -1; }

}  // namespace one