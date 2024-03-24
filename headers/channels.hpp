#if !CHANNELS_HPP
#define CHANNELS_HPP

class channels_c {
    private:
            std::string _channel_name;
            std::string _channel_password;
            std::string _channel_topic;
            uint32_t    _user_limit;
            uint32_t    _user_count;
            bool        _isprotected;
            bool        _isinvite_only;
            bool        _isrestricted_topic;
            bool        _isuser_limit;
    public:
            channels_c();
            ~channels_c();

            std::vector<uint32_t>       _operators;
            std::vector<uint32_t>       _members;
            std::set<uint32_t>       _invited;

            std::string getName();
            std::string getChannelPassword();
            std::string getTopic();
            uint32_t    getuser_limit();
            uint32_t    getuser_count();
            bool        getisProtected();
            bool        getisinvite_only();
            bool        getisrestricted_topic();
            bool        getisuser_limit();

            void        setName(const std::string &tmp);
            void        setChannelPassword(const std::string &tmp);
            void        setTopic(std::string tmp);
            void        setuser_limit(uint32_t tmp);
            void        setisProtected(const bool &tmp);
            void        setisinvite_only(bool tmp);
            void        setisrestricted_topic(bool tmp);
            void        setisuser_limit(bool tmp);

            void        incrementUser_count();
};

#endif
