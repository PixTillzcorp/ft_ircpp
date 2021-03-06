#ifndef FT_IRC_SOCKSTREAM_HPP
#define FT_IRC_SOCKSTREAM_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <sys/socket.h>

#include "Message.hpp"

// #define READ_DELAY (CLOCKS_PER_SEC * 0.2f)

class SockStream {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~SockStream(void);
	SockStream(void);
	SockStream(SockStream const &src);
	SockStream &operator=(SockStream const &src);

	// _____________Constructor______________
	SockStream(int const sock);

	// __________Member functions____________
	bool read(void) throw(FailRecv);
	void write(void) throw(FailSend);
	Message getLastMessage(void);
	void queueMessage(Message msg);

	bool hasInputMessage(void) const;
	bool hasOutputMessage(void) const;

	void clearIn(void);
	void clearOut(void);

	// ____________Setter / Getter___________
	// _sock
	int					getSock(void) const;
	void				setSock(int const &src);

	// _rmsg
	std::queue<Message>	getRmsg(void) const;
	void				setRmsg(std::queue<Message> const &src);

	// _wmsg
	std::queue<Message>	getWmsg(void) const;
	void				setWmsg(std::queue<Message> const &src);

	// ______________Exceptions______________
	typedef class SendFunctionException : public std::exception { // send() return -1
	public:
		typedef std::exception inherited;

		virtual ~SendFunctionException(void) throw();
		SendFunctionException(void);
		SendFunctionException(SendFunctionException const &src);
		SendFunctionException &operator=(SendFunctionException const &src);
		virtual const char *what() const throw();
	} FailSend;

	typedef class RecvFunctionException : public std::exception { // recv() return -1
	public:
		typedef std::exception inherited;
		
		virtual ~RecvFunctionException(void) throw();
		RecvFunctionException(void);
		RecvFunctionException(RecvFunctionException const &src);
		RecvFunctionException &operator=(RecvFunctionException const &src);
		virtual const char *what() const throw();
	} FailRecv;


private: // ####################################################################

	int					_sock;
	std::queue<Message>	_rmsg;
	std::queue<Message>	_wmsg;
	
	void freshMessage(void);
	void readMessage(std::stringstream &input);
	void writeMessage(Message &msg);
};

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, SockStream const &src);

#endif //FT_IRC_SOCKSTREAM_HPP
