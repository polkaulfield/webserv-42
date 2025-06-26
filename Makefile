CXX := c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

NAME  = webserv
SRC_DIR := src
INCLUDES_DIR := include
INCLUDES := $(INCLUDES_DIR)/utils.hpp \
$(INCLUDES_DIR)/webserv.hpp \
$(INCLUDES_DIR)/clientRequest.hpp \
$(INCLUDES_DIR)/serverResponse.hpp \
$(INCLUDES_DIR)/server.hpp
SRCS := $(SRC_DIR)/webserv.cpp \
$(SRC_DIR)/utils.cpp \
$(SRC_DIR)/clientRequest.cpp \
$(SRC_DIR)/serverResponse.cpp \
$(SRC_DIR)/server.cpp
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
