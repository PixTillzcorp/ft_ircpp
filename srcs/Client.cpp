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
**----- File ----------------{ Client.cpp }-----------------------------------**
**----- Created -------------{ 2021-06-15 10:22:55 }--------------------------**
**----- Updated -------------{ 2022-01-26 03:07:43 }--------------------------**
********************************************************************************
*/

#include "../incs/Client.hpp"

// ____________Canonical Form____________
Client::~Client(void) { return; }
Client::Client(Client const &cpy) : inherited(static_cast<inherited const &>(cpy)) { return; }
Client  &Client::operator=(Client const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	nickname = cpy.nickname;
	username = cpy.username;
	realname = cpy.realname;
	awaymsg = cpy.awaymsg;
	chans = cpy.chans;
	modes = cpy.modes;
	return *this;
}

// _____________Constructor______________
Client::Client(Connection *&src, NickCommand const &cmd) : inherited(*src), modes(CLIENT_NOMODE) {
	isClient(true);
	nickname = cmd.nickname();
	delete src;
}

Client::Client(Connection *&src, UserCommand const &cmd) : inherited(*src), modes(CLIENT_NOMODE) {
	isClient(true);
	username = cmd.username();
	realname = cmd.realname();
	delete src;
}

Client::Client(Connection *link, size_t hop) : inherited(link, CONX_CLIENT, hop) { return; }

// __________Member functions____________
std::string const	&Client::name(void) const {
	if (isRegistered())
		return nickname;
	else {
		if (nickname.empty())
			return username;
		else
			return nickname;
	}
}

std::string const	Client::fullId(void) const {
	std::ostringstream ss;

	if (!isRegistered())
		return std::string();
	ss << nickname << '!' << username << '@' << hostname();
	return (ss.str());
}

Command::argvec		Client::nickArgs(std::string const &servertoken) const {
	Command::argvec ret;

	ret.push_back(nickname);
	ret.push_back("2");
	ret.push_back(username);
	ret.push_back(hostname());
	ret.push_back(servertoken);
	ret.push_back(getModesFlags());
	ret.push_back(realname);

	return ret;
}

bool				Client::compare(Connection *cmp) const {
	if (!cmp->isClient())
		return true;
	return compare(static_cast<Client *>(cmp));
}

bool				Client::compare(Client &cmp) const {
	if (!nickname.compare(cmp.nickname) &&
		!username.compare(cmp.username) &&
		!realname.compare(cmp.realname)) {
		return false;
	}
	return true;
}

bool				Client::compare(Client *cmp) const {
	if (!cmp)
		return true;
	if (!nickname.compare(cmp->nickname) &&
		!username.compare(cmp->username) &&
		!realname.compare(cmp->realname)) {
		return false;
	}
	return true;
}

bool				Client::validNames(void) const {
	if (!Utils::validNickName(nickname))
		return false;
	if (!Utils::validUserName(username))
		return false;
	if (!Utils::validRealName(realname))
		return false;
	return true;
}

bool				Client::isAway(void) const			{ return (modes & CLIENT_AWAY); }
bool				Client::isInvisible(void) const		{ return (modes & CLIENT_INVISIBLE); }
bool				Client::isWallops(void) const		{ return (modes & CLIENT_WALLOPS); }
bool				Client::isRestricted(void) const	{ return (modes & CLIENT_RESTRICTED); }
bool				Client::isOperator(void) const		{ return (modes & CLIENT_OPERATOR || isLocalop()); }
bool				Client::isLocalop(void) const		{ return (modes & CLIENT_LOCALOP); }

void				Client::isAway(bool set)			{ applyMode(CLIENT_AWAY, set); }
void				Client::isInvisible(bool set)		{ applyMode(CLIENT_INVISIBLE, set); }
void				Client::isWallops(bool set)			{ applyMode(CLIENT_WALLOPS, set); }
void				Client::isRestricted(bool set)		{ applyMode(CLIENT_RESTRICTED, set); }
void				Client::isOperator(bool set)		{ applyMode(CLIENT_OPERATOR, set); }
void				Client::isLocalop(bool set)			{ applyMode(CLIENT_LOCALOP, set); }

bool				Client::checkModeFlag(char flag) const {
	if (flag == CLIENT_FLAG_AWAY)
		return isAway();
	else if (flag == CLIENT_FLAG_INVISIBLE)
		return isInvisible();
	else if (flag == CLIENT_FLAG_WALLOPS)
		return isWallops();
	else if (flag == CLIENT_FLAG_RESTRICTED)
		return isRestricted();
	else if (flag == CLIENT_FLAG_OPERATOR)
		return isOperator();
	else if (flag == CLIENT_FLAG_LOCALOP)
		return isLocalop();
	else
		return false;
}

bool				Client::isRegistered(void) const { return (!nickname.empty() && !username.empty() && !realname.empty()); }
bool				Client::sharedChans(Connection *peer) const {
	if (!peer || !peer->isClient())
		return false;
	if (!compare(static_cast<Client *>(peer)))
		return true;
	for (Client::chanlist_cit it = chans.begin(); it != chans.end(); it++) {
		if (static_cast<Client *>(peer)->isOnChan(*it))
			return true;
	}
	return false;
}
bool				Client::isOnChan(std::string const &chan) const {
	return ((std::find(chans.begin(), chans.end(), chan) != chans.end() ? true : false));
}
bool				Client::hasChans(void) const { return !(chans.empty()); }
void				Client::addChanToList(std::string const &chan) { chans.push_back(chan); }
void				Client::removeChanFromList(std::string const &chan) { chans.remove(chan); }

void				Client::applyModeFlag(char flag, bool set) {
	std::string	const flags = CLIENT_MODE_FLAGS;

	if (flags.find(flag) == std::string::npos)
		throw (Command::InvalidCommand(ERR_UMODEUNKNOWNFLAG));
	else if (flag == CLIENT_FLAG_AWAY || (flag == CLIENT_FLAG_OPERATOR && set))
		throw (Command::InvalidCommand(ERR_DISCARDCOMMAND));
	else if (flag == CLIENT_FLAG_INVISIBLE)
		isInvisible(set);
	else if (flag == CLIENT_FLAG_WALLOPS)
		isWallops(set);
	else if (flag == CLIENT_FLAG_RESTRICTED)
		isRestricted(set);
	else if (flag == CLIENT_FLAG_OPERATOR)
		isOperator(set);
}

void				Client::applyModeFlag(char flag) {
	std::string	const flags = CLIENT_MODE_FLAGS;

	if (flags.find(flag) == std::string::npos)
		return;
	else if (flag == CLIENT_FLAG_AWAY)
		isAway(true);
	else if (flag == CLIENT_FLAG_INVISIBLE)
		isInvisible(true);
	else if (flag == CLIENT_FLAG_WALLOPS)
		isWallops(true);
	else if (flag == CLIENT_FLAG_RESTRICTED)
		isRestricted(true);
	else if (flag == CLIENT_FLAG_OPERATOR)
		isOperator(true);
	else if (flag == CLIENT_FLAG_LOCALOP)
		isLocalop(true);
}

std::string	const	Client::getModesFlags(void) const {
	std::string ret("+");

	if (isAway())
		ret.append(1, CLIENT_FLAG_AWAY);
	if (isInvisible())
		ret.append(1, CLIENT_FLAG_INVISIBLE);
	if (isWallops())
		ret.append(1, CLIENT_FLAG_WALLOPS);
	if (isRestricted())
		ret.append(1, CLIENT_FLAG_RESTRICTED);
	if (isOperator())
		ret.append(1, CLIENT_FLAG_OPERATOR);
	return ret;
}

// ########################################
// 				   PRIVATE
// ########################################

void				Client::applyMode(unsigned short mode, bool set) {
	if (!set)
		modes ^= ((modes ^ mode) > modes ? CLIENT_NOMODE : mode);
	else
		modes |= mode;
}

// ########################################
// 					DEBUG
// ########################################

std::ostream		&operator<<(std::ostream &flux, Client const &src) {
	flux << static_cast<Connection const &>(src);
	flux << "M[";
	flux << (src.isAway() ? "1" : "0");
	flux << (src.isInvisible() ? "1" : "0");
	flux << (src.isWallops() ? "1" : "0");
	flux << (src.isRestricted() ? "1" : "0");
	flux << (src.isOperator() ? "1" : "0");
	flux << (src.isLocalop() ? "1" : "0");
	flux << "]n[" << src.nickname << "]";
	flux << "u[" << src.username << "]";
	flux << "r[" << src.realname << "]";
	if (src.isLink())
		flux << std::endl << "Link [" << src.link->name() << "]";
	if (src.hasChans()) {
		flux << std::endl << "\t\tchans -> ";
		for (Client::chanlist_cit it = src.chans.begin(); it != src.chans.end(); it++)
			flux << (*it) << " ";
	}
	return flux;
}
