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
**----- Updated -------------{ 2022-01-27 05:06:24 }--------------------------**
********************************************************************************
*/

#include "../incs/Command.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
Command::~Command(void) { return; }
Command::Command(Command const &cpy) :
	inherited(static_cast<inherited const &>(cpy)),
	prefix(cpy.prefix),
	command(cpy.command),
	args(cpy.args) { return; }
Command &Command::operator=(Command const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	prefix = cpy.prefix;
	command = cpy.command;
	args = cpy.args;
	return *this;
}

// _____________Constructor______________
Command::Command(Message msg) : inherited(msg) {
	std::string data = content;
	std::string del = " ";
	std::string shard;

	while (!data.empty())
	{
		shard = data.substr(0, data.find(del));
		data.erase(0, data.find(del) + (data.size() == shard.size() ? 0 : del.size()));
		if (!shard.compare(del))
			continue;
		if (command.empty())
		{
			if (prefix.empty() && shard[0] == ':')
				prefix = shard.substr(1, std::string::npos);
			else
				command = shard;
		}
		else {
			if (shard[0] != ':')
				args.push_back(shard);
			else
			{
				shard = shard.substr(1, std::string::npos);
				del = " :";
				if (data.empty())
					args.push_back(shard);
				else {
					shard += " " + data.substr(0, data.find(del));
					data.erase(0, data.find(del) + (data.size() == data.substr(0, data.find(del)).size() ? 0 : del.size()));
					args.push_back(shard);
				}
			}
		}
	}
}

Command::Command(std::string const &prefix, std::string const &command, Command::argvec const &args) :	inherited(""),
																										prefix(prefix),
																										command(command),
																										args(args) {
	argvec::const_iterator it = args.begin();
	std::string content;
	bool lst = false;

	if (!prefix.empty())
	{
		if (prefix[0] != ':')
			content = ':' + prefix;
		else
			content = prefix;
	}
	if (!prefix.empty() && !command.empty())
		content += ' ';
	content += command;
	if (args.size() > 0)
		content += ' ';
	while (it != args.end())
	{
		if (!lst && (*it).find(' ') == std::string::npos)
			content += (*it);
		else
		{
			content += ':' + (*it);
			lst = true;
		}
		it++;
		if (it != args.end())
			content += ' ';
	}
	static_cast<inherited &>(*this) = inherited(content);
}

Command::Command(std::string const &prefix, std::string const &command, std::string const &arg) {
	argvec args(1, arg);

	if (arg.empty())
		args.pop_back();
	*this = Command(prefix, command, args);
}

// __________Member functions____________
void	Command::addArg(std::string arg) {
	Utils::clearSpaces(arg, false);
	if (!arg.empty()) {
		if (!args.empty() && (args.back().find(' ') != std::string::npos ||
		!args.back().rfind(":", 0)) && arg[0] != ':')
			content.append(" :" + arg);
		else if (arg.find(' ') == std::string::npos || arg[0] == ':')
			content.append(" " + arg);
		else
			content.append(" :" + arg);
		args.push_back(arg);
	}
}
std::string const Command::argX(size_t x) const {
	if (args.empty() || x >= args.size())
		return std::string();
	return args[x];
}
Command::argvec	Command::getArgs(void) const { return args; }
size_t	Command::findIn(size_t x, std::string const &str) const { return (argX(x).find(str)); }
size_t	Command::argNbr(void) const { return args.size(); }
bool	Command::argNbr(size_t n) const { return args.size() == n; }
bool	Command::compare(std::string const &cmp) const { return !command.compare(cmp); }
Message Command::message(void) const { return inherited(static_cast<inherited const &>(*this)); }

void Command::isValid(void) const throw(Command::InvalidCommand) {
	if (command.empty() || argNbr() > 15 || content.size() > 510)
		throw (Command::InvalidCommand(ERR_DISCARDCOMMAND));
	else if (!prefix.empty() && prefix.find_first_not_of(CHAR_PREFIX) != std::string::npos)
		throw (Command::InvalidCommand(ERR_DISCARDCOMMAND));
}

// ########################################
// 				 EXECEPTIONS
// ########################################

Command::InvalidCommand::~InvalidCommandException(void) throw() { return; }
Command::InvalidCommand::InvalidCommandException(unsigned short code) : code(code) { return; }
Command::InvalidCommand::InvalidCommandException(InvalidCommand const &cpy) :
	inherited(static_cast<inherited const &>(cpy)), code(cpy.code) { return; }
Command::InvalidCommand &Command::InvalidCommand::operator=(InvalidCommand const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	code = cpy.code;
	return *this;
}
const char *Command::InvalidCommand::what() const throw() {
	return ("Invalid Command.");
}

// ########################################
// 					DEBUG
// ########################################

bool			operator==(Command const &src, std::string const &cmp) { return (src.compare(cmp)); }
bool			operator==(Command const &src, char const *cmp) { return (src == std::string(cmp)); }
bool			operator!=(Command const &src, std::string const &cmp) { return !(src.compare(cmp)); }
bool			operator!=(Command const &src, char const *cmp) { return (src != std::string(cmp)); }
std::ostream	&operator<<(std::ostream &flux, Command const &src) {
	// std::list<std::string>::const_iterator it = src.getArgs().cbegin();

	// flux << "\033[38;5;57m+++++++++++ Command +++++++++++++" << std::endl;
	flux << "[" << static_cast<Message const>(src) << "]";
	flux << "args[" << src.argNbr() << "]";
	// flux << "Cmd" << std::endl << "{" << std::endl;
	// flux << "\tprefix: [" + src.getPrefix() + "]" << std::endl;
	// flux << "\tcommand: [" + src.getCommand() + "]" << std::endl;
	// flux << "\targs:" << std::endl;
	// for (; it != src.getArgs().cend(); it++)
	// 	flux << "\t[" + (*it) + "]" << std::endl;
	// flux << "}" << std::endl << "+++++++++++++++++++++++++++++++++\033[0m" << std::endl;
	return flux;
}
