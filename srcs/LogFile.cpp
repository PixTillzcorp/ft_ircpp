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
**----- Updated -------------{ 2022-02-21 20:47:51 }--------------------------**
********************************************************************************
*/

#include "LogFile.hpp"

LogFile::~LogFile() {}
LogFile::LogFile() {}
LogFile::LogFile(LogFile const &cpy) :
	fname(cpy.fname), start(cpy.start), stream(nullptr), fd(cpy.fd), err(cpy.err) {
	init();
}
LogFile &LogFile::operator=(LogFile const &cpy) {
	fname = cpy.fname;
	start = cpy.start;
	stream = nullptr;
	fd = cpy.fd;
	err = cpy.err;

	init();
	return *this;
}

LogFile::LogFile(std::string const &filename) :
	fname(filename), start(Utils::date()), stream(nullptr), fd(-1), err(false) {

	init();
}

void LogFile::append(bool timestamp, std::string const color, std::string const content) {
	if (!err && !content.empty()) {
		if (timestamp)
			buf << "= " << Utils::date() << " = ";
		if (!color.empty())
			buf << color << content << LOG_NORMAL << std::endl;
		else
			buf << content << std::endl;
	}
}

void	LogFile::init(void) {
	if (fname.empty())
		setErrorState();
	else if (!(stream = std::fopen(fname.c_str(), "a")))
		setErrorState();
	else if ((fd = fileno(stream)) < 0)
		setErrorState();
	else
		fcntl(fd, F_SETFL, O_NONBLOCK);
}

void	LogFile::end(void) {
	std::string tmp;

	if (!err) {
		while (!(tmp = Utils::readSomeCXX(buf, 64)).empty())
			Utils::writeSomeC(stream, tmp);
	}
	std::fclose(stream);
}

void	LogFile::setErrorState(void) { err = true; }

bool	LogFile::operator!(void) const { return !err; }
