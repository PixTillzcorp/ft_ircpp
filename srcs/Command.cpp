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
**----- Updated -------------{ 2022-02-10 01:53:12 }--------------------------**
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
				prefix = shard.substr(1, STRNPOS);
			else
				command = shard;
		}
		else {
			if (shard[0] != ':')
				args.push_back(shard);
			else
			{
				shard = shard.substr(1, STRNPOS);
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
	for (argvec_cit it = args.begin(); it != args.end(); it++)
	{
		if (!lst && it->find(' ') == STRNPOS && (*it).rfind(":", 0) == STRNPOS)
			content += ' ' + (*it);
		else {
			content += (!(*it).rfind(":", 0) ? " " : " :") + (*it);
			lst = true;
		}
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
	bool lst = false;

	Utils::clearSpaces(arg, false);
	if (!arg.empty()) {
		if (!args.empty() && args.back().find(' ') != STRNPOS)
			content.append((arg[0] == ':' ? " " : " :") + arg);
		else {
			if (arg[0] == ':')
				lst = true;
			if (!lst && arg.find(' ') == STRNPOS)
				content.append(" " + arg);
			else
				content.append((arg[0] == ':' ? " " : " :") + arg);
		}
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
	else if (!prefix.empty() && prefix.find_first_not_of(CHAR_PREFIX) != STRNPOS)
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
