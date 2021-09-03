#ifndef FT_IRCPP_SOCKINFO_HPP
#define FT_IRCPP_SOCKINFO_HPP

#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <string>
#include <iostream>
#include <vector>

typedef struct addrinfo		t_ai;
typedef struct sockaddr		t_sa;
typedef struct sockaddr_in	t_sin;
typedef struct sockaddr_in6	t_sin6;

#define BINDABLE true
#define CONNECTABLE false

class SockInfo {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~SockInfo(void);
	SockInfo(void);
	SockInfo(SockInfo const &src);
	SockInfo &operator=(SockInfo const &src);

	// ____________Constructors______________
	SockInfo(std::string const &host, std::string const &port, u_int16_t family, bool bindable);
	SockInfo(t_sa const *sockaddr, bool bindable);

	// __________Member functions____________
	t_ai const			*addrinfo(void) const;
	t_ai const			*addrinfoX(unsigned int x) const throw(NullPtrDataException);
	t_sa const			*sockaddr(void) const;
	t_sa const			*sockaddrX(unsigned int x) const throw(NullPtrDataException);

	u_int16_t			socktype(void) const;
	u_int16_t			socktypeX(unsigned int x) const;
	u_int16_t			protocol(void) const;
	u_int16_t			protocolX(unsigned int x) const;
	socklen_t			addrLen(void) const;
	socklen_t			addrLenX(unsigned int x) const;
	std::string			addrString(void) const;
	std::string			addrStringX(unsigned int x) const;
	u_int16_t			family(void) const;
	u_int16_t			familyX(unsigned int x) const;
	u_int16_t			port(void) const;
	u_int16_t			portX(unsigned int x) const;
	int					nbrInfo(void) const;
	
	// ____________Setter / Getter___________
	// _host
	std::string	const	getHost(void) const;
	void				setHost(std::string const &src);

	// _port
	std::string	const	getPort(void) const;
	void				setPort(std::string const &src);

	// _type
	bool const			getBindable(void) const;
	void				setBindable(bool const &src);

	// _info
	t_ai				*getInfo(void) const;
	t_ai 				*getInfoX(unsigned int x) const;

	// ______________Exceptions______________
	class TcpProtocolException : public std::exception { // Tcp protocol unknown
	public:
		virtual ~TcpProtocolException(void) throw();
		TcpProtocolException(void);
		TcpProtocolException(TcpProtocolException const &src);
		TcpProtocolException &operator=(TcpProtocolException const &src);
		virtual const char *what() const throw();
	};

	class GetAddrInfoException : public std::exception { // getaddrinfo failure
	public:
		virtual ~GetAddrInfoException(void) throw();
		GetAddrInfoException(void);
		GetAddrInfoException(GetAddrInfoException const &src);
		GetAddrInfoException &operator=(GetAddrInfoException const &src);
		virtual const char *what() const throw();
	};

	class NullPtrDataException : public std::exception { // getaddrinfo failure
	public:
		virtual ~NullPtrDataException(void) throw();
		NullPtrDataException(void);
		NullPtrDataException(NullPtrDataException const &src);
		NullPtrDataException &operator=(NullPtrDataException const &src);
		virtual const char *what() const throw();
	};


private: // ####################################################################
	
	std::string		_host;
	std::string		_port;
	bool			_bindable;
	t_ai			*_info;

	bool			isIP(u_int16_t family) const;
	bool			isIPv6(t_sa const *sockaddr) const;
	bool			isIPv4(t_sa const *sockaddr) const;
	t_sin6 const	*getSin6(t_sa const *sockaddr) const;
	t_sin const		*getSin(t_sa const *sockaddr) const;
	socklen_t		getSaLen(t_sa const *sockaddr) const;
	std::string		getAddrFromSa(t_sa const *sockaddr) const;
	u_int16_t		getPortFromSa(t_sa const *sockaddr) const;

	void			retrieveInfo(u_int16_t family) throw (GetAddrInfoException);
};

// Debug function
std::ostream &operator<<(std::ostream &flux, SockInfo const &src);

#endif // FT_IRCPP_SOCKINFO_HPP
