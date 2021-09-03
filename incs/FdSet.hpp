#ifndef FT_IRCPP_FDSET_HPP
#define FT_IRCPP_FDSET_HPP

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <sstream>
#include <string>
#include <iostream>
#include <string>
#include <vector>

class FdSet {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~FdSet(void);
	FdSet(void);
	FdSet(FdSet const &src);
	FdSet &operator=(FdSet const &src);

	// __________Member functions____________
	fd_set		*getPtr(void);
	void		addFd(int const &fd);
	void		removeFd(int const &fd);
	void		zeroFd(void);
	bool		checkFd(int const &fd) const;

	// ____________Setter / Getter___________
	// _fds
	fd_set		const &getFds(void) const;
	void		setFds(fd_set const &src);

private: // ####################################################################

	fd_set	_fds;
};

// Debug function
std::ostream &operator<<(std::ostream &flux, FdSet const &src);

#endif //FT_IRCPP_FDSET_HPP
