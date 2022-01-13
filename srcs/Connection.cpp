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
**----- File ----------------{ Connection.cpp }-------------------------------**
**----- Created -------------{ 2021-04-30 18:32:26 }--------------------------**
**----- Updated -------------{ 2022-01-12 12:12:02 }--------------------------**
********************************************************************************
*/

#include "Connection.hpp"

// ____________Canonical Form____________
Connection::~Connection(void) { return; }
Connection::Connection(void) { return; }
Connection::Connection(Connection const &cpy) :
	info(cpy.info), stream(cpy.stream), status(cpy.status), hop(cpy.hop), link(cpy.link) { return; }
Connection	&Connection::operator=(Connection const &cpy) {
	info = cpy.info;
	stream = cpy.stream;
	status = cpy.status;
	hop = cpy.hop;
	link = cpy.link;
	return *this;
}

// ____________Constructors______________
Connection::Connection(std::string const &host, std::string const &port, u_int16_t family) throw(Connection::ConxInit) :
	status(family == AF_INET6 ? CONX_IPV6 : CONX_NOFLAG), link(NO_LINK), hop(1) {
	int yes = 1;
	int	sock;

	try { info = SockInfo(host, port, family, (host.empty() ? BIND : CONNECT)); }
	catch (std::exception &ex) { throw(Connection::ConxInit("SockInfo() constructor failed.")); }
	if ((sock = socket(info.family(), info.socktype(), info.protocol())) == -1)
		throw(Connection::ConxInit("socket() function failed."));
	if (host.empty()) {
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
			throw(Connection::ConxInit("setsockopt() function failed."));
		if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes)) == -1)
			throw(Connection::ConxInit("setsockopt() function failed."));
		if (bind(sock, info.sockaddr(), info.addrLen()) == -1)
			throw(Connection::ConxInit("bind() function failed."));
		if (listen(sock, MAX_PENDING_CONNECTION) == -1)
			throw(Connection::ConxInit("listen() function failed."));
	} else {
		if (connect(sock, info.sockaddr(), info.addrLen()) == -1)
			throw(Connection::ConxInit("connect() function failed."));
	}
	try { stream = SockStream(sock); }
	catch (std::exception &ex) { throw(Connection::ConxInit("SockStream() constructor failed.")); }
}

Connection::Connection(int lsock) throw(Connection::ConxInit) : status(0), link(NO_LINK), hop(1) {
	t_saddr_s	output_addr;
	socklen_t	addr_len;
	int			sock;

	addr_len = sizeof(output_addr);	
	if ((sock = accept(lsock, reinterpret_cast<t_sa *>(&output_addr), &addr_len)) == -1)
		throw(Connection::ConxInit("Function accept() failed."));
	try {
		info = SockInfo(output_addr, ACCEPT);
		stream = SockStream(sock);
	} catch (std::exception &ex) {
		throw(Connection::ConxInit("SockInfo() or SockStream() constructor failed."));
	}
}

Connection::Connection(Connection *link, unsigned short status, size_t hop) :
	status(status), link(link), hop(hop) { return; }

// __________Member functions____________
std::string const &Connection::hostname(void) const { return info.host; }
std::string const &Connection::name(void) const { return hostname(); }

bool	Connection::read(void) { return(!stream.read() ? false : true); }
void	Connection::write(void) throw(SockStream::SendFunctionException) { stream.write(); }
int		Connection::sock(void) const { return stream.getSock(); }
bool	Connection::hasOutputMessage(void) const { return (stream.hasOutputMessage()); }
bool	Connection::hasInputMessage(void) const { return (stream.hasInputMessage()); }
void	Connection::clearMessages(void) { stream.clear(); }
void	Connection::send(Message const &msg) { stream.queueMessage(msg); }
void	Connection::send(Command const &cmd) { stream.queueMessage(cmd.message()); }
Message Connection::getLastMessage(void) { return (stream.getLastMessage()); }
Command Connection::getLastCommand(void) {
	if (hasInputMessage()) {
		return (Command(stream.getLastMessage()));
	}
	return (Command(NO_PREFIX, "", ""));
}

bool	Connection::isLocal(void) const			{ return (status & CONX_LOCAL); }
bool	Connection::isServer(void) const		{ return (status & CONX_SERVER); }
bool	Connection::isClient(void) const		{ return (status & CONX_CLIENT); }
bool	Connection::isService(void) const		{ return (status & CONX_SERVICE); }
bool	Connection::isConnect(void) const		{ return (status & CONX_CONNECT); }
bool	Connection::isIPv6(void) const			{ return (status & CONX_IPV6); }
bool	Connection::isAuthentified(void) const	{ return (status & CONX_AUTHEN); }
bool	Connection::isFinished(void) const		{ return (status & CONX_FINISH); }
bool	Connection::isLink(void) const			{ return (link != NO_LINK); }
bool	Connection::isPending(void) const		{ return (!isServer() && !isClient() && !isService()); }

void	Connection::isLocal(bool set)			{ applyFlag(CONX_LOCAL, set); }
void	Connection::isServer(bool set)			{ applyFlag(CONX_SERVER, set); }
void	Connection::isClient(bool set)			{ applyFlag(CONX_CLIENT, set); }
void	Connection::isService(bool set)			{ applyFlag(CONX_SERVICE, set); }
void	Connection::isConnect(bool set)			{ applyFlag(CONX_CONNECT, set); }
void	Connection::isIPv6(bool set)			{ applyFlag(CONX_IPV6, set); }
void	Connection::isAuthentified(bool set)	{ applyFlag(CONX_AUTHEN, set); }
void	Connection::isFinished(bool set)		{ applyFlag(CONX_FINISH, set); }

bool	Connection::checkStatus(unsigned short check) const {
	if (check == CONX_PENDING)
		return (isPending());
	return (status & check);
}
void	Connection::applyFlag(unsigned short flag, bool set) {
	if (!set)
		status ^= ((status ^ flag) > status ? CONX_NOFLAG : flag);
	else
		status |= flag;
}

void	Connection::end(void) throw(FailClose) {
	if (!isLink()) {
		if (close(sock()) == -1) {
			delete this;
			throw(Connection::FailClose());
		}
	}
	delete this;
}

// ########################################
// 				 EXECEPTIONS
// ########################################

Connection::ConxInit::~ConnectionInitException(void) throw() { return; }
Connection::ConxInit::ConnectionInitException(void) : content("Local socket exception occured.") { return; }
Connection::ConxInit::ConnectionInitException(std::string const &content) : content(content) { return; }
Connection::ConxInit::ConnectionInitException(ConnectionInitException const &cpy) :
	inherited(static_cast<inherited const &>(cpy)), content(cpy.content) { return; }
Connection::ConxInit &Connection::ConxInit::operator=(ConnectionInitException const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	content = cpy.content;
	return *this;
}
const char *Connection::ConxInit::what() const throw() {
	return (content.c_str());
}

Connection::FailClose::~CloseSocketException(void) throw() { return; }
Connection::FailClose::CloseSocketException(void) { return; }
Connection::FailClose::CloseSocketException(CloseSocketException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
Connection::FailClose &Connection::FailClose::operator=(CloseSocketException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *Connection::FailClose::what() const throw() {
	return ("close() function failed.");
}

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, Connection const &src) {
	flux << "Sock(" << src.sock() << ")flags[";
	flux << (src.isLocal() ? "1" : "0");
	flux << (src.isServer() ? "1" : "0");
	flux << (src.isClient() ? "1" : "0");
	flux << (src.isService() ? "1" : "0");
	flux << (src.isConnect() ? "1" : "0");
	flux << (src.isIPv6() ? "1" : "0");
	flux << (src.isAuthentified() ? "1" : "0");
	flux << (src.isFinished() ? "1" : "0");
	flux << "]";
	 // << src.info;
	return flux;
}
