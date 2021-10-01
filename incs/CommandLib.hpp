#ifndef FT_IRC_COMMAND_LIB_HPP
#define FT_IRC_COMMAND_LIB_HPP

#include "../incs/Command.hpp"

// ########################################
// 					UNIQUE
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PassCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~PassCommand(void);
	// PassCommand(void);
	PassCommand(PassCommand const &src);
	PassCommand &operator=(PassCommand const &src);

	// _____________Constructor______________
	PassCommand(Command const &src);
	PassCommand(std::list<std::string> const &args);

	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			password(void) const;
	std::string			version(void) const;
	std::string			implem(void) const;
	std::string			author(void) const;

private:
	PassCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ErrorCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~ErrorCommand(void);
	// ErrorCommand(void);
	ErrorCommand(ErrorCommand const &src);
	ErrorCommand &operator=(ErrorCommand const &src);

	// _____________Constructor______________
	ErrorCommand(Command const &src);
	ErrorCommand(std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			message(void) const;

private:
	ErrorCommand(void);
};

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					CLIENT
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class NickCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~NickCommand(void);
	// NickCommand(void);
	NickCommand(NickCommand const &src);
	NickCommand &operator=(NickCommand const &src);

	// _____________Constructor______________
	NickCommand(Command const &src);
	NickCommand(std::string const &prefix, std::string const &nickname);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			nickname(void) const;

private:
	NickCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UserCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~UserCommand(void);
	// UserCommand(void);
	UserCommand(UserCommand const &src);
	UserCommand &operator=(UserCommand const &src);

	// _____________Constructor______________
	UserCommand(Command const &src);
	UserCommand(std::list<std::string> const &args);
	UserCommand(std::string const &username, std::string const &modes, std::string const &realname);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			username(void) const;
	std::string			modes(void) const;
	std::string			unused(void) const;
	std::string			realname(void) const;

private:
	UserCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class QuitCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~QuitCommand(void);
	// QuitCommand(void);
	QuitCommand(QuitCommand const &src);
	QuitCommand &operator=(QuitCommand const &src);

	// _____________Constructor______________
	QuitCommand(Command const &src);
	QuitCommand(std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			message(void) const;

private:
	QuitCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PrivmsgCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~PrivmsgCommand(void);
	// PrivmsgCommand(void);
	PrivmsgCommand(PrivmsgCommand const &src);
	PrivmsgCommand &operator=(PrivmsgCommand const &src);

	// _____________Constructor______________
	PrivmsgCommand(Command const &src);
	PrivmsgCommand(std::string const &prefix, std::list<std::string> const &args);
	PrivmsgCommand(std::string const &prefix, std::string const &target, std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			target(void) const;
	std::string			message(void) const;

private:
	PrivmsgCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PingCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~PingCommand(void);
	// PingCommand(void);
	PingCommand(PingCommand const &src);
	PingCommand &operator=(PingCommand const &src);

	// _____________Constructor______________
	PingCommand(Command const &src);
	PingCommand(std::string const &sender);
	PingCommand(std::string const &sender, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			sender(void) const;
	std::string			target(void) const;

private:
	PingCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PongCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~PongCommand(void);
	// PongCommand(void);
	PongCommand(PongCommand const &src);
	PongCommand &operator=(PongCommand const &src);

	// _____________Constructor______________
	PongCommand(Command const &src);
	PongCommand(std::string const &sender);
	PongCommand(std::string const &sender, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			sender(void) const;
	std::string			target(void) const;

private:
	PongCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class MotdCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~MotdCommand(void);
	// MotdCommand(void);
	MotdCommand(MotdCommand const &src);
	MotdCommand &operator=(MotdCommand const &src);

	// _____________Constructor______________
	MotdCommand(Command const &src);
	MotdCommand(std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			target(void) const;

private:
	MotdCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LusersCommand : public Command {
public:
	// ____________Canonical Form____________
	virtual ~LusersCommand(void);
	// LusersCommand(void);
	LusersCommand(LusersCommand const &src);
	LusersCommand &operator=(LusersCommand const &src);

	// _____________Constructor______________
	LusersCommand(Command const &src);
	LusersCommand(std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(Command::InvalidCommandException);
	std::string			target(void) const;

private:
	LusersCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					SERVER
// ########################################

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// class ServerCommand : public Command {
// public:
// 	// ____________Canonical Form____________
// 	virtual ~ServerCommand(void);
// 	// ServerCommand(void);
// 	ServerCommand(ServerCommand const &src);
// 	ServerCommand &operator=(ServerCommand const &src);

// 	// _____________Constructor______________
// 	ServerCommand(std::string const prefix, std::list<std::string> const &args);
// 	ServerCommand(std::list<std::string> const &args);
// 	ServerCommand(std::string const prefix, ServerCommand const &cmd);
// 	ServerCommand(Command const &src);

// 	virtual bool		isValid(void) const;

// 	std::string servername;
// 	int hopcount;
// 	std::string token;
// 	std::string info;

// private:
// 	ServerCommand(void);

// };

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// class NickServerCommand : public Command {
// public:
// 	// ____________Canonical Form____________
// 	virtual ~NickServerCommand(void);
// 	// NickServerCommand(void);
// 	NickServerCommand(NickServerCommand const &src);
// 	NickServerCommand &operator=(NickServerCommand const &src);

// 	// _____________Constructor______________
// 	NickServerCommand(std::string const prefix, std::string const &nick);
// 	NickServerCommand(std::list<std::string> const &args);
// 	NickServerCommand(Command const &src);

// 	virtual bool		isValid(void) const;

// 	std::string nickname;
// 	size_t		hopcount;
// 	std::string username;
// 	std::string host;
// 	std::string token;
// 	int			usermode;
// 	std::string realname;

// private:
// 	NickServerCommand(void);
// };

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// class SquitCommand : public Command {
// public:
// 	// ____________Canonical Form____________
// 	virtual ~SquitCommand(void);
// 	// SquitCommand(void);
// 	SquitCommand(SquitCommand const &src);
// 	SquitCommand &operator=(SquitCommand const &src);

// 	// _____________Constructor______________
// 	SquitCommand(std::string const prefix, std::string const &target);
// 	SquitCommand(Command const &src);

// 	virtual bool		isValid(void) const;

// 	std::string target;

// private:
// 	SquitCommand(void);

// };

#endif //FT_IRC_COMMAND_LIB_HPP
