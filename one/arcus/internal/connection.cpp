#include "connection.h"

#include "socket.h"

namespace one {

Connection::Connection(Socket& socket, size_t max_messages_in, size_t max_messages_out):
    _socket(socket)
{

}

Connection::~Connection()
{

}

} // namespace one