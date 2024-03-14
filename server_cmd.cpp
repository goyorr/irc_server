#include "headers/includes.h"

void    server_c::pars_cmd(const std::string &buffer, const uint16_t &client_socket) {
    if (clients_map.find(client_socket) != clients_map.end() && clients_map[client_socket].getRegistered()) {
        std::string cmd = select_cmd(buffer);

        if (cmd == "PRIVMSG")
            priv_msg(buffer, client_socket);
        else if (cmd == "PASS" || cmd == "USER") {
            std::string message = "462 " + clients_map[client_socket].getClient_nick() + " :You may not reregister\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
        else if (cmd == "NICK") {
            std::string message;
            std::pair<uint16_t, std::string> nickpair = regi_parse(buffer, 1);

            if (!nickpair.first) {
                for (std::map<uint16_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); ++it) {
                    if (nickpair.second == clients_map[it->first].getClient_nick()) {
                        std::string err = ":" + nickpair.second + ":Nickname is already in use\n";
                    if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                    return ;
                    }
                }
                message = ":" + clients_map[client_socket].getClient_nick() + " NICK " + nickpair.second + "\n";
                clients_map[client_socket].setClient_nick(nickpair.second);
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else if (nickpair.first == 2) {
                message = "461 " + clients_map[client_socket].getClient_nick() + " NICK :Not enough parameters\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
        }
        else if (cmd == "JOIN") {
            join(buffer, client_socket);
        }
        // else if (cmd == "KICK") {
        //     kick(buffer, client_socket);
        // }
        else if (cmd == "QUIT") {
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " QUIT :lol\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
            std::cout << "#" << client_socket << " disconnected" << std::endl;
            clients_map.erase(client_socket);
            for (size_t i = 0; i < client_c::_disc.size(); i++) {
                if (client_c::_disc[i].fd == client_socket)
                    client_c::_disc.erase(client_c::_disc.begin() + i);
            }
            if (close(client_socket) == -1)
                std::cerr << "Error: close.";
        }
        else {
            std::string message = "421 " + clients_map[client_socket].getClient_nick() + " " + cmd + " :Unknown command\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
    else {
        std::string cmd = select_cmd(buffer);

        if (cmd == "USER")
            reg_user(buffer, client_socket);
        else if (cmd == "NICK")
            reg_nick(buffer, client_socket);
        else if (cmd == "PASS") {
            if (!clients_map[client_socket].getAuth())
                reg_pass(buffer, client_socket);
        }
        if (clients_map[client_socket].getAuth() && clients_map[client_socket].getRegNick() && clients_map[client_socket].getRegUser()) {
            std::cout << clients_map[client_socket].getClient_nick()  << " authenticated successfully." << std::endl;
            std::string message = "001 " + clients_map[client_socket].getClient_nick() + " :Welcome to the ft_irc Network\n";
            if (send(client_socket, message.c_str(),  message.size(), 0) == -1) {
                std::cerr << "Error: send." << std::endl;
                exit (1);
            }
            clients_map[client_socket].setRegistered(true);
        }
    }
}

void    server_c::priv_msg(const std::string &buffer, const uint16_t &client_socket) {
    std::pair<std::vector<std::string>, std::string> msgpair = prvmsg_parse(buffer);

    if (!msgpair.first.empty()) {
        for (size_t i = 0; i < msgpair.first.size(); i++) {
            std::vector<uint16_t> pool;
            if (msgpair.first[i][0] == '#') {
                if (channels_map.find(msgpair.first[i]) != channels_map.end()) {
                    pool.insert(pool.end(), channels_map[msgpair.first[i]]._members.begin(), channels_map[msgpair.first[i]]._members.end());

                    std::set<uint16_t> betterPool(pool.begin(), pool.end());
                    for (std::set<uint16_t>::iterator it = betterPool.begin(); it != betterPool.end(); ++it) {
                        if (*it == client_socket)
                            continue;
                        std::string message = ":" + clients_map[client_socket].getClient_nick() + " PRIVMSG " + msgpair.first[i] + " :" + msgpair.second + "\r\n";
                        if (send(*it, message.c_str(), message.size(), 0) == -1)
                            std::cerr << "Error: send." << std::endl;
                    }
                }
                else {
                    std::string message = "403 " + clients_map[client_socket].getClient_nick() + " " + msgpair.first[i] + " :No such channel\r\n";
                    if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                }
                msgpair.first.erase(msgpair.first.begin() + i);
                --i;
            }
        }

        std::vector<uint16_t> pool;
        for (size_t i = 0; i < msgpair.first.size(); i++) {
            bool a = false;
            for (std::map<uint16_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); ++it) {
                if (clients_map[it->first].getClient_nick() == msgpair.first[i]) {
                    pool.push_back(it->first);
                    a = true;
                    break ;
                }
            }
            if (!a) {
                std::string message = "401 " + clients_map[client_socket].getClient_nick() + " " + msgpair.first[i] + " :No such nick\r\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
        }
        std::set<uint16_t> betterPool(pool.begin(), pool.end());
        for (std::set<uint16_t>::iterator it = betterPool.begin(); it != betterPool.end(); ++it) {
            if (*it == client_socket)
                continue;
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " PRIVMSG " + clients_map[*it].getClient_nick() +" :" + msgpair.second + "\r\n";
            if (send(*it, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
}

void    server_c::join(const std::string &buffer, const uint16_t &client_socket) {
    std::vector<std::pair<std::string, std::string> >   join_pair = join_kick(buffer, 0);
    std::string                                         message;

    for (size_t i = 0; i < join_pair.size(); i++) {
        if (join_pair[i].first[join_pair[i].first.size() - 1] == 13)
            join_pair[i].first = join_pair[i].first.substr(0, join_pair[i].first.size() - 1);
        if (channels_map.find(join_pair[i].first) == channels_map.end()) {
            channels_c newChnl;

            channels_map[join_pair[i].first] = newChnl;
            channels_map[join_pair[i].first].setName(join_pair[i].first);
            channels_map[join_pair[i].first]._members.push_back(client_socket);
            channels_map[join_pair[i].first]._operators.push_back(client_socket);
            message = ":" + clients_map[client_socket].getClient_nick() + " JOIN " + join_pair[i].first + "\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
        else {
            if (channels_map[join_pair[i].first].getProtected()) {
                if (join_pair[i].second != channels_map[join_pair[i].first].getChannelPassword())
                    message = "475 " + clients_map[client_socket].getClient_nick() + " " + join_pair[i].first + ":Cannot join channel\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else {
                channels_map[join_pair[i].first]._members.push_back(client_socket);
                message = ":" + clients_map[client_socket].getClient_nick() + " JOIN " + join_pair[i].first + "\n";
                for (size_t j = 0; j < channels_map[join_pair[i].first]._members.size(); j++) {
                    if (send(channels_map[join_pair[i].first]._members[j], message.c_str(), message.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                }
                std::string users;
                for (size_t j = 0; j < channels_map[join_pair[i].first]._members.size(); j++) {
                    if (std::find(channels_map[join_pair[i].first]._operators.begin(), channels_map[join_pair[i].first]._operators.end(),
                        channels_map[join_pair[i].first]._members[j]) != channels_map[join_pair[i].first]._operators.end())
                        users += " @" + clients_map[channels_map[join_pair[i].first]._members[j]].getClient_nick();
                    else
                        users += " " + clients_map[channels_map[join_pair[i].first]._members[j]].getClient_nick();
                }
                message = "353 " + clients_map[client_socket].getClient_nick() + " = " + join_pair[i].first + " :" + users + "\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
                message = "366 " + clients_map[client_socket].getClient_nick() + " " + join_pair[i].first + " :End of /NAMES list\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;

                return ;
            }
        }
    }
}
