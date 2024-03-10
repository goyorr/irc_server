#ifndef INCLUDES_H
#define INCLUDES_H

#include <poll.h>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <signal.h>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "channels.hpp"
#include "client.hpp"
#include "server.hpp"

std::pair<int, std::string>                         regi_parse(std::string str, int flag);
std::pair<std::vector<std::string>, std::string >   prvmsg_parse(std::string buffer);
int                                                 is_ws(int c);
int                                                 is_end(std::string str, int *i);
std::string                                         select_cmd(std::string buffer);
std::vector<std::pair<std::string, std::string> >   joiner(std::string buffer);

#endif
