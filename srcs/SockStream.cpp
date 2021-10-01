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
**----- Updated -------------{ 2021-09-27 18:12:23 }--------------------------**
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
	this->_sock = src.getSock();
	this->_rmsg = src.getRmsg();
	this->_wmsg = src.getWmsg();
	return *this;
}

// _____________Constructor______________
SockStream::SockStream(int const sock) : _sock(sock) { return; }

// __________Member functions____________
bool SockStream::read(void) {
	std::stringstream	ss;
	char				tmp_buff[READ_SIZE + 1];

	std::memset(tmp_buff, 0, READ_SIZE + 1);
	if (!recv(this->_sock, tmp_buff, READ_SIZE, 0))
		return false;
	else
		ss.str(tmp_buff);
	this->readMessage(ss);
	return true;
}

void SockStream::write(void) throw(SendFunctionException) {
	if (!this->_wmsg.empty())
		this->writeMessage(this->_wmsg.front());
}

Message	*SockStream::getLastMessage(void) {
	Message *ret;

	ret = nullptr;
	if (!this->_rmsg.empty())
	{
		if (this->_rmsg.front().isReceived())
		{
			ret = new Message(this->_rmsg.front());
			this->_rmsg.pop();
		}
	}
	return (ret);
}

void SockStream::queueMessage(Message msg) {
	this->_wmsg.push(msg);
}

bool SockStream::hasInputMessage(void) const { return (!this->_rmsg.empty() && this->_rmsg.front().isReceived()); }
bool SockStream::hasOutputMessage(void) const { return (!this->_wmsg.empty() && this->_wmsg.front().isReceived()); }

void SockStream::clear(void) {
	while (!this->_wmsg.empty())
		this->_wmsg.pop();
}

// ____________Setter / Getter___________
// _sock
int					SockStream::getSock(void) const { return this->_sock; }
void				SockStream::setSock(int const &src) { this->_sock = src; }

// _rmsg
std::queue<Message>	SockStream::getRmsg(void) const { return this->_rmsg; }
void				SockStream::setRmsg(std::queue<Message> const &src) { this->_rmsg = src; }

// _wmsg
std::queue<Message>	SockStream::getWmsg(void) const { return this->_wmsg; }
void				SockStream::setWmsg(std::queue<Message> const &src) { this->_wmsg = src; }

// ########################################
// 					PRIVATE
// ########################################

void SockStream::freshMessage(void) {
	this->_rmsg.push(Message());
}

void SockStream::readMessage(std::stringstream &input) {
	std::string tmp;

	std::getline(input, tmp);
	if (tmp.empty())
		return;
	if (this->_rmsg.empty())
		this->freshMessage(); 			// If it is the first msg, init one
	this->_rmsg.back().load(tmp); 		// Add to latest msg new data
	if (input.eof() == false)			// If EOF not reached (we find a '\n')
	{
		this->_rmsg.back().received();	// Mark message as finished
		this->_rmsg.back().purify();	// Remove '\r' from Win line return
		this->freshMessage();			// Init next one
		this->readMessage(input);		// Loop until no more data
	}
}

void SockStream::writeMessage(Message &msg) {
	std::string	tmp;

	if (msg.isReceived())
	{
		tmp = msg.unload(READ_SIZE);
		if (send(this->_sock, tmp.c_str(), tmp.length(), 0) == -1)
			throw(SockStream::SendFunctionException());
		if (msg.isWritten())
			this->_wmsg.pop();
	}
}

// ########################################
// 				 EXECEPTIONS
// ########################################

SockStream::SendFunctionException::~SendFunctionException(void) throw() { return; }
SockStream::SendFunctionException::SendFunctionException(void) { return; }
SockStream::SendFunctionException::SendFunctionException(SendFunctionException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
SockStream::SendFunctionException &SockStream::SendFunctionException::operator=(SendFunctionException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *SockStream::SendFunctionException::what() const throw() {
	return ("Send function failed.");
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
