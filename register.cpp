#include "headers/includes.h"

void    server_c::reg_user(const std::string &buffer, const uint32_t &client_socket) {
    std::pair<uint32_t, std::string> userpair = regi_parse(buffer, 2);

    if (!userpair.first) {
        if (clients_map.find(client_socket) == clients_map.end()) {
            client_c cln;
            cln.setClient_socket(client_socket);
            clients_map[client_socket] = cln;
        }
        size_t pos = userpair.second.find(32);
        clients_map[client_socket].setClient_user(userpair.second.substr(0, pos));
        clients_map[client_socket].setClient_real_name(userpair.second.substr(pos + 1, userpair.second.size() - pos + 1));
        clients_map[client_socket].setRegUser(true);
    }
    else {
        std::string err;
        
        if (userpair.first == 1)
            err = "ERROR: Malformed USER command. Syntax should be: <username> 0 * <realname>\r\n";
        else if (userpair.first == 3)
            err = "ERROR: Malformed USER command. Syntax should be: <username> 0 * <realname>\r\n";
        else if (userpair.first == 2)
            err = "461 USER :Not enough parameters\r\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
}

void    server_c::reg_nick(const std::string &buffer, const uint32_t &client_socket) {
    std::pair<uint32_t, std::string> nickpair = regi_parse(buffer, 1);

    if (!nickpair.first) {
        for (std::map<uint32_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); ++it) {
            if (nickpair.second == clients_map[it->first].getClient_nick()) {
                std::string err = "433 " + nickpair.second + ":Nickname is already in use\r\n";
                if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
                return ;
            }
        }
        if (clients_map.find(client_socket) == clients_map.end()) {
            client_c cln;
            cln.setClient_socket(client_socket);
            clients_map[client_socket] = cln;
        }
        clients_map[client_socket].setClient_nick(nickpair.second);
        clients_map[client_socket].setRegNick(true);
    }
    else {
        std::string err;
        
        if (nickpair.first == 2)
            err = "461 NICK :Not enough parameters\r\n";
        else if (nickpair.first == 1)
            err = "431 " + nickpair.second + " :Erroneus nickname\r\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
}

void    server_c::reg_pass(const std::string &buffer, const uint32_t &client_socket) {
    std::pair<uint32_t, std::string> pairpass = regi_parse(buffer, 0);

    std::string err;

    if (!pairpass.first) {
        if (pairpass.second == getPassword()) {
            if (clients_map.find(client_socket) == clients_map.end()) {
                client_c cln;
                cln.setClient_socket(client_socket);
                clients_map[client_socket] = cln;
            }
            clients_map[client_socket].setAuth(true);
        }
        else
            err = "464 PASS :Password incorrect\r\n";
    }
    if (pairpass.first == 2)
        err = "461 PASS :Not enough parameters\r\n";
    if (send(client_socket, err.c_str(), err.size(), 0) == -1)
        std::cerr << "Error: send." << std::endl;
}
