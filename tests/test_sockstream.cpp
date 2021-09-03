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
**----- File ----------------{ test_sockstream.cpp }--------------------------**
**----- Created -------------{ 2021-08-20 17:35:53 }--------------------------**
**----- Updated -------------{ 2021-08-30 20:32:00 }--------------------------**
********************************************************************************
*/

#include "../incs/SockStream.hpp"
#include "../incs/SockInfo.hpp"
#include "../incs/Debug.hpp"

#include <fcntl.h> //open()

int main(int argc, char const *argv[])
{
	struct sockaddr_storage recv_addr;
	socklen_t	addr_len;
	SockStream	*ss = nullptr;
	SockInfo	*si = nullptr;
	Message		*tmp = nullptr;
	int			sock;
	int			sock_recv;
	int			yes = 1;

	if (argc != 2) {
		DEBUG_DISP(COUT, "Usage: ./test_sockstream port");
		return (1);
	}
	si = new SockInfo("", argv[argc - 1], AF_INET, BINDABLE);
	if ((sock = socket(si->family(), si->socktype(), si->protocol())) == -1) {
		DEBUG_DISP(COUT, "Socket function failed.");
		return (1);
	}
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		DEBUG_DISP(COUT, "Setsockopt function failed.");
		return (1);
	}
	if (bind(sock, si->sockaddr(), si->addrLen()) == -1)
	{
		DEBUG_DISP(COUT, "Bind function failed.");
		return (1);
	}
	listen(sock, 1);
	addr_len = sizeof(recv_addr);
	sock_recv = accept(sock, (struct sockaddr *)&recv_addr, &addr_len);
	DEBUG_DISPC(COUT, "Connection established !", GREEN);
	ss = new SockStream(sock_recv);
	while (ss->read()) {
		if ((tmp = ss->getLastMessage()))
		{
			if (!tmp->compare("PING") || !tmp->compare("Ping") || !tmp->compare("ping")) {
				ss->queueMessage(Message("Pong."));
				while (ss->hasOutputMessage())
					ss->write();
			}
			else if (!tmp->compare("EXIT") || !tmp->compare("Exit") || !tmp->compare("exit")) {
				delete tmp;
				tmp = nullptr;
				break;
			}
			else
				DEBUG_DISPCB(COUT, *tmp, PINK, '[');
			delete tmp;
			tmp = nullptr;
		}
	}
	delete ss;
	delete si;
	return 0;
}
