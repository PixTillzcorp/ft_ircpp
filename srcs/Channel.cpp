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
**----- File ----------------{ Channel.cpp }----------------------------------**
**----- Created -------------{ 2021-10-11 15:03:32 }--------------------------**
**----- Updated -------------{ 2022-02-10 14:23:25 }--------------------------**
********************************************************************************
*/

#include "Channel.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Channel::~Channel(void) { return; }
// Channel::Channel(void) { return; }
Channel::Channel(Channel const &src) { *this = src; }
Channel &Channel::operator=(Channel const &src) {
	_members = src.getMembers();
	_name = src.getName();
	_topic = src.getTopic();
	_invites = src.getInvites();
	_bans = src.getBans();
	_modes = src.getModes();
	return *this;
}

// _____________Constructor______________
Channel::Channel(Client *creator, std::string const &name) :_name(name),
															_modes(CHAN_DEFAULT_MODES) {
	_members[creator] = CHAN_USER_CREATOR;
	creator->addChanToList(_name);
	creator->send(Command(creator->fullId(), "JOIN", getName()));
	if (_name[0] == '&')
		applyLocalChannel(true);
}

Channel::Channel(std::string const &name, Client *creator) : _name(name), _modes(CHAN_DEFAULT_MODES) {
	_members[creator] = CHAN_USER_CREATOR;
	creator->addChanToList(getName());
}

// __________Member functions____________
unsigned short	Channel::join(Client *joiner) {
	if (!isOnChan(joiner)) {
		if (!joiner->isOperator() && isInviteonly()) {
			if (!isInvited(joiner->name()))
				return ERR_INVITEONLYCHAN;
			else
				revokeInvite(joiner->name());
		}
		_members[joiner] = (joiner->isOperator() ? CHAN_USER_OPERATOR : CHAN_USER_MEMBER);
		joiner->addChanToList(_name);
		broadcast(nullptr, JoinCommand(joiner->fullId(), _name));
	}
	else
		return ERR_USERONCHANNEL;
	return 0;
}

void			Channel::njoin(Client *joiner, char status) {
	if (!isOnChan(joiner)) {
		_members[joiner] = status;
		joiner->addChanToList(_name);
	}
}

void			Channel::leave(Client *leaver) {
	if (!leaver)
		return;
	leaver->removeChanFromList(_name);
	_members.erase(leaver);
	if (isCreator(leaver))
		swapCreator();
}

bool			Channel::empty(void) const { return (_members.empty()); }
bool			Channel::hasTopic(void) { return !(_topic.empty()); }
void			Channel::namesList(std::list<std::string> &lst) const {
	Client		*creator;

	lst.clear();
	if (!(creator = pickMember(CHAN_USER_CREATOR)))
		return;
	lst.push_back("@@" + creator->nickname);
	for (clientmap_cit it = _members.begin(); it != _members.end(); it++) {
		if (it->second == CHAN_USER_CREATOR)
			continue;
		else if (it->second == CHAN_USER_OPERATOR || it->first->isOperator())
			lst.push_back("@" + it->first->nickname);
		else if (it->second == CHAN_USER_VOICE)
			lst.push_back("+" + it->first->nickname);
		else
			lst.push_back(it->first->nickname);
	}
}

void			Channel::clientsList(std::list<Client *> &lst) const {
	lst.clear();
	for (clientmap_cit it = _members.begin(); it != _members.end(); it++)
		lst.push_back(it->first);
}

void			Channel::send(Client *sender, PrivmsgCommand const &cmd) {
	if (sender) {
		if (!isOnChan(sender) && isNoOutMessage())
			throw (Command::InvalidCommandException(ERR_CANNOTSENDTOCHAN));
		else if (!canTalk(sender))
			throw (Command::InvalidCommandException(ERR_CANNOTSENDTOCHAN));
	}
	broadcast(sender, cmd);
}

void			Channel::updateClients(Client *sender, Command const &cmd) {
	if (sender && isOnChan(sender)) {
		if (cmd == "NICK")
			broadcast(sender, NickCommand(sender->fullId(), cmd.args));
		else if ((cmd == "QUIT" || cmd == "PART")) {
			broadcast(nullptr, Command(sender->fullId(), cmd.command, cmd.args));
			leave(sender);
		}
	}
}

bool			Channel::isOnChan(Client *user) const {
	if (!user)
		return false;
	return _members.count(user);
}


void			Channel::addInvite(std::string const &nick) {
	if (!isInvited(nick))
		_invites.push_back(nick);
}
bool			Channel::isInvited(std::string const &nick) const { return (std::find(_invites.begin(), _invites.end(), nick) != _invites.end()); }

bool			Channel::checkStatus(Client *user, char status) const {
	if (!isOnChan(user))
		return false;
	return _members.find(user)->second == status;
}

Client			*Channel::pickMember(char status) const {
	for (clientmap_cit it = _members.begin(); it != _members.end(); it++) {
		if (it->second == status)
			return (it->first);
	}
	return nullptr;
}

bool Channel::isCreator(Client *user) const	 { return checkStatus(user, CHAN_USER_CREATOR); }
bool Channel::isOperator(Client *user) const {
	if (isCreator(user) || (user && user->isOperator()))
		return true;
	return checkStatus(user, CHAN_USER_OPERATOR);
}
bool Channel::isVoice(Client *user) const 	 { return checkStatus(user, CHAN_USER_VOICE); }
bool Channel::isMember(Client *user) const	 { return checkStatus(user, CHAN_USER_MEMBER); }
bool Channel::canTalk(Client *user) const {
	if (!user)
		return false;
	else if (!isModerated())
		return true;
	else
		return isVoice(user);
}

bool Channel::isAnonymous(void) const		{ return (this->_modes & CHAN_ANONYMOUS); }
bool Channel::isInviteonly(void) const		{ return (this->_modes & CHAN_INVITEONLY); }
bool Channel::isModerated(void) const		{ return (this->_modes & CHAN_MODERATED); }
bool Channel::isNoOutMessage(void) const	{ return (this->_modes & CHAN_NOOUTMESSAGE); }
bool Channel::isQuiet(void) const			{ return (this->_modes & CHAN_QUIET); }
bool Channel::isPrivate(void) const			{ return (this->_modes & CHAN_PRIVATE); }
bool Channel::isSecret(void) const			{ return (this->_modes & CHAN_SECRET); }
bool Channel::isServerReop(void) const		{ return (this->_modes & CHAN_SERVERREOP); }
bool Channel::isTopic(void) const			{ return (this->_modes & CHAN_TOPIC); }
bool Channel::isKey(void) const				{ return (this->_modes & CHAN_KEY); }
bool Channel::isUserLimit(void) const		{ return (this->_modes & CHAN_USERLIMIT); }
bool Channel::isLocalChannel(void) const	{ return (this->_modes & CHAN_LOCALCHANNEL); }

void Channel::applyAnonymous(bool set)		{ this->applyMode(CHAN_ANONYMOUS, set); }
void Channel::applyInviteonly(bool set)		{ this->applyMode(CHAN_INVITEONLY, set); }
void Channel::applyModerated(bool set)		{ this->applyMode(CHAN_MODERATED, set); }
void Channel::applyNoOutMessage(bool set)	{ this->applyMode(CHAN_NOOUTMESSAGE, set); }
void Channel::applyQuiet(bool set)			{ this->applyMode(CHAN_QUIET, set); }
void Channel::applyPrivate(bool set)		{ this->applyMode(CHAN_PRIVATE, set); }
void Channel::applySecret(bool set)			{ this->applyMode(CHAN_SECRET, set); }
void Channel::applyServerReop(bool set)		{ this->applyMode(CHAN_SERVERREOP, set); }
void Channel::applyTopic(bool set)			{ this->applyMode(CHAN_TOPIC, set); }
void Channel::applyKey(bool set)			{ this->applyMode(CHAN_KEY, set); }
void Channel::applyUserLimit(bool set)		{ this->applyMode(CHAN_USERLIMIT, set); }
void Channel::applyLocalChannel(bool set)	{ this->applyMode(CHAN_LOCALCHANNEL, set); }

void			Channel::applyModeFlag(Client *sender, char flag, bool set) {
	std::string	const flags = CHAN_MODE_FLAGS;

	if (!isCreator(sender) && !isOperator(sender))
		throw (Command::InvalidCommandException(ERR_CHANOPRIVSNEEDED));
	if (flags.find(flag) == std::string::npos)
		throw (Command::InvalidCommandException(ERR_UNKNOWNMODE));
	if (flag == 'a')
		applyAnonymous(set);
	if (flag == 'i')
		applyInviteonly(set);
	if (flag == 'm')
		applyModerated(set);
	if (flag == 'n')
		applyNoOutMessage(set);
	if (flag == 'q')
		applyQuiet(set);
	if (flag == 'p')
		applyPrivate(set);
	if (flag == 's')
		applySecret(set);
	if (flag == 'r')
		applyServerReop(set);
	if (flag == 't')
		applyTopic(set);
	if (flag == 'k')
		applyKey(set);
	if (flag == 'l')
		applyUserLimit(set);
	broadcast(nullptr, ModeCommand(sender->fullId(), _name, getModeString(flag, set)));
}

bool			Channel::applyModeFlag(char flag, bool set) {
	std::string	const flags = CHAN_MODE_FLAGS;

	if (flags.find(flag) == std::string::npos)
		return false;
	if (flag == 'a')
		applyAnonymous(set);
	if (flag == 'i')
		applyInviteonly(set);
	if (flag == 'm')
		applyModerated(set);
	if (flag == 'n')
		applyNoOutMessage(set);
	if (flag == 'q')
		applyQuiet(set);
	if (flag == 'p')
		applyPrivate(set);
	if (flag == 's')
		applySecret(set);
	if (flag == 'r')
		applyServerReop(set);
	if (flag == 't')
		applyTopic(set);
	if (flag == 'k')
		applyKey(set);
	if (flag == 'l')
		applyUserLimit(set);
	return true;
}

void			Channel::applyUserModeFlag(Client *sender, Client *target, char flag, bool set) {
	std::string	const flags = CHAN_UMODE_FLAGS;

	if (!isOperator(sender))
		throw (Command::InvalidCommandException(ERR_CHANOPRIVSNEEDED));
	if (flags.find(flag) == std::string::npos)
		throw (Command::InvalidCommandException(ERR_UNKNOWNMODE));
	if (flag == 'o') {
		if (set)
			promote(target);
		else
			demote(target);
	}
	if (flag == 'v') {
		if (set)
			giveVoice(target);
		else
			takeVoice(target);
	}
}

std::string		Channel::getModesFlags(void) {
	std::ostringstream ret;

	ret << "+";
	ret << (isAnonymous() ? "a" : "");
	ret << (isInviteonly() ? "i" : "");
	ret << (isModerated() ? "m" : "");
	ret << (isNoOutMessage() ? "n" : "");
	ret << (isQuiet() ? "q" : "");
	ret << (isPrivate() ? "p" : "");
	ret << (isSecret() ? "s" : "");
	ret << (isServerReop() ? "r" : "");
	ret << (isTopic() ? "t" : "");
	ret << (isKey() ? "k" : "");
	ret << (isUserLimit() ? "l" : "");

	return ret.str();
}

unsigned int	Channel::size(void) const { return _members.size(); }

// ########################################
// 				   PRIVATE
// ########################################

void			Channel::broadcast(Client *sender, Command const &cmd) {
	for (clientmap_it it = _members.begin(); it != _members.end(); it++) {
		if (!sender || sender->compare(it->first))
			it->first->send(cmd);
	}
}

void			Channel::swapCreator(void) {
	Client			*tmp;

	if ((tmp = pickMember(CHAN_USER_OPERATOR)))
		_members[tmp] = CHAN_USER_CREATOR;
	else if ((tmp = pickMember(CHAN_USER_VOICE)))
		_members[tmp] = CHAN_USER_CREATOR;
	else if ((tmp = pickMember(CHAN_USER_MEMBER)))
		_members[tmp] = CHAN_USER_CREATOR;
	delete this;
}

void			Channel::giveVoice(Client *user) {
	if (!user || !isOnChan(user))
		return;
	if (!isCreator(user) && !isOperator(user))
		_members.find(user)->second = CHAN_USER_VOICE;
}

void			Channel::takeVoice(Client *user) {
	if (!user || !isOnChan(user))
		return;
	if (!isCreator(user) && !isOperator(user))
		_members.find(user)->second = CHAN_USER_MEMBER;
}

void			Channel::promote(Client *user) {
	if (!user || !isOnChan(user))
		return;
	if (!isCreator(user))
		_members.find(user)->second = CHAN_USER_OPERATOR;
}

void			Channel::demote(Client *user) {
	if (!user || !isOnChan(user))
		return;
	if (!isCreator(user))
		_members.find(user)->second = CHAN_USER_MEMBER;
}

std::string		Channel::getModeString(char flag, bool set) const { return std::string((set ? "+" : "-") + std::string(1, flag)); }

void			Channel::applyMode(unsigned short mode, bool set) {
	if (!set)
		_modes ^= ((_modes ^ mode) > _modes ? CHAN_NOMODE : mode);
	else
		_modes |= mode;
}

// bool			Channel::isBanned(std::string const &name) { return (std::find(_bans.begin(), _bans.end(), joiner->name()) != _bans.end()); }
void			Channel::revokeInvite(std::string const &nick) { _invites.erase(std::find(_invites.begin(), _invites.end(), nick)); }

std::ostream		&operator<<(std::ostream &flux, Channel const &src) {
	std::list<Client *> names;

	src.clientsList(names);
	flux << src.getName() << std::endl;
	flux << "MODES [";
	flux << (src.isAnonymous() ? '1' : '0');
	flux << (src.isInviteonly() ? '1' : '0');
	flux << (src.isModerated() ? '1' : '0');
	flux << (src.isNoOutMessage() ? '1' : '0');
	flux << (src.isQuiet() ? '1' : '0');
	flux << (src.isPrivate() ? '1' : '0');
	flux << (src.isSecret() ? '1' : '0');
	flux << (src.isServerReop() ? '1' : '0');
	flux << (src.isTopic() ? '1' : '0');
	flux << (src.isKey() ? '1' : '0');
	flux << (src.isUserLimit() ? '1' : '0');
	flux << (src.isLocalChannel() ? '1' : '0');
	flux << ']' << std::endl;
	flux << "Creator: " << *(src.pickMember(CHAN_USER_CREATOR)) << std::endl;
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	if (!names.empty()) {
		for (std::list<Client *>::const_iterator it = names.begin(); it != names.end(); it++) {
			flux << "~~> " << ((*it)->isAway() ? "zZz\t" : "\t");
			flux << (src.isCreator(*it) ? '!' : (src.isOperator(*it) ? '@' : (src.isVoice(*it) ? '+' : ' ')));
			flux << (*it)->name() << std::endl;
		}
	}
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	return flux;
}
