OS = $(shell uname -s)
CWD = $(shell pwd)
ifeq ($(MAKE_TRACE), 1)
$(info OS: $(OS))
$(info CWD: $(CWD))
endif
include $(CWD)/mk/sources.mk

NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude -g
BUILD_DIR = obj/
OBJS = $(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME)

$(BUILD_DIR)%.o : %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)


valgrind: $(NAME)
ifeq ($(OS), Linux)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME)
else
	@printf "OS: $(OS) is not supported by valgrind\n"
endif

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all re fclean clean valgrind
