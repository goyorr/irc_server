NAME = ircserv

SRC = main.cpp server.cpp client.cpp

HEADER = server.hpp client.hpp

OBJ = ${SRC:.cpp=.o}

C++ = c++

RM = rm -f

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

all: ${NAME}

${NAME}: ${OBJ}
		 ${C++} ${OBJ} -o $@
 
%.o: %.cpp ${HEADER}
	 ${C++} ${CPPFLAGS} -c $<

clean:
		${RM} ${OBJ}

fclean: clean
		sleep 1 && ${RM} ${NAME}

re: fclean all