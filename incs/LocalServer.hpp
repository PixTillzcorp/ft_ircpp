#ifndef FT_IRC_LOCAL_SERVER_HPP
#define FT_IRC_LOCAL_SERVER_HPP

#include "ConfigParser.hpp"
#include "SelectModule.hpp"
#include "CommandLib.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "LogFile.hpp"

#include <map>
#include <fstream>

#define LOCALSERV_WHITELIST_FILE	"whitelist.config"

#define MAX_RPL_ENTITY		5

#define ALLNAME				255
#define NICKNAME			1
#define USERNAME			2
#define REALNAME			4
#define CLIENTNAME			7

class LocalServer : public Server {
public: // #####################################################################
	typedef Server									inherited;
	typedef std::map<std::string, Connection *> 	namesmap;
	typedef std::list<Connection *>					conxlist;
	typedef std::list<Connection *>::iterator		conxlist_it;
	typedef std::list<Connection *>::const_iterator	conxlist_cit;
	typedef std::list<Channel *>					chanlist;
	typedef std::list<Channel *>::iterator			chanlist_it;
	typedef std::list<Channel *>::const_iterator	chanlist_cit;


	// ____________Canonical Form____________
	virtual ~LocalServer();
	// LocalServer(); /* Private standard constructor*/
	LocalServer(LocalServer const &src);
	LocalServer &operator=(LocalServer const &src);

	// _____________Constructor______________
	LocalServer(std::string const &port, u_int16_t family, std::string const &password);

	// __________Member functions____________
	bool		run(void);
	
	void		joinNet(std::string const &host, std::string const &port, std::string const &password);

	// ____________Setter / Getter___________
	// _nicknames
	std::map<std::string, Connection *> const	&getNicknames(void) const { return this->_nicknames; }
	void										setNicknames(std::map<std::string, Connection *> &src) { this->_nicknames = src; }

	// _usernames
	std::map<std::string, Connection *> const	&getUsernames(void) const { return this->_usernames; }
	void										setUsernames(std::map<std::string, Connection *> &src) { this->_usernames = src; }

	// _realnames
	std::map<std::string, Connection *> const	&getRealnames(void) const { return this->_realnames; }
	void										setRealnames(std::map<std::string, Connection *> &src) { this->_realnames = src; }

	// _servnames
	std::map<std::string, Connection *> const	&getServnames(void) const { return this->_servnames; }
	void										setServnames(std::map<std::string, Connection *> &src) { this->_servnames = src; }

	// _conxs
	std::list<Connection *> const				&getConxs(void) const { return this->_conxs; }
	void										setConxs(std::list<Connection *> &src) { this->_conxs = src; }

	// _chans
	std::list<Channel *> const					&getChans(void) const { return this->_chans; }
	void										setChans(std::list<Channel *> &src) { this->_chans = src; }

	// _sm
	SelectModule const							&getSM(void) const { return this->_sm; }
	void										setSM(SelectModule &src) { this->_sm = src; }

	// _password
	std::string const							&getPassword(void) const { return this->_password; }

	// _password
	std::string const							&getOppass(void) const { return this->_oppass; }
	void										setOppass(std::string const &src) { this->_oppass = src; }

	// _logfile
	LogFile const								&getLogfile(void) const { return this->_logfile; }
	void										setLogfile(LogFile const &src) { this->_logfile = src; }

	// _whitelist
	ConfigParser const							&getWhitelist(void) const { return this->_whitelist; }
	void										setWhitelist(ConfigParser const &src) { this->_whitelist = src; }

	// ______________Exceptions______________

private: // ####################################################################

	std::map<std::string, Connection *>	_nicknames;
	std::map<std::string, Connection *>	_usernames;
	std::map<std::string, Connection *>	_realnames;
	std::map<std::string, Connection *>	_servnames;
	std::list<Connection *>				_conxs;
	std::list<Channel *>				_chans;
	SelectModule						_sm;
	std::string							_password;
	std::string							_oppass;
	LogFile								_logfile;
	ConfigParser						_whitelist;


	// ___________Connection utils___________
	void		newConx(void);
	void		finishConx(Connection *target, bool clear);
	Client		*findClient(std::string const &name, unsigned char type);
	Server		*findServer(std::string const &name);

	Channel		*newChan(Client *&sender, std::string const &name);
	void		finishChan(Channel *target);
	Channel		*findChannel(std::string const &name);

	void		mapName(namesmap &names, std::string const &name, Connection *conx);
	bool		mapHasName(namesmap &names, std::string const &name);
	void		unmapName(namesmap &names, std::string const &name);

	bool		isWhitelisted(std::string const &info, bool servername) const;
	bool		checkWhitelistHost(std::string const &servername, std::string const &host) const;
	std::string const	whitelistPassword(std::string const &servername) const;

	// ___________Connection count___________
	unsigned int		howMany(unsigned short check) const;
	unsigned int		howMany(unsigned short check, bool direct) const;

	std::string const	howManyClient(bool direct) const;
	std::string const	howManyServer(bool direct, bool self) const;
	std::string const 	howManyService(bool direct) const;
	std::string const	howManyUnknown(void) const;
	std::string const	howManyOperator(void) const;
	std::string const	howManyChannel(void) const;

	// ____________Select module_____________
	void		selectCall(void);

	bool		isReadable(Connection *conx) const;
	bool		isWritable(Connection *conx) const;

	void		checkStd(void);
	void		checkSock(void);
	void		checkConxs(void);

	// __________Execution module____________
	void		execCommand(Connection *&sender, Command cmd);
	void		execCommandPending(Connection *&sender, Command const &cmd);
	void		execCommandClient(Client *&sender, Command const &cmd);

	void		execPass(Connection *&sender, PassCommand const &cmd);
	void		execNick(Connection *&sender, NickCommand const &cmd);
	void		execUser(Connection *&sender, UserCommand const &cmd);
	void		execServer(Connection *&sender, ServerCommand const &cmd);

	void		execNick(Client *sender, NickCommand const &cmd);
	void		execUser(Client *sender, UserCommand const &cmd);
	void		execPrivmsg(Client *sender, PrivmsgCommand const &cmd);
	void		execNotice(Client *sender, NoticeCommand const &cmd);
	void		execQuit(Client *sender, QuitCommand const &cmd);
	void		execPong(Client *sender, PongCommand const &cmd);
	void		execPing(Client *sender, PingCommand const &cmd);
	void		execMotd(Client *sender, MotdCommand const &cmd);
	void		execLusers(Client *sender, LusersCommand const &cmd);
	void		execWhois(Client *sender, WhoisCommand const &cmd);
	void		execJoin(Client *sender, JoinCommand const &cmd);
	void		execPart(Client *sender, PartCommand const &cmd);
	void		execTopic(Client *sender, TopicCommand const &cmd);
	void		execOppass(Client *sender, OppassCommand const &cmd);
	void		execOper(Client *sender, OperCommand const &cmd);
	void		execMode(Client *sender, ModeCommand const &cmd);
	void		execAway(Client *sender, AwayCommand const &cmd);
	void		execNames(Client *sender, NamesCommand const &cmd);
	void		execList(Client *sender, ListCommand const &cmd);
	void		execWho(Client *sender, WhoCommand const &cmd);

	// ___________Numeric replies____________
	void		numericReply(Connection *sender, unsigned short code);
	void		numericReply(Connection *sender, unsigned short code, std::string const &field);
	void		numericReply(Connection *sender, unsigned short code, Command const &cmd);
	void		numericReply(Connection *sender, unsigned short code, Channel *chan);
	void		numericReply(Connection *sender, unsigned short code, Client *target);
	void		numericReplyNames(Client *sender, std::string const &chan, std::list<std::string> &names);
	void		numericReplyWho(Client *sender, Channel *chan, Client *target);
	void		numericReplyWhoischannel(Client *sender, Client *target, std::list<std::string> chans);
	void		namesListAloneClient(std::list<std::string> &lst) const;
	void		updateChans(Client *sender, Command const &cmd);
	std::string	const codeToStr(unsigned short code) const;

	// ____________Debug Display_____________
	void		showLocalServer(void) const;
	void		showNames(std::map<std::string, Connection *> names) const;
	void		showChans(void) const;
	void		showNet(void) const;
};

#endif //FT_IRC_LOCAL_SERVER_HPP
