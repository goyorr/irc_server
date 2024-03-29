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

bool is_num(std::string str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (!isnumber(str[i]))
            return false;
    }
    return true;
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
    if (is_end(usr, &i)){
        res.first = 2; return res;
    }

    if (usr[i] != ' ' && usr[i] != '\t'){
        res.first = 3; return res;
    }
    while (is_ws(usr[i]))    
        i++;
    if (is_end(usr, &i)){
        res.first = 2; return res;
    }
    pos = i;
    if (usr[i] != ':')
    {
        while (!is_ws(usr[i]))      // realname;
            i++;
    }
    else{
        pos++;                     // skip FIRST ws for real name in case of : ?
        while (!is_end(usr, &i))
            i++;
    }
    std::string realname = usr.substr(pos, i - pos);
    res.second = username + " " + realname;
    res.first = 0;
    return res;
}

std::pair<int, std::string> nick_parse(std::string str){

    std::pair<int, std::string> nick_check;
    int i = 0;
    while (!is_end(str, &i))
        i++;
    std::string nick = str.substr(0, i);
    i = 0;
    while(nick[i]){
        if (!isalnum(nick[i]) && nick[i] != '[' && nick[i] != ']' && nick[i] != '|' && nick[i] != '{' && nick[i] != '}' && nick[i] != '\'' && nick[i] != '_')
            nick_check.first = 1;   
        i++;
    }
    return nick_check ;
}

std::pair<int, std::string> regi_parse(std::string str, int flag) {
    std::string cmd;
    std::pair<int, std::string> res;
    if (!flag)
        cmd = "PASS";
    else if (flag == 1)
        cmd = "NICK";
    else if (flag == 2)
        cmd = "USER";
    else if (flag == 3)
        cmd = "PRIVMSG";
    
    int i = 0;
    while(is_ws(str[i]))
        i++;

    int tmp = i;
    while (!is_ws(str[i])) {
        if (is_end(str, &i))
            break ;
        i++;
    }
    std::string pre = str.substr(tmp, i - tmp);
    if (strcmp(pre.c_str(), cmd.c_str())) {
        res.first = 1; res.second = pre;
        return (res);
    }
    if (str.find(" ") == std::string::npos && str.find("\t") == std::string::npos) {
        res.first = 2; res.second = pre;
        return (res);
    }
    while (is_ws(str[i]))
        i++;
    if (is_end(str, &i)) {
        res.first = 2; res.second = pre;
        return (res);
    }
    tmp = i;
    while(!is_ws(str[i]) && !is_end(str, &i))
        i++;

    if (flag == 1)
    {
        std::string post = str.substr(tmp, str.size() - tmp);
        std::pair<int, std::string> nick_check = nick_parse(post);
        if (nick_check.first == 1)
        {
            res.second = nick_check.second;
            res.first = 1; return res;
        }
        else
            res.first = 0;
    }
    if (flag == 2) {
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

std::pair<std::vector<std::string>, std::string > prvmsg_parse(std::string buffer) {
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
        while(!is_end(tmp, &i)) {
            pos = tmp.find(',');
            if (pos == std::string::npos) {
                res.first.push_back(tmp.substr(i, tmp.size() - i));
                break ;
            }
            res.first.push_back(tmp.substr(i, pos - i));
            tmp = strchr(tmp.c_str(), ',');
            tmp.erase(tmp.begin());
        }
    return (res);
}

std::string select_cmd(std::string buffer) {
    int i = 0;
    while (is_ws(buffer[i]))
        i++;
    int pos = i;
    while (!is_ws(buffer[i]) && !is_end(buffer, &i))
        i++;
    std::string cmd = buffer.substr(pos, i - pos);

    return cmd;
}

std::pair<std::string, std::pair<std::string, std::string> > parse_mode(std::string buffer) {
    
    std::pair<std::string, std::pair<std::string, std::string> >res;
    int pos = buffer.find('E');

    pos++;
    while (is_ws(buffer[pos]))
        pos++;
    int i = pos;
    while (!is_ws(buffer[pos]))
        pos++;
    if (is_end(buffer, &pos)) {
        res.second.first = "xx";    // not enough param
        return res;
    }
    std::string chanel = buffer.substr(i, pos - i);
    res.first = chanel;

    while(is_ws(buffer[pos]))
        pos++;
    if (is_end(buffer, &i)) {
        res.second.first = "xx";    // not enough param
        return res;
    }
    i = pos;
    while (!is_ws(buffer[pos]) && !is_end(buffer, &pos))
        pos++;
    // if (is_end(buffer, &pos))
    std::string mods = buffer.substr(i, pos - i);
    res.second.first = mods;
    size_t err = mods.find_first_not_of("tikol+-");
    if (err != std::string::npos) {
        res.second.first = "00";
        return res;         // unknown mod;
    }
    else {
        std::string mo;
        int j = 0;
        int nj;
        for (std::string::iterator it = mods.begin(); it != mods.end(); it+=2 ) {
            nj = j+2;
            mo = mods.substr(j, nj - j);
            if ((mo[0] != '-' && mo[0] != '+') || (mo[1] != 'i' && mo[1] != 't' && mo[1] != 'k' && mo[1] != 'o' && mo[1] != 'l')) {
                res.second.first = "00";
                return res;    // unknown mod, wrong syntax;
            }
            j = nj;
        }
        while (is_ws(buffer[pos]))
            pos++;
        if (!is_end(buffer, &pos)){
            i = pos;
            while (!is_ws(buffer[pos]) && !is_end(buffer, &pos))
                pos++;
            std::string extra = buffer.substr(i, pos - i);
            res.second.second = extra;
        }

    }   
    return res;
}

std::vector<std::string> sort_subs(std::string subjects) {

    std::vector<std::string> res;
    size_t pos;
    for (size_t i = 0; i < subjects.size(); i++) {

        pos = subjects.find(',', i);
        if (pos != std::string::npos)
            res.push_back(subjects.substr(i, pos - i));
        else {
            res.push_back(subjects.substr(i, pos - i));
            break;
        }
        i = pos;
    }
    return res;
}


std::pair<std::string, std:: string> topic_parse(std::string buffer) {

    std::pair<std::string, std::string> res;
    int i = buffer.find('C');
    i++;
    while (is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
    {
        res.first = ""; // no chanel;
        return res;
    }
    int pos = i;
    while (!is_ws(buffer[i]) && !is_end(buffer, &i))
        i++;
    std::string chanel = buffer.substr(pos, i - pos);
    res.first = chanel;

    while(is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
    {
        res.second = "";      // no second arg
        return res;
    }

    if (buffer[i] == ':')
    {
        while (buffer[i] == ':')
            i++;          // : detected, skip all of them
    }
    else
    {
        res.second = "";   // NO : detected ignoring everything after chanel name. 
        return res;
    }
    while (is_ws(buffer[i]) && !is_end(buffer, &i))
        i++;
    if (is_end(buffer, &i))
    {
        res.second = ":";               // : added in second but no topic (delete topic)
        return res;
    }
    else
    {                                   // : and more (everything after is topic)
            pos = i;
            while (!is_end(buffer, &i))
                i++;
            std::string topic = buffer.substr(pos, i - pos);
            res.second = topic;
        
    }
    return res;
}
