#include "server.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
        return std::cerr << "usage: ircserv [port] [password]" << std::endl, 1;

    server_c    srv;

    srv.pars_port(av[1]);
    srv.init_server(av[1], av[2]);
}
