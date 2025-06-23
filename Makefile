CXX := c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

NAME  = webserv
SRC_DIR := .
INCLUDES_DIR := .
INCLUDES := $(INCLUDES_DIR)/utils.hpp \
$(INCLUDES_DIR)/webserv.hpp \
$(INCLUDES_DIR)/clientRequest.hpp
SRCS := $(SRC_DIR)/server.cpp \
$(SRC_DIR)/utils.cpp \
$(SRC_DIR)/clientRequest.cpp
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)
LDFLAGS :=

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp $(INCLUDES) Makefile
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all
