################################################################################
#______________________________________________________________________________#
#______/\/\/\/\/\___/\/\____________/\/\/\/\/\/\_/\/\__/\/\__/\/\______________#
#_____/\/\____/\/\_______/\/\__/\/\____/\/\___________/\/\__/\/\__/\/\/\/\/\___#
#____/\/\/\/\/\___/\/\____/\/\/\______/\/\_____/\/\__/\/\__/\/\______/\/\______#
#___/\/\_________/\/\____/\/\/\______/\/\_____/\/\__/\/\__/\/\____/\/\_________#
#__/\/\_________/\/\__/\/\__/\/\____/\/\_____/\/\__/\/\__/\/\__/\/\/\/\/\______#
#______________________________________________________________________________#
################################################################################

# **************************************************************************** #
#																			   #
#								  INTRO RULES								   #
#																			   #
# **************************************************************************** #

.PHONY: clean, fclean, re, open, reset, all, cleared

.SUFFIXES:

# **************************************************************************** #
#																			   #
#								    DISPLAY									   #
#																			   #
# **************************************************************************** #

# source file number
SRC_NBR = $(words $(SRCO_SVR))

# variable incremented after each file compilation
DONE = 0

# percentage done
PRCENT = $(shell echo \($(DONE) \* 100\) \/ $(SRC_NBR) | bc)

# rest from euclidian div
REST = $(shell echo \($(DONE) \* 100\) \% $(SRC_NBR) | bc)

# bar len based on percentage
PRGRSS = $(shell echo \($(PRCENT) \/ 2\) + 1 | bc)

# filled bar string cut with PRGRSS variable
BAR = $(shell echo "\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#" | cut -c 1-$(PRGRSS))

# empty bar string cut with PRGRSS variable
EMPTY = $(shell echo "                                                  " | cut -c $(PRGRSS)-50)

# color variable for a better display
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BLACK 		= \033[38;5;0m
NORMAL 		= \033[0m
# * ~ * #
DARK_RED 	= \033[38;5;88m
RED 		= \033[38;5;196m
# * ~ * #
DARK_GREEN 	= \033[38;5;28m
GREEN 		= \033[38;5;46m
# * ~ * #
DARK_YELLOW	= \033[38;5;136m
YELLOW 		= \033[38;5;226m
# * ~ * #
DARK_ORANGE	= \033[38;5;130m
ORANGE 		= \033[38;5;202m
# * ~ * #
DARK_BLUE 	= \033[38;5;18m
BLUE 		= \033[38;5;27m
# * ~ * #
DARK_PINK 	= \033[38;5;169m
PINK 		= \033[38;5;207m
# * ~ * #
DARK_PURPLE	= \033[38;5;54m
PURPLE 		= \033[38;5;57m
# * ~ * #
DARK_CYAN 	= \033[38;5;37m
CYAN 		= \033[38;5;45m
# * ~ * #
DARK_GREY 	= \033[38;5;240m
GREY 		= \033[38;5;245m
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# **************************************************************************** #
#																			   #
#								  MAKEFILE CORE								   #
#																			   #
# **************************************************************************** #

# CLIENT = client

# SERVER = server

# mode of the makefile (ex: NORMAL, DEBUG)
MAKEFILE_MODE = NORMAL

ifdef DEBUG
	MAKEFILE_MODE = DEBUG
endif

NAME = ircserv

VPATH = srcs:\
		incs:\
		tests:

OBJDIR = obj

TESTDIR = tests_exec

INCDIR = incs

HEADERS = $(addprefix $(INCDIR)/,	Connection.hpp	\
									FdSet.hpp		\
									Message.hpp		\
									ServerInfo.hpp	\
									SockInfo.hpp	\
									SockStream.hpp	\
									Command.hpp		\
									CommandLib.hpp	\
									Server.hpp		\
									Client.hpp)

MESSAGE = hello world !

SRCO_TEST_ = test.o

SRCO_TEST_FDSET =	FdSet.o		\
					test_fdset.o

SRCO_TEST_MESSAGE =	Message.o		\
					test_message.o

SRCO_TEST_SOCKSTREAM =	Message.o		\
						SockInfo.o		\
						SockStream.o	\
						test_sockstream.o

SRCO_TEST_SOCKINFO =	SockInfo.o		\
						test_sockinfo.o

SRCO_TEST_CONNECTION = 	Message.o		\
						SockInfo.o		\
						SockStream.o	\
						Connection.o	\
						test_connection.o

SRCO_TEST_SELECTMODULE = 	Message.o		\
							FdSet.o			\
							SockInfo.o		\
							SockStream.o	\
							Connection.o	\
							SelectModule.o	\
							test_selectmodule.o

SRCO_TEST_COMMAND =	Message.o		\
					Command.o		\
					test_command.o

SRCO_SVR = 	Connection.o	\
			FdSet.o			\
			Message.o		\
			ServerInfo.o	\
			SockInfo.o		\
			SockStream.o	\
			Command.o		\
			CommandLib.o	\
			Server.o		\
			Client.o		\
			main.o

# SRCO_CLT =	$(addprefix $(OBJDIR)/, client_main.o)

CXX = g++

CXXFLAGS = -std=c++98 -pedantic-errors

ifeq ($(MAKEFILE_MODE), DEBUG)
	CXXFLAGS += -g
endif

# **************************************************************************** #
#																			   #
#								   MASTER RULE								   #
#																			   #
# **************************************************************************** #

all: hide_cursor $(NAME) show_cursor

# **************************************************************************** #
#																			   #
#								  DISPLAY RULES								   #
#																			   #
# **************************************************************************** #

hide_cursor:
	@ echo "\033[?25l\c"

show_cursor:
	@ echo "\033[?25h\c"

# display_mode:
# 	@ echo "$(DARK_GREY)Makefile launched in [$(MAKEFILE_MODE)] mode.$(NORMAL)"

cleared:
	@ echo "$(RED)[CLEAN]$(NORMAL) $(DARK_GREY)\xe2\x9e\x9c $(RED)Object files$(NORMAL)"

full_clear:
	@ echo "$(RED)[CLEAN]$(NORMAL) $(DARK_GREY)\xe2\x9e\x9c $(RED)$(NAME) executable$(NORMAL)"

re_init:
	@ echo "$(PURPLE)[RESET]$(NORMAL) $(DARK_GREY)\xe2\x9e\x9c $(PURPLE)$(NAME)$(NORMAL)"

re_done:
	@ echo "$(PURPLE)[RESET]$(NORMAL) $(DARK_GREY)\xe2\x9e\x9c $(GREEN)Ok$(NORMAL)"

# **************************************************************************** #
#																			   #
#								OBJECT FILES RULES							   #
#																			   #
# **************************************************************************** #

$(OBJDIR)/%.o: %.cpp
	@ $(eval DONE = $(shell echo $(DONE) + 1 | bc ))
	@ echo "\r$(PURPLE)[$(NORMAL)$(BAR)$(EMPTY)$(PURPLE)] {$(NORMAL)$(PRCENT).$(REST)$(PURPLE)}   $(NORMAL)\c"
	@ mkdir -p $(OBJDIR)
	@ $(CXX) $(CXXFLAGS) -c $<
	@ mv ./$(notdir $@) ./$(OBJDIR)/

# **************************************************************************** #
#																			   #
#								EXECUTABLES RULES							   #
#																			   #
# **************************************************************************** #

$(NAME): $(addprefix $(OBJDIR)/,$(SRCO_SVR))
	@ $(eval DONE = $(shell echo $(SRC_NBR)))
	@ $(CXX) $(CXXFLAGS) -o $(NAME) $^
	@ echo "\r$(PURPLE)[$(NORMAL)$(BAR)$(EMPTY)$(PURPLE)] {$(NORMAL)$(PRCENT).$(REST)$(PURPLE)} $(NORMAL)\t\c"
	@ echo "$(PURPLE)[$(GREEN)\xe2\x9c\x94$(PURPLE)] $(NAME)$(NORMAL)"
	@ echo "$(ORANGE)\t\t\t\t\t\t\t\t \xe2\x86\xb3 [$(NAME)]$(NORMAL)"

# $(CLIENT): $(LIB_PATH)/$(LIB) $(SRCO_CLT)
# 	@ $(eval DONE = $(shell echo $(SRC_NBR)))
# 	@ echo "\r$(PURPLE)[$(NORMAL)$(BAR)$(EMPTY)$(PURPLE)] {$(NORMAL)$(PRCENT).$(REST)$(PURPLE)} $(NORMAL)\t\c"
# 	@ echo "$(PURPLE)[$(GREEN)\xe2\x9c\x94$(PURPLE)] $(CLIENT)$(NORMAL)"
# 	@ echo "$(ORANGE)\t\t\t\t\t\t\t\t \xe2\x86\xb3 [$(CLIENT)]$(NORMAL)"
# 	@ $(CXX) $(CXXFLAGS) -o $(CLIENT) $^

# **************************************************************************** #
#																			   #
#							   COMPILE TESTS RULES							   #
#																			   #
# **************************************************************************** #

test_%:
	@ $(MAKE) $(addprefix $(OBJDIR)/, $(SRCO_TEST_$(shell echo '$*' | tr '[:lower:]' '[:upper:]')))
	@ $(eval DONE = $(shell echo $(SRC_NBR)))
	@ $(CXX) $(CXXFLAGS) -o $@ $(addprefix $(OBJDIR)/, $(SRCO_TEST_$(shell echo '$*' | tr '[:lower:]' '[:upper:]')))
	@ echo "\r$(PURPLE)[$(NORMAL)$(BAR)$(EMPTY)$(PURPLE)] {$(NORMAL)$(PRCENT).$(REST)$(PURPLE)} $(NORMAL)\t\c"
	@ echo "$(PURPLE)[$(GREEN)\xe2\x9c\x94$(PURPLE)] $@$(NORMAL)"
	@ echo "$(ORANGE)\t\t\t\t\t\t\t\t \xe2\x86\xb3 [$@]$(NORMAL)"
	@ mkdir -p $(TESTDIR)
	@ mv ./$(notdir $@) ./$(TESTDIR)/


# **************************************************************************** #
#																			   #
#								  CLEANING RULES							   #
#																			   #
# **************************************************************************** #

clean: cleared
	@ rm -rf $(OBJDIR)

fclean: clean full_clear
	@ rm -rf ./$(OBJDIR)
	@ rm -rf ./$(TESTDIR)
	@ rm -f $(SERVER)

re: re_init fclean all re_done

# **************************************************************************** #