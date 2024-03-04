#if !SERVER_HPP
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
            uint16_t    _port;
            std::string _password;
            uint16_t    _server_socket;
    public:
            server_c();
            ~server_c();

            std::map<std::string, client_c>                     registered_clients_map;
            std::map<uint16_t, client_c>                        authed_clients_map;
            std::map<std::string, std::vector<std::string> >    channels_map;

            void                setPort(const std::string &tmp);
            void                setPassword(const std::string &tmp);
            void                setServer_socket(const uint16_t &tmp);

            uint16_t            getPort() const;
            uint16_t            getServer_socket() const;
            const std::string   getPassword() const;

            void                pars_port(const std::string &port);
            void                init_server(const std::string &tmp_port, const std::string &tmp_password);

            void                pass_cmd(const std::string &buffer, const uint16_t &client_socket);
            void                msg_cmd(const std::string &buffer, const uint16_t &client_socket);

};

#endif
