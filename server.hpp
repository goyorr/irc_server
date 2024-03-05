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


};

std::pair<int, std::string> regi_parse(std::string str, int flag);

#endif

// NICKNAME & USER
//only counts as registered if atlest nock is set.
// else if (clients_map[client_socket].getClient_nick() != "_Default_") {
//     std::pair <int , std::string> userpair = regi_parse(buffer, 2);
//     if (!userpair.first) {
//         clients_map[client_socket].setClient_user(userpair.second);
//         clients_map[client_socket].setRegistered(true);
//     }
//     else {
//         if (userpair.first == 1)
//             std::cerr << userpair.second << " :Unknown comand" << std::endl;
//         else if (userpair.first == 2) // find appropriate Error Msg.
//             std::cerr << userpair.second << " :Not enough parameters" << std::endl;
//         else if (userpair.first == 3)
//             std::cerr << userpair.second << " :Too many parameters" << std::endl;
//         return ;
//     }
