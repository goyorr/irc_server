NAME = ircserv

DEPS = ${NAME}.d

C++ = c++

RM = rm -f

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP

all: ${NAME}

${NAME}: Makefile
	${C++} ${CPPFLAGS} ${shell ls | grep ".cpp"} -o $@

-include ${DEPS}

clean:
	${RM} ${DEPS}

fclean: clean
	${RM} ${NAME}

re: fclean all