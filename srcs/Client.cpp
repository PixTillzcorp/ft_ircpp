/*
********************************************************************************
**____________________________________________________________________________**
**_____/\/\/\/\/\___/\/\____________/\/\/\/\/\/\_/\/\__/\/\__/\/\_____________**
**____/\/\____/\/\_______/\/\__/\/\____/\/\___________/\/\__/\/\__/\/\/\/\/\__**
**___/\/\/\/\/\___/\/\____/\/\/\______/\/\_____/\/\__/\/\__/\/\______/\/\_____**
**__/\/\_________/\/\____/\/\/\______/\/\_____/\/\__/\/\__/\/\____/\/\________**
**_/\/\_________/\/\__/\/\__/\/\____/\/\_____/\/\__/\/\__/\/\__/\/\/\/\/\_____**
**____________________________________________________________________________**
**                                                                            **
**----- Author --------------{ PixTillz }-------------------------------------**
**----- File ----------------{ Client.cpp }-----------------------------------**
**----- Created -------------{ 2021-06-15 10:22:55 }--------------------------**
**----- Updated -------------{ 2021-10-01 19:39:45 }--------------------------**
********************************************************************************
*/

#include "../incs/Client.hpp"

// ____________Canonical Form____________
Client::~Client(void) { return; }
// Client(void) { return; } /* default constructor is private */
Client::Client(Client const &src) { *this = src; }
Client  &Client::operator=(Client const &src) {
	static_cast<Connection &>(*this) = static_cast<Connection const &>(src);
	this->_nickname = src.getNickname();
	this->_username = src.getUsername();
	this->_realname = src.getRealname();
	this->_modes = src.getModes();
	return *this;
}

// _____________Constructor______________
Client::Client(Connection *src, NickCommand const &cmd) : Connection(*src), _modes(NOMODE) {
	this->setClient(); //catch FlagException later or catch it higher.
	this->_nickname = cmd.nickname();
	delete src;
}

Client::Client(Connection *src, UserCommand const &cmd) : Connection(*src), _modes(NOMODE) {
	this->setClient(); //catch FlagException later or catch it higher.
	DEBUG_DISPCB(COUT, src, DARK_CYAN, '[');
	this->_username = cmd.username();
	if (!cmd.modes().empty()) //further check later, or a function to set modes better.
		this->_modes = cmd.modes()[0] % 48;
	this->_realname = cmd.realname();
	delete src;
}

// __________Member functions____________
bool				Client::isRegistered(void) const { return (!this->_nickname.empty() && !this->_username.empty() && !this->_realname.empty()); }
std::string const	Client::fullId(void) const {
	std::ostringstream ss;

	if (!this->isRegistered())
		return std::string();
	ss << this->getNickname() << '!' << this->getUsername() << '@' << this->hostname();
	return (ss.str());
}

std::string const	Client::name(void) const {
	if (this->isRegistered())
		return this->getNickname();
	else {
		if (this->getNickname().empty())
			return this->getUsername();
		else
			return this->getNickname();
	}
}

bool				Client::isOperator(void) const { return ((this->_status >> BIT_OPERATOR) & 1); }
void				Client::setOperator(void) { this->setStatusFlag(CONX_OPERATOR); }
void				Client::unsetOperator(void) { this->unsetStatusFlag(CONX_OPERATOR); }

// ____________Setter / Getter___________
// _nickname
std::string const	Client::getNickname(void) const { return this->_nickname; }
void				Client::setNickname(std::string const &src) { this->_nickname = src;}

// _username
std::string const	Client::getUsername(void) const { return this->_username; }
void				Client::setUsername(std::string const &src) { this->_username = src; }

// _realname
std::string const	Client::getRealname(void) const { return this->_realname; }
void				Client::setRealname(std::string const &src) { this->_realname = src; }

// _modes
bool				Client::checkMode(unsigned char mode) const { return (this->_modes & mode); }
unsigned char 		Client::getModes(void) const { return this->_modes; }
void				Client::setModes(unsigned char src) { this->_modes = src; }

std::ostream		&operator<<(std::ostream &flux, Client const &src) {
	flux << static_cast<Connection const &>(src);
	flux << "[" << src.getNickname() << "]";
	flux << "[" << src.getUsername() << "]";
	flux << "[" << src.getRealname() << "]";
	return flux;
}
