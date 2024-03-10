#include "./headers/includes.h"

std::vector<pollfd> client_c::_disc;

client_c::client_c() {
        _client_nick = "_Default_";
        _client_user = "_Default_";
        _client_real_name = "_Default_";
        _client_socket = -1;
        _registered = false;
        _authed = false;
}

client_c::~client_c() {
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

int     client_c::getClient_socket() const {
        return _client_socket;
}

bool     client_c::getRegistered() const {
        return _registered;
}
bool    client_c::getAuth() const {
        return _authed;
}

bool    client_c::getRegUser() const {
        return _userReg;
}

bool    client_c::getRegNick() const {
        return _nickReg;
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

void    client_c::setClient_socket(int tmp) {
        _client_socket = tmp;
}

void    client_c::setRegistered(bool con) {
        _registered = con;
}

void    client_c::setAuth(bool tmp) {
        _authed = tmp;
}

void    client_c::setRegUser(bool tmp) {
        _userReg = tmp;
}

void    client_c::setRegNick(bool tmp) {
        _nickReg = tmp;
}
