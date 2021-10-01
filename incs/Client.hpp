#ifndef FT_IRCPP_CLIENT_HPP
#define FT_IRCPP_CLIENT_HPP

#include "../incs/Connection.hpp"
#include "../incs/CommandLib.hpp"

#define NOMODE 0

#define CONX_OPERATOR CONX_UNUSED
#define BIT_OPERATOR BIT_UNUSED

class Client : public Connection {
public: // #####################################################################
	// ____________Canonical Form____________
	virtual ~Client(void);
	// Client(void); /* default constructor is private */
	Client(Client const &src);
	Client  &operator=(Client const &src);

	// _____________Constructor______________
	Client(Connection *src, NickCommand const &cmd);
	Client(Connection *src, UserCommand const &cmd);

	// __________Member functions____________
	bool				isRegistered(void) const;
	std::string const	fullId(void) const;

	virtual std::string const name(void) const;

	bool				isOperator(void) const;
	void				setOperator(void);
	void				unsetOperator(void);
	
	// ____________Setter / Getter___________
	// _nickname
	std::string const	getNickname(void) const;
	void				setNickname(std::string const &src);

	// _username
	std::string const	getUsername(void) const;
	void				setUsername(std::string const &src);

	// _realname
	std::string const	getRealname(void) const;
	void				setRealname(std::string const &src);

	// _modes
	bool				checkMode(unsigned char mode) const;
	unsigned char 		getModes(void) const;
	void				setModes(unsigned char src);

private: // ####################################################################

	std::string		_nickname;
	std::string		_username;
	std::string		_realname;
	unsigned char	_modes;

	Client(void);
};

std::ostream		&operator<<(std::ostream &flux, Client const &src);

#endif //FT_IRCPP_SERVER_HPP
