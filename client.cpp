#include "client.hpp"

client_c::client_c() {
        _client_nick = "_Default_";
        _client_user = "_Default_";
        _client_real_name = "_Default_";
        _client_id = "_Default_";
        _connected = false;
}

client_c::~client_c() {
}

client_c::client_c(std::string client_nick, std::string client_real_name,
                    std::string client_user, std::string client_id, std::vector<std::string> client_channels) {
        _client_nick = client_nick;
        _client_real_name = client_real_name;
        _client_user = client_user;
        _client_id = client_id;
        setClient_channels(client_channels);
        _connected = false;
}

std::string     client_c::getClient_nick() const {
        return _client_nick;
}

std::string     client_c::getClient_user() const {
        return _client_user;
}

std::string     client_c::getClient_real_name() const {
        return _client_real_name;
}

std::vector<std::string>        client_c::getClient_channels() const {
        return _client_channels;
}

std::string     client_c::getClient_id() const {
        return _client_id;
}

int     client_c::getClient_socket() const {
        return _client_socket;
}

bool     client_c::getConnected() const {
        return _connected;
}


void    client_c::setClient_nick(std::string tmp) {
        _client_nick = tmp;
}

void    client_c::setClient_user(std::string tmp) {
        _client_user = tmp;
}

void    client_c::setClient_real_name(std::string tmp) {
        _client_real_name = tmp;
}

void    client_c::setClient_channels(std::vector<std::string> tmp) {
        _client_channels = tmp;
}

void    client_c::setClient_id(std::string tmp) {
        _client_id = tmp;
}

void    client_c::setClient_socket(int tmp) {
        _client_socket = tmp;
}

void    client_c::setConnected(bool con) {
        _connected = con;
}
