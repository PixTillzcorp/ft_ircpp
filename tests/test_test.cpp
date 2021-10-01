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
**----- File ----------------{ test_test.cpp }--------------------------------**
**----- Created -------------{ 2021-05-07 12:43:55 }--------------------------**
**----- Updated -------------{ 2021-10-01 14:26:17 }--------------------------**
********************************************************************************
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <vector>
#include <map>
#include <stdarg.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include <unistd.h>
#include <err.h>

#include "../incs/Debug.hpp"

typedef struct addrinfo		t_addrinfo;
typedef struct sockaddr		t_saddr;
typedef struct sockaddr_in	t_saddr_in;

#define READ_SIZE 64

int main(int argc, char **argv) {
	std::ifstream file("Goals.txt");
	std::string buff;

	if (file.fail())
		std::cout << "failed to open file." << std::endl;
	else {
		while (!file.eof()) {
			std::getline(file, buff);
			std::cout << buff << std::endl;
		}
	}

	// ##############################################################
	// std::string str("test[\r]jeiasd[\r]adqwd[\r]dqw[\r]gter");

	// std::cout << str << std::endl;

	// str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

	// std::cout << str << std::endl;

	// ##############################################################
	// std::map<int, std::string> m;

	// m[0] = "hello";

	// m[0] = "taxi";

	// std::cout << m[0] << std::endl;

	// char myname[_SC_HOST_NAME_MAX];

	// gethostname(myname, _SC_HOST_NAME_MAX);

	// std::cout << myname << std::endl;
	// ##############################################################
	// std::string	test = "0.1.10.450";
	// std::string	ret;
	// std::string	tmp;
	// size_t last = 0;
	// size_t next = 0;

	// while (next != std::string::npos)
	// {
	// 	next = test.find('.', (last ? last : 1));
	// 	tmp = test.substr((last ? last : 0), next - last);
	// 	DISPB(tmp, '[');
	// 	ret += tmp;
	// 	std::cout << "last[" << last << "] | next[" << next << "]" << std::endl;
	// 	last = next + 1;
	// }
	// DISPB(ret, '[');
	// ##############################################################
	// std::string test = "he:llo wo:rld !";
	// std::string sub;
	// size_t len;

	// std::cout << test + ":yo" << std::endl;

	// while (!test.empty())
	// {
	// 	len = test.find(':');
	// 	sub = test.substr(0, len);
	// 	test.erase(0, (len == std::string::npos ? len : len + 1));
	// 	DISPB(sub, '[');
	// }

	// ##############################################################
	// std::string test = "hello world !";
	// std::string del = " ";
	// std::string res;

	// while (!test.empty())
	// {
	// 	res = test.substr(0, test.find(del));
	// 	test.erase(0, test.find(del));
	// 	if (!test.empty())
	// 		test.erase(0, 1);
	// 	DISPB(res, '[');
	// }
	// ##############################################################
	// std::list<std::string> lst;

	// lst.push_back("hello");
	// lst.push_back(" ");
	// lst.push_back("world!");

	// std::list<std::string>::iterator it = lst.begin();

	// for (;it != lst.end(); it++)
	// 	COUT *it;
	// JUMP;
	// ##############################################################

	// std::string result("test string\n");
	// std::string tmp;
	// std::stringstream ss(result);

	// while (!ss.eof())
	// {
	// 	std::getline(ss, tmp);
	// 	std::cout << "[" + tmp + "]" << std::endl;
	// }

	// std::cout << ss.eof() << std::endl;
	// t_saddr_in	test;
	// std::string addr = std::string(inet_ntoa(test.sin_addr));
	// std::string port = std::to_string(ntohs(test.sin_port));

	// DISP(result);
	// DISPB(result, '[');
	// while (!ss.eof())
	// {
	// 	info.push_back(std::string());
	// 	std::getline(ss, info.back(), ':');
	// 	DISPB(info.back(), '[');
	// }
	// std::cout << ss.str() << std::endl;


	// std::cout << addr << ":" << port <<std::endl;
	// std::string test = "abcedfghijklmnop";
	// std::string tmp;
	// char const str[100] = "Hello - salute\neren Jeager";
	// std::queue<int> queue;

	// std::cout << "test: [" << test << "]" << std::endl;
	// tmp = test.substr(0,3);
	// test.erase(0,100);
	// std::cout << "tmp: [" << tmp << "]" << std::endl;
	// std::cout << "test: [" << test << "]" << std::endl;

	// std::cout << "------------------------------------" << std::endl;
	// queue.push(10);
	// queue.push(9);
	// queue.push(8);
	// queue.push(7);
	// queue.push(6);
	// queue.push(5);
	// queue.push(4);
	// queue.push(3);
	// queue.push(2);
	// queue.push(1);
	// queue.push(0);

	// while (!queue.empty())
	// {
	// 	std::cout << "First = " << queue.front() << std::endl;
	// 	queue.pop();
	// }
	// ss.str(str);
	// std::cout << ss.str() << std::endl;

	// getline(ss, result);
	// std::cout << result << std::endl;
	// result.clear();
	// std::cout << result << std::endl;
	// if (ss.eof())
	// 	std::cout << "stream met EOF !" << std::endl;
	// else
	// {
	// 	std::cout << "rest: [";
	// 	while (ss.eof() == false)
	// 	{
	// 		getline(ss, result);
	// 		std::cout << result;
	// 	}
	// 	std::cout << "]" <<std::endl;
	// }
	// while (1)
	// {
	// 	std::cout << "[+] ";
	// 	std::cin >> input;
	// 	input.read(buff.c_str(), READ_SIZE);
	// 	// std::cout << "You entered: [" << buff << "]" << std::endl;
	// 	if (!buff.compare("Exit"))
	// 		break;
	// 	else
	// 		std::cout << "Unknown command." << std::endl;
	// }

	return (0);
}
