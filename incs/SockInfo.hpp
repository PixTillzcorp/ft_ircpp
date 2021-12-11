#ifndef FT_IRCPP_SOCKINFO_HPP
#define FT_IRCPP_SOCKINFO_HPP

#include "AddrInfo.hpp"
#include <exception>
#include <list>
#include <sstream>

#define BIND true
#define ACCEPT false

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
	ailist_it		begin(void)			/*_*/ throw(NoData);
	ailist_it		end(void)			/*_*/ throw(NoData);
	ailist_cit		cbegin(void)		const throw(NoData);
	ailist_cit		cend(void)			const throw(NoData);
	size_t			size(void)			const throw(NoData);
	AddrInfo const	&addrinfo(void)		const throw(NoData);
	AddrInfo const	&addrinfo(size_t x)	const throw(NoData);

	t_sa const		*sockaddr(void)		const throw(NoData);
	u_int16_t		flags(void)			const throw(NoData);
	u_int16_t		family(void)		const throw(NoData);
	u_int16_t		socktype(void)		const throw(NoData);
	u_int16_t		protocol(void)		const throw(NoData);
	socklen_t		addrLen(void)		const throw(NoData);
	std::string		addr(void)			const throw(NoData);
	u_int16_t		portNbr(void)		const throw(NoData);
	std::string		canonname(void)		const throw(NoData);

	t_sa const		*sockaddr(size_t x) const throw(NoData);
	u_int16_t		flags(size_t x)		const throw(NoData);
	u_int16_t		family(size_t x)	const throw(NoData);
	u_int16_t		socktype(size_t x)	const throw(NoData);
	u_int16_t		protocol(size_t x)	const throw(NoData);
	socklen_t		addrLen(size_t x)	const throw(NoData);
	std::string		addr(size_t x)		const throw(NoData);
	u_int16_t		portNbr(size_t x)	const throw(NoData);
	std::string		canonname(size_t x)	const throw(NoData);

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

	typedef class EmptyDataException : public std::exception { // getaddrinfo failure
	public:
		typedef std::exception inherited;

		virtual ~EmptyDataException(void) throw();
		EmptyDataException(void);
		EmptyDataException(EmptyDataException const &src);
		EmptyDataException &operator=(EmptyDataException const &src);
		virtual const char *what() const throw();
	} NoData;


private: // ####################################################################
	bool				isIP(u_int16_t family) const;
	std::string	const	nbrToStr(unsigned int nbr) const;
	void				retrieveInfo(u_int16_t family, bool bind) throw(FailGai, NoData);
};

// Debug function
std::ostream &operator<<(std::ostream &flux, SockInfo const &src);

#endif // FT_IRCPP_SOCKINFO_HPP
