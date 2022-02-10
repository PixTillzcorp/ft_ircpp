#ifndef FT_IRCPP_SERVER_HPP
#define FT_IRCPP_SERVER_HPP

#include "Connection.hpp"
#include "Client.hpp"

#include <map>

#define SERVER_VERSION "0.1.0.0"
#define SERVER_AUTHOR "PixTillz_zlliTxiP"
#define SERVER_IMPLEM "FT_IRC"
#define SERVER_TOKEN_LEN 5
#define SERVER_ERR_TOKEN "none"

class Server : public Connection {
public:
	typedef Connection								inherited;
	typedef std::list<Connection *>					conxlist;
	typedef std::list<Connection *>::iterator		conxlist_it;
	typedef std::list<Connection *>::const_iterator	conxlist_cit;

	std::string						servername;
	std::list<Connection *>			conxs;
	std::string						token;
	std::string						implem;
	std::string						version;
	std::string						desc;

	// ____________Canonical Form____________
    virtual ~Server(void);
    Server(Server const &src);
    Server  &operator=(Server const &src);

	// _____________Constructor______________
    Server(Connection *&src, ServerCommand const &cmd);
    Server(Server *link, ServerCommand const &cmd);
    Server(std::string const &host, std::string const &port, u_int16_t family);

	// __________Member functions____________
	virtual std::string const &name(void) const;
	Command::argvec const serverArgsShare(std::string const &servertoken) const;
	Command::argvec const serverArgsConnect(void) const;
	Command::argvec const serverArgsAccept(void) const;

	unsigned int howMany(char flag) const;
	unsigned int howMany(unsigned short check, bool direct) const;

	std::string const howManyClient(bool direct) const;
	std::string const howManyServer(bool direct, bool self) const;
	std::string const howManyService(bool direct) const;
	std::string const howManyUnknown(void) const;
	std::string const howManyOperator(void) const;

	Client		*findClient(std::string const &name, char type);
	void		shareClients(Server *target);
	Server		*findServer(std::string const &name);
	void		shareServs(Server *sender, Server *target);

	Command		finishLinkConx(Client *conx, std::string const &msg);
	Command		finishLinkConx(Server *conx, std::string const &msg);

	void		addLink(Connection *conx);

	bool		compare(Connection *conx) const;
	bool		isToken(std::string const &cmp) const;
	bool		knowToken(std::string const &cmp) const;
	virtual void newToken(void);
	Server		*getTokenSource(std::string const &tk);

	void		newLink(Server *sender, ServerCommand const &cmd);

	void		endDeadLinks(void);

private:
	Server(void);
};

std::ostream &operator<<(std::ostream &flux, Server const &src);

#endif //FT_IRCPP_SERVER_HPP
