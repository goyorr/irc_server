#include "headers/includes.h"

void server_c::signalHandler(int signal) {
    std::cout << "\nCleaning and exiting." << std::endl;

    for(size_t i = 0; i < client_c::_disc.size(); i++) {
        std::cout << client_c::_disc[i].fd << std::endl;
        close(client_c::_disc[i].fd);
    }
    std::exit(signal);
}
