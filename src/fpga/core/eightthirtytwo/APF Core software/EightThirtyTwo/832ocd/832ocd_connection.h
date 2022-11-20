#ifndef OCD832_CONNECTION_H
#define OCD832_CONNECTION_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "832ocd.h"
#include "832util.h"

#define OCD_ADDR "127.0.0.1"
#define OCD_PORT 33581

#define OCD_READ(con,addr) ocd_command((con),DBG832_READ,4,4,0,(addr),0)
#define OCD_WRITE(con,addr,v) ocd_command((con),DBG832_WRITE,8,0,0,(addr),(v))
#define OCD_READREG(con,reg) ocd_command((con),DBG832_READREG,0,4,(reg),0,0)
#define OCD_SINGLESTEP(con) ocd_command((con),DBG832_SINGLESTEP,0,0,0,0,0)
#define OCD_RUN(con) ocd_command((con),DBG832_RUN,0,0,0,0,0)
#define OCD_STOP(con) ocd_command((con),DBG832_STOP,0,0,0,0,0)
#define OCD_BREAKPOINT(con,addr) ocd_command((con),DBG832_BREAKPOINT,4,0,0,(addr),0)
#define OCD_RELEASE(con) ocd_command((con),DBG832_RELEASE,0,1,0,0,0)

struct ocd_connection
{
	char cmdbuffer[12];
	int sock;
	int bridgeconnected;
	int cpuconnected;
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
};

struct ocd_connection *ocd_connection_new();
void ocd_connection_delete(struct ocd_connection *con);
const char *ocd_connect(struct ocd_connection *con,const char *ip,int port);
int ocd_command(struct ocd_connection *con,enum dbg832_op op,int paramcount,int responsecount,int p1,int p2,int p3);
int ocd_uploadfile(struct ocd_connection *con,const char *filename, int addr, enum eightthirtytwo_endian endian);
void ocd_release(struct ocd_connection *con);


#endif

