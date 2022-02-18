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
**----- Updated -------------{ 2022-02-15 22:10:11 }--------------------------**
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
PassCommand::PassCommand(inherited const &src) : inherited(src) { return; }
PassCommand::PassCommand(std::string const &pass) : inherited(NO_PREFIX, "PASS", pass) { return; }

// __________Member functions____________
void		PassCommand::isValid(void) const throw(inherited::InvalidCommand) {
	if (!prefix.empty())
		throw (inherited::InvalidCommand(ERR_DISCARDCOMMAND));
	if (!argNbr(1))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
ErrorCommand::ErrorCommand(inherited const &src) : inherited(src) { return; }
ErrorCommand::ErrorCommand(std::string const &msg) : inherited(NO_PREFIX, "ERROR", msg) { return; }

// __________Member functions____________
void		ErrorCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!prefix.empty())
		throw (inherited::InvalidCommand(ERR_DISCARDCOMMAND));
	if (!argNbr(1))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
NickCommand::NickCommand(inherited const &src) : inherited(src) { return; }
NickCommand::NickCommand(std::string const &prefix, std::string const &nickname) : inherited(prefix, "NICK", nickname) { return; }
NickCommand::NickCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "NICK", args) { return; }

// __________Member functions____________
void		NickCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(1) && !argNbr(7))
		throw (inherited::InvalidCommand(ERR_NONICKNAMEGIVEN));
}

std::string		NickCommand::nickname(void) const { return (argX(0)); }
std::string		NickCommand::hopcount(void) const { return (argX(1)); }
std::string		NickCommand::username(void) const { return (argX(2)); }
std::string		NickCommand::host(void) const {
	std::string tmp;

	tmp = argX(3);
	if (!tmp.rfind("0:", 0))
		return (tmp.substr(1));
	return (tmp);
}
std::string		NickCommand::servertoken(void) const { return (argX(4)); }
std::string		NickCommand::umode(void) const { return (argX(5)); }
std::string		NickCommand::realname(void) const { return (argX(6)); }

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
UserCommand::UserCommand(inherited const &src) : inherited(src) { return; }
UserCommand::UserCommand(inherited::argvec const &args) : inherited(NO_PREFIX, "USER", args) { return; }
UserCommand::UserCommand(std::string const &username, std::string const &modes, std::string const &realname) :
	Command(NO_PREFIX, "USER", inherited::argvec()) {
	addArg(username);
	addArg(modes);
	addArg("*");
	addArg(realname);
}

// __________Member functions____________
void		UserCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(4))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
QuitCommand::QuitCommand(inherited const &src) : inherited(src) { return; }
QuitCommand::QuitCommand(std::string const &prefix) : inherited(prefix, "QUIT", inherited::argvec()) { return; }
QuitCommand::QuitCommand(std::string const &prefix, std::string const &msg) : inherited(prefix, "QUIT", msg) { return; }
QuitCommand::QuitCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "QUIT", args) { return; }

// __________Member functions____________
void		QuitCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
PrivmsgCommand::PrivmsgCommand(inherited const &src) : inherited(src) { return; }
PrivmsgCommand::PrivmsgCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "PRIVMSG", args) { return; }
PrivmsgCommand::PrivmsgCommand(std::string const &prefix, std::string const &target, std::string const &msg) :
	inherited(prefix, "PRIVMSG", inherited::argvec()) {
	addArg(target);
	addArg(msg);
}

// __________Member functions____________
void		PrivmsgCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommand(ERR_NORECIPIENT));
	if (argNbr() < 2)
		throw (inherited::InvalidCommand(ERR_NOTEXTTOSEND));
	if (argNbr(2) && message().empty())
		throw (inherited::InvalidCommand(ERR_NOTEXTTOSEND));
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
NoticeCommand::NoticeCommand(inherited const &src) : inherited(src) { return; }
NoticeCommand::NoticeCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "NOTICE", args) { return; }
NoticeCommand::NoticeCommand(std::string const &prefix, std::string const &target, std::string const &msg) :
	inherited(prefix, "NOTICE", inherited::argvec()) {
	addArg(target);
	addArg(msg);
}

// __________Member functions____________
void		NoticeCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommand(ERR_DISCARDCOMMAND));
	if (argNbr() < 2)
		throw (inherited::InvalidCommand(ERR_DISCARDCOMMAND));
	if (argNbr(2) && message().empty())
		throw (inherited::InvalidCommand(ERR_DISCARDCOMMAND));
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
PingCommand::PingCommand(inherited const &src) : inherited(src) { return; }
PingCommand::PingCommand(std::string const &sender) : inherited(NO_PREFIX, "PING", sender) { return; }
PingCommand::PingCommand(std::string const &sender, std::string const &target) :
	inherited(NO_PREFIX, "PING", inherited::argvec()){
	addArg(sender);
	addArg(target);
	static_cast<Command &>(*this) = Command(NO_PREFIX, "PING", args);
}

// __________Member functions____________
void		PingCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommand(ERR_NOORIGIN));
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
PongCommand::PongCommand(inherited const &src) : inherited(src) { return; }
PongCommand::PongCommand(std::string const &sender) : inherited(NO_PREFIX, "PONG", sender) { return; }
PongCommand::PongCommand(std::string const &sender, std::string const &target) :
	inherited(NO_PREFIX, "PONG", inherited::argvec()) {
	addArg(sender);
	addArg(target);
}

// __________Member functions____________
void		PongCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr())
		throw (inherited::InvalidCommand(ERR_NOORIGIN));
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
MotdCommand::MotdCommand(inherited const &src) : inherited(src) { return; }
MotdCommand::MotdCommand(std::string const &target) : inherited(NO_PREFIX, "MOTD", target) { return; }

// __________Member functions____________
void		MotdCommand::isValid(void) const throw(inherited::InvalidCommand) { inherited::isValid(); }

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
LusersCommand::LusersCommand(inherited const &src) : inherited(src) { return; }
LusersCommand::LusersCommand(std::string const &target) : inherited(NO_PREFIX, "LUSERS", target) { return; }

// __________Member functions____________
void		LusersCommand::isValid(void) const throw(inherited::InvalidCommand) { inherited::isValid(); }

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
WhoisCommand::WhoisCommand(inherited const &src) : inherited(src) { return; }
WhoisCommand::WhoisCommand(std::string const &target) : inherited(NO_PREFIX, "WHOIS", target) { return; }

// __________Member functions____________
void		WhoisCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr(0))
		throw (inherited::InvalidCommand(ERR_DISCARDCOMMAND));
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
JoinCommand::JoinCommand(inherited const &src) : inherited(src) { return; }
JoinCommand::JoinCommand(std::string const &prefix, std::string const &target) : inherited(prefix, "JOIN", target) { return; }

// __________Member functions____________
void		JoinCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(1))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
PartCommand::PartCommand(inherited const &src) : inherited(src) { return; }
PartCommand::PartCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "PART", args) { return; }
PartCommand::PartCommand(std::string const &prefix, std::string const &target, std::string const &msg) :
	inherited(prefix, "PART", inherited::argvec()){
	addArg(target);
	addArg(msg);
}

// __________Member functions____________
void		PartCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr(0))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
TopicCommand::TopicCommand(inherited const &src) : inherited(src) { return; }
TopicCommand::TopicCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "TOPIC", args) { return; }
TopicCommand::TopicCommand(std::string const &prefix, std::string const &target, std::string const &topic) :
	inherited(prefix, "TOPIC", inherited::argvec()){
	addArg(target);
	addArg(topic);
}

// __________Member functions____________
void		TopicCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr(0))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
OperCommand::OperCommand(inherited const &src) : inherited(src) { return; }
OperCommand::OperCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "OPER", args) { return; }
OperCommand::OperCommand(std::string const &prefix, std::string const &name, std::string const &password) :
	inherited(prefix, "OPER", inherited::argvec()) {
	addArg(name);
	addArg(password);
}

// __________Member functions____________
void		OperCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(2))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
OppassCommand::OppassCommand(inherited const &src) : inherited(src) { return; }
OppassCommand::OppassCommand(std::string const &prefix, std::string const &password) : inherited(prefix, "OPPASS", password) { return; }

// __________Member functions____________
void		OppassCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(1))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
ModeCommand::ModeCommand(inherited const &src) : inherited(src) { return; }
ModeCommand::ModeCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "MODE", args) { return; }
ModeCommand::ModeCommand(std::string const &prefix, std::string const &chan) : inherited(prefix, "MODE", chan) { return; }
ModeCommand::ModeCommand(std::string const &prefix, std::string const &target, std::string const &mode) :
	inherited(prefix, "MODE", inherited::argvec()) {
	addArg(target);
	addArg(mode);
}
ModeCommand::ModeCommand(std::string const &prefix, std::string const &chan, std::string const &mode, std::string const &nick) :
	inherited(prefix, "MODE", inherited::argvec()) {
	addArg(chan);
	addArg(mode);
	addArg(nick);
}

// __________Member functions____________
void		ModeCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!this->argNbr(1) && !this->argNbr(2) && !this->argNbr(3))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
AwayCommand::AwayCommand(inherited const &src) : inherited(src) { return; }
AwayCommand::AwayCommand(std::string const &prefix, std::string const &message) : inherited(prefix, "AWAY", message) { return; }

// __________Member functions____________
void		AwayCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
NamesCommand::NamesCommand(inherited const &src) : inherited(src) { return; }
NamesCommand::NamesCommand(std::string const &prefix, std::string const &target) : inherited(prefix, "NAMES", target) { return; }

// __________Member functions____________
void		NamesCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
ListCommand::ListCommand(inherited const &src) : inherited(src) { return; }
ListCommand::ListCommand(std::string const &prefix, std::string const &target) : inherited(prefix, "LIST", target) { return; }

// __________Member functions____________
void		ListCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
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
WhoCommand::WhoCommand(inherited const &src) : inherited(src) { return; }
WhoCommand::WhoCommand(std::string const &prefix, std::string const &mask) : inherited(prefix, "WHO", mask) { return; }

// __________Member functions____________
void		WhoCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (argNbr() > 1)
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		WhoCommand::mask(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
ConnectCommand::~ConnectCommand(void) { return; }
// ConnectCommand::ConnectCommand(void) { return; }
ConnectCommand::ConnectCommand(ConnectCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
ConnectCommand &ConnectCommand::operator=(ConnectCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
ConnectCommand::ConnectCommand(inherited const &src) : inherited(src) { return; }
ConnectCommand::ConnectCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "CONNECT", args) { return; }
ConnectCommand::ConnectCommand(std::string const &prefix, std::string const &target, std::string const &port) :
	inherited(prefix, "CONNECT", inherited::argvec()) {
	addArg(target);
	addArg(port);
}

// __________Member functions____________
void		ConnectCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(2))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		ConnectCommand::target(void) const { return (argX(0)); }
std::string		ConnectCommand::port(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
WallopsCommand::~WallopsCommand(void) { return; }
// WallopsCommand::WallopsCommand(void) { return; }
WallopsCommand::WallopsCommand(WallopsCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
WallopsCommand &WallopsCommand::operator=(WallopsCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
WallopsCommand::WallopsCommand(inherited const &src) : inherited(src) { return; }
WallopsCommand::WallopsCommand(std::string const &prefix, std::string const &message) :
	inherited(prefix, "WALLOPS", message) { return; }

// __________Member functions____________
void		WallopsCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(1))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		WallopsCommand::message(void) const { return (argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
InviteCommand::~InviteCommand(void) { return; }
// InviteCommand::InviteCommand(void) { return; }
InviteCommand::InviteCommand(InviteCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
InviteCommand &InviteCommand::operator=(InviteCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
InviteCommand::InviteCommand(inherited const &src) : inherited(src) { return; }
InviteCommand::InviteCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "INVITE", args) { return; }
InviteCommand::InviteCommand(std::string const &prefix, std::string const &nick, std::string const &chan) :
	inherited(prefix, "INVITE", inherited::argvec()) {
	addArg(nick);
	addArg(chan);
}

// __________Member functions____________
void		InviteCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(2))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		InviteCommand::nick(void) const { return (argX(0)); }
std::string		InviteCommand::chan(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
KickCommand::~KickCommand(void) { return; }
// KickCommand::KickCommand(void) { return; }
KickCommand::KickCommand(KickCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
KickCommand &KickCommand::operator=(KickCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
KickCommand::KickCommand(inherited const &src) : inherited(src) { return; }
KickCommand::KickCommand(std::string const &prefix, inherited::argvec const &args) : inherited(prefix, "KICK", args) { return; }
KickCommand::KickCommand(std::string const &prefix, std::string const &chan, std::string const &nicks, std::string const &msg) :
	inherited(prefix, "KICK", inherited::argvec()) {
	addArg(chan);
	addArg(nicks);
	addArg(msg);
}

// __________Member functions____________
void		KickCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(2) && !argNbr(3))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		KickCommand::chan(void) const { return (argX(0)); }
std::string		KickCommand::nicks(void) const { return (argX(1)); }
std::string		KickCommand::comment(void) const { return (argX(2)); }

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
ServerCommand::ServerCommand(inherited const &src) : inherited(src) { return; }
ServerCommand::ServerCommand(std::string const prefix, inherited::argvec const &args) :
	inherited(prefix, "SERVER", args) { return; }
ServerCommand::ServerCommand(inherited::argvec const &args) :
	inherited(NO_PREFIX, "SERVER", args) { return; }
// ServerCommand::ServerCommand(std::string const prefix, ServerCommand const &cmd) :
// 	inherited(prefix, "SERVER", cmd.getArgs()) { return; } //hopcount + 1

void		ServerCommand::isValid(void) const throw(InvalidCommand) {
	inherited::isValid();
}

std::string			ServerCommand::servername(void)		const { return (argX(0)); }
std::string			ServerCommand::hopcount(void)		const { return (argX(1)); }
std::string			ServerCommand::token(void)			const { return (argX(2)); }
std::string			ServerCommand::info(void)			const { return (argX(3)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
NjoinCommand::~NjoinCommand(void) { return; }
// NjoinCommand::NjoinCommand(void) { return; }
NjoinCommand::NjoinCommand(NjoinCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
NjoinCommand &NjoinCommand::operator=(NjoinCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
NjoinCommand::NjoinCommand(inherited const &src) : inherited(src) { return; }
NjoinCommand::NjoinCommand(std::string const &prefix, std::string const &chan, std::string const &members) :
inherited(prefix, "NJOIN", inherited::argvec()) {
	addArg(chan);
	addArg(members);
}

// __________Member functions____________
void		NjoinCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(2))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		NjoinCommand::chan(void) const { return (argX(0)); }
std::string		NjoinCommand::members(void) const { return (argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
SquitCommand::~SquitCommand(void) { return; }
// SquitCommand::SquitCommand(void) { return; }
SquitCommand::SquitCommand(SquitCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)) { return; }
SquitCommand &SquitCommand::operator=(SquitCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	return *this;
}

// _____________Constructor______________
SquitCommand::SquitCommand(inherited const &src) : inherited(src) { return; }
SquitCommand::SquitCommand(std::string const &prefix, inherited::argvec const &args) :
inherited(prefix, "SQUIT", args) { return; }
SquitCommand::SquitCommand(std::string const &prefix, std::string const &server, std::string const &comment) :
inherited(prefix, "SQUIT", inherited::argvec()) {
	addArg(server);
	addArg(comment);
}

// __________Member functions____________
void		SquitCommand::isValid(void) const throw(inherited::InvalidCommand) {
	inherited::isValid();
	if (!argNbr(2))
		throw (inherited::InvalidCommand(ERR_WRONGPARAMSNUMBER));
}

std::string		SquitCommand::server(void) const { return (argX(0)); }
std::string		SquitCommand::comment(void) const { return (argX(1)); }



