# --- Name and Compiler ---
NAME	= codexion
CC	= cc
CFLAGS	= -Wall -Wextra -Werror -pthread -g -I ./inc
# -fsanitize=thread
# -fsanitize=address

# --- Files ---
HEADERS     = inc/codexion.h
SRCS        = src/main.c \
              src/parse.c \
              src/init.c \
              src/routine.c \
              src/routine_actions.c \
              src/scheduler.c \
              src/monitor.c \
              src/utils.c

# --- Generating .o files ---
OBJS        = $(SRCS:.c=.o)

# --- Remove ---
RM          = rm -f

# --- Colors ---
RESET       = \e[0;39m
GREEN       = \e[1;92m
YELLOW      = \e[0;93m
BLUE        = \e[1;94m

# --- Rules ---

%.o : %.c $(HEADERS) Makefile
	@echo "$(YELLOW)Compiling: $< $(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Codexion compiled successfully! $(RESET)"

clean :
	@$(RM) $(OBJS)
	@echo "$(BLUE)Clean $(RESET)"

fclean : clean
	@$(RM) $(NAME)
	@echo "$(BLUE)Fclean $(RESET)"

re : fclean all
	@echo "$(BLUE)Fclean and rebuilt $(RESET)"

.PHONY: all clean fclean re
