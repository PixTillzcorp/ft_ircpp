#ifndef FT_IRC_MESSAGE_HPP
#define FT_IRC_MESSAGE_HPP

#include <iostream>
#include <string>

class Message {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~Message(void);
	Message(void); /* default constructor is private */
	Message(Message const &src);
	Message &operator=(Message const &src);

	// _____________Constructor______________
	Message(std::string const &content);

	// __________Member functions____________
	bool			isWritten(void);

	// ____________Setter / Getter___________
	// _content
	std::string		getContent(void) const;
	size_t			load(std::string const &src);
	std::string		unload(size_t const n);
	bool			compare(std::string const &sample) const;

	// _received
	bool			isReceived(void) const;
	void			received(void);

protected: // ##################################################################
	std::string		_content;
	bool			_received;
};

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, Message const &src);

#endif //FT_IRC_MESSAGE_HPP
