#ifndef FT_IRCPP_SERVERINFO_HPP
#define FT_IRCPP_SERVERINFO_HPP

#include "../incs/CommandLib.hpp"
#include "../incs/SockInfo.hpp"
#include "../incs/FdSet.hpp"
#include "../incs/Connection.hpp"
#include "../incs/Server.hpp"
#include "../incs/Client.hpp"
#include <sys/select.h>
#include <list>

#define VERSION "0.1.0.0"
#define AUTHOR "PixTillz_zlliTxiP"
#define IMPLEM "FT_IRC"

class ServerInfo {
public:

	// ____________Canonical Form____________
	virtual ~ServerInfo( void );
	// ServerInfo( void ); /* default constructor is private */
	ServerInfo(ServerInfo const &src);
	ServerInfo &operator=(ServerInfo const &src);
	// ______________________________________

	// _____________Constructor______________
	// -----------------------------
	//		Parametric constructor
	// -----------------------------
	ServerInfo(std::string const info, std::string const port, std::string const password);
	// ______________________________________


	// _____________Select Call______________
	// -----------------------------
	//		Select function call
	// -----------------------------
	void selectCall(void);
	// ______________________________________


	// ______________Debugging_______________
	// Print every variables
	void debugShowContent(void) const;
	// ______________________________________


	// ____________Setter / Getter___________
	// -----------------------------
	//			   _name
	// -----------------------------
	std::string	getName(void) const;
	void		setName(std::string const &src);

	// -----------------------------
	//			 _password
	// -----------------------------
	std::string	getPassword(void) const;
	void		setPassword(std::string const &src);

	// -----------------------------
	//			   _sock
	// -----------------------------
	int			getSock(void) const;
	void		setSock(int const &src);

	// -----------------------------
	//	 		 _upperfd
	// -----------------------------
	int			getUpperfd(void) const;
	void		setUpperfd(int const &src);

	// -----------------------------
	//			 _sockinfo
	// -----------------------------
	SockInfo	const &getSockinfo(void) const;
	void		setSockinfo(SockInfo const &src);

	// -----------------------------
	//			   _mfds
	// -----------------------------
	FdSet		const &getMfds(void) const;
	void		setMfds(FdSet const &src);

	// -----------------------------
	//			   _rfds
	// -----------------------------
	FdSet		const &getRfds(void) const;
	void		setRfds(FdSet const &src);

	// -----------------------------
	//			   _wfds
	// -----------------------------
	FdSet		const &getWfds(void) const;
	void		setWfds(FdSet const &src);

	// -----------------------------
	//			  _version
	// -----------------------------
	std::string		const &getVersion(void) const;
	std::string		getNetVersion(void) const;
	void		setVersion(std::string const &src);

	// -----------------------------
	//			  _implem
	// -----------------------------
	std::string		const &getImplem(void) const;
	void		setImplem(std::string const &src);

	// -----------------------------
	//			   _conxs
	// -----------------------------
	std::list<Connection *> const &getConxs(void) const;
	void		setConxs(std::list<Connection *> &src);
	// -----------------------------
	//			   _done
	// -----------------------------
	bool		isDone(void) const;
	void		setDone(void);
	// ______________________________________

	// class ConnectExecption : public std::execption {
	// public:
	// 	virtual ~ConnectExecption(void);
	// 	ConnectExecption(void);
	// 	ConnectExecption(ConnectExecption const &src);
	// 	ConnectExecption &operator=(ConnectExecption const &src);
	// 	virtual const std:string what() const;
	// }

private:

	std::string				_name;
	std::string				_password;
	int						_sock;
	int						_upperfd;
	SockInfo				_sockinfo;
	FdSet					_mfds;
	FdSet					_rfds;
	FdSet					_wfds;
	std::string				_version;
	std::string				_implem;
	Connection				*_nolink;
	std::list<Connection *>	_conxs;
	bool					_done;

	// ____________Initialization____________
	void init(std::string const &host, std::string const &port);
	void initServerSock(void);
	void bindServerSock(void) const;
	void listenServerSock(void) const;
	void initMasterFds(void);
	void initRWFds(void);
	// ______________________________________

	// _______________Network________________
	void joinNet(std::string const &info);
	void shareNet(Connection const &target);
	void connectTo(std::string const &host, std::string const &port, std::string const &pass);
	void broadcast(Connection const *sender, Command const &cmd); //add broadcast for client in the future
	void shareNet(Connection *target);
	std::list<std::string> getListPassCommand(std::string const &pass) const;
	std::list<std::string> getListServerCommand(void) const;
	// ______________________________________

	// _____________Connections______________
	void newConx(SockInfo const &sin, SockStream const &ss);
	void newLink(Connection *&link, ServerCommand const &cmd);
	void newLink(Connection *&link, NickCommand const &cmd);
	void closeConx(std::list<Connection *>::iterator const &it);
	void finishConx(Connection *&target, std::string const &msg);
	Connection *getConxVs(int sock);
	Connection *getConxVn(std::string const &name);
	// ______________________________________

	// ______________Execution_______________
	void execCommand(Connection *&sender, Command const &cmd);
	// -----------------------------
	//			   Commands
	// -----------------------------
	void execPass(Connection *&sender, PassCommand const &cmd);
	void execServer(Connection *&sender, ServerCommand const &cmd);
	void execSquit(Connection *&sender, SquitCommand const &cmd);
	void execError(Connection *&sender, ErrorCommand const &cmd);
	// ______________________________________

	// _______________Checking_______________
	void checkStd(void);
	void checkSock(void);
	void checkFds(void);
	void checkUpperfd(int const nsock);
	bool isReadable(Connection const *conx) const;
	bool isWritable(Connection const *conx) const;
	// ______________________________________

	ServerInfo( void );

	// ########################################
	// 					DEBUG
	// ########################################

	void debugShowInfo(void) const;
	void debugShowNet(void) const;
	void sendTo(void);
};

#endif //FT_IRCPP_LOCALINFO_HPP
