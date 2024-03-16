NAME = ircserv

SRC = ${shell ls | grep ".cpp"}

OBJ = ${SRC:.cpp=.o}

DEPS = ${SRC:.cpp=.d}

C++ = c++

RM = rm -f

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP

all: ${NAME}

${NAME}: ${OBJ}
	${C++} ${OBJ} -o $@

-include ${DEPS}

%.o: %.cpp Makefile
	${C++} ${CPPFLAGS} -c $<

clean:
	${RM} ${OBJ}

fclean: clean
	${RM} ${NAME} ${DEPS}

re: fclean all