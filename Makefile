CXX := c++
CXXFLAGS = -g -Wall -Werror -Wextra -std=c++98

NAME  = webserv
SRC_DIR := src
INCLUDES_DIR := include
INCLUDES := $(INCLUDES_DIR)/utils.hpp \
$(INCLUDES_DIR)/webserv.hpp \
$(INCLUDES_DIR)/clientRequest.hpp \
$(INCLUDES_DIR)/cgi.hpp \
$(INCLUDES_DIR)/uploadedFiles.hpp \
$(INCLUDES_DIR)/serverResponse.hpp \
$(INCLUDES_DIR)/server.hpp \
$(INCLUDES_DIR)/location.hpp \
$(INCLUDES_DIR)/config.hpp \
$(INCLUDES_DIR)/pollManager.hpp
SRCS := $(SRC_DIR)/webserv.cpp \
$(SRC_DIR)/utils.cpp \
$(SRC_DIR)/deleteMethod.cpp \
$(SRC_DIR)/uploadedFilesCR.cpp \
$(SRC_DIR)/uploadedFilesSR.cpp \
$(SRC_DIR)/clientRequest.cpp \
$(SRC_DIR)/cgi.cpp \
$(SRC_DIR)/cgiUtils.cpp \
$(SRC_DIR)/serverResponse.cpp \
$(SRC_DIR)/location.cpp \
$(SRC_DIR)/config.cpp \
$(SRC_DIR)/configUtils.cpp \
$(SRC_DIR)/server.cpp \
$(SRC_DIR)/pollManager.cpp
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
