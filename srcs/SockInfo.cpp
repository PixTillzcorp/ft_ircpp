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
**----- File ----------------{ SockInfo.cpp }---------------------------------**
**----- Created -------------{ 2021-05-05 14:52:08 }--------------------------**
**----- Updated -------------{ 2021-09-03 15:24:04 }--------------------------**
********************************************************************************
*/

#include "../incs/SockInfo.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
SockInfo::~SockInfo(void) { 
	if (this->_info)
		freeaddrinfo(this->_info);
	return;
}
SockInfo::SockInfo(void) :	_host(""),
							_port(""),
							_bindable(false),
							_info(nullptr) { return; }
SockInfo::SockInfo(SockInfo const &src) {
	*this = src;
	return;
}
SockInfo	&SockInfo::operator=(SockInfo const &src) {
	this->_host = src.getHost();
	this->_port = src.getPort();
	this->_bindable = src.getBindable();
	if (src.getInfo())
		this->retrieveInfo(src.getInfo()->ai_family);
	return *this;
}

// ____________Constructors______________
SockInfo::SockInfo(std::string const &host, std::string const &port, u_int16_t family, bool bindable) :	_info(nullptr),
																										_host(host),
																										_port(port),
																										_bindable(bindable) {
	t_ai	hints;

	std::memset(&hints, 0, sizeof(hints));
	if (!getprotobyname("tcp") || !this->isIP(family))
	{
		throw(SockInfo::TcpProtocolException());
		delete this;
	}
	else
		this->retrieveInfo(family);
}

SockInfo::SockInfo(t_sa const *sockaddr, bool bindable) : _info(nullptr), _bindable(bindable) {
	if (!sockaddr || !getprotobyname("tcp") || (!this->isIPv4(sockaddr) && !this->isIPv6(sockaddr))) {
		throw(SockInfo::TcpProtocolException());
		delete this;
	}
	else {
		this->_host = this->getAddrFromSa(sockaddr);
		this->_port = std::to_string(this->getPortFromSa(sockaddr));
		this->retrieveInfo(sockaddr->sa_family);
	}
}

// __________Member functions____________
t_ai const			*SockInfo::addrinfo(void) const { return this->_info; }
t_ai const			*SockInfo::addrinfoX(unsigned int x) const throw(SockInfo::NullPtrDataException) {
	t_ai			*ptr = this->_info;

	if (!ptr)
		throw(SockInfo::NullPtrDataException());
	while (ptr and x > 0) {
		ptr = ptr->ai_next;
		x--;
	}
	return (!ptr ? this->addrinfo() : ptr);
}

t_sa const			*SockInfo::sockaddr(void) const { return this->_info->ai_addr; }
t_sa const			*SockInfo::sockaddrX(unsigned int x) const throw(SockInfo::NullPtrDataException) {
	t_ai			*ptr = this->_info;

	if (!ptr)
		throw(SockInfo::NullPtrDataException());
	while (ptr and x > 0) {
		ptr = ptr->ai_next;
		x--;
	}
	return (!ptr ? this->sockaddr() : ptr->ai_addr);
}

u_int16_t			SockInfo::socktype(void) const { return this->addrinfo()->ai_socktype; }
u_int16_t			SockInfo::socktypeX(unsigned int x) const { return this->addrinfoX(x)->ai_socktype; }
u_int16_t			SockInfo::protocol(void) const { return this->addrinfo()->ai_protocol; }
u_int16_t			SockInfo::protocolX(unsigned int x) const { return this->addrinfoX(x)->ai_protocol; }
socklen_t			SockInfo::addrLen(void) const { return this->addrinfo()->ai_addrlen; }
socklen_t			SockInfo::addrLenX(unsigned int x) const { return this->addrinfoX(x)->ai_addrlen; }
std::string			SockInfo::addrString(void) const { return this->getAddrFromSa(this->sockaddr()); }
std::string			SockInfo::addrStringX(unsigned int x) const { return this->getAddrFromSa(this->sockaddrX(x)); }
u_int16_t			SockInfo::family(void) const { return this->sockaddr()->sa_family; }
u_int16_t			SockInfo::familyX(unsigned int x) const { return this->sockaddrX(x)->sa_family; }
u_int16_t			SockInfo::port(void) const { return this->getPortFromSa(this->sockaddr()); }
u_int16_t			SockInfo::portX(unsigned int x) const { return this->getPortFromSa(this->sockaddrX(x)); }
int					SockInfo::nbrInfo(void) const {
	t_ai			*ptr = this->_info;
	int				c = 0;

	while (ptr) {
		ptr = ptr->ai_next;
		c++;
	}
	return (c);
}

// ____________Setter / Getter___________
// _host
std::string	const	SockInfo::getHost(void) const { return this->_host; }
void				SockInfo::setHost(std::string const &src) { this->_host = src; }

// _port
std::string	const	SockInfo::getPort(void) const { return this->_port; }
void				SockInfo::setPort(std::string const &src) { this->_port = src; }

// _bindable
bool const			SockInfo::getBindable(void) const { return this->_bindable; }
void				SockInfo::setBindable(bool const &src) { this->_bindable = src; }

// _info
t_ai 				*SockInfo::getInfo(void) const { return this->_info; }
t_ai 				*SockInfo::getInfoX(unsigned int x) const {
	t_ai			*ptr = this->_info;

	while (ptr and x > 0) {
		ptr = ptr->ai_next;
		x--;
	}
	if (!ptr)
		return (nullptr);
	else
		return (ptr);
}

// ########################################
// 				   PRIVATE
// ########################################

bool			SockInfo::isIP(u_int16_t family) const { return (family == AF_INET || family == AF_INET6 || family == AF_UNSPEC); }
bool			SockInfo::isIPv6(t_sa const *sockaddr) const { return (sockaddr->sa_family == AF_INET6); }
bool			SockInfo::isIPv4(t_sa const *sockaddr) const { return (sockaddr->sa_family == AF_INET); }
t_sin6 const	*SockInfo::getSin6(t_sa const *sockaddr) const { return (reinterpret_cast<t_sin6 const *>(sockaddr)); }
t_sin const		*SockInfo::getSin(t_sa const *sockaddr) const { return (reinterpret_cast<t_sin const *>(sockaddr)); }
socklen_t		SockInfo::getSaLen(t_sa const *sockaddr) const { return (this->isIPv6(sockaddr) ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN); }

std::string		SockInfo::getAddrFromSa(t_sa const *sockaddr) const {
	char 			ret6[INET6_ADDRSTRLEN];
	char 			ret[INET_ADDRSTRLEN];

	if (this->isIPv6(sockaddr)) {
		if (!inet_ntop(sockaddr->sa_family, &(this->getSin6(sockaddr)->sin6_addr), ret6, this->getSaLen(sockaddr)))
			return (std::string());
		return (std::string(ret6));
	}
	else {
		if (!inet_ntop(sockaddr->sa_family, &(this->getSin(sockaddr)->sin_addr), ret, this->getSaLen(sockaddr)))
			return (std::string());
		return (std::string(ret));
	}
}

u_int16_t		SockInfo::getPortFromSa(t_sa const *sockaddr) const {
	if (this->isIPv6(sockaddr))
		return (ntohs(this->getSin6(sockaddr)->sin6_port));
	else 
		return (ntohs(this->getSin(sockaddr)->sin_port));
}

void			SockInfo::retrieveInfo(u_int16_t family) throw (SockInfo::GetAddrInfoException) {
	t_ai	hints;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = SOCK_STREAM;
	// hints.ai_protocol = getprotobyname("tcp")->p_proto;
	if (this->_bindable)
		hints.ai_flags |= AI_PASSIVE; /* For bind() function call if used */
	if (getaddrinfo(this->_host.c_str(), this->_port.c_str(), &hints, &this->_info))
	{
		throw(SockInfo::GetAddrInfoException());
		delete this;
	}
}

// ########################################
// 				 EXECEPTIONS
// ########################################

SockInfo::TcpProtocolException::~TcpProtocolException(void) throw() { return; }
SockInfo::TcpProtocolException::TcpProtocolException(void) { return; }
SockInfo::TcpProtocolException::TcpProtocolException(TcpProtocolException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
SockInfo::TcpProtocolException &SockInfo::TcpProtocolException::operator=(TcpProtocolException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *SockInfo::TcpProtocolException::what() const throw() {
	return ("Tcp Protocol is unknown on the current device.");
}

SockInfo::GetAddrInfoException::~GetAddrInfoException(void) throw() { return; }
SockInfo::GetAddrInfoException::GetAddrInfoException(void) { return; }
SockInfo::GetAddrInfoException::GetAddrInfoException(GetAddrInfoException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
SockInfo::GetAddrInfoException &SockInfo::GetAddrInfoException::operator=(GetAddrInfoException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *SockInfo::GetAddrInfoException::what() const throw() {
	return ("\'getaddrinfo\' function failed.");
}

SockInfo::NullPtrDataException::~NullPtrDataException(void) throw() { return; }
SockInfo::NullPtrDataException::NullPtrDataException(void) { return; }
SockInfo::NullPtrDataException::NullPtrDataException(NullPtrDataException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
SockInfo::NullPtrDataException &SockInfo::NullPtrDataException::operator=(NullPtrDataException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *SockInfo::NullPtrDataException::what() const throw() {
	return ("Private class variable \'_info\' or its \'sockaddr\' was egal to \'nullptr\'.");
}
// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, SockInfo const &src) {
	flux << "_host: " << src.getHost() << std::endl;
	flux << "_port: " << src.getPort() << std::endl;
	flux << "address: " << src.addrString() << std::endl;
	return flux;
}
