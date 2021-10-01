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
**----- File ----------------{ Command.cpp }----------------------------------**
**----- Created -------------{ 2021-05-21 12:59:56 }--------------------------**
**----- Updated -------------{ 2021-09-29 23:37:40 }--------------------------**
********************************************************************************
*/

#include "../incs/Command.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Command::~Command(void) { return; }
Command::Command(void) { return; }
Command::Command(Command const &src) { *this = src; }
Command &Command::operator=(Command const &src) {
	static_cast<Message &>(*this) = static_cast<Message const &>(src);
	this->_prefix = src.getPrefix();
	this->_command = src.getCommand();
	this->_args = src.getArgs();
	return *this;
}

// _____________Constructor______________
Command::Command(Message *msg) : Message(*msg) {
	std::string data = this->_content;
	std::string del = " ";
	std::string shard;

	while (!data.empty())
	{
		shard = data.substr(0, data.find(del));
		data.erase(0, data.find(del) + (data.size() == shard.size() ? 0 : del.size()));
		if (this->_command.empty())
		{
			if (this->_prefix.empty() && shard[0] == ':')
				this->_prefix = shard.substr(1, std::string::npos);
			else
				this->_command = shard;
		}
		else {
			if (shard[0] != ':')
				this->_args.push_back(shard);
			else
			{
				shard = shard.substr(1, std::string::npos);
				del = " :";
				if (data.empty())
					this->_args.push_back(shard);
				else {
					shard += " " + data.substr(0, data.find(del));
					data.erase(0, data.find(del) + (data.size() == data.substr(0, data.find(del)).size() ? 0 : del.size()));
					this->_args.push_back(shard);
				}
			}
		}
	}
	delete msg;
}

Command::Command(std::string const &prefix, std::string const &command, std::list<std::string> const &args) :	Message(""),
																												_prefix(prefix),
																												_command(command),
																												_args(args) {
	std::list<std::string>::iterator it = this->_args.begin();
	std::string content;
	bool lst = false;

	if (!this->_prefix.empty())
	{
		if (this->_prefix[0] != ':')
			content = ':' + this->_prefix;
		else
			content = this->_prefix;
	}
	if (!this->_prefix.empty() && !this->_command.empty())
		content += ' ';
	content += this->_command;
	if (this->_args.size() > 0)
		content += ' ';
	while (it != this->_args.end())
	{
		if (!lst && (*it).find(' ') == std::string::npos)
			content += (*it);
		else
		{
			content += ':' + (*it);
			lst = true;
		}
		it++;
		if (it != this->_args.end())
			content += ' ';
	}
	static_cast<Message &>(*this) = Message(content);
}

Command::Command(std::string const &prefix, std::string const &command, std::string const &arg) {
	std::list<std::string> args(1, arg);

	if (arg.empty())
		args.pop_front();
	*this = Command(prefix, command, args);
}

// __________Member functions____________
void	Command::addArg(std::string const arg) { this->_args.push_back(arg); }
std::string const Command::argX(size_t x) const {
	std::list<std::string>::const_iterator it;

	if (this->_args.empty() || this->_args.size() < x)
		return std::string();
	for (it = static_cast<std::list<std::string>::const_iterator>(this->_args.begin()); x; it++)
		x--;
	return *it;
}
size_t	Command::findIn(size_t x, std::string const &str) const { return (this->argX(x).find(str)); }
size_t	Command::argNbr(void) const { return this->_args.size(); }
bool	Command::checkArgNbr(size_t n) const { return this->argNbr() == n; }
Message Command::message(void) const { return Message(static_cast<Message const &>(*this)); }

void Command::isValid(void) const throw(Command::InvalidCommandException) {
	if (this->_command.empty() || this->argNbr() > 15 || this->getContent().length() > 510)
		throw (Command::InvalidCommandException(ERR_DISCARDCOMMAND));
	else if (!this->_prefix.empty() && this->_prefix.find_first_not_of(AUL_DDD) != std::string::npos)
		throw (Command::InvalidCommandException(ERR_DISCARDCOMMAND));
}

// ____________Setter / Getter___________
// _prefix
std::string		const Command::getPrefix(void) const { return this->_prefix; }
void			Command::setPrefix(std::string const &src) {
	if (!src.empty() && this->_prefix.empty()) {
		this->_prefix = (src[0] == ':' ? src.substr(1) : src);
		this->_content = ":" + this->_prefix + (this->_command.empty() ? "" : " ") + this->_content;
	}
}
// _command
std::string		const Command::getCommand(void) const { return this->_command; }
void			Command::setCommand(std::string const &src) { this->_command = src; }
bool			Command::checkCommand(std::string const &cmp) const { return !this->_command.compare(cmp); }
// _args
std::list<std::string> const &Command::getArgs(void) const { return this->_args; }
void			Command::setArgs(std::list<std::string> const &src) { this->_args = src; }

// ########################################
// 				 EXECEPTIONS
// ########################################

Command::InvalidCommandException::~InvalidCommandException(void) throw() { return; }
Command::InvalidCommandException::InvalidCommandException(void) : code(ERR_DISCARDCOMMAND) { return; }
Command::InvalidCommandException::InvalidCommandException(unsigned short src) : code(src) { return; }
Command::InvalidCommandException::InvalidCommandException(InvalidCommandException const &src) { *this = src; }
Command::InvalidCommandException &Command::InvalidCommandException::operator=(InvalidCommandException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	this->code = src.code;
	return *this;
}
const char *Command::InvalidCommandException::what() const throw() {
	return ("Invalid Command.");
}

// ########################################
// 					DEBUG
// ########################################

bool			operator==(Command const &src, std::string const &cmp) { return (src.checkCommand(cmp)); }
bool			operator==(Command const &src, char const *cmp) { return (src == std::string(cmp)); }
bool			operator!=(Command const &src, std::string const &cmp) { return !(src.checkCommand(cmp)); }
bool			operator!=(Command const &src, char const *cmp) { return (src != std::string(cmp)); }
std::ostream	&operator<<(std::ostream &flux, Command const &src) {
	// std::list<std::string>::const_iterator it = src.getArgs().cbegin();

	// flux << "\033[38;5;57m+++++++++++ Command +++++++++++++" << std::endl;
	flux << static_cast<Message const>(src);
	// flux << "Cmd" << std::endl << "{" << std::endl;
	// flux << "\t_prefix: [" + src.getPrefix() + "]" << std::endl;
	// flux << "\t_command: [" + src.getCommand() + "]" << std::endl;
	// flux << "\t_args:" << std::endl;
	// for (; it != src.getArgs().cend(); it++)
	// 	flux << "\t[" + (*it) + "]" << std::endl;
	// flux << "}" << std::endl << "+++++++++++++++++++++++++++++++++\033[0m" << std::endl;
	return flux;
}
