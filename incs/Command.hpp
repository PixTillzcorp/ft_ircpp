#ifndef FT_IRC_COMMAND_HPP
#define FT_IRC_COMMAND_HPP

#include "Message.hpp"

#include <vector>

#define MAX_MOTD_READ 80
#define MAX_LEN_NICK 9
#define NO_PREFIX ""
#define STRNPOS std::string::npos

#define RPL_WELCOME				001
#define RPL_YOURHOST			002
#define RPL_CREATED				003
#define RPL_MYINFO				004
#define	RPL_LUSERCLIENT			251
#define	RPL_LUSEROP				252
#define	RPL_LUSERUNKNOWN		253
#define	RPL_LUSERCHANNELS		254
#define	RPL_LUSERME				255
#define RPL_AWAY				301
#define RPL_UNAWAY				305
#define RPL_NOWAWAY				306
#define RPL_WHOISUSER			311
#define RPL_WHOISSERVER			312
#define RPL_WHOISOPERATOR		313
#define RPL_ENDOFWHO			315
#define RPL_ENDOFWHOIS			318
#define RPL_WHOISCHANNELS		319
#define RPL_LIST				322
#define RPL_LISTEND				323
#define RPL_CHANNELMODEIS		324
#define RPL_NOTOPIC				331
#define RPL_TOPIC				332
#define RPL_INVITING			341
#define RPL_WHOREPLY			352
#define RPL_NAMESREPLY			353
#define RPL_ENDOFNAMES			366
#define RPL_MOTD				372
#define RPL_MOTDSTART			375
#define RPL_ENDOFMOTD			376
#define RPL_YOUREOP				381

#define ERR_DISCARDCOMMAND		000
#define ERR_NOSUCHNICK			401
#define ERR_NOSUCHSERVER		402
#define ERR_NOSUCHCHANNEL		403
#define ERR_CANNOTSENDTOCHAN	404
#define ERR_TOOMANYCHANNELS		405
#define ERR_NOORIGIN			409
#define ERR_NORECIPIENT			411
#define ERR_NOTEXTTOSEND		412
#define ERR_UNKNOWNCOMMAND		421
#define ERR_NOMOTD				422
#define ERR_NONICKNAMEGIVEN		431
#define ERR_ERRONEOUSNICKNAME	432
#define ERR_NICKNAMEINUSE		433
#define ERR_ERRONEOUSUSERNAME	434
#define ERR_ERRONEOUSREALNAME	435
#define ERR_USERNOTINCHANNEL	441
#define ERR_NOTONCHANNEL		442
#define ERR_USERONCHANNEL		443
#define ERR_NOTREGISTERED		451
#define ERR_WRONGPARAMSNUMBER	461
#define ERR_ALREADYREGISTERED	462
#define ERR_PASSWDMISMATCH		464
#define ERR_UNKNOWNMODE			472
#define ERR_INVITEONLYCHAN		473
#define ERR_NOPRIVILEGES		481
#define ERR_CHANOPRIVSNEEDED	482
#define ERR_NOTLOCALOP			486
#define ERR_UMODEUNKNOWNFLAG	501
#define ERR_USERSDONTMATCH		502

class Command : public Message {
public: // #####################################################################
	typedef Message						inherited;
	typedef std::vector<std::string>	argvec;
	typedef argvec::iterator			argvec_it;
	typedef argvec::const_iterator		argvec_cit;

	// ____________Canonical Form____________
	virtual ~Command(void);
	Command(Command const &cpy);
	Command &operator=(Command const &cpy);

	// _____________Constructor______________
	Command(Message msg);
	Command(std::string const &prefix, std::string const &command, argvec const &args);
	Command(std::string const &prefix, std::string const &command, std::string const &arg);

	// __________Member functions____________
	void				addArg(std::string arg);
	std::string const	argX(size_t x) const;
	argvec				argList(void) const;
	argvec				getArgs(void) const;
	size_t				findIn(size_t x, std::string const &str) const;
	size_t				argNbr(void) const;
	bool				argNbr(size_t n) const;
	Message				message(void) const;
	virtual void		isValid(void) const throw(InvalidCommand);
	bool				compare(std::string const &cmp) const;

	// ______________Exceptions______________
	typedef class InvalidCommandException : public std::exception { // thrown by !isValid()
	public:
		typedef std::exception inherited;

		unsigned short code;

		virtual ~InvalidCommandException() throw();
		InvalidCommandException(unsigned short src);
		InvalidCommandException(InvalidCommandException const &cpy);
		InvalidCommandException &operator=(InvalidCommandException const &cpy);
		virtual const char *what() const throw();

	private:
		InvalidCommandException();
	} InvalidCommand;
	
	std::string	prefix;
	std::string	command;
	argvec		args;

private:
	Command(void);
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
