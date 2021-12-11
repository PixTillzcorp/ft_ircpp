#ifndef FT_IRCPP_CLIENT_HPP
#define FT_IRCPP_CLIENT_HPP

#include "../incs/Connection.hpp"

#define CLIENT_MODE_FLAGS "aiwroO"

#define CLIENT_NOMODE		0

#define CLIENT_AWAY			1		// a
#define CLIENT_INVISIBLE	2		// i
#define CLIENT_WALLOPS		4		// w
#define CLIENT_RESTRICTED	8		// r
#define CLIENT_OPERATOR		16		// o
#define CLIENT_LOCALOP		32		// O

class Client : public Connection {
public: // #####################################################################
	typedef Connection inherited;

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

	// __________Member functions____________
	virtual std::string const &name(void) const;
	std::string const	fullId(void) const;
	bool				compare(Client &cmp) const;
	bool				compare(Client *cmp) const;

	bool				isRegistered(void) const;
	bool				isOnChan(std::string const &chan) const;
	bool				hasChans(void) const;

	void				addChanToList(std::string const &chan);
	void				removeChanFromList(std::string const &chan);
	void				applyModeFlag(char flag, bool set);

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

	bool				checkMode(unsigned short mode) const;

private: // ####################################################################
	Client();

	void applyMode(unsigned short mode, bool set);
};

std::ostream		&operator<<(std::ostream &flux, Client const &src);

#endif //FT_IRCPP_SERVER_HPP
