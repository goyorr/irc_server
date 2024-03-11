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
        else if (cmd == "KICK") {
            kick(buffer, client_socket);
        }
        else if (cmd == "QUIT") {
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " QUIT :lol\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
            std::cout << client_socket << " Disconnected" << std::endl;
            clients_map.erase(client_socket);
            for (size_t i = 0; i < client_c::_disc.size(); i++) {
                if (client_c::_disc[i].fd == client_socket)
                    client_c::_disc.erase(client_c::_disc.begin() + i);
            }
            close(client_socket);
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
            std::cout << "socket #" << client_socket << " authenticated successfully." << std::endl;
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
        std::vector<uint16_t> pool;

        for (size_t i = 0; i < msgpair.first.size(); i++) {
            //change PRIVMSG for channels after making JOIN! (maybe not?)
            if (msgpair.first[i][0] == '#') {
                //if channel exists.
                if (channels_map.find(msgpair.first[i]) != channels_map.end()) {
                    pool.insert(pool.end(), channels_map[msgpair.first[i]]._members.begin(), channels_map[msgpair.first[i]]._members.end());
                    msgpair.first.erase(msgpair.first.begin() + i);
                    --i;
                }
                else {
                    std::string message = "403 " + clients_map[client_socket].getClient_nick() + " " + msgpair.first[i] + " :No such channel\n";
                    if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                }
            }
            else {
                bool a = false;
                for (std::map<uint16_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); ++it) {
                    if (clients_map[it->first].getClient_nick() == msgpair.first[i]) {
                        pool.push_back(it->first);
                        a = true;
                        break ;
                    }
                }
                if (!a) {
                    std::string message = "401 " + clients_map[client_socket].getClient_nick() + " " + msgpair.first[i] + " :No such nick\n";
                    if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                }
            }
        }
        std::set<uint16_t> betterPool(pool.begin(), pool.end());

        for (std::set<uint16_t>::iterator it = betterPool.begin(); it != betterPool.end(); ++it) {
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " PRIVMSG " + clients_map[*it].getClient_nick() +" :" + msgpair.second + "\n";
            if (send(*it, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
}

void    server_c::join(const std::string &buffer, const uint16_t &client_socket) {
    //                        name        password
        std::cout << "------??????????..." << std::endl;
    std::vector<std::pair<std::string, std::string> >   join_pair = join_kick(buffer, 0);
        std::cout << "------232323..." << join_pair.size()  << std::endl;
    std::string                                         message;

        std::cout << "------first ..." << join_pair[0].first << std::endl;
        std::cout << "------seconf ..." << join_pair[0].second << std::endl;
    for (size_t i = 0; i < join_pair.size(); i++) {
        if (channels_map.find(join_pair[i].first) == channels_map.end()) {
            channels_c newChnl;

            std::cout << "------created ..." << join_pair[i].first << std::endl;
            channels_map[join_pair[i].first] = newChnl;
            channels_map[join_pair[i].first].setName(join_pair[i].first);
            channels_map[join_pair[i].first]._members.push_back(client_socket);
            channels_map[join_pair[i].first]._operators.push_back(client_socket);
            message = ":" + clients_map[client_socket].getClient_nick() + " JOIN " + join_pair[i].first + "\n";
        }
        else {
            std::cout << "---------joined ..." << join_pair[i].first << std::endl;
            if (channels_map[join_pair[i].first].getProtected()) {
                if (join_pair[i].second != channels_map[join_pair[i].first].getChannelPassword())
                    message = "475 " + clients_map[client_socket].getClient_nick() + " " + join_pair[i].first + ":Cannot join channel\n";
            }
            else
                channels_map[join_pair[i].first]._members.push_back(client_socket);
        }
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
}


void    kick(const std::string &buffer, const uint16_t &client_socket){

    server_c server;

    std::vector<std::pair<std::string, std::string> >   kick_pairs = join_kick(buffer, 1);
        std::string                                         message;
        
        for (size_t i = 0; i < kick_pairs.size(); i++)
        {


                for (std::map<std::uint16_t, client_c>::iterator itt = server.clients_map.begin(); itt != server.clients_map.end(); itt ++)
                {
                    if (itt->second.getClient_nick() == kick_pairs[i].second)
                    {
                        if ()
                    }

                }
            for (std::map<std::string, channels_c>::iterator it = server.channels_map.begin(); it != server.channels_map.end(); it ++)
            {
                if (it->first == kick_pairs[i].first)
                {
                
                }
            }
            
        }
}