#include "server.hpp"
#include <poll.h>

server_c::server_c() {
}

server_c::~server_c() {
}

void    server_c::setPort(std::string tmp) {
    _port = std::atoi(tmp.c_str());
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
    setPort(tmp_port);
    setPassword(tmp_password);

    setServer_socket(socket(AF_INET, SOCK_STREAM, 0));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(getPort());

    bind(getServer_socket(), (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(getServer_socket(), SOMAXCONN);

    client_c::_disc.push_back(pollfd());
    client_c::_disc[0].fd = getServer_socket();
    client_c::_disc[0].events = POLLIN;
    client_c::_disc[0].revents = 0;

    while (true) {
        if (poll(client_c::_disc.data(), client_c::_disc.size(), -1) == -1) {
            std::cerr << "Error: poll" << std::endl;
            exit (1);
        }
        for (size_t i = 0; i < client_c::_disc.size(); ++i) {
            if (client_c::_disc[i].revents) {
                if (client_c::_disc[i].fd == getServer_socket()) {
                    int tmp_socket = accept(getServer_socket(), NULL, NULL);

                    client_c::_disc.push_back(pollfd());
                    client_c::_disc.back().fd = tmp_socket;
                    client_c::_disc.back().events = POLLIN;
                    client_c::_disc.back().revents = 0;

                    std::cout << "New socket #" << tmp_socket << std::endl;
                }
                else {
                    try {
                        char buffer[1024];
                        ssize_t bytes = recv(client_c::_disc[i].fd, buffer, sizeof(buffer), 0);

                        buffer[bytes] = '\0';
                        if (bytes > 1) {
                            std::cout << "data from socket #" << client_c::_disc[i].fd << ": " << buffer << std::endl;
                            server_c::pars_cmd(buffer, client_c::_disc[i].fd);
                        }
                        else if (bytes == 0) {
                            close(client_c::_disc[i].fd);
                            std::cout << "Socket #" << client_c::_disc[i].fd << " closed!" << std::endl;
                            if (clients_map.find(client_c::_disc[i].fd) != clients_map.end())
                                clients_map.erase(client_c::_disc[i].fd);
                            client_c::_disc.erase(client_c::_disc.begin() + i);
                        }
                        else if (bytes == -1) {
                            std::cerr << "Error: recv" << std::endl;
                            exit (1);
                        }
                    }
                    catch (...) {}
                }
            }
        }
    }
    close(getServer_socket());
}

void    server_c::pars_cmd(std::string buffer, int client_socket) {
    (void)buffer;
    if (clients_map.find(client_socket) != clients_map.end()) {
        if (clients_map[client_socket].getRegistered()) {
            //pars then check for all commands normaly.
        }
        else {
            //only check for registration commands NICK, USER.
            //if valid then then just fill the variable with appropriet one.(setClient_nick, setClient_user).
            //if both are set use setRegistered(true).
        }
    }
    else {
      //pars and only check for PASS, if valid add to map with client_socket as
      //key and value as a new client_c object (usee setClient_socket(client_socket)).
    }
}

// bool    server_c::auth_pass() {
//     char        buffer[1024];
//     int         bytes_received;
//     const char  *message = "Welcome to IRC...\n";

//     while ((bytes_received = recv(_clients[_clients.size() - 1].getClient_socket(), buffer, sizeof(buffer), 0)) > 0) {
//         buffer[bytes_received] = '\0';
//         try {
//             std::string str = buffer;
//             std::string pass_cmd = str.substr(0, 5);
//             std::string auth_pass = str.substr(5, str.size() - 6);

//             if (pass_cmd == "PASS " && auth_pass == getPassword()) {
//                 send(_clients[_clients.size() - 1].getClient_socket(), message, strlen(message), 0), (void)NULL;
//                 return 1; 
//             }
//         }
//         catch (...) {
//             return 0;
//         }
//     }
//     return 0;
// }

void    server_c::pars_port(std::string port) {
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
