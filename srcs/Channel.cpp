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
**----- Updated -------------{ 2022-01-10 07:30:32 }--------------------------**
********************************************************************************
*/

#include "../incs/Channel.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Channel::~Channel(void) { return; }
// Channel::Channel(void) { return; }
Channel::Channel(Channel const &src) { *this = src; }
Channel &Channel::operator=(Channel const &src) {
	_name = src.getName();
	_topic = src.getTopic();
	_creator = src.getCreator();
	_operators = src.getMembers();
	_members = src.getOperators();
	_voices = src.getVoices();
	_invites = src.getInvites();
	_bans = src.getBans();
	_modes = src.getModes();
	return *this;
}

// _____________Constructor______________
Channel::Channel(Client *creator, std::string const &name) :_creator(creator),
															_name(name),
															_modes(CHAN_DEFAULT_MODES) {
	creator->addChanToList(getName());
	creator->send(Command(creator->fullId(), "JOIN", getName()));
	if (_name[0] == '&')
		applyLocalChannel(true);
	return;
}

// __________Member functions____________
unsigned short	Channel::join(Client *joiner) {
	// if (isBanned(joiner->name())) {
	// 	if (isInviteonly()) {
	// 		if (!isInvited(joiner->name()))
	// 			// joiner is banned
	// 		else
	// 			revokeInvite(joiner->name());
	// 	}
	// }
	if (!isOnChan(joiner)) {
		_members.push_back(joiner);
		joiner->addChanToList(_name);
		broadcast(nullptr, JoinCommand(joiner->fullId(), _name));
	}
	else
		return 1; //already on chan
	return 0;
}

void			Channel::leave(Client *leaver) {
	if (!leaver)
		return;
	leaver->removeChanFromList(getName());
	if (!leaver->compare(_creator))
		swapCreator();
	removeMember(leaver, _operators);
	removeMember(leaver, _members);
}

bool			Channel::empty(void) const { return (!_creator); }
bool			Channel::hasTopic(void) { return !(_topic.empty()); }
void			Channel::namesList(std::list<std::string> &lst) const {
	lst.clear();
	lst.push_back("@" + _creator->nickname);
	for (std::list<Client *>::const_iterator it = _operators.begin(); it != _operators.end(); it++)
		lst.push_back("@" + (*it)->nickname);
	for (std::list<Client *>::const_iterator it = _members.begin(); it != _members.end(); it++) {
		if ((*it)->isOperator())
			lst.push_back("@" + (*it)->nickname);
		else if (isModerated() && canTalk((*it)))
			lst.push_back("+" + (*it)->nickname);
		else
			lst.push_back((*it)->nickname);
	}
}

void			Channel::clientsList(std::list<Client *> &lst) const {
	lst.clear();
	lst.push_back(_creator);
	lst.insert(lst.end(), _operators.begin(), _operators.end());
	lst.insert(lst.end(), _members.begin(), _members.end());
}

void			Channel::send(Client *sender, PrivmsgCommand const &cmd) {
	if (sender) {
		if (!isOnChan(sender) && isNoOutMessage())
			throw (Command::InvalidCommandException(ERR_CANNOTSENDTOCHAN));
		else if (isModerated() && !canTalk(sender))
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
	if (isCreator(user) || isMember(user))
		return true;
	else if (std::find(_operators.begin(), _operators.end(), user) != _operators.end())
		return true;
	return false;
}

bool			Channel::isCreator(Client *user) const {
	if (!user)
		return false;
	return (_creator == user);
}

bool			Channel::isOperator(Client *user) const {
	if (!user)
		return false;
	if (isCreator(user))
		return true;
	else if (user->isOperator() || user->isLocalop())
		return true;
	else if (std::find(_operators.begin(), _operators.end(), user) != _operators.end())
		return true;
	return false;
}

bool			Channel::isMember(Client *user) const {
	if (!user)
		return false;
	if (std::find(_members.begin(), _members.end(), user) != _members.end())
		return true;
	return false;
}

bool			Channel::canTalk(Client *user) const {
	if (!isModerated())
		return true;
	if (!user)
		return false;
	if (isOperator(user))
		return true;
	if (std::find(_voices.begin(), _voices.end(), user->fullId()) != _voices.end())
		return true;
	return false;
}

bool			Channel::checkVoice(std::string const &voice) const {
	std::list<std::string>::const_iterator it;

	if ((it = std::find(_voices.begin(), _voices.end(), voice)) != _voices.end())
		return true;
	return false;
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

	if (!isOperator(sender))
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
			addVoice(target);
		else
			removeVoice(target);
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

unsigned int	Channel::size(void) const { return (1 + _operators.size() + _members.size()); }

// ########################################
// 				   PRIVATE
// ########################################

void				Channel::broadcast(Client *sender, Command const &cmd) {
	if (!sender || sender->compare(_creator))
		_creator->send(cmd);
	for (std::list<Client *>::iterator it = _operators.begin(); it != _operators.end(); it++) {
		if (!sender || sender->compare(*it))
			(*it)->send(cmd);
	}
	for (std::list<Client *>::iterator it = _members.begin(); it != _members.end(); it++) {
		if (!sender || sender->compare(*it))
			(*it)->send(cmd);
	}
}

void				Channel::swapCreator(void) {
	if (!_operators.empty()) {
		_creator = _operators.front();
		_operators.pop_front();
	}
	else if (!_members.empty()){
		_creator = _members.front();
		_members.pop_front();
	}
	else
		_creator = nullptr;
}

void				Channel::addVoice(Client *user) {
	if (!user || canTalk(user))
		return;
	_voices.push_back(user->fullId());
}

void				Channel::removeVoice(Client *user) {
	std::list<std::string>::iterator it;

	if (!user || !canTalk(user))
		return;
	if ((it = std::find(_voices.begin(), _voices.end(), user->fullId())) != _voices.end())
		_voices.erase(it);
}

void				Channel::promote(Client *user) {
	if (!user || isOperator(user))
		return;
	removeMember(user, _members);
	_operators.push_back(user);
}

void				Channel::demote(Client *user) {
	if (!user || isCreator(user) || isMember(user))
		return;
	removeMember(user, _operators);
	_members.push_back(user);
}

void				Channel::removeMember(Client *leaver, std::list<Client *> &type) {
	std::list<Client *>::iterator it;

	if ((it = std::find(type.begin(), type.end(), leaver)) != type.end())
		type.erase(it);
}

std::string			Channel::getModeString(char flag, bool set) const { return std::string((set ? "+" : "-") + std::string(1, flag)); }

void				Channel::applyMode(unsigned short mode, bool set) {
	if (!set)
		_modes ^= ((_modes ^ mode) > _modes ? CHAN_NOMODE : mode);
	else
		_modes |= mode;
}

// bool				Channel::isBanned(std::string const &name) { return (std::find(_bans.begin(), _bans.end(), joiner->name()) != _bans.end()); }
// bool				Channel::isInvited(std::string const &name) { return (std::find(_invites.begin(), _invites.end(), joiner->name()) != _invites.end()); }
// void				Channel::revokeInvite(std::string const &name) { _invites.erase(std::find(_invites.begin(), _invites.end(), joiner->name())); } //im not sure

std::ostream		&operator<<(std::ostream &flux, Channel const &src) {
	std::list<std::string> names;

	src.namesList(names);
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
	flux << "Creator: " << *(src.getCreator()) << std::endl;
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	if (!names.empty()) {
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); it++)
			flux << "\t~~~> \t" << (*it) << (src.checkVoice(*it) ? " [v]" : "") << std::endl;
	}
	DEBUG_BAR_DISPC(COUT, '>', 35, DARK_GREY);
	return flux;
}
