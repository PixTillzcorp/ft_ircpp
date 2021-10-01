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
**----- File ----------------{ LocalServer.cpp }------------------------------**
**----- Created -------------{ 2021-09-07 16:32:43 }--------------------------**
**----- Updated -------------{ 2021-10-01 20:53:00 }--------------------------**
********************************************************************************
*/

#include "../incs/LocalServer.hpp"
#include "../incs/Debug.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
LocalServer::~LocalServer() { return; }
// LocalServer::LocalServer() { return; } /* Private standard constructor*/
LocalServer::LocalServer(LocalServer const &src) { *this = src; }
LocalServer &LocalServer::operator=(LocalServer const &src) {
	static_cast<Connection &>(*this) = static_cast<Connection const &>(src);
	this->_nicknames = src.getNicknames();
	this->_usernames = src.getUsernames();
	this->_realnames = src.getRealnames();
	this->_count = src.getCount();
	this->_conxs = src.getConxs();
	this->_sm = src.getSM();
	this->_password = src.getPassword();
	return *this;
}

// _____________Constructor______________
LocalServer::LocalServer(std::string const &port, u_int16_t family, std::string password) : Connection(port, family),
																							_password(password) {
	this->_sm = SelectModule(this->sock(), true);
	this->_count.push_back(0); // direct server
	this->_count.push_back(0); // server
	this->_count.push_back(0); // direct client
	this->_count.push_back(0); // client
	this->_count.push_back(0); // service
	this->_count.push_back(0); // unknown (pending connection)
	this->_count.push_back(0); // operator (client with op flag)
	this->_count.push_back(0); // channel
}

// __________Member functions____________

bool		LocalServer::run(void) {
	DEBUG_DISPC(COUT, "\t+ Local server running +", DARK_PURPLE);
	while(!this->isFinished()) {
		this->selectCall();
		this->checkStd();
		this->checkSock();
		this->checkConxs();
	}
	return true;
}

Connection *LocalServer::acceptConx(void) const {
	struct sockaddr_storage output_addr;
	socklen_t				addr_len;
	int						sock;

	addr_len = sizeof(output_addr);
	if ((sock = accept(this->sock(), (struct sockaddr *)&output_addr, &addr_len)) == -1) {
		std::cout << "Function accept() failed." << std::endl;
		return nullptr;
	}
	return(new Connection(SockInfo((struct sockaddr *)&output_addr, CONNECTABLE), sock, CONX_NOFLAG));
}

void		LocalServer::newConx(void) {
	DEBUG_DISPC(COUT, "~> New Connection !", DARK_ORANGE);
	// if (this->_conxs.empty())
	// 	this->newChan();
	this->_conxs.push_back(this->acceptConx());
	if (!this->_conxs.empty() && !this->_conxs.back())
		this->_conxs.pop_back();
	else {
		this->_count[COUNT_UNKNOWN]++;
		this->_sm.addFd(this->_conxs.back()->sock());
	}
}

void		LocalServer::finishConx(Connection *target, bool clear) {
	target->setFinished();
	if (clear)
		target->clearMessages();
	if (target->isPending())
		this->_count[COUNT_UNKNOWN]--;
	if (target->isServer()) {
		if (target->isLink())
			this->_count[COUNT_SERVER]--;
		else
			this->_count[COUNT_DIRECTSERVER]--;
	}
	if (target->isService())
		this->_count[COUNT_SERVICE]--;
	else if (target->isClient()) {
		if (target->isLink())
			this->_count[COUNT_CLIENT]--;
		else {
			this->_count[COUNT_DIRECTCLIENT]--;
			if (static_cast<Client *>(target)->isOperator())
				this->_count[COUNT_OPERATOR]--;
		}
		this->_nicknames.erase(static_cast<Client *>(target)->getNickname());
		this->_usernames.erase(static_cast<Client *>(target)->getUsername());
		this->_realnames.erase(static_cast<Client *>(target)->getRealname());
	}
}

void		LocalServer::selectCall(void) {
	this->_sm.call(this->_conxs); //check exception later and return bool in order to fix _sm
	DEBUG_DISPC(COUT, "~> Select triggered !", DARK_PINK);
}

Client		*LocalServer::findClientByName(std::string const &name, unsigned char type) {
	std::map<std::string, Connection *>::iterator it;

	if (type & NICKNAME) {
		if ((it = this->_nicknames.find(name)) != this->_nicknames.end())
			return (static_cast<Client *>(it->second));
	}
	if (type & USERNAME) {
		if ((it = this->_usernames.find(name)) != this->_usernames.end())
			return (static_cast<Client *>(it->second));
	}
	if (type & REALNAME) {
		if ((it = this->_realnames.find(name)) != this->_realnames.end())
			return (static_cast<Client *>(it->second));
	}
	return (nullptr);
}

std::string const 	LocalServer::howManyDirectClient(void) const { return this->nbrToStr(this->_count[COUNT_DIRECTCLIENT]); }
std::string const 	LocalServer::howManyClient(void) const { return this->nbrToStr(this->_count[COUNT_CLIENT] + this->_count[COUNT_DIRECTCLIENT]); }
std::string const 	LocalServer::howManyDirectServer(bool self) const { return this->nbrToStr(this->_count[COUNT_DIRECTSERVER] + (self ? 1 : 0)); }
std::string const 	LocalServer::howManyServer(bool self) const { return this->nbrToStr(this->_count[COUNT_SERVER] + this->_count[COUNT_DIRECTSERVER] + (self ? 1 : 0)); }
std::string const 	LocalServer::howManyService(void) const { return this->nbrToStr(this->_count[COUNT_SERVICE]); }
std::string const 	LocalServer::howManyUnknown(void) const { return this->nbrToStr(this->_count[COUNT_UNKNOWN]); }
std::string const 	LocalServer::howManyOperator(void) const { return this->nbrToStr(this->_count[COUNT_OPERATOR]); }
std::string const 	LocalServer::howManyChannel(void) const { return this->nbrToStr(this->_count[COUNT_CHANNEL]); }

// ____________Setter / Getter___________
// _nicknames
std::map<std::string, Connection *> const	&LocalServer::getNicknames(void) const { return this->_nicknames; }
void										LocalServer::setNicknames(std::map<std::string, Connection *> &src) { this->_nicknames = src; }

// _usernames
std::map<std::string, Connection *> const	&LocalServer::getUsernames(void) const { return this->_usernames; }
void										LocalServer::setUsernames(std::map<std::string, Connection *> &src) { this->_usernames = src; }

// _realnames
std::map<std::string, Connection *> const	&LocalServer::getRealnames(void) const { return this->_realnames; }
void										LocalServer::setRealnames(std::map<std::string, Connection *> &src) { this->_realnames = src; }

// _count
std::vector<unsigned int> const				&LocalServer::getCount(void) const { return this->_count; }
void										LocalServer::setCount(std::vector<unsigned int> &src) { this->_count = src; }

// _conxs
std::list<Connection *> const				&LocalServer::getConxs(void) const { return this->_conxs; }
void										LocalServer::setConxs(std::list<Connection *> &src) { this->_conxs = src; }

// _sm
SelectModule const							&LocalServer::getSM(void) const { return this->_sm; }
void										LocalServer::setSM(SelectModule &src) { this->_sm = src; }

// _password
std::string const							&LocalServer::getPassword(void) const { return this->_password; }

// ########################################
// 				   PRIVATE
// ########################################

bool		LocalServer::isReadable(Connection *conx) const { return this->_sm.checkRfds(conx->sock()); }
bool		LocalServer::isWritable(Connection *conx) const { return this->_sm.checkWfds(conx->sock()); }

void		LocalServer::checkStd(void) {
	std::string input;

	if (this->_sm.checkStd()) {
		std::getline(std::cin, input);
		DEBUG_LDISPCB(COUT, "Input: ", input, PINK, '[');
		if (!input.compare("EXIT") || !input.compare("Exit") || !input.compare("exit"))
			this->setFinished();
		if (!input.compare("NET") || !input.compare("Net") || !input.compare("net"))
			this->showNet();
		if (!input.compare("LOCAL") || !input.compare("Local") || !input.compare("local"))
			this->showLocalServer();
		if (!input.compare("NICKS") || !input.compare("Nicks") || !input.compare("nicks"))
			this->showNames(this->_nicknames);
		if (!input.compare("USERS") || !input.compare("Users") || !input.compare("users"))
			this->showNames(this->_usernames);
		if (!input.compare("REALNAMES") || !input.compare("Realnames") || !input.compare("realnames"))
			this->showNames(this->_realnames);
	}
}

void		LocalServer::checkSock(void) {
	if (this->isReadable(static_cast<Connection *>(this)))
		this->newConx();
}

void		LocalServer::checkConxs(void) {
	for (std::list<Connection *>::iterator it = this->_conxs.begin(); it != this->_conxs.end(); it++) {
		if (this->isReadable(*it)) {
			if (!(*it)->read()) {
				DEBUG_DISPC(COUT, "~> Someone left !", RED);
				if ((*it)->isClient())
					this->finishConx(*it, true);
				continue;
			}
			else if ((*it)->hasInputMessage())
				this->execCommand(*it, (*it)->getLastCommand());
		}
		if (this->isWritable(*it)) {
			(*it)->write();
		}
	}
}

// Execution module
void		LocalServer::execCommand(Connection *&sender, Command *cmd) {
	if (!cmd)
		return;
	std::cout << *cmd << std::endl;
	try {
		if (sender->isPending())
			this->execCommandPending(sender, cmd);
		else if (sender->isClient())
			this->execCommandClient(sender, cmd);
	}
	catch (Command::InvalidCommandException &e) {
		if (e.code == ERR_WRONGPARAMSNUMBER || e.code == ERR_NORECIPIENT)
			this->numericReply(sender, e.code, cmd->getCommand());
		else
			this->numericReply(sender, e.code);
		if (!sender->isAuthentified())
			sender->setFinished();
	}
	// std::cout << cmd->message() << std::endl;
	delete cmd;
}

void		LocalServer::execCommandPending(Connection *&sender, Command *cmd) {
	if (!sender->isAuthentified()) {
		if (*cmd == "CAP")
			this->numericReply(sender, ERR_DISCARDCOMMAND);
		else if (*cmd == "PASS")
			this->execPass(sender, PassCommand(*cmd));
		else
			this->numericReply(sender, ERR_NOTREGISTERED);
	}
	else {
		if (*cmd == "NICK")
			this->execNick(sender, NickCommand(*cmd));
		else if (*cmd == "USER")
			this->execUser(sender, UserCommand(*cmd));
		else
			this->numericReply(sender, ERR_NOTREGISTERED);
		// else if (*cmd == "SERVER")
		// 	this->execServer(sender, ServerCommand(*cmd));
	}
}

void		LocalServer::execCommandClient(Connection *&sender, Command *cmd) {
	if (!(static_cast<Client *>(sender)->isRegistered())) {
		if (*cmd == "NICK")
			this->execNick(sender, NickCommand(*cmd));
		else if (*cmd == "USER")
			this->execUser(sender, UserCommand(*cmd));
		else
			this->numericReply(sender, ERR_NOTREGISTERED); 
	}
	else {
		if (*cmd == "NICK")
			this->execNick(sender, NickCommand(*cmd));
		else if (*cmd == "USER")
			this->execUser(sender, UserCommand(*cmd));
		else if (*cmd == "QUIT")
			this->execQuit(sender, QuitCommand(*cmd));
		else if (*cmd == "PRIVMSG")
			this->execPrivmsg(sender, PrivmsgCommand(*cmd));
		else if (*cmd == "PING")
			this->execPing(sender, PingCommand(*cmd));
		else if (*cmd == "PONG")
			this->execPong(sender, PongCommand(*cmd));
		else if (*cmd == "MOTD")
			this->execMotd(sender, MotdCommand(*cmd));
		else if (*cmd == "LUSERS")
			this->execLusers(sender, LusersCommand(*cmd));
		else if (*cmd == "PASS")
			this->numericReply(sender, ERR_ALREADYREGISTERED);
		else
			this->numericReply(sender, ERR_UNKNOWNCOMMAND, cmd->getCommand());
	}
}

// void		LocalServer::execCommandServer(Connection *&sender, Command *cmd) {
	
// }

// void		LocalServer::execCommandService(Connection *&sender, Command *cmd) {
	
// }

void		LocalServer::execPass(Connection *&sender, PassCommand const &cmd) {
	cmd.isValid();
	if (!sender->isAuthentified()) {
		if (cmd.password() == this->_password)
			sender->setAuthentified();
		else {
			std::cout << "Password: [" << cmd.password() << "] -> [" << this->_password << "]" << std::endl;
			this->numericReply(sender, ERR_PASSWDMISMATCH);
			this->finishConx(sender, false);
		}
	}
	else
		this->numericReply(sender, ERR_ALREADYREGISTERED);
}

void		LocalServer::execNick(Connection *&sender, NickCommand const &cmd) {
	Client *tmp;

	cmd.isValid();
	if (this->_nicknames.count(cmd.nickname()))
		this->numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	if (sender->isClient()) {
		tmp = static_cast<Client *>(sender);
		if (!tmp->isRegistered()) {
			if (tmp->getNickname().empty()) {
				tmp->setNickname(cmd.nickname());
				this->numericReply(sender, RPL_WELCOME, tmp->fullId());
			}
			else {
				this->_nicknames.erase(tmp->getNickname());
				tmp->setNickname(cmd.nickname());
			}
			this->_nicknames[tmp->getNickname()] = sender;
		}
		else
			this->numericReply(sender, ERR_ALREADYREGISTERED); //change nickname and notify net later
	}
	else if (sender->isPending()) {
		sender = static_cast<Connection *>(new Client(sender, cmd));
		this->_count[COUNT_UNKNOWN]--;
		this->_count[COUNT_DIRECTCLIENT]++;
		this->_nicknames[static_cast<Client *>(sender)->getNickname()] = sender;
	}
}

void		LocalServer::execUser(Connection *&sender, UserCommand const &cmd) {
	Client *tmp;

	cmd.isValid();
	if (sender->isClient()) {
		tmp = static_cast<Client *>(sender);
		if (!tmp->isRegistered()) {
			if (tmp->getUsername().empty()) {
				tmp->setUsername(cmd.username());
				tmp->setRealname(cmd.realname());
				this->numericReply(sender, RPL_WELCOME, tmp->fullId());
			}
			else {
				this->_usernames.erase(tmp->getUsername());
				tmp->setUsername(cmd.username());
				this->_realnames.erase(tmp->getRealname());
				tmp->setRealname(cmd.realname());
			}
			tmp->setModes(cmd.modes()[0] % 48); //temporary setup
			this->_usernames[tmp->getUsername()] = sender;
			this->_realnames[tmp->getRealname()] = sender;
		}
		else
			this->numericReply(sender, ERR_ALREADYREGISTERED);
	}
	else if (sender->isPending()) {
		sender = static_cast<Connection *>(new Client(sender, cmd));
		this->_count[COUNT_UNKNOWN]--;
		this->_count[COUNT_DIRECTCLIENT]++;
		this->_usernames[static_cast<Client *>(sender)->getUsername()] = sender;
		this->_realnames[static_cast<Client *>(sender)->getRealname()] = sender;
	}
}

void		LocalServer::execPrivmsg(Connection *&sender, PrivmsgCommand const &cmd) {
	Client	*tmp;

	cmd.isValid();
	if ((tmp = this->findClientByName(cmd.target(), CLIENTNAME))) {
		tmp->send(PrivmsgCommand(static_cast<Client *>(sender)->getNickname(), cmd.getArgs()));
	}
	else
		this->numericReply(sender, ERR_NOSUCHNICK, cmd.target());
}

void		LocalServer::execQuit(Connection *&sender, QuitCommand const &cmd) {
	if (sender->isClient()) {
		sender->send(ErrorCommand("Closing connection"));
		this->finishConx(sender, false);
	}
}

void		LocalServer::execPing(Connection *&sender, PingCommand const &cmd) { cmd.isValid(); sender->send(PongCommand(this->hostname())); } //later change hostname for servername
void		LocalServer::execPong(Connection *&sender, PongCommand const &cmd) { cmd.isValid(); } //command is discarded
void		LocalServer::execMotd(Connection *&sender, MotdCommand const &cmd) {
	std::ifstream file("motd.txt");
	std::string input;
	std::string tmp;

	cmd.isValid();
	// if (!cmd.target().empty()) { //not mandatory
	// 	file.close();
	// 	this->share()
	// }
	if (file.fail()) {
		file.close();
		this->numericReply(sender, ERR_NOMOTD);
	}
	else {
		this->numericReply(sender, RPL_MOTDSTART, this->hostname());
		while (!file.eof()) {
			std::getline(file, input);
			while (!input.empty()) {
				if (input.length() < MAX_MOTD_READ)
					input.append(std::string(MAX_MOTD_READ - input.length(), ' '));
				tmp = input.substr(0, MAX_MOTD_READ);
				input.erase(0, MAX_MOTD_READ);
				this->numericReply(sender, RPL_MOTD, tmp);
			}
		}
		this->numericReply(sender, RPL_ENDOFMOTD);
		file.close();
	}
}

void		LocalServer::execLusers(Connection *&sender, LusersCommand const &cmd) {
	cmd.isValid();

	this->numericReply(sender, RPL_LUSERCLIENT);
	this->numericReply(sender, RPL_LUSEROP);
	this->numericReply(sender, RPL_LUSERUNKNOWN);
	this->numericReply(sender, RPL_LUSERCHANNELS);
	this->numericReply(sender, RPL_LUSERME);
}

void		LocalServer::numericReply(Connection *&sender, unsigned short code) {
	std::list<std::string> args;

	args.push_back(sender->name());
	if (code == ERR_DISCARDCOMMAND)
		return;
	else if (code == RPL_LUSERCLIENT)									// 251
		args.push_back("There are " + this->howManyClient() + " users and " + this->howManyService() + " services on " + this->howManyServer(true) + " servers");
	else if (code == RPL_LUSEROP) {										// 252
		args.push_back(this->howManyOperator());
		args.push_back("operator(s) online");
	}
	else if (code == RPL_LUSERUNKNOWN) {								// 253
		args.push_back(this->howManyUnknown());
		args.push_back("unknown connection(s)");
	}
	else if (code == RPL_LUSERCHANNELS) {								// 254
		args.push_back(this->howManyChannel());
		args.push_back("channels formed");
	}
	else if (code == RPL_LUSERME)										// 251
		args.push_back("I have " + this->howManyDirectClient() + " clients and " + this->howManyDirectServer(false) + " servers");
	else if (code == RPL_ENDOFMOTD)										// 376
		args.push_back("End of MOTD command");
	else if (code == ERR_NOORIGIN)										// 409
		args.push_back("No origin specified");
	else if (code == ERR_NOTEXTTOSEND)									// 412
		args.push_back("No text to send");
	else if (code == ERR_NOMOTD)										// 422
		args.push_back("MOTD file is missing");
	else if (code == ERR_NONICKNAMEGIVEN)								// 431
		args.push_back("No nickname given");
	else if (code == ERR_NOTREGISTERED)									// 451
		args.push_back("You have not registered");
	else if (code == ERR_ALREADYREGISTERED)								// 462
		args.push_back("Unauthorized command (already registered)");
	else if (code == ERR_PASSWDMISMATCH)								// 464
		args.push_back("Wrong password");
	else
		return;
	sender->send(Command(this->hostname(), codeToStr(code), args));
}

void		LocalServer::numericReply(Connection *&sender, unsigned short code, std::string const &field1) {
	std::list<std::string> args;

	args.push_back(sender->name());
	if (code == ERR_DISCARDCOMMAND)
		return;
	else if (code == RPL_WELCOME)										// 001
		args.push_back("Welcome to the PixTillz\'s Internet Relay Network " + field1);
	else if (code == RPL_MOTD)											// 372
		args.push_back("- " + field1 + "-");
	else if (code == RPL_MOTDSTART)										// 375 
		args.push_back("- " + field1 + " Message of the day -");
	else if (code == ERR_NOSUCHNICK) {									// 401
		args.push_back(field1);
		args.push_back("No such nickname/channel");
	}
	else if (code == ERR_NOSUCHSERVER) {								// 402
		args.push_back(field1);
		args.push_back("No such server");
	}
	else if (code == ERR_NOSUCHCHANNEL) {								// 403
		args.push_back(field1);
		args.push_back("No such channel");
	}
	else if (code == ERR_NORECIPIENT)									// 411
		args.push_back("No recipient given (" + field1 + ")");
	else if (code == ERR_UNKNOWNCOMMAND) {								// 421
		args.push_back(field1);
		args.push_back("Unknown command");
	}
	else if (code == ERR_ERRONEOUSNICKNAME) {							// 432
		args.push_back(field1);
		args.push_back("Erroneous nickname");
	}
	else if (code == ERR_NICKNAMEINUSE) {								// 433
		args.push_back(field1);
		args.push_back("Nickname is already in use");
	}
	else if (code == ERR_WRONGPARAMSNUMBER){							// 461
		args.push_back(field1);
		args.push_back("Wrong number of parameters");
	}
	else
		return;
	sender->send(Command(this->hostname(), codeToStr(code), args));
	// else if (code == ERR_NOSUCHSERVER)
	// else if (code == ERR_NOSUCHCHANNEL)
	// else if (code == ERR_CANNOTSENDTOCHAN)
	// else if (code == ERR_TOOMANYCHANNELS)
}

std::string	const LocalServer::nbrToStr(unsigned int nbr) const { return static_cast<std::ostringstream*>(&(std::ostringstream() << nbr))->str(); }

std::string	const LocalServer::codeToStr(unsigned short code) const { 
	if (code < 10)
		return ("00" + nbrToStr(static_cast<unsigned int>(code)));
	else if (code < 100)
		return ("0" + nbrToStr(static_cast<unsigned int>(code)));
	else
		return nbrToStr(static_cast<unsigned int>(code));
}

// ########################################
// 				 EXECEPTIONS
// ########################################

// ########################################
// 					DEBUG
// ########################################

void		LocalServer::showLocalServer(void) const {
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	for (unsigned int i = 0; i < this->_sin.nbrInfo(); i++) {
		DEBUG_LDISPB(COUT, (this->_sin.familyX(i) == AF_INET ? "IPv4" : "IPv6"), this->_sin.addrStringX(i), '[');
		DEBUG_LDISPB(COUT, "Canonname: ", this->_sin.canonnameX(i), '[');
	}
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}

void		LocalServer::showNames(std::map<std::string, Connection *> names) const {
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	for (std::map<std::string, Connection *>::const_iterator it = names.begin(); it != names.end(); it++) {
		DEBUG_LDISPB(COUT, "\t->", it->first, '\'');
	}
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}

void		LocalServer::showNet(void) const {
	Connection *tmp;

	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	DEBUG_LDISPCB(COUT, "Unknown:  ", this->howManyUnknown(), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "D. Client:", this->howManyDirectClient(), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "Client:   ", this->howManyClient(), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "D. Server:", this->howManyDirectServer(true), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "Server:   ", this->howManyServer(true), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "Service:  ", this->howManyService(), PURPLE, '[');
	for (std::list<Connection *>::const_iterator it = this->_conxs.begin(); it != this->_conxs.end(); it++) {
		tmp = *it;
		if (tmp->isClient())
			DEBUG_LDISPCB(COUT, "Client  ~> ", *(static_cast<Client *>(tmp)), GREEN, '{');
		else
			DEBUG_LDISPCB(COUT, "Pending ~> ", *tmp, ORANGE, '{');
	}
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}
