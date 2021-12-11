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
**----- Updated -------------{ 2021-12-10 05:37:18 }--------------------------**
********************************************************************************
*/

#include "../incs/LocalServer.hpp"

static bool checkPort(std::string const &str) {
	int	port;

	port = std::atoi(str.c_str());
	return (port < 6667 || 6669 < port ? false : true);
}

static bool checkPassword(std::string const &str) {
	if (str.length() > 12)
		return false;
	else if (str.find_first_of(std::string(CHAR_FORBIDDEN)) != std::string::npos)
		return false;
	return true;
}

int main(int argc, char const *argv[])
{
	LocalServer *ls;
	std::string				authentication;
	std::string				port;
	std::string				password;

	if (argc == 3) {
		port = argv[argc - 2];
		password = argv[argc - 1];
	}
	else if (argc == 4) {
		authentication = argv[argc - 3];
		port = argv[argc - 2];
		password = argv[argc - 1];
	}
	else {
		std::cout << "Usage: ./ircserv [host:network_port:network_password] port password" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!checkPort(port)) {
		std::cout << "Invalid Port [" << port << "]. Port range is 6667-6669." << std::endl;
		exit(EXIT_FAILURE);
	} else
		port = port.substr(0, 4);
	if (!checkPassword(password)) {
		std::cout << "Invalid Password [" << password << "] due to invalid length/characters." << std::endl;
		exit(EXIT_FAILURE);
	}
	// try {
		ls = new LocalServer(port, AF_INET, password);
	// } catch (Connection::LocalSocketException &ex) {
	// 	std::cout << ex.what() << std::endl;
	// 	return (EXIT_FAILURE);
	// }

	ls->run();

	return (EXIT_SUCCESS);
}
