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
**----- File ----------------{ test_connection.cpp }--------------------------**
**----- Created -------------{ 2021-05-14 18:52:40 }--------------------------**
**----- Updated -------------{ 2021-09-03 16:08:19 }--------------------------**
********************************************************************************
*/

#include "../incs/Connection.hpp"
#include "../incs/Debug.hpp"

Connection *acceptInc(int sock) {
	struct sockaddr_storage recv_addr;
	socklen_t	addr_len;
	int sock_recv;

	addr_len = sizeof(recv_addr);
	DEBUG_DISPC(COUT, "Waiting for connection...", YELLOW);
	sock_recv = accept(sock, (struct sockaddr *)&recv_addr, &addr_len);
	return(new Connection(SockInfo((struct sockaddr *)&recv_addr, CONNECTABLE), sock_recv, CONX_NOFLAG));
}

int main(int argc, char *argv[])
{
	Connection	*local;
	Connection	*distant;
	std::string port;
	std::string ip_ver;
	unsigned int read_nbr = 0;
	Message		*tmp;

	if (argc != 3) {
		DEBUG_DISP(COUT, "Usage: ./test_connection port ip_ver[4/6]");
		return (1);
	}
	port = argv[argc - 2];
	ip_ver = argv[argc - 1];
	if (ip_ver.compare("4") && ip_ver.compare("6"))
	{
		DEBUG_DISP(COUT, "Usage: ./test_connection port ip_ver[4/6]");
		return (1);
	}
	try {
		local = new Connection(port, (ip_ver == "4" ? AF_INET : AF_INET6));
		distant = acceptInc(local->sock());
	}
	catch (Connection::LocalSocketException &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	DEBUG_DISPC(COUT, "Connection established !", GREEN);
	while (distant->read()) {
		DEBUG_DISPC(COUT, std::to_string(++read_nbr), DARK_PURPLE);
		if ((tmp = distant->getLastMessage()))
		{
			if (!tmp->compare("PING") || !tmp->compare("Ping") || !tmp->compare("ping")) {
				distant->queueMessage(Message("Pong."));
				while (distant->hasOutputMessage())
					distant->write();
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
	return (0);
}
