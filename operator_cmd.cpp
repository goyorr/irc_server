#include "headers/includes.h"

void    server_c::mode_cmd(const std::string &buffer, const uint32_t &client_socket) {

    std::pair <std::string, std::pair<std::string, std::string> > pair_mode = parse_mode(buffer);
    uint32_t    mode = 0;
    std::string channel_name = pair_mode.first;
    std::string modes = pair_mode.second.first;
    std::string subjects = pair_mode.second.second;
    std::vector<std::string> subs;
    if (subjects.size() > 0)
        subs = sort_subs(subjects);

    //check if channels exists.
    if (channels_map.find(channel_name) == channels_map.end()) {
        std::string err = "403 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :No such channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }
    //check if user joined the channel.
    if (!search_user(channels_map, client_socket, 'm', channel_name)) {
        std::string err = "442 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not on that channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }
    //check if caller is an operatoer of that channel
    if (!search_user(channels_map, client_socket, 'o', channel_name)) {
        std::string err = "482 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not channel operator\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }

    size_t s = 0;
    for (size_t i = 0; i < modes.size(); i += 2)
    {
        //fix: subs strings has 2 extra characters.
        mode = (int)modes[i+1];
        switch(mode) {
            case(105):
                mode_i(channel_name, client_socket, modes[i] == '+' ? true : false);
                break;
            case(116):
                mode_t(channel_name, client_socket, modes[i] == '+' ? true : false);
                break;
            case(107):
                mode_k(channel_name, client_socket, subs.size() > s ? subs[s] : "", modes[i] == '+' ? true : false); s++;
                break;
            case(111):
                mode_o(channel_name, client_socket, subs.size() > s ? subs[s] : "", modes[i] == '+' ? true : false); s++;
                break;
            case(108):
                mode_l(channel_name, client_socket, modes[i] == '+' ? true : false, subs.size() < s ? subs[s] : ""); s++;
                break;
        }
    }
}

void    server_c::mode_i(std::string channel_name, uint32_t client_socket, bool set) {
    if (set) {
        channels_map[channel_name].setisinvite_only(true);
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " +i\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
    else {
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " -i\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        channels_map[channel_name].setisinvite_only(false);
    }
}

void    server_c::mode_t(std::string channel_name, uint32_t client_socket, bool set) {
    if (set) {
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " +t\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        channels_map[channel_name].setisrestricted_topic(true);
    }
    else {
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " -t\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        channels_map[channel_name].setisrestricted_topic(false);
    }
}

void    server_c::mode_k(std::string channel_name, uint32_t client_socket, std::string password, bool set) {
    if (set) {
        if (password.empty())
            return ;
        channels_map[channel_name].setChannelPassword(password);
        channels_map[channel_name].setisProtected(true);
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " +k\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
    else {
        channels_map[channel_name].setChannelPassword(NULL);
        channels_map[channel_name].setisProtected(false);
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " -k\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
}

void    server_c::mode_o(std::string channel_name, uint32_t client_socket, std::string target, bool set) {
    uint32_t target_socket = 0;
    if (target.empty())
            return ;
    bool found = false;
    for (std::map<uint32_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); it++) {
        if (clients_map[it->first].getClient_nick() == target) {
            target_socket = it->first;
            found = true;
            break;
        }
    }
    if (!found) {
        std::string err_msg = "401 " + clients_map[client_socket].getClient_nick() + " " + target + " :No such nick\n";
        if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }
    if (!target_socket) {
        std::string err = "441 " + clients_map[client_socket].getClient_nick() + " " + target + " " + channel_name + " :They aren't on that channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl; 
    }
    else if (set) {
        if (!search_user(channels_map, target_socket, 'o', channel_name)) {
            channels_map[channel_name]._operators.push_back(target_socket);
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " +o " + target + "\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
    else {
        if (search_user(channels_map, target_socket, 'o', channel_name)) {
            channels_map[channel_name]._operators.erase(std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), target_socket));
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " -o " + target + "\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
}

void    server_c::mode_l(std::string channel_name, uint32_t client_socket, bool set, std::string limit) {
    //check if limit is a possitive number.
    if (limit.empty())
        return ;
    uint32_t limit_int = std::atoi(limit.c_str());
    if (set) {
        channels_map[channel_name].setuser_limit(limit_int);
        channels_map[channel_name].setisuser_limit(true);
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " +l\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
    else {
        std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " -l\n";
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        channels_map[channel_name].setisuser_limit(true);
    }
}

void    server_c::kick_cmd(const std::string &buffer, const uint32_t &client_socket) {
    std::vector<std::pair<std::string, std::string> > kick_Vpair = join_kick_inv(buffer, 1);
    std::string err_msg;

    if (kick_Vpair[0].second == "") {
       std::string err = "461 " + clients_map[client_socket].getClient_nick() + " KICK :Not enough parameters\n" ;
       if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return;
    }
    // kick one at a time
    std::string channel = kick_Vpair[0].first;
    std::string user = kick_Vpair[0].second;
    std::string comment = kick_Vpair[1].first;
    if (channels_map.find(channel) != channels_map.end()) {
        if (search_user(channels_map, client_socket, 'm', channel)) {
            if (search_user(channels_map, client_socket, 'o', channel)) {
                for (std::map<uint32_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); it++) {
                    if (clients_map[it->first].getClient_nick() == user) {
                        if(search_user(channels_map, it->first, 'm', channel)) {
                            for (size_t i = 0; i < channels_map[channel]._members.size(); i++) {
                                if (it->first == channels_map[channel]._members[i]) {
                                    std::string rpl_msg = ":" + clients_map[client_socket].getClient_nick() + " KICK " + channel + " " + user + " :" + comment + "\n";
                                    if (send(client_socket, rpl_msg.c_str(), rpl_msg.size(), 0) == -1)
                                    std::cerr << "Error: send." << std::endl;
                                    channels_map[channel]._members.erase(channels_map[channel]._members.begin() + i);      //ERASE user
                                    return ;
                                }
                            }
                        }
                        else {
                            err_msg = "441 " + clients_map[client_socket].getClient_nick() + " " + user + " " + channel + " :They aren't on that channel\n";
                            if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                                std::cerr << "Error: send." << std::endl;
                            return ;
                        }
                    }
                }
                //not in server
                err_msg = "401 " + clients_map[client_socket].getClient_nick() + " " + user + " :No such nick\n";
                if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
            else {
                err_msg = "482 " + clients_map[client_socket].getClient_nick() + " " + channel + " :You're not channel operator\n";
                if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
        }
        else {
            err_msg = "442 " + clients_map[client_socket].getClient_nick() + " " + channel + " :You're not on that channel\n";
            if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
    else {
            err_msg = "403 " + clients_map[client_socket].getClient_nick() + " " + channel + " :No such channel \n";
            if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
    }
}

void    server_c::invite_cmd(const std::string &buffer, const uint32_t &client_socket) {
    
    std::vector<std::pair<std::string, std::string> > invitations = join_kick_inv(buffer, 3);

    std::string err_msg;
    std::string chanel;
    std::string user;
    for (size_t j = 0; j != invitations.size(); j++)
    {
            user = invitations[j].first;
            chanel = invitations[j].second;
            if (channels_map.find(invitations[j].second) != channels_map.end()) 
            {
                if (search_user(channels_map, client_socket, 'm', chanel))        
                {
                    if (search_user(channels_map, client_socket, 'o', chanel))       
                    {
                        for (std::map<uint32_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); it++){
                            if (clients_map[it->first].getClient_nick() == user)
                            {
                                if (!search_user(channels_map, it->first, 'm', chanel))  
                                {
                                    std::string rpl_msg = "341 " + clients_map[client_socket].getClient_nick() + " " + user + " " + chanel + "\n";
                                    channels_map[chanel]._invited.push_back(it->first);
                                    if (send(client_socket, rpl_msg.c_str(), rpl_msg.size(), 0) == -1)
                                        std::cerr << "Error: send." << std::endl;
                                }
                                else
                                {                               
                                    err_msg = "443 " + clients_map[client_socket].getClient_nick() + " " + user + " " + chanel + " :is already on channel\n";
                                    if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                                        std::cerr << "Error: send." << std::endl;

                                }
                            }

                        }
                    }
                    else
                    {
                        err_msg = "482 " + clients_map[client_socket].getClient_nick() + " " + chanel + " :You're not channel operator\n";
                        if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                            std::cerr << "Error: send." << std::endl;
                    }
                }
                else 
                {
                        err_msg = "442 " + clients_map[client_socket].getClient_nick() + " " + chanel + " :You're not on that channel\n";
                        if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                            std::cerr << "Error: send." << std::endl;
                }
            }
            else
            {
                err_msg = "403 " + clients_map[client_socket].getClient_nick() + " " + chanel + " :No such channel \n";  //  "<client> <channel> :No such channel"  403
                if (send(client_socket, err_msg.c_str(), err_msg.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
            }
    }



    (void)client_socket;
}

void    server_c::topic_cmd(const std::string &buffer, const uint32_t &client_socket) {
    //after pars return channel name andd topic;

    std::pair<std::string, std::string> to_pair = topic_parse(buffer); 
    std::string channel_name = to_pair.first;
    std::string topic = to_pair.second;
    if (channels_map.find(channel_name) != channels_map.end()) {
        if (!search_user(channels_map, client_socket, 'm', channel_name)) {
            std::string err = "442 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not on that channel\n";
            if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
        //view mode.
        else if (topic.empty()) {
            std::string message;

            if (channels_map[channel_name].getTopic().empty())
                message = "331 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :No topic is set\n";
            else
                message = "332 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :" + channels_map[channel_name].getTopic() + "\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
        //chanege mode.
        else {
            if (channels_map[channel_name].getisrestricted_topic()) {
                if (!search_user(channels_map, client_socket, 'o', channel_name)) {
                    std::string err = "482 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not channel operator\n";
                    if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                    return ;
                }
            }
            std::string message;
            if (topic == ":") {
                message =  clients_map[client_socket].getClient_nick() + " " + channel_name
                + " TOPIC :" + clients_map[client_socket].getClient_nick() + " has cleared the channel's topic\n";
                channels_map[channel_name].setTopic(NULL), void(NULL);
            }
            else {
                message =  clients_map[client_socket].getClient_nick() + " " + channel_name
                + " TOPIC :" + clients_map[client_socket].getClient_nick() + " has set new topic to " + topic + "\n";
                channels_map[channel_name].setTopic(topic), void(NULL);
            }
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
    else {
        std::string err = "403 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :No such channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }
}
