NAME        =   ircserv
NAME_BOT	=   ircbot

CC          =   c++

FLAG        =   -Wall -Wextra -Werror -std=c++98 -g3


C_FILE		 =	main.cpp srcs/Server.cpp srcs/Client.cpp srcs/Channel.cpp srcs/Command/Join.cpp srcs/Command/Part.cpp srcs/Command/Mode.cpp srcs/Command/Kick.cpp srcs/Command/Invit.cpp srcs/Command/Topic.cpp
C_FILE_BOT 	= srcs/Bot/mainBot.cpp srcs/Bot/Bot.cpp srcs/Server.cpp srcs/Client.cpp srcs/Channel.cpp srcs/Command/Join.cpp srcs/Command/Part.cpp srcs/Command/Mode.cpp

OBJS        =   $(C_FILE:.cpp=.o)
OBJS_BOT	=	$(C_FILE_BOT:.cpp=.o)
DEPS		=	$(OBJS:.o=.d)

all: $(NAME)
Bot : $(NAME_BOT)
-include $(DEPS)

.cpp.o:
	@printf "\r\033[K[IRC] \033[0;32mBuilding : $<\033[0m\n"
	@$(CC) $(FLAG) -c $< -o $@ -MMD -MP

$(NAME): $(OBJS)
	@printf "\r\033[K[IRC] \033[0;32mLinking...\033[0m"
	@$(CC) $(OBJS) -o $(NAME)
	@printf "\r\033[K[IRC] \033[0;32mDone!\033[0m\n"

$(NAME_BOT): $(OBJS_BOT)
	@printf "\r\033[K[IRC] \033[0;32mLinking...\033[0m"
	@$(CC) $(OBJS_BOT) -o $(NAME_BOT)
	@printf "\r\033[K[IRC] \033[0;32mDone!\033[0m\n"

clean:
	@rm -f $(OBJS) $(OBJS_BOT) $(DEPS)
	@printf "[IRC] \033[1;31mCleaned .o!\033[0m\n"

fclean: clean
	@rm -f $(NAME) $(NAME_BOT)
	@printf "[IRC] \033[1;31mCleaned all!\033[0m\n"

re: fclean all

.PHONY: all clean fclean re
