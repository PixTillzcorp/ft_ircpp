#ifndef FT_IRC_COMMAND_LIB_HPP
#define FT_IRC_COMMAND_LIB_HPP

#include "../incs/Command.hpp"

// ########################################
// 					UNIQUE
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PassCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~PassCommand(void);
	// PassCommand(void);
	PassCommand(PassCommand const &cpy);
	PassCommand &operator=(PassCommand const &cpy);

	// _____________Constructor______________
	PassCommand(inherited const &src);
	PassCommand(std::string const &pass);

	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
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
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~ErrorCommand(void);
	// ErrorCommand(void);
	ErrorCommand(ErrorCommand const &cpy);
	ErrorCommand &operator=(ErrorCommand const &cpy);

	// _____________Constructor______________
	ErrorCommand(inherited const &src);
	ErrorCommand(std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
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
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~NickCommand(void);
	// NickCommand(void);
	NickCommand(NickCommand const &cpy);
	NickCommand &operator=(NickCommand const &cpy);

	// _____________Constructor______________
	NickCommand(inherited const &src);
	NickCommand(std::string const &prefix, std::string const &nickname);
	NickCommand(std::string const &prefix, inherited::argvec const &args);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			nickname(void) const;
	std::string			hopcount(void) const;
	std::string			username(void) const;
	std::string			host(void) const;
	std::string			servertoken(void) const;
	std::string			umode(void) const;
	std::string			realname(void) const;

private:
	NickCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UserCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~UserCommand(void);
	// UserCommand(void);
	UserCommand(UserCommand const &cpy);
	UserCommand &operator=(UserCommand const &cpy);

	// _____________Constructor______________
	UserCommand(inherited const &src);
	UserCommand(inherited::argvec const &args);
	UserCommand(std::string const &username, std::string const &modes, std::string const &realname);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
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
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~QuitCommand(void);
	// QuitCommand(void);
	QuitCommand(QuitCommand const &cpy);
	QuitCommand &operator=(QuitCommand const &cpy);

	// _____________Constructor______________
	QuitCommand(inherited const &src);
	QuitCommand(std::string const &prefix);
	QuitCommand(std::string const &prefix, std::string const &msg);
	QuitCommand(std::string const &prefix, inherited::argvec const &args);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			message(void) const;

private:
	QuitCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PrivmsgCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~PrivmsgCommand(void);
	// PrivmsgCommand(void);
	PrivmsgCommand(PrivmsgCommand const &cpy);
	PrivmsgCommand &operator=(PrivmsgCommand const &cpy);

	// _____________Constructor______________
	PrivmsgCommand(inherited const &src);
	PrivmsgCommand(std::string const &prefix, inherited::argvec const &args);
	PrivmsgCommand(std::string const &prefix, std::string const &target, std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;
	std::string			message(void) const;

private:
	PrivmsgCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class NoticeCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~NoticeCommand(void);
	// NoticeCommand(void);
	NoticeCommand(NoticeCommand const &cpy);
	NoticeCommand &operator=(NoticeCommand const &cpy);

	// _____________Constructor______________
	NoticeCommand(inherited const &src);
	NoticeCommand(std::string const &prefix, inherited::argvec const &args);
	NoticeCommand(std::string const &prefix, std::string const &target, std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;
	std::string			message(void) const;

private:
	NoticeCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PingCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~PingCommand(void);
	// PingCommand(void);
	PingCommand(PingCommand const &cpy);
	PingCommand &operator=(PingCommand const &cpy);

	// _____________Constructor______________
	PingCommand(inherited const &src);
	PingCommand(std::string const &sender);
	PingCommand(std::string const &sender, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			sender(void) const;
	std::string			target(void) const;

private:
	PingCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PongCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~PongCommand(void);
	// PongCommand(void);
	PongCommand(PongCommand const &cpy);
	PongCommand &operator=(PongCommand const &cpy);

	// _____________Constructor______________
	PongCommand(inherited const &src);
	PongCommand(std::string const &sender);
	PongCommand(std::string const &sender, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			sender(void) const;
	std::string			target(void) const;

private:
	PongCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class MotdCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~MotdCommand(void);
	// MotdCommand(void);
	MotdCommand(MotdCommand const &cpy);
	MotdCommand &operator=(MotdCommand const &cpy);

	// _____________Constructor______________
	MotdCommand(inherited const &src);
	MotdCommand(std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;

private:
	MotdCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LusersCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~LusersCommand(void);
	// LusersCommand(void);
	LusersCommand(LusersCommand const &cpy);
	LusersCommand &operator=(LusersCommand const &cpy);

	// _____________Constructor______________
	LusersCommand(inherited const &src);
	LusersCommand(std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;

private:
	LusersCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class WhoisCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~WhoisCommand(void);
	// WhoisCommand(void);
	WhoisCommand(WhoisCommand const &cpy);
	WhoisCommand &operator=(WhoisCommand const &cpy);

	// _____________Constructor______________
	WhoisCommand(inherited const &src);
	WhoisCommand(std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;

private:
	WhoisCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class JoinCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~JoinCommand(void);
	// JoinCommand(void);
	JoinCommand(JoinCommand const &cpy);
	JoinCommand &operator=(JoinCommand const &cpy);

	// _____________Constructor______________
	JoinCommand(inherited const &src);
	JoinCommand(std::string const &prefix, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;

private:
	JoinCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PartCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~PartCommand(void);
	// PartCommand(void);
	PartCommand(PartCommand const &cpy);
	PartCommand &operator=(PartCommand const &cpy);

	// _____________Constructor______________
	PartCommand(inherited const &src);
	PartCommand(std::string const &prefix, inherited::argvec const &args);
	PartCommand(std::string const &prefix, std::string const &target, std::string const &msg);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;
	std::string			message(void) const;

private:
	PartCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TopicCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~TopicCommand(void);
	// TopicCommand(void);
	TopicCommand(TopicCommand const &cpy);
	TopicCommand &operator=(TopicCommand const &cpy);

	// _____________Constructor______________
	TopicCommand(inherited const &src);
	TopicCommand(std::string const &prefix, inherited::argvec const &args);
	TopicCommand(std::string const &prefix, std::string const &target, std::string const &topic);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;
	std::string			topic(void) const;

private:
	TopicCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class OperCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~OperCommand(void);
	// OperCommand(void);
	OperCommand(OperCommand const &cpy);
	OperCommand &operator=(OperCommand const &cpy);

	// _____________Constructor______________
	OperCommand(inherited const &src);
	OperCommand(std::string const &prefix, inherited::argvec const &args);
	OperCommand(std::string const &prefix, std::string const &name, std::string const &password);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			name(void) const;
	std::string			password(void) const;

private:
	OperCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class OppassCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~OppassCommand(void);
	// OppassCommand(void);
	OppassCommand(OppassCommand const &cpy);
	OppassCommand &operator=(OppassCommand const &cpy);

	// _____________Constructor______________
	OppassCommand(inherited const &src);
	OppassCommand(std::string const &prefix, std::string const &password);

	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			password(void) const;

private:
	OppassCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ModeCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~ModeCommand(void);
	// ModeCommand(void);
	ModeCommand(ModeCommand const &cpy);
	ModeCommand &operator=(ModeCommand const &cpy);

	// _____________Constructor______________
	ModeCommand(inherited const &src);
	ModeCommand(std::string const &prefix, inherited::argvec const &args);
	ModeCommand(std::string const &prefix, std::string const &chan);
	ModeCommand(std::string const &prefix, std::string const &target, std::string const &mode);
	ModeCommand(std::string const &prefix, std::string const &chan, std::string const &mode, std::string const &nick);

	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			field1(void) const;
	std::string			field2(void) const;
	std::string			field3(void) const;

private:
	ModeCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class AwayCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~AwayCommand(void);
	// AwayCommand(void);
	AwayCommand(AwayCommand const &cpy);
	AwayCommand &operator=(AwayCommand const &cpy);

	// _____________Constructor______________
	AwayCommand(inherited const &src);
	AwayCommand(std::string const &prefix, std::string const &message);

	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			message(void) const;

private:
	AwayCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class NamesCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~NamesCommand(void);
	// NamesCommand(void);
	NamesCommand(NamesCommand const &cpy);
	NamesCommand &operator=(NamesCommand const &cpy);

	// _____________Constructor______________
	NamesCommand(inherited const &src);
	NamesCommand(std::string const &prefix, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;

private:
	NamesCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ListCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~ListCommand(void);
	// ListCommand(void);
	ListCommand(ListCommand const &cpy);
	ListCommand &operator=(ListCommand const &cpy);

	// _____________Constructor______________
	ListCommand(inherited const &src);
	ListCommand(std::string const &prefix, std::string const &target);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			target(void) const;

private:
	ListCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class WhoCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~WhoCommand(void);
	// WhoCommand(void);
	WhoCommand(WhoCommand const &cpy);
	WhoCommand &operator=(WhoCommand const &cpy);

	// _____________Constructor______________
	WhoCommand(inherited const &src);
	WhoCommand(std::string const &prefix, std::string const &mask);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			mask(void) const;

private:
	WhoCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ########################################
// 					SERVER
// ########################################

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ServerCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~ServerCommand(void);
	// ServerCommand(void);
	ServerCommand(ServerCommand const &cpy);
	ServerCommand &operator=(ServerCommand const &cpy);

	// _____________Constructor______________
	ServerCommand(std::string const prefix, inherited::argvec const &args);
	ServerCommand(inherited::argvec const &args);
	ServerCommand(inherited const &src);

	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			servername(void) const;
	std::string			hopcount(void) const;
	std::string			token(void) const;
	std::string			info(void) const;

private:
	ServerCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class NjoinCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~NjoinCommand(void);
	// NjoinCommand(void);
	NjoinCommand(NjoinCommand const &cpy);
	NjoinCommand &operator=(NjoinCommand const &cpy);

	// _____________Constructor______________
	NjoinCommand(inherited const &src);
	NjoinCommand(std::string const &prefix, std::string const &chan, std::string const &members);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			chan(void) const;
	std::string			members(void) const;

private:
	NjoinCommand(void);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class SquitCommand : public Command {
public:
	typedef Command inherited;

	// ____________Canonical Form____________
	virtual ~SquitCommand(void);
	// SquitCommand(void);
	SquitCommand(SquitCommand const &cpy);
	SquitCommand &operator=(SquitCommand const &cpy);

	// _____________Constructor______________
	SquitCommand(inherited const &src);
	SquitCommand(std::string const &prefix, inherited::argvec const &args);
	SquitCommand(std::string const &prefix, std::string const &server, std::string const &comment);
	
	// __________Member functions____________
	virtual void		isValid(void) const throw(inherited::InvalidCommand);
	std::string			server(void) const;
	std::string			comment(void) const;

private:
	SquitCommand(void);
};

#endif //FT_IRC_COMMAND_LIB_HPP
