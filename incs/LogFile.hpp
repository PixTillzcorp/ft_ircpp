#ifndef LOG_FILE_HPP
#define LOG_FILE_HPP

#include "Connection.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

// ____________Colors____________
#define LOG_BLACK 		"\033[38;5;0m"
#define LOG_NORMAL 		"\033[0m"
/* ~ */
#define LOG_DARKRED 	"\033[38;5;88m"
#define LOG_RED 		"\033[38;5;196m"
/* ~ */
#define LOG_DARKGREEN 	"\033[38;5;28m"
#define LOG_GREEN 		"\033[38;5;46m"
/* ~ */
#define LOG_DARKYELLOW	"\033[38;5;136m"
#define LOG_YELLOW 		"\033[38;5;226m"
/* ~ */
#define LOG_DARKORANGE	"\033[38;5;130m"
#define LOG_ORANGE 		"\033[38;5;202m"
/* ~ */
#define LOG_DARKBLUE 	"\033[38;5;18m"
#define LOG_BLUE 		"\033[38;5;27m"
/* ~ */
#define LOG_DARKPINK 	"\033[38;5;169m"
#define LOG_PINK 		"\033[38;5;207m"
/* ~ */
#define LOG_DARKPURPLE	"\033[38;5;54m"
#define LOG_PURPLE 		"\033[38;5;57m"
/* ~ */
#define LOG_DARKCYAN 	"\033[38;5;37m"
#define LOG_CYAN 		"\033[38;5;45m"
/* ~ */
#define LOG_DARKGREY 	"\033[38;5;240m"
#define LOG_GREY 		"\033[38;5;245m"
// ______________________________

#define LOG_DATE	true
#define LOG_NODATE	false

struct LogFile {
	std::string		fname;
	std::ofstream	ofs;
	std::string		start;

	// ____________Canonical Form____________
	~LogFile();
	LogFile(LogFile const &cpy);
	LogFile &operator=(LogFile const &cpy);

	// ____________Constructors______________
	LogFile(std::string const &filename);

	void append(bool timestamp, std::string const color, std::string const content);

	bool operator!(void) const;

private:
	LogFile();

	std::string date(void);
};

#endif //LOG_FILE_HPP
