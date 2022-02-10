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
**----- Updated -------------{ 2022-02-10 02:07:57 }--------------------------**
********************************************************************************
*/

#include "Server.hpp"
#include "Utils.hpp"

// ____________Canonical Form____________
Server::~Server(void) {
	if (!conxs.empty()) {
		for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
			(*it)->end();
		}
	}

}
// Server::Server(void) { return; }
Server::Server(Server const &cpy) :
	inherited(static_cast<inherited const &>(cpy)), servername(cpy.servername),
	conxs(cpy.conxs), token(cpy.token), implem(cpy.implem),
	version(cpy.version), desc(cpy.desc) { return; }
Server  &Server::operator=(Server const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	servername = cpy.servername;
	conxs = cpy.conxs;
	token = cpy.token;
	implem = cpy.implem;
	version = cpy.version;
	desc = cpy.desc;
	return *this;
}

// _____________Constructor______________
// Direct server constructor
Server::Server(Connection *&src, ServerCommand const &cmd) :
	inherited(*src), servername(cmd.servername()),
	implem(SERVER_IMPLEM), version(SERVER_VERSION) {
	if (cmd.argNbr(4)) {
		token = cmd.token();
		desc = cmd.info();
	} else
		desc = cmd.argX(1);
	isServer(true);
	delete src;
}

// Link server constructor
Server::Server(Server *link, ServerCommand const &cmd) :
	inherited(link, CONX_AUTHEN | CONX_SERVER, Utils::strToNbr(cmd.hopcount())),
	servername(cmd.servername()), token(cmd.token()), implem(SERVER_IMPLEM),
	version(SERVER_VERSION), desc(cmd.info()) {
	return;
}

// Local server constructor
Server::Server(std::string const &host, std::string const &port, u_int16_t family) :
	inherited(host, port, family), implem(SERVER_IMPLEM), version(SERVER_VERSION) {
	isServer(true);
	Utils::initTokenSeed(static_cast<unsigned int>(std::time(nullptr)));
	token = Utils::genToken(SERVER_TOKEN_LEN);
	return;
}

// __________Member functions____________
std::string const		&Server::name(void) const { return servername; }

Command::argvec const	Server::serverArgsShare(std::string const &servertoken) const {
	Command::argvec ret;

	ret.push_back(servername);
	try { ret.push_back(Utils::nbrToStr(hop)); }
	catch (Utils::FailStream &ex) { ret.push_back("none"); }
	ret.push_back(servertoken);
	ret.push_back(desc);
	return ret;
}

Command::argvec const	Server::serverArgsConnect(void) const {
	Command::argvec ret;

	ret.push_back(servername);
	ret.push_back(desc);
	return ret;
}

Command::argvec const	Server::serverArgsAccept(void) const {
	Command::argvec ret;

	ret.push_back(servername);
	ret.push_back("1");
	ret.push_back(token);
	ret.push_back(desc);
	return ret;
}


unsigned int		Server::howMany(char flag) const {
	unsigned int	count = 0;

	for (conxlist_cit it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isClient()) {
			if (static_cast<Client *>(*it)->checkModeFlag(flag))
				count++;
		} else if ((*it)->isServer() && !(*it)->isLink())
			count += static_cast<Server *>(*it)->howMany(flag);
	}
	return count;
}

unsigned int		Server::howMany(unsigned short check, bool direct) const {
	unsigned int	count = 0;

	for (conxlist_cit it = conxs.begin(); it != conxs.end(); it++) {
		if (direct && (*it)->isLink())
			continue;
		if ((*it)->checkStatus(check))
			count++;
		if (!direct && (*it)->isServer())
			count += static_cast<Server *>(*it)->howMany(check, direct);
	}
	return count;
}

std::string const 	Server::howManyClient(bool direct) const { return Utils::nbrToStr(howMany(CONX_CLIENT, direct)); }
std::string const 	Server::howManyServer(bool direct, bool self) const { return Utils::nbrToStr(howMany(CONX_SERVER, direct) + (self ? 1 : 0)); }
std::string const 	Server::howManyService(bool direct) const { return Utils::nbrToStr(howMany(CONX_SERVICE, direct)); }
std::string const 	Server::howManyUnknown(void) const { return Utils::nbrToStr(howMany(CONX_PENDING, false)); }
std::string const 	Server::howManyOperator(void) const { return Utils::nbrToStr(howMany(CLIENT_FLAG_OPERATOR)); }

void	Server::addLink(Connection *conx) {
	if (conx->isLink())
		conxs.push_back(conx);
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

bool	Server::isToken(std::string const &cmp) const {
	return (!token.compare(cmp));
}

bool	Server::knowToken(std::string const &cmp) const {
	if (isToken(cmp))
		return true;
	for (conxlist_cit it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer() && static_cast<Server *>(*it)->knowToken(cmp))
			return true;
	}
	return false;
}

void	Server::newToken(void) {
	std::string tmp;

	tmp = Utils::genToken(SERVER_TOKEN_LEN);
	if (knowToken(tmp))
		return newToken();
	token = tmp;
}

Server	*Server::getTokenSource(std::string const &tk) {
	if (tk.empty())
		return nullptr;
	if (isToken(tk))
		return this;
	for (conxlist_cit it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer() && static_cast<Server *>(*it)->isToken(tk))
			return static_cast<Server *>(*it);
	}
	return nullptr;
}

Client	*Server::findClient(std::string const &name, char type) {
	Client	*tmp;

	if ((type != 'n' && type != 'u' && type != 'r') || conxs.empty())
		return nullptr;
	if ((type == 'n' && !Utils::validNickName(name)) ||
		(type == 'u' && !Utils::validUserName(name)) ||
		(type == 'r' && !Utils::validRealName(name))) {
		return nullptr;
	}
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isClient()) {
			if ((type == 'n' && static_cast<Client *>(*it)->checkNickname(name)) ||
				(type == 'u' && static_cast<Client *>(*it)->checkUsername(name)) ||
				(type == 'r' && static_cast<Client *>(*it)->checkRealname(name))) {
				return static_cast<Client *>(*it);
			}
		} else if ((*it)->isServer()) {
			if ((tmp = static_cast<Server *>(*it)->findClient(name, type)))
				return tmp;
		}
	}
	return nullptr;
}

void	Server::shareClients(Server *target) {
	if (!target)
		return;
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if (target->compare(*it) && (*it)->isClient())
			target->send(NickCommand(name(), static_cast<Client *>(*it)->nickArgs(target->token)));
		if ((*it)->isServer() && !(*it)->isLink())
			static_cast<Server *>(*it)->shareClients(target);
	}
}

Server	*Server::findServer(std::string const &name) {
	Server *tmp;

	if (conxs.empty())
		return nullptr;
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer()) {
			if (!static_cast<Server *>(*it)->servername.compare(name))
				return static_cast<Server *>(*it);
			else {
				if ((tmp = static_cast<Server *>(*it)->findServer(name)))
					return tmp;
			}
		}
	}
	return nullptr;
}

void	Server::shareServs(Server *sender, Server *target) {
	if (!target || !sender)
		return;
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if (target->compare(*it) && (*it)->isServer()) {
			target->send(ServerCommand(sender->name(), static_cast<Server *>(*it)->serverArgsShare(sender->token)));
			sender->newToken();
			if (!(*it)->isLink())
				static_cast<Server *>(*it)->shareServs(sender, target);
		}
	}
}

void	Server::endDeadLinks(void) {
	if (!isLink()) {
		for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
			if ((*it)->isFinished()) {
				try { (*it)->end(); }
				catch (Connection::FailClose &ex) { std::cerr << ex.what() << std::endl; }
				conxs.erase(it--);
			}
		}
	}
}

std::ostream &operator<<(std::ostream &flux, Server const &src) {
	flux << static_cast<Connection const &>(src);
	flux << "[" << src.version << "]\t| [" << src.servername << "] - [" << src.implem << "][" << src.token << "]";
	if (!src.desc.empty())
		flux << std::endl << "\tDesc -> [" << src.desc << "]";
	for (Server::conxlist_cit it = src.conxs.begin(); it != src.conxs.end(); it++) {
		if ((*it)->isClient())
			flux << std::endl << "\t - [" << static_cast<Client *>(*it)->fullId() << "]";
		else
			flux << std::endl << "\t - [" << (*it)->name() << "]";
	}
	return flux;
}
