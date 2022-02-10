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
**----- Updated -------------{ 2022-02-03 21:17:58 }--------------------------**
********************************************************************************
*/

#include "../incs/Message.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Message::~Message(void) { return; }
Message::Message(void) : content(""), _received(false) { return; }
Message::Message(Message const &cpy) :
	content(cpy.content), _received(cpy.received()) { return; }
Message			&Message::operator=(Message const &cpy) {
	content = cpy.content;
	_received = cpy.received();
	return *this;
}

// _____________Constructor______________
Message::Message(std::string const &content) : content(content) { 
	received(true);
	return;
}

// __________Member functions____________
bool			Message::isWritten(void) { return (_received && content.empty()); }

size_t			Message::load(std::string &src) {
	if (!src.empty() && !_received && !isFull())
	{
		src.erase(std::remove_if(src.begin(), src.end(), isNotGraph), src.end());
		if (content.size() + src.size() >= MMAX_LEN)
			content.append(src.substr(0, MMAX_LEN - content.size()));
		else
			content.append(src);
		return (src.size());
	}
	return (0);
}
std::string		Message::unload(size_t const n) {
	std::string	tmp;

	if (!isWritten() && n)
	{
		tmp = content.substr(0, n);
		content.erase(0, n);
		if (isWritten() && tmp.compare("\n")) {
			if (tmp.size() < n)
				tmp.append("\n");
			else
				content.append("\n");
		}
		return (tmp);
	}
	return ("");
}
bool			Message::compare(std::string const &sample) const { return content.compare(sample); }
bool			Message::empty(void) const { return content.empty(); }
void			Message::purify(void) {
	if (received()) {
		content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
		Utils::clearSpaces(content, false);
	}
}
bool			Message::isFull(void) const { return (content.size() >= MMAX_LEN); }

// ____________Setter / Getter___________
bool			Message::received(void) const { return _received; }
void			Message::received(bool set) { _received = set; }

// ########################################
// 				   PRIVATE
// ########################################

bool			Message::isNotGraph(int c) { return (c == 32 ? false : !isgraph(c)); }

// ########################################
// 					DEBUG
// ########################################

std::ostream	&operator<<(std::ostream &flux, Message const &src) {
	flux << src.content;
	return flux;
}
