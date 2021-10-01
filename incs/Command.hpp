#ifndef FT_IRC_COMMAND_HPP
#define FT_IRC_COMMAND_HPP

#include "../incs/Message.hpp"
#include "../incs/Debug.hpp"

#include <list>

#define MAX_MOTD_READ 80
#define NO_PREFIX ""
#define AUL_DDD "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-"

#define RPL_WELCOME 001
#define	RPL_LUSERCLIENT 251
#define	RPL_LUSEROP 252
#define	RPL_LUSERUNKNOWN 253
#define	RPL_LUSERCHANNELS 254
#define	RPL_LUSERME 255
#define RPL_MOTD 372
#define RPL_MOTDSTART 375
#define RPL_ENDOFMOTD 376

#define ERR_DISCARDCOMMAND 000
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHSERVER 402
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS 405
#define ERR_NOORIGIN 409
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NOMOTD 422
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEOUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NOTREGISTERED 451
#define ERR_WRONGPARAMSNUMBER 461
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464

class Command : public Message {
public: // #####################################################################
	// ____________Canonical Form____________
	virtual ~Command(void);
	Command(void);
	Command(Command const &src);
	Command &operator=(Command const &src);

	// _____________Constructor______________
	Command(Message *msg);
	Command(std::string const &prefix, std::string const &command, std::list<std::string> const &args);
	Command(std::string const &prefix, std::string const &command, std::string const &arg);

	// __________Member functions____________
	void				addArg(std::string const arg);
	std::string const	argX(size_t x) const;
	size_t				findIn(size_t x, std::string const &str) const;
	size_t				argNbr(void) const;
	bool				checkArgNbr(size_t n) const;
	Message				message(void) const;
	virtual void		isValid(void) const throw(InvalidCommandException);

	// ____________Setter / Getter___________
	// _prefix
	std::string	const 	getPrefix(void) const;
	void				setPrefix(std::string const &src);
	// _command
	std::string	const 	getCommand(void) const;
	void				setCommand(std::string const &src);
	bool				checkCommand(std::string const &cmp) const;
	// _args
	std::list<std::string> const &getArgs(void) const;
	void				setArgs(std::list<std::string> const &src);

	// ______________Exceptions______________
	class InvalidCommandException : public std::exception { // thrown by !isValid()
	public:
		unsigned short code;

		virtual ~InvalidCommandException(void) throw();
		InvalidCommandException(void);
		InvalidCommandException(unsigned short src);
		InvalidCommandException(InvalidCommandException const &src);
		InvalidCommandException &operator=(InvalidCommandException const &src);
		virtual const char *what() const throw();
	};

protected: // ##################################################################
	std::string				_prefix;
	std::string				_command;
	std::list<std::string>	_args;
};

// ########################################
// 					DEBUG
// ########################################

std::ostream	&operator<<(std::ostream &flux, Command const &src);
bool			operator==(Command const &src, std::string const &cmp);
bool			operator==(Command const &src, char const *cmp);
bool			operator!=(Command const &src, std::string const &cmp);
bool			operator!=(Command const &src, char const *cmp);

#endif //FT_IRC_COMMAND_HPP
