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
**----- File ----------------{ ConfigParser.cpp }-----------------------------**
**----- Created -------------{ 2021-12-14 01:24:08 }--------------------------**
**----- Updated -------------{ 2022-01-13 04:01:10 }--------------------------**
********************************************************************************
*/

#include "ConfigParser.hpp"

ConfigParser::~ConfigParser() {}
ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(ConfigParser const &cpy) :
	fields(cpy.fields), sfile(cpy.sfile) { return; }
ConfigParser &ConfigParser::operator=(ConfigParser const &cpy) {
	fields = cpy.fields;
	sfile = cpy.sfile;
	return *this;
}

ConfigParser::ConfigParser(std::string const &filename) throw(ConfigParser::FailFileStream) {
	std::memset(&sfile, 0, sizeof(stats));
	load(filename);
}

ConfigParser::fieldmap_it ConfigParser::begin(void) { return fields.begin(); }
ConfigParser::fieldmap_it ConfigParser::end(void) { return fields.end(); }

ConfigParser::fieldmap_cit ConfigParser::cbegin(void) const { return fields.begin(); }
ConfigParser::fieldmap_cit ConfigParser::cend(void) const { return fields.end(); }

std::string const ConfigParser::operator[](std::string const &str) const {
	ConfigParser::fieldmap_cit it;

	if ((it = fields.find(str)) != fields.end())
		return it->second;
	else
		return std::string();
}

bool	ConfigParser::empty(void) const { return fields.empty(); }

void	ConfigParser::load(std::string const &filename) throw(ConfigParser::FailFileStream) {
	std::ifstream	file(filename);

	if (!file.is_open())
		throw(ConfigParser::FailFileStream("Couldn\'t open ifstream for \'" + filename + "\'. (not found/permission denied)"));
	if (isModified(filename)) {
		getStats(filename, &sfile);
		if (!getNextField(file))
			throw(ConfigParser::FailFileStream("Ifstream set fail/bad bit for \'" + filename + "\' while parsing."));
	}
}

bool	ConfigParser::getNextField(std::ifstream &file) throw(ConfigParser::FailFileStream) {
	std::string line;

	if (!std::getline(file, line)) {
		if (file.eof()) {
			makeKeyValue(line.substr(0, line.find('#')));
			return true;
		}
		return false;
	}
	makeKeyValue(line.substr(0, line.find('#')));
	return getNextField(file);
}

bool	ConfigParser::isNotGraph(int c) { return !isgraph(c); }

void	ConfigParser::getStats(std::string const &filename, stats *dest) throw(ConfigParser::FailFileStream) {
	if (lstat(filename.c_str(), dest) == -1)
		throw(ConfigParser::FailFileStream("lstat() failed for \'" + filename + "\'. (not found/permissions)"));
}

bool	ConfigParser::isModified(std::string const &filename) throw(ConfigParser::FailFileStream) {
	stats			tmp;

	getStats(filename, &tmp);
	if (tmp.st_mtimespec.tv_sec == sfile.st_mtimespec.tv_sec)
		return tmp.st_mtimespec.tv_nsec > sfile.st_mtimespec.tv_nsec;
	return tmp.st_mtimespec.tv_sec > sfile.st_mtimespec.tv_sec;
}

void	ConfigParser::makeKeyValue(std::string line) {
	std::string key;
	size_t	equal;

	if (!line.empty()) {
		line.erase(std::remove_if(line.begin(), line.end(), isNotGraph), line.end());
		if ((equal = line.find('=')) != std::string::npos && equal != line.size()) {
			if (!(key = line.substr(0, equal)).empty())
				fields[key] = line.substr(equal + 1);
		}
	}
}


// ########################################
// 				 EXECEPTIONS
// ########################################

ConfigParser::FailFileStream::~FailFileStreamException(void) throw() { return; }
ConfigParser::FailFileStream::FailFileStreamException(void) : content("Config file\'s parsing failed.") { return; }
ConfigParser::FailFileStream::FailFileStreamException(std::string const &content) : content(content) { return; }
ConfigParser::FailFileStream::FailFileStreamException(FailFileStreamException const &cpy) :
	inherited(static_cast<inherited const &>(cpy)), content(cpy.content) { return; }
ConfigParser::FailFileStream &ConfigParser::FailFileStream::operator=(FailFileStreamException const &cpy) {
	static_cast<inherited &>(*this) = static_cast<inherited const &>(cpy);
	content = cpy.content;
	return *this;
}
const char *ConfigParser::FailFileStream::what() const throw() {
	return (content.c_str());
}

// ########################################
// 					DEBUG
// ########################################

std::ostream &operator<<(std::ostream &flux, ConfigParser const &src) {
	for (ConfigParser::fieldmap_cit it = src.cbegin(); it != src.cend(); it++) {
		flux << "Key [" << it->first << "] value [" << it->second << "]" << std::endl;
	}
	return flux;
}
