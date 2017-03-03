SNAME=nsminishell
CC=gcc
CFLAGS=-Wall -Werror --pedantic -I../../include 
LFLAGS=-L../../lib -llist -lmy -lcurses 
SRC=nsminishell.c
OBJS= $(SRC:.c=.o)

all: $(OBJS)
	$(CC) $(OBJS) -o $(SNAME) $(CFLAGS) $(LFLAGS)
clean:
	rm -f $(OBJS)
	rm -f *.c~
fclean: clean
	rm -f $(SNAME)
re: fclean all
