#ifndef FT_IRCPP_CLIENT_HPP
#define FT_IRCPP_CLIENT_HPP

#include "Connection.hpp"

#define CLIENT_MODE_FLAGS "aiwroO"

#define CLIENT_NOMODE		0

#define CLIENT_AWAY				1
#define CLIENT_INVISIBLE		2
#define CLIENT_WALLOPS			4
#define CLIENT_RESTRICTED		8
#define CLIENT_OPERATOR			16
#define CLIENT_LOCALOP			32

#define CLIENT_FLAG_AWAY		'a'
#define CLIENT_FLAG_INVISIBLE	'i'
#define CLIENT_FLAG_WALLOPS		'w'
#define CLIENT_FLAG_RESTRICTED	'r'
#define CLIENT_FLAG_OPERATOR	'o'
#define CLIENT_FLAG_LOCALOP		'O'

class Client : public Connection {
public: // #####################################################################
	typedef Connection					inherited;
	typedef std::list<std::string>		chanlist;
	typedef chanlist::iterator			chanlist_it;
	typedef chanlist::const_iterator	chanlist_cit;

	std::string				nickname;
	std::string				username;
	std::string				realname;
	std::string				awaymsg;
	std::list<std::string>	chans;
	unsigned short			modes;

	virtual ~Client(void);
	Client(Client const &cpy);
	Client  &operator=(Client const &cpy);

	// _____________Constructor______________
	Client(Connection *&src, NickCommand const &cmd);
	Client(Connection *&src, UserCommand const &cmd);
	Client(Connection *link, size_t hop);

	// __________Member functions____________
	virtual std::string const name(void) const;
	std::string const	fullId(void) const;
	Command::argvec		nickArgs(std::string const &servertoken) const;
	bool				compare(Connection *cmp) const;
	bool				compare(Client &cmp) const;
	bool				compare(Client *cmp) const;
	bool				checkNickname(std::string const &cmp) const;
	bool				checkUsername(std::string const &cmp) const;
	bool				checkRealname(std::string const &cmp) const;

	bool				validNames(void) const;

	bool				isRegistered(void) const;
	bool				sharedChans(Connection *peer) const;
	bool				isOnChan(std::string const &chan) const;
	bool				hasChans(void) const;

	void				addChanToList(std::string const &chan);
	void				removeChanFromList(std::string const &chan);
	void				applyModeFlag(char flag, bool set);
	void				setModeFlag(char flag);
	void				unsetModeFlag(char flag);
	bool				checkModeFlag(char flag) const;
	std::string	const	getModesFlags(void) const;

	bool				isAway(void) const;
	bool				isInvisible(void) const;
	bool				isWallops(void) const;
	bool				isRestricted(void) const;
	bool				isOperator(void) const;
	bool				isLocalop(void) const;

	void				isAway(bool set);
	void				isInvisible(bool set);
	void				isWallops(bool set);
	void				isRestricted(bool set);
	void				isOperator(bool set);
	void				isLocalop(bool set);

private: // ####################################################################
	Client();

	void applyMode(unsigned short mode, bool set);
};

std::ostream		&operator<<(std::ostream &flux, Client const &src);

#endif //FT_IRCPP_SERVER_HPP
