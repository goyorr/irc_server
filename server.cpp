#include "server.hpp"

server_c::server_c() {
}

server_c::~server_c() {
}

void    server_c::setPort(std::string tmp) {
    _port = std::stoi(tmp);
}

void    server_c::setPassword(std::string tmp) {
    _password = tmp;
}

void    server_c::setServer_socket(int tmp) {
    _server_socket = tmp;
}

int server_c::getPort() const {
    return _port;
}

std::string server_c::getPassword() const {
    return _password;
}

int server_c::getServer_socket() const {
    return _server_socket;
}

void    server_c::init_server(std::string tmp_port, std::string tmp_password) {
    client_c cln;

    setPort(tmp_port);
    setPassword(tmp_password);

    setServer_socket(socket(AF_INET, SOCK_STREAM, 0));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(getPort());
    bind(getServer_socket(), (struct sockaddr*)&server_addr, sizeof(server_addr));

    listen(getServer_socket(), SOMAXCONN);
    cln.setClient_socket(accept(getServer_socket(), NULL, NULL));

    _clients.push_back(cln);
}

void    server_c::auth_pass() {
    char        buffer[1024];
    int         bytes_received;
    const char  *message = "Welcome to IRC...\n";

    while ((bytes_received = recv(_clients[_clients.size() - 1].getClient_socket(), buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        try {
            std::string str = buffer;
            std::string pass_cmd = str.substr(0, 5);
            std::string auth_pass = str.substr(5, str.size() - 6);

            if (pass_cmd == "PASS " && auth_pass == getPassword()) {
                std::cout << "New user #" << _clients[_clients.size() - 1].getClient_socket() << std::endl;
                return send(_clients[_clients.size() - 1].getClient_socket(), message, strlen(message), 0), (void)NULL;
            }
        }
        catch (...) {}
    }
}

void    server_c::pars(std::string port) {
    if (port.size() > 5) {
        std::cerr << "Error: invalid port!" << std::endl;
        exit (1);
    }
    for (size_t i = 0; i < port.size(); i++) {
        if (!std::isdigit(port[i])) {
            std::cerr << "Error: invalid port!" << std::endl;
            exit (1);
        }
    }
} 
