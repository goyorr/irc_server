#include "./headers/includes.h"

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

std::pair<int, std::string> user_parse(std::string usr)     // mlk 0 * realname
{
    std::pair <int , std::string> res;
    int i = 0;
    int pos = i;
    while(!is_ws(usr[i]))  // user;
        i++;
    std::string username = usr.substr(pos, i - pos);
    while (is_ws(usr[i]))
        i++;
    if (is_end(usr, &i)) {
        res.first = 2; return res;
    }
    if (usr[i] != '0') {
        res.first = 1; return res;
    }
    i++;
    if (is_end(usr, &i)) {
        res.first = 2; return res;
    }
    while(is_ws(usr[i]))   
        i++;
    if (is_end(usr, &i)) {
        res.first = 2; return res;
    }
    if (usr[i] != '*') {
        res.first = 3; return res;
    }
    i++;
    if (is_end(usr, &i)) {
        res.first = 2; return res;
    }
    while (is_ws(usr[i]))    
        i++;
    if (is_end(usr, &i)) {
        res.first = 2; return res;
    }
    pos = i;
    while (!is_ws(usr[i]))      // realname;
        i++;
    std::string realname = usr.substr(pos, i - pos);
    res.second = username + " " + realname;
    res.first = 0;
    return res;
}

std::pair<int, std::string> regi_parse(std::string str, int flag)
{
    std::string cmd;
    std::pair<int, std::string> res;
    if (!flag)
        cmd = "PASS";
    else if (flag == 1)
        cmd = "NICK";
    else if (flag == 2)
    {
        cmd = "USER";
    }
    else if (flag == 3)
        cmd = "PRIVMSG";
    
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
    if (flag == 2)
    {
        std::string post = str.substr(tmp, str.size() - tmp);
        std::pair<int, std::string> user_check = user_parse(post);
        if (!user_check.first)
        {
            res.first = 0;
            res.second = user_check.second;
            return (res);
        }
        else if (user_check.first == 1) {
            res.first = 1; return res;
        }
        else if (user_check.first == 3) {
            res.first = 3; return res;
        }
        else if (user_check.first == 2) {
            res.first = 2; return res;
        }
    }
    std::string post = str.substr(tmp, i - tmp);
    res.second = post;
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

std::pair<std::vector<std::string>, std::string > prvmsg_parse(std::string buffer)
{
    std::pair <std::vector<std::string>, std::string> res;
    std::string msg;
    std::string tmp;
    size_t pos = 0;

    int i = 0;
    while(!is_ws(buffer[i]))
        i++;
    while(is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
        return res;
    pos = i;
    while(buffer[i] && !is_ws(buffer[i]))
        i++;
    tmp = buffer.substr(pos, i - pos);
    while(is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
        return (res);
        pos = i;
        while(!is_end(buffer, &i))
            i++;
        msg = buffer.substr(pos, i - pos);
        res.second = msg;
        i = 0;
        while(!is_end(tmp, &i))
        {
            pos = tmp.find(',');
            if (pos == std::string::npos)
            {
                res.first.push_back(tmp.substr(i, tmp.size() - i));
                break ;
            }
            res.first.push_back(tmp.substr(i, pos - i));
            tmp = strchr(tmp.c_str(), ',');
            tmp.erase(tmp.begin());
        }
    return (res);
}

std::string select_cmd(std::string buffer)
{
    int i = 0;
    while (is_ws(buffer[i]))
        i++;
    int pos = i;
    while (!is_ws(buffer[i]) && !is_end(buffer, &i))
        i++;
    std::string cmd = buffer.substr(pos, i - pos);

    return cmd;
}
