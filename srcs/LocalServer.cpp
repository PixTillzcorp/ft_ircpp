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
**----- Updated -------------{ 2022-01-28 20:23:54 }--------------------------**
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
	_logfile.append(LOG_DATE, LOG_PURPLE, "\t\t+ Server closed successfuly +");
	_logfile.append(LOG_DATE, LOG_PURPLE, "###############################################");
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
	if (!Utils::validServName(servername))
		throw(Connection::ConxInit("Config file requires valid servername. ([aA0-zZ9]) *(\'.\' [aA0-zZ9]) max 63."));
	if ((desc = conf["description"]).empty())
		desc = std::string("No description for this server");
	if ((_oppass = conf["oppass"]).empty())
		throw(Connection::ConxInit("Config file requires valid operator password. \'oppass = [password]\'"));
	if (!(_logfile = LogFile(conf["logfile"])))
		throw(Connection::ConxInit("Config file requires valid log file name. \'logfile = [filename]\'"));
	if ((_whitelist = ConfigParser(LOCALSERV_WHITELIST_FILE)).empty())
		logNotify(false, "Empty whitelist: server launched in standalone.");
	return;
}

// __________Member functions____________

bool		LocalServer::run(void) {
	_logfile.append(LOG_DATE, LOG_PURPLE, "\t[" + std::string(SERVER_VERSION) + "] ################################");
	_logfile.append(LOG_DATE, LOG_PURPLE, "\t\t+ Server launched successfuly +");
	while(!isFinished() || !_conxs.empty()) {
		selectCall();
		checkStd();
		checkSock();
		checkConxs();
	}
	return true;
}

void		LocalServer::joinNet(std::string const &authinfo) {
	Connection *tmp;
	std::string host;
	std::string port;
	std::string password;

	if (!Utils::splitAuthInfo(authinfo, host, port, password))
		return logNotify(true, "Joining \'Server\': Erroneous authentication's info !");
	if (!isWhitelisted(authinfo, false))
		return logNotify(true, "Joining \'Server\' -> [" + host + "]: Not whitelisted !");
	try {
		tmp = new Connection(host, port, info.family());
	} catch (std::exception &ex) {
		logNotify(true, ex.what());
		logNotify(true, "Joining \'Server\' -> [" + host + "]: failed !");
		return;
	}
	logSuccess("Joining \'Server\' -> [" + host + "]");
	_conxs.push_back(tmp);
	_sm.addFd(tmp->sock());
	tmp->send(PassCommand(password));
	tmp->send(ServerCommand(serverArgsConnect()));
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
		logNotify(true, ex.what());
		logNotify(true, "New \'Unknown\' connection: failed !");
		return;
	}
	logSuccess("New \'Unknown\' connection: [" + tmp->name() + "]");
	_conxs.push_back(tmp);
	_sm.addFd(_conxs.back()->sock());
}

Server		*LocalServer::newLink(Server *sender, ServerCommand const &cmd) {
	Server	*svr;
	std::string modes;

	try { svr = new Server(sender, cmd); }
	catch (std::exception &ex) {
		logError(sender, "Internal error from \'newLink\' !", "Internal error");
		return (nullptr);
	}
	_conxs.push_back(svr);
	mapName(_servnames, cmd.servername(), svr);
	return svr;
}

Client		*LocalServer::newLink(Server *sender, NickCommand const &cmd) {
	Client	*cli;
	std::string modes;

	try { cli = new Client(sender, Utils::strToNbr(cmd.hopcount())); }
	catch (std::exception &ex) {
		logError(sender, "Internal error from \'newLink\' !", "Internal error");
		return (nullptr);
	}
	_conxs.push_back(cli);
	cli->nickname = cmd.nickname();
	mapName(_nicknames, cmd.nickname(), cli);
	cli->username = cmd.username();
	mapName(_usernames, cmd.username(), cli);
	cli->realname = cmd.realname();
	mapName(_realnames, cmd.realname(), cli);
	cli->info.host = cmd.host();
	modes = cmd.umode();
	while (!modes.empty()) {
		cli->setModeFlag(modes[0]);
		modes.erase(0, 1);
	}
	return cli;
}

void		LocalServer::finishConx(Server *sender, Client *target, bool clear, std::string const &quitmsg) {
	if (!target)
		return;
	if (clear)
		target->clearMessages();
	else if (quitmsg.empty())
		target->send(ErrorCommand("[" + target->name() + "] Closing connection"));
	else
		target->send(ErrorCommand("[" + target->name() + "] Closing connection (" + quitmsg + ")"));
	target->isFinished(true);
	unmapName(_nicknames, target->nickname);
	unmapName(_usernames, target->username);
	unmapName(_realnames, target->realname);

	// leaves channels if necessary
	updateChans(target, QuitCommand(target->fullId(), quitmsg));

	// notify servers
	if (sender && !sender->isFinished())
		broadcastToServers(sender, QuitCommand(target->nickname, quitmsg));

	// // notify clients
	// broadcastToClients(target, QuitCommand(target->fullId(), quitmsg));

	logNotify(true, "Ending \'Client\' connection -> [" + target->name() + "]");
}

void		LocalServer::finishConx(Server *sender, Server *target, bool clear, SquitCommand const &cmd) {
	if (!target)
		return;
	if (clear)
		target->clearMessages();
	target->isFinished(true);

	// notify servers
	broadcastToServers(sender, cmd);

	// break links associated to this server
	if (!target->isLink())
		breakLinks(target);

	unmapName(_servnames, target->servername);
	if (!target->compare(this))
		purge();
	else
		logNotify(true, "Ending \'Server\' connection -> [" + target->name() + "]");
}

void		LocalServer::finishConx(Connection *target, bool clear) {
	if (!target)
		return;
	target->isFinished(true);
	if (clear)
		target->clearMessages();
	logNotify(true, "Ending \'Unknown\' connection -> [" + target->name() + "]");
}

void		LocalServer::breakLinks(Server *origin) {
	if (!origin)
		return;
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isLink() && !(static_cast<Server *>((*it)->link)->compare(origin))) {
			if ((*it)->isClient())
				finishConx(origin, static_cast<Client *>((*it)), true, "Netsplit from [" + origin->name() + "]");
			else if ((*it)->isServer())
				finishConx(nullptr, static_cast<Server *>((*it)), true, SquitCommand(NO_PREFIX, static_cast<Server *>((*it))->servername, "Netsplit."));
			else
				finishConx((*it), true);
		}
	}
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

	if(!(tmp = new(std::nothrow) Channel(sender, name))) {
		logNotify(true, "New \'Channel\' creation: failed !");
		return (nullptr);
	}
	_chans.push_back(tmp);
	logSuccess("New \'Channel\' creation: [" + name + "]");
	return _chans.back();
}

void		LocalServer::finishChan(Channel *target) {
	if (!target)
		return;
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		if ((*it) == target) {
			logNotify(true, "Channel deleted ! [" + target->getName() + "]");
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
	return (_whitelist[servername].substr(_whitelist[servername].rfind(':') + 1, std::string::npos));
}

void		LocalServer::purge(void) {
	logColored(LOG_PURPLE, "Closing \'Local Server\' [" + name() + "]");
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isClient())
			finishConx(nullptr, static_cast<Client *>((*it)), false, "Server closing.");
		else if ((*it)->isServer())
			finishConx(nullptr, static_cast<Server *>((*it)), false, SquitCommand(NO_PREFIX, static_cast<Server *>((*it))->servername, "Server closing."));
		else
			finishConx((*it), true);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//			 BROADCAST
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void	LocalServer::broadcastServer(Server *sender, Server *shared) {
	if (!shared)
		return;
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isServer() && !(*it)->isLink()) {
			if (!sender || sender->compare(*it))
				(*it)->send(ServerCommand(name(), shared->serverArgsShare(static_cast<Server *>(*it)->token)));
		}
	}
}

void	LocalServer::broadcastToServers(Server *sender, Command const &cmd) {
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isServer() && !(*it)->isLink()) {
			if (!sender || sender->compare(static_cast<Server *>(*it)))
				(*it)->send(cmd);
		}
	}
}

void	LocalServer::broadcastClient(Server *sender, Client *shared) {
	if (!shared)
		return;
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isServer() && !(*it)->isLink()) {
			if (!sender || sender->compare(*it))
				(*it)->send(NickCommand(name(), shared->nickArgs(static_cast<Server *>(*it)->token)));
		}
	}
}

void	LocalServer::broadcastToClients(Client *sender, Command const &cmd) {
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isClient() && !(*it)->isLink()) {
			if (!sender || sender->compare(static_cast<Client *>(*it)))
				(*it)->send(cmd);
		}
	}
}

void	LocalServer::shareServs(Server *target) {
	if (!target)
		return;
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if (target->compare(*it) && (*it)->isServer()) {
			target->send(ServerCommand(name(), static_cast<Server *>(*it)->serverArgsShare(token)));
			try { token = Utils::incToken(token); }
			catch (Utils::FailStream &ex) { 
				logNotify(true, "Internal error.");
				return finishConx(nullptr, this, true, SquitCommand(NO_PREFIX, name(), "Internal error."));
			}
		}
	}
}

void	LocalServer::shareConxs(Server *target) {
	if (!target)
		return;
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if (target->compare(*it) && (*it)->isClient())
			target->send(NickCommand(name(), static_cast<Client *>(*it)->nickArgs(target->token)));
	}
}

void	LocalServer::shareChans(Server *target) {
	std::list<std::string>::iterator itnames;
	std::list<std::string> names;
	std::string tmp;

	if (!target)
		return;
	for (chanlist_it it = _chans.begin(); it != _chans.end(); it++) {
		if (!(*it)->isLocalChannel()) {
			(*it)->namesList(names);
			while (!names.empty()) {
				itnames = names.begin();
				std::advance(itnames, (names.size() < MAX_RPL_ENTITY ? names.size() : MAX_RPL_ENTITY));
				while (names.begin() != itnames) {
					tmp.append(names.front());
					names.pop_front();
					if (names.begin() != itnames)
						tmp.append(1, ',');
				}
				target->send(NjoinCommand(name(), (*it)->getName(), ":" + tmp));
				tmp.clear();
			}
			target->send(ModeCommand(name(), (*it)->getName(), (*it)->getModesFlags()));
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 CONNECTION COUNT
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

unsigned int		LocalServer::howMany(char flag) const {
	unsigned int	count = 0;

	for (conxlist_cit it = _conxs.begin(); it != _conxs.end(); it++) {
		if ((*it)->isClient()) {
			if (static_cast<Client *>(*it)->checkModeFlag(flag))
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
std::string const 	LocalServer::howManyOperator(void) const { return Utils::nbrToStr(howMany(CLIENT_FLAG_OPERATOR)); }
std::string const 	LocalServer::howManyChannel(void) const { return Utils::nbrToStr(_chans.size()); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		  SELECT MODULE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::selectCall(void) {
	_sm.call(_conxs, isFinished()); //check exception later and return bool in order to fix _sm
	// _logfile.append(LOG_DATE, LOG_DARKPINK, " Select triggered !");
}

bool		LocalServer::isReadable(Connection *conx) const { return _sm.checkRfds(conx->sock()); }
bool		LocalServer::isWritable(Connection *conx) const { return _sm.checkWfds(conx->sock()); }

void		LocalServer::checkStd(void) {
	std::string input;

	if (_sm.checkStd()) {
		if (std::getline(std::cin, input)) {
			if (!input.compare("EXIT") || !input.compare("Exit") || !input.compare("exit"))
				finishConx(nullptr, this, true, SquitCommand(NO_PREFIX, name(), "Console shutdown."));
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
			_logfile.append(LOG_DATE, LOG_PINK, "Command line input $> [" + input + "]");
		} else {
			finishConx(nullptr, this, true, SquitCommand(NO_PREFIX, name(), "Console shutdown."));
		}
	}
}

void		LocalServer::checkSock(void) {
	if (isReadable(static_cast<Connection *>(this)))
		newConx();
}

void		LocalServer::checkConxs(void) {
	for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
		if (!(*it)->isLink()) {
			if (isReadable(*it)) {
				try {
					if (!(*it)->read()) {
						logError(*it, "Connection lost with \'" + (*it)->name() + "\'");
						continue;
					}
				} catch (SockStream::FailRecv &ex) {
					logError(*it, "Connection lost while receiving from \'" + (*it)->name() + "\'");
				}
			}
			while ((*it)->hasInputMessage())
				execCommand(*it, (*it)->getLastCommand());
			if (isWritable(*it)) {
				try {
					(*it)->write();
				} catch (SockStream::FailSend &ex) {
					logError(*it, "Connection lost while sending to \'" + (*it)->name() + "\'");
				}
			}
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 EXECUTION MODULE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ###### EXEC PATH ######

void		LocalServer::execCommand(Connection *&sender, Command cmd) {
	Client	*cli;
	Server	*svr;

	std::cout << cmd << std::endl; // debug
	try {
		cmd.isValid();
		if (sender->isPending())
			execCommandPending(sender, cmd);
		else if (sender->isClient())
			execCommandClient((cli = static_cast<Client *>(sender)), cmd);
		else if (sender->isServer()) {
			try {
				execCommandServer((svr = static_cast<Server *>(sender)), cmd);
			} catch (Command::InvalidCommand &ex) {
				logError(static_cast<Server *>(sender), "Invalid command received from [" + sender->name() + "].", "Invalid command.");
			}
		}
	}
	catch (Command::InvalidCommand &ex) {
		if (ex.code == ERR_WRONGPARAMSNUMBER || ex.code == ERR_NORECIPIENT)
			numericReply(sender, ex.code, cmd.command);
		else if (ex.code == ERR_CANNOTSENDTOCHAN)
			numericReply(sender, ex.code, cmd.argX(0));
		else if (ex.code == ERR_CHANOPRIVSNEEDED)
			numericReply(sender, ex.code, cmd);
		else if (ex.code == ERR_UNKNOWNMODE)
			numericReply(sender, ex.code, cmd);
		else
			numericReply(sender, ex.code);
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
			logNotify(true, "Connection [" + sender->hostname() + "] did not authentify itself.");
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
		return numericReply(sender, ERR_UNKNOWNCOMMAND, cmd.command);
	logNotify(false, "~{" + cmd.command + "}~ from client [" + sender->name() + "] executed successfuly.");
}

void		LocalServer::execCommandServer(Server *sender, Command const &cmd) {
	if (cmd == "NICK")
		execNick(sender, NickCommand(cmd));
	else if (cmd == "JOIN")
		execJoin(sender, JoinCommand(cmd));
	else if (cmd == "SERVER")
		execServer(sender, ServerCommand(cmd));
	else if (cmd == "PART")
		execPart(sender, PartCommand(cmd));
	else if (cmd == "PRIVMSG")
		execPrivmsg(sender, PrivmsgCommand(cmd));
	else if (cmd == "NOTICE")
		execNotice(sender, NoticeCommand(cmd));
	else if (cmd == "NJOIN")
		execNjoin(sender, NjoinCommand(cmd));
	else if (cmd == "MODE")
		execMode(sender, ModeCommand(cmd));
	else if (cmd == "QUIT")
		execQuit(sender, QuitCommand(cmd));
	else if (cmd == "SQUIT")
		execSquit(sender, SquitCommand(cmd));
	else
		logError(sender, "Unknown command received.", "Unknown or forbidden command");
	logNotify(false, "~{" + cmd.command + "}~ from server [" + sender->name() + "] executed successfuly.");
}

// void		LocalServer::execCommandService(Service *sender, Command const &cmd);

// ###### PENDING ######

void		LocalServer::execPass(Connection *&sender, PassCommand const &cmd) {
	cmd.isValid();
	if (!sender->isAuthentified()) {
		if (cmd.password() == _password) {
			logSuccess("Connection [" + sender->hostname() + "] authentified successfuly.");
			sender->isAuthentified(true);
		}
		else
			return logError(sender, "Connection attempt: failed, Bad password.", "Bad password");
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
		logPromote("Client", "Internal error.", false);
		return finishConx(sender, true);
	}
	logPromote("Client", "[" + cmd.nickname() + "]", true);
	mapName(_nicknames, cmd.nickname(), sender);
}

void		LocalServer::execUser(Connection *&sender, UserCommand const &cmd) {
	cmd.isValid();
	if (!Utils::validUserName(cmd.username()))
		return numericReply(sender, ERR_ERRONEOUSUSERNAME, cmd.username());
	if (!Utils::validRealName(cmd.realname()))
		return numericReply(sender, ERR_ERRONEOUSREALNAME, cmd.realname());
	try {
		sender = new Client(sender, cmd);
	} catch (std::exception &ex) {
		logPromote("Client", "failed !", false);
		return finishConx(sender, true);
	}
	logPromote("Client", "[" + cmd.username() + "]", true);
	mapName(_usernames, cmd.username(), sender);
	mapName(_realnames, cmd.realname(), sender);
}

void		LocalServer::execServer(Connection *&sender, ServerCommand const &cmd) {
	cmd.isValid();

	if (!isWhitelisted(cmd.servername(), true) || !checkWhitelistHost(cmd.servername(), sender->hostname()) ||
	whitelistPassword(cmd.servername()).empty() || !Utils::validServName(cmd.servername())) {
		logPromote("Server", "Not whitelisted !", false);
		return logError(sender, "", "Access denied. Not whitelisted");
	} else if (findServer(cmd.servername())) {
		logPromote("Server", "Already registered !", false);
		return logError(sender, "", "ID \'" + cmd.servername() + "\' already registered.");
	}
	try {
		sender = new Server(sender, cmd);
	} catch (std::exception &ex) {
		logPromote("Server", "Internal error.", false);
		return logError(sender, "", "Internal error.");
	}
	logPromote("Server", "[" + cmd.servername() + "]", true);
	mapName(_servnames, cmd.servername(), sender);
	if (!sender->isConnect()) {
		sender->send(PassCommand(whitelistPassword(cmd.servername())));
		sender->send(ServerCommand(serverArgsAccept()));
		static_cast<Server *>(sender)->token = token;
		try { token = Utils::incToken(token); }
		catch (Utils::FailStream &ex) { 
			logNotify(true, "Internal error.");
			return finishConx(nullptr, this, true, SquitCommand(NO_PREFIX, name(), "Internal error."));
		}
		shareServs(static_cast<Server *>(sender));
		shareConxs(static_cast<Server *>(sender));
		shareChans(static_cast<Server *>(sender));
	}
}

// ###### SERVER ######

void		LocalServer::execNick(Server *sender, NickCommand const &cmd) {
	cmd.isValid();
	if (_nicknames.count(cmd.nickname())) {
		logPromote("Link", "Redundancy between two nicknames.", false);
		logError(sender, "", "Spanning tree broken for this connection.");
	}
	else if (Utils::validNickName(cmd.nickname()) && sender->isToken(cmd.servertoken()) &&
	Utils::validUserName(cmd.username()) && Utils::validRealName(cmd.realname()) &&
	Utils::checkNbr(cmd.hopcount())) {
		logPromote("Link", "[" + cmd.nickname() + "!" + cmd.username() + "@" + cmd.host() + "]", true);
		broadcastClient(sender, newLink(sender, cmd));
	} else {
		logPromote("Link", "Failed !", false);
		logNotify(true, "Server \'" + sender->name() + "\' sent an erroneous \'" + cmd.command + "\' command !");
		logError(sender, "", "Erroneous command.");
	}
}

void		LocalServer::execServer(Server *sender, ServerCommand const &cmd) {
	Server *source;

	cmd.isValid();
	if (_servnames.count(cmd.servername())) {
		logPromote("Link", "Redundancy between two servernames.", false);
		return logError(sender, "", "Spanning tree broken for this connection.");
	}
	if (cmd.prefix.empty() || !(source = findServer(cmd.prefix)))
		return logError(sender, "SERVER -> Unknown source server.", "SERVER Command failed.");
	if (!Utils::validServName(cmd.servername()))
		return logError(sender, "SERVER -> Wrong server name.", "SERVER Command failed.");
	if (!Utils::checkNbr(cmd.hopcount()))
		return logError(sender, "SERVER -> Wrong Hop count.", "SERVER Command failed.");
	if (!source->isToken(cmd.token()))
		return logError(sender, "SERVER -> Wrong token for \'" + source->name() +"\'.", "SERVER Command failed.");
	logPromote("Link", "[" + cmd.servername() + "]", true);
	broadcastServer(sender, newLink(sender, cmd));
}

void		LocalServer::execNjoin(Server *sender, NjoinCommand const &cmd) {
	std::string members;
	std::string cliname;
	Client	*cli;
	Channel *tmp;

	cmd.isValid();
	if (!Utils::validChanName(cmd.chan()) || findChannel(cmd.chan()))
		return logError(sender, "NJOIN -> Invalid channel name.", "NJOIN Command failed.");
	if (!cmd.chan().rfind("&", 0))
		return logError(sender, "NJOIN -> Shared a local channel.", "NJOIN Command failed.");
	if ((members = cmd.members()).empty())
		return logError(sender, "NJOIN -> Invalid member list.", "NJOIN Command failed.");
	cliname = Utils::getToken(members, ",");
	if (!cliname.empty() && !cliname.rfind("@@", 0) && cliname.compare("@@"))
		cliname.erase(0,2);
	else
		return logError(sender, "NJOIN -> Invalid channel\'creator name.", "NJOIN Command failed.");
	if (!(cli = findClient(cliname, NICKNAME)))
		return logError(sender, "NJOIN -> Channel\'s creator does not exist.", "NJOIN Command failed.");
	if (!(tmp = new(std::nothrow) Channel(cmd.chan(), cli)))
		return logError(sender, "Internal error.", "Internal error.");
	while (!members.empty()) {
		if ((cliname = Utils::getToken(members, ",")).empty())
			continue;
		if (!cliname.rfind("@", 0))
			cliname.erase(0, 1);
		if (!(cli = findClient(cliname, NICKNAME)))
			continue;
		tmp->njoin(cli);
	}
	_chans.push_back(tmp);
	logSuccess("New \'Channel\' created: [" + cmd.chan() + "]");
}

void		LocalServer::execMode(Server *sender, ModeCommand const &cmd) {
	Channel		*chan;
	Client		*source;
	Client		*target;
	std::string	modes;
	bool		set;

	cmd.isValid();

	modes = cmd.field2();

	// retreiving modes string and bool set/unset
	if (modes.empty() || (modes.rfind("-", 0) && modes.rfind("+", 0)) || modes.length() < 2)
		return logError(sender, "MODE -> Invalid mode string \'" + modes + "\'.", "MODE Command failed.");
	set = (!modes.rfind("+", 0) ? true : false);
	modes.erase(0, 1);

	if (cmd.argNbr() == 2) {
		if (!cmd.field1().rfind("#", 0) || !cmd.field1().rfind("&", 0)) {
			if (!findClient(cmd.prefix, NICKNAME) && !findServer(cmd.prefix))
				return logError(sender, "MODE -> Invalid modification's source.", "MODE Command failed.");
			if (!(chan = findChannel(cmd.field1())))
				return logError(sender, "MODE -> Targeted channel does not exist.", "MODE Command failed.");
			while (!modes.empty()) {
				if (!chan->applyModeFlag(modes[0], set))
					return logError(sender, "MODE -> Invalid mode string \'" + modes + "\'.", "MODE Command failed.");
				modes.erase(0, 1);
			}
			broadcastToServers(sender, cmd);
		}
		else {
			if (!(source = findClient(cmd.prefix, NICKNAME)))
				return logError(sender, "MODE -> Invalid modification's source.", "MODE Command failed.");
			if (!(target = findClient(cmd.field1(), NICKNAME)))
				return logError(sender, "MODE -> Targeted user does not exist.", "MODE Command failed.");
			if (target->nickname.compare(source->nickname))
				return logError(sender, "MODE -> Target does not match the source.", "MODE Command failed.");
			while (!modes.empty()) {
				try { target->applyModeFlag(modes[0], set); }
				catch (Command::InvalidCommand &ex) {
					if (modes[0] == CLIENT_FLAG_AWAY || modes[0] == CLIENT_FLAG_OPERATOR) {
						if (set)
							target->setModeFlag(modes[0]);
						else
							target->unsetModeFlag(modes[0]);
					} else
						return logError(sender, "MODE -> Invalid mode string \'" + modes + "\'.", "MODE Command failed.");
				}
				modes.erase(0, 1);
			}
			broadcastToServers(sender, cmd);
		}
	}
	else { // user changed a channel member's mode
		if (!(source = findClient(cmd.prefix, NICKNAME)))
			return logError(sender, "MODE -> Invalid modification's source.", "MODE Command failed.");
		if (!chan->isOnChan(source))
			return logError(sender, "MODE -> Invalid modification's source.", "MODE Command failed.");
		if (!(chan = findChannel(cmd.field1())))
			return logError(sender, "MODE -> Targeted channel does not exist.", "MODE Command failed.");
		if (!(target = findClient(cmd.field3(), NICKNAME)))
			return logError(sender, "MODE -> Targeted user does not exist.", "MODE Command failed.");
		else if (!chan->isOnChan(target))
			return logError(sender, "MODE -> Targeted user not inside channel.", "MODE Command failed.");
		try { chan->applyUserModeFlag(source, target, modes[0], set); }
		catch (Command::InvalidCommand &ex) {
			if (ex.code == ERR_CHANOPRIVSNEEDED)
				return logError(sender, "MODE -> Invalid modification's source.", "MODE Command failed.");
			return logError(sender, "MODE -> Invalid mode string \'" + modes + "\'.", "MODE Command failed.");
		}
		chan->send(nullptr, ModeCommand(source->fullId(), cmd.args));
	}
}

void		LocalServer::execJoin(Server *sender, JoinCommand const &cmd) {
	unsigned short code;
	std::string	names;
	std::string token;
	Client		*source;
	Channel		*chan;

	cmd.isValid();
	names = cmd.target();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return logError(sender, "JOIN -> Command\'s source does not exist.", "JOIN Command failed.");
	while(!names.empty()) {
		token = Utils::getToken(names, ",");
		if (!Utils::validChanName(token))
			return logError(sender, "JOIN -> Invalid channel name \'" + token + "\'.", "JOIN Command failed.");
		else {
			if (!(chan = findChannel(token))) {
				if (!(chan = newChan(source, token)))
					return logNotify(true, "New \'Channel\' created Internal error.");
			}
			else if ((code = chan->join(source))) { // add key arg later for -k mode
				continue; // treat error codes (banned, not invited, ...)
			}
		}
	}
	broadcastToServers(sender, cmd);
}

void		LocalServer::execPart(Server *sender, PartCommand const &cmd) {
	Client	*source;
	Channel	*chan;

	cmd.isValid();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return logError(sender, "PART -> Command\'s source does not exist.", "PART Command failed.");
	if (!(chan = findChannel(cmd.target())))
		return logError(sender, "PART -> Invalid channel name \'" + cmd.target() + "\'.", "PART Command failed.");
	else if (!chan->isOnChan(source))
		return logError(sender, "PART -> Target not inside channel \'" + cmd.target() + "\'.", "PART Command failed.");
	else {
		chan->updateClients(source, cmd);
		if (chan->empty())
			finishChan(chan);
		broadcastToServers(sender, cmd);
	}
}

void		LocalServer::execPrivmsg(Server *sender, PrivmsgCommand const &cmd) {
	Client	*source;
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return logError(sender, "PRIVMSG -> Command\'s source does not exist.", "PRIVMSG Command failed.");
	if ((tmp = findClient(cmd.target(), NICKNAME))) {
		if (tmp->isLink())
			tmp->link->send(cmd);
		else
			tmp->send(PrivmsgCommand(source->fullId(), cmd.args));
	}
	else if ((chan = findChannel(cmd.target()))) {
		chan->send(source, PrivmsgCommand(source->fullId(), cmd.args));
		broadcastToServers(sender, cmd);
	}
	else
		return logError(sender, "PRIVMSG -> Command\'s target does not exist.", "PRIVMSG Command failed.");
}

void		LocalServer::execNotice(Server *sender, NoticeCommand const &cmd) {
	Client	*source;
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return;
	if ((tmp = findClient(cmd.target(), CLIENTNAME))) {
		if (tmp->isLink())
			tmp->link->send(cmd);
		else
			tmp->send(NoticeCommand(source->fullId(), cmd.args));
	}
	else if ((chan = findChannel(cmd.target()))) {
		chan->send(source, NoticeCommand(source->fullId(), cmd.args));
		broadcastToServers(sender, cmd);
	}
	else
		return;
}

void		LocalServer::execQuit(Server *sender, QuitCommand const &cmd) {
	Client		*source;

	cmd.isValid();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return logError(sender, "QUIT -> Command\'s source does not exist.", "QUIT Command failed.");
	finishConx(sender, source, true, cmd.message());
}

void		LocalServer::execSquit(Server *sender, SquitCommand const &cmd) {
	Server		*target;

	cmd.isValid();
	if (!cmd.server().compare(servername))
		finishConx(nullptr, sender, true, SquitCommand(NO_PREFIX, sender->servername, "Rogered a SQUIT command."));
	else if (!(target = findServer(cmd.server())))
		return logError(sender, "SQUIT -> Command\'s target does not exist.", "SQUIT Command failed.");
	else if (cmd.prefix.empty())
		finishConx(sender, target, true, SquitCommand(sender->servername, cmd.server(), cmd.comment()));
	else	
		finishConx(sender, target, true, cmd);
}

// ###### CLIENT ######

void		LocalServer::execNick(Client *sender, NickCommand const &cmd) {
	cmd.isValid();
	if (!Utils::validNickName(cmd.nickname()))
		return numericReply(sender, ERR_ERRONEOUSNICKNAME, cmd.nickname());
	if (_nicknames.count(cmd.nickname()))
		return numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	if (!sender->isRegistered()) {
		if (sender->nickname.empty()) {
			sender->nickname = cmd.nickname();
			if (howMany(CONX_CLIENT, true) == 1)
				sender->isLocalop(true);
			numericReply(sender, RPL_WELCOME, sender->fullId());
			numericReply(sender, RPL_YOURHOST);
			numericReply(sender, RPL_CREATED);
			numericReply(sender, RPL_MYINFO);
			broadcastClient(nullptr, sender);
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
	if (!Utils::validUserName(cmd.username()))
		return numericReply(sender, ERR_ERRONEOUSUSERNAME, cmd.username());
	if (!Utils::validRealName(cmd.realname()))
		return numericReply(sender, ERR_ERRONEOUSREALNAME, cmd.realname());
	if (!sender->isRegistered()) {
		if (sender->username.empty()) {
			sender->username = cmd.username();
			sender->realname = cmd.realname();
			if (howMany(CONX_CLIENT, true) == 1)
				sender->isLocalop(true);
			numericReply(sender, RPL_WELCOME, sender->fullId());
			numericReply(sender, RPL_YOURHOST);
			numericReply(sender, RPL_CREATED);
			numericReply(sender, RPL_MYINFO);
			broadcastClient(nullptr, sender);
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
	if ((tmp = findClient(cmd.target(), NICKNAME))) {
		if (tmp->isAway())
			numericReply(sender, RPL_AWAY, tmp);
		else if (tmp->isLink())
			tmp->link->send(PrivmsgCommand(sender->nickname, cmd.args));
		else
			tmp->send(PrivmsgCommand(sender->fullId(), cmd.args));
	}
	else if ((chan = findChannel(cmd.target()))) {
		chan->send(sender, PrivmsgCommand(sender->fullId(), cmd.args));
		broadcastToServers(nullptr, PrivmsgCommand(sender->nickname, cmd.args));
	}
	else
		numericReply(sender, ERR_NOSUCHNICK, cmd.target());
}

void		LocalServer::execNotice(Client *sender, NoticeCommand const &cmd) {
	Channel	*chan;
	Client	*tmp;

	cmd.isValid();
	if ((tmp = findClient(cmd.target(), CLIENTNAME))) {
		if (tmp->isLink())
			tmp->link->send(NoticeCommand(sender->nickname, cmd.args));
		else
			tmp->send(NoticeCommand(sender->fullId(), cmd.args));
	}
	else if ((chan = findChannel(cmd.target()))) {
		chan->send(sender, NoticeCommand(sender->fullId(), cmd.args));
		broadcastToServers(nullptr, NoticeCommand(sender->nickname, cmd.args));
	}
}

void		LocalServer::execQuit(Client *sender, QuitCommand const &cmd) {
	logNotify(true, "Client \'" + sender->name() + "\' is leaving.");
	finishConx(nullptr, sender, false, cmd.message());
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
			logNotify(true, "Error while reading MOTD file.");
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
					return logNotify(true, "New \'Channel\' created Internal error.");
			}
			else if ((code = chan->join(sender))) { // add key arg later for -k mode
				continue; // treat error codes (banned, not invited, ...)
			}
			if (chan->hasTopic())
				numericReply(sender, RPL_TOPIC, chan);
			execNames(sender, NamesCommand(NO_PREFIX, chan->getName()));
		}
	}
	broadcastToServers(nullptr, JoinCommand(sender->nickname, cmd.target()));
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
		broadcastToServers(nullptr, PartCommand(sender->nickname, cmd.args));
	}
}

void		LocalServer::execTopic(Client *sender, TopicCommand const &cmd) {
	Channel	*chan;

	cmd.isValid();
	if (!(chan = findChannel(cmd.target())))
		numericReply(sender, ERR_NOSUCHCHANNEL, cmd.target());
	else if (!chan->isOnChan(sender))
		numericReply(sender, ERR_NOTONCHANNEL, cmd.target());
	else {
		if (cmd.argNbr() == 2) {
			if (chan->isTopic() && !chan->isOperator(sender))
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
	if (sender->isLocalop()) {
		cmd.isValid();
		setOppass(cmd.password());
		logNotify(false, "A new operator password as been set !");
	}
	else
		numericReply(sender, ERR_NOTLOCALOP);
}

void		LocalServer::execOper(Client *sender, OperCommand const &cmd) {
	cmd.isValid();
	if (!cmd.password().compare(_oppass)) {
		if (sender->isOperator())
			return;
		sender->isOperator(true);
		numericReply(sender, RPL_YOUREOP);
		logNotify(false, "[" + sender->nickname + "] is now an IRC operator !");
		broadcastToServers(nullptr, ModeCommand(sender->name(), sender->name(), ":+o"));
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
			throw (Command::InvalidCommand(ERR_NOSUCHCHANNEL));
		return numericReply(sender, RPL_CHANNELMODEIS, chan);
	}
	else if (cmd.argNbr() == 2) {
		if (cmd.field1()[0] == '#' || cmd.field1()[0] == '&') {
			if (!(chan = findChannel(cmd.field1())))
				throw (Command::InvalidCommand(ERR_NOSUCHCHANNEL));
			mode = cmd.field2();
			if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
				throw (Command::InvalidCommand(ERR_UMODEUNKNOWNFLAG));
			chan->applyModeFlag(sender, mode[1], (mode[0] == '+' ? true : false));
		}
		else {
			if (!(target = findClient(cmd.field1(), NICKNAME)))
				throw (Command::InvalidCommand(ERR_USERSDONTMATCH));
			if (target->nickname.compare(sender->nickname))
				throw (Command::InvalidCommand(ERR_USERSDONTMATCH));
			mode = cmd.field2();
			if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
				throw (Command::InvalidCommand(ERR_UMODEUNKNOWNFLAG));
			target->applyModeFlag(mode[1], (mode[0] == '+' ? true : false));
			target->send(ModeCommand(target->fullId(), cmd.args));
		}
	}
	else {
		if (!(chan = findChannel(cmd.field1())))
			throw (Command::InvalidCommand(ERR_NOSUCHCHANNEL));
		mode = cmd.field2();
		if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
			throw (Command::InvalidCommand(ERR_UNKNOWNMODE));
		if (!(target = findClient(cmd.field3(), NICKNAME)))
			throw (Command::InvalidCommand(ERR_USERNOTINCHANNEL));
		else if (!chan->isOnChan(target))
			throw (Command::InvalidCommand(ERR_USERNOTINCHANNEL));
		chan->applyUserModeFlag(sender, target, mode[1], (mode[0] == '+' ? true : false));
		chan->send(nullptr, ModeCommand(sender->fullId(), cmd.args));
	}
	broadcastToServers(nullptr, ModeCommand(sender->name(), cmd.args));
}

void		LocalServer::execAway(Client *sender, AwayCommand const &cmd) {
	cmd.isValid();
	if (cmd.argNbr(1) && !cmd.message().empty()) {
		sender->isAway(true);
		sender->awaymsg = cmd.message();
		numericReply(sender, RPL_NOWAWAY);
		broadcastToServers(nullptr, ModeCommand(sender->name(), sender->name(), ":+a"));
		return logNotify(false, "[" + sender->nickname + "] is now marked as being away !");
	}
	sender->isAway(false);
	numericReply(sender, RPL_UNAWAY);
	broadcastToServers(nullptr, ModeCommand(sender->name(), sender->name(), ":-a"));
	logNotify(false, "[" + sender->nickname + "] is no longer away !");
}

void		LocalServer::execNames(Client *sender, NamesCommand const &cmd) {
	std::list<std::string> lst;
	Channel *chan;

	cmd.isValid();
	if (cmd.argNbr(0)) {
		if (!_chans.empty()) {
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

	cmd.isValid();
	if (cmd.argNbr(0) || !cmd.argX(0).compare("0") || !cmd.argX(0).compare("*")) {
		for (conxlist_it it = _conxs.begin(); it != _conxs.end(); it++) {
			if ((*it)->isClient() && !sender->sharedChans(*it))
				numericReplyWho(sender, nullptr, static_cast<Client *>(*it));
		}
		numericReply(sender, RPL_ENDOFWHO, "*");
	}
	else {
		if ((chan = findChannel(cmd.mask()))) {
			chan->clientsList(lst);
			for (Channel::clientlist_it it = lst.begin(); it != lst.end(); it++)
				numericReplyWho(sender, chan, (*it));
		}
		numericReply(sender, RPL_ENDOFWHO, cmd.mask());
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 NUMERIC REPLIES
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::numericReply(Connection *sender, unsigned short code) {
	Command ret(servername, codeToStr(code), Command::argvec());

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
	Command ret(servername, codeToStr(code), Command::argvec());

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
		case ERR_ERRONEOUSUSERNAME: ret.addArg(field);
									ret.addArg("Erroneous username"); break;
		case ERR_ERRONEOUSREALNAME: ret.addArg(field);
									ret.addArg("Erroneous realname"); break;
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
	Command ret(servername, codeToStr(code), Command::argvec());

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
	Command ret(servername, codeToStr(code), Command::argvec());

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
	Command ret(servername, codeToStr(code), Command::argvec());

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
		if (!target->isLink())
			ret.addArg(servername);
		else
			ret.addArg(target->link->name());
		ret.addArg("Active on this server"); // change it to whatever information about the client later
	}
	else if (code == RPL_AWAY) {
		ret.addArg(target->nickname);
		ret.addArg(target->awaymsg);
	}
	sender->send(ret);
}

void		LocalServer::numericReplyNames(Client *sender, std::string const &chan, std::list<std::string> &names) {
	std::list<std::string>::iterator itnames;
	std::string			tmp;

	while (!names.empty()) {
		Command ret(Command(servername, codeToStr(RPL_NAMESREPLY), Command::argvec()));
		itnames = names.begin();
		tmp.clear();
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
	Command ret(servername, codeToStr(RPL_WHOREPLY), Command::argvec());

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
		Command ret(servername, codeToStr(RPL_WHOISCHANNELS), Command::argvec());
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 LOG FUNCTIONS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::logNotify(bool error, std::string const &msg) {
	_logfile.append(LOG_DATE, (error ? LOG_DARKRED : LOG_DARKGREY), msg);
}

void		LocalServer::logSuccess(std::string const &msg) {
	_logfile.append(LOG_DATE, LOG_GREEN, msg);
}

void		LocalServer::logColored(std::string const &color, std::string const &msg) {
	_logfile.append(LOG_DATE, color, msg);
}

void		LocalServer::logPromote(std::string const &rank, std::string const &reason, bool success) {
	if (!success)
		logColored(LOG_DARKRED, "Promotion \'Unknown\' -> \'" + rank + "\': " + reason);
	else if (!rank.compare("Server"))
		logColored(LOG_BLUE, "Promotion \'Unknown\' -> \'" + rank + "\': " + reason);
	else if (!rank.compare("Client"))
		logColored(LOG_GREEN, "Promotion \'Unknown\' -> \'" + rank + "\': " + reason);
	else if (!rank.compare("Link"))
		logColored(LOG_YELLOW, "Promotion \'Unknown\' -> \'" + rank + "\': " + reason);
}

void		LocalServer::logError(Connection *target, std::string const &msg) {
	logNotify(true, msg);
	if (target->isClient())
		finishConx(nullptr, static_cast<Client *>(target), true, msg);
	else if (target->isServer())
		finishConx(nullptr, static_cast<Server *>(target), true, SquitCommand(NO_PREFIX, static_cast<Server *>(target)->servername, msg));
	else
		finishConx(target, true);
}

void		LocalServer::logError(Connection *target, std::string const &msg, std::string const &errmsg) {
	logNotify(true, msg);
	if (!errmsg.empty())
		target->send(ErrorCommand(errmsg));
	if (target->isClient())
		finishConx(nullptr, static_cast<Client *>(target), false, msg);
	else if (target->isServer())
		finishConx(nullptr, static_cast<Server *>(target), false, SquitCommand(NO_PREFIX, static_cast<Server *>(target)->servername, msg));
	else
		finishConx(target, false);
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
		DEBUG_DDISPC(COUT, "->", *(*it), DARK_GREEN);
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
			if (tmp->isClient()) {
				if (tmp->isLink())
					DEBUG_LDISPCB(COUT, "Client*\t", *(static_cast<Client *>(tmp)), YELLOW, '{');
				else
					DEBUG_LDISPCB(COUT, "Client\t", *(static_cast<Client *>(tmp)), GREEN, '{');
			}
			else if (tmp->isServer()){
				if (tmp->isLink())
					DEBUG_LDISPCB(COUT, "Server*\t", *(static_cast<Server *>(tmp)), YELLOW, '{');
				else
					DEBUG_LDISPCB(COUT, "Server\t", *(static_cast<Server *>(tmp)), BLUE, '{');
			}
			else
				DEBUG_LDISPCB(COUT, "Pending\t", *tmp, ORANGE, '{');
		}
	}
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	DEBUG_BAR_DISPC(COUT, '=', 45, ORANGE);
}
