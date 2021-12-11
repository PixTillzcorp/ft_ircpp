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
**----- File ----------------{ ServerInfo.cpp }-------------------------------**
**----- Created -------------{ 2021-05-04 16:39:04 }--------------------------**
**----- Updated -------------{ 2021-08-18 15:23:33 }--------------------------**
********************************************************************************
*/

#include <unistd.h>
#include <fcntl.h>
#include "../incs/ServerInfo.hpp"

// ########################################
// 					PUBLIC
// ########################################

// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

// ____________Canonical Form____________

// -----------------------------
//	 Constructor / Destructor
// -----------------------------

ServerInfo::~ServerInfo(void) {
	while (!this->_conxs.empty())
	{
		delete this->_conxs.front();
		this->_conxs.erase(this->_conxs.begin());
	}
	return;
}
ServerInfo::ServerInfo(void) { return; }

// -----------------------------
//	 	  Copy constructor
// -----------------------------

ServerInfo::ServerInfo (ServerInfo const &src) :_name(src.getName()), 
												_password(src.getPassword()),
												_sock(src.getSock()),
												_upperfd(src.getUpperfd()),
												_sockinfo(src.getSockinfo()),
												_mfds(src.getMfds()),
												_rfds(src.getRfds()),
												_wfds(src.getWfds()),
												_version(src.getVersion()),
												_implem(src.getImplem()),
												_conxs(src.getConxs()),
												_done(src.isDone()),
												_nolink(NULL) {
	return;
}

// -----------------------------
//	 	Assignation operator
// -----------------------------

ServerInfo	&ServerInfo::operator=(ServerInfo const &src) {
	this->_name = src.getName();
	this->_password = src.getPassword();
	this->_sock = src.getSock();
	this->_upperfd = src.getUpperfd();
	this->_sockinfo = src.getSockinfo();
	this->_mfds = src.getMfds();
	this->_rfds = src.getRfds();
	this->_wfds = src.getWfds();
	this->_version = src.getVersion();
	this->_implem = src.getImplem();
	this->_conxs = src.getConxs();
	this->_done = src.isDone();
	this->_nolink = NULL;
	return *this;
}
// ______________________________________


// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

// _____________Constructor______________

// -----------------------------
//		Parametric constructor
// -----------------------------

ServerInfo::ServerInfo(std::string const info, std::string const port, std::string const password) {
	DEBUG_DISP(COUT, CBRACKET(DARK_GREEN, "LAUNCH", '[') << " Server starting: " << CBRACKET(DARK_GREEN, info, '[') << CBRACKET(DARK_GREEN, port, '[') << CBRACKET(DARK_GREEN, password, '['));
	this->_name = std::string("test");
	this->_password = password;
	this->_mfds = FdSet();
	this->_mfds.zeroFd();
	this->_rfds = FdSet();
	this->_rfds.zeroFd();
	this->_wfds = FdSet();
	this->_wfds.zeroFd();
	this->init(std::string("localhost"), port); //change "localhost" and try to connect on non-local addresses 
	this->_version = std::string(VERSION);
	this->_implem = std::string(IMPLEM) + '|' + std::string(AUTHOR);
	this->_nolink = NULL;
	this->_done = false;
	if (!info.empty())
		this->joinNet(info);
}
// ______________________________________


// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

// -----------------------------
//		Select function call
// -----------------------------

// ....................................
// In order:
//		- Init fd sets for select call.
//		- Call to select.
//		- Checks inside sets for fds.
// ....................................

void ServerInfo::selectCall(void) {
	while (!this->_done)
	{
		DEBUG_DISPCB(COUT, std::string(50, '-'), DARK_PURPLE, '~');
		this->listenServerSock();
		this->initRWFds();
		// std::cout << "rfds ->" << this->_rfds << std::endl;
		// std::cout << "wfds ->" << this->_wfds << std::endl;
		if (select(this->_upperfd + 1, this->_rfds.getPtr(), this->_wfds.getPtr(), NULL, NULL) == -1)
		{
			std::cout << "Select function failed." << std::endl;
			break;
		}
		this->checkStd();
		this->checkSock();
		this->checkFds();
		DEBUG_DISPCB(COUT, std::string(50, '-'), DARK_PURPLE, '~');
	}
}
// ______________________________________


// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

// ____________Setter / Getter___________
// -----------------------------
//			  _name
// -----------------------------
std::string	ServerInfo::getName(void) const { return this->_name; }
void		ServerInfo::setName(std::string const &src) { this->_name = src; }

// -----------------------------
//			_password
// -----------------------------
std::string	ServerInfo::getPassword(void) const { return this->_password; }
void		ServerInfo::setPassword(std::string const &src) { this->_password = src; }

// -----------------------------
//			   _sock
// -----------------------------
int			ServerInfo::getSock(void) const { return this->_sock; }
void		ServerInfo::setSock(int const &src) { this->_sock = src; }

// -----------------------------
//	  		 _upperfd
// -----------------------------
int			ServerInfo::getUpperfd(void) const { return this->_upperfd; }
void		ServerInfo::setUpperfd(int const &src) { this->_upperfd = src; }

// -----------------------------
//			  _sockinfo
// -----------------------------
SockInfo	const &ServerInfo::getSockinfo(void) const { return this->_sockinfo; }
void		ServerInfo::setSockinfo(SockInfo const &src) { this->_sockinfo = src; }

// -----------------------------
//			   _mfds
// -----------------------------
FdSet		const &ServerInfo::getMfds(void) const { return this->_mfds; }
void		ServerInfo::setMfds(FdSet const &src) { this->_mfds = src; }

// -----------------------------
//			   _rfds
// -----------------------------
FdSet		const &ServerInfo::getRfds(void) const { return this->_rfds; }
void		ServerInfo::setRfds(FdSet const &src) { this->_rfds = src; }

// -----------------------------
//			   _wfds
// -----------------------------
FdSet		const &ServerInfo::getWfds(void) const { return this->_wfds; }
void		ServerInfo::setWfds(FdSet const &src) { this->_wfds = src; }
// -----------------------------
//			  _version
// -----------------------------
std::string		const &ServerInfo::getVersion(void) const { return this->_version; }
std::string		ServerInfo::getNetVersion(void) const {
	std::string	ret;
	size_t last = 0;
	size_t next = 0;

	while (next != std::string::npos)
	{
		next = this->_version.find('.', (last ? last : 1));
		ret += this->_version.substr((last ? last : 0), next - last);
		last = next + 1;
	}
	return ret;
}
void		ServerInfo::setVersion(std::string const &src) { this->_version = src; }

// -----------------------------
//			  _implem
// -----------------------------
std::string		const &ServerInfo::getImplem(void) const { return this->_implem; }
void		ServerInfo::setImplem(std::string const &src) { this->_implem = src; }
// -----------------------------
//			   _conxs
// -----------------------------
std::list<Connection *> const &ServerInfo::getConxs(void) const { return this->_conxs; }
void		ServerInfo::setConxs(std::list<Connection *> &src) { this->_conxs = src; }
// -----------------------------
//			   _done
// -----------------------------
bool		ServerInfo::isDone(void) const { return this->_done; }
void		ServerInfo::setDone(void) { this->_done = true; }
// ______________________________________

// ########################################
// 					PRIVATE
// ########################################

// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

void ServerInfo::init(std::string const &host, std::string const &port) {
	if (host == "localhost")
	{
		DEBUG_DISP(COUT, CBRACKET(DARK_YELLOW, "LOCAL", '[') << " Server is local.");
		std::cout << "Enter local server name: ";
		std::cin >> this->_name;
	}
	try {
		this->_sockinfo = SockInfo(host, port);
	}
	catch (/*getaddrinfo error*/) {

	}
	catch (/*tcp protocol error*/) {

	}
	this->initServerSock();
	this->bindServerSock();
	this->initMasterFds();
}

void ServerInfo::initServerSock(void) {
	int yes = 1; //debug
	this->_sock = socket(this->_sockinfo.family(), this->_sockinfo.socktype(), this->_sockinfo.protocol());
	if (setsockopt(this->_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) //debug
		return ; //debug
	this->_upperfd = this->_sock;
	if (this->_sock == -1)
		std::cout << "Sock function failed." << std::endl;
}

void ServerInfo::bindServerSock(void) const {
	if (bind(this->_sock, this->_sockinfo.address(), this->_sockinfo.addressLen()) == -1)
		std::cout << "Bind function failed." << std::endl;
}

void ServerInfo::listenServerSock(void) const {
	if (listen(this->_sock, 200) == -1)
		std::cout << "Listen function failed." << std::endl;
}

void ServerInfo::initMasterFds(void) {
	this->_mfds.addFd(this->_sock);
	fcntl(this->_sock, F_SETFL, O_NONBLOCK);
	this->_mfds.addFd(STDIN_FILENO);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void ServerInfo::initRWFds(void) {
	std::list<Connection *>::iterator it;

	this->_rfds.zeroFd();
	this->_rfds.setFds(this->_mfds.getFds());
	this->_wfds.zeroFd();
	if (this->_conxs.empty())
		return ;
	for(it = this->_conxs.begin(); it != this->_conxs.end(); it++)
	{
		if ((*it)->isFinished())
		{
			this->_rfds.removeFd((*it)->getSock());
			if (!(*it)->hasMessage())
			{
				this->closeConx(it);
				continue;
			}
		}
		if ((*it)->hasMessage())
			this->_wfds.addFd((*it)->getSock());
	}
}

// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

void ServerInfo::joinNet(std::string const &info) {
	std::stringstream ss(info);
	std::vector<std::string> args;

	while (!ss.eof()) {
		args.push_back(std::string());
		std::getline(ss, args.back(), ':');
	}
	if (args.size() != 3) {
		std::cout << "Argument error for remote connection to server." << std::endl;
		return;
		//throw
	}
	// try {
	this->connectTo(args[0], args[1], args[2]);
	// }
	// catch(std::exception &e) {
	// 	std::cout << "SockInfo error." << std::endl;
	// }
}

void ServerInfo::connectTo(std::string const &host, std::string const &port, std::string const &pass) {
	SockStream	tmpss;
	SockInfo 	tmpsi(host, port);

	tmpss.setSock(socket(tmpsi.family(), tmpsi.socktype(), tmpsi.protocol()));
	if (tmpss.getSock() == -1) {
		std::cout << "Socket function error." << std::endl;
		//throw
		return;
	}
	if (connect(tmpss.getSock(), tmpsi.address(), tmpsi.addressLen()) == -1) {
		std::cout << "Connect function error." << std::endl;
		//throw
		return;
	}
	this->newConx(tmpsi, tmpss);
	this->_conxs.back()->authentify();
	this->_conxs.back()->notAccepted();
	this->_conxs.back() = new Server(this->_conxs.back(), this->_version, this->_implem);
	this->_conxs.back()->sendCommand(PassCommand(this->getListPassCommand(pass)));
	this->_conxs.back()->sendCommand(ServerCommand(this->getListServerCommand()));
}

void ServerInfo::broadcast(Connection const *sender, Command const &cmd) {
	std::list<Connection *>::iterator it;

	DEBUG_DISP(COUT, CBRACKET(DARK_PINK, "BROADCAST", '[') << CBRACKET(DARK_PINK, static_cast<Message const>(cmd), '{'));
	for (it = this->_conxs.begin(); it != this->_conxs.end(); it++)
	{
		if ((*it) != sender && !(*it)->isLink() && (*it)->isServer())
		{
			DEBUG_DISP(COUT, CBRACKET(DARK_PINK, "BROADCAST", '[') << " -> " << CBRACKET(PINK, (*it)->getName(), '[') << CBRACKET(PINK, (*it)->getSock(), '[') << CBRACKET(PINK, (*it)->getStatus(), '['));
			(*it)->sendCommand(cmd);
		}
	}
}

void ServerInfo::shareNet(Connection *target) {
	std::list<Connection *>::iterator it;

	DEBUG_DISP(COUT, CBRACKET(DARK_PINK, "SHARING", '[') << " to Connection" << CBRACKET(GREEN, target->getName(), '[') << CBRACKET(GREEN, target->getStatus(), '['));
	for (it = this->_conxs.begin(); it != this->_conxs.end(); it++)
	{
		if ((*it) != target)
		{
			if ((*it)->isServer())
			{
				DEBUG_DISP(COUT, CBRACKET(DARK_PINK, "SHARING", '[') << " -> " << CBRACKET(PINK, (*it)->getName(), '[') << CBRACKET(PINK, (*it)->getSock(), '[') << CBRACKET(PINK, (*it)->getStatus(), '['));
				target->sendCommand(ServerCommand(this->_name, static_cast<Server *>(*it)->getArgList()));
			}
		}
	}
}

std::list<std::string> ServerInfo::getListPassCommand(std::string const &pass) const {
	std::list<std::string> ret;

	ret.push_back(pass);
	ret.push_back(this->getNetVersion());
	ret.push_back(this->_implem);

	return ret;
}

std::list<std::string> ServerInfo::getListServerCommand(void) const {
	std::list<std::string> ret;

	ret.push_back(this->_name);
	ret.push_back("1");
	ret.push_back(this->getNetVersion());
	ret.push_back(this->_implem);

	return ret;
}

// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

void ServerInfo::newConx(SockInfo const &sin, SockStream const &ss) {
	DEBUG_DISP(COUT, CBRACKET(GREEN, "NEW", '[') << " Connection " << CBRACKET(GREEN, ss.getSock(), '[') << "[P]");
	this->_conxs.push_back(new Connection(sin, ss, this->_nolink));
	fcntl(ss.getSock(), F_SETFL, O_NONBLOCK);
	this->_mfds.addFd(ss.getSock());
	this->checkUpperfd(ss.getSock());
}

void ServerInfo::newLink(Connection *&link, ServerCommand const &cmd) {
	DEBUG_DISP(COUT, CBRACKET(GREEN, "NEW", '[') << " Server Link.");
	this->_conxs.push_back(new Server(link, cmd));
}

void ServerInfo::newLink(Connection *&link, NickCommand const &cmd) {
	DEBUG_DISP(COUT, CBRACKET(GREEN, "NEW", '[') << " Client Link.");
	this->_conxs.push_back(new Client(link, cmd));
}

void ServerInfo::closeConx(std::list<Connection *>::iterator const &it) {
	if (!(*it)->isLink())
	{
		DEBUG_DISP(COUT, CBRACKET(RED, "END", '[') << " Connection " << CBRACKET(GREEN, (*it)->getName(), '[') << CBRACKET(GREEN, (*it)->getStatus(), '['));
		this->_mfds.removeFd((*it)->getSock());
		if (close((*it)->getSock()) == -1)
			DEBUG_DISP(COUT, CBRACKET(RED, "ERROR", '[') << " Close function failed for socket " << CBRACKET(GREEN, (*it)->getSock(), '['));
	}
	else
		DEBUG_DISP(COUT, CBRACKET(RED, "END", '[') << " Link " << CBRACKET(GREEN, (*it)->getName(), '[') << CBRACKET(GREEN, (*it)->getStatus(), '[') << CBRACKET(YELLOW, (*it)->getLink()->getName(), '['));
	delete (*it);
	this->_conxs.erase(it);
}

void ServerInfo::finishConx(Connection *&target, std::string const &msg) {
	if (!target->isLink())
	{
		if (!msg.empty())
			target->sendCommand(ErrorCommand(msg));
		if (target->isServer())
			target->sendCommand(SquitCommand(NO_PREFIX, target->getName()));
		// else if (target->isClient())
		// 	target->sendCommand(QuitCommand(NO_PREFIX, msg));
	}
	target->finish();
}

Connection *ServerInfo::getConxVs(int sock) {
	std::list<Connection *>::iterator it;

	for(it = this->_conxs.begin(); it != this->_conxs.end(); it++) {
		if ((*it)->getSock() == sock)
			return (*it);
	}
	return (NULL);
}

Connection *ServerInfo::getConxVn(std::string const &name) {
	std::list<Connection *>::iterator it;

	for(it = this->_conxs.begin(); it != this->_conxs.end(); it++) {
		if ((*it)->isServer())
		{
			if ((*it)->getName() == name)
				return (*it);
		}
		//same block for client later;
	}
	return (NULL);
}

// *****************************************************************************
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *****************************************************************************

void ServerInfo::execCommand(Connection *&sender, Command const &cmd) {
	DEBUG_DISP(COUT, CBRACKET(GREEN, "NEW", '[') << " Command from Connection" << CBRACKET(GREEN, sender->getName(), '[') << CBRACKET(GREEN, sender->getStatus(), '['));
	DEBUG_DISP(COUT, CBRACKET(GREEN, "EXE", '[') << " " << BRACKET(cmd, '\''));
	if (!cmd.isValid())
		this->finishConx(sender, "Invalid command: \'" + cmd.getContent() + "\'");
	else
	{
		DEBUG_DISP(COUT, CBRACKET(DARK_CYAN, "DEBUG", '[') << " " << BRACKET( cmd, '\''));
		if (!sender->isAuthentified())
		{
			if (!cmd.checkCommand("PASS"))
				this->finishConx(sender, "Authentication failed.");
			else
				this->execPass(sender, cmd);
		}
		// else if (cmd.checkCommand("NICK"))
		// 	this->execNick(sender, cmd);
		else if (cmd.checkCommand("SQUIT"))
			this->execSquit(sender, cmd);
		else if (cmd.checkCommand("PASS"))
			DEBUG_DISP(COUT, CBRACKET(RED, "BAD", '[') << COLORED(" Sender Already registered.", RED));
		else if (cmd.checkCommand("SERVER"))
			this->execServer(sender, cmd);
		else
			DEBUG_DISP(COUT, CBRACKET(RED, "BAD", '[') << " Invalid command: " << CBRACKET(RED, cmd.getCommand(), '\''));
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ServerInfo::execPass(Connection *&sender, PassCommand const &cmd) {
	bool server = false;

	if (!cmd.implem.empty() || !cmd.version.empty())
	{
		server = !server;
		if (cmd.implem != this->getImplem())
			this->finishConx(sender, "Implementation not compatible [" + cmd.implem + "]");
		else if (cmd.version != this->getNetVersion())
			this->finishConx(sender, "Version not compatible [" + cmd.version + "]");
	}
	if (cmd.pass == this->_password && cmd.getPrefix().empty())
	{
		sender->authentify();
		if (server)
			sender = new Server(sender, cmd.version, cmd.implem);
		else
			sender = new Client(sender);
	}
	else
		this->finishConx(sender, "Password Mismatch for [" + cmd.pass + "]");
}

void ServerInfo::execServer(Connection *&sender, ServerCommand const &cmd) {
	if (!sender->isServer() || this->getConxVn(cmd.servername)) //add check for isValid() function later.
		this->finishConx(sender, "SERVER command: \'Not a server or already registered.\'");
	if (cmd.getPrefix().empty())
	{
		if (sender->getName().empty())
		{
			static_cast<Server *>(sender)->init(cmd.servername, cmd.token, cmd.info, cmd.hopcount);
			if (sender->isAccepted())
				sender->sendCommand(ServerCommand(this->getListServerCommand()));
		}
		else
			this->finishConx(sender, "SERVER command: \'Already registered.\'"); //change for numeric reply here
		this->shareNet(sender);
	}
	else
		this->newLink(sender, cmd);
	this->broadcast(sender, ServerCommand(this->_name, cmd));
}

// void ServerInfo::execNick(Connection *&sender, NickCommand const &cmd) {
// 	if (!sender->isClient() || this->get)
// }

void ServerInfo::execSquit(Connection *&sender, SquitCommand const &cmd) {
	std::list<Connection *>::iterator it;
	Connection *target;

	if (cmd.getPrefix().empty())
	{
		sender->finish();
		this->broadcast(sender, SquitCommand(this->_name, sender->getName()));
	}
	else
	{
		DEBUG_DISP(COUT, CBRACKET(DARK_CYAN, "DEBUG", '[') << " " << BRACKET(cmd.target, '\''));
		target = this->getConxVn(cmd.target);
		for (it = this->_conxs.begin(); it != this->_conxs.end(); it++)
		{
			if ((*it) == target || (*it)->checkLink(target))
				this->finishConx((*it), "SQUIT command: \'connection ended.\'");
		}
		this->broadcast(sender, SquitCommand(this->_name, cmd.target));
	}
}

void ServerInfo::execError(Connection *&sender, ErrorCommand const &cmd) {
	DEBUG_DISP(COUT, cmd.msg);
	// sender->finish();
}
// ______________________________________

// _______________Checking_______________
void ServerInfo::checkStd(void) {
	std::string	input;

	if (this->_rfds.checkFd(STDIN_FILENO))
	{
		std::getline(std::cin, input);
		if (!input.compare("EXIT") || !input.compare("Exit") || !input.compare("exit"))
		{
			DEBUG_DISP(COUT, CBRACKET(DARK_RED, "CLOSING", '[') << " Server shutting down.");
			this->setDone();
		}
		else if (!input.compare("PING") || !input.compare("Ping") || !input.compare("ping"))
			std::cout << "Pong !" << std::endl;
		else if (!input.compare("NET") || !input.compare("Net") || !input.compare("net"))
			this->debugShowNet();
		else if (!input.compare("INFO") || !input.compare("Info") || !input.compare("info"))
			this->debugShowInfo();
		else if (!input.compare("SEND") || !input.compare("Send") || !input.compare("send"))
			this->sendTo(); //debug
		else if (!input.empty())
			DEBUG_DISP(COUT, CBRACKET(RED, "ERROR", '[') << " Unknow command: " << CBRACKET(GREY, input, '\''));
	}
}

void ServerInfo::checkSock(void) {
	t_saddr 	sin;
	socklen_t	sin_size;
	int			sock;

	if (this->_rfds.checkFd(this->_sock))
	{
		sin_size = sizeof(sin);
		if ((sock = accept(this->_sock, &sin, reinterpret_cast<socklen_t *>(&sin_size))) == -1)
		{
			std::cout << "Accept function failed." << std::endl;
			//throw
		}
		this->newConx(SockInfo(reinterpret_cast<t_saddr_in *>(&sin)), SockStream(sock));
	}
}

void ServerInfo::checkFds(void) {
	std::list<Connection *>::iterator it;
	Command	*cmd;

	for (it = this->_conxs.begin(); it != this->_conxs.end(); it++)
	{
		if (this->isReadable(*it)) //is inside RFDS
		{
			if (!(*it)->read())
			{
				(*(it))->finish();
				this->broadcast((*it), SquitCommand(this->_name, (*it)->getName()));
				continue ;
			}
		}
		while ((cmd = (*it)->getNextCommand()) && !(*it)->isFinished())
		{
			this->execCommand((*it), Command(*cmd));
			// std::cout << "checkFds ptr:" << (*it) << std::endl << "Ptr status[" << (*it)->getStatus() << "]." << std::endl;
			delete cmd;
		}
		if (this->isWritable(*it)) //is inside WFDS
			(*it)->write();
	}
}

void ServerInfo::checkUpperfd(int const nsock) {
	if (this->_upperfd < nsock)
		this->_upperfd = nsock;
}

bool ServerInfo::isReadable(Connection const *conx) const {
	return this->_rfds.checkFd(conx->getSock());
}

bool ServerInfo::isWritable(Connection const *conx) const {
	return this->_wfds.checkFd(conx->getSock());
}

// ______________________________________

// ########################################
// 					DEBUG
// ########################################

void ServerInfo::debugShowInfo(void) const {
	DEBUG_DISPCB(COUT, std::string(40, '+'), PURPLE, '~');
	DEBUG_DISPC(COUT, "Name: " << BRACKET(this->_name, '['), PURPLE);
	DEBUG_DISPC(COUT, "Pass: " << BRACKET(this->_password, '['), PURPLE);
	DEBUG_DISPC(COUT, "Sock: " << BRACKET(this->_sock, '['), PURPLE);
	DEBUG_DISPC(COUT, "Version: " << BRACKET(this->_version, '['), PURPLE);
	DEBUG_DISPC(COUT, "Implem: " << BRACKET(this->_implem, '['), PURPLE);
	std::cout << this->_sockinfo;
	DEBUG_DISPCB(COUT, std::string(40, '+'), PURPLE, '~');
}

void ServerInfo::debugShowNet(void) const {
	std::list<Connection *>::const_iterator it;

	DEBUG_DISPCB(COUT, std::string(45, '#'), ORANGE, '~');
	for (it = this->_conxs.begin(); it != this->_conxs.end(); it++)
	{
		if ((*it)->isPending())
			std::cout << GREY << *(*it);
		if ((*it)->isServer())
			std::cout << GREEN << *static_cast<Server *>(*it);
		// if ((*it)->isClient())
		// 	std::cout << "\033[38;5;18m" << *static_cast<Client *>(*it);
		// if ((*it)->isService())
		// 	std::cout << "\033[38;5;207m" << *static_cast<Service *>(*it);
		std::cout << NORMAL << std::endl;
	}
	DEBUG_DISPCB(COUT, std::string(45, '#'), ORANGE, '~');
}

void ServerInfo::sendTo(void) {
	Connection *conx;
	std::string str;
	int	sock;

	std::cout << "Enter socket to sent: ";
	std::cin >> sock;
	if (!(conx = this->getConxVs(sock))) {
		std::cout << "Unknown socket." << std::endl;
		return ;
	}
	std::cout << "Message:" << std::endl;
	std::getline(std::cin, str);
	std::getline(std::cin, str);
	std::cout << "[" << str << "]" << std::endl;
	conx->sendCommand(Command(new Message(str)));
	std::cout << "Message sent." << std::endl;
}
