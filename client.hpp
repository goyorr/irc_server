#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class client_c {
    private:
            std::string                 _client_nick;
            std::string                 _client_user;
            std::string                 _client_real_name;
            std::vector<std::string>    _client_channels;
            std::string                 _client_id;
            int                         _client_socket;
            bool                        _connected;
    public:
            client_c();
            ~client_c();
            client_c(std::string client_nick, std::string client_real_name,
                    std::string client_user, std::string client_id, std::vector<std::string> client_channels);

            std::string                 getClient_nick() const;
            std::string                 getClient_user() const;
            std::string                 getClient_real_name() const;
            std::vector<std::string>    getClient_channels() const;
            std::string                 getClient_id() const;
            int                         getClient_socket() const;
            bool                        getConnected() const;

            void                        setClient_nick(std::string tmp);
            void                        setClient_user(std::string tmp);
            void                        setClient_real_name(std::string tmp);
            void                        setClient_channels(std::vector<std::string> tmp);
            void                        setClient_id(std::string tmp);
            void                        setClient_socket(int tmp);
            void                        setConnected(bool con);

};

#endif
