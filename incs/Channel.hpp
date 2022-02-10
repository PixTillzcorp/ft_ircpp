#ifndef FT_IRC_CHANNEL_HPP
#define FT_IRC_CHANNEL_HPP

#include "Client.hpp"

#include <map>

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

#define CHAN_USER_CREATOR	'!'
#define CHAN_USER_OPERATOR	'@'
#define CHAN_USER_VOICE		'+'
#define CHAN_USER_MEMBER	'-'

#define CHAN_DEFAULT_MODES CHAN_TOPIC
#define CHAN_DEFAULT_USERLIMIT 10

class Channel {
public: // #####################################################################
	typedef std::map<Client *, char>	clientmap;
	typedef clientmap::iterator			clientmap_it;
	typedef clientmap::const_iterator	clientmap_cit;
	typedef std::list<std::string>		idlist;
	typedef idlist::iterator			idlist_it;
	typedef idlist::const_iterator		idlist_cit;

	// ____________Canonical Form____________
	virtual ~Channel(void);
	Channel(void);
	Channel(Channel const &src);
	Channel &operator=(Channel const &src);

	// _____________Constructor______________
	Channel(Client *creator, std::string const &name); // JOIN
	Channel(std::string const &name, Client *creator); // NJOIN

	// __________Member functions____________
	unsigned short		join(Client *joiner);
	void				njoin(Client *joiner, char status);
	void				leave(Client *leaver);
	bool				empty(void) const;
	bool				hasTopic(void);

	void				namesList(std::list<std::string> &lst) const;
	void				clientsList(std::list<Client *> &lst) const;

	void				send(Client *sender, PrivmsgCommand const &cmd);
	void				updateClients(Client *sender, Command const &cmd);
	unsigned int		size(void) const;

	Client				*pickMember(char status) const;
	bool				checkStatus(Client *user, char status) const;
	bool				isOnChan(Client *user) const;
	bool				isCreator(Client *user) const;
	bool				isOperator(Client *user) const;
	bool				isVoice(Client *user) const;
	bool				isMember(Client *user) const;
	bool				canTalk(Client *user) const;

	std::string			getModesFlags(void);
	void				applyModeFlag(Client *sender, char flag, bool set);
	bool				applyModeFlag(char flag, bool set);
	void				applyUserModeFlag(Client *sender, Client *target, char flag, bool set);

	bool				isAnonymous(void) const;
	bool				isInviteonly(void) const;
	bool				isModerated(void) const;
	bool				isNoOutMessage(void) const;
	bool				isQuiet(void) const;
	bool				isPrivate(void) const;
	bool				isSecret(void) const;
	bool				isServerReop(void) const;
	bool				isTopic(void) const;
	bool				isKey(void) const;
	bool				isUserLimit(void) const;
	bool				isLocalChannel(void) const;

	void				applyAnonymous(bool set);
	void				applyInviteonly(bool set);
	void				applyModerated(bool set);
	void				applyNoOutMessage(bool set);
	void				applyQuiet(bool set);
	void				applyPrivate(bool set);
	void				applySecret(bool set);
	void				applyServerReop(bool set);
	void				applyTopic(bool set);
	void				applyKey(bool set);
	void				applyUserLimit(bool set);
	void				applyLocalChannel(bool set);

	void				addInvite(std::string const &nick);
	bool				isInvited(std::string const &nick) const;

	// ____________Setter / Getter___________
	// _members
	clientmap const		getMembers(void) const { return this->_members; }
	void				setMembers(clientmap const &src) { this->_members = src; }

	// _name
	std::string	const	getName(void) const { return this->_name; }
	void				setName(std::string const &src) { this->_name = src; }
	bool				checkName(std::string const &cmp) { return !(this->_name.compare(cmp)); }

	// _topic
	std::string	const	getTopic(void) const { return this->_topic; }
	void				setTopic(std::string const &src) { this->_topic = src; }

	// _bans
	idlist const		getBans(void) const { return this->_bans; }
	void				setBans(idlist const &src) { this->_bans = src; }

	// _invites
	idlist const		getInvites(void) const { return this->_invites; }
	void				setInvites(idlist const &src) { this->_invites = src; }

	// _modes
	unsigned short		getModes(void) const { return this->_modes; }
	void				setModes(unsigned short src) { this->_modes = src; }

	// ______________Exceptions______________

protected: // ##################################################################
	clientmap			_members;
	std::string			_name;
	std::string			_topic;
	idlist				_invites;
	idlist				_bans;
	unsigned short		_modes;

	bool				joinedAlready(Client *&joiner) const;
	void				broadcast(Client *sender, Command const &cmd);
	void				swapCreator(void);
	void				removeMember(Client *leaver, std::list<Client *> &type);

	void				giveVoice(Client *user);
	void				takeVoice(Client *user);
	void				promote(Client *user);
	void				demote(Client *user);

	std::string			getModeString(char flag, bool set) const;
	void				applyMode(unsigned short mode, bool set);

	void				revokeInvite(std::string const &nick);
};


std::ostream		&operator<<(std::ostream &flux, Channel const &src);

#endif //FT_IRC_CHANNEL_HPP
