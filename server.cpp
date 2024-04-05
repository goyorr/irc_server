#include "./headers/includes.h"

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

    //for IPv4
    struct      sockaddr_in socket_addr;
    int32_t     tmp_client_socket;
    int32_t     non_block;
    int32_t     tmp_socket;
    ssize_t     bytes;

    std::time_t currentTime = std::time(0);
    _time = std::ctime(&currentTime);

    signal(SIGINT, signalHandler);
    setPort(tmp_port);
    setPassword(tmp_password);

    tmp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tmp_socket == -1)
        return std::cerr << "Error: socket." << std::endl, (void)NULL;

    int a = 1;
    if (setsockopt(tmp_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&a, sizeof(a)) == -1)
        return std::cerr << "Error: setsockopt." << std::endl, (void)NULL;

    setServer_socket(tmp_socket);

    socket_addr.sin_family = AF_INET;
    // socket_addr.sin_addr.s_addr = INADDR_ANY;
    socket_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
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
    if (gethostname(hostname, sizeof(hostname)) == -1)
        return std::cerr << "Error: gethostname." << std::endl, (void)NULL;
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
                        bytes = recv(client_c::_disc[i].fd, po, 1024, 0);
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
