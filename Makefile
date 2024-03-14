NAME = ircserv

SRC = main.cpp server.cpp client.cpp parse.cpp signal.cpp server_cmd.cpp register.cpp channels.cpp jk_parse.cpp

HEADER = headers/server.hpp headers/client.hpp headers/includes.h headers/channels.hpp

OBJ = ${SRC:.cpp=.o}

C++ = c++

RM = rm -f

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

all: ${NAME}

${NAME}: ${OBJ}
		 ${C++} ${OBJ} -o $@
 
%.o: %.cpp ${HEADER} Makefile
	 ${C++} ${CPPFLAGS} -c $<

clean:
		${RM} ${OBJ}

fclean: clean
		sleep 1 && ${RM} ${NAME}

re: fclean all