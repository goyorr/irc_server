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
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <netdb.h>

void                                                            assign_operator(std::map<std::string, channels_c>& channels_map, const std::string& channel);
std::pair<std::vector<std::string>, std::vector<std::string> >  join_pars(std::string buffer);
std::vector<std::pair<std::string, std::string> >               kick_inv(std::string buffer, int ki);
std::pair<std::string, std::pair<std::string, std::string> >    parse_mode(std::string buffer);
std::pair<std::vector<std::string>, std::string >               prvmsg_parse(std::string buffer);
std::pair<std::string, std:: string>                            topic_parse(std::string buffer);
std::pair<int, std::string>                                     regi_parse(std::string str, int flag);
std::vector<std::string>                                        sort_subs(std::string subjects);
std::string                                                     select_cmd(std::string buffer);
int                                                             is_ws(int c);
int                                                             is_end(std::string str, int *i);
bool                                                            is_num(std::string str);

#endif
