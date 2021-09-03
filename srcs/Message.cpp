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
**----- File ----------------{ Message.cpp }----------------------------------**
**----- Created -------------{ 2021-05-11 16:27:23 }--------------------------**
**----- Updated -------------{ 2021-08-23 19:49:07 }--------------------------**
********************************************************************************
*/

#include "../incs/Message.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Message::~Message(void) { return; }
Message::Message(void) : _content(""), _received(false) { return; }
Message::Message (Message const &src) {
	*this = src;
	return;
}
Message			&Message::operator=(Message const &src) {
	this->_content = src.getContent();
	this->_received = src.isReceived();
	return *this;
}

// _____________Constructor______________
Message::Message(std::string const &content) : _content(content) { 
	this->received();
	return;
}

// __________Member functions____________
bool			Message::isWritten(void) { return (this->_received && this->_content.empty()); }
size_t			Message::load(std::string const &src) {
	if (!this->_received)
	{
		this->_content.append(src);
		return (src.size());
	}
	return (0);
}
std::string		Message::unload(size_t const n) {
	std::string	tmp;

	if (!this->isWritten() && n)
	{
		tmp = this->_content.substr(0, n);
		this->_content.erase(0, n);
		if (this->isWritten() && tmp.compare("\n")) {
			if (tmp.size() < n)
				tmp.append("\n");
			else
				this->_content.append("\n");
		}
		return (tmp);
	}
	return ("");
}
bool			Message::compare(std::string const &sample) const { return this->_content.compare(sample); }

// ____________Setter / Getter___________
// _content
std::string		Message::getContent(void) const { return this->_content; }

// _received
bool			Message::isReceived(void) const { return this->_received; }
void			Message::received(void) { this->_received = true; }

// ########################################
// 					DEBUG
// ########################################

std::ostream	&operator<<(std::ostream &flux, Message const &src) {
	flux << src.getContent();
	return flux;
}
