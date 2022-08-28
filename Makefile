SRC	= $(SRCDIR)/main.cpp $(SRCDIR)/User.cpp $(SRCDIR)/Server.cpp $(SRCDIR)/utils.cpp

SRCDIR	= src

INC	= inc

OBJDIR	= obj

OBJ		= $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

DEP		= $(OBJ:%.o=%.d)

CFLAGS	= -Wall -Wextra -Werror -std=c++98

HOST = $(shell whoami)
ifeq ($(HOST),louisraffin)
	CC = c++-11
else
	CC = c++
endif

RM	= rm -rf

NAME	= ircserv

all	: $(NAME)

$(NAME)	: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -I $(INC) -o $@

-include $(DEP)

$(OBJDIR)/%.o	: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -I $(INC) -c $< -o $@

clean	:
	$(RM) $(OBJDIR)

fclean	: clean
	$(RM) $(NAME)

re	: fclean all
