#include "./headers/includes.h"

channels_c::channels_c() {
}

channels_c::~channels_c() {
}

std::string channels_c::getName() {
    return _channel_name;
}

std::string channels_c::getChannelPassword() {
    return _channel_password;
}

bool    channels_c::getisProtected() {
    return _isprotected;
}

uint32_t    channels_c::getuser_limit() {
    return _user_limit;
}

bool    channels_c::getisinvite_only() {
    return _isinvite_only;
}

bool    channels_c::getisrestricted_topic() {
    return _isrestricted_topic;
}

bool    channels_c::getisuser_limit() {
    return _isuser_limit;
}

std::string channels_c::getTopic() {
    return _channel_topic;
}

uint32_t    channels_c::getuser_count() {
    return _user_count;
}

void    channels_c::setName(const std::string &tmp) {
    _channel_name = tmp;
}

void    channels_c::setChannelPassword(const std::string &tmp) {
    _channel_password = tmp;
}

void    channels_c::setisProtected(const bool &tmp) {
    _isprotected = tmp;
}

void    channels_c::setuser_limit(uint32_t tmp) {
    _user_limit = tmp;
}

void    channels_c::setisinvite_only(bool tmp) {
    _isinvite_only = tmp;
}

void    channels_c::setisrestricted_topic(bool tmp) {
    _isrestricted_topic = tmp;
}

void    channels_c::setisuser_limit(bool tmp) {
    _isuser_limit = tmp;
}

void    channels_c::setTopic(std::string tmp) {
    _channel_topic = tmp;
}

void        channels_c::incrementUser_count() {
    _user_count++;
}
