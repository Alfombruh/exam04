NAME = microshell
CC = gcc
CFLAGS = -Werror -Wextra -Wall -fsanitize=address -g3
RM = rm -rf
FILES = srcs/microshell
SRCS = $(addsuffix .c, $(FILES))
OBJS = $(addsuffix .o, $(FILES))
S_FILES = srcs/microshell_shorter
S_SRCS = $(addsuffix .c, $(S_FILES))
S_OBJS = $(addsuffix .o, $(S_FILES))


.c.o: $(SRCS)
	$(CC) $(CFLAGS) -c $^ -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(NAME)

s_objs: $(S_SRCS)
	$(CC) $(CFLAGS) -c $^ -o $(S_OBJS)

shorter: s_objs
	$(CC) $(CFLAGS) $(S_OBJS) -o $(addsuffix _shorter,$(NAME))

clean:
	$(RM) *.dSYM
	$(RM) $(OBJS)
	$(RM) $(S_OBJS)

fclean: clean
	$(RM) $(NAME)
	$(RM) $(addsuffix _shorter,$(NAME))

re: clean all

.PHONY: all clean fclean re
