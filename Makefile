NAME = ircserv

SRC = ${shell ls | grep ".cpp"}

DEPS = ${NAME}.d

C++ = c++

RM = rm -f

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP

all: ${NAME}

${NAME}:
	${C++} ${CPPFLAGS} ${SRC} -o $@

-include ${DEPS}

clean:
	${RM} ${DEPS}

fclean: clean
	${RM} ${NAME}

re: fclean all