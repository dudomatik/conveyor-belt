#ifndef TELNET_H_
#define TELNET_H_

class Telnet
{
	
public:
	Telnet();
	~Telnet();
	

	
private:
	char output[64];
};

void setLocalIp();
VOID tcpServerWorkTask(int sFd, /* server's socket file descriptor */
char * address, /* client's socket address */
u_short port /* client's socket port */
) ;
STATUS tcpServer(void);


#endif // TELNET_H_
