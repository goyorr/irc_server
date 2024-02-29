#include "server.hpp"

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
    int     tmp_client_socket;
    int     non_block;
    int     tmp_socket;
    ssize_t bytes;

    setPort(tmp_port);
    setPassword(tmp_password);

    #ifdef COMMENTED_OUT
        socket() params.
            PARAM1 domain:
                specifies communication domain. We use AF_ LOCAL as defined in the POSIX standard for communication between processes on the same host.
                For communicating between processes on different hosts connected by IPV4, we use AF_INET and AF_I NET 6 for processes connected by IPV6.
            PARAM2 type:
                SOCK_STREAM == TCP. as specified in subject.
            PARAM3 protocol:
                if set to 0 the operating system will choose the default protocol based on the specified address family (AF_INET) and socket type (SOCK_STREAM).
                insted ill expicitly specify it by using (IPPROTO_TCP).
    #endif

    tmp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tmp_socket == -1)
        return std::cerr << "Error: socket" << std::endl, (void)NULL;

    setServer_socket(tmp_socket);

    struct sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons(getPort());

    if (bind(getServer_socket(), (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1)
        return std::cerr << "Error: bind" << std::endl, (void)NULL;

    if (listen(getServer_socket(), SOMAXCONN) == -1)
        return std::cerr << "Error: listen" << std::endl, (void)NULL;

    client_c::_disc.push_back(pollfd());
    client_c::_disc[0].fd = getServer_socket();
    client_c::_disc[0].events = POLLIN;
    client_c::_disc[0].revents = 0;

    while (true) {
        non_block = poll(client_c::_disc.data(), client_c::_disc.size(), 0);
        if (non_block == -1)
            return std::cerr << "Error: poll" << std::endl, (void)NULL;
        if (non_block == 0)
            continue ;
        for (size_t i = 0; i < client_c::_disc.size(); ++i) {
            if (client_c::_disc[i].revents) {
                if (client_c::_disc[i].fd == getServer_socket()) {
                    tmp_client_socket = accept(getServer_socket(), NULL, NULL);
                    if (tmp_client_socket == -1)
                        return std::cerr << "Error: accept" << std::endl, (void)NULL;

                    client_c::_disc.push_back(pollfd());
                    client_c::_disc.back().fd = tmp_client_socket;
                    client_c::_disc.back().events = POLLIN;
                    client_c::_disc.back().revents = 0;

                    std::cout << "New socket #" << tmp_client_socket << std::endl;
                }
                else {
                    try {
                        char    buffer[1024];
                        bytes = recv(client_c::_disc[i].fd, buffer, 1024, 0);
                        if (bytes == -1)
                            return std::cerr << "Error: recv" << std::endl, (void)NULL;
                        buffer[bytes] = '\0';
                        if (bytes >= 1) {
                            std::cout << "data from socket #" << client_c::_disc[i].fd << ": " << buffer;
                            server_c::pars_cmd(buffer, client_c::_disc[i].fd);
                        }
                        else if (bytes == 0) {
                            if (close(client_c::_disc[i].fd) == -1)
                                return std::cerr << "Error: close" << std::endl, (void)NULL;
                            std::cout << "Socket #" << client_c::_disc[i].fd << " closed!" << std::endl;
                            if (clients_map.find(client_c::_disc[i].fd) != clients_map.end())
                                clients_map.erase(client_c::_disc[i].fd);
                            client_c::_disc.erase(client_c::_disc.begin() + i);
                        }
                        std::cout << std::flush;
                    }
                    catch (...) {}
                }
            }
        }
    }
    if (close(getServer_socket()) == -1)
        std::cerr << "Error: close" << std::endl;
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
