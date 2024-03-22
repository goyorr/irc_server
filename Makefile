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





# NAME = ircserv

# SRC = ${shell ls | grep ".cpp"}

# OPP_DIR = opp
# DEPENDENCIES_DIR = dependencies

# OBJ = ${SRC:%.cpp=$(OPP_DIR)/%.o}
# DEPS = ${SRC:%.cpp=$(DEPENDENCIES_DIR)/%.d}

# C++ = c++
# RM = rm -f

# CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP
# all: ${NAME}

# ${NAME}: ${OBJ}
# 	${C++} ${OBJ} -o $@

# -include ${DEPS}

# $(OPP_DIR)/%.o: %.cpp Makefile | $(OPP_DIR)
# 	${C++} ${CPPFLAGS} -c $< -o $@

# $(DEPENDENCIES_DIR)/%.d: %.cpp Makefile | $(DEPENDENCIES_DIR)
# 	${C++} ${CPPFLAGS} -MT $(@:%.d=$(OPP_DIR)/%.o) -MM $< > $@

# $(OPP_DIR) $(DEPENDENCIES_DIR):
# 	mkdir -p $@

# clean:
# 	${RM} ${OBJ}

# fclean: clean
# 	${RM} ${NAME}

# re: fclean all
