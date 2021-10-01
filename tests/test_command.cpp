/*
********************************************************************************
**____________________________________________________________________________**
**_____/\/\/\/\/\___/\/\____________/\/\/\/\/\/\_/\/\__/\/\__/\/\_____________**
**____/\/\____/\/\_______/\/\__/\/\____/\/\___________/\/\__/\/\__/\/\/\/\/\__**
**___/\/\/\/\/\___/\/\____/\/\/\______/\/\_____/\/\__/\/\__/\/\______/\/\_____**
**__/\/\_________/\/\____/\/\/\______/\/\_____/\/\__/\/\__/\/\____/\/\________**
**_/\/\_________/\/\__/\/\__/\/\____/\/\_____/\/\__/\/\__/\/\__/\/\/\/\/\_____**
**____________________________________________________________________________**
**                                                                            **
**----- Author --------------{ PixTillz }-------------------------------------**
**----- File ----------------{ test_command.cpp }-----------------------------**
**----- Created -------------{ 2021-07-02 12:57:32 }--------------------------**
**----- Updated -------------{ 2021-09-15 17:30:17 }--------------------------**
********************************************************************************
*/

#include "../incs/Command.hpp"
#include "../incs/Debug.hpp"

void testFromMessage(Message *input) {
	Command cmd(input);
	
	DEBUG_DISPCB(COUT, std::string(55, '#'), ORANGE, '-');
	DEBUG_DISP(COUT, "Prefix: " + cmd.getPrefix());
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	DEBUG_DISP(COUT, "Command: " + cmd.getCommand());
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	std::list<std::string>::const_iterator it = cmd.getArgs().begin();
	for (int i = 0; i < cmd.argNbr(); i++)
		DEBUG_DISPB(COUT, *(it++), '[');
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	DEBUG_DISPB(COUT, cmd.getContent(), '[');
	DEBUG_DISPCB(COUT, std::string(55, '#'), ORANGE, '-');
}

void testFromArgs(std::string const prefix, std::string const command, std::list<std::string> &args) {
	Command cmd(prefix, command, args);
	
	DEBUG_DISPCB(COUT, std::string(55, '#'), ORANGE, '-');
	DEBUG_DISP(COUT, "Prefix: " + cmd.getPrefix());
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	DEBUG_DISP(COUT, "Command: " + cmd.getCommand());
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	std::list<std::string>::const_iterator it = cmd.getArgs().begin();
	for (int i = 0; i < cmd.argNbr(); i++)
		DEBUG_DISPB(COUT, *(it++), '[');
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	DEBUG_DISPB(COUT, cmd.getContent(), '[');
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	DEBUG_LDISPB(COUT, "Command == COMMAND ? ", std::to_string(cmd == "COMMAND"), '[');
	DEBUG_DISPCB(COUT, std::string(45, '='), PURPLE, '-');
	DEBUG_LDISPB(COUT, "Command == EXIT ? ", std::to_string(cmd == ""), '[');
	DEBUG_DISPCB(COUT, std::string(55, '#'), ORANGE, '-');
}

int main (void) {
	// ----------------------------------------
	// test with parametric constructor 1
	testFromMessage(new Message(":prefix COMMAND arg0 arg1 arg2 :L arg 0 :L arg 1 :L arg 2 :L arg 3 :L arg 4"));
	// ----------------------------------------

	// ----------------------------------------
	// test with parametric constructor 2
	std::string prefix("prefix");
	std::string command("COMMAND");
	std::list<std::string> args;

	args.push_back("arg0");
	args.push_back("arg1");
	args.push_back("arg2");
	args.push_back("L arg 0");
	args.push_back("L arg 1");
	args.push_back("L arg 2");
	args.push_back("L arg 3");
	args.push_back("L arg 4");

	testFromArgs(prefix, command, args);
	// ----------------------------------------

	return (0);
}
