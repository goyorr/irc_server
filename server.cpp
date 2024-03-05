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
        std::cerr << "Error: close." << std::endl;
}

void    server_c::pars_cmd(const std::string &buffer, const uint16_t &client_socket) {
    if (clients_map.find(client_socket) != clients_map.end()) {
        if (clients_map[client_socket].getRegistered()) {
            //pars then check for all commands normaly.
            //check for all commands normaly.

            //if NICK is used again just check for it then change the nickname (check socket then use setNick_name()).
            //if user/real name used just change it from the default (nickname).

            //ex:if its PRIVMSG.
            priv_msg(buffer, client_socket);

            // join(buffer, client_socket);
            // nick(buffer, client_socket);
            // user(buffer, client_socket);
        }
        //only enter NICK once then use block above if used again (to change).
        else if (!clients_map[client_socket].getRegistered() && clients_map[client_socket].getAuth())
            regi_user(buffer, client_socket);
    }
    //only auth once.
    else
        auth_user(buffer, client_socket);
}

void    server_c::regi_user(const std::string &buffer, const uint16_t &client_socket) {
    std::pair<uint16_t, std::string> nickpair = regi_parse(buffer, 1);

    if (!nickpair.first) {
        clients_map[client_socket].setClient_nick(nickpair.second);
        clients_map[client_socket].setClient_user(nickpair.second);
        clients_map[client_socket].setClient_real_name(nickpair.second);
        clients_map[client_socket].setRegistered(true);
    }
    else {
        if (nickpair.first == 1)
            std::cerr << nickpair.second << " :Unknown comand" << std::endl;
        else if (nickpair.first == 2) // find appropriate Error Msg.
            std::cerr << nickpair.second << " :Not enough parameters" << std::endl;
        else if (nickpair.first == 3)
            std::cerr << nickpair.second << " :Too many parameters" << std::endl;
        return ;
    }
}

void    server_c::auth_user(const std::string &buffer, const uint16_t &client_socket) {
    std::pair<uint16_t, std::string> pairpass = regi_parse(buffer, 0);

    if (!pairpass.first) {
        if (pairpass.second == getPassword()) {
            client_c cln;
            cln.setAuth(true);
            cln.setClient_socket(client_socket);
            clients_map[client_socket] = cln;
            std::cout << "socket #" << client_socket << " authenticated successfully." << std::endl;
            if (send(client_socket, "Welcome to IRC server...\n",  25, 0) == -1) {
                std::cerr << "Error: send." << std::endl;
                exit (1);
            }
        }
    }
    else {
    if (pairpass.first == 1)
        std::cerr << pairpass.second << " :Unknown comand" << std::endl;
    else if (pairpass.first == 2) // find appropriate Error Msg.
        std::cerr << pairpass.second << " :Not enough parameters" << std::endl;
    else if (pairpass.first == 3)
        std::cerr << pairpass.second << " :Too many parameters" << std::endl;
    }
}

void    server_c::priv_msg(const std::string &buffer, const uint16_t &client_socket) {
    //pars PRIVMSG 1st pair:target 2nd:message.
    //remove after.
    (void)buffer;
    (void)client_socket;
    #ifdef NOT_MADE_YET

    std::pair<std::string, std::string> msgpair = prvmsg_parse(buffer, 3);
    //if PRIVMSG is for a channel.
    if (msgpair.first[0] == '#') {
        std::map<std::string, std::vector<uint16_t> > ::iterator it;

        for (it = channels_map.begin(); it != channels_map.end(); ++it) {
        //check if channels exists.
            if (it->first == msgpair.first) {
                for (int i = 0; channels_map[msgpair.first].size(); i++)
                    send(channels_map[msgpair.first][0], msgpair.second.c_str(), msgpair.second.size(), 0);
                return ;
            }
        }
        //return some error message (no user found).
        return ;
    }
    //if its for a user.
    if (msgpair.first != "0") {
        std::map<uint16_t, client_c>::iterator it;

        for (it = clients_map.begin(); it != clients_map.end(); ++it) {
            if (clients_map[it->first].getClient_nick() == msgpair.first) {
                send(clients_map[it->first].getClient_socket(), msgpair.second.c_str(), msgpair.second.size(), 0);
                return ;
            }
        }
        //return some error message (no user found).
        return ;
    }
    //error.
    else {
        if (msgpair.first == "1")
            std::cerr << msgpair.second << " :Unknown comand" << std::endl;
        else if (msgpair.first == "2") // find appropriate Error Msg.
            std::cerr << msgpair.second << " :Not enough parameters" << std::endl;
        else if (msgpair.first == "3")
            std::cerr << msgpair.second << " :Too many parameters" << std::endl;
        return ;
    }

    #endif
}


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
