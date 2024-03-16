#include "headers/includes.h"

void    server_c::mode_cmd(const std::string &buffer, const uint32_t &client_socket) {
    // after parsing get mode and channel_name
    uint32_t    mode = 0;
    std::string channel_name;

    //check if channel exists and user is in that channel and is an operator.
    switch(mode) {
        case(100)://i
            mode_i_pars(buffer, client_socket);
            break;
        case(116)://t
            mode_t_pars(buffer, client_socket);
            break;
        case(107)://k
            mode_k_pars(buffer, client_socket);
            break;
        case(111)://o
            mode_o_pars(buffer, client_socket);
            break;
        case(108)://l
            mode_l_pars(buffer, client_socket);
    }
}

void    server_c::mode_i_pars(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    std::string channel_name = "d";
    bool set = 0;
    mode_i(channel_name, client_socket, set);
}

void    server_c::mode_t_pars(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    std::string channel_name = "d";
    bool set = 0;
    mode_t(channel_name, client_socket, set);
}

void    server_c::mode_k_pars(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    std::string channel_name = "d", password = "e";
    bool set = 0;
    mode_k(channel_name, client_socket, password, set);
}

void    server_c::mode_o_pars(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    std::string channel_name = "d", op = "d";
    uint32_t target = 0;
    mode_o(channel_name, client_socket, target, op);
}

void    server_c::mode_l_pars(const std::string &buffer, const uint32_t &client_socket) {
    (void)buffer;
    std::string channel_name = "D";
    bool set = 0;
    uint32_t limit = 0;
    mode_l(channel_name, client_socket, set, limit);
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
void    server_c::mode_o(std::string channel_name, uint32_t client_socket, uint32_t target, std::string op) {
    (void)client_socket;
    if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), client_socket)
            != channels_map[channel_name]._operators.end()) {
        //send
    }
    else if (op == "give")
        channels_map[channel_name]._operators.push_back(target);
    else
        channels_map[channel_name]._operators.erase(std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), target));
}

void    server_c::mode_l(std::string channel_name, uint32_t client_socket, bool set, uint32_t limit) {
    (void)client_socket;
    if (set) {
        channels_map[channel_name].setuser_limit(limit);
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
