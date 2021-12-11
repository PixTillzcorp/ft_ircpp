#ifndef FT_IRCPP_CONNECTION_HPP
#define FT_IRCPP_CONNECTION_HPP

#include "../incs/CommandLib.hpp"
#include "../incs/SockStream.hpp"
#include "../incs/SockInfo.hpp"

#define MAX_PENDING_CONNECTION 15
#define NO_SOCK -1
#define NO_LINK nullptr

#define CONX_LOCAL		128
#define CONX_SERVER		64
#define CONX_CLIENT		32
#define CONX_SERVICE	16
#define CONX_PENDING	8
#define CONX_IPV6		4
#define CONX_AUTHEN		2
#define CONX_FINISH		1
#define CONX_NOFLAG		0

class Connection {
public:
	SockInfo			info;
	SockStream			stream;
	unsigned short		status;
	Connection			*link;
	size_t				hop;

	// ____________Canonical Form____________
	virtual ~Connection();
	Connection();
	Connection(Connection const &src);
	Connection &operator=(Connection const &src);

	// ____________Constructors______________
	Connection(std::string const &port, u_int16_t family) throw(Connection::ConxInit);	// socket/bind/listen
	Connection(int lsock) throw(Connection::ConxInit);									// accept
	Connection(Connection *link, unsigned short status, size_t hop);					// link

	// __________Member functions____________
	std::string const			&hostname(void) const;
	virtual std::string const	&name(void) const;

	bool	read(void);
	void	write(void) throw(SockStream::SendFunctionException);
	int		sock(void) const;
	bool	hasOutputMessage(void) const;
	bool	hasInputMessage(void) const;
	void	clearMessages(void);
	void	send(Message const &msg);
	void	send(Command const &cmd);
	Message *getLastMessage(void);
	Command *getLastCommand(void);

	bool	isLocal(void) const;
	bool	isServer(void) const;
	bool	isClient(void) const;
	bool	isService(void) const;
	bool	isIPv6(void) const;
	bool	isAuthentified(void) const;
	bool	isFinished(void) const;
	bool	isLink(void) const;
	bool	isPending(void) const;

	void	isLocal(bool set);
	void	isServer(bool set);
	void	isClient(bool set);
	void	isService(bool set);
	void	isIPv6(bool set);
	void	isAuthentified(bool set);
	void	isFinished(bool set);

	typedef class ConnectionInitException : public std::exception { // socket setup failed
	public:
		typedef std::exception inherited;

		std::string content;

		virtual ~ConnectionInitException() throw();
		ConnectionInitException(ConnectionInitException const &cpy);
		ConnectionInitException &operator=(ConnectionInitException const &cpy);
		ConnectionInitException(std::string const &content);
		virtual const char *what() const throw();

	private:
		ConnectionInitException();
	} ConxInit;

	bool	checkStatus(unsigned short check) const;
	void	applyFlag(unsigned short flag, bool set);
};

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, Connection const &src);

#endif //FT_IRCPP_CONNECTION_HPP
