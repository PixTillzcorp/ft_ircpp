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
**----- File ----------------{ test_selectmodule.cpp }------------------------**
**----- Created -------------{ 2021-08-25 15:38:48 }--------------------------**
**----- Updated -------------{ 2021-09-03 17:43:35 }--------------------------**
********************************************************************************
*/

#include "../incs/SelectModule.hpp"
#include "../incs/Debug.hpp"

#include <map>

#define MAX_CLIENT 4

Connection *acceptInc(int sock) {
	struct sockaddr_storage recv_addr;
	socklen_t	addr_len;
	int sock_recv;

	addr_len = sizeof(recv_addr);
	DEBUG_DISPC(COUT, "New Connection !", GREEN);
	sock_recv = accept(sock, (struct sockaddr *)&recv_addr, &addr_len);
	return(new Connection(SockInfo((struct sockaddr *)&recv_addr, CONNECTABLE), sock_recv, CONX_NOFLAG));
}

void dispMessage(Connection *sender, Message *msg) {
	DEBUG_DISPC_NOENDL(COUT, sender->name() << std::string(": "), PINK);
	DEBUG_DISPCB(COUT, *msg, PURPLE, '[');
}

void sendAllClient(std::list<Connection *> conxs, Connection *sender, std::string msg) {
	for (std::list<Connection *>::iterator it = conxs.begin(); it != conxs.end(); it++) {
		if ((*it) != conxs.front() && (*it) != sender) {
			(*it)->queueMessage(Message(msg));
		}
	}
}

std::string	getColor(size_t len) {
	if (len == 2)
		return (GREEN);
	if (len == 3)
		return (YELLOW);
	if (len == 4)
		return (RED);
	if (len == 5)
		return (PINK);
	if (len == 6)
		return (PURPLE);
	return (GREY);
}

int main(int argc, char const *argv[])
{
	std::list<Connection *> conxs;
	std::map<Connection *, std::string> color;
	SelectModule			*sm;
	std::string				port;
	std::string				ip_ver;
	std::string				input;

	Message		*tmp;

	if (argc != 3) {
		DEBUG_DISP(COUT, "Usage: ./test_selectmodule port ip_ver[4/6]");
		return (1);
	}
	port = argv[argc - 2];
	ip_ver = argv[argc - 1];
	if (ip_ver.compare("4") && ip_ver.compare("6"))
	{
		DEBUG_DISP(COUT, "Usage: ./test_selectmodule port ip_ver[4/6]");
		return (1);
	}
	try {
		conxs.push_back(new Connection(port, (ip_ver == "4" ? AF_INET : AF_INET6)));
		color[conxs.front()] = BLUE;
		sm = new SelectModule(conxs.front()->sock(), true);
		while (1) {
			DEBUG_DISPC(COUT, "Waiting for select...", YELLOW);
			sm->call(conxs);
			DEBUG_DISPC(COUT, "Select triggered !", GREEN);
			if (sm->checkStd()) {
				std::getline(std::cin, input);
				DEBUG_DISPCB(COUT, input, PINK, '[');
				if (!input.compare("EXIT") || !input.compare("Exit") || !input.compare("exit"))
					break;
				if (!input.compare("PING") || !input.compare("Ping") || !input.compare("ping")) {
					sendAllClient(conxs, nullptr, input);
				}
			}
			for (std::list<Connection *>::iterator it = conxs.begin(); it != conxs.end(); it++) {
				if (sm->checkRfds((*it)->sock())) {
					if ((*it) == conxs.front()) {
						if (conxs.size() < MAX_CLIENT) {
							conxs.push_back(acceptInc(conxs.front()->sock()));
							sm->addFd(conxs.back()->sock());
							color[conxs.back()] = getColor(conxs.size());
						}
					}
					else {
						(*it)->read();
						if ((*it)->hasInputMessage()) {
							tmp = (*it)->getLastMessage();
							dispMessage((*it), tmp);
							sendAllClient(conxs, (*it), std::string(color[(*it)]) + (*it)->name() + std::string(": ") + tmp->getContent() + std::string(NORMAL));
							delete tmp;
							tmp = nullptr;
						}
					}
				}
				if (sm->checkWfds((*it)->sock())) {
					(*it)->write();
				}
			}
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	// while (distant->read()) {
	// 	DEBUG_DISPC(COUT, std::to_string(++read_nbr), DARK_PURPLE);
	// 	if ((tmp = distant->getLastMessage()))
	// 	{
	// 		if (!tmp->compare("PING") || !tmp->compare("Ping") || !tmp->compare("ping")) {
	// 			distant->queueMessage(Message("Pong."));
	// 			while (distant->hasOutputMessage())
	// 				distant->write();
	// 		}
	// 		else if (!tmp->compare("EXIT") || !tmp->compare("Exit") || !tmp->compare("exit")) {
	// 			delete tmp;
	// 			tmp = nullptr;
	// 			break;
	// 		}
	// 		else
	// 			DEBUG_DISPCB(COUT, *tmp, PINK, '[');
	// 		delete tmp;
	// 		tmp = nullptr;
	// 	}
	// }
	return (0);
}
