#ifndef SERVER_HPP
#define SERVER_HPP

#include "client.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class server_c {
    private:
            int                                                 _port;
            std::string                                         _password;
            int                                                 _server_socket;
    public:
            server_c();
            ~server_c();

            std::map<int, client_c>                             _clients_map;
            std::map<std::string, std::vector<std::string> >    _channels_map;

            void        setPort(std::string tmp);
            void        setPassword(std::string tmp);
            void        setServer_socket(int tmp);

            int         getPort() const;
            std::string getPassword() const;
            int         getServer_socket() const;

            void        pars_port(std::string port);
            void        init_server(std::string tmp_port, std::string tmp_password);
            void        pars_cmd(std::string buffer, int client_socket);
            // bool        auth_pass();
};

#endif
