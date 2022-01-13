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
**----- Updated -------------{ 2022-01-13 04:01:57 }--------------------------**
********************************************************************************
*/

#include "LocalServer.hpp"
#include "Debug.hpp"
#include "Utils.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
LocalServer::~LocalServer() {
	_logfile.append(LOG_DATE, LOG_DARKPURPLE, "\t\t+ Server closed successfuly +");
	_logfile.append(LOG_DATE, LOG_DARKPURPLE, " ###############################################");
	return;
}
// LocalServer::LocalServer() { return; } /* Private standard constructor*/
LocalServer::LocalServer(LocalServer const &src) :
	inherited(static_cast<inherited const &>(src)), _nicknames(src.getNicknames()),
	_usernames(src.getUsernames()), _realnames(src.getRealnames()), _conxs(src.getConxs()),
	_sm(src.getSM()), _password(src.getPassword()), _oppass(src.getOppass()),
	_logfile(src.getLogfile()), _whitelist(src.getWhitelist()) { return; }
LocalServer &LocalServer::operator=(LocalServer const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	_nicknames = src.getNicknames();
	_usernames = src.getUsernames();
	_realnames = src.getRealnames();
	_servnames = src.getServnames();
	_conxs = src.getConxs();
	_sm = src.getSM();
	_password = src.getPassword();
	_oppass = src.getOppass();
	_logfile = src.getLogfile();
	_whitelist = src.getWhitelist();
	return *this;
}

// _____________Constructor______________
LocalServer::LocalServer(std::string const &port, u_int16_t family, std::string const &password) :
inherited("", port, family), _password(password), _sm(sock(), true), _logfile("") {
	ConfigParser conf("server.config");

	if (conf.empty())
		throw(Connection::ConxInit("Config file \'server.config\' is empty or doesn\'t exist."));
	if ((servername = conf["servername"]).empty())
		throw(Connection::ConxInit("Config file requires valid servername.\'servername = [servername]\'"));
	if ((desc = conf["description"]).empty())
		desc = std::string("No description for this server");
	if ((_oppass = conf["oppass"]).empty())
		throw(Connection::ConxInit("Config file requires valid operator password. \'oppass = [password]\'"));
	if (!(_logfile = LogFile(conf["logfile"])))
		throw(Connection::ConxInit("Config file requires valid log file name. \'logfile = [filename]\'"));
	if ((_whitelist = ConfigParser(LOCALSERV_WHITELIST_FILE)).empty())
		_logfile.append(LOG_DATE, DARK_GREY, " Empty whitelist: server launched in standalone.");
	return;
}

// __________Member functions____________

bool		LocalServer::run(void) {
	_logfile.append(LOG_DATE, LOG_DARKPURPLE, "\t[" + std::string(SERVER_VERSION) + "] ################################");
	_logfile.append(LOG_DATE, LOG_DARKPURPLE, "\t\t+ Server launched successfuly +");
	while(!isFinished()) {
		selectCall();
		checkStd();
		checkSock();
		checkConxs();
	}
	return true;
}

void		LocalServer::joinNet(std::string const &host, std::string const &port, std::string const &password) {
	Connection *tmp;

	if (!isWhitelisted(host + "," + port + "," + password, false)) {
		_logfile.append(LOG_DATE, LOG_DARKRED, " Joining \'Server\' -> [" + host + "]: Not whitelisted !");
		return;
	}
	try {
		tmp = new Connection(host, port, info.family());
	} catch (std::exception &ex) {
		_logfile.append(LOG_DATE, LOG_DARKGREY, ex.what());
		_logfile.append(LOG_DATE, LOG_DARKRED, " Joining \'Server\' -> [" + host + "]: failed !");
		return;
	}
	_logfile.append(LOG_DATE, LOG_YELLOW, " Joining \'Server\' -> [" + host + "]");
	_conxs.push_back(tmp);
	_sm.addFd(tmp->sock());
	tmp->send(PassCommand(password));
	tmp->send(ServerCommand(getArgListConnect()));
	tmp->isConnect(true);
}

// ########################################
// 				   PRIVATE
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 CONNECTION UTILS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::newConx(void) {
	Connection *tmp;

	try {
		tmp = new Connection(sock());
	} catch (std::exception &ex) {
		_logfile.append(LOG_DATE, LOG_DARKGREY, ex.what());
		_logfile.append(LOG_DATE, LOG_DARKRED, " New \'Unknown\' connection: failed !");
	}
	_logfile.append(LOG_DATE, LOG_DARKGREY, " New \'Unknown\' connection: [" + tmp->name() + "]");
	_conxs.push_back(tmp);
	_sm.addFd(_conxs.back()->sock());
}

void		LocalServer::finishConx(Connection *target, bool clear) {
	if (!target)
		return;
	target->isFinished(true);
	if (clear)
		target->clearMessages();
	if (target->isClient()) {
		unmapName(_nicknames, static_cast<Client *>(target)->nickname);
		unmapName(_usernames, static_cast<Client *>(target)->username);
		unmapName(_realnames, static_cast<Client *>(target)->realname);
		_logfile.append(LOG_DATE, LOG_DARKRED, " Ending \'Client\' connection: [" + target->name() + "]");
	} else if (target->isServer()) {
		unmapName(_nicknames, static_cast<Server *>(target)->servername);
		_logfile.append(LOG_DATE, LOG_DARKRED, " Ending \'Server\' connection: [" + target->name() + "]");
	} else
		_logfile.append(LOG_DATE, LOG_DARKRED, " Ending \'Unknown\' connection: [" + target->name() + "]");
}

void		LocalServer::mapName(namesmap &names, std::string const &name, Connection *conx) {
	if (name.empty() || !conx)
		return;
	if (!names.count(name))
		names[name] = conx;
}

void		LocalServer::unmapName(namesmap &names, std::string const &name) {
	if (names.empty() || name.empty())
		return;
	if (names.count(name))
		names.erase(name);
}

Client		*LocalServer::findClient(std::string const &name, unsigned char type) {
	if (!name.empty()) {
		if (type & NICKNAME) {
			if (_nicknames.count(name))
				return (static_cast<Client *>(_nicknames[name]));
		}
		if (type & USERNAME) {
			if (_usernames.count(name))
				return (static_cast<Client *>(_usernames[name]));
		}
		if (type & REALNAME) {
			if (_realnames.count(name))
				return (static_cast<Client *>(_realnames[name]));
		}
	}
	return (nullptr);
}

Server		*LocalServer::findServer(std::string const &name) {
	if (!name.empty()) {
		if (_servnames.count(name))
			return (static_cast<Server *>(_servnames[name]));
	}
	return (nullptr);
}

Channel		*LocalServer::newChan(Client *&sender, std::string const &name) {
	Channel *tmp;

	if(!(tmp = new(std::nothrow) Channel(sender, name)))
		return (nullptr);
	_chans.push_back(tmp);
	_logfile.append(LOG_DATE, LOG_DARKGREY, " New \'Channel\' created: [" + name + "]");
	return _chans.back();
}

void		LocalServer::finishChan(Channel *target) {
	if (!target)
		return;
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		if ((*it) == target) {
			_logfile.append(LOG_DATE, LOG_DARKRED, " Channel deleted ! [" + target->getName() + "]");
			_chans.erase(it);
			delete target;
			break;
		}
	}
}

Channel		*LocalServer::findChannel(std::string const &name) {
	if (name.empty())
		return (nullptr);
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		if ((*it)->checkName(name))
			return (*it);
	}
	return (nullptr);
}

bool		LocalServer::isWhitelisted(std::string const &info, bool servername) const {
	if (_whitelist.empty())
		return false;
	for (ConfigParser::fieldmap_cit it = _whitelist.cbegin(); it != _whitelist.cend(); it++) {
		if ((servername && !it->first.compare(info)) ||
			(!servername && !it->second.compare(info)))
				return true;
	}
	return false;
}

bool		LocalServer::checkWhitelistHost(std::string const &servername, std::string const &host) const {
	if (_whitelist.empty())
		return false;
	for (ConfigParser::fieldmap_cit it = _whitelist.cbegin(); it != _whitelist.cend(); it++) {
		if (!it->first.compare(servername)) {
			if (!host.compare("localhost") || !host.compare("127.0.0.1") || !host.compare("::1")) {
				if (!it->second.rfind("localhost", 0) || !it->second.rfind("127.0.0.1", 0) || !it->second.rfind("::1", 0))
					return true;
			} else if (!it->second.rfind(host, 0))
				return true;
			else
				return false;
		}
	}
	return false;
}

std::string const LocalServer::whitelistPassword(std::string const &servername) const {
	if (_whitelist.empty())
		return std::string();
	return (_whitelist[servername].substr(_whitelist[servername].rfind(',') + 1, std::string::npos));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 CONNECTION COUNT
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
		if ((*it)->checkStatus(check))
			count++;
	}
	return count;
}

std::string const 	LocalServer::howManyClient(bool direct) const { return Utils::nbrToStr(howMany(CONX_CLIENT, direct)); }
std::string const 	LocalServer::howManyServer(bool direct, bool self) const { return Utils::nbrToStr(howMany(CONX_SERVER, direct) + (self ? 1 : 0)); }
std::string const 	LocalServer::howManyService(bool direct) const { return Utils::nbrToStr(howMany(CONX_SERVICE, direct)); }
std::string const 	LocalServer::howManyUnknown(void) const { return Utils::nbrToStr(howMany(CONX_PENDING, false)); }
std::string const 	LocalServer::howManyOperator(void) const { return Utils::nbrToStr(howMany(CLIENT_OPERATOR)); }
std::string const 	LocalServer::howManyChannel(void) const { return Utils::nbrToStr(_chans.size()); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		  SELECT MODULE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::selectCall(void) {
	_sm.call(_conxs); //check exception later and return bool in order to fix _sm
	// _logfile.append(LOG_DATE, LOG_DARKPINK, " Select triggered !");
}

bool		LocalServer::isReadable(Connection *conx) const { return _sm.checkRfds(conx->sock()); }
bool		LocalServer::isWritable(Connection *conx) const { return _sm.checkWfds(conx->sock()); }

void		LocalServer::checkStd(void) {
	std::string input;

	if (_sm.checkStd()) {
		if (std::getline(std::cin, input)) {
			if (!input.compare("EXIT") || !input.compare("Exit") || !input.compare("exit"))
				isFinished(true);
			else if (!input.compare("NET") || !input.compare("Net") || !input.compare("net"))
				showNet();
			else if (!input.compare("LOCAL") || !input.compare("Local") || !input.compare("local"))
				showLocalServer();
			else if (!input.compare("NICKS") || !input.compare("Nicks") || !input.compare("nicks"))
				showNames(_nicknames);
			else if (!input.compare("USERS") || !input.compare("Users") || !input.compare("users"))
				showNames(_usernames);
			else if (!input.compare("REALNAMES") || !input.compare("Realnames") || !input.compare("realnames"))
				showNames(_realnames);
			else if (!input.compare("SERVERNAMES") || !input.compare("Servernames") || !input.compare("servernames"))
				showNames(_servnames);
			else if (!input.compare("CHANS") || !input.compare("Chans") || !input.compare("chans"))
				showChans();
			else
				return;
			_logfile.append(LOG_DATE, LOG_PINK, " Command line input >> [" + input + "]");
		}
	}
}

void		LocalServer::checkSock(void) {
	if (isReadable(static_cast<Connection *>(this)))
		newConx();
}

void		LocalServer::checkConxs(void) {
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if (isReadable(*it)) {
			try {
				if (!(*it)->read()) {
					_logfile.append(LOG_DATE, LOG_RED, " Someone left unexpectedly ! [" + (*it)->name() + "]");
					finishConx(*it, true);
					continue;
				}
			} catch (SockStream::FailRecv &ex) {
				_logfile.append(LOG_DATE, LOG_RED, " Connection lost while receiving !");
				finishConx(*it, true);
			}
		}
		while ((*it)->hasInputMessage())
			execCommand(*it, (*it)->getLastCommand());
		if (isWritable(*it)) {
			try {
				(*it)->write();
			} catch (SockStream::FailSend &ex) {
				_logfile.append(LOG_DATE, LOG_RED, " Connection lost while sending !");
				finishConx(*it, true);
			}
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 EXECUTION MODULE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::execCommand(Connection *&sender, Command cmd) {
	Client	*cli;
	Server	*svr;

	std::cout << cmd << std::endl;
	try {
		if (sender->isPending())
			execCommandPending(sender, cmd);
		else if (sender->isClient()) {
			execCommandClient((cli = static_cast<Client *>(sender)), cmd);
		}
	}
	catch (Command::InvalidCommandException &e) {
		if (e.code == ERR_WRONGPARAMSNUMBER || e.code == ERR_NORECIPIENT)
			numericReply(sender, e.code, cmd.command);
		else if (e.code == ERR_CANNOTSENDTOCHAN)
			numericReply(sender, e.code, cmd.argX(0));
		else if (e.code == ERR_CHANOPRIVSNEEDED)
			numericReply(sender, e.code, cmd);
		else if (e.code == ERR_UNKNOWNMODE)
			numericReply(sender, e.code, cmd);
		else
			numericReply(sender, e.code);
		if (!sender->isAuthentified())
			sender->isFinished(true);
	}
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
		else if (cmd == "SERVER")
			execServer(sender, ServerCommand(cmd));
		else
			numericReply(sender, ERR_NOTREGISTERED);
	}
}

void		LocalServer::execCommandClient(Client *&sender, Command const &cmd) {
	if (!sender->isRegistered()) {
		if (cmd == "NICK")
			execNick(sender, NickCommand(cmd));
		else if (cmd == "USER")
			execUser(sender, UserCommand(cmd));
		else
			numericReply(sender, ERR_NOTREGISTERED); 
	}
	else if (cmd == "NICK")
		execNick(sender, NickCommand(cmd));
	else if (cmd == "USER")
		execUser(sender, UserCommand(cmd));
	else if (cmd == "QUIT")
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
			sender->send(ErrorCommand("Bad password"));
			finishConx(sender, false);
		}
	}
	else
		numericReply(sender, ERR_ALREADYREGISTERED);
}

void		LocalServer::execNick(Connection *&sender, NickCommand const &cmd) {
	cmd.isValid();
	if (!Utils::validNickName(cmd.nickname()))
		return numericReply(sender, ERR_ERRONEOUSNICKNAME, cmd.nickname());
	if (_nicknames.count(cmd.nickname()))
		return numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	try {
		sender = new Client(sender, cmd);
	} catch (std::exception &ex) {
		_logfile.append(LOG_DATE, LOG_DARKRED, " Promotion \'Unknown\' -> \'Client\': failed !");
		finishConx(sender, true); //check if this line works (make promote fail)
		return;
	}
	_logfile.append(LOG_DATE, LOG_GREEN, " Promotion \'Unknown\' -> \'Client\': [" + cmd.nickname() + "]");
	mapName(_nicknames, cmd.nickname(), sender);
}

void		LocalServer::execUser(Connection *&sender, UserCommand const &cmd) {
	cmd.isValid();
	try {
		sender = new Client(sender, cmd);
	} catch (std::exception &ex) {
		_logfile.append(LOG_DATE, LOG_DARKRED, " Promotion \'Unknown\' -> \'Client\': failed !");
		finishConx(sender, true);
		return;
	}
	_logfile.append(LOG_DATE, LOG_GREEN, " Promotion \'Unknown\' -> \'Client\': [" + cmd.username() + "]");
	mapName(_usernames, cmd.username(), sender);
	mapName(_realnames, cmd.realname(), sender);
}

void		LocalServer::execServer(Connection *&sender, ServerCommand const &cmd) {
	cmd.isValid();
	if (!isWhitelisted(cmd.servername(), true) ||
		!checkWhitelistHost(cmd.servername(), sender->hostname()) ||
		whitelistPassword(cmd.servername()).empty()) {
		_logfile.append(LOG_DATE, LOG_DARKRED, " Promotion \'Unknown\' -> \'Server\': Not whitelisted !");
		sender->send(ErrorCommand("Access denied. You are not whitelisted by this server."));
		finishConx(sender, false);
		return;
	} else if (findServer(cmd.servername())) {
		_logfile.append(LOG_DATE, LOG_DARKRED, " Promotion \'Unknown\' -> \'Server\': Already registered !");
		sender->send(ErrorCommand("ID \'" + cmd.servername() + "\' already registered."));
		finishConx(sender, false);
		return;
	}
	try {
		sender = new Server(sender, cmd);
	} catch (std::exception &ex) {
		_logfile.append(LOG_DATE, LOG_DARKRED, " Promotion \'Unknown\' -> \'Server\': failed !");
		sender->send(ErrorCommand("Internal error."));
		finishConx(sender, false);
		return;
	}
	_logfile.append(LOG_DATE, LOG_BLUE, " Promotion \'Unknown\' -> \'Server\': [" + cmd.servername() + "]");
	mapName(_servnames, cmd.servername(), sender);
	if (!sender->isConnect()) {
		sender->send(PassCommand(whitelistPassword(cmd.servername())));
		sender->send(ServerCommand(getArgListAccept()));
		static_cast<Server *>(sender)->token = token;
		token = Utils::incToken(token);
	}
}

void		LocalServer::execNick(Client *sender, NickCommand const &cmd) {
	cmd.isValid();
	if (!Utils::validNickName(cmd.nickname()))
		return numericReply(sender, ERR_ERRONEOUSNICKNAME, cmd.nickname());
	if (_nicknames.count(cmd.nickname()))
		return numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	if (!sender->isRegistered()) {
		if (sender->nickname.empty()) {
			sender->nickname = cmd.nickname();
			if (!howMany(CONX_CLIENT, true))
				sender->isLocalop(true);
			numericReply(sender, RPL_WELCOME, sender->fullId());
			numericReply(sender, RPL_YOURHOST);
			numericReply(sender, RPL_CREATED);
			numericReply(sender, RPL_MYINFO);
		}
		else {
			unmapName(_nicknames, sender->nickname);
			sender->nickname = cmd.nickname();
		}
		mapName(_nicknames, sender->nickname, sender);
	}
	else {
		updateChans(sender, cmd);
		unmapName(_nicknames, sender->nickname);
		sender->nickname = cmd.nickname();
		mapName(_nicknames, sender->nickname, sender);
	}
}

void		LocalServer::execUser(Client *sender, UserCommand const &cmd) {
	cmd.isValid();
	if (!sender->isRegistered()) {
		if (sender->username.empty()) {
			sender->username = cmd.username();
			sender->realname = cmd.realname();
			if (!howMany(CONX_CLIENT, true))
				sender->isLocalop(true);
			numericReply(sender, RPL_WELCOME, sender->fullId());
			numericReply(sender, RPL_YOURHOST);
			numericReply(sender, RPL_CREATED);
			numericReply(sender, RPL_MYINFO);
		}
		else {
			unmapName(_usernames, sender->username);
			sender->username = cmd.username();
			unmapName(_realnames, sender->realname);
			sender->realname = cmd.realname();
		}
		mapName(_usernames, sender->username, sender);
		mapName(_realnames, sender->realname, sender);
	}
	else
		numericReply(sender, ERR_ALREADYREGISTERED);
}
void		LocalServer::execPrivmsg(Client *sender, PrivmsgCommand const &cmd) {
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if ((tmp = findClient(cmd.target(), CLIENTNAME))) {
		if (tmp->isAway())
			numericReply(sender, RPL_AWAY, tmp);
		else
			tmp->send(PrivmsgCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	}
	else if ((chan = findChannel(cmd.target())))
		chan->send(static_cast<Client *>(sender), PrivmsgCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	else
		numericReply(sender, ERR_NOSUCHNICK, cmd.target());
}

void		LocalServer::execNotice(Client *sender, NoticeCommand const &cmd) {
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if ((tmp = findClient(cmd.target(), CLIENTNAME)))
		tmp->send(NoticeCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	else if ((chan = findChannel(cmd.target())))
		chan->send(static_cast<Client *>(sender), NoticeCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	else
		numericReply(sender, ERR_DISCARDCOMMAND, cmd.target());
}

void		LocalServer::execQuit(Client *sender, QuitCommand const &cmd) {
	if (sender->isClient()) {
		updateChans(sender, cmd);
		sender->send(ErrorCommand("Closing connection"));
		finishConx(sender, false);
	}
}

void		LocalServer::execPing(Client *sender, PingCommand const &cmd) { cmd.isValid(); sender->send(PongCommand(servername)); } //later change hostname for servername
void		LocalServer::execPong(Client *sender, PongCommand const &cmd) { cmd.isValid(); } //command is discarded
void		LocalServer::execMotd(Client *sender, MotdCommand const &cmd) {
	std::ifstream file("motd.txt");
	std::string input;
	std::string tmp;

	cmd.isValid();
	if (!file.is_open())
		numericReply(sender, ERR_NOMOTD);
	else {
		numericReply(sender, RPL_MOTDSTART, hostname());
		while (std::getline(file, input)) {
			while (!input.empty()) {
				if (input.length() < MAX_MOTD_READ)
					input.append(std::string(MAX_MOTD_READ - input.length(), ' '));
				tmp = input.substr(0, MAX_MOTD_READ);
				input.erase(0, MAX_MOTD_READ);
				numericReply(sender, RPL_MOTD, tmp);
			}
		}
		if (file.bad())
			_logfile.append(LOG_DATE, LOG_DARKRED, " Error while reading MOTD file.");
		numericReply(sender, RPL_ENDOFMOTD);
		file.close();
	}
}

void		LocalServer::execLusers(Client *sender, LusersCommand const &cmd) {
	cmd.isValid();

	numericReply(sender, RPL_LUSERCLIENT);
	numericReply(sender, RPL_LUSEROP);
	numericReply(sender, RPL_LUSERUNKNOWN);
	numericReply(sender, RPL_LUSERCHANNELS);
	numericReply(sender, RPL_LUSERME);
}

void		LocalServer::execWhois(Client *sender, WhoisCommand const &cmd) {
	Client	*tmp;

	cmd.isValid();
	if (!(tmp = findClient(cmd.target(), ALLNAME)))
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

void		LocalServer::execJoin(Client *sender, JoinCommand const &cmd) {
	unsigned short code;
	std::string	names;
	std::string token;
	Channel *chan;

	cmd.isValid();
	names = cmd.target();
	while(!names.empty()) {
		token = Utils::getToken(names, ",");
		if (!Utils::validChanName(token))
			numericReply(sender, ERR_NOSUCHCHANNEL, token);
		else {
			if (!(chan = findChannel(token))) {
				if (!(chan = newChan(sender, token)))
					_logfile.append(LOG_DATE, LOG_RED, " New \'Channel\' created: failed !");
					return;
			}
			else if ((code = chan->join(sender))) { // add key arg later for -k mode
				continue; // treat error codes (banned, not invited, ...)
			}
			if (chan->hasTopic())
				numericReply(sender, RPL_TOPIC, chan);
			execNames(sender, NamesCommand(NO_PREFIX, chan->getName()));
		}
	}
}

void		LocalServer::execPart(Client *sender, PartCommand const &cmd) {
	Channel	*chan;

	cmd.isValid();
	if (!(chan = findChannel(cmd.target())))
		numericReply(sender, ERR_NOSUCHCHANNEL, cmd.target());
	else if (!chan->isOnChan(sender))
		numericReply(sender, ERR_NOTONCHANNEL, cmd.target());
	else {
		chan->updateClients(sender, cmd);
		if (chan->empty())
			finishChan(chan);
	}
}

void		LocalServer::execTopic(Client *sender, TopicCommand const &cmd) {
	Channel	*chan;

	cmd.isValid();
	if (!(chan = findChannel(cmd.target())))
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

void		LocalServer::execOppass(Client *sender, OppassCommand const &cmd) {
	if (static_cast<Client *>(sender)->isLocalop()) {
		cmd.isValid();
		setOppass(cmd.password());
	}
	else
		numericReply(sender, ERR_NOTLOCALOP);
}

void		LocalServer::execOper(Client *sender, OperCommand const &cmd) {
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

void		LocalServer::execMode(Client *sender, ModeCommand const &cmd) {
	Channel		*chan;
	Client		*target;
	std::string	mode;

	cmd.isValid();
	if (cmd.argNbr() == 1) {
		if (!(chan = findChannel(cmd.field1())))
			throw (Command::InvalidCommandException(ERR_NOSUCHCHANNEL));
		numericReply(sender, RPL_CHANNELMODEIS, chan);
	}
	else if (cmd.argNbr() == 2) {
		if (cmd.field1()[0] == '#' || cmd.field1()[0] == '&') {
			if (!(chan = findChannel(cmd.field1())))
				throw (Command::InvalidCommandException(ERR_NOSUCHCHANNEL));
			mode = cmd.field2();
			if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
				throw (Command::InvalidCommandException(ERR_UMODEUNKNOWNFLAG));
			chan->applyModeFlag(static_cast<Client *>(sender), mode[1], (mode[0] == '+' ? true : false));
		}
		else {
			if (!(target = findClient(cmd.field1(), NICKNAME)))
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
		if (!(chan = findChannel(cmd.field1())))
			throw (Command::InvalidCommandException(ERR_NOSUCHCHANNEL));
		mode = cmd.field2();
		if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
			throw (Command::InvalidCommandException(ERR_UNKNOWNMODE));
		if (!(target = findClient(cmd.field3(), NICKNAME)))
			throw (Command::InvalidCommandException(ERR_USERNOTINCHANNEL));
		else if (!chan->isOnChan(target))
			throw (Command::InvalidCommandException(ERR_USERNOTINCHANNEL));
		chan->applyUserModeFlag(static_cast<Client *>(sender), target, mode[1], (mode[0] == '+' ? true : false));
		chan->send(nullptr, ModeCommand(static_cast<Client *>(sender)->fullId(), cmd.args));
	}
}

void		LocalServer::execAway(Client *sender, AwayCommand const &cmd) {
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

void		LocalServer::execNames(Client *sender, NamesCommand const &cmd) {
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
		if ((chan = findChannel(cmd.target()))) {
			chan->namesList(lst);
			if (!lst.empty())
				numericReplyNames(sender, chan->getName(), lst);
		}
		numericReply(sender, RPL_ENDOFNAMES, cmd.target());
	}
}

void		LocalServer::execList(Client *sender, ListCommand const &cmd) {
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
		if ((chan = findChannel(cmd.target())))
			numericReply(sender, RPL_LIST, chan);
		numericReply(sender, RPL_LISTEND);
	}
}

void		LocalServer::execWho(Client *sender, WhoCommand const &cmd) {
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
		if ((chan = findChannel(cmd.mask()))) {
			chan->clientsList(lst);
			for (std::list<Client *>::iterator it = lst.begin(); it != lst.end(); it++)
				numericReplyWho(sender, chan, (*it));
		}
		numericReply(sender, RPL_ENDOFWHO, cmd.mask());
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 NUMERIC REPLIES
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::numericReply(Connection *sender, unsigned short code) {
	Command ret(servername, codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	switch(code) {
		case ERR_DISCARDCOMMAND: return;
		case RPL_YOURHOST:			ret.addArg("Your host is " + servername\
									+ ", running version " + version); break;
		case RPL_CREATED:			ret.addArg("This server was created "\
									+ _logfile.start); break;
		case RPL_MYINFO:			ret.addArg(servername + " " + version + " "
									+ std::string(CLIENT_MODE_FLAGS) + " "
									+ std::string(CHAN_MODE_FLAGS)); break;
		case RPL_LUSERCLIENT:		ret.addArg("There are " + howManyClient(false)
									+ " users and " + howManyService(false)
									+ " services on " + howManyServer(false, true)
									+ " servers"); break;
		case RPL_LUSEROP:			ret.addArg(howManyOperator());
									ret.addArg("operator(s) online"); break;
		case RPL_LUSERUNKNOWN:		ret.addArg(howManyUnknown());
									ret.addArg("unknown connection(s)"); break;
		case RPL_LUSERCHANNELS:		ret.addArg(howManyChannel());
									ret.addArg("channels formed"); break;
		case RPL_LUSERME: 			ret.addArg("I have " + howManyClient(true)
									+ " clients and " + howManyServer(true, false)
									+ " servers"); break;
		case RPL_UNAWAY: 			ret.addArg("You are no longer marked as being away"); break;
		case RPL_NOWAWAY: 			ret.addArg("You have been marked as being away"); break;
		case RPL_LISTEND: 			ret.addArg("End of list"); break;
		case RPL_ENDOFMOTD: 		ret.addArg("End of MOTD command"); break;
		case RPL_YOUREOP: 			ret.addArg("You are now an IRC operator"); break;
		case ERR_NOORIGIN: 			ret.addArg("No origin specified"); break;
		case ERR_NOTEXTTOSEND: 		ret.addArg("No text to send"); break;
		case ERR_NOMOTD: 			ret.addArg("MOTD file is missing"); break;
		case ERR_NONICKNAMEGIVEN: 	ret.addArg("No nickname given"); break;
		case ERR_NOTREGISTERED: 	ret.addArg("You have not registered"); break;
		case ERR_ALREADYREGISTERED: ret.addArg("Unauthorized command (already registered)"); break;
		case ERR_PASSWDMISMATCH: 	ret.addArg("Wrong password"); break;
		case ERR_NOTLOCALOP: 		ret.addArg("You\'re not a local operator"); break;
		case ERR_UMODEUNKNOWNFLAG: 	ret.addArg("Unknown MODE flag"); break;
		case ERR_USERSDONTMATCH: 	ret.addArg("Cannot change mode for other users"); break;
		default: return;
	}
	sender->send(ret);
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, std::string const &field) {
	Command ret(servername, codeToStr(code), Command::arglist());

	ret.addArg(sender->name());
	switch(code) {
		case ERR_DISCARDCOMMAND:
			return;
		case RPL_WELCOME: 			ret.addArg("Welcome to the PixTillz\'s Internet Relay Network " + field); break;
		case RPL_WHOISOPERATOR: 	ret.addArg(field);
									ret.addArg("is an IRC operator"); break;
		case RPL_ENDOFWHO: 			ret.addArg(field);
									ret.addArg("End of WHO list"); break;
		case RPL_ENDOFWHOIS: 		ret.addArg(field);
									ret.addArg("End of WHOIS list"); break;
		case RPL_NOTOPIC: 			ret.addArg(field);
									ret.addArg("No topic is set"); break;
		case RPL_ENDOFNAMES: 		ret.addArg(field);
									ret.addArg("End of NAMES list"); break;
		case RPL_MOTD: 				ret.addArg("- " + field + "-"); break;
		case RPL_MOTDSTART: 		ret.addArg("- " + field + " Message of the day -"); break;
		case ERR_NOSUCHNICK: 		ret.addArg(field);
									ret.addArg("No such nickname/channel"); break;
		case ERR_NOSUCHSERVER: 		ret.addArg(field);
									ret.addArg("No such server"); break;
		case ERR_NOSUCHCHANNEL: 	ret.addArg(field);
									ret.addArg("No such channel"); break;
		case ERR_CANNOTSENDTOCHAN: 	ret.addArg(field);
									ret.addArg("Cannot send to channel"); break;
		case ERR_NORECIPIENT: 		ret.addArg("No recipient given (" + field + ")"); break;
		case ERR_UNKNOWNCOMMAND: 	ret.addArg(field);
									ret.addArg("Unknown command"); break;
		case ERR_ERRONEOUSNICKNAME: ret.addArg(field);
									ret.addArg("Erroneous nickname"); break;
		case ERR_NICKNAMEINUSE: 	ret.addArg(field);
									ret.addArg("Nickname is already in use"); break;
		case ERR_NOTONCHANNEL: 		ret.addArg(field);
									ret.addArg("You\'re not on that channel"); break;
		case ERR_WRONGPARAMSNUMBER: ret.addArg(field);
									ret.addArg("Wrong number of parameters"); break;
		default: return;
	}
	sender->send(ret);
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, Command const &cmd) {
	Command ret(servername, codeToStr(code), Command::arglist());

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
	Command ret(servername, codeToStr(code), Command::arglist());

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
		ret.addArg(Utils::nbrToStr(chan->size()));
		if (chan->hasTopic())
			ret.addArg(chan->getTopic());
	}
	else
		return;
	sender->send(ret);
}

void		LocalServer::numericReply(Connection *sender, unsigned short code, Client *target) {
	Command ret(servername, codeToStr(code), Command::arglist());

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
			ret.addArg(servername);
		ret.addArg("Active on this server"); // change it to whatever information about the client later
	}
	else if (code == RPL_AWAY) {
		ret.addArg(target->nickname);
		ret.addArg(target->awaymsg);
	}
	sender->send(ret);
}

void		LocalServer::numericReplyNames(Client *sender, std::string const &chan, std::list<std::string> &names) {
	Command::arglist_it itnames;
	std::string			tmp;

	while (!names.empty()) {
		Command ret(Command(servername, codeToStr(RPL_NAMESREPLY), Command::arglist()));
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

void		LocalServer::numericReplyWho(Client *sender, Channel *chan, Client *target) {
	Command ret(servername, codeToStr(RPL_WHOREPLY), Command::arglist());

	ret.addArg(sender->nickname);
	ret.addArg((chan ? chan->getName() : "*"));
	ret.addArg(target->username);
	ret.addArg(target->hostname());
	ret.addArg(servername);
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
	ret.addArg(Utils::nbrToStr(target->hop));
	ret.addArg(target->realname);
	sender->send(ret);
}

void		LocalServer::numericReplyWhoischannel(Client *sender, Client *target, std::list<std::string> chans) {
	std::ostringstream str;
	unsigned int count;
	Channel *chan;

	count = 0;
	str.str(":");
	while (!chans.empty()) {
		Command ret(servername, codeToStr(RPL_WHOISCHANNELS), Command::arglist());
		if (!(chan = findChannel(chans.front())))
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

std::string	const LocalServer::codeToStr(unsigned short code) const { 
	if (code < 10)
		return ("00" + Utils::nbrToStr(static_cast<unsigned int>(code)));
	else if (code < 100)
		return ("0" + Utils::nbrToStr(static_cast<unsigned int>(code)));
	else
		return Utils::nbrToStr(static_cast<unsigned int>(code));
}

void		LocalServer::namesListAloneClient(std::list<std::string> &lst) const {
	lst.clear();
	for (conxlist_cit it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isClient() && !static_cast<Client *>(*it)->hasChans())
			lst.push_back(static_cast<Client *>(*it)->nickname);
	}
}

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

void		LocalServer::showNames(namesmap names) const {
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
	for (namesmap::const_iterator it = names.begin(); it != names.end(); it++) {
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
				DEBUG_LDISPCB(COUT, "Client   ", *(static_cast<Client *>(tmp)), GREEN, '{');
			else if (tmp->isServer())
				DEBUG_LDISPCB(COUT, "Server   ", *(static_cast<Server *>(tmp)), BLUE, '{');
			else
				DEBUG_LDISPCB(COUT, "Pending  ", *tmp, ORANGE, '{');
		}
	}
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}
