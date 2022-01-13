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
**----- File ----------------{ CommandLib.cpp }-------------------------------**
**----- Created -------------{ 2021-06-14 11:07:57 }--------------------------**
**----- Updated -------------{ 2022-01-05 18:29:42 }--------------------------**
********************************************************************************
*/

#include "CommandLib.hpp"

// ########################################
// 					UNIQUE
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PassCommand::~PassCommand(void) { return; }
// PassCommand::PassCommand(void) { return; }
PassCommand::PassCommand(PassCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
PassCommand &PassCommand::operator=(PassCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
PassCommand::PassCommand(Command const &src) : inherited(src) { return; }
PassCommand::PassCommand(std::string const &pass) : inherited(NO_PREFIX, "PASS", pass) { return; }

// __________Member functions____________
void		PassCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	if (!prefix.empty())
		throw (inherited::InvalidCommandException(ERR_DISCARDCOMMAND));
	if (!argNbr(1))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		PassCommand::password(void) const { return (argX(0)); }
std::string		PassCommand::version(void) const { return (argX(1)); }
std::string		PassCommand::implem(void) const { return (argX(2).substr(0, findIn(2, "|"))); }
std::string		PassCommand::author(void) const {
	size_t			spot = findIn(2, "|");

	if (spot == argX(2).size())
		return (std::string());
	else
		return (argX(2).substr(spot + 1));
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
ErrorCommand::~ErrorCommand(void) { return; }
// ErrorCommand::ErrorCommand(void) { return; }
ErrorCommand::ErrorCommand(ErrorCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
ErrorCommand &ErrorCommand::operator=(ErrorCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
ErrorCommand::ErrorCommand(Command const &src) : inherited(src) { return; }
ErrorCommand::ErrorCommand(std::string const &msg) : inherited(NO_PREFIX, "ERROR", msg) { return; }

// __________Member functions____________
void		ErrorCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!prefix.empty())
		throw (inherited::InvalidCommandException(ERR_DISCARDCOMMAND));
	if (!argNbr(1))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		ErrorCommand::message(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					CLIENT
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
NickCommand::~NickCommand(void) { return; }
// NickCommand::NickCommand(void) { return; }
NickCommand::NickCommand(NickCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
NickCommand &NickCommand::operator=(NickCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
NickCommand::NickCommand(Command const &src) : inherited(src) { return; }
NickCommand::NickCommand(std::string const &prefix, std::string const &nickname) : inherited(prefix, "NICK", nickname) { return; }
NickCommand::NickCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "NICK", args) { return; }

// __________Member functions____________
void		NickCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr(1))
		throw (inherited::InvalidCommandException(ERR_NONICKNAMEGIVEN));
}

std::string		NickCommand::nickname(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
UserCommand::~UserCommand(void) { return; }
// UserCommand::UserCommand(void) { return; }
UserCommand::UserCommand(UserCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
UserCommand &UserCommand::operator=(UserCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
UserCommand::UserCommand(Command const &src) : inherited(src) { return; }
UserCommand::UserCommand(inherited::arglist const &args) : inherited(NO_PREFIX, "USER", args) { return; }
UserCommand::UserCommand(std::string const &username, std::string const &modes, std::string const &realname) :
	Command(NO_PREFIX, "USER", inherited::arglist()) {
	addArg(username);
	addArg(modes);
	addArg("*");
	addArg(realname);
}

// __________Member functions____________
void		UserCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr(4))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		UserCommand::username(void) const { return (argX(0)); }
std::string		UserCommand::modes(void) const { return (argX(1)); }
std::string		UserCommand::unused(void) const { return (argX(2)); }
std::string		UserCommand::realname(void) const { return (argX(3)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
QuitCommand::~QuitCommand(void) { return; }
// QuitCommand::QuitCommand(void) { return; }
QuitCommand::QuitCommand(QuitCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
QuitCommand &QuitCommand::operator=(QuitCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
QuitCommand::QuitCommand(Command const &src) : inherited(src) { return; }
QuitCommand::QuitCommand(std::string const &prefix, std::string const &msg) : inherited(prefix, "QUIT", msg) { return; }
QuitCommand::QuitCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "QUIT", args) { return; }

// __________Member functions____________
void		QuitCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		QuitCommand::message(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PrivmsgCommand::~PrivmsgCommand(void) { return; }
// PrivmsgCommand::PrivmsgCommand(void) { return; }
PrivmsgCommand::PrivmsgCommand(PrivmsgCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
PrivmsgCommand &PrivmsgCommand::operator=(PrivmsgCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
PrivmsgCommand::PrivmsgCommand(Command const &src) : inherited(src) { return; }
PrivmsgCommand::PrivmsgCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "PRIVMSG", args) { return; }
PrivmsgCommand::PrivmsgCommand(std::string const &prefix, std::string const &target, std::string const &msg) :
	inherited(prefix, "PRIVMSG", inherited::arglist()) {
	addArg(target);
	addArg(msg);
}

// __________Member functions____________
void		PrivmsgCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommandException(ERR_NORECIPIENT));
	if (argNbr() < 2)
		throw (inherited::InvalidCommandException(ERR_NOTEXTTOSEND));
	if (argNbr(2) && message().empty())
		throw (inherited::InvalidCommandException(ERR_NOTEXTTOSEND));
}

std::string		PrivmsgCommand::target(void) const { return (argX(0)); }
std::string		PrivmsgCommand::message(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
NoticeCommand::~NoticeCommand(void) { return; }
// NoticeCommand::NoticeCommand(void) { return; }
NoticeCommand::NoticeCommand(NoticeCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
NoticeCommand &NoticeCommand::operator=(NoticeCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
NoticeCommand::NoticeCommand(Command const &src) : inherited(src) { return; }
NoticeCommand::NoticeCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "NOTICE", args) { return; }
NoticeCommand::NoticeCommand(std::string const &prefix, std::string const &target, std::string const &msg) :
	inherited(prefix, "NOTICE", inherited::arglist()) {
	addArg(target);
	addArg(msg);
}

// __________Member functions____________
void		NoticeCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommandException(ERR_DISCARDCOMMAND));
	if (argNbr() < 2)
		throw (inherited::InvalidCommandException(ERR_DISCARDCOMMAND));
	if (argNbr(2) && message().empty())
		throw (inherited::InvalidCommandException(ERR_DISCARDCOMMAND));
}

std::string		NoticeCommand::target(void) const { return (argX(0)); }
std::string		NoticeCommand::message(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PingCommand::~PingCommand(void) { return; }
// PingCommand::PingCommand(void) { return; }
PingCommand::PingCommand(PingCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
PingCommand &PingCommand::operator=(PingCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
PingCommand::PingCommand(Command const &src) : inherited(src) { return; }
PingCommand::PingCommand(std::string const &sender) : inherited(NO_PREFIX, "PING", sender) { return; }
PingCommand::PingCommand(std::string const &sender, std::string const &target) :
	inherited(NO_PREFIX, "PING", inherited::arglist()){
	addArg(sender);
	addArg(target);
	static_cast<Command &>(*this) = Command(NO_PREFIX, "PING", args);
}

// __________Member functions____________
void		PingCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommandException(ERR_NOORIGIN));
}

std::string		PingCommand::sender(void) const { return (argX(0)); }
std::string		PingCommand::target(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PongCommand::~PongCommand(void) { return; }
// PongCommand::PongCommand(void) { return; }
PongCommand::PongCommand(PongCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
PongCommand &PongCommand::operator=(PongCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
PongCommand::PongCommand(Command const &src) : inherited(src) { return; }
PongCommand::PongCommand(std::string const &sender) : inherited(NO_PREFIX, "PONG", sender) { return; }
PongCommand::PongCommand(std::string const &sender, std::string const &target) :
	inherited(NO_PREFIX, "PONG", inherited::arglist()) {
	addArg(sender);
	addArg(target);
}

// __________Member functions____________
void		PongCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommandException(ERR_NOORIGIN));
}

std::string		PongCommand::sender(void) const { return (argX(0)); }
std::string		PongCommand::target(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
MotdCommand::~MotdCommand(void) { return; }
// MotdCommand::MotdCommand(void) { return; }
MotdCommand::MotdCommand(MotdCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
MotdCommand &MotdCommand::operator=(MotdCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
MotdCommand::MotdCommand(Command const &src) : inherited(src) { return; }
MotdCommand::MotdCommand(std::string const &target) : inherited(NO_PREFIX, "MOTD", target) { return; }

// __________Member functions____________
void		MotdCommand::isValid(void) const throw(inherited::InvalidCommandException) { inherited::isValid(); }

std::string		MotdCommand::target(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
LusersCommand::~LusersCommand(void) { return; }
// LusersCommand::LusersCommand(void) { return; }
LusersCommand::LusersCommand(LusersCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
LusersCommand &LusersCommand::operator=(LusersCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
LusersCommand::LusersCommand(Command const &src) : inherited(src) { return; }
LusersCommand::LusersCommand(std::string const &target) : inherited(NO_PREFIX, "LUSERS", target) { return; }

// __________Member functions____________
void		LusersCommand::isValid(void) const throw(inherited::InvalidCommandException) { inherited::isValid(); }

std::string		LusersCommand::target(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
WhoisCommand::~WhoisCommand(void) { return; }
// WhoisCommand::WhoisCommand(void) { return; }
WhoisCommand::WhoisCommand(WhoisCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
WhoisCommand &WhoisCommand::operator=(WhoisCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
WhoisCommand::WhoisCommand(Command const &src) : inherited(src) { return; }
WhoisCommand::WhoisCommand(std::string const &target) : inherited(NO_PREFIX, "WHOIS", target) { return; }

// __________Member functions____________
void		WhoisCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr(0))
		throw (inherited::InvalidCommandException(ERR_DISCARDCOMMAND));
}

std::string		WhoisCommand::target(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
JoinCommand::~JoinCommand(void) { return; }
// JoinCommand::JoinCommand(void) { return; }
JoinCommand::JoinCommand(JoinCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
JoinCommand &JoinCommand::operator=(JoinCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
JoinCommand::JoinCommand(Command const &src) : inherited(src) { return; }
JoinCommand::JoinCommand(std::string const &prefix, std::string const &target) : inherited(prefix, "JOIN", target) { return; }

// __________Member functions____________
void		JoinCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr(1))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		JoinCommand::target(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PartCommand::~PartCommand(void) { return; }
// PartCommand::PartCommand(void) { return; }
PartCommand::PartCommand(PartCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
PartCommand &PartCommand::operator=(PartCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
PartCommand::PartCommand(Command const &src) : inherited(src) { return; }
PartCommand::PartCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "PART", args) { return; }
PartCommand::PartCommand(std::string const &prefix, std::string const &target, std::string const &msg) :
	inherited(prefix, "PART", inherited::arglist()){
	addArg(target);
	addArg(msg);
}

// __________Member functions____________
void		PartCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr(0))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		PartCommand::target(void) const { return (argX(0)); }
std::string		PartCommand::message(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
TopicCommand::~TopicCommand(void) { return; }
// TopicCommand::TopicCommand(void) { return; }
TopicCommand::TopicCommand(TopicCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
TopicCommand &TopicCommand::operator=(TopicCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
TopicCommand::TopicCommand(Command const &src) : inherited(src) { return; }
TopicCommand::TopicCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "TOPIC", args) { return; }
TopicCommand::TopicCommand(std::string const &prefix, std::string const &target, std::string const &topic) :
	inherited(prefix, "TOPIC", inherited::arglist()){
	addArg(target);
	addArg(topic);
}

// __________Member functions____________
void		TopicCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr(0))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		TopicCommand::target(void) const { return (argX(0)); }
std::string		TopicCommand::topic(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
OperCommand::~OperCommand(void) { return; }
// OperCommand::OperCommand(void) { return; }
OperCommand::OperCommand(OperCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
OperCommand &OperCommand::operator=(OperCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
OperCommand::OperCommand(Command const &src) : inherited(src) { return; }
OperCommand::OperCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "OPER", args) { return; }
OperCommand::OperCommand(std::string const &prefix, std::string const &name, std::string const &password) :
	inherited(prefix, "OPER", inherited::arglist()) {
	addArg(name);
	addArg(password);
}

// __________Member functions____________
void		OperCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr(2))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		OperCommand::name(void) const { return (argX(0)); }
std::string		OperCommand::password(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
OppassCommand::~OppassCommand(void) { return; }
// OppassCommand::OppassCommand(void) { return; }
OppassCommand::OppassCommand(OppassCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
OppassCommand &OppassCommand::operator=(OppassCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
OppassCommand::OppassCommand(Command const &src) : inherited(src) { return; }
OppassCommand::OppassCommand(std::string const &prefix, std::string const &password) : inherited(prefix, "OPPASS", password) { return; }

// __________Member functions____________
void		OppassCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!argNbr(1))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		OppassCommand::password(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
ModeCommand::~ModeCommand(void) { return; }
// ModeCommand::ModeCommand(void) { return; }
ModeCommand::ModeCommand(ModeCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
ModeCommand &ModeCommand::operator=(ModeCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
ModeCommand::ModeCommand(Command const &src) : inherited(src) { return; }
ModeCommand::ModeCommand(std::string const &prefix, inherited::arglist const &args) : inherited(prefix, "MODE", args) { return; }
ModeCommand::ModeCommand(std::string const &prefix, std::string const &chan) : inherited(prefix, "MODE", chan) { return; }
ModeCommand::ModeCommand(std::string const &prefix, std::string const &target, std::string const &mode) :
	inherited(prefix, "MODE", inherited::arglist()) {
	addArg(target);
	addArg(mode);
}
ModeCommand::ModeCommand(std::string const &prefix, std::string const &chan, std::string const &mode, std::string const &nick) :
	inherited(prefix, "MODE", inherited::arglist()) {
	addArg(chan);
	addArg(mode);
	addArg(nick);
}

// __________Member functions____________
void		ModeCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (!this->argNbr(1) && !this->argNbr(2) && !this->argNbr(3))
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		ModeCommand::field1(void) const { return (argX(0)); }
std::string		ModeCommand::field2(void) const { return (argX(1)); }
std::string		ModeCommand::field3(void) const { return (argX(2)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
AwayCommand::~AwayCommand(void) { return; }
// AwayCommand::AwayCommand(void) { return; }
AwayCommand::AwayCommand(AwayCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
AwayCommand &AwayCommand::operator=(AwayCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
AwayCommand::AwayCommand(Command const &src) : inherited(src) { return; }
AwayCommand::AwayCommand(std::string const &prefix, std::string const &message) : inherited(prefix, "AWAY", message) { return; }

// __________Member functions____________
void		AwayCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		AwayCommand::message(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
NamesCommand::~NamesCommand(void) { return; }
// NamesCommand::NamesCommand(void) { return; }
NamesCommand::NamesCommand(NamesCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
NamesCommand &NamesCommand::operator=(NamesCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
NamesCommand::NamesCommand(Command const &src) : inherited(src) { return; }
NamesCommand::NamesCommand(std::string const &prefix, std::string const &target) : inherited(prefix, "NAMES", target) { return; }

// __________Member functions____________
void		NamesCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		NamesCommand::target(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
ListCommand::~ListCommand(void) { return; }
// ListCommand::ListCommand(void) { return; }
ListCommand::ListCommand(ListCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
ListCommand &ListCommand::operator=(ListCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
ListCommand::ListCommand(Command const &src) : inherited(src) { return; }
ListCommand::ListCommand(std::string const &prefix, std::string const &target) : inherited(prefix, "LIST", target) { return; }

// __________Member functions____________
void		ListCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		ListCommand::target(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
WhoCommand::~WhoCommand(void) { return; }
// WhoCommand::WhoCommand(void) { return; }
WhoCommand::WhoCommand(WhoCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
WhoCommand &WhoCommand::operator=(WhoCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
WhoCommand::WhoCommand(Command const &src) : inherited(src) { return; }
WhoCommand::WhoCommand(std::string const &prefix, std::string const &mask) : inherited(prefix, "WHO", mask) { return; }

// __________Member functions____________
void		WhoCommand::isValid(void) const throw(inherited::InvalidCommandException) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		WhoCommand::mask(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					SERVER
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
ServerCommand::~ServerCommand(void) { return; }
// ServerCommand::ServerCommand(void) { return; }
ServerCommand::ServerCommand(ServerCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
ServerCommand &ServerCommand::operator=(ServerCommand const &cpy) {
	static_cast<Command &>(*this) = static_cast<Command const &>(cpy);
	return *this;
}

// _____________Constructor______________
ServerCommand::ServerCommand(Command const &src) : inherited(src) { return; }
ServerCommand::ServerCommand(std::string const prefix, inherited::arglist const &args) :
	inherited(prefix, "SERVER", args) { return; }
ServerCommand::ServerCommand(inherited::arglist const &args) :
	inherited(NO_PREFIX, "SERVER", args) { return; }
// ServerCommand::ServerCommand(std::string const prefix, ServerCommand const &cmd) :
// 	inherited(prefix, "SERVER", cmd.getArgs()) { return; } //hopcount + 1

void		ServerCommand::isValid(void) const throw(InvalidCommandException) {
	inherited::isValid();
}

std::string			ServerCommand::servername(void)		const { return (argX(0)); }
std::string			ServerCommand::hopcount(void)		const { return (argX(1)); }
std::string			ServerCommand::token(void)			const { return (argX(2)); }
std::string			ServerCommand::info(void)			const { return (argX(3)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// // ____________Canonical Form____________
// NickServerCommand::~NickServerCommand(void) { return; }
// // NickServerCommand::NickServerCommand(void) { return; }
// NickServerCommand::NickServerCommand(NickServerCommand const &src) { *this = src; }
// NickServerCommand &NickServerCommand::operator=(NickServerCommand const &src) {
// 	static_cast<Command &>(*this) = static_cast<Command const &>(src);
// 	nickname = src.nickname;
// 	hopcount = src.hopcount;
// 	username = src.username;
// 	host = src.host;
// 	token = src.token;
// 	usermode = src.usermode;
// 	realname = src.realname;
// 	return *this;
// }

// // _____________Constructor______________
// NickServerCommand::NickServerCommand(std::string const prefix, std::string const &nick) : inherited(prefix, "NICK", std::list<std::string>(1, nick)){ return; }
// NickServerCommand::NickServerCommand(inherited::arglist const &args) : inherited("", "NICK", args) { return; }
// NickServerCommand::NickServerCommand(Command const &src) : inherited(src) {
// 	nickname = argX(0);
// 	if (src.argNbr() == 7)
// 	{
// 		hopcount = std::stoi(argX(1));
// 		username = argX(2);
// 		host = argX(3);
// 		token = argX(4);
// 		usermode = argX(5);
// 		realname = argX(6);
// 	}
// 	return;
// }

// bool		NickServerCommand::isValid(void) const {
// 	if (prefix.find_first_not_of(AUL_DDD) != std::string::npos)
// 		return false;
// 	if (command.empty() || command.compare("NICK"))
// 		return false;
// 	if (argNbr() != 1 && argNbr() != 7)
// 		return false;
// 	return true;
// }

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// // ____________Canonical Form____________
// SquitCommand::~SquitCommand(void) { return; }
// // SquitCommand::SquitCommand(void) { return; }
// SquitCommand::SquitCommand(SquitCommand const &src) { *this = src; }
// SquitCommand &SquitCommand::operator=(SquitCommand const &src) {
// 	static_cast<Command &>(*this) = static_cast<Command const &>(src);
// 	target = src.target;
// 	return *this;
// }

// // _____________Constructor______________
// SquitCommand::SquitCommand(std::string const prefix, std::string const &target) : inherited(prefix, "SQUIT", std::list<std::string>(1, target)) { return; }
// SquitCommand::SquitCommand(Command const &src) : inherited(src) {
// 	target = argX(0);
// 	return;
// }

// bool		SquitCommand::isValid(void) const {
// 	if (prefix.find_first_not_of(AUL_DDD) != std::string::npos)
// 		return false;
// 	if (command.empty() || command.compare("SQUIT"))
// 		return false;
// 	if (argNbr() != 2)
// 		return false;
// 	return true;
// }

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
