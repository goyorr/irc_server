#include "server.hpp"
#include "client.hpp"

int is_ws(int c)
{
    if (c == 32 || c == '\t')
        return 1;
    return 0;
}

int is_end(std::string str, int *i)
{
    if (str[*i] == '\r' || str[*i] == '\n' || !str[*i])
        return 1;
    return 0;
    
}


std::pair<int, std::string> regi_parse(std::string str, int flag)
{
    std::string cmd;
    if (!flag)
        cmd = "PASS";
    else if (flag == 1)
        cmd = "NICK";
    else if (flag == 2)
        cmd = "USER";
    
    std::pair<int, std::string> res;
    int i = 0;
    while(is_ws(str[i]))
        i++;

    int tmp = i;
    while (!is_ws(str[i]))
    {
        if (is_end(str, &i))
            break ;
        i++;
    }
    std::string pre = str.substr(tmp, i - tmp);
    if (strcmp(pre.c_str(), "PASS"))
    {
        res.first = 1; res.second = pre;
        return (res);
    }
    if (str.find(" ") == std::string::npos && str.find("\t") == std::string::npos)
    {
        res.first = 2; res.second = pre;
        return (res);
    }
    while (is_ws(str[i]))
        i++;
    if (is_end(str, &i))
    {
        res.first = 2; res.second = pre;
        return (res);
    }
    tmp = i;
    while(!is_ws(str[i]) && !is_end(str, &i))
        i++;
    std::string post = str.substr(tmp, i - tmp);
    while (is_ws(str[i]))
        i++;
    if (is_end(str, &i))
    {
        res.first = 0; res.second = post;
        return (res);
    }
    res.first = 3; res.second = pre;
    return (res);
}


void    server_c::pars_cmd(const std::string &buffer, const uint16_t &client_socket)
{
    if (authed_clients_map.find(client_socket) != authed_clients_map.end()) 
    {
        // if (registered_clients_map.find("?"/*nick name from buffer*/) != registered_clients_map.end()) 
        if (authed_clients_map[client_socket] == 2)
        // if (registered_clients_map[client_socket].getRegistered() == true)
        {
            //pars then check for all commands normaly.
            //check for all commands normaly (already set pass, nick, user?).
        }
        else 
        {
            //still needs to be registered (only check nick and user?).
            std::pair<int , std::string> nickpair = regi_parse(buffer, 1);
            if (!nickpair.first) {
                client_c _client;

                _client.setClient_nick(nickpair.second);
                registered_clients_map[nickpair.second] = _client;
            }
            else {
                if (nickpair.first == 1)
                    std::cerr << nickpair.second << " :Unknown comand" << std::endl;
                else if (nickpair.first == 2) // find appropriate Error Msg.
                    std::cerr << nickpair.second << " :Not enough parameters" << std::endl;
                else if (nickpair.first == 3)
                    std::cerr << nickpair.second << " :Too many parameters" << std::endl;
                return ;
            }

            std::pair<int , std::string> userpair = regi_parse(buffer, 2);
            if (!userpair.first) {
                client_c _client;
                _client.setClient_user(userpair.second);
                registered_clients_map[userpair.second] = _client;
            }
            else {
                if (userpair.first == 1)
                    std::cerr << userpair.second << " :Unknown comand" << std::endl;
                else if (userpair.first == 2) // find appropriate Error Msg.
                    std::cerr << userpair.second << " :Not enough parameters" << std::endl;
                else if (userpair.first == 3)
                    std::cerr << userpair.second << " :Too many parameters" << std::endl;
                return ;
            }

            if ((registered_clients_map[nickpair.second].getClient_nick()) != "_Default_"  && registered_clients_map[userpair.second].getClient_user() != "_Default_")
                authed_clients_map[client_socket] = 2;
        }
    }
    else {
        std::pair<int , std::string> pairpass = regi_parse(buffer, 0);
        if (!pairpass.first) {
            if (pairpass.second == getPassword()) {
                authed_clients_map[client_socket] = true;
                std::cout << "socket #" << client_socket << " authenticated successfully." << std::endl;
                if (send(client_socket, "Welcome to IRC server...\n",  25, 0) == -1) {
                    std::cerr << "Error: send." << std::endl;
                    exit (1);
                }
            }
        }
        else {
        if (pairpass.first == 1)
            std::cerr << pairpass.second << " :Unknown comand" << std::endl;
        else if (pairpass.first == 2) // find appropriate Error Msg.
            std::cerr << pairpass.second << " :Not enough parameters" << std::endl;
        else if (pairpass.first == 3)
            std::cerr << pairpass.second << " :Too many parameters" << std::endl;
        }
    }
}
