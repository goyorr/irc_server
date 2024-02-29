#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

class client_c {
    private:
            std::string                 _client_nick;
            std::string                 _client_user;
            std::string                 _client_real_name;
            int                         _client_socket;
            bool                        _registered;
    public:
            client_c();
            ~client_c();

            static std::vector<pollfd> _disc;

            std::string                 getClient_nick() const;
            std::string                 getClient_user() const;
            std::string                 getClient_real_name() const;
            int                         getClient_socket() const;
            bool                        getRegistered() const;

            void                        setClient_nick(std::string tmp);
            void                        setClient_user(std::string tmp);
            void                        setClient_real_name(std::string tmp);
            void                        setClient_socket(int tmp);
            void                        setRegistered(bool con);
};

#endif
