#include "./headers/includes.h"
#include <netdb.h>

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

void    server_c::setServer_socket(const uint32_t &tmp) {
    _server_socket = tmp;
}

uint32_t server_c::getPort() const {
    return _port;
}

const std::string server_c::getPassword() const {
    return _password;
}

uint32_t server_c::getServer_socket() const {
    return _server_socket;
}

void    server_c::init_server(const std::string &tmp_port, const std::string &tmp_password) {

    struct      sockaddr_in socket_addr;
    int32_t     tmp_client_socket;
    int32_t     non_block;
    int32_t     tmp_socket;
    ssize_t     bytes;

    std::time_t currentTime = std::time(nullptr);
    _time = std::ctime(&currentTime);

    signal(SIGINT, signalHandler);
    setPort(tmp_port);
    setPassword(tmp_password);

    tmp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tmp_socket == -1)
        return std::cerr << "Error: socket." << std::endl, (void)NULL;

    int i1 = 1;
    setsockopt(tmp_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&i1, sizeof(i1));

    setServer_socket(tmp_socket);

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = INADDR_ANY;
    socket_addr.sin_port = htons(getPort());

    if (bind(getServer_socket(), (struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1)
        return std::cerr << "Error: bind." << std::endl, (void)NULL;

    if (listen(getServer_socket(), SOMAXCONN) == -1)
        return std::cerr << "Error: listen." << std::endl, (void)NULL;

    client_c::_disc.push_back(pollfd());
    client_c::_disc[0].fd = getServer_socket();
    client_c::_disc[0].events = POLLIN;
    client_c::_disc[0].revents = 0;

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        std::cerr << "Error: gethostname" << std::endl;
        return ;
    }
    _hostname = hostname;

    while (true) {
        non_block = poll(client_c::_disc.data(), client_c::_disc.size(), 0);
        if (non_block == -1)
            return std::cerr << "Error: poll." << std::endl, (void)NULL;
        if (non_block == 0)
            continue ;
        for (size_t i = 0; i < client_c::_disc.size(); ++i) {
            if (client_c::_disc[i].revents) {
                if (client_c::_disc[i].fd == static_cast<int>(getServer_socket())) {
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
                        char po[1024];
                        bytes = recv(client_c::_disc[i].fd, po, 512, 0);
                        if (bytes == -1)
                            return std::cerr << "Error: recv." << std::endl, (void)NULL;
                        if (bytes >= 1) {
                            po[bytes] = '\0';
                            if (po[bytes - 1] == 10) {
                                std::string betterbuffer = buffers_map[client_c::_disc[i].fd] + po;
                                buffers_map[client_c::_disc[i].fd].clear();
                                int i1 = 0;
                                while (true) {
                                    size_t j = betterbuffer.find('\n', i1); 
                                    if (j == std::string::npos) 
                                        break;
                                    std::string newbuffer = betterbuffer.substr(i1, j - i1 + 1);
                                    i1 = j + 1; 
                                    newbuffer += '\0';
                                    server_c::pars_cmd(newbuffer , client_c::_disc[i].fd);
                                }
                            }
                            else {
                                if (buffers_map[client_c::_disc[i].fd].empty())
                                    buffers_map[client_c::_disc[i].fd] = po;
                                else
                                    buffers_map[client_c::_disc[i].fd] += po;
                            }
                        }
                        else if (bytes == 0) {
                            if (close(client_c::_disc[i].fd) == -1)
                                std::cerr << "Error: close." << std::endl;
                            std::cout << "#" << client_c::_disc[i].fd << " disconnected" << std::endl;

                            std::map<std::string, channels_c>::iterator it = channels_map.begin();
                            while ( it != channels_map.end()) {
                                if (search_user(channels_map, client_c::_disc[i].fd, 'o', it->first)) {
                                    channels_map[it->first]._operators.erase(std::find(channels_map[it->first]._operators.begin(), channels_map[it->first]._operators.end(), client_c::_disc[i].fd));
                                    if (channels_map[it->first]._operators.size() == 0 && channels_map[it->first]._members.size() > 1)
                                        assign_operator(it->first);
                                }
                                if (search_user(channels_map, client_c::_disc[i].fd, 'm', it->first)) {
                                    channels_map[it->first]._members.erase(std::find(channels_map[it->first]._members.begin(), channels_map[it->first]._members.end(), client_c::_disc[i].fd));
                                    channel_checker(it->first);
                                    if (channels_map.begin() == channels_map.end())
                                        break ;
                                    it = channels_map.begin();
                                    it++;
                                    continue;
                                }
                                it++;
                            }
                            clients_map.erase(client_c::_disc[i].fd);
                            buffers_map.erase(client_c::_disc[i].fd);
                            client_c::_disc.erase(client_c::_disc.begin() + i);
                        }
                    }
                    catch (...) {}
                }
            }
        }
    }
    if (close(getServer_socket()) == -1)
        std::cerr << "Error: close." << std::endl;
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
