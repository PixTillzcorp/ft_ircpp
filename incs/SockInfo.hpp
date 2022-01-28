#ifndef FT_IRCPP_SOCKINFO_HPP
#define FT_IRCPP_SOCKINFO_HPP

#include "AddrInfo.hpp"
#include <exception>
#include <list>
#include <sstream>

#define BIND true
#define ACCEPT false
#define CONNECT false

class SockInfo {
public: // #####################################################################
	typedef std::list<AddrInfo>		ailist;
	typedef ailist::iterator		ailist_it;
	typedef ailist::const_iterator	ailist_cit;

	std::string		host;
	std::string		port;
	ailist			infos;

	// ____________Canonical Form____________
	~SockInfo();
	SockInfo();
	SockInfo(SockInfo const &cpy);
	SockInfo &operator=(SockInfo const &cpy);

	// ____________Constructors______________
	SockInfo(std::string const &host, std::string const &port, u_int16_t family, bool bind);
	SockInfo(t_saddr_s const &ss, bool bind);

	// __________Member functions____________
	ailist_it		begin(void)			/*_*/;
	ailist_it		end(void)			/*_*/;
	ailist_cit		cbegin(void)		const;
	ailist_cit		cend(void)			const;
	size_t			size(void)			const;
	AddrInfo const	&addrinfo(void)		const;
	AddrInfo const	&addrinfo(size_t x)	const;

	t_sa const		*sockaddr(void)		const;
	u_int16_t		flags(void)			const;
	u_int16_t		family(void)		const;
	u_int16_t		socktype(void)		const;
	u_int16_t		protocol(void)		const;
	socklen_t		addrLen(void)		const;
	std::string		addr(void)			const;
	u_int16_t		portNbr(void)		const;
	std::string		canonname(void)		const;

	t_sa const		*sockaddr(size_t x) const;
	u_int16_t		flags(size_t x)		const;
	u_int16_t		family(size_t x)	const;
	u_int16_t		socktype(size_t x)	const;
	u_int16_t		protocol(size_t x)	const;
	socklen_t		addrLen(size_t x)	const;
	std::string		addr(size_t x)		const;
	u_int16_t		portNbr(size_t x)	const;
	std::string		canonname(size_t x)	const;

	// ______________Exceptions______________
	typedef class TcpProtocolException : public std::exception { // Tcp protocol unknown
	public:
		typedef std::exception inherited;

		virtual ~TcpProtocolException(void) throw();
		TcpProtocolException(void);
		TcpProtocolException(TcpProtocolException const &src);
		TcpProtocolException &operator=(TcpProtocolException const &src);
		virtual const char *what() const throw();
	} NoTcp;

	typedef class GetAddrInfoException : public std::exception { // getaddrinfo failure
	public:
		typedef std::exception inherited;

		virtual ~GetAddrInfoException(void) throw();
		GetAddrInfoException(void);
		GetAddrInfoException(GetAddrInfoException const &src);
		GetAddrInfoException &operator=(GetAddrInfoException const &src);
		virtual const char *what() const throw();
	} FailGai;

private: // ####################################################################
	bool isIP(u_int16_t family) const;
	void retrieveInfo(u_int16_t family, bool bind) throw(FailGai);
};

// Debug function
std::ostream &operator<<(std::ostream &flux, SockInfo const &src);

#endif // FT_IRCPP_SOCKINFO_HPP
