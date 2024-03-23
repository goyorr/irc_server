#if !SERVER_HPP
#define SERVER_HPP

class server_c {
    private:
                std::string _password;
                uint32_t    _port;
                uint32_t    _server_socket;
    public:
                server_c();
                ~server_c();

                std::map<uint32_t,    client_c>     clients_map;
                std::map<std::string, channels_c>   channels_map;

                void                setPort(const std::string &tmp);
                void                setPassword(const std::string &tmp);
                void                setServer_socket(const uint32_t &tmp);

                const std::string   getPassword() const;
                uint32_t            getPort() const;
                uint32_t            getServer_socket() const;

                void                pars_port(const std::string &port);

                void                init_server(const std::string &tmp_port, const std::string &tmp_password);

                void                pars_cmd(const std::string &buffer, const uint32_t &client_socket);

                void                reg_pass(const std::string &buffer, const uint32_t &client_socket);
                void                reg_user(const std::string &buffer, const uint32_t &client_socket);
                void                reg_nick(const std::string &buffer, const uint32_t &client_socket);

                void                priv_msg(const std::string &buffer, const uint32_t &client_socket);
                void                join(const std::string &buffer, const uint32_t &client_socket);

                static void         signalHandler(int signal);

                //oeprator commands.
                void                mode_cmd(const std::string &buffer, const uint32_t &client_socket);
                void                mode_i(std::string channel_name, uint32_t client_socket, bool set);
                void                mode_t(std::string channel_name, uint32_t client_socket, bool set);
                void                mode_k(std::string channel_name, uint32_t client_socket, std::string password, bool set);
                void                mode_o(std::string channel_name, uint32_t client_socket, std::string target, bool set);
                void                mode_l(std::string channel_name, uint32_t client_socket, bool set, std::string limit);

                void                kick_cmd(const std::string &buffer, const uint32_t &client_socket);
                void                invite_cmd(const std::string &buffer, const uint32_t &client_socket);
                void                topic_cmd(const std::string &buffer, const uint32_t &client_socket);

                template<typename container>
                bool    search_user(container location, uint32_t target, char type, std::string key) {
                    if (type == 'm') {
                        if (std::find(location[key]._members.begin(), location[key]._members.end(), target) == location[key]._members.end())
                            return false;
                        return true;
                    }
                    else if (type == 'o') {
                    if (std::find(location[key]._operators.begin(), location[key]._operators.end(), target) == location[key]._operators.end())
                        return false;
                    }
                    else if (type == 'i') {
                        if (std::find(location[key]._invited.begin(), location[key]._invited.end(), target) == location[key]._invited.end())
                            return false;
                    }
                    return true;
                }
};

#endif
