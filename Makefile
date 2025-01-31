NAME = ircserv

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = src/main.cpp \
		src/server_build.cpp \
		src/server.cpp \
		src/client.cpp \
		channel/channel.cpp \
		channel/channel_operations.cpp \
		communication/send_message.cpp \

OBJ = $(SRC:.cpp=.o)

RM = rm -f

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re