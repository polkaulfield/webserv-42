CXX := c++
CXXFLAGS = -g -Wall -Werror -Wextra -std=c++98

NAME  = webserv
SRC_DIR := src
INCLUDES_DIR := include
INCLUDES := $(INCLUDES_DIR)/utils.hpp \
$(INCLUDES_DIR)/ClientRequest.hpp \
$(INCLUDES_DIR)/Cgi.hpp \
$(INCLUDES_DIR)/ServerResponse.hpp \
$(INCLUDES_DIR)/Server.hpp \
$(INCLUDES_DIR)/Location.hpp \
$(INCLUDES_DIR)/Config.hpp \
$(INCLUDES_DIR)/PollManager.hpp \
$(INCLUDES_DIR)/Directory.hpp \
$(INCLUDES_DIR)/UploadedFile.hpp
SRCS := $(SRC_DIR)/webserv.cpp \
$(SRC_DIR)/utils.cpp \
$(SRC_DIR)/ClientRequest.cpp \
$(SRC_DIR)/Cgi.cpp \
$(SRC_DIR)/cgiUtils.cpp \
$(SRC_DIR)/ServerResponse.cpp \
$(SRC_DIR)/Directory.cpp \
$(SRC_DIR)/deleteMethod.cpp \
$(SRC_DIR)/Location.cpp \
$(SRC_DIR)/Config.cpp \
$(SRC_DIR)/configUtils.cpp \
$(SRC_DIR)/Server.cpp \
$(SRC_DIR)/uploadedFilesSR.cpp \
$(SRC_DIR)/uploadedFilesCR.cpp \
$(SRC_DIR)/PollManager.cpp
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)
LDFLAGS :=

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS) $(INCLUDES) Makefile
	$(CXX) $(OBJS) $(CXXFLAGS) -o $@

%.o: %.cpp $(INCLUDES) Makefile
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all
