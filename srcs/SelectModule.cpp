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
**----- Updated -------------{ 2022-02-10 18:17:46 }--------------------------**
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
	_mfds = src.getMfds();
	_rfds = src.getRfds();
	_wfds = src.getWfds();
	_ufd = src.getUfd();
	return *this;
}

// ____________Constructors______________
SelectModule::SelectModule(int sock, bool std) {
	if (sock && sock >= 0)
	{
		_mfds.addFd(sock);
		fcntl(sock, F_SETFL, O_NONBLOCK);
		_ufd = sock + 1;
	}
	if (std)
	{
		_mfds.addFd(STDIN_FILENO);
		fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
		_ufd = (!sock ? STDIN_FILENO : sock) + 1;
	}
}

// __________Member functions____________

void	SelectModule::call(std::list<Connection *> &conxs, bool purge) {
	std::list<Connection *>::iterator it;

	_rfds.zeroFd();
	if (!purge)
		_rfds.setFds(_mfds.getFds());
	_wfds.zeroFd();
	for (it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it)->isServer())
			static_cast<Server *>(*it)->endDeadLinks();
		if ((*it)->isFinished()) {
			if (!(*it)->isLink())
				_rfds.removeFd((*it)->sock());
			if (!(*it)->hasOutputMessage()) {
				_mfds.removeFd((*it)->sock());
				try { (*it)->end(); }
				catch (Connection::FailClose &ex) { std::cerr << ex.what() << std::endl; }
				conxs.erase(it--);
				continue;
			}
		}
		if ((*it)->hasOutputMessage())
			_wfds.addFd((*it)->sock());
	}
	if (purge && conxs.empty())
		return;
	if (select(_ufd, _rfds.getPtr(), _wfds.getPtr(), nullptr, nullptr) == -1)
		throw(SelectModule::SelectException());
}

void	SelectModule::addFd(int sock) {
	if (sock < 0)
		return;
	if (sock >= _ufd)
		_ufd = sock + 1;
	_mfds.addFd(sock);
	fcntl(sock, F_SETFL, O_NONBLOCK);
	return;
}

void	SelectModule::removeFd(int sock) { _mfds.removeFd(sock); }
bool	SelectModule::checkRfds(int sock) const { return _rfds.checkFd(sock); }
bool	SelectModule::checkWfds(int sock) const { return _wfds.checkFd(sock); }
bool	SelectModule::checkStd(void) const { return _rfds.checkFd(STDIN_FILENO); }

// ____________Setter / Getter___________
// _mfds
FdSet const		&SelectModule::getMfds(void) const { return _mfds; }
void			SelectModule::setMfds(FdSet const &src) { _mfds = src; }

// _rfds
FdSet const		&SelectModule::getRfds(void) const { return _rfds; }
void			SelectModule::setRfds(FdSet const &src) { _rfds = src; }

// _wfds
FdSet const		&SelectModule::getWfds(void) const { return _wfds; }
void			SelectModule::setWfds(FdSet const &src) { _wfds = src; }

// _ufd
unsigned int	SelectModule::getUfd(void) const { return _ufd; }
void			SelectModule::setUfd(unsigned int src) { _ufd = src; }

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
