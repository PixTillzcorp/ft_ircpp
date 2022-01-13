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
**----- Updated -------------{ 2022-01-13 03:51:35 }--------------------------**
********************************************************************************
*/

#include "../incs/LocalServer.hpp"

static bool checkPort(std::string const &str) {
	int	port;

	if (str.empty())
		return false;
	port = std::atoi(str.c_str());
	return (port < 6667 || 6669 < port ? false : true);
}

static bool checkPassword(std::string const &str) {
	if (str.empty() || str.length() > 12)
		return false;
	else if (str.find_first_of(std::string(CHAR_FORBIDDEN)) != std::string::npos)
		return false;
	return true;
}

static std::vector<std::string> getAuth(std::string input) {
	std::vector<std::string> ret;
	size_t					 pos1;
	size_t					 pos2;

	if (input.empty())
		return ret;
	pos1 = input.find(':');
	pos2 = input.find(':', pos1 + 1);
	if (pos1 != std::string::npos && pos2 != std::string::npos) {
		ret.push_back(input.substr(0, pos1));
		ret.push_back(input.substr(pos1 + 1, pos2 - (pos1 + 1)));
		ret.push_back(input.substr(pos2 + 1, std::string::npos));
	}
	return ret;
}

int main(int argc, char const *argv[])
{
	LocalServer *ls;
	std::vector<std::string> auth;
	std::string				 port;
	std::string				 password;

	if (argc == 3) {
		port = argv[argc - 2];
		password = argv[argc - 1];
	}
	else if (argc == 4) {
		auth = getAuth(argv[argc - 3]);
		port = argv[argc - 2];
		password = argv[argc - 1];
	}
	else {
		std::cout << "Usage: ./ircserv [host:network_port:network_password] port password" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!checkPort(port)) {
		if (port.empty())
			std::cout << "Invalid \'Port\', cannot be an empty string." << std::endl;
		else
			std::cout << "Invalid \'Port\' [" << port << "]. \'Port\' range is 6667-6669." << std::endl;
		exit(EXIT_FAILURE);
	} else
		port = port.substr(0, 4);
	if (!checkPassword(password)) {
		if (password.empty())
			std::cout << "Invalid \'Password\', cannot be an empty string." << std::endl;
		else
			std::cout << "Invalid \'Password\' [" << password << "] due to invalid length/characters." << std::endl;
		exit(EXIT_FAILURE);
	}
	try {
		ls = new LocalServer(port, AF_INET, password);
	} catch (Connection::ConxInit &ex) {
		std::cout << ex.what() << std::endl;
		return (EXIT_FAILURE);
	} catch (ConfigParser::FailFileStream &ex) {
		std::cout << ex.what() << std::endl;
		std::cout << "\'server.config\' file required with following fields in it:" << std::endl;
		std::cout << "\t- Mandatory fields: \'servername\' \'oppass\' \'logfile\'" << std::endl;
		std::cout << "\t- Optional fields:  \'description\'" << std::endl;
		return (EXIT_FAILURE);
	}

	if (!auth.empty())
		ls->joinNet(auth[0], auth[1], auth[2]);

	ls->run();

	delete ls;

	return (EXIT_SUCCESS);
}
