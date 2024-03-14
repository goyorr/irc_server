#include "headers/includes.h"

//????
 std::vector<std::pair<std::string, std::string> > join_kick(std::string buffer, int jk){
    
    std::vector<std::pair<std::string, std::string> > chan_pass;
    int i = 0;
    while (is_ws(buffer[i]))
        i++;
    while (!is_ws(buffer[i]))
        i++;
    while (is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
        return chan_pass.push_back(std::make_pair("/'", "\'")), chan_pass;  // not enough args ERR;

    size_t pos = buffer.size();
    pos--;
    while(is_end(buffer, &i))
        pos--;
    while (is_ws(buffer[pos]))
        pos--;

    std::string args = buffer.substr(i, pos - i);    //#chan,#chan1 pass,pass1
    i = 0;
    pos = i;
    while (!is_ws(args[i]))
        i++;

    std::string chanels = args.substr(pos, i - pos);
    while(is_ws(args[i]))
        i++;
    if (is_end(args, &i))
        return chan_pass.push_back(std::make_pair(chanels, "")), chan_pass;
    pos = i;
    while (args[i] && !is_ws(args[i] && !is_end(args, &i)))
        i++;
    std::string mps = args.substr(pos, (i + 1) - pos );

    std::vector<std::string> chans;
    i = 0;
    size_t cpos;

    while (!is_end(chanels, &i))
    {
        cpos = chanels.find(',', i);
        if (cpos == std::string::npos)
        {   
            chans.push_back(chanels.substr(i, chanels.size() - i));
            break;
        }
        chans.push_back(chanels.substr(i, cpos - i));
        i = cpos + 1;
    }

    std::vector<std::string> pass;
    i = 0;
    while (!is_end(mps, &i))
    {
        cpos = mps.find(',', i);
        if (cpos == std::string::npos)
        {
            pass.push_back(mps.substr(i, mps.size() - i));
            break;
        }

        pass.push_back(mps.substr(i, cpos - i));
        i = cpos + 1;
    }

    if (jk == 0)
    {
        std::vector<std::string>::iterator it_pass = pass.begin();
        for (std::vector<std::string>::iterator it_chans = chans.begin(); it_chans != chans.end(); ++it_chans)
        {
            while(it_pass != pass.end())
            {
                chan_pass.push_back(std::make_pair(*it_chans, *it_pass));
                it_pass++;
                it_chans++;
            }
            if (it_pass == pass.end() && it_chans != chans.end())
                chan_pass.push_back(std::make_pair(*it_chans, ""));
            else
                break;
        }
    }
    else if (jk == 1)
    {
        std::vector<std::pair<std::string, std::string> > chan_usr;
        std::vector<std::string> users = pass;
        std::vector<std::string>::iterator it_users = users.begin();
        std::vector<std::string>::iterator max = it_users;
        for (std::vector<std::string>::iterator it_chans = chans.begin(); it_chans != chans.end(); ++it_chans)
        {
            while(it_users != users.end())
            {
                chan_usr.push_back(std::make_pair(*it_chans, *it_users));
                it_users++;
                it_chans++;
                if (it_users != users.end())
                    max++;
            }
            if (it_users == users.end())
            {
                while (it_chans != chans.end())
                {
                    chan_usr.push_back(std::make_pair(*it_chans, *max));
                    it_chans++;
                }
            }
            break;
        }
        return (chan_usr);
    }
    return chan_pass;
}
