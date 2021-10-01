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
**----- File ----------------{ main.cpp }-------------------------------------**
**----- Created -------------{ 2021-09-22 18:21:01 }--------------------------**
**----- Updated -------------{ 2021-09-24 02:31:36 }--------------------------**
********************************************************************************
*/

#include "../incs/LocalServer.hpp"

#include <fcntl.h>

int main(int argc, char const *argv[])
{
	LocalServer *ls;
	std::string				port;
	std::string				password;

	if (argc != 3) {
		DEBUG_DISP(COUT, "Usage: ./ircserv port password");
		return (1);
	}
	port = argv[argc - 2];
	password = argv[argc - 1];
	ls = new LocalServer(port, AF_UNSPEC, password);

	ls->run();

	return (0);
}
