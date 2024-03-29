#include "headers/includes.h"

void server_c::signalHandler(int signal) {
    std::cout << "\nCleaning and exiting." << std::endl;

    for(size_t i = 0; i < client_c::_disc.size(); i++) {
        std::cout << "Closing socket #" << client_c::_disc[i].fd << "..." << std::endl;
        close(client_c::_disc[i].fd);
    }
    std::exit(signal);
}

void server_c::channel_checker(std::string channel_name)
{
    if (channels_map[channel_name]._members.size() == 0)
        channels_map.erase(channel_name);
}

void server_c::assign_operator(const std::string channel_name) {
    
    channels_map[channel_name]._operators.push_back(channels_map[channel_name]._members[0]);
}
