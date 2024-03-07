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

                std::map<uint16_t, client_c>                        clients_map;
                std::map<std::string, std::vector<uint16_t> >    channels_map;

                void                setPort(const std::string &tmp);
                void                setPassword(const std::string &tmp);
                void                setServer_socket(const uint16_t &tmp);

                uint16_t            getPort() const;
                uint16_t            getServer_socket() const;
                const std::string   getPassword() const;

                void                pars_port(const std::string &port);

                void                init_server(const std::string &tmp_port, const std::string &tmp_password);

                void                pars_cmd(const std::string &buffer, const uint16_t &client_socket);

                void                auth_user(const std::string &buffer, const uint16_t &client_socket);
                void                regi_user(const std::string &buffer, const uint16_t &client_socket);

                void                priv_msg(const std::string &buffer, const uint16_t &client_socket);
                void                user_real(const std::string &buffer, const uint16_t &client_socket);
};

std::pair<int, std::string>                         regi_parse(std::string str, int flag);
std::pair<std::vector<std::string>, std::string >   prvmsg_parse(std::string buffer);

int                                                 is_ws(int c);
int                                                 is_end(std::string str, int *i);
std::string                                         select_cmd(std::string buffer);
#endif
