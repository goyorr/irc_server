#if !CHANNELS_HPP
#define CHANNELS_HPP

class channels_c {
    private:
                std::string _channel_name;
                std::string _channel_password;
                bool        _channel_protected;

                //modes
    public:
                channels_c();
                ~channels_c();

                std::vector<uint16_t>       _operators;
                std::vector<uint16_t>    _members;

                std::string getName();
                std::string getChannelPassword();
                bool        getProtected();

                void    setName(const std::string &tmp);
                void    setChannelPassword(const std::string &tmp);
                void    setProtected(const bool &tmp);

};

#endif
