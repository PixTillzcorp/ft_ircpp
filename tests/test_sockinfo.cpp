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
**----- File ----------------{ test_sockinfo.cpp }----------------------------**
**----- Created -------------{ 2021-07-29 18:40:30 }--------------------------**
**----- Updated -------------{ 2021-09-24 17:16:55 }--------------------------**
********************************************************************************
*/

#include "../incs/SockInfo.hpp"
#include "../incs/Debug.hpp"

#include <list>

void		showInfo(SockInfo *src) {
	DEBUG_BAR_DISPC(COUT, '*', 35, ORANGE);
	if (src)
	{
		DEBUG_LDISPCB(COUT, "Host: ", src->getHost(), DARK_ORANGE, '[');
		DEBUG_LDISPCB(COUT, "Port: ", src->getPort(), DARK_ORANGE, '[');
		DEBUG_LDISPCB(COUT, "Canonname: ", src->canonname(), DARK_ORANGE, '[');
		DEBUG_LDISPCB(COUT, "Address string: ", src->addrString(), DARK_ORANGE, '[');
	}
	DEBUG_BAR_DISPC(COUT, '*', 35, ORANGE);
}

int main(void) {
	SockInfo	*si;
	SockInfo	*si_copy;
	std::string port;
	std::list<std::string> hostlist;
	int nbr_info;

	// hostlist.push_back("qweqwdqw.fqwefqwef.fqwefqe"); /*Wrong one*/
	// hostlist.push_back("www.google.fr");
	// hostlist.push_back("www.google.com");
	// hostlist.push_back("www.amazon.com");
	// hostlist.push_back("www.amazon.fr");
	// hostlist.push_back("www.amazon.de");
	hostlist.push_back("localhost");
	hostlist.push_back("fe80::2e:c3fc:4419:d152");
	try {
		while (!hostlist.empty()) {	
			si = new SockInfo(hostlist.front(), port, AF_UNSPEC, BINDABLE);
			nbr_info = si->nbrInfo();
			DEBUG_LDISPB(COUT, "-------> Host name: ", hostlist.front(), '[');
			DEBUG_LDISPB(COUT, "Nbr Info: ", nbr_info, '[');
			for (unsigned int i = 0; i < nbr_info; i++)
			{
				DEBUG_DISPC(COUT, "--------------------------------------------------", DARK_GREY);
				DEBUG_LDISPB(COUT, (si->familyX(i) == AF_INET ? "IPv4" : "IPv6"), si->addrStringX(i), '[');
				DEBUG_LDISPB(COUT, "Canonname: ", si->canonnameX(i), '[');
				if (!port.empty())
					DEBUG_LDISPB(COUT, "Port: ", si->portX(i), '[');
			}
			delete si;
			hostlist.pop_front();
			if (!hostlist.empty())
				DEBUG_DISPC(COUT, "==================================================", DARK_GREEN);
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	DEBUG_DISPC(COUT, "--------------------------------------------------", DARK_GREY);
	si = new SockInfo("www.decathlon.com", port, AF_UNSPEC, CONNECTABLE);
	si_copy = new SockInfo(*si);
	showInfo(si);
	showInfo(si_copy);
	delete si;
	si = nullptr;
	DEBUG_DISPC(COUT, "--------------------------------------------------", DARK_GREY);
	showInfo(si);
	showInfo(si_copy);
	delete si_copy;
	si_copy = nullptr;
	DEBUG_DISPC(COUT, "--------------------------------------------------", DARK_GREY);
	showInfo(si);
	showInfo(si_copy);
	DEBUG_DISPC(COUT, "--------------------------------------------------", DARK_GREY);



	// #########################################################################
	// tests local port;
	// SockInfo	*si;
	// int nbr_info;
	// int port = 0;

	// while (port < 3000) {
	// 	try {
	// 		si = new SockInfo("", std::to_string(port), AF_INET);
	// 	}
	// 	catch (const std::exception& e) {
	// 		std::cout << e.what() << std::endl;
	// 	}
	// 	nbr_info = si->nbrInfo();
	// 	DEBUG_LDISPB(COUT, "Nbr Info: ", nbr_info, '[');
	// 	for (unsigned int i = 0; i < nbr_info; i++)
	// 	{
	// 		DEBUG_LDISPB(COUT, "SockType: ", (si->familyX(i) == AF_INET ? "IPv4" : "IPv6"), '[');
	// 		DEBUG_LDISPB(COUT, "Address: ", si->addrStringX(i), '[');
	// 		DEBUG_LDISPB(COUT, "Port: ", si->portX(i), '[');
	// 	}
	// 	port++;
	// }

	// #########################################################################
	// First test, checking if the bascis works.
	// SockInfo *si;

	// try {
	// 	si = new SockInfo("", "http", AF_INET6);
	// }
	// catch (const std::exception& e) {
	// 	std::cout << e.what() << std::endl;
	// }
	// DEBUG_DISPB(COUT, (si->family() == AF_INET ? "IPv4" : "IPv6"), '[');
	// DEBUG_DISPB(COUT, si->getHost(), '[');
	// DEBUG_DISPB(COUT, si->getPort(), '[');
	// DEBUG_DISPB(COUT, si->addrString(), '[');
	// #########################################################################
	return (0);
}
