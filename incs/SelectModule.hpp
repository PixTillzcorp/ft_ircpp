#ifndef FT_IRCPP_SELECT_MODULE_HPP
#define FT_IRCPP_SELECT_MODULE_HPP

#include "../incs/Connection.hpp"
#include "../incs/FdSet.hpp"
#include <list>
#include <unistd.h>
#include <fcntl.h>

class SelectModule {
public: // #####################################################################

	// ____________Canonical Form____________
	virtual ~SelectModule(void);
	SelectModule(void);
	SelectModule(SelectModule const &src);
	SelectModule &operator=(SelectModule const &src);

	// ____________Constructors______________
	SelectModule(int sock, bool std);

	// __________Member functions____________
	void	call(std::list<Connection *> &conxs);
	void	addFd(int sock);
	void	removeFd(int sock);
	bool	checkRfds(int sock) const;
	bool	checkWfds(int sock) const;
	bool	checkStd(void) const;

	// ____________Setter / Getter___________
	// _mfds
	FdSet const		&getMfds(void) const;
	void			setMfds(FdSet const &src);

	// _rfds
	FdSet const		&getRfds(void) const;
	void			setRfds(FdSet const &src);

	// _wfds
	FdSet const		&getWfds(void) const;
	void			setWfds(FdSet const &src);

	// _ufd
	unsigned int	getUfd(void) const;
	void			setUfd(unsigned int src);

	// ______________Exceptions______________
	class SelectException : public std::exception { // Flag set/unset error
	public:
		virtual ~SelectException(void) throw();
		SelectException(void);
		SelectException(SelectException const &src);
		SelectException &operator=(SelectException const &src);
		virtual const char *what() const throw();
	};

	class CloseSocketException : public std::exception { // Flag set/unset error
	public:
		virtual ~CloseSocketException(void) throw();
		CloseSocketException(void);
		CloseSocketException(CloseSocketException const &src);
		CloseSocketException &operator=(CloseSocketException const &src);
		virtual const char *what() const throw();
	};

private: // ####################################################################

	unsigned int	_ufd;
	FdSet			_mfds;
	FdSet			_rfds;
	FdSet			_wfds;
};

#endif //FT_IRCPP_SELECT_MODULE_HPP
