#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <string>

#define CHAR_SPECIAL	"[]\\`_^{|}"
#define CHAR_DIGIT 		"0123456789"
#define CHAR_ALPHA_LOW	"abcdefghijklmnopqrstuvwxyz"
#define CHAR_ALPHA_UP	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHAR_ALPHANUM	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CHAR_FORBIDDEN	"\x00\x0a\x0d\x20\x3A"
#define CHAR_NICKNAME	"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\`_^{|}-"
#define CHAR_PREFIX		"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\`_^{|}-!@."
#define EMPTY_STRING	""

#define UTILS_MAX_LEN_NICK 9
#define UTILS_MAX_LEN_USER 15
#define UTILS_MAX_LEN_REAL 63
#define UTILS_MAX_LEN_SERV 63

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
	void			initTokenSeed(unsigned int seed);
	std::string		genToken(size_t len);
	std::string		getToken(std::string &src, std::string const del);
	bool			splitAuthInfo(std::string const &infos, std::string &host, std::string &port, std::string &pass);
	bool			checkNbr(std::string const &nbr);
	bool			validPassword(std::string const &pass);
	bool			validPort(std::string const &port);
	bool			validChanName(std::string const &name);
	bool			validNickName(std::string const &name);
	bool			validUserName(std::string const &name);
	bool			validRealName(std::string const &name);
	bool			validServName(std::string const &name);
	void			clearSpaces(std::string &str, bool all);
}

#endif //UTILS_HPP
