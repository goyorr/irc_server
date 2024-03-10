#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

void joiner(std::string args){

    // std::vector <std::string> chanels;
    // std::vector <std::string> passwords;
    // std::pair<int, std::string> sorted_chanels;
    std::pair<std::string , std::string> chanelsWpass;

        int space = args.find(32);
        std::string chanels = args.substr(0, space);
        std::string passwords = args.substr(space + 1, strlen(args.c_str()));
        // chanels.push_back(chanels);
        // passwords.push_back(pass);
    
    int tmp = NULL;
    int tmp1 = NULL;
    for (int x = 0; x < strlen(args.c_str() - 2); x++)
    {
        for (int i = 0; chanels[i] != ',' ; i++)
        {
            tmp = i;
            while (chanels[i] != ',' || chanels[i])
                i++;
        }
        for (int j = 0; j < strlen(passwords.c_str()) - 1; j++)
        {
            tmp1 = j;
            while (passwords[j] != ',' || passwords[j])
                j++;

        }       
            chanelsWpass.first = chanels.substr(tmp, i);
            chanelsWpass.second = passwords.substr(tmp1, j);
            chanelsWpass
    }
    
    }
    static int chanel_number = 0;
    for (std::vector<std::string>::iterator it = chanels.begin() ; it < chanels.end() ; it++)
    {
        std::vector<std::string>::iterator init = it;
        size_t virgule = it->find(',');
        sorted_chanels.first = ++chanel_number;
        if (virgule != std::string::npos)
        {
            std::vector<std::string>::iterator tmp = it++;
            if (tmp->find(',') != std::string::npos)
            {
                for (it; it < tmp; it++)
                    sorted_chanels.second.push_back(*it);
            //    sorted_chanels.second = chanels.substr(it, tmp - it);
            }
        }
    }
}
