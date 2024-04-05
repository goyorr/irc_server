NAME = ircserv

SRC = ${shell ls | grep ".cpp"}

HEADER = headers/channels.hpp headers/client.hpp headers/server.hpp headers/includes.h

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
	${RM} ${NAME}

re: fclean all