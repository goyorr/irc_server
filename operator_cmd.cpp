#include "headers/includes.h"

void    server_c::mode_cmd(const std::string &buffer, const uint32_t &client_socket) {

    std::pair <std::string, std::pair<std::string, std::string> > pair_mode = parse_mode(buffer);
    uint32_t    mode = 0;
    std::string chanel_name = pair_mode.first;
    std::string modes = pair_mode.second.first;  // +i+t+l
    std::string subjects = pair_mode.second.second;
    std::vector<std::string> subs;  //gshdgh,hsfdhsd,hsgdhsgd,hgdhsgd
    if (subjects.size() > 0)
        subs = sort_subs(subjects);
    size_t s = 0;
    for (size_t i = 0; i < modes.size(); i += 2)
    {
        mode = (int)modes[i+1];
        switch(mode) {
            case(100)://i
                mode_i(chanel_name, client_socket, modes[i] != '+' ? true : false);
                break;
            case(116)://t
                mode_t(chanel_name, client_socket, modes[i] == '+' ? true : false);
                break;
            case(107)://k
                mode_k(chanel_name, client_socket, subs.size() < s ? subs[s] : "", modes[i] == '+' ? true : false); s++;
                break;
            case(111)://o
                mode_o(chanel_name, client_socket, subs.size() < s ? subs[s] : "", modes[i] == '+' ? true : false); s++;
                break;
            case(108)://l
                mode_l(chanel_name, client_socket, modes[i] == '+' ? true : false, subs.size() < s ? subs[s] : ""); s++;
                break;
        }
    }
}

//done + add send.
void    server_c::mode_i(std::string channel_name, uint32_t client_socket, bool set) {
    (void)client_socket;
    if (set)
        channels_map[channel_name].setisinvite_only(true);
    else
        channels_map[channel_name].setisinvite_only(false);
}
//done + add send.
void    server_c::mode_t(std::string channel_name, uint32_t client_socket, bool set) {
    (void)client_socket;
    if (set)
        channels_map[channel_name].setisrestricted_topic(true);
    else
        channels_map[channel_name].setisrestricted_topic(false);
}
//done + add send.
void    server_c::mode_k(std::string channel_name, uint32_t client_socket, std::string password, bool set) {
    (void)client_socket;
    if (set) {
        channels_map[channel_name].setChannelPassword(password);
        channels_map[channel_name].setisProtected(true);
    }
    else {
        channels_map[channel_name].setChannelPassword(NULL);
        channels_map[channel_name].setisProtected(false);
    }
}
//done + add send.
void    server_c::mode_o(std::string channel_name, uint32_t client_socket, std::string target, bool set) {
    (void)client_socket;
    (void)target;
    if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), client_socket)
            != channels_map[channel_name]._operators.end()) {
        //send
    }
    else if (set)
        ;// channels_map[channel_name]._operators.push_back(target);
    else
        // channels_map[channel_name]._operators.erase(std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), target));
        ;
}

void    server_c::mode_l(std::string channel_name, uint32_t client_socket, bool set, std::string limit) {
    (void)limit;
    (void)client_socket;
    if (set) {
        // channels_map[channel_name].setuser_limit(limit);
        channels_map[channel_name].setisuser_limit(true);
    }
    else
        channels_map[channel_name].setisuser_limit(true);
}

void    server_c::kick_cmd(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    (void)client_socket;
}

void    server_c::invite_cmd(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    (void)client_socket;
}

void    server_c::topic_cmd(const std::string &buffer, const uint32_t &client_socket) {
    //after pars return channel name andd topic;
    (void)buffer;
    std::string channel_name;
    std::string topic;

    if (channels_map.find(channel_name) != channels_map.end()) {
        if (std::find(channels_map[channel_name]._members.begin(), channels_map[channel_name]._members.end(), client_socket)
            != channels_map[channel_name]._members.end()) {
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
                if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), client_socket)
                == channels_map[channel_name]._operators.end()) {
                    std::string err = "482 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not channel operator\n";
                    if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                        std::cerr << "Error: send." << std::endl;
                    return ;
                }
            }
            std::string message;
            if (topic == ":") {
                message =  clients_map[client_socket].getClient_nick() + " " + channel_name
                + "TOPIC :" + clients_map[client_socket].getClient_nick() + " has cleared the channel's topic\n";
                channels_map[channel_name].setTopic(NULL), void(NULL);
            }
            else {
                message =  clients_map[client_socket].getClient_nick() + " " + channel_name
                + "TOPIC :" + clients_map[client_socket].getClient_nick() + " has set new topic to " + topic +"\n";
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
