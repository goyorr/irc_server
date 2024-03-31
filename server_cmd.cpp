#include "headers/includes.h"

void    server_c::pars_cmd(const std::string &buffer, const uint32_t &client_socket) {
    if (clients_map.find(client_socket) != clients_map.end() && clients_map[client_socket].getRegistered()) {
        std::string cmd = select_cmd(buffer);


        if (cmd == "PRIVMSG")
            priv_msg(buffer, client_socket);
        else if (cmd == "PASS" || cmd == "USER") {
            std::string message = "462 " + clients_map[client_socket].getClient_nick() + " :You may not reregister\r\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
        else if (cmd == "NICK") {
            std::string message;
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
                message = ":" + clients_map[client_socket].getClient_nick() + " NICK " + nickpair.second + "\r\n";
                clients_map[client_socket].setClient_nick(nickpair.second);
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else if (nickpair.first == 1){
                message = "432 " + clients_map[client_socket].getClient_nick() + " " + nickpair.second + " :Erroneus nickname\r\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;

            }
            else if (nickpair.first == 2) {
                message = "461 " + clients_map[client_socket].getClient_nick() + " NICK :Not enough parameters\r\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
        }
        else if (cmd == "JOIN")
            join(buffer, client_socket);
        else if (cmd == "KICK")
            kick_cmd(buffer, client_socket);
        else if (cmd == "INVITE")
            invite_cmd(buffer, client_socket);
        else if (cmd == "TOPIC")
            topic_cmd(buffer, client_socket);
        else if (cmd == "MODE")
            mode_cmd(buffer, client_socket);
        else if (cmd == "BOT")
            bot_cmd(buffer, client_socket);
        else if (cmd == "QUIT") {
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " QUIT :lol\r\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
            std::cout << "#" << client_socket << " disconnected" << std::endl;

            std::map<std::string, channels_c>::iterator it = channels_map.begin();
            while (it != channels_map.end()) {
                if (search_user(channels_map, client_socket, 'o', it->first)) {
                    channels_map[it->first]._operators.erase(std::find(channels_map[it->first]._operators.begin(), channels_map[it->first]._operators.end(), client_socket));
                    if (channels_map[it->first]._operators.size() == 0 && channels_map[it->first]._members.size() > 1)
                        assign_operator(it->first);
                }
                if (search_user(channels_map, client_socket, 'm', it->first)) {
                    channels_map[it->first]._members.erase(std::find(channels_map[it->first]._members.begin(), channels_map[it->first]._members.end(), client_socket));
                    channel_checker(it->first);
                    if (channels_map.begin() == channels_map.end())
                        break ;
                    it = channels_map.begin();
                    it++;
                    continue;
                }
                it++;
            }

            for (size_t i = 0; i < client_c::_disc.size(); i++) {
                if (client_c::_disc[i].fd == static_cast<int>(client_socket)) {
                    buffers_map.erase(client_c::_disc[i].fd);
                    client_c::_disc.erase(client_c::_disc.begin() + i);
                }
            }
            clients_map.erase(client_socket);
            if (close(client_socket) == -1)
                std::cerr << "Error: close.";
        }
        else {
            std::string message = "421 " + clients_map[client_socket].getClient_nick() + " " + cmd + " :Unknown command\r\n";
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
            clients_map[client_socket].setRegistered(true);

            struct sockaddr_in cl_socket_addr;
            socklen_t addr_len = sizeof(cl_socket_addr);
            getsockname(client_socket, (struct sockaddr *)&cl_socket_addr, &addr_len);
            clients_map[client_socket].ipaddr = inet_ntoa(cl_socket_addr.sin_addr);


            // :adrift.sg.quakenet.org 001 fg :Welcome to the QuakeNet IRC Network, fg
            std::string message = ":" + _hostname + " 001 " + clients_map[client_socket].getClient_nick() + " :Welcome to the ft_irc Network, " + clients_map[client_socket].getClient_nick() + "!" + clients_map[client_socket].getClient_user() + "@" + clients_map[client_socket].ipaddr + "\r\n";
            if (send(client_socket, message.c_str(),  message.size(), 0) == -1) {
                std::cerr << "Error: send." << std::endl;
                exit (1);
            }
            // :adrift.sg.quakenet.org 002 fg :Your host is adrift.sg.quakenet.org, running version u2.10.12.10+snircd(1.3.4a)
            message = ":" + _hostname + " 002 " + clients_map[client_socket].getClient_nick() + " :Your host is " + _hostname + ", running version V80.34f.2.09-0D\r\n";
            if (send(client_socket, message.c_str(),  message.size(), 0) == -1) {
                std::cerr << "Error: send." << std::endl;
                exit (1);
            }
            // :adrift.sg.quakenet.org 003 fg :This server was created Mon 3 Aug 2020 at 13:56:48 BST
            message = ":" + _hostname + " 003 " + clients_map[client_socket].getClient_nick() + " :This server was created " + _time + "\r\n";
            if (send(client_socket, message.c_str(),  message.size(), 0) == -1) {
                std::cerr << "Error: send." << std::endl;
                exit (1);
            }
            // :adrift.sg.quakenet.org 004 fg adrift.sg.quakenet.org u2.10.12.10+snircd(1.3.4a) dioswkgxRXInP biklmnopstvrDcCNuMT bklov
            message = ":" + _hostname + " 004 " + clients_map[client_socket].getClient_nick() + " " + _hostname + " V80.34f.2.09-0D itkol\r\n";
            if (send(client_socket, message.c_str(),  message.size(), 0) == -1) {
                std::cerr << "Error: send." << std::endl;
                exit (1);
            }
        }
    }
}

void    server_c::priv_msg(const std::string &buffer, const uint32_t &client_socket) {
    std::pair<std::vector<std::string>, std::string> msgpair = prvmsg_parse(buffer);

    if (msgpair.second[0] == ':')
        msgpair.second.erase(msgpair.second.begin());
    if (!msgpair.first.empty()) {
        for (size_t i = 0; i < msgpair.first.size(); i++) {
            std::vector<uint32_t> pool;
            if (msgpair.first[i][0] == '#') {
                if (channels_map.find(msgpair.first[i]) != channels_map.end()) {
                    if (search_user(channels_map, client_socket, 'm', msgpair.first[i])) {
                        pool.insert(pool.end(), channels_map[msgpair.first[i]]._members.begin(), channels_map[msgpair.first[i]]._members.end());

                        std::set<uint32_t> betterPool(pool.begin(), pool.end());
                        for (std::set<uint32_t>::iterator it = betterPool.begin(); it != betterPool.end(); ++it) {
                            if (*it == client_socket)
                                continue;
                            std::string message = ":" + clients_map[client_socket].getClient_nick() + "!~" + clients_map[client_socket].getClient_user() + "@" + clients_map[client_socket].ipaddr + " PRIVMSG " + msgpair.first[i] + " :" + msgpair.second + "\r\n";
                            if (send(*it, message.c_str(), message.size(), 0) == -1)
                                std::cerr << "Error: send." << std::endl;
                        }
                    }
                    else {
                        std::string message = "404 " + clients_map[client_socket].getClient_nick() + " " + msgpair.first[i] + " :Cannot send to channel\r\n";
                        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
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

        std::vector<uint32_t> pool;
        for (size_t i = 0; i < msgpair.first.size(); i++) {
            bool a = false;
            for (std::map<uint32_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); ++it) {
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
        std::set<uint32_t> betterPool(pool.begin(), pool.end());
        for (std::set<uint32_t>::iterator it = betterPool.begin(); it != betterPool.end(); ++it) {
            if (*it == client_socket)
                continue;
            std::string message = ":" + clients_map[client_socket].getClient_nick() + "!~" + clients_map[client_socket].getClient_user() + "@" + clients_map[client_socket].ipaddr + " PRIVMSG " + clients_map[*it].getClient_nick() + " :" + msgpair.second + "\r\n";
            if (send(*it, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
}

void    server_c::join(const std::string &buffer, const uint32_t &client_socket) {
    std::pair<std::vector<std::string>, std::vector<std::string> >   join_pair = join_pars(buffer);
    std::string                                         message;
    size_t k = 0;

    for (size_t i = 0; i < join_pair.first.size(); i++) {
        if (k >= join_pair.second.size())
            k = join_pair.second.size();
        if (join_pair.first[i][0] != '#') {
            std::string err = "403 " + clients_map[client_socket].getClient_nick() + " " + join_pair.first[i] + " :No such channel\r\n";
            if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
            continue ;
        }
        if (join_pair.first[i][join_pair.first[i].size() - 1] == 13)
            join_pair.first[i] = join_pair.first[i].substr(0, join_pair.first[i].size() - 1);
        if (channels_map.find(join_pair.first[i]) == channels_map.end()) {
            channels_c newChnl;

            channels_map[join_pair.first[i]] = newChnl;
            channels_map[join_pair.first[i]].setName(join_pair.first[i]);
            channels_map[join_pair.first[i]]._members.push_back(client_socket);
            channels_map[join_pair.first[i]]._operators.push_back(client_socket);
            //:fg!~g@197.230.30.146 JOIN #i
            message = ":" + clients_map[client_socket].getClient_nick() + "!~@" + clients_map[client_socket].ipaddr + " JOIN " + join_pair.first[i] + "\r\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
            message = "353 " + clients_map[client_socket].getClient_nick() + " = " + join_pair.first[i] + " :@" +clients_map[client_socket].getClient_nick() + "\r\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
            message = "366 " + clients_map[client_socket].getClient_nick() + " " + join_pair.first[i] + " :End of /NAMES list\r\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
        }
        else {
            if (search_user(channels_map, client_socket, 'm', join_pair.first[i]))
                ;
            else if (channels_map[join_pair.first[i]].getisinvite_only() && !search_user(channels_map, client_socket, 'i', join_pair.first[i])) {
                message = "473 " + clients_map[client_socket].getClient_nick() + " " + join_pair.first[i] + " :Cannot join channel (+i)\r\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else if (channels_map[join_pair.first[i]].getisuser_limit() && channels_map[join_pair.first[i]].getuser_limit() == channels_map[join_pair.first[i]].getuser_count()) {
                message = "471 " + clients_map[client_socket].getClient_nick() + " " + join_pair.first[i] + " :Cannot join channel (+l)\r\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else if (channels_map[join_pair.first[i]].getisProtected() && channels_map[join_pair.first[i]].getChannelPassword() != join_pair.second[k]) {
                k++;
                message = "475 " + clients_map[client_socket].getClient_nick() + " " + join_pair.first[i] + " :Cannot join channel (+k)\r\n";
                if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else
                join_channel(join_pair.first[i], client_socket);
        }
    }
}

void    server_c::join_channel(const std::string &channel_name, const uint32_t &client_socket) {
    std::string message;

    channels_map[channel_name]._members.push_back(client_socket);
    channels_map[channel_name].incrementUser_count();
    message = ":" + clients_map[client_socket].getClient_nick() + "!~@" + clients_map[client_socket].ipaddr + " JOIN " + channel_name + "\r\n";
    for (size_t j = 0; j < channels_map[channel_name]._members.size(); j++) {
        if (send(channels_map[channel_name]._members[j], message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
    if (channels_map[channel_name].getTopic() == "")
        message = "331 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :No topic is set\r\n";
    else
        message = "332 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :" + channels_map[channel_name].getTopic() + "\r\n";
    if (send(client_socket, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Error: send." << std::endl;
    std::string users;
    for (size_t j = 0; j < channels_map[channel_name]._members.size(); j++) {
        if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(),
            channels_map[channel_name]._members[j]) != channels_map[channel_name]._operators.end())
            users += "@" + clients_map[channels_map[channel_name]._members[j]].getClient_nick() + " ";
        else
            users += clients_map[channels_map[channel_name]._members[j]].getClient_nick() + " ";
    }
    message = "353 " + clients_map[client_socket].getClient_nick() + " = " + channel_name + " :" + users + "\r\n";
    if (send(client_socket, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Error: send." << std::endl;
    message = "366 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :End of /NAMES list\r\n";
    if (send(client_socket, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Error: send." << std::endl;
}

std::string bot_pars(std::string buffer)
{
    int pos = buffer.find('T');
    pos++;
    while (is_ws(buffer[pos]))
        pos++;
    puts ("k2");
    std::cout << (int)buffer[pos] << std::endl;
    if (!isprint(buffer[pos]))
        return "";
    puts ("k3");
    int i = pos;
    while (!is_ws(buffer[i]) && !is_end(buffer, &i))
        i++;
    std::string channel = buffer.substr(pos, i - pos);
    return channel;

}

void    server_c::bot_cmd(const std::string &buffer, const uint32_t &client_socket) {

    std::string channel_name = bot_pars(buffer);
    // std::cout << "|" << channel_name << "|" << std::endl;
    if (channel_name.empty())
        return ;
    std::string message;
    std::string message1;

    if (channels_map.find(channel_name) == channels_map.end()) {
        std::string err = "403 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :No such channel\r\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }
    message = "The channel " + channel_name + " has " + std::to_string(channels_map[channel_name]._members.size()) + " memebers: ";
    for (size_t i = 0; i < channels_map[channel_name]._members.size(); i++) {
        if (search_user(channels_map, channels_map[channel_name]._members[i], 'o', channel_name))
            message += "@";
        message += clients_map[channels_map[channel_name]._members[i]].getClient_nick() + " ";
    }
    message1 = ":" + clients_map[client_socket].getClient_nick() + " PRIVMSG BOT :" + message + "\r\n";
    if (send(client_socket, message1.c_str(), message1.size(), 0) == -1)
        std::cerr << "Error: send." << std::endl;
}

void server_c::channel_checker(std::string channel_name) {
    if (channels_map[channel_name]._members.size() == 0)
        channels_map.erase(channel_name);
}

void server_c::assign_operator(const std::string channel_name) {
    channels_map[channel_name]._operators.push_back(channels_map[channel_name]._members[0]);
}
