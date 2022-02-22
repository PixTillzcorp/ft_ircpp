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
**----- Updated -------------{ 2022-02-22 02:19:09 }--------------------------**
********************************************************************************
*/

#include "SockInfo.hpp"

SockInfo::~SockInfo() { return; }
SockInfo::SockInfo() { return; }
SockInfo::SockInfo(SockInfo const &cpy) : 
	host(cpy.host),
	port(cpy.port),
	infos(cpy.infos) { return; }
SockInfo	&SockInfo::operator=(SockInfo const &cpy) {
	host = cpy.host;
	port = cpy.port;
	infos = cpy.infos;
	return *this;
}

SockInfo::SockInfo(std::string const &host, std::string const &port, u_int16_t family, bool bind) :
	host(host), port(port) {
	if (!getprotobyname("tcp") || !isIP(family))
		throw(SockInfo::NoTcp());
	else
		retrieveInfo(family, bind);
}

SockInfo::SockInfo(t_saddr_s const &ss, bool bind) {
	AddrInfo *tmp;

	if (!getprotobyname("tcp") || !isIP(ss.ss_family))
		throw(SockInfo::NoTcp());
	tmp = new AddrInfo(ss);
	host = tmp->retrieveAddr();
	port = Utils::nbrToStr(tmp->retrievePort());
	delete tmp;
	retrieveInfo(ss.ss_family, bind);
}

// __________Member functions____________
SockInfo::ailist_it		SockInfo::begin(void)	  /*_*/ { return infos.begin(); }
SockInfo::ailist_it		SockInfo::end(void)		  /*_*/ { return infos.end(); }
SockInfo::ailist_cit	SockInfo::cbegin(void)	  const { return infos.begin(); }
SockInfo::ailist_cit	SockInfo::cend(void)	  const { return infos.end(); }

size_t				SockInfo::size(void)		  const { return infos.size(); }
AddrInfo const		&SockInfo::addrinfo(void)	  const { return addrinfo(0); }
AddrInfo const		&SockInfo::addrinfo(size_t x) const {
	ailist::const_iterator it = infos.begin();

	if (x >= infos.size())
		x = infos.size() - 1;
	if (x > 0)
		std::advance(it, x);
	return (*it);
}

t_sa const			*SockInfo::sockaddr(void)	  const { return sockaddr(0); }
u_int16_t			SockInfo::flags(void)		  const { return flags(0); }
u_int16_t			SockInfo::family(void)		  const { return family(0); }
u_int16_t			SockInfo::socktype(void)	  const { return socktype(0); }
u_int16_t			SockInfo::protocol(void)	  const { return protocol(0); }
socklen_t			SockInfo::addrLen(void)		  const { return addrLen(0); }
std::string			SockInfo::addr(void)		  const { return addr(0); }
u_int16_t			SockInfo::portNbr(void)		  const { return portNbr(0); }
std::string			SockInfo::canonname(void)	  const { return canonname(0); }

t_sa const			*SockInfo::sockaddr(size_t x) const { return reinterpret_cast<t_sa const *>(&(addrinfo(x).ai_addr)); }
u_int16_t			SockInfo::flags(size_t x)	  const { return addrinfo(x).ai_flags; }
u_int16_t			SockInfo::family(size_t x) 	  const { return addrinfo(x).ai_family; }
u_int16_t			SockInfo::socktype(size_t x)  const { return addrinfo(x).ai_socktype; }
u_int16_t			SockInfo::protocol(size_t x)  const { return addrinfo(x).ai_protocol; }
socklen_t			SockInfo::addrLen(size_t x)	  const { return addrinfo(x).ai_addrlen; }
std::string			SockInfo::addr(size_t x)	  const { return addrinfo(x).retrieveAddr(); }
u_int16_t			SockInfo::portNbr(size_t x)	  const { return addrinfo(x).retrievePort(); }
std::string			SockInfo::canonname(size_t x) const {
	if (addrinfo(x).ai_canonname.empty()) {
		if (!(addr(x).compare("::1")) || !(addr(x).compare("::ffff:127.0.0.1")))
			return("localhost");
		else if (!(addr(x).compare("127.0.0.1")))
			return("localhost");
		else
			return addr(x);
	}
	else
		return std::string(addrinfo(x).ai_canonname);
}

bool			SockInfo::isIP(u_int16_t family) const {
	if (family == AF_UNSPEC || family == AF_INET || family == AF_INET6)
		return true;
	return false;
}

void			SockInfo::retrieveInfo(u_int16_t family, bool bind) throw(SockInfo::FailGai) {
	t_ai	*ret;
	t_ai	hints;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = getprotobyname("tcp")->p_proto;
	if (bind)
		hints.ai_flags |= AI_PASSIVE; /* For bind() function call if used */
	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &ret) || !ret)
		throw(SockInfo::FailGai());
	for (t_ai *head = ret; head != nullptr; head = head->ai_next)
		infos.push_back(AddrInfo(head));
	freeaddrinfo(ret);
}

// ########################################
// 				 EXECEPTIONS
// ########################################

SockInfo::NoTcp::~TcpProtocolException(void) throw() { return; }
SockInfo::NoTcp::TcpProtocolException(void) { return; }
SockInfo::NoTcp::TcpProtocolException(TcpProtocolException const &src) :
	inherited(static_cast<inherited const &>(src)) { return; }
SockInfo::NoTcp &SockInfo::NoTcp::operator=(TcpProtocolException const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	return *this;
}
const char *SockInfo::NoTcp::what() const throw() {
	return ("Tcp Protocol is unknown on the current device.");
}

SockInfo::FailGai::~GetAddrInfoException(void) throw() { return; }
SockInfo::FailGai::GetAddrInfoException(void) { return; }
SockInfo::FailGai::GetAddrInfoException(GetAddrInfoException const &src) :
	inherited(static_cast<inherited const &>(src)) { return; }
SockInfo::FailGai &SockInfo::FailGai::operator=(GetAddrInfoException const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	return *this;
}
const char *SockInfo::FailGai::what() const throw() {
	return ("\'getaddrinfo()\' failed.");
}

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, SockInfo const &src) {
	flux << "host \'" << src.host << "\' port [" << (src.port.empty() ? "None" : src.port) << "]" << std::endl;
	for (SockInfo::ailist_cit it = src.cbegin(); it != src.cend(); it++) {
		flux << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		flux << (*it);
	}
	flux << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	return flux;
}
