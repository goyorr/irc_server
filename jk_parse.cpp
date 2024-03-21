#include "headers/includes.h"

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
    {
        chan_pass.push_back(std::make_pair("/'", "\'"));  // not enough args ERR;
        return chan_pass;
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
    
    std::string chanels = args.substr(pos, i - pos);
    

    while(is_ws(args[i]))
        i++;

    int no_pss = 0;
    if (is_end(args, &i))
    {
        no_pss = 1;
        //  chan_pass.push_back(std::make_pair(chanels, ""));
        //  return chan_pass;
    }

    pos = i;
    std::string mps;
    if (!no_pss)
    {
        while (args[i] && !is_ws(args[i] && !is_end(args, &i)))
            i++;
        mps = args.substr(pos, (i + 1) - pos );
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
    puts("BRUHH");

if (jk == 0)
{
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
}
else if (jk == 1)
{   

        puts("1");
    if (no_pss)
    {
        puts("2"); 
        chan_pass.push_back(std::make_pair(chanels, ""));
        return chan_pass;
    }
    puts ("tf");
    std::vector<std::pair<std::string, std::string> > chan_usr;
    std::vector<std::string> users = pass;
    std::vector<std::string>::iterator it_users = users.begin();
    std::vector<std::string>::iterator max = it_users;
    for (std::vector<std::string>::iterator it_chans = chans.begin(); it_chans != chans.end(); ++it_chans)
    {
        puts("3");
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

    for (std::vector<std::pair<std::string, std::string> >::iterator it = chan_usr.begin(); it != chan_usr.end(); it++)
    {

        std::cout << "First:|" << it->first << "|     seconde:|" << it->second << "|" << std::endl;
    }

        return (chan_usr);
}

// for (std::vector<std::pair<std::string, std::string> >::iterator it = chan_pass.begin(); it != chan_pass.end(); it++)
// {

//     std::cout << "First:|" << it->first << "|     seconde:|" << it->second << "|" << std::endl;
// }
    return chan_pass;
}