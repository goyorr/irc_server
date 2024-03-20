#ifndef CLIENT_HPP
#define CLIENT_HPP

class client_c {
    private:
            std::string                 _client_nick;
            std::string                 _client_user;
            std::string                 _client_real_name;
            int                         _client_socket;
            bool                        _registered;
            bool                        _authed;
            bool                        _nickReg;
            bool                        _userReg;
    public:
            client_c();
            ~client_c();

            static std::vector<pollfd> _disc;
            static std::vector<char [1024]> _buffer;

            std::string                 getClient_nick() const;
            std::string                 getClient_user() const;
            std::string                 getClient_real_name() const;
            int                         getClient_socket() const;
            bool                        getRegistered() const;
            bool                        getAuth() const;
            bool                        getRegNick() const;
            bool                        getRegUser() const;

            void                        setClient_nick(std::string tmp);
            void                        setClient_user(std::string tmp);
            void                        setClient_real_name(std::string tmp);
            void                        setClient_socket(int tmp);
            void                        setRegistered(bool con);
            void                        setAuth(bool tmp);
            void                        setRegUser(bool tmp);
            void                        setRegNick(bool tmp);
};

#endif
