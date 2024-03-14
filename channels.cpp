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

bool        channels_c::getProtected() {
    return _channel_protected;
}


void    channels_c::setName(const std::string &tmp) {
    _channel_name = tmp;
}

void    channels_c::setChannelPassword(const std::string &tmp) {
    _channel_password = tmp;
}

void    channels_c::setProtected(const bool &tmp) {
    _channel_protected = tmp;
}
