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
**----- Updated -------------{ 2021-10-01 16:13:51 }--------------------------**
********************************************************************************
*/

#include "../incs/CommandLib.hpp"

// ########################################
// 					UNIQUE
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PassCommand::~PassCommand(void) { return; }
// PassCommand::PassCommand(void) { return; }
PassCommand::PassCommand(PassCommand const &src) { *this = src; }
PassCommand &PassCommand::operator=(PassCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
PassCommand::PassCommand(Command const &src) : Command(src) { return; }
PassCommand::PassCommand(std::list<std::string> const &args) : Command(NO_PREFIX, "PASS", args) { return; }

// __________Member functions____________
void		PassCommand::isValid(void) const throw(Command::InvalidCommandException) {
	if (!this->_prefix.empty())
		throw (Command::InvalidCommandException(ERR_DISCARDCOMMAND));
	if (!this->checkArgNbr(3) && !this->checkArgNbr(1))
		throw (Command::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
	if (this->checkArgNbr(3) && this->argX(2).find('|') == std::string::npos)
		throw (Command::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		PassCommand::password(void) const { return (this->argX(0)); }
std::string		PassCommand::version(void) const { return (this->argX(1)); }
std::string		PassCommand::implem(void) const { return (this->argX(2).substr(0, this->findIn(2, "|"))); }
std::string		PassCommand::author(void) const {
	size_t			spot = this->findIn(2, "|");

	if (spot == this->argX(2).size())
		return (std::string());
	else
		return (this->argX(2).substr(spot + 1));
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
ErrorCommand::~ErrorCommand(void) { return; }
// ErrorCommand::ErrorCommand(void) { return; }
ErrorCommand::ErrorCommand(ErrorCommand const &src) { *this = src; }
ErrorCommand &ErrorCommand::operator=(ErrorCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
ErrorCommand::ErrorCommand(Command const &src) : Command(src) { return; }
ErrorCommand::ErrorCommand(std::string const &msg) : Command(NO_PREFIX, "ERROR", msg) { return; }

// __________Member functions____________
void		ErrorCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (!this->_prefix.empty())
		throw (Command::InvalidCommandException(ERR_DISCARDCOMMAND));
	if (!this->checkArgNbr(1))
		throw (Command::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		ErrorCommand::message(void) const { return (this->argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					CLIENT
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
NickCommand::~NickCommand(void) { return; }
// NickCommand::NickCommand(void) { return; }
NickCommand::NickCommand(NickCommand const &src) { *this = src; }
NickCommand &NickCommand::operator=(NickCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
NickCommand::NickCommand(Command const &src) : Command(src) { return; }
NickCommand::NickCommand(std::string const &prefix, std::string const &nickname) : Command(prefix, "NICK", nickname) { return; }

// __________Member functions____________
void		NickCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (!this->checkArgNbr(1))
		throw (Command::InvalidCommandException(ERR_NONICKNAMEGIVEN));
}

std::string		NickCommand::nickname(void) const { return (this->argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
UserCommand::~UserCommand(void) { return; }
// UserCommand::UserCommand(void) { return; }
UserCommand::UserCommand(UserCommand const &src) { *this = src; }
UserCommand &UserCommand::operator=(UserCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
UserCommand::UserCommand(Command const &src) : Command(src) { return; }
UserCommand::UserCommand(std::list<std::string> const &args) : Command(NO_PREFIX, "USER", args) { return; }
UserCommand::UserCommand(std::string const &username, std::string const &modes, std::string const &realname) {
	std::list<std::string> args;

	if (!username.empty())
		args.push_back(username);
	if (!modes.empty())
		args.push_back(modes);
	args.push_back("*");
	if (!realname.empty())
		args.push_back(realname);
	*this = UserCommand(args);
}

// __________Member functions____________
void		UserCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (!this->checkArgNbr(4))
		throw (Command::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		UserCommand::username(void) const { return (this->argX(0)); }
std::string		UserCommand::modes(void) const { return (this->argX(1)); }
std::string		UserCommand::unused(void) const { return (this->argX(2)); }
std::string		UserCommand::realname(void) const { return (this->argX(3)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
QuitCommand::~QuitCommand(void) { return; }
// QuitCommand::QuitCommand(void) { return; }
QuitCommand::QuitCommand(QuitCommand const &src) { *this = src; }
QuitCommand &QuitCommand::operator=(QuitCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
QuitCommand::QuitCommand(Command const &src) : Command(src) { return; }
QuitCommand::QuitCommand(std::string const &msg) : Command("", "QUIT", msg) { return; }

// __________Member functions____________
void		QuitCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (this->argNbr() > 1)
		throw (Command::InvalidCommandException(ERR_WRONGPARAMSNUMBER));
}

std::string		QuitCommand::message(void) const { return (this->argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PrivmsgCommand::~PrivmsgCommand(void) { return; }
// PrivmsgCommand::PrivmsgCommand(void) { return; }
PrivmsgCommand::PrivmsgCommand(PrivmsgCommand const &src) { *this = src; }
PrivmsgCommand &PrivmsgCommand::operator=(PrivmsgCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
PrivmsgCommand::PrivmsgCommand(Command const &src) : Command(src) { return; }
PrivmsgCommand::PrivmsgCommand(std::string const &prefix, std::list<std::string> const &args) : Command(prefix, "PRIVMSG", args) { return; }
PrivmsgCommand::PrivmsgCommand(std::string const &prefix, std::string const &target, std::string const &msg) {
	std::list<std::string> args;

	if (!target.empty())
		args.push_back(target);
	if (!msg.empty())
		args.push_back(msg);
	static_cast<Command &>(*this) = Command(prefix, "PRIVMSG", args);
}

// __________Member functions____________
void		PrivmsgCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (!this->argNbr())
		throw (Command::InvalidCommandException(ERR_NORECIPIENT));
	if (this->argNbr() < 2)
		throw (Command::InvalidCommandException(ERR_NOTEXTTOSEND));
}

std::string		PrivmsgCommand::target(void) const { return (this->argX(0)); }
std::string		PrivmsgCommand::message(void) const { return (this->argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PingCommand::~PingCommand(void) { return; }
// PingCommand::PingCommand(void) { return; }
PingCommand::PingCommand(PingCommand const &src) { *this = src; }
PingCommand &PingCommand::operator=(PingCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
PingCommand::PingCommand(Command const &src) : Command(src) { return; }
PingCommand::PingCommand(std::string const &sender) : Command(NO_PREFIX, "PING", sender) { return; }
PingCommand::PingCommand(std::string const &sender, std::string const &target) {
	std::list<std::string> args;

	if (!sender.empty())
		args.push_back(sender);
	if (!target.empty())
		args.push_back(target);
	static_cast<Command &>(*this) = Command(NO_PREFIX, "PING", args);
}

// __________Member functions____________
void		PingCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (!this->argNbr())
		throw (Command::InvalidCommandException(ERR_NOORIGIN));
}

std::string		PingCommand::sender(void) const { return (this->argX(0)); }
std::string		PingCommand::target(void) const { return (this->argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
PongCommand::~PongCommand(void) { return; }
// PongCommand::PongCommand(void) { return; }
PongCommand::PongCommand(PongCommand const &src) { *this = src; }
PongCommand &PongCommand::operator=(PongCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
PongCommand::PongCommand(Command const &src) : Command(src) { return; }
PongCommand::PongCommand(std::string const &sender) : Command(NO_PREFIX, "PONG", sender) { return; }
PongCommand::PongCommand(std::string const &sender, std::string const &target) {
	std::list<std::string> args;

	if (!sender.empty())
		args.push_back(sender);
	if (!target.empty())
		args.push_back(target);
	static_cast<Command &>(*this) = Command(NO_PREFIX, "PONG", args);
}

// __________Member functions____________
void		PongCommand::isValid(void) const throw(Command::InvalidCommandException) {
	this->Command::isValid();
	if (!this->argNbr())
		throw (Command::InvalidCommandException(ERR_NOORIGIN));
}

std::string		PongCommand::sender(void) const { return (this->argX(0)); }
std::string		PongCommand::target(void) const { return (this->argX(1)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
MotdCommand::~MotdCommand(void) { return; }
// MotdCommand::MotdCommand(void) { return; }
MotdCommand::MotdCommand(MotdCommand const &src) { *this = src; }
MotdCommand &MotdCommand::operator=(MotdCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
MotdCommand::MotdCommand(Command const &src) : Command(src) { return; }
MotdCommand::MotdCommand(std::string const &target) : Command(NO_PREFIX, "MOTD", target) { return; }

// __________Member functions____________
void		MotdCommand::isValid(void) const throw(Command::InvalidCommandException) { this->Command::isValid(); }

std::string		MotdCommand::target(void) const { return (this->argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ____________Canonical Form____________
LusersCommand::~LusersCommand(void) { return; }
// LusersCommand::LusersCommand(void) { return; }
LusersCommand::LusersCommand(LusersCommand const &src) { *this = src; }
LusersCommand &LusersCommand::operator=(LusersCommand const &src) {
	static_cast<Command &>(*this) = static_cast<Command const &>(src);
	return *this;
}

// _____________Constructor______________
LusersCommand::LusersCommand(Command const &src) : Command(src) { return; }
LusersCommand::LusersCommand(std::string const &target) : Command(NO_PREFIX, "LUSERS", target) { return; }

// __________Member functions____________
void		LusersCommand::isValid(void) const throw(Command::InvalidCommandException) { this->Command::isValid(); }

std::string		LusersCommand::target(void) const { return (this->argX(0)); }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					SERVER
// ########################################

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// // ____________Canonical Form____________
// ServerCommand::~ServerCommand(void) { return; }
// // ServerCommand::ServerCommand(void) { return; }
// ServerCommand::ServerCommand(ServerCommand const &src) { *this = src; }
// ServerCommand &ServerCommand::operator=(ServerCommand const &src) {
// 	static_cast<Command &>(*this) = static_cast<Command const &>(src);
// 	this->servername = src.servername;
// 	this->hopcount = src.hopcount;
// 	this->token = src.token;
// 	this->info = src.info;
// 	return *this;
// }

// // _____________Constructor______________
// ServerCommand::ServerCommand(std::string const prefix, std::list<std::string> const &args) : Command(prefix, "SERVER", args) {
// 	this->servername = this->argX(0);
// 	this->hopcount = std::stoi(this->argX(1));
// 	this->token = this->argX(2);
// 	this->info = this->argX(3);
// 	return;
// }
// ServerCommand::ServerCommand(std::list<std::string> const &args) : Command(NO_PREFIX, "SERVER", args) {
// 	this->servername = this->argX(0);
// 	this->hopcount = std::stoi(this->argX(1));
// 	this->token = this->argX(2);
// 	this->info = this->argX(3);
// 	return;
// }
// ServerCommand::ServerCommand(std::string const prefix, ServerCommand const &cmd) : Command(prefix, "SERVER", cmd.getArgs()) {
// 	this->hopcount += 1;
// 	return;
// }
// ServerCommand::ServerCommand(Command const &src) : Command(src) {
// 	this->servername = this->argX(0);
// 	this->hopcount = std::stoi(this->argX(1));
// 	this->token = this->argX(2);
// 	this->info = this->argX(3);
// 	return;
// }

// bool		ServerCommand::isValid(void) const {
// 	if (this->_prefix.find_first_not_of(AUL_DDD) != std::string::npos)
// 		return false;
// 	if (this->_command.empty() || this->_command.compare("SERVER"))
// 		return false;
// 	if (this->argNbr() != 4)
// 		return false;
// 	return true;
// }

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// // ____________Canonical Form____________
// NickServerCommand::~NickServerCommand(void) { return; }
// // NickServerCommand::NickServerCommand(void) { return; }
// NickServerCommand::NickServerCommand(NickServerCommand const &src) { *this = src; }
// NickServerCommand &NickServerCommand::operator=(NickServerCommand const &src) {
// 	static_cast<Command &>(*this) = static_cast<Command const &>(src);
// 	this->nickname = src.nickname;
// 	this->hopcount = src.hopcount;
// 	this->username = src.username;
// 	this->host = src.host;
// 	this->token = src.token;
// 	this->usermode = src.usermode;
// 	this->realname = src.realname;
// 	return *this;
// }

// // _____________Constructor______________
// NickServerCommand::NickServerCommand(std::string const prefix, std::string const &nick) : Command(prefix, "NICK", std::list<std::string>(1, nick)){ return; }
// NickServerCommand::NickServerCommand(std::list<std::string> const &args) : Command("", "NICK", args) { return; }
// NickServerCommand::NickServerCommand(Command const &src) : Command(src) {
// 	this->nickname = this->argX(0);
// 	if (src.argNbr() == 7)
// 	{
// 		this->hopcount = std::stoi(this->argX(1));
// 		this->username = this->argX(2);
// 		this->host = this->argX(3);
// 		this->token = this->argX(4);
// 		this->usermode = this->argX(5);
// 		this->realname = this->argX(6);
// 	}
// 	return;
// }

// bool		NickServerCommand::isValid(void) const {
// 	if (this->_prefix.find_first_not_of(AUL_DDD) != std::string::npos)
// 		return false;
// 	if (this->_command.empty() || this->_command.compare("NICK"))
// 		return false;
// 	if (this->argNbr() != 1 && this->argNbr() != 7)
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
// 	this->target = src.target;
// 	return *this;
// }

// // _____________Constructor______________
// SquitCommand::SquitCommand(std::string const prefix, std::string const &target) : Command(prefix, "SQUIT", std::list<std::string>(1, target)) { return; }
// SquitCommand::SquitCommand(Command const &src) : Command(src) {
// 	this->target = this->argX(0);
// 	return;
// }

// bool		SquitCommand::isValid(void) const {
// 	if (this->_prefix.find_first_not_of(AUL_DDD) != std::string::npos)
// 		return false;
// 	if (this->_command.empty() || this->_command.compare("SQUIT"))
// 		return false;
// 	if (this->argNbr() != 2)
// 		return false;
// 	return true;
// }

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
