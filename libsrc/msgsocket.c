#include "msgsocket.h"
#include "log.h"
#include <sys/time.h>

int client_sockfd[FD_SETSIZE];

void nowtime_ns(bool sts)
{
    static struct timespec ts_old={0,0};
    struct timespec ts={0,0};

    ts.tv_sec = ts_old.tv_sec;
    ts.tv_nsec = ts_old.tv_nsec;
    clock_gettime(CLOCK_MONOTONIC, &ts_old);
    if(true == sts)
    {
        ts.tv_sec =  ts_old.tv_sec-ts.tv_sec;
        ts.tv_nsec =  ts_old.tv_nsec-ts.tv_nsec;
        debug_msg("clock_gettime : tv_sec=%ld, tv_nsec=%ld\n", ts.tv_sec, ts.tv_nsec);
    }
}

/*
 * Create a UNIX-domain socket address in the Linux "abstract namespace".
 *
 * The socket code doesn't require null termination on the filename, but
 * we do it anyway so string functions work.
 */
static int makeAddr(const char* name, struct sockaddr_un* pAddr, socklen_t* pSockLen)
{
    int nameLen = strlen(name);
    if (nameLen >= (int) sizeof(pAddr->sun_path) -1)  /* too long? */
        return -1;
    pAddr->sun_path[0] = '\0';  /* abstract namespace */
    strcpy(pAddr->sun_path+1, name);
    pAddr->sun_family = AF_UNIX;
    *pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
    return 0;
}
 
bool set_tcp_nodelay(int fd)
{
    bool ret = true;
    int yes = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
            ret = false;
        }/**/
    return ret;
}

int server_create(void)
{
	int server_sockfd;
    socklen_t server_len = 0;;
	struct sockaddr_un server_addr;

	//delete the old server socket
    unlink(SERVER_NAME);
	//create socket
	server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    set_tcp_nodelay(server_sockfd);
    makeAddr(SERVER_NAME, &server_addr, &server_len);
	bind(server_sockfd, (struct sockaddr *)&server_addr, server_len); 
	//listen the server
	listen(server_sockfd, 5);
	return server_sockfd;
}

static int client_create()
{
	int sockfd;
    socklen_t len = 0;
	struct sockaddr_un address;
	int conret = 0;

	//create socket 
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    set_tcp_nodelay(sockfd);
	//name the server socket
    makeAddr(SERVER_NAME, &address, &len);
	//connect to server
	conret = connect(sockfd, (struct sockaddr*)&address, len);
	if(-1 == conret)
	{
		perror("opps:client1");
    }
    debug_msg("client_create sockfd = %d\n",sockfd);
	return sockfd;
}

int writeToSocket(int fd ,psocketCmd skcmd,psocketMsg skmsg)
{
	int ret = -1;
	int n = 0;
    if((NULL == skcmd)&&(NULL == skmsg))
    {
        debug_msg("write data all NULL\n");
        return ret;
    }
    if(NULL == skmsg)
    {
       n = sizeof(socketCmd);
       if(NULL != skcmd)ret = write(fd, skcmd, n);
    }
    else
    {
        socketData skDat;
        if(NULL != skcmd)memcpy(&skDat.cmd,skcmd,sizeof(socketCmd));
        if(NULL != skmsg)memcpy(&skDat.msg,skmsg,sizeof(socketMsg));
        n = sizeof(socketData);
        ret = write(fd, &skDat, n);
    }
    nowtime_ns(false);
    //debug_msg("\r\n"__DATE__"-"__TIME__"\r\n");
    //debug_msg("writeToSocket size: %d\n",ret);
	if(ret == n)
	{
#if 1
        debugprintf(skmsg);
        debug_msg("writeToSocket client_fd = %d ,connection is exist \n",fd);
        debug_msg("writeToSocket skcmd->cmd:%d\n",skcmd->cmd);
        debug_msg("writeToSocket skcmd->key:%x\n",skcmd->key);
        debug_msg("writeToSocket skcmd->msqId:%d\n",skcmd->msqId);
        debug_msg("writeToSocket skcmd->sts:%d\n",skcmd->sts);
        debug_msg("writeToSocket skcmd->msgType:%ld\n",skcmd->msgType);
        debug_msg("writeToSocket ok\n");
#endif
	}
    else
    {
        int errsv = errno;
        debug_msg("ret = %d,error code is %d",ret,errsv);
    }
	return ret;
}


int readFromSocket(int fd ,psocketCmd skcmd,psocketMsg skmsg)
{
	int ret = -1;
    int n = 0;
    socketData skDat;
    if((NULL == skcmd)&&(NULL == skmsg))
    {
        debug_msg("write data all NULL\n");
        return ret;
    }
    if ( (n = read(fd, &skDat, sizeof(socketData))) == 0)
	{
        //nowtime_ns(true);
        debug_msg("readFromSocket client_fd = %d ,connection closed by client \n",fd);
	}
    else if(-1 == n)
    {
        //nowtime_ns(true);
        int errsv = errno;
        debug_msg("error code is %d",errsv);
    }
    else if(n == sizeof(socketCmd))
	{
        //nowtime_ns(true);
        debug_msg("\r\n"__DATE__"-"__TIME__"\r\n");
        if(NULL != skcmd)memcpy(skcmd,&skDat.cmd,n);
	}
    else if(n == sizeof(socketData))
    {
        //nowtime_ns(true);
        debug_msg("\r\n"__DATE__"-"__TIME__"\r\n");
        debugprintf(&(skDat.msg));
        if(NULL != skcmd)memcpy(skcmd,&skDat.cmd,sizeof(socketCmd));
        if(NULL != skmsg)memcpy(skmsg,&skDat.msg,sizeof(socketMsg));
    }
    ret = n;
#if 1
    if(ret > 0)
    {
        debug_msg("readFromSocket client_fd = %d ,connection is exist \n",fd);
        debug_msg("readFromSocket skcmd->cmd:%d\n",skcmd->cmd);
        debug_msg("readFromSocket skcmd->key:%x\n",skcmd->key);
        debug_msg("readFromSocket skcmd->msqId:%d\n",skcmd->msqId);
        debug_msg("readFromSocket skcmd->sts:%d\n",skcmd->sts);
        debug_msg("read ok\n");
    }
#endif
    //close(fd);
	return ret;
}
static void client_close(int fd)
{
    debug_msg("client_close fd = %d\n",fd);
    close(fd);
}
static int clientMsgGet(int key,enum SKCMD cmd,bool &exist)
{
	int msqid = -1;
	int n,sockfd;
    sockfd = client_create();
    socketCmd skCmd;
    socketCmd skCmdR;
    memset(&skCmd,0,sizeof(socketCmd));
    memset(&skCmdR,0,sizeof(socketCmd));

    skCmd.key = key;
    skCmd.cmd = cmd;
    n = writeToSocket(sockfd,&skCmd,NULL);
	if(-1!=n)
	{
        debug_msg("clientMsgGet write ok\n");
        debug_msg("clientMsgGet read\n");
        if(0 < readFromSocket(sockfd,&skCmdR,NULL))
        {
            if(skCmdR.cmd == CMD_MSG_GET_CREATE || skCmdR.cmd == CMD_MSG_GET_NOCRTE)
            {
                msqid = skCmdR.msqId;
            }
            if(skCmdR.sts == EXISTED)
            {
                exist = true;
                debug_msg("clientMsgGet que is exist\n");
            }
            else if(skCmdR.sts == NOTEXIST)
            {
                exist = false;
                debug_msg("clientMsgGet que is not exist\n");
            }
        }
    }
    client_close(sockfd);
    debug_msg("clientMsgGet msqid = %d\n",msqid);
	return msqid;
}

int clientMsgGetCreate(int key,bool &exist)
{
    int ret = -1;
    ret = clientMsgGet(key,CMD_MSG_GET_CREATE,exist);
    return ret;
}
int clientMsgGetNoCrte(int key,bool &exist)
{
    int ret = -1;
    ret = clientMsgGet(key,CMD_MSG_GET_NOCRTE,exist);
    return ret;
}
static int clientMsgRcvNoWait(int msqid,long int __msgtyp,enum SKCMD cmd,socketMsg *msg)
{
	int ret = -1;
	int n,sockfd;
    sockfd = client_create();
    socketCmd skCmd;
    socketCmd skCmdR;
    memset(&skCmd,0,sizeof(socketCmd));
    memset(&skCmdR,0,sizeof(socketCmd));

    skCmd.cmd = cmd;
    skCmd.msqId = msqid;
    skCmd.msgType = __msgtyp;
    debug_msg("clientMsgRcvNoWait skCmd.msgType =  %ld\n",skCmd.msgType);
    n = writeToSocket(sockfd,&skCmd,NULL);
	if(-1!=n)
	{
        debug_msg("clientMsgRcvNoWait write ok\n");
	}
    debug_msg("clientMsgRcvNoWait read\n");
    ret = readFromSocket(sockfd,&skCmdR,msg);
    debug_msg("clientMsgRcvNoWait read msg.msgType = %ld,msg.msg =%s\n",msg->msgType,msg->msg);
    if((EMPTY == skCmdR.sts)||(0 == ret))
    {
        ret = -1;
        debug_msg("clientMsgRcvNoWait empty \n");
    }
    client_close(sockfd);
    debug_msg("clientMsgRcvNoWait end \n");
	return ret;
}
static int clientMsgRcvWait(int msqid,long int __msgtyp,enum SKCMD cmd,socketMsg *msg)
{
	int ret = -1;
	int n,sockfd;
    socketCmd skCmd;
    socketCmd skCmdR;

	do{
        sockfd = client_create();
        memset(&skCmd,0,sizeof(socketCmd));
        skCmd.cmd = cmd;
        skCmd.msqId = msqid;
        skCmd.msgType = __msgtyp;
        n = writeToSocket(sockfd,&skCmd,NULL);
		if(-1!=n)
		{
            memset(&skCmdR,0,sizeof(socketCmd));
            ret = readFromSocket(sockfd,&skCmdR,msg);
        }
        client_close(sockfd);
	}
    while(EMPTY== skCmdR.sts);
    if((ERROR== skCmdR.sts)||(0 == ret))
    {
        ret = -1;
        debug_msg("clientMsgRcvWait send msg error\n");
    }
    debug_msg("clientMsgRcvWait read skCmdR.sts=%d, msg->msgType = %ld,msg->msg =%s\n",skCmdR.sts,msg->msgType,msg->msg);
	return ret;
}

int clientMsgRcvNowaitBytype(int msqid,long int __msgtyp,void *msg,size_t __msgsz,bool noerror)
{
    int ret = -1;
    int msgsize = -1;
    socketMsg skmsg;
    memset(&skmsg,0,sizeof(socketMsg));
    ret = clientMsgRcvNoWait(msqid,__msgtyp,CMD_MSG_RECV_NOWT_BYTYPE,&skmsg);
    if(-1 != ret)
    {
        debugprintf(&skmsg);
        msgsize = skmsg.size;
        debug_msg("clientMsgRcvNowaitBytype msgsize = %d,__msgsz =%d,noerror:%s\n",msgsize,(int)__msgsz,(noerror==true)?"true":"false");
        if(msgsize <= (int)__msgsz||((msgsize >(int)__msgsz)&&(noerror==true)))
        {
            *((long int *)msg) = skmsg.msgType;
            memcpy(((char *)msg+sizeof(long int)),&skmsg.msg,__msgsz);
            msgsize = __msgsz;
        }
        else
        {
            msgsize = -1;
        }
    }
    else
    {
        debug_msg("clientMsgRcvNowaitBytype reciev msg error or no this type msg\n");
    }
    return msgsize;
}
int clientMsgRcvNowaitExtype(int msqid,long int __msgtyp,void *msg,size_t __msgsz,bool noerror)
{
    int ret = -1;
    int msgsize = -1;
    socketMsg skmsg;
    memset(&skmsg,0,sizeof(socketMsg));
    ret = clientMsgRcvNoWait(msqid,__msgtyp,CMD_MSG_RECV_NOWT_EXTYPE,&skmsg);
    if(-1 != ret)
    {
        debugprintf(&skmsg);
        msgsize = skmsg.size;
        debug_msg("clientMsgRcvNowaitExtype msgsize = %d,__msgsz =%d,noerror:%s\n",msgsize,(int)__msgsz,(noerror==true)?"true":"false");
        if(msgsize <= (int)__msgsz||((msgsize > (int)__msgsz)&&(noerror==true)))
        {
            *((long int *)msg) = skmsg.msgType;
            memcpy(((char *)msg+sizeof(long int)),&skmsg.msg,__msgsz);
            msgsize = __msgsz;
        }
        else
        {
            msgsize = -1;
        }
    }
    else
    {
        debug_msg("clientMsgRcvNowaitExtype reciev msg error or no this type msg\n");
    }
    return msgsize;
}
int clientMsgRcvWaitBytype(int msqid,long int __msgtyp,void *msg,size_t __msgsz,bool noerror)
{
    int ret = -1;
    int msgsize = -1;
    socketMsg skmsg;
    memset(&skmsg,0,sizeof(socketMsg));
    ret = clientMsgRcvWait(msqid,__msgtyp,CMD_MSG_RECV_WAIT_BYTYPE,&skmsg);
    if(-1 != ret)
    {
        debugprintf(&skmsg);
        msgsize = skmsg.size;
        debug_msg("clientMsgRcvWaitBytype msgsize = %d,__msgsz =%d,noerror:%s\n",msgsize,(int)__msgsz,(noerror==true)?"true":"false");
        if(msgsize <= (int)__msgsz||((msgsize > (int)__msgsz)&&(noerror==true)))
        {
            *((long int *)msg) = skmsg.msgType;
            memcpy(((char *)msg+sizeof(long int)),&skmsg.msg,__msgsz);
            msgsize = __msgsz;
            debug_msg("msgsize = %d\n",msgsize);
        }
        else
        {
            msgsize = -1;
        }
    }
    else
    {
        debug_msg("clientMsgRcvWaitBytype reciev msg error or no this type msg\n");
    }
    return msgsize;
}

int clientMsgRcvWaitExtype(int msqid,long int __msgtyp,void *msg,size_t __msgsz,bool noerror)
{
    int ret = -1;
    int msgsize = -1;
    socketMsg skmsg;
    memset(&skmsg,0,sizeof(socketMsg));
    ret = clientMsgRcvWait(msqid,__msgtyp,CMD_MSG_RECV_WAIT_EXTYPE,&skmsg);
    if(-1 != ret)
    {
        debugprintf(&skmsg);
        msgsize = skmsg.size;
        debug_msg("clientMsgRcvWaitExtype msgsize = %d,__msgsz =%d,noerror:%s\n",msgsize,(int)__msgsz,(noerror==true)?"true":"false");
        if(msgsize <= (int)__msgsz||((msgsize > (int)__msgsz)&&(noerror==true)))
        {
            *((long int *)msg) = skmsg.msgType;
            memcpy(((char *)msg+sizeof(long int)),&skmsg,__msgsz);
            msgsize = __msgsz;
        }
        else
        {
            msgsize = -1;
        }
    }
    else
    {
        debug_msg("clientMsgRcvWaitExtype reciev msg error or no this type msg\n");
    }
    return msgsize;
}


static int clientMsgSendWait(int msqid,socketMsg *msg)
{
	int ret = -1;
	int n,sockfd;
    socketCmd skCmd;
    socketCmd skCmdR;

	do{
        sockfd = client_create();
        memset(&skCmd,0,sizeof(socketCmd));
        skCmd.cmd = CMD_MSG_SEND_WAIT;
        skCmd.msqId = msqid;
        n = writeToSocket(sockfd,&skCmd,msg);
		if(-1!=n)
		{
            debug_msg("clientMsgSendWait write ok\n");
		}
        debug_msg("clientMsgSendWait read\n");
        debug_msg("clientMsgSendWait read\n");
        memset(&skCmdR,0,sizeof(socketCmd));
        ret = readFromSocket(sockfd,&skCmdR,NULL);
        client_close(sockfd);
	}
    while(FULLED == skCmdR.sts);
    if((ERROR== skCmdR.sts)||(0 == ret))
    {
        ret = -1;
        debug_msg("send msg error\n");
    }
	return ret;
}

static int clientMsgSendNoWait(int msqid,socketMsg *msg)
{
	int ret = -1;
	int n,sockfd;
    socketCmd skCmd;
    socketCmd skCmdR;
    memset(&skCmd,0,sizeof(socketCmd));
    memset(&skCmdR,0,sizeof(socketCmd));

    sockfd = client_create();
    skCmd.cmd = CMD_MSG_SEND_NOWT;
    skCmd.msqId = msqid;

    n = writeToSocket(sockfd,&skCmd,msg);
	if(-1!=n)
	{
        debug_msg("clientMsgSendWait write ok\n");
	}
    debug_msg("recvMsgHandler read\n");
    ret = readFromSocket(sockfd,&skCmdR,NULL);
    if(FULLED == skCmdR.sts)ret = -1;
    if((ERROR== skCmdR.sts)||(0 == ret))
    {
        ret = -1;
        debug_msg("send msg error\n");
    }
    client_close(sockfd);
	return ret;
}

int clientMsgSendWaitExt(int msqid,const void *msg,size_t __msgsz)
{
    int ret = -1;
    socketMsg skmsg;
    if(__msgsz>(MAXLINE-1))
    {
        debug_msg("the send msg should below (MAXLINE-1)(%d),cann't send to server\n",(MAXLINE-1));
    }
    else
    {
        skmsg.size = __msgsz;
        skmsg.msgType = *((long int*)msg);
        memcpy(skmsg.msg,((char*)msg+sizeof(long int)),__msgsz);
        ret = clientMsgSendWait(msqid,&skmsg);
        if(-1 == ret)
        {
            debug_msg("reciev msg error or no this type msg\n");
        }
    }


    return ret;
}
int clientMsgSendNoWaitExt(int msqid,const void *msg,size_t __msgsz)
{
    int ret = -1;
    socketMsg skmsg;
    if(__msgsz>(MAXLINE-1))
    {
        debug_msg("the send msg should below (MAXLINE-1)(%d),cann't send to server\n",(MAXLINE-1));
    }
    else
    {
        skmsg.size = __msgsz;
        skmsg.msgType = *((long int*)msg);
        memcpy(skmsg.msg,((char*)msg+sizeof(long int)),__msgsz);
        ret = clientMsgSendNoWait(msqid,&skmsg);
        if(-1 == ret)
        {
            debug_msg("reciev msg error or no this type msg\n");
        }
    }
    return ret;
}

void debugprintf(socketMsg *msg)
{
    msg=msg;
}

void initClientFdArr()
{
    int i;
    for( i = 0;i<FD_SETSIZE;i++)
    {
        client_sockfd[i] = -1;
    }
}

int clearFdInMap(int fd)
{
    int ret = -1;
    int j;
    for(j = 0;j<FD_SETSIZE;j++)
    {
        if ( client_sockfd[j] == fd)
        {
            LOGE("client_sockfd[%d] %d\n",j,client_sockfd[j]);
            client_sockfd[j] = -1;
            break;
        }
    }
    return ret;
}
int checkExistClient()
{
    int ret =1;
    int i;

    for(i = 0;i<FD_SETSIZE;i++)
    {
        if ( client_sockfd[i] != -1)
        {
            break;
        }
    }
    if(i == FD_SETSIZE)
    {
        ret = 0;
    }
    return ret;
}

