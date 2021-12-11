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
**----- Updated -------------{ 2021-12-11 02:02:14 }--------------------------**
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
LocalServer::LocalServer(LocalServer const &src) :
	inherited(static_cast<inherited const &>(src)), _nicknames(src.getNicknames()),
	_usernames(src.getUsernames()), _realnames(src.getRealnames()), _conxs(src.getConxs()),
	_sm(src.getSM()), _password(src.getPassword()), _oppass(src.getOppass()) { return; }
LocalServer &LocalServer::operator=(LocalServer const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	_nicknames = src.getNicknames();
	_usernames = src.getUsernames();
	_realnames = src.getRealnames();
	_conxs = src.getConxs();
	_sm = src.getSM();
	_password = src.getPassword();
	_oppass = src.getOppass();
	return *this;
}

// _____________Constructor______________
LocalServer::LocalServer(std::string const &port, u_int16_t family, std::string password) :
Connection(port, family), _password(password), _sm(sock(), true) { return; }

// __________Member functions____________

bool		LocalServer::run(void) {
	DEBUG_DISPC(COUT, "\t+ Local server running +", DARK_PURPLE);
	while(!isFinished()) {
		selectCall();
		checkStd();
		checkSock();
		checkConxs();
	}
	return true;
}

void		LocalServer::newConx(void) {
	Connection *tmp;

	try {
		tmp = new Connection(sock());
		DEBUG_DISPC(COUT, "~> New Connection !", DARK_ORANGE); //write to log
	} catch (Connection::ConxInit &ex) {
		DEBUG_DISPC(COUT, "~> Failed to accept a Connection !", DARK_RED); //write to log
		tmp = nullptr;
	} if (tmp) {
		_conxs.push_back(tmp);
		_sm.addFd(_conxs.back()->sock());
	}
}

Channel		*LocalServer::newChan(Client *&sender, std::string const &name) {
	DEBUG_DISPC(COUT, "~> New Channel created !", DARK_ORANGE);
	_chans.push_back(new Channel(sender, name));
	return _chans.back();
}

void		LocalServer::finishConx(Connection *target, bool clear) {
	// DEBUG_DISPC(COUT, "~> Ending Connection !", DARK_RED);
	if (!target)
		return;
	target->isFinished(true);
	if (clear)
		target->clearMessages();
	else if (target->isClient()) {
		_nicknames.erase(static_cast<Client *>(target)->nickname);
		_usernames.erase(static_cast<Client *>(target)->username);
		_realnames.erase(static_cast<Client *>(target)->realname);
	}
}

void		LocalServer::finishChan(Channel *target) {
	if (!target)
		return;
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		if ((*it) == target) {
			_chans.erase(it);
			delete target;
			break;
		}
	}
}

void		LocalServer::selectCall(void) {
	_sm.call(_conxs); //check exception later and return bool in order to fix _sm
	// DEBUG_DISPC(COUT, "~> Select triggered !", DARK_PINK);
}

Client		*LocalServer::findClientByName(std::string const &name, unsigned char type) {
	clientnames::iterator it;

	if (name.empty())
		return (nullptr);
	if (type & NICKNAME) {
		if ((it = _nicknames.find(name)) != _nicknames.end())
			return (static_cast<Client *>(it->second));
	}
	if (type & USERNAME) {
		if ((it = _usernames.find(name)) != _usernames.end())
			return (static_cast<Client *>(it->second));
	}
	if (type & REALNAME) {
		if ((it = _realnames.find(name)) != _realnames.end())
			return (static_cast<Client *>(it->second));
	}
	return (nullptr);
}

Channel		*LocalServer::findChannelByName(std::string const &name) {
	if (name.empty())
		return (nullptr);
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		if ((*it)->checkName(name))
			return (*it);
	}
	return (nullptr);
}

unsigned int		LocalServer::howMany(unsigned short check) const {
	unsigned int	count = 0;

	for (conxlist_cit it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isClient()) {
			if (static_cast<Client *>(*it)->checkMode(check))
				count++;
		}
	}
	return count;
}

unsigned int		LocalServer::howMany(unsigned short check, bool direct) const {
	unsigned int	count = 0;

	for (conxlist_cit it = _conxs.begin(); it != _conxs.end(); it++) {
		if (direct && (*it)->isLink())
			continue;
		if (check == CONX_PENDING) {
			if ((*it)->isPending())
				count++;
		} else {
			if ((*it)->checkStatus(check))
				count++;
		}
	}
	return count;
}

std::string const 	LocalServer::howManyClient(bool direct) const { return nbrToStr(howMany(CONX_CLIENT, direct)); }
std::string const 	LocalServer::howManyServer(bool direct, bool self) const { return nbrToStr(howMany(CONX_SERVER, direct) + (self ? 1 : 0)); }
std::string const 	LocalServer::howManyService(bool direct) const { return nbrToStr(howMany(CONX_SERVICE, direct)); }
std::string const 	LocalServer::howManyUnknown(void) const { return nbrToStr(howMany(CONX_PENDING, false)); }
std::string const 	LocalServer::howManyOperator(void) const { return nbrToStr(howMany(CLIENT_OPERATOR)); }
std::string const 	LocalServer::howManyChannel(void) const { return nbrToStr(_chans.size()); }

// ########################################
// 				   PRIVATE
// ########################################

bool		LocalServer::isReadable(Connection *conx) const { return _sm.checkRfds(conx->sock()); }
bool		LocalServer::isWritable(Connection *conx) const { return _sm.checkWfds(conx->sock()); }

void		LocalServer::checkStd(void) {
	std::string input;

	if (_sm.checkStd()) {
		std::getline(std::cin, input);
		DEBUG_LDISPCB(COUT, "Input: ", input, PINK, '[');
		if (!input.compare("EXIT") || !input.compare("Exit") || !input.compare("exit"))
			isFinished(true);
		if (!input.compare("NET") || !input.compare("Net") || !input.compare("net"))
			showNet();
		if (!input.compare("LOCAL") || !input.compare("Local") || !input.compare("local"))
			showLocalServer();
		if (!input.compare("NICKS") || !input.compare("Nicks") || !input.compare("nicks"))
			showNames(_nicknames);
		if (!input.compare("USERS") || !input.compare("Users") || !input.compare("users"))
			showNames(_usernames);
		if (!input.compare("REALNAMES") || !input.compare("Realnames") || !input.compare("realnames"))
			showNames(_realnames);
		if (!input.compare("CHANS") || !input.compare("Chans") || !input.compare("chans"))
			showChans();
	}
}

void		LocalServer::checkSock(void) {
	if (isReadable(static_cast<Connection *>(this)))
		newConx();
}

void		LocalServer::checkConxs(void) {
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if (isReadable(*it)) {
			if (!(*it)->read()) {
				DEBUG_DISPC(COUT, "~> Someone left !", RED);
				finishConx(*it, true);
				continue;
			}
		}
		while ((*it)->hasInputMessage())
			execCommand(*it, (*it)->getLastCommand());
		if (isWritable(*it)) {
			try {
				(*it)->write();
			} catch (SockStream::SendFunctionException &ex) {
				DEBUG_DISPC(COUT, "~> PIPE may have broke with a connection !", RED);
				finishConx(*it, true);
			}	 
		}
	}
}

// Execution module
void		LocalServer::execCommand(Connection *&sender, Command *cmd) {
	Client	*tmp;
	if (!cmd)
		return;
	// std::cout << *cmd << std::endl;
	try {
		if (sender->isPending())
			execCommandPending(sender, *cmd);
		else if (sender->isClient()) {
			tmp = static_cast<Client *>(sender);
			if (!tmp->isRegistered()) {
				if (*cmd == "NICK")
					execNick(sender, NickCommand(*cmd));
				else if (*cmd == "USER")
					execUser(sender, UserCommand(*cmd));
				else
					numericReply(sender, ERR_NOTREGISTERED); 
			}
			else if (*cmd == "NICK")
				execNick(sender, NickCommand(*cmd));
			else if (*cmd == "USER")
				execUser(sender, UserCommand(*cmd));
			else
				execCommandClient(tmp, *cmd);
		}
	}
	catch (Command::InvalidCommandException &e) {
		if (e.code == ERR_WRONGPARAMSNUMBER || e.code == ERR_NORECIPIENT)
			numericReply(sender, e.code, cmd->command);
		else if (e.code == ERR_CANNOTSENDTOCHAN)
			numericReply(sender, e.code, cmd->argX(0));
		else if (e.code == ERR_CHANOPRIVSNEEDED)
			numericReply(sender, e.code, *cmd);
		else if (e.code == ERR_UNKNOWNMODE)
			numericReply(sender, e.code, *cmd);
		else
			numericReply(sender, e.code);
		if (!sender->isAuthentified())
			sender->isFinished(true);
	}
	delete cmd;
}

void		LocalServer::execCommandPending(Connection *&sender, Command const &cmd) {
	if (!sender->isAuthentified()) {
		if (cmd == "CAP")
			numericReply(sender, ERR_DISCARDCOMMAND);
		else if (cmd == "PASS")
			execPass(sender, PassCommand(cmd));
		else {
			numericReply(sender, ERR_NOTREGISTERED);
			finishConx(sender, false);
		}
	}
	else {
		if (cmd == "CAP")
			numericReply(sender, ERR_DISCARDCOMMAND);
		else if (cmd == "NICK")
			execNick(sender, NickCommand(cmd));
		else if (cmd == "USER")
			execUser(sender, UserCommand(cmd));
		else
			numericReply(sender, ERR_NOTREGISTERED);
		// else if (cmd == "SERVER")
		// 	execServer(sender, ServerCommand(cmd));
	}
}

void		LocalServer::execCommandClient(Client *&sender, Command const &cmd) {
	if (cmd == "QUIT")
		execQuit(sender, QuitCommand(cmd));
	else if (cmd == "PRIVMSG")
		execPrivmsg(sender, PrivmsgCommand(cmd));
	else if (cmd == "NOTICE")
		execNotice(sender, NoticeCommand(cmd));
	else if (cmd == "PING")
		execPing(sender, PingCommand(cmd));
	else if (cmd == "PONG")
		execPong(sender, PongCommand(cmd));
	else if (cmd == "MOTD")
		execMotd(sender, MotdCommand(cmd));
	else if (cmd == "LUSERS")
		execLusers(sender, LusersCommand(cmd));
	else if (cmd == "WHOIS")
		execWhois(sender, WhoisCommand(cmd));
	else if (cmd == "PASS")
		numericReply(sender, ERR_ALREADYREGISTERED);
	else if (cmd == "JOIN")
		execJoin(sender, JoinCommand(cmd));
	else if (cmd == "PART")
		execPart(sender, PartCommand(cmd));
	else if (cmd == "TOPIC")
		execTopic(sender, TopicCommand(cmd));
	else if (cmd == "OPPASS")
		execOppass(sender, OppassCommand(cmd));
	else if (cmd == "OPER")
		execOper(sender, OperCommand(cmd));
	else if (cmd == "MODE")
		execMode(sender, ModeCommand(cmd));
	else if (cmd == "AWAY")
		execAway(sender, AwayCommand(cmd));
	else if (cmd == "NAMES")
		execNames(sender, NamesCommand(cmd));
	else if (cmd == "LIST")
		execList(sender, ListCommand(cmd));
	else if (cmd == "WHO")
		execWho(sender, WhoCommand(cmd));
	else
		numericReply(sender, ERR_UNKNOWNCOMMAND, cmd.command);
}

// void		LocalServer::execCommandServer(Connection *&sender, Command *cmd) {
	
// }

// void		LocalServer::execCommandService(Connection *&sender, Command *cmd) {
	
// }

void		LocalServer::execPass(Connection *&sender, PassCommand const &cmd) {
	cmd.isValid();
	if (!sender->isAuthentified()) {
		if (cmd.password() == _password)
			sender->isAuthentified(true);
		else {
			std::cout << "Password: [" << cmd.password() << "] -> [" << _password << "]" << std::endl;
			numericReply(sender, ERR_PASSWDMISMATCH);
			finishConx(sender, false);
		}
	}
	else
		numericReply(sender, ERR_ALREADYREGISTERED);
}

void		LocalServer::execNick(Connection *&sender, NickCommand const &cmd) {
	Client *tmp;

	cmd.isValid();
	if (_nicknames.count(cmd.nickname()))
		numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	if (sender->isClient()) {
		tmp = static_cast<Client *>(sender);
		if (!tmp->isRegistered()) {
			if (tmp->nickname.empty()) {
				tmp->nickname = cmd.nickname();
				numericReply(sender, RPL_WELCOME, tmp->fullId());
			}
			else {
				_nicknames.erase(tmp->nickname);
				tmp->nickname = cmd.nickname();
			}
			_nicknames[tmp->nickname] = sender;
		}
		else {
			updateChans(tmp, cmd);
			_nicknames.erase(tmp->nickname);
			tmp->nickname = cmd.nickname();
			_nicknames[tmp->nickname] = sender;
		}
	}
	else if (sender->isPending()) {
		try {
			new Client(sender, cmd);
			if (!howMany(CONX_CLIENT, true))
				static_cast<Client *>(sender)->isLocalop(true);
			_nicknames[static_cast<Client *>(sender)->nickname] = sender;
		} catch (std::exception &ex) {
			throw; // write log and find a good exception to throw
		}
	}
}

void		LocalServer::execUser(Connection *&sender, UserCommand const &cmd) {
	Client *tmp;

	cmd.isValid();
	if (sender->isClient()) {
		tmp = static_cast<Client *>(sender);
		if (!tmp->isRegistered()) {
			if (tmp->username.empty()) {
				tmp->username = cmd.username();
				tmp->realname = cmd.realname();
				numericReply(sender, RPL_WELCOME, tmp->fullId());
			}
			else {
				_usernames.erase(tmp->username);
				tmp->username = cmd.username();
				_realnames.erase(tmp->realname);
				tmp->realname = cmd.realname();
			}
			// tmp->setModes(cmd.modes()[0] % 48); //temporary setup
			_usernames[tmp->username] = sender;
			_realnames[tmp->realname] = sender;
		}
		else
			numericReply(sender, ERR_ALREADYREGISTERED);
	}
	else if (sender->isPending()) {
		try {
			new Client(sender, cmd);
			if (!howMany(CONX_CLIENT, true))
				static_cast<Client *>(sender)->isLocalop(true);
			_usernames[static_cast<Client *>(sender)->username] = sender;
			_realnames[static_cast<Client *>(sender)->realname] = sender;
		} catch (std::exception &ex) {
			throw; // write log and find a good exception to throw
		}
	}
}

void		LocalServer::execPrivmsg(Client *&sender, PrivmsgCommand const &cmd) {
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if ((tmp = findClientByName(cmd.target(), CLIENTNAME))) {
		if (tmp->isAway())
			numericReply(sender, RPL_AWAY, tmp);
		else
			tmp->send(PrivmsgCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	}
	else if ((chan = findChannelByName(cmd.target())))
		chan->send(static_cast<Client *>(sender), PrivmsgCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	else
		numericReply(sender, ERR_NOSUCHNICK, cmd.target());
}

void		LocalServer::execNotice(Client *&sender, NoticeCommand const &cmd) {
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if ((tmp = findClientByName(cmd.target(), CLIENTNAME))) //change 'getNickname' to 'fullId' if needed later on
		tmp->send(NoticeCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	else if ((chan = findChannelByName(cmd.target())))
		chan->send(static_cast<Client *>(sender), NoticeCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	else
		numericReply(sender, ERR_DISCARDCOMMAND, cmd.target());
}

void		LocalServer::execQuit(Client *&sender, QuitCommand const &cmd) {
	if (sender->isClient()) {
		updateChans(sender, cmd);
		sender->send(ErrorCommand("Closing connection"));
		finishConx(sender, false);
	}
}

void		LocalServer::execPing(Client *&sender, PingCommand const &cmd) { cmd.isValid(); sender->send(PongCommand(hostname())); } //later change hostname for servername
void		LocalServer::execPong(Client *&sender, PongCommand const &cmd) { cmd.isValid(); } //command is discarded
void		LocalServer::execMotd(Client *&sender, MotdCommand const &cmd) {
	std::ifstream file("motd.txt");
	std::string input;
	std::string tmp;

	cmd.isValid();
	// if (!cmd.target().empty()) { //not mandatory
	// 	file.close();
	// 	share()
	// }
	if (file.fail()) {
		file.close();
		numericReply(sender, ERR_NOMOTD);
	}
	else {
		numericReply(sender, RPL_MOTDSTART, hostname());
		while (!file.eof()) {
			std::getline(file, input);
			while (!input.empty()) {
				if (input.length() < MAX_MOTD_READ)
					input.append(std::string(MAX_MOTD_READ - input.length(), ' '));
				tmp = input.substr(0, MAX_MOTD_READ);
				input.erase(0, MAX_MOTD_READ);
				numericReply(sender, RPL_MOTD, tmp);
			}
		}
		numericReply(sender, RPL_ENDOFMOTD);
		file.close();
	}
}

void		LocalServer::execLusers(Client *&sender, LusersCommand const &cmd) {
	cmd.isValid();

	numericReply(sender, RPL_LUSERCLIENT);
	numericReply(sender, RPL_LUSEROP);
	numericReply(sender, RPL_LUSERUNKNOWN);
	numericReply(sender, RPL_LUSERCHANNELS);
	numericReply(sender, RPL_LUSERME);
}

void		LocalServer::execWhois(Client *&sender, WhoisCommand const &cmd) {
	Client	*tmp;

	cmd.isValid();
	if (!(tmp = findClientByName(cmd.target(), ALLNAME)))
		numericReply(sender, ERR_NOSUCHNICK, cmd.target());
	else {
		if (!tmp->isRegistered())
			numericReply(sender, ERR_NOSUCHNICK, cmd.target());
		numericReply(sender, RPL_WHOISUSER, tmp);
		if (tmp->hasChans())
			numericReplyWhoischannel(sender, tmp, tmp->chans);
		numericReply(sender, RPL_WHOISSERVER, tmp);
		if (tmp->isAway())
			numericReply(sender, RPL_AWAY, tmp);
		if (tmp->isOperator())
			numericReply(sender, RPL_WHOISOPERATOR, cmd.target());
		numericReply(sender, RPL_ENDOFWHOIS, cmd.target());
	}
}

void		LocalServer::execJoin(Client *&sender, JoinCommand const &cmd) {
	unsigned short code;
	Channel *chan;

	cmd.isValid();
	if (cmd.target()[0] != '#' && cmd.target()[0] != '&')
		numericReply(sender, ERR_NOSUCHCHANNEL, cmd.target());
	else {
		if (!(chan = findChannelByName(cmd.target())))
			chan = newChan(sender, cmd.target());
		else if ((code = chan->join(static_cast<Client *>(sender), cmd))) { // add key arg later for -k mode
			throw (Command::InvalidCommandException(ERR_DISCARDCOMMAND));
		}
		if (chan->hasTopic())
			numericReply(sender, RPL_TOPIC, chan);
		execNames(sender, NamesCommand(NO_PREFIX, chan->getName()));
		// else {
		// 	// other errors.
		// }
	}
}

void		LocalServer::execPart(Client *&sender, PartCommand const &cmd) {
	Channel	*chan;

	cmd.isValid();
	if (!(chan = findChannelByName(cmd.target())))
		numericReply(sender, ERR_NOSUCHCHANNEL, cmd.target());
	else if (!chan->isOnChan(sender))
		numericReply(sender, ERR_NOTONCHANNEL, cmd.target());
	else {
		chan->updateClients(sender, cmd);
		if (chan->empty())
			finishChan(chan);
	}
}

void		LocalServer::execTopic(Client *&sender, TopicCommand const &cmd) {
	Channel	*chan;

	cmd.isValid();
	if (!(chan = findChannelByName(cmd.target())))
		numericReply(sender, ERR_NOSUCHCHANNEL, cmd.target());
	else if (!chan->isOnChan(static_cast<Client *>(sender)))
		numericReply(sender, ERR_NOTONCHANNEL, cmd.target());
	else {
		if (cmd.argNbr() == 2) {
			if (chan->isTopic() && !chan->isOperator(static_cast<Client *>(sender)))
				numericReply(sender, ERR_CHANOPRIVSNEEDED, cmd);
			else
				chan->setTopic(cmd.topic());
		}
		else if (chan->hasTopic())
			numericReply(sender, RPL_TOPIC, chan);
		else
			numericReply(sender, RPL_NOTOPIC, chan->getName());
	}
}

void		LocalServer::execOppass(Client *&sender, OppassCommand const &cmd) {
	if (static_cast<Client *>(sender)->isLocalop()) {
		cmd.isValid();
		setOppass(cmd.password());
	}
	else
		numericReply(sender, ERR_NOTLOCALOP);
}

void		LocalServer::execOper(Client *&sender, OperCommand const &cmd) {
	cmd.isValid();
	if (!cmd.password().compare(_oppass)) {
		if (static_cast<Client *>(sender)->isOperator())
			return;
		static_cast<Client *>(sender)->isOperator(true);
		numericReply(sender, RPL_YOUREOP);
	}
	else
		numericReply(sender, ERR_PASSWDMISMATCH);
}

void		LocalServer::execMode(Client *&sender, ModeCommand const &cmd) {
	Channel		*chan;
	Client		*target;
	std::string	mode;

	cmd.isValid();
	if (cmd.argNbr() == 1) {
		if (!(chan = findChannelByName(cmd.field1())))
			throw (Command::InvalidCommandException(ERR_NOSUCHCHANNEL));
		numericReply(sender, RPL_CHANNELMODEIS, chan);
	}
	else if (cmd.argNbr() == 2) {
		if (cmd.field1()[0] == '#' || cmd.field1()[0] == '&') {
			if (!(chan = findChannelByName(cmd.field1())))
				throw (Command::InvalidCommandException(ERR_NOSUCHCHANNEL));
			mode = cmd.field2();
			if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
				throw (Command::InvalidCommandException(ERR_UMODEUNKNOWNFLAG));
			chan->applyModeFlag(static_cast<Client *>(sender), mode[1], (mode[0] == '+' ? true : false));
		}
		else {
			if (!(target = findClientByName(cmd.field1(), NICKNAME)))
				throw (Command::InvalidCommandException(ERR_USERSDONTMATCH));
			if (target->nickname.compare(static_cast<Client *>(sender)->nickname))
				throw (Command::InvalidCommandException(ERR_USERSDONTMATCH));
			mode = cmd.field2();
			if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
				throw (Command::InvalidCommandException(ERR_UMODEUNKNOWNFLAG));
			target->applyModeFlag(mode[1], (mode[0] == '+' ? true : false));
			target->send(Command(target->fullId(), "MODE", cmd.args));
		}
	}
	else {
		if (!(chan = findChannelByName(cmd.field1())))
			throw (Command::InvalidCommandException(ERR_NOSUCHCHANNEL));
		mode = cmd.field2();
		if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
			throw (Command::InvalidCommandException(ERR_UNKNOWNMODE));
		if (!(target = findClientByName(cmd.field3(), NICKNAME)))
			throw (Command::InvalidCommandException(ERR_USERNOTINCHANNEL));
		else if (!chan->isOnChan(target))
			throw (Command::InvalidCommandException(ERR_USERNOTINCHANNEL));
		chan->applyUserModeFlag(static_cast<Client *>(sender), target, mode[1], (mode[0] == '+' ? true : false));
		chan->send(nullptr, ModeCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	}
}

void		LocalServer::execAway(Client *&sender, AwayCommand const &cmd) {
	cmd.isValid();
	if (cmd.argNbr(1)) {
		if (cmd.message().empty()) {
			static_cast<Client *>(sender)->isAway(false);
			numericReply(sender, RPL_UNAWAY);
		}
		else {
			static_cast<Client *>(sender)->isAway(true);
			static_cast<Client *>(sender)->awaymsg = cmd.message();
			numericReply(sender, RPL_NOWAWAY);
		}
	}
	else {
		static_cast<Client *>(sender)->isAway(false);
		numericReply(sender, RPL_UNAWAY);
	}
}

void		LocalServer::execNames(Client *&sender, NamesCommand const &cmd) {
	std::list<std::string> lst;
	Channel *chan;

	cmd.isValid();
	if (cmd.argNbr(0)) {
		if (!_chans.empty()) {
			std::cout << _chans.size() << std::endl;
			for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
				(*it)->namesList(lst);
				if (!lst.empty())
					numericReplyNames(sender, (*it)->getName(), lst);
				numericReply(sender, RPL_ENDOFNAMES, (*it)->getName());
			}
		}
		namesListAloneClient(lst);
		if (!lst.empty()) {
			numericReplyNames(sender, "*", lst);
			numericReply(sender, RPL_ENDOFNAMES, "*");
		}
	}
	else {
		if ((chan = findChannelByName(cmd.target()))) {
			chan->namesList(lst);
			if (!lst.empty())
				numericReplyNames(sender, chan->getName(), lst);
		}
		numericReply(sender, RPL_ENDOFNAMES, cmd.target());
	}
}

void		LocalServer::execList(Client *&sender, ListCommand const &cmd) {
	Channel *chan;

	cmd.isValid();
	if (cmd.argNbr(0)) {
		if (!_chans.empty()) {
			for (chanlist_it it = _chans.begin(); it != _chans.end(); it++)
				numericReply(sender, RPL_LIST, (*it));
		}
		numericReply(sender, RPL_LISTEND);
	}
	else {
		if ((chan = findChannelByName(cmd.target())))
			numericReply(sender, RPL_LIST, chan);
		numericReply(sender, RPL_LISTEND);
	}
}

void		LocalServer::execWho(Client *&sender, WhoCommand const &cmd) {
	std::list<Client *> lst;
	Channel *chan;
	Client	*tmp;

	cmd.isValid();
	if (cmd.argNbr(0) || !cmd.argX(0).compare("0") || !cmd.argX(0).compare("*")) {
		for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
			if ((*it)->isClient())
				numericReplyWho(sender, nullptr, static_cast<Client *>(*it));
		}
		numericReply(sender, RPL_ENDOFWHO, "*");
	}
	else {
		if ((chan = findChannelByName(cmd.mask()))) {
			chan->clientsList(lst);
			for (std::list<Client *>::iterator it = lst.begin(); it != lst.end(); it++)
				numericReplyWho(sender, chan, (*it));
		}
		numericReply(sender, RPL_ENDOFWHO, cmd.mask());
	}
}

void		LocalServer::numericReply(Connection *sender, unsigned short code) {
	Command ret(hostname(), codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	if (code == ERR_DISCARDCOMMAND)
		return;
	else if (code == RPL_LUSERCLIENT)									// 251
		ret.addArg("There are " + howManyClient(false) + " users and " + howManyService(false) + " services on " + howManyServer(false, true) + " servers");
	else if (code == RPL_LUSEROP) {										// 252
		ret.addArg(howManyOperator());
		ret.addArg("operator(s) online");
	}
	else if (code == RPL_LUSERUNKNOWN) {								// 253
		ret.addArg(howManyUnknown());
		ret.addArg("unknown connection(s)");
	}
	else if (code == RPL_LUSERCHANNELS) {								// 254
		ret.addArg(howManyChannel());
		ret.addArg("channels formed");
	}
	else if (code == RPL_LUSERME)										// 251
		ret.addArg("I have " + howManyClient(true) + " clients and " + howManyServer(true, false) + " servers");
	else if (code == RPL_UNAWAY)										// 305
		ret.addArg("You are no longer marked as being away");
	else if (code == RPL_NOWAWAY)										// 306
		ret.addArg("You have been marked as being away");
	else if (code == RPL_LISTEND)										// 323
		ret.addArg("End of list");
	else if (code == RPL_ENDOFMOTD)										// 376
		ret.addArg("End of MOTD command");
	else if (code == RPL_YOUREOP)										// 381
		ret.addArg("You are now an IRC operator");
	else if (code == ERR_NOORIGIN)										// 409
		ret.addArg("No origin specified");
	else if (code == ERR_NOTEXTTOSEND)									// 412
		ret.addArg("No text to send");
	else if (code == ERR_NOMOTD)										// 422
		ret.addArg("MOTD file is missing");
	else if (code == ERR_NONICKNAMEGIVEN)								// 431
		ret.addArg("No nickname given");
	else if (code == ERR_NOTREGISTERED)									// 451
		ret.addArg("You have not registered");
	else if (code == ERR_ALREADYREGISTERED)								// 462
		ret.addArg("Unauthorized command (already registered)");
	else if (code == ERR_PASSWDMISMATCH)								// 464
		ret.addArg("Wrong password");
	else if (code == ERR_NOTLOCALOP)									// 486
		ret.addArg("You\'re not a local operator");
	else if (code == ERR_UMODEUNKNOWNFLAG)								// 501
		ret.addArg("Unknown MODE flag");
	else if (code == ERR_USERSDONTMATCH)								// 502
		ret.addArg("Cannot change mode for other users");
	else
		return;
	sender->send(ret);
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, std::string const &field) {
	Command ret(hostname(), codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	if (code == ERR_DISCARDCOMMAND)
		return;
	else if (code == RPL_WELCOME)										// 001
		ret.addArg("Welcome to the PixTillz\'s Internet Relay Network " + field);
	else if (code == RPL_WHOISOPERATOR) {								// 313
		ret.addArg(field);
		ret.addArg("is an IRC operator");
	}
	else if (code == RPL_ENDOFWHO) {									// 315
		ret.addArg(field);
		ret.addArg("End of WHO list");
	}
	else if (code == RPL_ENDOFWHOIS) {									// 318
		ret.addArg(field);
		ret.addArg("End of WHOIS list");
	}
	else if (code == RPL_NOTOPIC) {										// 331
		ret.addArg(field);
		ret.addArg("No topic is set");
	}
	else if (code == RPL_ENDOFNAMES) {									// 366
		ret.addArg(field);
		ret.addArg("End of NAMES list");
	}
	else if (code == RPL_MOTD)											// 372
		ret.addArg("- " + field + "-");
	else if (code == RPL_MOTDSTART)										// 375 
		ret.addArg("- " + field + " Message of the day -");
	else if (code == ERR_NOSUCHNICK) {									// 401
		ret.addArg(field);
		ret.addArg("No such nickname/channel");
	}
	else if (code == ERR_NOSUCHSERVER) {								// 402
		ret.addArg(field);
		ret.addArg("No such server");
	}
	else if (code == ERR_NOSUCHCHANNEL) {								// 403
		ret.addArg(field);
		ret.addArg("No such channel");
	}
	else if (code == ERR_CANNOTSENDTOCHAN) {							// 404
		ret.addArg(field);
		ret.addArg("Cannot send to channel");
	}
	else if (code == ERR_NORECIPIENT)									// 411
		ret.addArg("No recipient given (" + field + ")");
	else if (code == ERR_UNKNOWNCOMMAND) {								// 421
		ret.addArg(field);
		ret.addArg("Unknown command");
	}
	else if (code == ERR_ERRONEOUSNICKNAME) {							// 432
		ret.addArg(field);
		ret.addArg("Erroneous nickname");
	}
	else if (code == ERR_NICKNAMEINUSE) {								// 433
		ret.addArg(field);
		ret.addArg("Nickname is already in use");
	}
	else if (code == ERR_NOTONCHANNEL) {								// 442
		ret.addArg(field);
		ret.addArg("You\'re not on that channel");
	}
	else if (code == ERR_WRONGPARAMSNUMBER){							// 461
		ret.addArg(field);
		ret.addArg("Wrong number of parameters");
	}
	else
		return;
	sender->send(ret);
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, Command const &cmd) {
	Command ret(hostname(), codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	if (code == ERR_UNKNOWNMODE) {										// 472
		ret.addArg((cmd.argX(1).size() < 2 ? "None" : &(cmd.argX(1)[1])));
		ret.addArg("is unknown mode char to me for " + cmd.argX(0));
	}
	else if (code == ERR_CHANOPRIVSNEEDED) {							// 482
		ret.addArg((cmd == "INVITE" ? cmd.argX(1) : cmd.argX(0)));
		ret.addArg("You\'re not a channel operator");
	}
	else if (code == ERR_USERNOTINCHANNEL){
		if (cmd == "MODE") {
			ret.addArg(cmd.argX(2));
			ret.addArg(cmd.argX(0));
			ret.addArg("They aren\'t on that channel");
		}
	}
	else
		return;
	sender->send(ret);	
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, Channel *chan) {
	Command ret(hostname(), codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	if (code == RPL_TOPIC) {
		ret.addArg(chan->getName());
		ret.addArg(chan->getTopic());
	}
	else if (code == RPL_CHANNELMODEIS) {
		ret.addArg(chan->getName());
		ret.addArg(chan->getModesFlags());
	}
	else if (code == RPL_LIST) {
		ret.addArg(chan->getName());
		ret.addArg(nbrToStr(chan->size()));
		if (chan->hasTopic())
			ret.addArg(chan->getTopic());
	}
	else
		return;
	sender->send(ret);
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, Client *target) {
	Command ret(hostname(), codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	if (code == RPL_WHOISUSER) {										// 311
		ret.addArg(target->nickname);
		ret.addArg(target->username);
		ret.addArg(target->hostname());
		ret.addArg("*");
		ret.addArg(target->realname);
	}
	else if (code == RPL_WHOISSERVER) {									// 312
		ret.addArg(target->nickname);
		if (!target->isLink()) //else get link hostname
			ret.addArg(hostname());
		ret.addArg("Active on this server"); // change it to whatever information about the client later
	}
	else if (code == RPL_AWAY) {
		ret.addArg(target->nickname);
		ret.addArg(target->awaymsg);
	}
	sender->send(ret);
}

void		LocalServer::numericReplyNames(Client *&sender, std::string const &chan, std::list<std::string> &names) {
	Command::arglist_it itnames;
	std::string			tmp;

	while (!names.empty()) {
		Command ret(Command(hostname(), codeToStr(RPL_NAMESREPLY), Command::arglist()));
		itnames = names.begin();
		ret.addArg(sender->nickname);
		ret.addArg("=");
		ret.addArg(chan);
		std::advance(itnames, (names.size() < MAX_RPL_ENTITY ? names.size() : MAX_RPL_ENTITY));
		while (names.begin() != itnames) {
			tmp.append(names.front());
			names.pop_front();
			if (names.begin() != itnames)
				tmp.append(" ");
		}
		ret.addArg(tmp);
		sender->send(ret);
	}
}

void		LocalServer::numericReplyWho(Client *&sender, Channel *chan, Client *target) {
	Command ret(hostname(), codeToStr(RPL_WHOREPLY), Command::arglist());

	ret.addArg(sender->nickname);
	ret.addArg((chan ? chan->getName() : "*"));
	ret.addArg(target->username);
	ret.addArg(target->hostname());
	ret.addArg(hostname());
	ret.addArg(target->nickname);
	ret.addArg((target->isAway() ? "G" : "H"));
	if (target->isOperator())
		ret.addArg("*");
	if (chan) {
		if (chan->isOperator(target))
			ret.addArg("@");
		else if (chan->isModerated() && chan->canTalk(target))
			ret.addArg("+");
	}
	ret.addArg("0 " + target->realname);
	sender->send(ret);
}

void		LocalServer::numericReplyWhoischannel(Client *&sender, Client *target, std::list<std::string> chans) {
	std::ostringstream str;
	unsigned int count;
	Channel *chan;

	count = 0;
	str.str(":");
	while (!chans.empty()) {
		Command ret(hostname(), codeToStr(RPL_WHOISCHANNELS), Command::arglist());
		if (!(chan = findChannelByName(chans.front())))
			chans.pop_front();
		else {
			count++;
			chans.pop_front();
			if (chan->isOperator(target))
				str << "@";
			else if (chan->isModerated() && chan->canTalk(target))
				str << "+";
			str << chan->getName();
			if (count != MAX_RPL_ENTITY && !chans.empty())
				str << " ";
			else {
				ret.addArg(sender->fullId());
				ret.addArg(target->nickname);
				ret.addArg(str.str());
				sender->send(ret);
				str.str(":");
			}
		}
	}
}

void		LocalServer::updateChans(Client *sender, Command const &cmd) {
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		(*it)->updateClients(sender, cmd);
		if ((*it)->empty())
			finishChan(*(it--));
	}
}

std::string	const LocalServer::nbrToStr(unsigned int nbr) const { return static_cast<std::ostringstream *>(&(std::ostringstream() << nbr))->str(); }

std::string	const LocalServer::codeToStr(unsigned short code) const { 
	if (code < 10)
		return ("00" + nbrToStr(static_cast<unsigned int>(code)));
	else if (code < 100)
		return ("0" + nbrToStr(static_cast<unsigned int>(code)));
	else
		return nbrToStr(static_cast<unsigned int>(code));
}

void		LocalServer::namesListAloneClient(std::list<std::string> &lst) const {
	lst.clear();
	for (conxlist_cit it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isClient() && !static_cast<Client *>(*it)->hasChans())
			lst.push_back(static_cast<Client *>(*it)->nickname);
	}
}

// ########################################
// 				 EXECEPTIONS
// ########################################

// ########################################
// 					DEBUG
// ########################################

void		LocalServer::showLocalServer(void) const {
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	for (unsigned int i = 0; i < info.size(); i++) {
		DEBUG_LDISPB(COUT, (info.family(i) == AF_INET ? "IPv4" : "IPv6"), info.addr(i), '[');
		DEBUG_LDISPB(COUT, "Canonname: ", info.canonname(i), '[');
	}
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}

void		LocalServer::showNames(clientnames names) const {
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	for (clientnames::const_iterator it = names.begin(); it != names.end(); it++) {
		DEBUG_LDISPB(COUT, "\t->", it->first, '\'');
	}
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}

void		LocalServer::showChans(void) const {
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	for (chanlist::const_iterator it = _chans.begin(); it != _chans.end(); it++) {
		DEBUG_DDISPC(COUT, "\t ->", *(*it), DARK_GREEN);
	}
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}

void		LocalServer::showNet(void) const {
	Connection *tmp;

	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	DEBUG_LDISPCB(COUT, "Unknown:  ", howManyUnknown(), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "D. Client:", howManyClient(true), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "Client:   ", howManyClient(false), PURPLE, '[');
	DEBUG_ADISPCB(COUT, "D. Server:", howManyServer(true, false), " + 1 (this)", PURPLE, '[');
	DEBUG_ADISPCB(COUT, "Server:   ", howManyServer(false, false), " + 1 (this)", PURPLE, '[');
	DEBUG_LDISPCB(COUT, "Service:  ", howManyService(false), PURPLE, '[');
	DEBUG_LDISPCB(COUT, "Chans:    ", howManyChannel(), PURPLE, '[');
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	if (_conxs.empty())
		DEBUG_DISPCB(COUT, " No connection yet ", DARK_GREY, '~');
	else {
		for (conxlist_cit it = _conxs.begin(); it != _conxs.end(); it++) {
			tmp = *it;
			if (tmp->isClient())
				DEBUG_LDISPCB(COUT, "Client  ~> ", *(static_cast<Client *>(tmp)), GREEN, '{');
			else
				DEBUG_LDISPCB(COUT, "Pending ~> ", *tmp, ORANGE, '{');
		}
	}
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}
