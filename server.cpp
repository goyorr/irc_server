#include "server.hpp"

server_c::server_c() {
}

server_c::~server_c() {
}

void    server_c::setPort(const std::string &tmp) {
    _port = std::atoi(tmp.c_str());
}

void    server_c::setPassword(const std::string &tmp) {
    _password = tmp;
}

void    server_c::setServer_socket(const uint16_t &tmp) {
    _server_socket = tmp;
}

uint16_t server_c::getPort() const {
    return _port;
}

const std::string server_c::getPassword() const {
    return _password;
}

uint16_t server_c::getServer_socket() const {
    return _server_socket;
}

/*
after getting buffer call func(), pass buffer, parse it then call right function (PASS, PRIVMSG, JOIN, etc...)
*/

void    server_c::init_server(const std::string &tmp_port, const std::string &tmp_password) {
    struct      sockaddr_in sockAddr;
    int32_t     tmp_client_socket;
    int32_t     non_block;
    int32_t     tmp_socket;
    ssize_t     bytes;

    setPort(tmp_port);
    setPassword(tmp_password);

    #ifdef SOCKET_PARAM
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
        return std::cerr << "Error: socket." << std::endl, (void)NULL;

    setServer_socket(tmp_socket);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons(getPort());

    if (bind(getServer_socket(), (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1)
        return std::cerr << "Error: bind." << std::endl, (void)NULL;

    if (listen(getServer_socket(), SOMAXCONN) == -1)
        return std::cerr << "Error: listen." << std::endl, (void)NULL;

    client_c::_disc.push_back(pollfd());
    client_c::_disc[0].fd = getServer_socket();
    client_c::_disc[0].events = POLLIN;
    client_c::_disc[0].revents = 0;

    while (true) {
        non_block = poll(client_c::_disc.data(), client_c::_disc.size(), 0);
        if (non_block == -1)
            return std::cerr << "Error: poll." << std::endl, (void)NULL;
        if (non_block == 0)
            continue ;
        for (size_t i = 0; i < client_c::_disc.size(); ++i) {
            if (client_c::_disc[i].revents) {
                if (client_c::_disc[i].fd == getServer_socket()) {
                    tmp_client_socket = accept(getServer_socket(), NULL, NULL);
                    if (tmp_client_socket == -1)
                        return std::cerr << "Error: accept." << std::endl, (void)NULL;

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
                            return std::cerr << "Error: recv." << std::endl, (void)NULL;
                        buffer[bytes] = '\0';
                        if (bytes >= 1) {
                            std::cout << "data from socket #" << client_c::_disc[i].fd << ": " << buffer;
                            //call pars_cmd() and from there call correct command.
                            server_c::pars_cmd(buffer, client_c::_disc[i].fd);
                        }
                        else if (bytes == 0) {
                            if (close(client_c::_disc[i].fd) == -1)
                                return std::cerr << "Error: close." << std::endl, (void)NULL;
                            std::cout << "Socket #" << client_c::_disc[i].fd << " closed!" << std::endl;
                            if (authed_clients_map.find(client_c::_disc[i].fd) != authed_clients_map.end())
                                authed_clients_map.erase(client_c::_disc[i].fd);
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
        std::cerr << "Error: close." << std::endl;
}

void    server_c::pars_port(const std::string &port) {
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

// void    server_c::msg_cmd(const std::string &buffer, const uint16_t &client_socket) {
//     (void)buffer;
//     // std::map<uint16_t, client_c>::iterator it = authed_clients_map.find(client_socket);
//     authed_clients_map.at(client_socket).setClient_nick("lmao");
// }

// void    server_c::pass_cmd(const std::string &buffer, const uint16_t &client_socket) {
//     uint16_t    i = 0;
//     while (buffer[i] == ' ')
//         i++;
//     std::string str = &buffer[i];
//     std::string str3 = str.substr(0, 5);
//     if (str3 != "PASS ")
//         return ;
//     std::string str1 = buffer.substr(i + 5, buffer.size());
//     i = 0;
//     while (str1[i] == ' ')
//         i++;
//     if (!std::isprint(str1[i]))
//         return ;
//     while (str1[i] != ' ')
//         i++;
//     while (str1[i] == ' ')
//         i++;
//     if (std::isprint(str1[i]))
//         return ;

//     i = 0;
//     while (str1[i] == ' ')
//         i++;
//     int i1 = i;
//     while (str1[i1] != ' ')
//         i1++;






#ifdef NOTES

kernel will automatically bind the socket to a suitable port number when you try to connect or send. 





 Sockets in C:

1. **Socket Creation:**
   - The `socket()` function is used to create a socket.
   - Example: `int server_socket = socket(AF_INET, SOCK_STREAM, 0);`
   - This creates a socket for IPv4 communication using TCP.

2. **Socket Address Structure (`sockaddr_in`):**
   - The `struct sockaddr_in` structure is used to define socket addresses.
   - It includes fields like `sin_family`, `sin_addr`, and `sin_port`.

### Binding a Socket:

3. **Binding to a Local Address:**
   - The `bind()` function associates a socket with a local address and port.
   - Example: `bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));`

4. **INADDR_ANY (IPv4) and in6addr_any (IPv6):**
   - Using `INADDR_ANY` in the `sin_addr.s_addr` field means the socket can accept connections on any available local IPv4 address.
   - For IPv6, `in6addr_any` serves a similar purpose.

5. **Flexibility with INADDR_ANY:**
   - Binding to `INADDR_ANY` allows a server to accept connections on all available local IP addresses, whether private or public.
   - This flexibility is useful when a machine is connected to multiple networks.

6. **Specific Local IP Address:**
   - If desired, a specific local IP address can be used instead of `INADDR_ANY` by specifying the IP address in the `sin_addr.s_addr` field.

 Network Configuration:

7. **Public and Private IP Addresses:**
   - `INADDR_ANY` allows a server to handle connections on both private (e.g., 192.168.x.x) and public IP addresses.

8. **Handling Connections Across Networks:**
   - The flexibility provided by `INADDR_ANY` is particularly useful when a machine is connected to multiple networks, enabling the server to handle connections from various sources.

 Notes:

- **Security Considerations:**
  - When exposing a server to the internet, security measures (firewall rules, encryption) should be implemented.
  
- **IPv6 Considerations:**
  - For IPv6, use `in6addr_any` and handle IPv6-specific configurations.

- **Port Configuration:**
  - The port number is set using `sin_port`. In the examples, `htons(8080)` specifies port 8080.

- **Client Binding:**
  - Clients typically do not bind unless using a specific local port.

- **Error Handling:**
  - Check for errors after each system call (`socket()`, `bind()`) and handle them appropriately.

These concepts are foundational for socket programming, allowing for the creation of flexible and configurable networked applications.

#endif
