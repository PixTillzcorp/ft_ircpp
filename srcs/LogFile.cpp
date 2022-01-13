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
**----- File ----------------{ LogFile.cpp }----------------------------------**
**----- Created -------------{ 2021-12-18 17:24:21 }--------------------------**
**----- Updated -------------{ 2022-01-11 02:43:45 }--------------------------**
********************************************************************************
*/

#include "LogFile.hpp"

LogFile::~LogFile() {}
LogFile::LogFile() {}
LogFile::LogFile(LogFile const &cpy) :
	fname(cpy.fname), ofs(cpy.fname, std::ios_base::app), start(cpy.start) { return; }
LogFile &LogFile::operator=(LogFile const &cpy) {
	fname = cpy.fname;
	start = cpy.start;

	if (!ofs.is_open())
		ofs.open(cpy.fname, std::ios_base::app);
	else {
		ofs.close();
		ofs.open(cpy.fname, std::ios_base::app);
	}
	return *this;
}

LogFile::LogFile(std::string const &filename) :
	fname(filename), ofs(fname, std::ios_base::app), start(date()) { return; }

void LogFile::append(bool timestamp, std::string const color, std::string const content) {
	if (!!ofs && !content.empty()) {
		if (timestamp)
			ofs << "[" << date() << "]";
		if (!color.empty())
			ofs << color;
		ofs << content << LOG_NORMAL << std::endl;
	}
}

std::string LogFile::date(void) {
	std::time_t rawtime;
	std::string	str;

	if (std::time(&rawtime) != -1) {
		str = ctime(&rawtime);
		if (!str.empty())
			str.erase(str.end() - 1);
		return str;
	}
	else
		return std::string("N/A");
}

bool	LogFile::operator!(void) const { return (ofs.good() ? false : true); }
