#ifndef FT_IRC_CHANNEL_HPP
#define FT_IRC_CHANNEL_HPP

#include "../incs/Client.hpp"

#define CHAN_MODE_FLAGS		"aimnqpsrtkl"
#define CHAN_UMODE_FLAGS	"ov"

#define CHAN_NOMODE			0

#define CHAN_ANONYMOUS		1		// a
#define CHAN_INVITEONLY		2		// i
#define CHAN_MODERATED		4		// m
#define CHAN_NOOUTMESSAGE	8		// n
#define CHAN_QUIET			16		// q
#define CHAN_PRIVATE		32		// p
#define CHAN_SECRET			64		// s
#define CHAN_SERVERREOP		128		// r
#define CHAN_TOPIC			256		// t
#define CHAN_KEY			512		// k
#define CHAN_USERLIMIT		1024	// l
#define CHAN_LOCALCHANNEL	2048	// prefix == '&'

#define CHAN_DEFAULT_MODES CHAN_TOPIC
#define CHAN_DEFAULT_USERLIMIT 10

class Channel {
public: // #####################################################################
	typedef std::list<Client *>			clientlist;
	typedef clientlist::iterator		clientlist_it;
	typedef clientlist::const_iterator	clientlist_cit;
	typedef std::list<std::string>		namelist;
	typedef namelist::iterator			namelist_it;
	typedef namelist::const_iterator	namelist_cit;

	// ____________Canonical Form____________
	virtual ~Channel(void);
	Channel(void);
	Channel(Channel const &src);
	Channel &operator=(Channel const &src);

	// _____________Constructor______________
	Channel(Client *creator, std::string const &name); // JOIN
	Channel(std::string const &name, Client *creator); // NJOIN

	// __________Member functions____________
	unsigned short			join(Client *joiner);
	void					njoin(Client *joiner);
	void					leave(Client *leaver);
	bool					empty(void) const;
	bool					hasTopic(void);

	void					namesList(std::list<std::string> &lst) const;
	void					clientsList(clientlist &lst) const;

	void					send(Client *sender, PrivmsgCommand const &cmd);
	void					updateClients(Client *sender, Command const &cmd);
	unsigned int			size(void) const;

	bool					isOnChan(Client *user) const;
	bool					isCreator(Client *user) const;
	bool					isOperator(Client *user) const;
	bool					isMember(Client *user) const;

	bool					canTalk(Client *user) const;
	bool					checkVoice(std::string const &voice) const;

	std::string				getModesFlags(void);
	void					applyModeFlag(Client *sender, char flag, bool set);
	bool					applyModeFlag(char flag, bool set);
	void					applyUserModeFlag(Client *sender, Client *target, char flag, bool set);

	bool isAnonymous(void) const;
	bool isInviteonly(void) const;
	bool isModerated(void) const;
	bool isNoOutMessage(void) const;
	bool isQuiet(void) const;
	bool isPrivate(void) const;
	bool isSecret(void) const;
	bool isServerReop(void) const;
	bool isTopic(void) const;
	bool isKey(void) const;
	bool isUserLimit(void) const;
	bool isLocalChannel(void) const;

	void applyAnonymous(bool set);
	void applyInviteonly(bool set);
	void applyModerated(bool set);
	void applyNoOutMessage(bool set);
	void applyQuiet(bool set);
	void applyPrivate(bool set);
	void applySecret(bool set);
	void applyServerReop(bool set);
	void applyTopic(bool set);
	void applyKey(bool set);
	void applyUserLimit(bool set);
	void applyLocalChannel(bool set);

	// ____________Setter / Getter___________
	// _name
	std::string	const				getName(void) const { return this->_name; }
	void							setName(std::string const &src) { this->_name = src; }
	bool							checkName(std::string const &cmp) { return !(this->_name.compare(cmp)); }

	// _topic
	std::string	const				getTopic(void) const { return this->_topic; }
	void							setTopic(std::string const &src) { this->_topic = src; }

	// _creator
	Client							*getCreator(void) const { return this->_creator; }
	void							setCreator(Client *src) { this->_creator = src; }

	// _members
	clientlist	const				&getMembers(void) const { return this->_operators; }
	void							setMembers(clientlist const &src) { this->_operators = src; }

	// _operators
	clientlist	const				&getOperators(void) const { return this->_members; }
	void							setOperators(clientlist const &src) { this->_members = src; }

	// _voices
	std::list<std::string>	const	&getVoices(void) const { return this->_voices; }
	void							setVoices(std::list<std::string> const &src) { this->_voices = src; }

	// _invites
	std::list<std::string>	const	&getInvites(void) const { return this->_invites; }
	void							setInvites(std::list<std::string> const &src) { this->_invites = src; }

	// _bans
	std::list<std::string>	const	&getBans(void) const { return this->_bans; }
	void							setBans(std::list<std::string> const &src) { this->_bans = src; }

	// _modes
	unsigned short					getModes(void) const { return this->_modes; }
	void							setModes(unsigned short src) { this->_modes = src; }

	// ______________Exceptions______________

protected: // ##################################################################
	std::string								_name;
	std::string								_topic;
	Client									*_creator;
	clientlist								_operators;
	clientlist								_members;
	std::list<std::string>					_voices;
	std::list<std::string>					_invites;
	std::list<std::string>					_bans;
	unsigned short							_modes;

	bool joinedAlready(Client *&joiner) const;
	void broadcast(Client *sender, Command const &cmd);
	void swapCreator(void);
	void removeMember(Client *leaver, std::list<Client *> &type);

	void addVoice(Client *user);
	void removeVoice(Client *user);
	void promote(Client *user);
	void demote(Client *user);

	std::string getModeString(char flag, bool set) const;
	void applyMode(unsigned short mode, bool set);
};


std::ostream		&operator<<(std::ostream &flux, Channel const &src);

#endif //FT_IRC_CHANNEL_HPP
