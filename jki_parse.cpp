#include "headers/includes.h"

std::pair<std::vector<std::string>, std::vector<std::string> > join_pars(std::string buffer) {

    std::pair<std::vector<std::string>, std::vector<std::string> > join_pair;
    int i = 0;
    while (is_ws(buffer[i]))
        i++;
    while (!is_ws(buffer[i]))
        i++;
    while (is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
    {
        join_pair.first.push_back("");  // not enough args ERR;
        return join_pair;
    }

    int pos = buffer.size();
    while(is_end(buffer, &pos))
        pos--;
    while (is_ws(buffer[pos]))
        pos--;
    std::string args = buffer.substr(i, pos - i + 1);  
    i = 0;
    pos = i;
    while (!is_ws(args[i]))
        i++;
    std::string chanels = args.substr(pos, i - pos);  
    while(is_ws(args[i]))
        i++;
    int no_pss = 0;
    if (is_end(args, &i))
        no_pss = 1;
    pos = i;
    std::string mps;
    if (!no_pss)
    {
        while (args[i] && !is_ws(args[i]) && !is_end(args, &i))
            i++;
        mps = args.substr(pos, i - pos);
    }
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

    if (!no_pss)
    {
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
    }
    else
        pass.push_back("");
    join_pair.first = chans;
    join_pair.second = pass;
    return join_pair;
}

std::vector<std::pair<std::string, std::string> > kick_inv(std::string buffer, int ki){

    std::vector<std::pair<std::string, std::string> > ki_generic;
    int i = 0;
    while (is_ws(buffer[i]))
        i++;
    while (!is_ws(buffer[i]))
        i++;
    while (is_ws(buffer[i]))
        i++;

    if (!isprint(buffer[i]))
        buffer[i] = '\0';
    

    if (!buffer[i] || buffer[i] == '\n' || is_end(buffer, &i) || buffer[i] == ':')
    {
        ki_generic.push_back(std::make_pair("", "X"));  // not enough args ERR;
        return ki_generic;
    }
    int i1 = i;
    while (!is_ws(buffer[i1]))
        i1++;
    while (is_ws(buffer[i1]))
        i1++;
    if (!isprint(buffer[i1]))
        buffer[i1] = '\0';
    if (!buffer[i1] || buffer[i1] == '\n' || is_end(buffer, &i1) || buffer[i1] == ':')
    {
        ki_generic.push_back(std::make_pair("", "X"));  // not enough args ERR;
        return ki_generic;
    }

    int pos = buffer.size();
    while(is_end(buffer, &pos))
        pos--;
    while (is_ws(buffer[pos]))
        pos--;

    std::string args = buffer.substr(i, pos - i + 1);  
    int multi_chn = 0;
    i = 0;
    pos = i;
    while (!is_ws(args[i]))
    {
        if (args[i] == ',')
        {
            multi_chn = 1;
            break;
        }
        i++;
    }
                                                            // chans for flag 0 -> 
    std::string first = args.substr(pos, i - pos);      // users for flag 1 -> invite     
    
    if (multi_chn)
    {
        while (!is_ws(args[i]))
            i++;
    }

    while(is_ws(args[i]))
        i++;

    int no_sec = 0;
    if (is_end(args, &i) || args[i] == ':')
        no_sec = 1;

    pos = i;
    std::string second;
    if (!no_sec)
    {
        while (args[i] && !is_ws(args[i]) && !is_end(args, &i))
            i++;
        second = args.substr(pos, i - pos);
    }
    
    std::string com;
    if (ki == 0 && !no_sec)
    {
        while(is_ws(args[i]))
            i++;
        if (args[i] == ':' && !args[i + 1])
            com = "For no apparent reason...";
        else {
            pos = i;
            while (args[i] && !is_end(args, &i))
                i++;
            com = args.substr(pos, (i + 1) - pos);
        }

    }
    
    std::vector<std::string> sec;

    size_t cpos;
    if (ki == 0 && !no_sec)
    {
        i = 0;
        while (!is_end(second, &i))
        {
            cpos = second.find(',', i);
            if (cpos == std::string::npos)
            {
                sec.push_back(second.substr(i, second.size() - i));
                break;
            }

            sec.push_back(second.substr(i, cpos - i));
            i = cpos + 1;
        }
    }
    if (ki == 0)
    {   
        std::vector<std::pair<std::string, std::string> > chan_usr;
        if (no_sec)
        {
            chan_usr.push_back(std::make_pair("", "X"));  // no users to kick, NOT ENOUGH ARGS
            return chan_usr;
        }
        
        chan_usr.push_back(std::make_pair(com, ""));
        for (std::vector<std::string>::iterator it = sec.begin(); it != sec.end(); it++)
        {
            chan_usr.push_back(std::make_pair(first, *it));
        }

        return (chan_usr);
    }
    else if (ki == 1)
    {

        size_t multi_usrs = first.find(',');
        if (multi_usrs != std::string::npos)
            first = first.substr(0, multi_usrs);

        std::vector<std::pair<std::string, std::string> > usr_chan;
        if (no_sec)
        {
            usr_chan.push_back(std::make_pair("", ""));  // no users for invite, NOT ENOUGH ARGS
            return usr_chan;
        }

        usr_chan.push_back(std::make_pair(first , second));
        return usr_chan;
    }

    return ki_generic;
}
