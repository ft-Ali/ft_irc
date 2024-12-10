NAME        =   ircserv

CC          =   c++

FLAG        =   -Wall -Wextra -Werror -std=c++98 -g3


C_FILE		 =	main.cpp srcs/Server.cpp srcs/Client.cpp srcs/Channel.cpp

OBJS        =   $(C_FILE:.cpp=.o)
DEPS		=	$(OBJS:.o=.d)

all: $(NAME)

-include $(DEPS)

.cpp.o:
	@printf "\r\033[K[IRC] \033[0;32mBuilding : $<\033[0m"
	@$(CC) $(FLAG) -c $< -o $@ -MMD -MP


$(NAME): $(OBJS)
	@printf "\r\033[K[IRC] \033[0;32mLinking...\033[0m"
	@$(CC) $(OBJS) -o $(NAME)
	@printf "\r\033[K[IRC] \033[0;32mDone!\033[0m\n"

clean:
	@rm -f $(OBJS) $(DEPS)
	@printf "[IRC] \033[1;31mCleaned .o!\033[0m\n"

fclean: clean
	@rm -f $(NAME) 
	@printf "[IRC] \033[1;31mCleaned all!\033[0m\n"

re: fclean all

.PHONY: all clean fclean re