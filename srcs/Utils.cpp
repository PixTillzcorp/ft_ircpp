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
**----- File ----------------{ Utils.cpp }------------------------------------**
**----- Created -------------{ 2021-12-14 19:24:06 }--------------------------**
**----- Updated -------------{ 2022-01-14 06:33:47 }--------------------------**
********************************************************************************
*/

#include "Utils.hpp"

static bool isAlpha(int c)		{ return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')); }
static bool isDigit(int c)		{ return ('0' <= c && c <= '9'); }
static bool isSpecial(int c)	{ return (('[' <= c && c <= '`') || ('{' <= c && c <= '}')); }

std::string	Utils::nbrToStr(unsigned int nbr) throw(Utils::FailStream) {
	std::stringstream ret;

	ret << nbr;
	if (!ret.good()) {
		if (!ret.eof())
			throw(Utils::FailStream("nbrToStr()\'s stream failed."));
	}
	return ret.str();
}

unsigned int Utils::strToNbr(std::string const &str) throw(Utils::FailStream) {
	std::stringstream ret;
	int nbr = 0;

	if (str.empty())
		return 0;
	ret.str(str);
	ret >> nbr;
	if (!ret.good()) {
		if (!ret.eof())
			throw(Utils::FailStream("strToNbr()\'s stream failed."));
	}
	return static_cast<unsigned int>(nbr);
}

std::string Utils::incToken(std::string const &token) throw(Utils::FailStream) {
	return nbrToStr(strToNbr(token) + 1);
}

std::string	Utils::getToken(std::string &src, std::string const del) {
	std::string token;
	size_t		pos;

	if (src.empty())
		return std::string();
	if ((pos = src.find(del)) == std::string::npos) {
		token = src;
		src.clear();
	} else {
		token = src.substr(0, pos);
		src.erase(0, pos + del.length());
	}
	return token;
}

bool	Utils::checkNbr(std::string const &nbr) { return (nbr.find_first_not_of("0123456789") == std::string::npos); }

bool	Utils::validChanName(std::string const &name) {
	if (name.empty() || name.length() < 2 || name.length() > 50)
		return false;
	if (name[0] != '#' && name[0] != '&')
		return false;
	for (std::string::const_iterator it = name.begin(); it != name.end(); it++) {
		if ((*it) < 33 || (*it) == ',' || (*it) == ':')
			return false;
	}
	return true;
}

bool	Utils::validNickName(std::string const &name) {
	if (name.empty() || name.length() < 3 || name.length() > UTILS_MAX_LEN_NICK)
		return false;
	if (isDigit(name[0]))
		return false;
	for (std::string::const_iterator it = name.begin(); it != name.end(); it++) {
		if (!isAlpha(*it) && !isDigit(*it) && !isSpecial(*it))
			return false;
	}
	return true;
}

bool	Utils::validUserName(std::string const &name) {
	if (name.empty() || name.length() < 3 || name.length() > UTILS_MAX_LEN_USER)
		return false;
	for (std::string::const_iterator it = name.begin(); it != name.end(); it++) {
		if (!isAlpha(*it) && !isDigit(*it) && !isSpecial(*it))
			return false;
	}
	return true;
}

bool	Utils::validRealName(std::string const &name) {
	if (name.empty() || name.length() < 3 || name.length() > UTILS_MAX_LEN_REAL)
		return false;
	for (std::string::const_iterator it = name.begin(); it != name.end(); it++) {
		if (!isAlpha(*it) && !isDigit(*it) && !isSpecial(*it) && *it != ' ')
			return false;
	}
	return true;
}

bool	Utils::validServName(std::string const &name) {
	if (name.empty() || name.length() < 3 || name.length() > UTILS_MAX_LEN_SERV)
		return false;
	if (name[0] == '-')
		return false;
	if (name.find(".-") != std::string::npos)
		return false;
	for (std::string::const_iterator it = name.begin(); it != name.end(); it++) {
		if (!isAlpha(*it) && !isDigit(*it) && *it != '-' && *it != '.')
			return false;
	}
	return true;
}

void	Utils::clearSpaces(std::string &str, bool all) {
	if (str.empty())
		return;
	while(!str.empty() && str[0] == ' ')
		str.erase(0, 1);
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == ' ') {
			while (i + 1 < str.length() && str[i + 1] == ' ')
				str.erase(i, 1);
			if (i + 1 == str.length() || all)
				str.erase(i, 1);
		}
	}
}

// ########################################
// 				 EXECEPTIONS
// ########################################

Utils::FailStream::~FailStreamException(void) throw() { return; }
Utils::FailStream::FailStreamException(void) : content("Utils namespace fonction using streams failed.") { return; }
Utils::FailStream::FailStreamException(std::string const &content) : content(content) { return; }
Utils::FailStream::FailStreamException(FailStream const &cpy) :
	inherited(static_cast<inherited const &>(cpy)), content(cpy.content) { return; }
Utils::FailStream &Utils::FailStream::operator=(FailStream const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	content = cpy.content;
	return *this;
}
const char *Utils::FailStream::what() const throw() {
	return (content.c_str());
}
