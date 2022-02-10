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
**----- File ----------------{ AddrInfo.cpp }---------------------------------**
**----- Created -------------{ 2021-12-03 02:58:12 }--------------------------**
**----- Updated -------------{ 2022-02-02 23:37:50 }--------------------------**
********************************************************************************
*/

#include "AddrInfo.hpp"

AddrInfo::~AddrInfo() { return; }

AddrInfo::AddrInfo(AddrInfo const &cpy) :
	ai_flags(cpy.ai_flags),
	ai_family(cpy.ai_family),
	ai_socktype(cpy.ai_socktype),
	ai_protocol(cpy.ai_protocol),
	ai_addrlen(cpy.ai_addrlen),
	ai_addr(cpy.ai_addr),
	ai_canonname(cpy.ai_canonname) {return; }

AddrInfo &AddrInfo::operator=(AddrInfo const &cpy) {
	ai_flags = cpy.ai_flags;
	ai_family = cpy.ai_family;
	ai_socktype = cpy.ai_socktype;
	ai_protocol = cpy.ai_protocol;
	ai_addrlen = cpy.ai_addrlen;
	ai_addr = cpy.ai_addr;
	ai_canonname = cpy.ai_canonname;
	return *this;
}

static void hexdump(void const *ptr, int buflen) {
  unsigned char *buf = (unsigned char *)ptr;
  int i, j;
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

AddrInfo::AddrInfo(t_ai *src) :
	ai_flags(src->ai_flags),
	ai_family(src->ai_family),
	ai_socktype(src->ai_socktype),
	ai_protocol(src->ai_protocol),
	ai_addrlen(src->ai_addrlen),
	ai_canonname((src->ai_canonname ? src->ai_canonname : std::string())) {
	std::memset(&ai_addr, 0, sizeof(t_saddr_s));
	std::memcpy(&ai_addr, reinterpret_cast<t_saddr_s *>(src->ai_addr), ai_addrlen);
}

AddrInfo::AddrInfo(t_saddr_s const &src) :
	ai_family(src.ss_family), ai_addr(src) { return; }

bool	AddrInfo::isIP(void) const { return (isIPv4() || isIPv6() ? true : false); }
bool	AddrInfo::isIPv6(void) const { return (ai_family == AF_INET6 ? true : false); }
bool	AddrInfo::isIPv4(void) const { return (ai_family == AF_INET ? true : false); }
bool	AddrInfo::isIP(u_int16_t family) const { return (isIPv4(family) || isIPv6(family) ? true : false); }
bool	AddrInfo::isIPv6(u_int16_t family) const { return (family == AF_INET6 ? true : false); }
bool	AddrInfo::isIPv4(u_int16_t family) const { return (family == AF_INET ? true : false); }

std::string		AddrInfo::retrieveAddr(void) const {
	char 		addr[INET6_ADDRSTRLEN];

	if (!inet_ntop(ai_family, getAddress(), addr, sizeof(addr)))
		return (std::string());
	return (std::string(addr));
}

u_int16_t		AddrInfo::retrievePort(void) const { return (ntohs(getPort())); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void const		*AddrInfo::getAddress(void) const {
	if (isIPv6())
		return (&(reinterpret_cast<t_saddr6 const *>(&ai_addr)->sin6_addr));
	else
		return (&(reinterpret_cast<t_saddr const *>(&ai_addr)->sin_addr));
}

u_int16_t		 AddrInfo::getPort(void) const {
	if (isIPv6())
		return (reinterpret_cast<t_saddr6 const *>(&ai_addr)->sin6_port);
	else
		return (reinterpret_cast<t_saddr const *>(&ai_addr)->sin_port);
}

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, AddrInfo const &src) {
	flux << "family [" << (src.ai_family == AF_INET ? "v4" : "v6") << "]";
	flux << " Addr \'" << src.retrieveAddr() << "\' port:" << src.retrievePort() << std::endl;
	return flux;
}
