#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <string>

#define UTILS_MAX_LEN_NICK 9
#define UTILS_MAX_LEN_USER 15
#define UTILS_MAX_LEN_REAL 64

namespace Utils {

	typedef class FailStreamException : public std::exception { // socket setup failed
	public:
		typedef std::exception inherited;

		std::string content;

		virtual ~FailStreamException() throw();
		FailStreamException(FailStreamException const &cpy);
		FailStreamException &operator=(FailStreamException const &cpy);
		FailStreamException(std::string const &content);
		virtual const char *what() const throw();

	private:
		FailStreamException();
	} FailStream;
	
	std::string		nbrToStr(unsigned int nbr) throw(Utils::FailStream);
	unsigned int	strToNbr(std::string const &str) throw(Utils::FailStream);
	std::string		incToken(std::string const &token) throw(Utils::FailStream);
	std::string		getToken(std::string &src, std::string const del);
	bool			validChanName(std::string const &name);
	bool			validNickName(std::string const &name);
	bool			validUserName(std::string const &name);
	bool			validRealName(std::string const &name);
}

#endif //UTILS_HPP
