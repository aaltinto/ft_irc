NAME = ircserv
BOT = ircbot

CC = c++

# Hata ayıklama için -g bayrağını ekleyin
# Kullanılmayan parametre uyarılarını bastırmak için -Wno-unused-parameter ekleyin
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g -Wno-unused-parameter

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

BOT_SRC = bot/main.cpp \
		  bot/bot.cpp \

OBJ = $(SRC:.cpp=.o)
BOT_OBJ = $(BOT_SRC:.cpp=.o)

RM = rm -f

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

$(BOT): $(BOT_OBJ)
	$(CC) $(CPPFLAGS) $(BOT_OBJ) -o $(BOT)

bot: $(BOT)

clean:
	$(RM) $(OBJ) $(BOT_OBJ)
	
fclean: clean
	$(RM) $(NAME) $(BOT)

re: fclean $(NAME)
	$(RM) $(OBJ)

rebot: fclean $(BOT)
	$(RM) $(BOT_OBJ)

.PHONY: all clean fclean re bot rebot