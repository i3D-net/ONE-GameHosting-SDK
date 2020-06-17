namespace one {

class Message;

class Client
{
public:
    // Todo: ip + port or combined as "ip:port" str?
    Client(const char* ip, int port);
    ~Client();

    // Connect to server.

    // Check status.
    int status();

    // Send soft stop.

    // Request server info.

    // Send custom command.
    void send(Message* message, int* error);
private:
    // Arcus connection.
};

Client::Client(const char* addr, int port)
{
}

Client::~Client()
{
}

int Client::status() {
    return 0;
}

} // namespace one