
#include "headers/includes.h"

std::vector<std::pair<std::string, std::string> > join_kick_inv(std::string buffer, int jk){

    std::vector<std::pair<std::string, std::string> > jki_generic;
    int i = 0;
    while (is_ws(buffer[i]))
        i++;
    while (!is_ws(buffer[i]))
        i++;
    while (is_ws(buffer[i]))
        i++;
    if (is_end(buffer, &i))
    {
        jki_generic.push_back(std::make_pair("", ""));  // not enough args ERR;
        return jki_generic;
    }

    int pos = buffer.size();
    while(is_end(buffer, &pos))
        pos--;
    while (is_ws(buffer[pos]))
        pos--;
    

    std::string args = buffer.substr(i, pos - i + 1);    //#chan,#chan1 pass,pass1

    i = 0;
    pos = i;
    while (!is_ws(args[i]))
        i++;
    
    std::string chanels = args.substr(pos, i - pos);      // users for flag 3 -> invite
    

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
    std::string com;
    if (jk == 1)
    {
        if (!no_pss)
        {
            while(is_ws(args[i]))
                i++;
            if (is_end(args, &i))
                com = "For no apparent reason...";
            else {
                pos = i;
                while (args[i] && !is_end(args, &i))
                    i++;
                com = args.substr(pos, (i + 1) - pos);
            }
        }
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

    if (jk == 0)
    {
        // JOIN
        std::vector<std::pair<std::string, std::string> > chan_pass;
        std::vector<std::string>::iterator it_pass = pass.begin();
        for (std::vector<std::string>::iterator it_chans = chans.begin(); it_chans != chans.end(); ++it_chans)
        {
            if (!no_pss)
            {
                while(it_pass != pass.end())
                {
                    chan_pass.push_back(std::make_pair(*it_chans, *it_pass));
                    it_pass++;
                    it_chans++;
                }
            }
            if ((it_pass == pass.end() || pass.empty()) && it_chans != chans.end())
                chan_pass.push_back(std::make_pair(*it_chans, ""));
            else
                break;
        }
        return chan_pass;
    }
    else if (jk == 1)
    {   

        std::vector<std::pair<std::string, std::string> > chan_usr;
        if (no_pss)
        {
            chan_usr.push_back(std::make_pair(chanels, ""));
            return chan_usr;
        }

        chan_usr.push_back(std::make_pair(chanels, mps));
        chan_usr.push_back(std::make_pair(com, ""));

        return (chan_usr);
    }
    else if (jk == 3)
    {
        std::vector<std::pair<std::string, std::string> > usr_chan;
        if (no_pss)
        {
            usr_chan.push_back(std::make_pair("", ""));  // no users for invite, NOT ENOUGH ARGS
            return usr_chan;
        }
        std::vector<std::string> invited = chans;
        std::vector<std::string> chanels = pass;
        for (std::vector<std::string>::iterator it = chanels.begin(); it != chanels.end(); it++)
        {
            for(std::vector<std::string>::iterator us = invited.begin(); us != invited.end(); us++)
            {
                usr_chan.push_back(std::make_pair(*us, *it));
            }
        }
        return usr_chan;
    }

    return jki_generic;
}
