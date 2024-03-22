#include "headers/includes.h"

void    server_c::mode_cmd(const std::string &buffer, const uint32_t &client_socket) {

    std::pair <std::string, std::pair<std::string, std::string> > pair_mode = parse_mode(buffer);
    uint32_t    mode = 0;
    std::string channel_name = pair_mode.first;
    std::string modes = pair_mode.second.first;

    if (modes == "X")
    {
        std::string err = "501 " + channel_name + " :Unknown MODE flag\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
    }

    std::string subjects = pair_mode.second.second;
    // std::cout << "|" << subjects << "|" << std::endl;
    std::vector<std::string> subs;
    if (subjects.size() > 0)
        subs = sort_subs(subjects);

    //check if channels exists.
    if (channels_map.find(channel_name) != channels_map.end()) {
        std::string err = "403 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :No such channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }
    //check if user joined the channel.
    if (std::find(channels_map[channel_name]._members.begin(), channels_map[channel_name]._members.end(), client_socket)
        != channels_map[channel_name]._members.end()) {
        std::string err = "442 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not on that channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }
    //check if caller is an operatoer of that channel
    if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), client_socket)
            == channels_map[channel_name]._operators.end()) {
        std::string err = "482 " + clients_map[client_socket].getClient_nick() + " " + channel_name + " :You're not channel operator\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return ;
    }

    size_t s = 0;
    for (size_t i = 0; i < modes.size(); i += 2)
    {
        mode = (int)modes[i+1];
        switch(mode) {
            case(100):
                mode_i(channel_name, client_socket, modes[i] != '+' ? true : false);
                break;
            case(116):
                mode_t(channel_name, client_socket, modes[i] == '+' ? true : false);
                break;
            case(107):
                mode_k(channel_name, client_socket, subs.size() < s ? subs[s] : "", modes[i] == '+' ? true : false); s++;
                break;
            case(111):
                mode_o(channel_name, client_socket, subs.size() < s ? subs[s] : "", modes[i] == '+' ? true : false); s++;
                break;
            case(108):
                mode_l(channel_name, client_socket, modes[i] == '+' ? true : false, subs.size() < s ? subs[s] : ""); s++;
                break;
            default:
                std::string err = "501 " + channel_name + " :Unknown MODE flag\n";
                if (send(client_socket, err.c_str(), err.size(), 0) == -1)
                    std::cerr << "Error: send." << std::endl;
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
    for (std::map<uint32_t, client_c>::iterator it = clients_map.begin(); it != clients_map.end(); it++) {
        if (clients_map[it->first].getClient_nick() == target) {
            target_socket = it->first;
            break;
        }
    }
    //check target doesnt exist.
    if (!target_socket) {
        std::string err = "441 " + clients_map[client_socket].getClient_nick() + " " + target + " " + channel_name + " :They aren't on that channel\n";
        if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl; 
    }
    else if (set) {
        if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), target_socket) == channels_map[channel_name]._operators.end()) {
            channels_map[channel_name]._operators.push_back(target_socket);
            std::string message = ":" + clients_map[client_socket].getClient_nick() + " MODE " + channel_name + " +o " + target + "\n";
            if (send(client_socket, message.c_str(), message.size(), 0) == -1)
                std::cerr << "Error: send." << std::endl;
        }
    }
    else {
        if (std::find(channels_map[channel_name]._operators.begin(), channels_map[channel_name]._operators.end(), target_socket) != channels_map[channel_name]._operators.end()) {
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
    
    channels_c chanels;
    std::vector<std::pair<std::string, std::string> > kick_Vpair = join_kick_inv(buffer, 1);
    // (void)client_socket;
    puts("hello");

    if (kick_Vpair[0].second == "")
    {
       std::string err = "461 " + clients_map[client_socket].getClient_nick() + " KICK :Not enough parameters\n" ;
       if (send(client_socket, err.c_str(), err.size(), 0) == -1)
            std::cerr << "Error: send." << std::endl;
        return;
    }
    // for (size_t i = 0; i < kick_Vpair.size(); i++)
    // {
    //     // std::cout << "first: " << kick_Vpair[i].first << "seconde: " << kick_Vpair[i].second << std::endl;
    //     if (channels_map.find(kick_Vpair[i].first) != channels_map.end())
    //     {
    //         if (std::find(channels_map[kick_Vpair[i].first]._members.begin(),
    //          channels_map[kick_Vpair[i].first]._members.end(), kick_Vpair[i].second) == channels_map[kick_Vpair[i].first]._members.end())
    //             ;// print u r not on this channel
    //         else{
    //             //chack if operator
    //             if (std::find(channels_map[kick_Vpair[i].first]._operators.begin(),channels_map[kick_Vpair[i].first]._operators.end(), kick_Vpair[i].second) == channels_map[kick_Vpair[i].first]._operators.end())
    //                 ; // not an operator;
    //             else
    //                 channels_map._members.erase()
    //         }
    //     }
    // }
    // for (size_t i = 0; i < kick_Vpair.size(); i++) {
    //     if (channels_map.find(kick_Vpair[i].first) != channels_map.end()) {
    //         channels_c &channel = channels_map[kick_Vpair[i].first]; // Reference to the channel object
    //         std::vector<uint32_t> &members = channel._members; // Reference to the member list of the channel

    //         // Check if the user is a member of the channel

    //         if (std::find(members.begin(), members.end(), kick_Vpair[i].second) != members.end()) {
    //             // Check if the user is an operator
    //             std::vector<uint32_t> &operators = channel._operators; // Reference to the operators list of the channel
    //             if (std::find(operators.begin(), operators.end(), kick_Vpair[i].second) != operators.end()) {
    //                 // User is an operator, remove from operators list
    //                 // members.erase(std::remove(operators.begin(), operators.end(), kick_Vpair[i].second), operators.end());
    //             }
    //             // Remove the user from the members list
    //             // members.erase(it);
    //         }
    //     }
    }



void    server_c::invite_cmd(const std::string &buffer, const uint32_t &client_socket) {
    
    // std::vector<uint32_t> &operators = channel._operators;
    std::vector<std::pair<std::string, std::string> > invitations = join_kick_inv(buffer, 3);

    for (size_t j = 0; j != invitations.size(); j++)
    {
            // if (invitations[j].second chanel exists)
            {
                //if (client_socket is opperator of invitation[j].second)
                {
                    // if (invititations[j].first user exists)
                    {
                        //add inivitations[j].first user to vector invited;
                    }
                }
            }
    }



    (void)client_socket;
}

void    server_c::topic_cmd(const std::string &buffer, const uint32_t &client_socket) {

    std::pair<std::string, std::string> to_pair = topic_parse(buffer);
    std::string channel_name = to_pair.first;
    std::string topic = to_pair.second;
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
                + "TOPIC :" + clients_map[client_socket].getClient_nick() + " has set new topic to " + topic + "\n";
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
