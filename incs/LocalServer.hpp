#ifndef FT_IRC_LOCAL_SERVER_HPP
#define FT_IRC_LOCAL_SERVER_HPP

#include "../incs/SelectModule.hpp"
#include "../incs/CommandLib.hpp"
#include "../incs/Client.hpp"

#include <map>
#include <fstream>

#define ALLNAME 255
#define NICKNAME 1
#define USERNAME 2
#define REALNAME 4
#define CLIENTNAME 7
#define SERVNAME 8

#define COUNT_DIRECTSERVER 0
#define COUNT_SERVER 1
#define COUNT_DIRECTCLIENT 2
#define COUNT_CLIENT 3
#define COUNT_SERVICE 4
#define COUNT_UNKNOWN 5
#define COUNT_OPERATOR 6
#define COUNT_CHANNEL 7

class LocalServer : public Connection {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~LocalServer();
	// LocalServer(); /* Private standard constructor*/
	LocalServer(LocalServer const &src);
	LocalServer &operator=(LocalServer const &src);

	// _____________Constructor______________
	LocalServer(std::string const &port, u_int16_t family, std::string password);

	// __________Member functions____________
	bool		run(void);
	Connection *acceptConx(void) const;
	void		newConx(void);
	void		finishConx(Connection *target, bool clear);
	void		selectCall(void);

	Client		*findClientByName(std::string const &name, unsigned char type);

	std::string const 	howManyDirectClient(void) const;
	std::string const	howManyClient(void) const;
	std::string const 	howManyDirectServer(bool self) const;
	std::string const	howManyServer(bool self) const;
	std::string const	howManyService(void) const;
	std::string const	howManyUnknown(void) const;
	std::string const	howManyOperator(void) const;
	std::string const	howManyChannel(void) const;

	// ____________Setter / Getter___________
	// _nicknames
	std::map<std::string, Connection *> const	&getNicknames(void) const;
	void										setNicknames(std::map<std::string, Connection *> &src);

	// _usernames
	std::map<std::string, Connection *> const	&getUsernames(void) const;
	void										setUsernames(std::map<std::string, Connection *> &src);

	// _realnames
	std::map<std::string, Connection *> const	&getRealnames(void) const;
	void										setRealnames(std::map<std::string, Connection *> &src);

	// _count
	std::vector<unsigned int> const				&getCount(void) const;
	void										setCount(std::vector<unsigned int> &src);

	// _conxs
	std::list<Connection *> const				&getConxs(void) const;
	void										setConxs(std::list<Connection *> &src);

	// _sm
	SelectModule const							&getSM(void) const;
	void										setSM(SelectModule &src);

	// _password
	std::string const							&getPassword(void) const;

	// ______________Exceptions______________

private: // ####################################################################

	std::map<std::string, Connection *>	_nicknames;
	std::map<std::string, Connection *>	_usernames;
	std::map<std::string, Connection *>	_realnames;
	std::vector<unsigned int>			_count;
	std::list<Connection *>				_conxs;
	SelectModule						_sm;
	std::string							_password;

	bool		isReadable(Connection *conx) const;
	bool		isWritable(Connection *conx) const;

	void		checkStd(void);
	void		checkSock(void);
	void		checkConxs(void);

	// __________Execution module____________
	void		execCommand(Connection *&sender, Command *cmd);
	void		execCommandPending(Connection *&sender, Command *cmd);
	void		execCommandClient(Connection *&sender, Command *cmd);
	void		execPass(Connection *&sender, PassCommand const &cmd);
	void		execNick(Connection *&sender, NickCommand const &cmd);
	void		execUser(Connection *&sender, UserCommand const &cmd);
	void		execPrivmsg(Connection *&sender, PrivmsgCommand const &cmd);
	void		execQuit(Connection *&sender, QuitCommand const &cmd);
	void		execPong(Connection *&sender, PongCommand const &cmd);
	void		execPing(Connection *&sender, PingCommand const &cmd);
	void		execMotd(Connection *&sender, MotdCommand const &cmd);
	void		execLusers(Connection *&sender, LusersCommand const &cmd);

	// ___________Numeric replies____________
	void		numericReply(Connection *&sender, unsigned short code);
	void		numericReply(Connection *&sender, unsigned short code, std::string const &field1);
	std::string	const nbrToStr(unsigned int nbr) const;
	std::string	const codeToStr(unsigned short code) const;

	// ____________Debug Display_____________
	void		showLocalServer(void) const;
	void		showNames(std::map<std::string, Connection *> names) const;
	void		showNet(void) const;
};

#endif //FT_IRC_LOCAL_SERVER_HPP
