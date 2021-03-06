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
**----- Updated -------------{ 2022-02-24 18:21:30 }--------------------------**
********************************************************************************
*/

#include "LocalServer.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
LocalServer::~LocalServer() {
	_log.append(LOG_DATE, LOG_PURPLE, "\t\t+ Server closed successfuly +");
	_log.append(LOG_DATE, LOG_PURPLE, "###############################################");
	_log.end();
	return;
}
// LocalServer::LocalServer() { return; } /* Private standard constructor*/
LocalServer::LocalServer(LocalServer const &src) :
	inherited(static_cast<inherited const &>(src)), _sm(src.getSM()),
	_password(src.getPassword()), _oppass(src.getOppass()),
	_whitelist(src.getWhitelist()), _log(src.getLog()),
	_motd(src.getMotd()) { return; }
LocalServer &LocalServer::operator=(LocalServer const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	_sm = src.getSM();
	_password = src.getPassword();
	_oppass = src.getOppass();
	_whitelist = src.getWhitelist();
	_log = src.getLog();
	_motd = src.getMotd();
	return *this;
}

// _____________Constructor______________
LocalServer::LocalServer(std::string const &port, u_int16_t family, std::string const &password) :
inherited("", port, family), _sm(sock(), true), _password(password), _log("") {
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
	if (!!(_log = LogFile(conf["logfile"])))
		throw(Connection::ConxInit("Config file requires valid log file name. \'logfile = [filename]\'"));
	if ((_whitelist = ConfigParser(LOCALSERV_WHITELIST_FILE)).empty())
		logNotify(false, "Empty whitelist: server launched in standalone.");
	loadMotd();
	if (std::signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		throw(Connection::ConxInit("signal() function failed. Cannot start server with SIGPIPE not ignored."));
}

// __________Member functions____________

bool		LocalServer::run(void) {
	_log.append(LOG_DATE, LOG_PURPLE, "\t[" + std::string(SERVER_VERSION) + "] ################################");
	_log.append(LOG_DATE, LOG_PURPLE, "\t\t+ Server launched successfuly +");
	while(!isFinished() || !conxs.empty()) {
		try { selectCall(); }
		catch (SelectModule::SelectException &ex) {
			finishConx(nullptr, this, "Internal error.", false);
		}
		checkStd();
		checkSock();
		checkConxs();
	}
	return true;
}

bool		LocalServer::joinNet(std::string const &authinfo) {
	Connection *tmp;
	std::string host;
	std::string port;
	std::string password;

	if (!Utils::splitAuthInfo(authinfo, host, port, password)) {
		logNotify(true, "Joining \'Server\': Erroneous authentication's info !");
		return false;
	}
	if (!isWhitelisted(authinfo, false)) {
		logNotify(true, "Joining \'Server\' -> [" + host + "]: Not whitelisted !");
		return false;
	}
	try {
		tmp = new Connection(host, port, info.family());
	} catch (std::exception &ex) {
		logNotify(true, ex.what());
		logNotify(true, "Joining \'Server\' -> [" + host + "]: failed !");
		return false;
	}
	logSuccess("Joining \'Server\' -> [" + host + "]");
	conxs.push_back(tmp);
	_sm.addFd(tmp->sock());
	tmp->send(PassCommand(password));
	tmp->send(ServerCommand(serverArgsConnect()));
	tmp->isConnect(true);
	return true;
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
		return logNotify(true, "New \'Unknown\' connection: failed !");
	}
	conxs.push_back(tmp);
	_sm.addFd(conxs.back()->sock());
	if (MAX_CONXS < conxs.size()) {
		tmp->send(ErrorCommand("Server is full."));
		return finishConx(tmp);
	} else
		logSuccess("New \'Unknown\' connection: [" + tmp->name() + "]");
}

Server		*LocalServer::newLink(Server *sender, ServerCommand const &cmd) {
	Server	*svr;
	std::string modes;
	std::string	tmp;

	try { svr = new Server(sender, cmd); }
	catch (std::exception &ex) {
		logError(sender, "Internal error from \'newLink\' !", "Internal error");
		return (nullptr);
	}
	sender->conxs.push_back(svr);
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
	sender->conxs.push_back(cli);
	cli->nickname = cmd.nickname();
	cli->username = cmd.username();
	cli->realname = cmd.realname();
	cli->info.host = cmd.host();
	modes = cmd.umode();
	while (!modes.empty()) {
		cli->setModeFlag(modes[0]);
		modes.erase(0, 1);
	}
	return cli;
}

void		LocalServer::finishConx(Server *sender, Client *target, std::string const &quitmsg, bool netsplit) {
	if (!target)
		return;
	if (!target->isLink()) {
		target->clearInputMessages();
		target->clearOutputMessages();
		if (quitmsg.empty())
			target->send(ErrorCommand("[" + target->name() + "] Closing connection"));
		else
			target->send(ErrorCommand("[" + target->name() + "] Closing connection (" + quitmsg + ")"));
		if (!netsplit)
			broadcastToServers(sender, QuitCommand(target->nickname, quitmsg));
		logNotify(true, "Ending \'Client\' connection -> [" + target->name() + "]");
	} else
		logNotify(true, "Ending \'Client *\' connection -> [" + target->name() + "]");
	target->isFinished(true);

	if (target->isLocalop())
		setNewLocalop();

	// leaves channels if necessary
	updateChans(target, QuitCommand(target->fullId(), quitmsg));
}

void		LocalServer::finishConx(Server *sender, Server *target, std::string const &quitmsg, bool netsplit) {
	if (!target)
		return;
	target->isFinished(true);
	if (!target->isLink()) {
		target->clearInputMessages();
		target->clearOutputMessages();
		if (!netsplit && target->compare(this))
			target->send(SquitCommand(name(), target->name(), quitmsg));
		for (conxlist_it it = target->conxs.begin(); it != target->conxs.end(); it++) {
			if ((*it)->isClient())
				finishConx(nullptr, static_cast<Client *>(*it), quitmsg, true);
			else if ((*it)->isServer()) {
				finishConx(nullptr, static_cast<Server *>(*it), quitmsg, true);
				if (netsplit && !target->compare(this))
					(*it)->send(SquitCommand(name(), (*it)->name(), quitmsg));
			}
			else {
				(*it)->send(ErrorCommand("Server shutdown."));
				finishConx(*it);
			}
		}
		if (!target->compare(this))
			logColored(LOG_PURPLE, "Closing \'Local Server\' [" + target->name() + "]");
		else
			logNotify(true, "Ending \'Server\' connection -> [" + target->name() + "]");
	} else
		logNotify(true, "- Ending \'Server *\' connection -> [" + target->name() + "]");
	if (target->isLink() && netsplit)
		return;
	if (target->compare(this)) {
		if (sender)
			broadcastToServers(sender, SquitCommand(sender->name(), target->name(), quitmsg));
		else
			broadcastToServers(target, SquitCommand(name(), target->name(), quitmsg));
	}
}

void		LocalServer::finishConx(Connection *target) {
	if (!target)
		return;
	target->isFinished(true);
	target->clearInputMessages();
	logNotify(true, "Ending \'Unknown\' connection -> [" + target->name() + "]");
}

void		LocalServer::setNewLocalop(void) {
	bool	hasClient = false;

	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if (!(*it)->isFinished() && (*it)->isClient()) {
			if (!hasClient)
				hasClient = true;
			if (static_cast<Client *>(*it)->isOperator()) {
				static_cast<Client *>(*it)->isLocalop(true);
				return warnAllOperators("The new local operator is \'" + (*it)->name() + "\'");
			}
		}
	}
	if (!hasClient)
		return;
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if (!(*it)->isFinished() && (*it)->isClient() && static_cast<Client *>(*it)->isWallops()) {
			static_cast<Client *>(*it)->isLocalop(true);
			return warnAllOperators("The new local operator is \'" + (*it)->name() + "\'");
		}
	}
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if (!(*it)->isFinished() && (*it)->isClient() && !static_cast<Client *>(*it)->isRestricted()) {
			static_cast<Client *>(*it)->isLocalop(true);
			return warnAllOperators("The new local operator is \'" + (*it)->name() + "\'");
		}
	}
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if (!(*it)->isFinished() && (*it)->isClient()) {
			static_cast<Client *>(*it)->isLocalop(true);
			return warnAllOperators("The new local operator is \'" + (*it)->name() + "\'");
		}
	}
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
			logNotify(true, "Channel \'" + target->getName() + "\' deleted, no members left.");
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

bool		LocalServer::isWhitelisted(std::string const &info, bool svrname) const {
	if (_whitelist.empty())
		return false;
	for (ConfigParser::fieldmap_cit it = _whitelist.cbegin(); it != _whitelist.cend(); it++) {
		if ((svrname && !it->first.compare(info)) || (!svrname && !it->second.compare(info)))
			return true;
	}
	return false;
}

bool		LocalServer::checkWhitelistHost(std::string const &svrname, std::string const &host) const {
	if (_whitelist.empty())
		return false;
	for (ConfigParser::fieldmap_cit it = _whitelist.cbegin(); it != _whitelist.cend(); it++) {
		if (!it->first.compare(svrname)) {
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

bool		LocalServer::checkWhitelistInfos(std::string const &svrname, std::string const &port) const {
	std::string hostname;
	std::string password;

	hostname = whitelistHost(svrname);
	password = whitelistPassword(svrname);
	if (svrname.empty() || !isWhitelisted(svrname, true))
		return false;
	if (!Utils::validPassword(password) || !Utils::validServName(hostname))
		return false;
	if (!Utils::validPort(port) || port.compare(whitelistPort(svrname)))
		return false;
	if (!isWhitelisted(hostname + ":" + port + ":" + password, false))
		return false;
	return true;
}

std::string const LocalServer::whitelistInfos(std::string const &svrname) const {
	if (_whitelist.empty())
		return std::string();
	else
		return _whitelist[svrname];
}

std::string const LocalServer::whitelistHost(std::string const &svrname) const {
	size_t pos = 0;

	if (_whitelist.empty())
		return std::string();
	if ((pos = _whitelist[svrname].find(':')) != std::string::npos)
		return (_whitelist[svrname].substr(0, pos));
	else
		return std::string();
}

std::string const LocalServer::whitelistPort(std::string const &svrname) const {
	size_t pos = 0;

	if (_whitelist.empty())
		return std::string();
	if ((pos = _whitelist[svrname].find(':')) != std::string::npos) {
		if (pos < _whitelist[svrname].size())
			return (_whitelist[svrname].substr(pos + 1, 4));
	}
	return std::string();
}

std::string const LocalServer::whitelistPassword(std::string const &svrname) const {
	size_t pos = 0;

	if (_whitelist.empty())
		return std::string();
	if ((pos = _whitelist[svrname].rfind(':')) != std::string::npos) {
		if (pos < _whitelist[svrname].size())
			return (_whitelist[svrname].substr(pos + 1));
	}
	return std::string();
}

void		LocalServer::newToken(void) {
	logNotify(false, "Generating new token for server...");
	inherited::newToken();
	if (!token.compare("none"))
		return finishConx(nullptr, this, "Internal error.", false);
	logNotify(false, "Generation done. New token is [" + token + "].");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//			 BROADCAST
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void	LocalServer::broadcastServer(Server *sender, Server *shared) {
	std::string tmp;

	if (!shared)
		return;
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer() && !(*it)->isLink() && static_cast<Server *>(*it)->compare(shared)) {
			if (!sender || sender->compare(*it)) {
				(*it)->send(ServerCommand(name(), shared->serverArgsShare(token)));
				newToken();
			}
		}
	}
}

void	LocalServer::broadcastToServers(Server *sender, Command const &cmd) {
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer() && !(*it)->isLink()) {
			if (!sender || sender->compare(static_cast<Server *>(*it)))
				(*it)->send(cmd);
		}
	}
}

void	LocalServer::broadcastClient(Server *sender, Client *shared) { // std::string const &tk
	// Server		*origin;
	// std::string tmp;

	if (!shared)
		return;
	// origin = (sender && sender->isToken(tk) ? sender : getTokenSource(tk));
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer() && !(*it)->isLink()) {
			if (!sender || sender->compare(*it)) {
				(*it)->send(NickCommand(name(), shared->nickArgs(static_cast<Server *>(*it)->token)));
			}
		}
	}
}

void	LocalServer::broadcastToClients(Client *sender, Command const &cmd) {
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isClient() && !(*it)->isLink()) {
			if (!sender || sender->compare(static_cast<Client *>(*it)))
				(*it)->send(cmd);
		}
	}
}

void	LocalServer::broadcastToSomeClients(Command const &cmd, clicheck fp) {
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isClient() && (static_cast<Client *>(*it)->*fp)())
			(*it)->send(cmd);
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

void	LocalServer::warnAllOperators(std::string const &msg) {
	broadcastToSomeClients(WallopsCommand(name(), msg), static_cast<clicheck>(&Client::isWallops));
}

std::string const 	LocalServer::howManyChannel(void) const { return Utils::nbrToStr(_chans.size()); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		  SELECT MODULE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::selectCall(void) {
	std::string stdtmp;
	std::string logtmp;

	if (_stdout) {
		stdtmp = Utils::readSomeCXX(_stdout, MAX_IOSTEP_LENGTH);
		if (_stdout.eof()) {
			if (!_stdout.bad())
				_stdout.clear();
			else
				logNotify(true, "Standard output died.");
		}
	}
	if (!!_log) {
		logtmp = Utils::readSomeCXX(_log.buf, MAX_IOSTEP_LENGTH);
		if (_log.buf.eof()) {
			if (!_log.buf.bad())
				_log.buf.clear();
			else
				_log.setErrorState();
		}
	}
	_sm.call(conxs, stdtmp, _log, logtmp, isFinished());
}

bool		LocalServer::isReadable(Connection *conx) const { return _sm.checkRfds(conx->sock()); }
bool		LocalServer::isWritable(Connection *conx) const { return _sm.checkWfds(conx->sock()); }

void		LocalServer::checkStd(void) {
	std::string cmd;
	std::string input;

	if (isFinished() || std::cin.bad())
		return;
	if (_sm.checkStdin()) {
		if (std::cin) {
			input = Utils::readSomeCXX(std::cin, MAX_IOSTEP_LENGTH);
			Utils::clearSpaces(input, false);
			if (!input.compare("\n") && _stdin.empty())
				return std::cin.clear();
			_stdin += input;
			if (_stdin.length() > MAX_IOSTEP_LENGTH)
				_stdin.erase(0, _stdin.length() - _stdin.length() % MAX_IOSTEP_LENGTH);
		}
		if (std::cin.eof()) {
			if (input.empty())
				return finishConx(nullptr, this, "Console shutdown. \'^D\'", false);
			else if (!std::cin.bad())
				std::cin.clear();
			else {
				_stdin.clear();
				return logNotify(true, "Standard input died.");
			}
		}
		while (_stdin.find('\n') != STRNPOS) {
			cmd = _stdin.substr(0, _stdin.find('\n'));
			Utils::graphicalOnly(cmd);
			_stdin.erase(0, _stdin.find('\n') + 1);
			if (cmd.empty())
				continue;
			if (!cmd.compare("EXIT") || !cmd.compare("Exit") || !cmd.compare("exit"))
				finishConx(nullptr, this, "Console shutdown. \'exit\')", true);
			else if (!cmd.compare("NET") || !cmd.compare("Net") || !cmd.compare("net"))
				showNet();
			else if (!cmd.compare("CHANS") || !cmd.compare("Chans") || !cmd.compare("chans"))
				showChans();
			else {
				_stdout << "Unknown command \'" << cmd << "\'" << std::endl;
				continue;
			}
			_log.append(LOG_DATE, LOG_PINK, "Command line input $> [" + cmd + "]");
		}
	}
}

void		LocalServer::checkSock(void) {
	if (isReadable(static_cast<Connection *>(this)))
		newConx();
}

void		LocalServer::checkConxs(void) {
	for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
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
			while (!(*it)->isFinished() && (*it)->hasInputMessage())
				execCommand(*it, (*it)->getLastCommand());
			if (isWritable(*it)) {
				try { (*it)->write(); }
				catch (SockStream::FailSend &ex) {
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
				logNotify(true, "Source: [" + cmd.command + "]");
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
			finishConx(sender);
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
			finishConx(sender);
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
	else if (cmd == "SQUIT")
		execSquit(sender, SquitCommand(cmd));
	else if (cmd == "CONNECT")
		execConnect(sender, ConnectCommand(cmd));
	else if (cmd == "INVITE")
		execInvite(sender, InviteCommand(cmd));
	else if (cmd == "KICK")
		execKick(sender, KickCommand(cmd));
	else
		return numericReply(sender, ERR_UNKNOWNCOMMAND, cmd.command);
	logNotify(false, "~{" + cmd.command + "}~ from client [" + sender->name() + "] executed successfuly.");
}

void		LocalServer::execCommandServer(Server *sender, Command const &cmd) {
	logNotify(false, "~{" + cmd.command + "}~ from server [" + sender->name() + "] received.");
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
	else if (cmd == "INVITE")
		execInvite(sender, InviteCommand(cmd));
	else if (cmd == "KICK")
		execKick(sender, KickCommand(cmd));
	else
		logError(sender, "Unknown command received \'" + cmd.command + "\'.", "Unknown or forbidden command");
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
	if (findClient(cmd.nickname(), NICKNAME))
		return numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	try {
		sender = new Client(sender, cmd);
	} catch (std::exception &ex) {
		logPromote("Client", "Internal error.", false);
		return finishConx(sender);
	}
	logPromote("Client", "[" + cmd.nickname() + "]", true);
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
		return finishConx(sender);
	}
	logPromote("Client", "[" + cmd.username() + "]", true);
}

void		LocalServer::execServer(Connection *&sender, ServerCommand const &cmd) {
	std::string tmp;

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
	if (!sender->isConnect()) { // we accepted sender so we provide the token
		if (cmd.argNbr(4))
			return logError(sender, "Accepted potential Server gave a token.", "Access denied.");
		sender->send(PassCommand(whitelistPassword(cmd.servername())));
		sender->send(ServerCommand(serverArgsAccept()));
		static_cast<Server *>(sender)->token = token;
		newToken();
	} else if (cmd.token().empty() || !cmd.token().compare("none"))
		return logError(sender, "Potential Server gave an error token.", "Access denied.");
	warnAllOperators("New server \'" + sender->name() + "\' connected.");
	inherited::shareServs(this, static_cast<Server *>(sender));
	inherited::shareClients(static_cast<Server *>(sender));
	shareChans(static_cast<Server *>(sender));
	broadcastServer(nullptr, static_cast<Server *>(sender));
}

// ###### SERVER ######

void		LocalServer::execNick(Server *sender, NickCommand const &cmd) {
	Client	*cli;
	Server	*svr;

	cmd.isValid();
	if ((cli = findClient(cmd.prefix, NICKNAME))) {
		if (cmd.argNbr(1)) {
			if (findClient(cmd.nickname(), NICKNAME)) {
				logPromote("Link", "Collision between two nicknames.", false);
				return logError(sender, "", "Spanning tree broken for this connection.");
			}
			updateChans(cli, cmd);
			cli->nickname = cmd.nickname();
			broadcastToServers(sender, cmd);
		} else {
			logNotify(true, "NICK -> Wrong arguments\' number.");
			logError(sender, "", "Erroneous command.");
		}
	} else if ((svr = findServer(cmd.prefix))) {
		if (findClient(cmd.nickname(), NICKNAME)) { // find client
			logPromote("Link", "Collision between two nicknames.", false);
			logError(sender, "", "Spanning tree broken for this connection.");
		}
		else if (Utils::validNickName(cmd.nickname()) && sender->knowToken(cmd.servertoken()) &&
		Utils::validUserName(cmd.username()) && Utils::validRealName(cmd.realname()) &&
		Utils::checkNbr(cmd.hopcount())) {
			logPromote("Link", "[" + cmd.nickname() + "!" + cmd.username() + "@" + cmd.host() + "]", true);
			broadcastClient(sender, newLink(sender, cmd));
		} else {
			logPromote("Link", "Failed !", false);
			logNotify(true, "Server \'" + sender->name() + "\' sent an erroneous \'" + cmd.command + "\' command !");
			logError(sender, "", "Erroneous command.");
		}
	} else {
		logNotify(true, "NICK -> No prefix.");
		logError(sender, "", "Erroneous command.");
	}
}

void		LocalServer::execServer(Server *sender, ServerCommand const &cmd) {
	Server *source;

	cmd.isValid();
	if (findServer(cmd.servername()) || !servername.compare(cmd.servername())) {
		logPromote("Link", "Redundancy between two servernames.", false);
		return logError(sender, "", "Spanning tree broken for this connection.");
	}
	if (cmd.prefix.empty() || !(source = findServer(cmd.prefix)))
		return logError(sender, "SERVER -> Unknown source server.", "SERVER Command failed.");
	if (!Utils::validServName(cmd.servername()))
		return logError(sender, "SERVER -> Wrong server name.", "SERVER Command failed.");
	if (!Utils::checkNbr(cmd.hopcount()))
		return logError(sender, "SERVER -> Wrong Hop count.", "SERVER Command failed.");
	logPromote("Link", "[" + cmd.servername() + "]", true);
	broadcastServer(sender, newLink(sender, cmd));
}

void		LocalServer::execNjoin(Server *sender, NjoinCommand const &cmd) {
	std::string members;
	std::string cname;
	char		status;
	Client	*c;
	Channel *tmp;

	cmd.isValid();
	if (!Utils::validChanName(cmd.chan()) || findChannel(cmd.chan()))
		return logError(sender, "NJOIN -> Invalid channel name.", "NJOIN Command failed.");
	if (!cmd.chan().rfind("&", 0))
		return logError(sender, "NJOIN -> Shared a local channel.", "NJOIN Command failed.");
	if ((members = cmd.members()).empty())
		return logError(sender, "NJOIN -> Invalid member list.", "NJOIN Command failed.");
	cname = Utils::getToken(members, ",");
	if (!cname.empty() && !cname.rfind("@@", 0) && cname.compare("@@"))
		cname.erase(0,2);
	else
		return logError(sender, "NJOIN -> Invalid creator name \'" + cname + "\'.", "NJOIN Command failed.");
	if (!(c = findClient(cname, NICKNAME)))
		return logError(sender, "NJOIN -> Channel\'s creator does not exist.", "NJOIN Command failed.");
	if (!(tmp = new(std::nothrow) Channel(cmd.chan(), c)))
		return logError(sender, "Internal error.", "Internal error.");
	while (!members.empty()) {
		if ((cname = Utils::getToken(members, ",")).empty())
			return logError(sender, "NJOIN -> Invalid member list.", "NJOIN Command failed.");
		status = cname[0];
		if (!cname.rfind("@", 0))
			cname.erase(0, 1);
		if (!(c = findClient(cname, NICKNAME)))
			return logError(sender, "NJOIN -> Invalid member list.", "NJOIN Command failed.");
		tmp->njoin(c, status);
	}
	_chans.push_back(tmp);
	logSuccess("New \'Channel\' created: [" + cmd.chan() + "]");
}

void		LocalServer::execMode(Server *sender, ModeCommand const &cmd) {
	Channel		*chan = nullptr;
	Client		*source = nullptr;
	Client		*target = nullptr;
	std::string	modes;
	bool		set;

	cmd.isValid();

	modes = cmd.field2();

	// retreiving modes string and bool set/unset
	if (modes.empty() || (modes.rfind("-", 0) && modes.rfind("+", 0)) || modes.length() < 2)
		return logError(sender, "MODE -> Invalid mode string \'" + modes + "\'.", "MODE Command failed.");
	set = (!modes.rfind("+", 0) ? true : false);
	modes.erase(0, 1);

	if (cmd.argNbr() == 2) { // :server MODE #chan +modes
		if (!cmd.field1().rfind("#", 0) || !cmd.field1().rfind("&", 0)) {
			if (!(source = findClient(cmd.prefix, NICKNAME)) && !findServer(cmd.prefix))
				return logError(sender, "MODE -> Invalid modification's source.", "MODE Command failed.");
			if (!(chan = findChannel(cmd.field1())))
				return logError(sender, "MODE -> Targeted channel does not exist.", "MODE Command failed.");
			while (!modes.empty()) {
				if (!chan->applyModeFlag(modes[0], set))
					return logError(sender, "MODE -> Invalid mode string \'" + modes + "\'.", "MODE Command failed.");
				modes.erase(0, 1);
			}
			logColored(LOG_ORANGE, "MODE -> Shared channel \'" + chan->getName() + "\' modes [" + cmd.field2() + "]");
			if (source)
				chan->send(nullptr, ModeCommand(source->fullId(), cmd.args));
			broadcastToServers(sender, cmd);
		}
		else { // :user1 MODE user2 +modes
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
			logMode("Client", target->name(), cmd.field2());
			broadcastToServers(sender, cmd);
		}
	}
	else { // user1 MODE #chan +modes user2
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
		logMode("Client", target->name(), cmd.field2());
		chan->send(nullptr, ModeCommand(source->fullId(), cmd.args));
		broadcastToServers(sender, cmd);
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
					return logNotify(true, "New \'Channel\' created an internal error.");
			}
			else if ((code = chan->join(source)))
				continue;
			logSuccess("[" + token + "] joined \'" + chan->getName() + "\' successfuly.");
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
		logNotify(true, "[" + source->name() + "] left \'" + chan->getName() + "\' successfuly.");
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
	Client	*target;

	cmd.isValid();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return;
	if ((target = findClient(cmd.target(), NICKNAME))) {
		if (target->isLink())
			target->link->send(cmd);
		else
			target->send(NoticeCommand(source->fullId(), cmd.args));
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
	finishConx(sender, source, cmd.message(), false);
}

void		LocalServer::execSquit(Server *sender, SquitCommand const &cmd) {
	Server		*target;

	cmd.isValid();
	if (!cmd.server().compare(servername)) {
		logNotify(true, "Netsplit with \'" + sender->name() + "\'.");
		if (cmd.comment().empty())
			warnAllOperators("Netsplit with \'" + sender->name() + "\'.");
		else
			warnAllOperators("Netsplit with \'" + sender->name() + "\'. (" + cmd.comment() + ")");
		if (cmd.comment().empty())
			return finishConx(nullptr, sender, "Netsplit with \'" + sender->name() + "\'.", true);
		else
			return finishConx(nullptr, sender, "Netsplit with \'" + sender->name() + "\' (" + cmd.comment() + ").", true);
	}
	if (!(target = findServer(cmd.server())))
		return logError(sender, "SQUIT -> Command\'s target does not exist.", "SQUIT Command failed.");
	finishConx(nullptr, target, cmd.comment(), false);
}

void		LocalServer::execInvite(Server *sender, InviteCommand const &cmd) {
	Client	*source;
	Client	*target;
	Channel	*chan;

	cmd.isValid();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return logError(sender, "INVITE -> Command\'s source does not exist.", "INVITE Command failed.");
	if (!(chan = findChannel(cmd.chan())))
		return logError(sender, "INVITE -> Channel does not exist.", "INVITE Command failed.");
	if (!chan->isOnChan(source))
		return logError(sender, "INVITE -> Source is not on channel.", "INVITE Command failed.");
	if (chan->isInviteonly() && !chan->isOperator(source))
		return logError(sender, "INVITE -> Source does not have operator privileges.", "INVITE Command failed.");
	if (!(target = findClient(cmd.nick(), NICKNAME)))
		return logError(sender, "INVITE -> Target does not exist.", "INVITE Command failed.");
	if (chan->isOnChan(target))
		return logError(sender, "INVITE -> Target is already on channel.", "INVITE Command failed.");
	if (target->isAway())
		return logError(sender, "INVITE -> Target is marked as AWAY.", "INVITE Command failed.");
	chan->addInvite(target->name());
	if (!target->isLink())
		target->send(InviteCommand(source->fullId(), cmd.args));
	broadcastToServers(sender, cmd);
}

void		LocalServer::execKick(Server *sender, KickCommand const &cmd) {
	std::string nicks;
	std::string token;
	Client	*source;
	Client	*target;
	Channel	*chan;

	cmd.isValid();
	nicks = cmd.nicks();
	if (!(source = findClient(cmd.prefix, NICKNAME)))
		return logError(sender, "KICK -> Command\'s source does not exist.", "KICK Command failed.");
	if (!(chan = findChannel(cmd.chan())))
		return logError(sender, "KICK -> Channel does not exist.", "KICK Command failed.");
	if (!chan->isOnChan(source))
		return logError(sender, "KICK -> Source is not on channel.", "KICK Command failed.");
	if (!chan->isOperator(source))
		return logError(sender, "KICK -> Source does not have operator privileges.", "KICK Command failed.");
	while (!nicks.empty()) {
		if ((token = Utils::getToken(nicks, ",")).empty())
			continue;
		if (!(target = findClient(token, NICKNAME)))
			continue;
		if (!chan->isOnChan(target))
			return logError(sender, "KICK -> Target is not on channel.", "KICK Command failed.");
		if (chan->isOperator(target) && !chan->isCreator(source))
			continue;
		if (cmd.comment().empty())
			chan->updateClients(target, PartCommand(NO_PREFIX, chan->getName(), "Kicked from channel"));
		else
			chan->updateClients(target, PartCommand(NO_PREFIX, chan->getName(), cmd.comment()));
	}
	warnAllOperators("\'" + source->name() + "\' banned clients from \'" + chan->getName() + "\'");
	broadcastToServers(sender, cmd);
}

// ###### CLIENT ######

void		LocalServer::execNick(Client *sender, NickCommand const &cmd) {
	cmd.isValid();
	if (!Utils::validNickName(cmd.nickname()))
		return numericReply(sender, ERR_ERRONEOUSNICKNAME, cmd.nickname());
	if (findClient(cmd.nickname(), NICKNAME))
		return numericReply(sender, ERR_NICKNAMEINUSE, cmd.nickname());
	if (!sender->isRegistered()) {
		if (sender->nickname.empty()) {
			sender->nickname = cmd.nickname();
			if (!howMany(CLIENT_FLAG_LOCALOP))
				sender->isLocalop(true);
			numericReply(sender, RPL_WELCOME, sender->fullId());
			numericReply(sender, RPL_YOURHOST);
			numericReply(sender, RPL_CREATED);
			numericReply(sender, RPL_MYINFO);
			broadcastClient(nullptr, sender);
		}
		else
			sender->nickname = cmd.nickname();
	}
	else {
		updateChans(sender, cmd);
		broadcastToServers(nullptr, NickCommand(sender->name(), cmd.nickname()));
		sender->nickname = cmd.nickname();
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
			if (!howMany(CLIENT_FLAG_LOCALOP))
				sender->isLocalop(true);
			numericReply(sender, RPL_WELCOME, sender->fullId());
			numericReply(sender, RPL_YOURHOST);
			numericReply(sender, RPL_CREATED);
			numericReply(sender, RPL_MYINFO);
			broadcastClient(nullptr, sender);
		}
		else {
			sender->username = cmd.username();
			sender->realname = cmd.realname();
		}
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
	if ((tmp = findClient(cmd.target(), NICKNAME))) {
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
	if (sender->isLocalop())
		warnAllOperators("Local operator \'" + sender->name() + "\' left the server.");
	else if (sender->isOperator())
		warnAllOperators("Operator \'" + sender->name() + "\' left the server.");
	finishConx(nullptr, sender, cmd.message(), false);
}

void		LocalServer::execPing(Client *sender, PingCommand const &cmd) {
	cmd.isValid();
	sender->send(PongCommand(servername));
}

void		LocalServer::execPong(Client *sender, PongCommand const &cmd) { //command is discarded
	sender->isFinished();
	cmd.isValid();
}

void		LocalServer::loadMotd(void) {
	std::ifstream file("motd.txt");
	std::string input;

	if (!file.is_open())
		return;
	while (std::getline(file, input)) {
		Utils::graphicalOnly(input);
		Utils::clearSpaces(input, false);
		while (input.length() > MAX_MOTD_READ) {
			_motd.push_back(input.substr(0, MAX_MOTD_READ));
			input.erase(0, MAX_MOTD_READ);			
		}
		if (input.length() < MAX_MOTD_READ)
			input += std::string(MAX_MOTD_READ - input.length(), '_');
		_motd.push_back(input);
	}
	if (file.bad()) {
		logNotify(true, "Error while reading MOTD file.");
		_motd.clear();
	}
	file.close();
}

void		LocalServer::execMotd(Client *sender, MotdCommand const &cmd) {
	cmd.isValid();
	if (_motd.empty())
		numericReply(sender, ERR_NOMOTD);
	else {
		numericReply(sender, RPL_MOTDSTART, name());
		for (std::list<std::string>::const_iterator it = _motd.begin(); it != _motd.end(); it++)
			numericReply(sender, RPL_MOTD, *it);
		numericReply(sender, RPL_ENDOFMOTD);
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
	if (!(tmp = findClient(cmd.target(), NICKNAME)))
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
			else if ((code = chan->join(sender))) {
				if (code != ERR_USERONCHANNEL)
					numericReply(sender, code, chan->getName());
				continue;
			}
			if (chan->hasTopic())
				numericReply(sender, RPL_TOPIC, chan);
			execNames(sender, NamesCommand(NO_PREFIX, chan->getName()));
			logSuccess("[" + token + "] joined \'" + chan->getName() + "\' successfuly.");
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
		logNotify(true, "[" + sender->name() + "] left \'" + chan->getName() + "\' successfuly.");
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
		warnAllOperators("[" + sender->nickname + "] is now an IRC operator !");
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
			logMode("Channel", chan->getName(), cmd.field2());
		} else {
			if (!(target = findClient(cmd.field1(), NICKNAME)))
				throw (Command::InvalidCommand(ERR_USERSDONTMATCH));
			if (target->nickname.compare(sender->nickname))
				throw (Command::InvalidCommand(ERR_USERSDONTMATCH));
			mode = cmd.field2();
			if ((mode[0] != '-' && mode[0] != '+') || mode.size() != 2)
				throw (Command::InvalidCommand(ERR_UMODEUNKNOWNFLAG));
			target->applyModeFlag(mode[1], (mode[0] == '+' ? true : false));
			target->send(ModeCommand(target->fullId(), cmd.args));
			logMode("Channel", target->name(), cmd.field2());
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
		logMode("Client", target->name(), cmd.field2());
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
		for (conxlist_it it = conxs.begin(); it != conxs.end(); it++) {
			if ((*it)->isClient() && !sender->sharedChans(*it))
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

void		LocalServer::execSquit(Client *sender, SquitCommand const &cmd) {
	Server *target;

	cmd.isValid();
	if (!sender->isLocalop())
		return numericReply(sender, ERR_NOTLOCALOP);
	if (cmd.server().empty() || !servername.compare(cmd.server()))
		return numericReply(sender, ERR_NOSUCHSERVER, cmd.server());
	if (!(target = findServer(cmd.server())))
		return numericReply(sender, ERR_NOSUCHSERVER, cmd.server());
	if (cmd.comment().empty())
		finishConx(nullptr, target, "No reason given.", false);
	else
		finishConx(nullptr, target, cmd.comment(), false);
	warnAllOperators("Shutting down \'" + target->name() + "\' from \'" + sender->name() + "\'");
}

void		LocalServer::execConnect(Client *sender, ConnectCommand const &cmd) {
	cmd.isValid();
	if (!sender->isLocalop())
		return numericReply(sender, ERR_NOTLOCALOP);
	if (!checkWhitelistInfos(cmd.target(), cmd.port())) {
		logNotify(true, "CONNECT whitelist error for servername \'" + cmd.target() + "\'");
		return numericReply(sender, ERR_NOSUCHSERVER, cmd.target());
	}
	if (joinNet(whitelistInfos(cmd.target())))
		warnAllOperators("Connect \'" + cmd.target() + " " + cmd.port() + "\' from \'" + sender->name() + "\' success.");
}

void		LocalServer::execInvite(Client *sender, InviteCommand const &cmd) {
	Client	*target;
	Channel	*chan;

	cmd.isValid();
	if (!(chan = findChannel(cmd.chan())))
		return numericReply(sender, ERR_NOSUCHCHANNEL, cmd.chan());
	if (!chan->isOnChan(sender))
		return numericReply(sender, ERR_NOTONCHANNEL);
	if (chan->isInviteonly() && !chan->isOperator(sender))
		return numericReply(sender, ERR_CHANOPRIVSNEEDED, cmd);
	if (!(target = findClient(cmd.nick(), NICKNAME)))
		return numericReply(sender, ERR_NOSUCHNICK, cmd.nick());
	if (chan->isOnChan(target))
		return numericReply(sender, ERR_USERONCHANNEL, cmd);
	if (target->isAway())
		return numericReply(sender, RPL_AWAY, target);
	chan->addInvite(target->name());
	if (!target->isLink())
		target->send(InviteCommand(sender->fullId(), cmd.args));
	broadcastToServers(nullptr, InviteCommand(sender->name(), cmd.args));
	return numericReply(sender, RPL_INVITING, cmd);
}

void		LocalServer::execKick(Client *sender, KickCommand const &cmd) {
	std::string nicks;
	std::string token;
	Client	*target;
	Channel	*chan;

	cmd.isValid();
	nicks = cmd.nicks();
	if (!(chan = findChannel(cmd.chan())))
		return numericReply(sender, ERR_NOSUCHCHANNEL, cmd.chan());
	if (!chan->isOnChan(sender))
		return numericReply(sender, ERR_NOTONCHANNEL);
	if (!chan->isOperator(sender))
		return numericReply(sender, ERR_CHANOPRIVSNEEDED, cmd);
	while (!nicks.empty()) {
		if ((token = Utils::getToken(nicks, ",")).empty())
			continue;
		if (!(target = findClient(token, NICKNAME)))
			continue;
		if (!chan->isOnChan(target))
			numericReply(sender, ERR_USERNOTINCHANNEL, KickCommand(NO_PREFIX, cmd.chan(), token, ""));
		if (chan->isOperator(target) && !chan->isCreator(sender))
			continue;
		if (cmd.comment().empty())
			chan->updateClients(target, PartCommand(NO_PREFIX, chan->getName(), "Kicked from channel"));
		else
			chan->updateClients(target, PartCommand(NO_PREFIX, chan->getName(), cmd.comment()));
	}
	warnAllOperators("\'" + sender->name() + "\' banned clients from \'" + chan->getName() + "\'");
	broadcastToServers(nullptr, KickCommand(sender->name(), cmd.args));
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
									+ _log.start); break;
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
		case RPL_ENDOFMOTD: 		ret.addArg("End of MOTD reply"); break;
		case RPL_YOUREOP: 			ret.addArg("You are now an IRC operator"); break;
		case ERR_NOORIGIN: 			ret.addArg("No origin specified"); break;
		case ERR_NOTEXTTOSEND: 		ret.addArg("No text to send"); break;
		case ERR_NOMOTD: 			ret.addArg("MOTD file is missing"); break;
		case ERR_NONICKNAMEGIVEN: 	ret.addArg("No nickname given"); break;
		case ERR_NOTREGISTERED: 	ret.addArg("You have not registered"); break;
		case ERR_ALREADYREGISTERED: ret.addArg("Unauthorized command (already registered)"); break;
		case ERR_PASSWDMISMATCH: 	ret.addArg("Wrong password"); break;
		case ERR_NOTLOCALOP: 		ret.addArg("You\'re not the local operator"); break;
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
									ret.addArg("No such nickname"); break;
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
		case ERR_INVITEONLYCHAN:	ret.addArg(field);
									ret.addArg("Cannot join channel (+i)"); break;
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
	} else if (code == ERR_CHANOPRIVSNEEDED) {							// 482
		ret.addArg((cmd == "INVITE" ? cmd.argX(1) : cmd.argX(0)));
		ret.addArg("You\'re not a channel operator");
	} else if (code == ERR_USERNOTINCHANNEL){
		if (cmd == "MODE") {
			ret.addArg(cmd.argX(2));
			ret.addArg(cmd.argX(0));
		} else {
			ret.addArg(cmd.argX(1));
			ret.addArg(cmd.argX(0));
		}
		ret.addArg("They aren\'t on that channel");
	} else if (code == ERR_USERONCHANNEL) {								// 443
		ret.addArg(cmd.argX(0));
		ret.addArg(cmd.argX(1));
		ret.addArg("is already on channel");
	} else if (code == RPL_INVITING) {								// 443
		ret.addArg(cmd.argX(1));
		ret.addArg(cmd.argX(0));
		ret.addArg("Invited successfuly");
	} else
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
	for (conxlist_cit it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isClient() && !static_cast<Client *>(*it)->hasChans())
			lst.push_back(static_cast<Client *>(*it)->nickname);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//		 LOG FUNCTIONS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LocalServer::logNotify(bool error, std::string const &msg) {
	_log.append(LOG_DATE, (error ? LOG_DARKRED : LOG_DARKGREY), msg);
}

void		LocalServer::logSuccess(std::string const &msg) {
	_log.append(LOG_DATE, LOG_GREEN, msg);
}

void		LocalServer::logColored(std::string const &color, std::string const &msg) {
	_log.append(LOG_DATE, color, msg);
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

void		LocalServer::logMode(std::string const &type, std::string const &name, std::string const &modes) {
	logColored(LOG_ORANGE, "MODE -> \'" + type + "\' [" + name + "] modes modified with [" + modes + "]");
}

void		LocalServer::logError(Connection *target, std::string const &msg) {
	logNotify(true, msg);
	if (target->isClient())
		finishConx(nullptr, static_cast<Client *>(target), msg, false);
	else if (target->isServer())
		finishConx(nullptr, static_cast<Server *>(target), msg, true);
	else
		finishConx(target);
}

void		LocalServer::logError(Connection *target, std::string const &msg, std::string const &errmsg) {
	logNotify(true, msg);
	if (!errmsg.empty())
		target->send(ErrorCommand(errmsg));
	if (target->isClient())
		finishConx(nullptr, static_cast<Client *>(target), msg, false);
	else if (target->isServer())
		finishConx(nullptr, static_cast<Server *>(target), msg, true);
	else
		finishConx(target);
}

// ########################################
// 					DEBUG
// ########################################

void		LocalServer::showChans(void) {
	DEBUG_BAR_DISPC(_stdout, '=', 45, ORANGE);
	for (chanlist::const_iterator it = _chans.begin(); it != _chans.end(); it++) {
		DEBUG_DDISPC(_stdout, "->", *(*it), DARK_GREEN);
	}
	DEBUG_BAR_DISPC(_stdout, '=', 45, ORANGE);
}

void		LocalServer::showNet(void) {
	Connection *tmp;

	DEBUG_BAR_DISPC(_stdout, '=', 45, ORANGE);
	DEBUG_LDISPCB(_stdout, "Unknown:  ", howManyUnknown(), PURPLE, '[');
	DEBUG_LDISPCB(_stdout, "D. Client:", howManyClient(true), PURPLE, '[');
	DEBUG_LDISPCB(_stdout, "Client:   ", howManyClient(false), PURPLE, '[');
	DEBUG_ADISPCB(_stdout, "D. Server:", howManyServer(true, false), " + 1 (this)", PURPLE, '[');
	DEBUG_ADISPCB(_stdout, "Server:   ", howManyServer(false, false), " + 1 (this)", PURPLE, '[');
	DEBUG_LDISPCB(_stdout, "Service:  ", howManyService(false), PURPLE, '[');
	DEBUG_LDISPCB(_stdout, "Chans:    ", howManyChannel(), PURPLE, '[');
	DEBUG_BAR_DISPC(_stdout, '>', 35, DARK_GREY);
	if (conxs.empty())
		DEBUG_DISPCB(_stdout, " No connection yet ", DARK_GREY, '~');
	else {
		for (conxlist_cit it = conxs.begin(); it != conxs.end(); it++) {
			tmp = *it;
			if (it != conxs.begin())
				DEBUG_BAR_DISPC(_stdout, '-', 80, DARK_GREY);
			if (tmp->isClient()) {
				if (tmp->isLink())
					DEBUG_LDISPCB(_stdout, "Client*\t", *(static_cast<Client *>(tmp)), YELLOW, '{');
				else
					DEBUG_LDISPCB(_stdout, "Client\t", *(static_cast<Client *>(tmp)), GREEN, '{');
			}
			else if (tmp->isServer()){
				if (tmp->isLink())
					DEBUG_LDISPCB(_stdout, "Server*\t", *(static_cast<Server *>(tmp)), YELLOW, '{');
				else
					DEBUG_LDISPCB(_stdout, "Server\t", *(static_cast<Server *>(tmp)), BLUE, '{');
			}
			else
				DEBUG_LDISPCB(_stdout, "Pending\t", *tmp, ORANGE, '{');
		}
	}
	DEBUG_BAR_DISPC(_stdout, '>', 35, DARK_GREY);
	DEBUG_BAR_DISPC(_stdout, '=', 45, ORANGE);
}

