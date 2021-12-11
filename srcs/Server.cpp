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
**----- File ----------------{ Server.cpp }-----------------------------------**
**----- Created -------------{ 2021-05-26 16:47:47 }--------------------------**
**----- Updated -------------{ 2021-12-10 04:28:58 }--------------------------**
********************************************************************************
*/

#include "../incs/Server.hpp"

// ____________Canonical Form____________
Server::~Server(void) { return; }
// Server::Server(void) { return; }
Server::Server(Server const &cpy) :
	inherited(static_cast<inherited const &>(cpy)),
	servername(cpy.servername),
	token(cpy.token),
	implem(cpy.implem),
	version(cpy.version),
	info(cpy.info) { return; }
Server  &Server::operator=(Server const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	servername = cpy.servername;
	token = cpy.token;
	implem = cpy.implem;
	version = cpy.version;
	info = cpy.info;
	return *this;
}

// _____________Constructor______________
Server::Server(Connection *&src, ServerCommand const &cmd) :
	inherited(src), servername(cmd.servername()), token(cmd.token()), hop(cmd.hopcount())
	version(SERVER_VERSION), implem(SERVER_IMPLEM), info(cmd.info()) {
	isServer(true);
	delete src;
	return;
}

// Server::Server(Server *link, ServerCommand const &cmd) :
// 	inherited(link, CONX_AUTHEN | CONX_SERVER, cmd.hopcount()), servername(cmd.servername()),
// 	token(cmd.token()), version(SERVER_VERSION), implem(SERVER_IMPLEM), info(cmd.info()) {
// 	return;
// }

// __________Member functions____________
std::string const		&Server::name(void) const { return servername; }
Command::arglist const	Server::getArgList(void) const {
	Command::arglist ret;

	ret.push_back(servername);
	ret.push_back(std::to_string(hop)); //change
	ret.push_back(token);
	ret.push_back(info);
	return ret;
}

std::ostream &operator<<(std::ostream &flux, Server const &src) {
	Server *tmp;

	flux << "+++++++++++ Server [" << src.getHopcount() << "] ++++++++++" << std::endl;
	if (!src.isAccepted())
		flux << "\033[38;5;226m" << "[---CONNECTED---]" << "\033[38;5;46m" << std::endl;
	else if (!src.isLink())
		flux << "\033[38;5;226m" << "[---ACCEPTED---]" << "\033[38;5;46m" << std::endl;
	if (!src.isLink())
		flux << src.getSockinfo();
	else
	{
		tmp = static_cast<Server *>(src.getLink());
		flux << "\033[38;5;226m[---LINK---][" << tmp->getServername() << "]\033[38;5;46m" << std::endl;
	}
	flux << "Name: [" << src.getServername() << "]" << std::endl;
	flux << "Token: [" << src.getToken() << "]"  << std::endl;
	flux << "Version: [" << src.getVersion() << "]"  << std::endl;
	flux << "Implem: [" << src.getImplem() << "]"  << std::endl;
	flux << "+++++++++++++++++++++++++++++++++" << std::endl;
	return flux;
}
