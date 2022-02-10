#ifndef ADDR_INFO_HPP
#define ADDR_INFO_HPP

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct addrinfo			t_ai;
typedef struct sockaddr			t_sa;
typedef struct sockaddr_in		t_saddr;
typedef struct sockaddr_in6		t_saddr6;
typedef struct sockaddr_storage	t_saddr_s;

class AddrInfo {
public:
	~AddrInfo();
	AddrInfo(AddrInfo const &cpy);
	AddrInfo &operator=(AddrInfo const &cpy);

	AddrInfo(t_ai *src);
	AddrInfo(t_saddr_s const &src);

	bool			isIP(void) const;
	bool			isIPv6(void) const;
	bool			isIPv4(void) const;
	bool			isIP(u_int16_t family) const;
	bool			isIPv6(u_int16_t family) const;
	bool			isIPv4(u_int16_t family) const;

	std::string		retrieveAddr(void) const;
	u_int16_t		retrievePort(void) const;

	u_int16_t		ai_flags;
	u_int16_t		ai_family;
	u_int16_t		ai_socktype;
	u_int16_t		ai_protocol;
	socklen_t		ai_addrlen;
	t_saddr_s		ai_addr;
	std::string		ai_canonname;

private:
	AddrInfo();

	void const		*getAddress(void) const;
	u_int16_t 		getPort(void) const;
};

// Debug function
std::ostream &operator<<(std::ostream &flux, AddrInfo const &src);

#endif //ADDR_INFO_HPP
