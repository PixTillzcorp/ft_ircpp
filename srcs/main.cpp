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
**----- Updated -------------{ 2022-01-28 19:00:06 }--------------------------**
********************************************************************************
*/

#include "LocalServer.hpp"

int main(int argc, char const *argv[])
{
	LocalServer *ls;
	std::string authinfo;
	std::string	port;
	std::string	password;

	if (argc == 3) {
		port = argv[argc - 2];
		password = argv[argc - 1];
	}
	else if (argc == 4) {
		authinfo = argv[argc - 3];
		port = argv[argc - 2];
		password = argv[argc - 1];
	}
	else {
		std::cerr << "Usage: ./ircserv [host:network_port:network_password] port password" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!Utils::validPort(port)) {
		if (port.empty())
			std::cerr << "Invalid \'Port\', cannot be an empty string." << std::endl;
		else
			std::cerr << "Invalid \'Port\' [" << port << "]. \'Port\' range is 6667-6669." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!Utils::validPassword(password)) {
		if (password.empty())
			std::cerr << "Invalid \'Password\', cannot be an empty string." << std::endl;
		else
			std::cerr << "Invalid \'Password\' [" << password << "] due to invalid length/characters." << std::endl;
		exit(EXIT_FAILURE);
	}
	try {
		ls = new LocalServer(port, AF_INET, password);
	} catch (Connection::ConxInit &ex) {
		std::cerr << ex.what() << std::endl;
		return (EXIT_FAILURE);
	} catch (ConfigParser::FailFileStream &ex) {
		std::cerr << ex.what() << std::endl;
		std::cerr << "\'whitelist.config\' file required with following syntax:" << std::endl;
		std::cerr << "\t- \'servername\' = \'host\',\'port\',\'password\'" << std::endl;
		std::cerr << "\'server.config\' file required with following fields:" << std::endl;
		std::cerr << "\t- Mandatory fields: \'servername\' \'oppass\' \'logfile\'" << std::endl;
		std::cerr << "\t- Optional fields:  \'description\'" << std::endl;
		std::cerr << "\t- \'key\' = \'value\'" << std::endl;
		return (EXIT_FAILURE);
	} catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
		std::cerr << "Internal error." << std::endl;
		return (EXIT_FAILURE);
	}

	if (!authinfo.empty())
		ls->joinNet(authinfo);

	ls->run();

	delete ls;

	return (EXIT_SUCCESS);
}
