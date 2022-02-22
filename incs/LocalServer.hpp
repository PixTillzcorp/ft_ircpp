#ifndef FT_IRC_LOCAL_SERVER_HPP
#define FT_IRC_LOCAL_SERVER_HPP

#include "ConfigParser.hpp"
#include "SelectModule.hpp"
#include "CommandLib.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Debug.hpp"

#include <map>
#include <fstream>

#define LOCALSERV_WHITELIST_FILE	"whitelist.config"
#define NO_TOKEN					""

#define MAX_CONXS					15
#define MAX_IOSTEP_LENGTH			64

#define MAX_RPL_ENTITY				5

#define NICKNAME					'n'
#define USERNAME					'u'
#define REALNAME					'r'

class LocalServer : public Server {
public: // #####################################################################
	typedef Server									inherited;
	typedef std::list<Channel *>					chanlist;
	typedef std::list<Channel *>::iterator			chanlist_it;
	typedef std::list<Channel *>::const_iterator	chanlist_cit;
	typedef bool(Client::*clicheck)(void) const;

	// ____________Canonical Form____________
	virtual ~LocalServer();
	// LocalServer(); /* Private standard constructor*/
	LocalServer(LocalServer const &src);
	LocalServer &operator=(LocalServer const &src);

	// _____________Constructor______________
	LocalServer(std::string const &port, u_int16_t family, std::string const &password);

	// __________Member functions____________
	bool		run(void);
	
	bool		joinNet(std::string const &authinfo);

	// ____________Setter / Getter___________

	// _chans
	std::list<Channel *> const					&getChans(void) const { return this->_chans; }
	void										setChans(std::list<Channel *> &src) { this->_chans = src; }

	// _sm
	SelectModule const							&getSM(void) const { return this->_sm; }
	void										setSM(SelectModule &src) { this->_sm = src; }

	// _password
	std::string const							&getPassword(void) const { return this->_password; }

	// _oppass
	std::string const							&getOppass(void) const { return this->_oppass; }
	void										setOppass(std::string const &src) { this->_oppass = src; }

	// _logfile
	LogFile const								&getLog(void) const { return this->_log; }
	void										setLog(LogFile const &src) { this->_log = src; }

	// _whitelist
	ConfigParser const							&getWhitelist(void) const { return this->_whitelist; }
	void										setWhitelist(ConfigParser const &src) { this->_whitelist = src; }

	// ______________Exceptions______________

private: // ####################################################################

	std::list<Channel *>				_chans;
	SelectModule						_sm;
	std::string							_password;
	std::string							_oppass;
	std::string							_stdin;
	std::stringstream					_stdout;
	ConfigParser						_whitelist;
	LogFile								_log;

	// ___________Connection utils___________
	void		newConx(void);
	void		finishConx(Server *sender, Client *target, std::string const &quitmsg, bool netsplit);
	void		finishConx(Server *sender, Server *target, std::string const &quitmsg, bool netsplit);
	void		finishConx(Connection *target);
	void		breakLinks(Server *origin);
	void		breakLinkTokens(Server *origin);
	void		setNewLocalop(void);

	Channel		*newChan(Client *&sender, std::string const &name);
	void		finishChan(Channel *target);
	Channel		*findChannel(std::string const &name);

	Server		*newLink(Server *sender, ServerCommand const &cmd);
	Client		*newLink(Server *sender, NickCommand const &cmd);

	bool		isWhitelisted(std::string const &info, bool svrname) const;
	bool		checkWhitelistHost(std::string const &svrname, std::string const &host) const;
	bool		checkWhitelistInfos(std::string const &svrname, std::string const &port) const;
	std::string const	whitelistInfos(std::string const &svrname) const;
	std::string const	whitelistHost(std::string const &svrname) const;
	std::string const	whitelistPort(std::string const &svrname) const;
	std::string const	whitelistPassword(std::string const &svrname) const;

	void		purge(void);
	virtual void newToken(void);
	bool		isUniqueToken(std::string const &cmp) const;

	// ______________Broadcast_______________
	void	broadcastToServers(Server *sender, Command const &cmd);
	void	broadcastToClients(Client *sender, Command const &cmd);
	void	broadcastToSomeClients(Command const &cmd, clicheck fp);

	void	warnAllOperators(std::string const &msg);

	void	broadcastServer(Server *sender, Server *shared);
	void	broadcastClient(Server *sender, Client *shared); // std::string const &tk

	void	shareServs(Server *target);
	void	shareClients(Server *target);
	void	shareChans(Server *target);

	// ___________Connection count___________
	std::string const	howManyChannel(void) const;

	// ____________Select module_____________
	void		selectCall(void);

	bool		isReadable(Connection *conx) const;
	bool		isWritable(Connection *conx) const;

	void		checkStd(void);
	void		checkSock(void);
	void		checkConxs(void);

	// __________Execution module____________
	// exec path
	void		execCommand(Connection *&sender, Command cmd);
	void		execCommandPending(Connection *&sender, Command const &cmd);
	void		execCommandClient(Client *&sender, Command const &cmd);
	void		execCommandServer(Server *sender, Command const &cmd);

	// pending
	void		execPass(Connection *&sender, PassCommand const &cmd);
	void		execNick(Connection *&sender, NickCommand const &cmd);
	void		execUser(Connection *&sender, UserCommand const &cmd);
	void		execServer(Connection *&sender, ServerCommand const &cmd);

	// server
	void		execNick(Server *sender, NickCommand const &cmd);
	void		execServer(Server *sender, ServerCommand const &cmd);
	void		execNjoin(Server *sender, NjoinCommand const &cmd);
	void		execMode(Server *sender, ModeCommand const &cmd);
	void		execJoin(Server *sender, JoinCommand const &cmd);
	void		execPart(Server *sender, PartCommand const &cmd);
	void		execPrivmsg(Server *sender, PrivmsgCommand const &cmd);
	void		execNotice(Server *sender, NoticeCommand const &cmd);
	void		execQuit(Server *sender, QuitCommand const &cmd);
	void		execSquit(Server *sender, SquitCommand const &cmd);
	void		execInvite(Server *sender, InviteCommand const &cmd);
	void		execKick(Server *sender, KickCommand const &cmd);

	// client
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
	void		execSquit(Client *sender, SquitCommand const &cmd);
	void		execConnect(Client *sender, ConnectCommand const &cmd);
	void		execInvite(Client *sender, InviteCommand const &cmd);
	void		execKick(Client *sender, KickCommand const &cmd);

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

	// ____________Log Functions_____________
	void		logErroneousCommand(std::string const &servername, std::string const &cmd);
	void		logNotify(bool error, std::string const &msg);
	void		logColored(std::string const &color, std::string const &msg);
	void		logError(Connection *target, std::string const &msg);
	void		logError(Connection *target, std::string const &msg, std::string const &errmsg);
	void		logPromote(std::string const &rank, std::string const &reason, bool success);
	void		logMode(std::string const &type, std::string const &name, std::string const &modes);
	void		logSuccess(std::string const &msg);

	// ____________Debug Display_____________
	void		showChans(void);
	void		showNet(void);
};

#endif //FT_IRC_LOCAL_SERVER_HPP
