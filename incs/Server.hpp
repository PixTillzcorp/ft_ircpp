#ifndef FT_IRCPP_SERVER_HPP
#define FT_IRCPP_SERVER_HPP

#include "../incs/Connection.hpp"

#define SERVER_VERSION "0.1.0.0"
#define SERVER_AUTHOR "PixTillz_zlliTxiP"
#define SERVER_IMPLEM "FT_IRC"

class Server : public Connection {
public:
	typedef Connection inherited;

	std::string		servername;
	std::string		token;
	std::string		implem;
	std::string		version;
	std::string		desc;

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
	Command::arglist const getArgListConnect(void) const;
	Command::arglist const getArgListAccept(void) const;

private:
	Server(void);
};

std::ostream &operator<<(std::ostream &flux, Server const &src);

#endif //FT_IRCPP_SERVER_HPP
