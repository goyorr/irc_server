#if !SERVER_HPP
#define SERVER_HPP

class server_c {
    private:
                std::string _password;
                uint16_t    _port;
                uint16_t    _server_socket;
    public:
                server_c();
                ~server_c();

                std::map<uint16_t,    client_c>     clients_map;
                std::map<std::string, channels_c>   channels_map;

                void                setPort(const std::string &tmp);
                void                setPassword(const std::string &tmp);
                void                setServer_socket(const uint16_t &tmp);

                const std::string   getPassword() const;
                uint16_t            getPort() const;
                uint16_t            getServer_socket() const;

                void                pars_port(const std::string &port);

                void                init_server(const std::string &tmp_port, const std::string &tmp_password);

                void                pars_cmd(const std::string &buffer, const uint16_t &client_socket);

                void                reg_pass(const std::string &buffer, const uint16_t &client_socket);
                void                reg_user(const std::string &buffer, const uint16_t &client_socket);
                void                reg_nick(const std::string &buffer, const uint16_t &client_socket);

                void                priv_msg(const std::string &buffer, const uint16_t &client_socket);
                void                join(const std::string &buffer, const uint16_t &client_socket);

                static void         signalHandler(int signal);
};

#endif
