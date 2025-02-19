NAME = ircserv

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = server/main.cpp \
		server/server_build.cpp \
		server/server.cpp \
		server/client.cpp \
		errors/server_errors.cpp \
		channel/mode.cpp \
		channel/channel_mods.cpp \
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
	$(RM) $(OBJ)

.PHONY: all clean fclean re