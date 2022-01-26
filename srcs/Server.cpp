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
**----- Updated -------------{ 2022-01-17 09:55:42 }--------------------------**
********************************************************************************
*/

#include "Server.hpp"
#include "Utils.hpp"

// ____________Canonical Form____________
Server::~Server(void) { return; }
// Server::Server(void) { return; }
Server::Server(Server const &cpy) :
	inherited(static_cast<inherited const &>(cpy)),
	servername(cpy.servername), token(cpy.token),
	implem(cpy.implem), version(cpy.version),
	desc(cpy.desc) { return; }
Server  &Server::operator=(Server const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	servername = cpy.servername;
	token = cpy.token;
	implem = cpy.implem;
	version = cpy.version;
	desc = cpy.desc;
	return *this;
}

// _____________Constructor______________
Server::Server(Connection *&src, ServerCommand const &cmd) :
	inherited(*src), servername(cmd.servername()),
	version(SERVER_VERSION), implem(SERVER_IMPLEM) {
	if (cmd.argNbr(4)) {
		token = cmd.token();
		desc = cmd.info();
	} else
		desc = cmd.argX(1);
	isServer(true);
	delete src;
}

Server::Server(Server *link, ServerCommand const &cmd) :
	inherited(link, CONX_AUTHEN | CONX_SERVER, Utils::strToNbr(cmd.hopcount())),
	servername(cmd.servername()), token(cmd.token()), version(SERVER_VERSION),
	implem(SERVER_IMPLEM), desc(cmd.info()) {
	return;
}

Server::Server(std::string const &host, std::string const &port, u_int16_t family) :
	inherited(host, port, family), version(SERVER_VERSION), implem(SERVER_IMPLEM), token("1") {
	isServer(true);
	return;
}

// __________Member functions____________
std::string const		&Server::name(void) const { return servername; }
Command::argvec const	Server::getArgListConnect(void) const {
	Command::argvec ret;

	ret.push_back(servername);
	ret.push_back(desc);
	return ret;
}

Command::argvec const	Server::getArgListAccept(void) const {
	Command::argvec ret;

	ret.push_back(servername);
	ret.push_back("1");
	ret.push_back(token);
	ret.push_back(desc);
	return ret;
}

bool	Server::compare(Connection *conx) const {
	Server	*tmp;

	if (!conx || !conx->isServer())
		return true;
	tmp = static_cast<Server *>(conx);
	if (!tmp->servername.compare(servername))
		return false;
	return true;
}

bool	Server::isToken(std::string const &cmp) const { return (!token.compare(cmp)); }

std::ostream &operator<<(std::ostream &flux, Server const &src) {
	flux << static_cast<Connection const &>(src);
	flux << "name[" << src.servername << "]v[" << src.implem << "|" << src.version << "]tok[" << src.token << "]";
	if (!src.desc.empty())
		flux << std::endl << "\t\tdesc ->" << src.desc;
	return flux;
}
