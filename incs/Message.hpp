#ifndef FT_IRC_MESSAGE_HPP
#define FT_IRC_MESSAGE_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>

#define MMAX_LEN 512

class Message {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~Message(void);
	Message(void);
	Message(Message const &cpy);
	Message &operator=(Message const &cpy);

	// _____________Constructor______________
	Message(std::string const &content);

	// __________Member functions____________
	bool			isWritten(void);
	size_t			load(std::string &src);
	std::string		unload(size_t const n);
	bool			compare(std::string const &sample) const;
	bool			empty(void) const;
	void			purify(void);
	bool			isFull(void) const;

	// ____________Setter / Getter___________

	// _received
	bool			received(void) const;
	void			received(bool set);

	std::string		content;

protected: // ##################################################################
	
	static bool		isNotGraph(int c);
	bool			_received;
};

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, Message const &src);

#endif //FT_IRC_MESSAGE_HPP
