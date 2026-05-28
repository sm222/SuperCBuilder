# - # - #
# Makefile Make with scb on Mon May 18 03:59:23 2026
# built by antoine
# project name -> prog
# config file -> defaultConfigFile.scb
# - # - #


CC				=		cc
CXX				=		c++
CFLAGS		=		-Wall -Werror -Wextra
CXXFLAGS	=		-Wall -Werror -Wextra



NAME			=		a
NAMEX			=		.out


F_prog		=		./
F_4_BAR		=		$(F_prog)bar/

O_4_BAR	=	\
	$(F_4_BAR)bar.c


O_prog	=	\
	$(F_prog)main.c\
	$(F_prog)foo.c

SRCS_FILES		=	\
	$(O_4_BAR)	\
	$(O_prog)

#ty https://stackoverflow.com/questions/22499486/how-to-replace-c-and-cpp-file-together
OBJS	=	$(addsuffix .o,$(basename $(SRCS_FILES)))

#is cpp: no

all: $(NAME)

$(NAME): $(CC)

$(CC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)$(NAMEX)

clean:
	@rm -fv $(OBJS)

fclean: clean
	@rm -fv $(NAME)

rebuild:
	scb -d .

re: fclean all

PHONY:
#END