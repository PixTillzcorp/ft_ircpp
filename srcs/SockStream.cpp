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
**----- File ----------------{ SockStream.cpp }-------------------------------**
**----- Created -------------{ 2021-05-07 16:21:55 }--------------------------**
**----- Updated -------------{ 2022-02-17 03:28:08 }--------------------------**
********************************************************************************
*/

#include "../incs/SockStream.hpp"

#define READ_SIZE 64

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
SockStream::~SockStream(void) { return; }
SockStream::SockStream(void) { return; }
SockStream::SockStream(SockStream const &src) { *this = src; }
SockStream	&SockStream::operator=(SockStream const &src) {
	_sock = src.getSock();
	_rmsg = src.getRmsg();
	_wmsg = src.getWmsg();
	return *this;
}

// _____________Constructor______________
SockStream::SockStream(int const sock) : _sock(sock) { return; }

// __________Member functions____________
bool SockStream::read(void) throw(SockStream::FailRecv){
	std::stringstream	ss;
	char				tmp_buff[READ_SIZE + 1];
	int					ret;

	std::memset(tmp_buff, 0, READ_SIZE + 1);
	if (!(ret = recv(_sock, tmp_buff, READ_SIZE, 0)))
		return false;
	else if (ret == -1)
		throw(SockStream::FailRecv());
	else
		ss.str(tmp_buff);
	readMessage(ss);
	return true;
}

void SockStream::write(void) throw(SockStream::FailSend) {
	if (!_wmsg.empty())
		writeMessage(_wmsg.front());
}

Message	SockStream::getLastMessage(void) {
	Message ret;

	if (!_rmsg.empty())
	{
		if (_rmsg.front().received())
		{
			ret = Message(_rmsg.front());
			_rmsg.pop();
		}
	}
	return (ret);
}

void SockStream::queueMessage(Message msg) {
	_wmsg.push(msg);
}

bool SockStream::hasInputMessage(void) const { return (!_rmsg.empty() && _rmsg.front().received()); }
bool SockStream::hasOutputMessage(void) const { return (!_wmsg.empty() && _wmsg.front().received()); }

void SockStream::clearIn(void) {
	while (!_rmsg.empty())
		_rmsg.pop();
}

void SockStream::clearOut(void) {
	while (!_wmsg.empty())
		_wmsg.pop();
}

// ____________Setter / Getter___________
// _sock
int					SockStream::getSock(void) const { return _sock; }
void				SockStream::setSock(int const &src) { _sock = src; }

// _rmsg
std::queue<Message>	SockStream::getRmsg(void) const { return _rmsg; }
void				SockStream::setRmsg(std::queue<Message> const &src) { _rmsg = src; }

// _wmsg
std::queue<Message>	SockStream::getWmsg(void) const { return _wmsg; }
void				SockStream::setWmsg(std::queue<Message> const &src) { _wmsg = src; }

// ########################################
// 					PRIVATE
// ########################################

void SockStream::freshMessage(void) { _rmsg.push(Message()); }
void SockStream::readMessage(std::stringstream &input) {
	std::string tmp;

	if (input.str().empty())
		return;
	if (!input.str().compare("\n") && (_rmsg.empty() || _rmsg.back().empty()))
		return;
	else {
		std::getline(input, tmp);
		if (!input.bad()) {
			if (_rmsg.empty())
				freshMessage(); 									// If it is the first msg, init one
			_rmsg.back().load(tmp); 								// Add to latest msg new data
			if (!input.eof() || _rmsg.back().isFull())			// If EOF not reached (we find a '\n')
			{
				_rmsg.back().received(true);						// Mark message as finished
				_rmsg.back().purify();							// Remove '\r' from Windows line return
				if (_rmsg.back().empty())
					_rmsg.back().received(false);
				else
					freshMessage();								// Init next one
				readMessage(input);								// Loop until no more data
			}
		}
	}
}

void SockStream::writeMessage(Message &msg) {
	std::string	tmp;

	if (msg.received())
	{
		tmp = msg.unload(READ_SIZE);
		if (send(_sock, tmp.c_str(), tmp.length(), MSG_NOSIGNAL) == -1)
			throw(SockStream::FailSend());
		if (msg.isWritten())
			_wmsg.pop();
	}
}

// ########################################
// 				 EXECEPTIONS
// ########################################

SockStream::FailSend::~SendFunctionException(void) throw() { return; }
SockStream::FailSend::SendFunctionException(void) { return; }
SockStream::FailSend::SendFunctionException(SockStream::FailSend const &src) : inherited(static_cast<inherited const &>(src)) { return; }
SockStream::FailSend &SockStream::FailSend::operator=(SockStream::FailSend const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	return *this;
}
const char *SockStream::FailSend::what() const throw() {
	return ("Send function failed.");
}

SockStream::FailRecv::~RecvFunctionException(void) throw() { return; }
SockStream::FailRecv::RecvFunctionException(void) { return; }
SockStream::FailRecv::RecvFunctionException(SockStream::FailRecv const &src) : inherited(static_cast<inherited const &>(src)) { return; }
SockStream::FailRecv &SockStream::FailRecv::operator=(SockStream::FailRecv const &src) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(src);
	return *this;
}
const char *SockStream::FailRecv::what() const throw() {
	return ("Recv function failed.");
}

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, SockStream const &src) {
	std::queue<Message> read = src.getRmsg();
	std::queue<Message> write = src.getWmsg();
	size_t nbr = 0;

	flux<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "_sock: " << src.getSock() << std::endl;
	flux << "_rqueue: " << std::endl;
	while (!read.empty()) {
		flux << nbr++ << ": " << read.front();
		read.pop();
	}
	nbr = 0;
	flux << std::endl;
	flux << "_wqueue: " << std::endl;
	while (!write.empty()) {
		flux << nbr++ << ": " << write.front();
		write.pop();
	}
	flux << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	return flux;
}
