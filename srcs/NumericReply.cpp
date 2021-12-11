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
**----- File ----------------{ NumericReply.cpp }-----------------------------**
**----- Created -------------{ 2021-06-15 13:56:27 }--------------------------**
**----- Updated -------------{ 2021-06-23 18:19:46 }--------------------------**
********************************************************************************
*/

#include "../incs/NumericReply.hpp"

// ------------------------------------------
// 			     Reply Defines
// ------------------------------------------

// 1 -> 4 sent after a client connection.
# define RPL_WELCOME "001"
# define RPL_YOURHOST "002"
# define RPL_CREATED "003"
# define RPL_MYINFO "004"
// # define RPL_BOUNCE 005

// ____________Canonical Form____________
NumericReply::~NumericReply(void) { return; }
// NumericReply::NumericReply(void) { return; } /* default constructor is private */
NumericReply::NumericReply(NumericReply const &src) : Command(static_cast<Command const &>(src)) {
	return;
}
NumericReply  &NumericReply::operator=(NumericReply const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}
// ______________________________________

// _____________Constructor______________
// -----------------------------
//		Parametric constructor
// -----------------------------
NumericReply::NumericReply(ServerInfo const &server, Connection const &dest, std::string const &code) : Command("", "000", std::list<std::string>()) {
	std::list<std::string> args;
	std::string sender;

	// if (code == RPL_WELCOME)
	// 	args.push_back("Welcome to the network !");
	// else
	// 	std::cout << "Unknown code for reply." << std::endl; //add _valid to 'Command' class. 
	// if (code == RPL_YOURHOST)
	// 	args.push_back("Welcome to the network !");
	static_cast<Command &>(*this) = Command(sender, std::to_string(code), args);
}
// ______________________________________