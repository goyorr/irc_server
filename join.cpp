#include "headers/includes.h"

 std::vector<std::pair<std::string, std::string> > joiner(std::string buffer){

    std::cout << "tst: " << buffer << std::endl;
    
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

    size_t pos = buffer.size();
    pos--;
    while(is_end(buffer, &i))
        pos--;
    while (is_ws(buffer[pos]))
        pos--;
    std::string args = buffer.substr(i, pos - i);    //#chan,#chan1 pass,pass1
    std::cout << "zx:|" << args << "|" << std::endl;
    i = 0;
    std::cout << "at i: " << (int)args[i] << std::endl;
    pos = i;
    while (!is_ws(args[i]))
        i++;
    std::string chanels = args.substr(pos, i - pos);
    std::cout << "chnls : |" << chanels << "|" << std::endl;

    while(is_ws(args[i]))
        i++;
    pos = i;
    while (args[i] && !is_ws(args[i] && !is_end(args, &i)))
        i++;
    std::cout << "last: " << args[i] << std::endl;
    std::string mps = args.substr(pos, (i + 1) - pos );

    std::cout << "mps : |" << mps << "|" << std::endl;

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

std::vector<std::string>::iterator it_pass = pass.begin();
for (std::vector<std::string>::iterator it_chans = chans.begin(); it_chans != chans.end(); ++it_chans)
{
        while(it_pass != pass.end())
        {
            chan_pass.push_back(std::make_pair(*it_chans, *it_pass));
            it_pass++;
        }
        if (it_pass == pass.end())
            chan_pass.push_back(std::make_pair(*it_chans, ""));
}

for (std::vector<std::pair<std::string, std::string> >::iterator it = chan_pass.begin(); it != chan_pass.end(); it++)
{

    std::cout << "First: |" << it->first << "seconde: |" << it->second << "|" << std::endl;
}
    return chan_pass;
}
