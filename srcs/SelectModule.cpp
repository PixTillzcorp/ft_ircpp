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
**----- File ----------------{ SelectModule.cpp }-----------------------------**
**----- Created -------------{ 2021-08-06 16:17:43 }--------------------------**
**----- Updated -------------{ 2021-12-15 19:54:29 }--------------------------**
********************************************************************************
*/

#include "SelectModule.hpp"

// ########################################
// 					PUBLIC
// ########################################

// ____________Canonical Form____________
SelectModule::~SelectModule(void) { return; }
SelectModule::SelectModule(void) { return; }
SelectModule::SelectModule(SelectModule const &src) { *this = src; }
SelectModule &SelectModule::operator=(SelectModule const &src) {
	this->_mfds = src.getMfds();
	this->_rfds = src.getRfds();
	this->_wfds = src.getWfds();
	this->_ufd = src.getUfd();
	return *this;
}

// ____________Constructors______________
SelectModule::SelectModule(int sock, bool std) {
	if (sock && sock >= 0)
	{
		this->_mfds.addFd(sock);
		fcntl(sock, F_SETFL, O_NONBLOCK);
		this->_ufd = sock + 1;
	}
	if (std)
	{
		this->_mfds.addFd(STDIN_FILENO);
		fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
		this->_ufd = (!sock ? STDIN_FILENO : sock) + 1;
	}
}

// __________Member functions____________

void	SelectModule::call(std::list<Connection *> &conxs) {
	std::list<Connection *>::iterator it;

	this->_rfds.zeroFd();
	this->_rfds.setFds(this->_mfds.getFds());
	this->_wfds.zeroFd();
	for (it = conxs.begin(); it != conxs.end(); it++)
	{
		if ((*it)->isFinished() || (*it)->isLink())
			this->_rfds.removeFd((*it)->sock());
		if ((*it)->hasOutputMessage())
			this->_wfds.addFd((*it)->sock());
		else if ((*it)->isFinished()) { // conx we need to end
			this->_mfds.removeFd((*it)->sock());
			try {
				(*it)->end(); // close then delete itself
			} catch (Connection::FailClose &ex) {
				std::cout << ex.what() << std::endl;
			}
			conxs.erase(it--);
		}
	}
	if (select(this->_ufd, this->_rfds.getPtr(), this->_wfds.getPtr(), nullptr, nullptr) == -1)
		throw(SelectModule::SelectException());
}

void	SelectModule::addFd(int sock) {
	if (sock < 0)
		return;
	if (sock >= this->_ufd)
		this->_ufd = sock + 1;
	this->_mfds.addFd(sock);
	fcntl(sock, F_SETFL, O_NONBLOCK);
	return;
}

void	SelectModule::removeFd(int sock) {
	this->_mfds.removeFd(sock);
	return;
}

bool	SelectModule::checkRfds(int sock) const {
	return this->_rfds.checkFd(sock);
}

bool	SelectModule::checkWfds(int sock) const {
	return this->_wfds.checkFd(sock);
}

bool	SelectModule::checkStd(void) const {
	return this->_rfds.checkFd(STDIN_FILENO);
}

// ____________Setter / Getter___________
// _mfds
FdSet const		&SelectModule::getMfds(void) const { return this->_mfds; }
void			SelectModule::setMfds(FdSet const &src) { this->_mfds = src; }

// _rfds
FdSet const		&SelectModule::getRfds(void) const { return this->_rfds; }
void			SelectModule::setRfds(FdSet const &src) { this->_rfds = src; }

// _wfds
FdSet const		&SelectModule::getWfds(void) const { return this->_wfds; }
void			SelectModule::setWfds(FdSet const &src) { this->_wfds = src; }

// _ufd
unsigned int	SelectModule::getUfd(void) const { return this->_ufd; }
void			SelectModule::setUfd(unsigned int src) { this->_ufd = src; }

// ########################################
// 				 EXECEPTIONS
// ########################################

SelectModule::SelectException::~SelectException(void) throw() { return; }
SelectModule::SelectException::SelectException(void) { return; }
SelectModule::SelectException::SelectException(SelectException const &src) : std::exception(static_cast<std::exception const &>(src)) { return; }
SelectModule::SelectException &SelectModule::SelectException::operator=(SelectException const &src) {
	static_cast<std::exception &>(*this) = static_cast<std::exception const &>(src);
	return *this;
}
const char *SelectModule::SelectException::what() const throw() {
	return ("select() function failed.");
}
