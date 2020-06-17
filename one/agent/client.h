namespace one {
class Client
{
public:
    // Todo: ip + port or combined as "ip:port" str?
    Client(const char* ip, int port);
    ~Client();

    // Connect to server.

    // Check status.

    // Send soft stop.

    // Request server info.

    // Send custom command.
    send(Message* message);
private:
    // Arcus connection.
};

Client::Client(const char* addr)
{
}

Client::~Client()
{
}

} // namespace one