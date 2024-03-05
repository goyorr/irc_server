#include "server.hpp"

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
    else if (flag == 3)
        cmd = "PRIVMSG";
    
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
    if (strcmp(pre.c_str(), cmd.c_str()))
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

void    server_c::pars_port(const std::string &port) {
    if (port.size() > 5) {
        std::cerr << "Error: invalid port!" << std::endl;
        exit (1);
    }
    for (size_t i = 0; i < port.size(); i++) {
        if (!std::isdigit(port[i])) {
            std::cerr << "Error: invalid port!" << std::endl;
            exit (1);
        }
    }
}
