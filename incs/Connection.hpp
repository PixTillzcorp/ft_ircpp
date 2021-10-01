#ifndef FT_IRCPP_CONNECTION_HPP
#define FT_IRCPP_CONNECTION_HPP

#include "../incs/CommandLib.hpp"
#include "../incs/SockStream.hpp"
#include "../incs/SockInfo.hpp"

#define MAX_PENDING_CONNECTION 15
#define NO_SOCK -1

#define CONX_LOCAL 128
#define CONX_SERVER 64
#define CONX_CLIENT 32
#define CONX_SERVICE 16
#define CONX_UNUSED 8
#define CONX_IPV6 4
#define CONX_AUTHEN 2
#define CONX_FINISH 1
#define CONX_NOFLAG 0

#define BIT_LOCAL 7
#define BIT_SERVER 6
#define BIT_CLIENT 5
#define BIT_SERVICE 4
#define BIT_UNUSED 3
#define BIT_IPV6 2
#define BIT_AUTHEN 1
#define BIT_FINISH 0

class Connection {
public: // #####################################################################
	
	// ____________Canonical Form____________
	virtual ~Connection();
	Connection();
	Connection(Connection const &src);
	Connection &operator=(Connection const &src);

	// _____________Constructor______________
	Connection(std::string const &port, u_int16_t family) throw(LocalSocketException);
	Connection(SockInfo const &sin, int sock, unsigned char status);

	// __________Member functions____________
	bool read(void);
	void write(void) throw(SockStream::SendFunctionException);
	bool hasOutputMessage(void) const;
	bool hasInputMessage(void) const;
	void clearMessages(void);

	//debug waiting to get operational 'Command.cpp'.
	Message *getLastMessage(void);
	void queueMessage(Message msg);
	Command *getLastCommand(void);
	void queueCommand(Command const &cmd);
	void send(Command const &cmd); //same that 'queueCommand' but more explicit.

	std::string const hostname(void) const;

	virtual std::string const name(void) const;

	bool isLocal(void) const;
	bool isServer(void) const;
	bool isClient(void) const;
	bool isService(void) const;
	bool isPending(void) const;
	bool isIPv6(void) const;
	bool isAuthentified(void) const;
	bool isFinished(void) const;
	bool isRegistered(void) const;
	int	 sock(void) const;

	void setLocal(void) throw(FlagException);
	void setServer(void) throw(FlagException);
	void setClient(void) throw(FlagException);
	void setService(void) throw(FlagException);
	void setIPv6(void) throw(FlagException);
	void setAuthentified(void) throw(FlagException);
	void setFinished(void) throw(FlagException);

	void unsetLocal(void) throw(FlagException);
	void unsetServer(void) throw(FlagException);
	void unsetClient(void) throw(FlagException);
	void unsetService(void) throw(FlagException);
	void unsetIPv6(void) throw(FlagException);
	void unsetAuthentified(void) throw(FlagException);
	void unsetFinished(void) throw(FlagException);
	// int	getSock(void) const;

	// ____________Setter / Getter___________
	// _sin
	SockInfo const		&getSockInfo(void) const;
	void				setSockinfo(SockInfo const &src);

	// _stream
	SockStream const	&getStream(void) const;
	void				setStream(SockStream const &src);

	// _status
	unsigned char const	&getStatus(void) const;
	void				setStatus(unsigned char const &src);

	// _link
	Connection			*getLink(void) const;
	void				setLink(Connection *src);
	bool				isLink(void) const;

	// ______________Exceptions______________
	class FlagException : public std::exception { // Flag set/unset error
	public:
		virtual ~FlagException(void) throw();
		FlagException(void);
		FlagException(FlagException const &src);
		FlagException &operator=(FlagException const &src);
		virtual const char *what() const throw();
	};

	class LocalSocketException : public std::exception { // socket setup failed
	public:
		std::string content;

		virtual ~LocalSocketException(void) throw();
		LocalSocketException(void);
		LocalSocketException(std::string const &src);
		LocalSocketException(LocalSocketException const &src);
		LocalSocketException &operator=(LocalSocketException const &src);
		virtual const char *what() const throw();
	};

protected: // ##################################################################

	SockInfo			_sin;
	SockStream			_stream;
	unsigned char		_status;
	Connection			*_link;

	void unsetStatusFlag(unsigned char flag);
	void setStatusFlag(unsigned char flag);
};

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, Connection const &src);

#endif //FT_IRCPP_CONNECTION_HPP
