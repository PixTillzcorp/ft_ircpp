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
**----- Updated -------------{ 2021-09-03 17:39:59 }--------------------------**
********************************************************************************
*/

#include "../incs/Connection.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Connection::~Connection(void) { return; }
// Connection::Connection(void) { return; }
Connection::Connection(Connection const &src) { *this = src; }
Connection	&Connection::operator=(Connection const &src) {
	this->_sin = src.getSockinfo();
	this->_stream = src.getStream();
	this->_status = src.getStatus();
	return *this;
}

// _____________Constructor______________
Connection::Connection(std::string const &port, u_int16_t family) throw(Connection::LocalSocketException) :
																	_sin(SockInfo("", port, family, BINDABLE)),
																	_status(CONX_LOCAL | (family == AF_INET6 ? CONX_IPV6 : CONX_NOFLAG)) {
	int yes = 1;
	int	sock;

	if ((sock = socket(this->_sin.family(), this->_sin.socktype(), this->_sin.protocol())) == -1)
		throw(Connection::LocalSocketException("socket() function failed."));
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		throw(Connection::LocalSocketException("setsockopt() function failed."));
	if (bind(sock, this->_sin.sockaddr(), this->_sin.addrLen()) == -1)
		throw(Connection::LocalSocketException("bind() function failed."));
	if (listen(sock, MAX_PENDING_CONNECTION) == -1)
		throw(Connection::LocalSocketException("listen() function failed."));
	this->_stream = SockStream(sock);
}

Connection::Connection(SockInfo const &sin, int sock, unsigned char status) :	_sin(sin),
																				_stream(SockStream(sock)),
																				_status(status) {
	return;
}

// __________Member functions____________
std::string Connection::name(void) { return this->_sin.addrString(); }
// Command		*Connection::getNextCommand(void) {
// 	Message *msg;

// 	if (!this->_finished)
// 	{
// 		if (!(msg = this->_stream.getLastMessage()))
// 			return NULL;
// 		else
// 			return (new Command(msg));
// 	}
// 	return NULL;
// }

bool Connection::read(void) {
	if (!(this->_stream.read()))
	{
		return false;
	}
	return true;
}

void Connection::write(void) throw(SockStream::SendFunctionException) { this->_stream.write(); }
bool Connection::hasOutputMessage(void) const { return (this->_stream.hasOutputMessage()); }
bool Connection::hasInputMessage(void) const { return (this->_stream.hasInputMessage()); }

// void Connection::sendCommand(Command const &cmd) {
// 	if (!this->_finished)
// 		this->_stream.queueMessage(cmd.message());
// }

bool	Connection::isLocal(void) const { return ((this->_status >> BIT_LOCAL) & 1); }
bool	Connection::isServer(void) const { return ((this->_status >> BIT_SERVER) & 1); }
bool	Connection::isClient(void) const { return ((this->_status >> BIT_CLIENT) & 1); }
bool	Connection::isService(void) const { return ((this->_status >> BIT_SERVICE) & 1); }
bool	Connection::isPending(void) const { return (!this->isServer() && !this->isClient() && !this->isService()); }
bool	Connection::isIPv6(void) const { return ((this->_status >> BIT_IPV6) & 1); }
bool	Connection::isAuthentified(void) const { return ((this->_status >> BIT_AUTHEN) & 1); }
bool	Connection::isFinished(void) const { return ((this->_status >> BIT_FINISH) & 1); }
int		Connection::sock(void) const { return this->_stream.getSock(); }

void	Connection::setLocal(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_LOCAL); }
void	Connection::setServer(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_SERVER); }
void	Connection::setClient(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_CLIENT); }
void	Connection::setService(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_SERVICE); }
void	Connection::setIPv6(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_IPV6); }
void	Connection::setAuthentified(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_AUTHEN); }
void	Connection::setFinished(void) throw(Connection::FlagException) { this->setStatusFlag(CONX_FINISH); }

void	Connection::unsetLocal(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_LOCAL); }
void	Connection::unsetServer(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_SERVER); }
void	Connection::unsetClient(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_CLIENT); }
void	Connection::unsetService(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_SERVICE); }
void	Connection::unsetIPv6(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_IPV6); }
void	Connection::unsetAuthentified(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_AUTHEN); }
void	Connection::unsetFinished(void) throw(Connection::FlagException) { this->unsetStatusFlag(CONX_FINISH); }

// int	Connection::getSock(void) const { return this->_stream.getSock(); }

// ____________Setter / Getter___________
// _sin
SockInfo const		&Connection::getSockinfo(void) const { return this->_sin; }
void		Connection::setSockinfo(SockInfo const &src) { this->_sin = src; }

// _stream
SockStream const	&Connection::getStream(void) const { return this->_stream; }
void		Connection::setStream(SockStream const &src) { this->_stream = src; }

// _status
unsigned char const	&Connection::getStatus(void) const { return this->_status; }
void		Connection::setStatus(unsigned char const &src) { this->_status = src; }

// ########################################
// 				   PRIVATE
// ########################################

void Connection::setStatusFlag(unsigned char flag) {
	if (this->_status & flag)
		throw(Connection::FlagException());
	if (flag == CONX_LOCAL && (this->isServer() || this->isClient() || this->isService() || !this->isAuthentified()))
		throw(Connection::FlagException());
	if (flag == CONX_SERVER && (this->isClient() || this->isService() || this->isLocal() || !this->isAuthentified()))
		throw(Connection::FlagException());
	if (flag == CONX_CLIENT && (this->isServer() || this->isService() || this->isLocal() || !this->isAuthentified()))
		throw(Connection::FlagException());
	if (flag == CONX_SERVICE && (this->isServer() || this->isClient() || this->isLocal() || !this->isAuthentified()))
		throw(Connection::FlagException());
	this->_status |= flag;
}

void Connection::unsetStatusFlag(unsigned char flag) {
	if ((this->_status ^ flag) > this->_status)
		throw(Connection::FlagException());
	this->_status ^= flag;
}

// ########################################
// 				 EXECEPTIONS
// ########################################

Connection::FlagException::~FlagException(void) throw() { return; }
Connection::FlagException::FlagException(void) { return; }
Connection::FlagException::FlagException(FlagException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
Connection::FlagException &Connection::FlagException::operator=(FlagException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *Connection::FlagException::what() const throw() {
	return ("Flag was already set/unset or a flag conflict occured.");
}

Connection::LocalSocketException::~LocalSocketException(void) throw() { return; }
Connection::LocalSocketException::LocalSocketException(void) : content("Local socket exception occured.") { return; }
Connection::LocalSocketException::LocalSocketException(std::string const &src) : content(src) { return; }
Connection::LocalSocketException::LocalSocketException(LocalSocketException const &src) { *this = src; }
Connection::LocalSocketException &Connection::LocalSocketException::operator=(LocalSocketException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	this->content = src.content;
	return *this;
}
const char *Connection::LocalSocketException::what() const throw() {
	return (this->content.c_str());
}

// ########################################
// 					DEBUG
// ########################################

Message *Connection::getLastMessage(void) {
	return this->_stream.getLastMessage();
}

void Connection::queueMessage(Message msg) {
	this->_stream.queueMessage(msg);
}

// std::ostream &operator<<(std::ostream &flux, Connection const &src) {
// 	flux << "Connection [" << src.getStatus() << "]" << std::endl;
// 	flux << src.getSockinfo();
// 	flux << "+++++++++++++++++++++++++++++++++" << std::endl;
// 	return flux;
// }