#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "Utils.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cctype>
#include <algorithm>
#include <sys/stat.h>

class ConfigParser {
public:
	typedef struct stat							stats;
	typedef std::map<std::string, std::string> 	fieldmap;
	typedef fieldmap::iterator					fieldmap_it;
	typedef fieldmap::const_iterator			fieldmap_cit;

	~ConfigParser();
	ConfigParser();
	ConfigParser(ConfigParser const &cpy);
	ConfigParser &operator=(ConfigParser const &cpy);

	ConfigParser(std::string const &filename) throw(FailFileStream);

	void	load(std::string const &filename) throw(FailFileStream);

	fieldmap_cit		cbegin(void) const;
	fieldmap_cit		cend(void) const;
	fieldmap_it			begin(void);
	fieldmap_it			end(void);
	std::string const	operator[](std::string const &str) const;
	bool				empty(void) const;

	fieldmap fields;
	stats	 sfile;

	typedef class FailFileStreamException : public std::exception { // socket setup failed
	public:
		typedef std::exception inherited;

		std::string content;

		virtual ~FailFileStreamException() throw();
		FailFileStreamException(FailFileStreamException const &cpy);
		FailFileStreamException &operator=(FailFileStreamException const &cpy);
		FailFileStreamException(std::string const &content);
		virtual const char *what() const throw();

	private:
		FailFileStreamException();
	} FailFileStream;

private:
	bool		getNextField(std::ifstream &file) throw(FailFileStream);
	void		makeKeyValue(std::string line);
	void		getStats(std::string const &filename, stats *dest) throw(FailFileStream);
	bool		isModified(std::string const &filename) throw(FailFileStream);
	static bool	isNotGraph(int c);
};

std::ostream &operator<<(std::ostream &flux, ConfigParser const &src);

#endif //CONFIG_PARSER_HPP
