#include "msgsocket.h"
#include "remsgque.h"

pthread_t pthrdsvListenCleint;
pthread_t pthrdsvRecieveMsg;

/*
	return -1:key is not exist in map
	others: key index in map;
*/
int msgQueIdGet(int fd,socketCmd *cmd)
{
    socketCmd skCmd;
    memset(&skCmd,0,sizeof(socketCmd));
    int id = -1;

    if(0 == cmd->key){
        id = createQueue(cmd->key);
    }
    else{
        id = getQidByKey(cmd->key);
        debug_msg("getQidByKey id = %d\n",id);
        if(-1 != id){
            debug_msg("this que and key is exist,send que id to client\n");
            skCmd.sts        = EXISTED;
        }
        else{
            if(CMD_MSG_GET_NOCRTE == cmd->cmd){
                skCmd.sts        = NOTEXIST;
            }
            else{
                id = createQueue(cmd->key);
                skCmd.sts        = NOTEXIST;
            }
        }
    }
    debug_msg("id = %d\n",id);
    skCmd.msqId = id;
    skCmd.key  = cmd->key;
    skCmd.cmd        = cmd->cmd;

     writeToSocket(fd,&skCmd,NULL);
    return 0;
}
static int insertMsgToQue(int __msqid,socketMsg *msg)
{
    debug_msg("inser msg to que __msqid =%d\n",__msqid);
	int ret = 0;
    MsgData msgDat;
    MsgNodeQue *pque = findQueByQueid(__msqid);
	if(NULL == pque)
	{
        debug_msg("insertMsgToQue NULL == pque\n");
		ret = -1;
		return ret;
    }
    msgDat.msgTyp = msg->msgType;
    debugprintf(msg);
    debug_msg("msg->size = %d\n",msg->size);
    msgDat.size = msg->size;
    msgDat.msg = mallocFromPool(__msqid,msgDat.size);
    if(NULL!=msgDat.msg)
    {
        memset(msgDat.msg,0,msgDat.size);
        memcpy(msgDat.msg,msg->msg,msgDat.size);
        debug_msg("msgDat.msg addr %lx\n",(long int)msgDat.msg);
        debug_msg("insertMsgToQue NULL!=msgDat.msg msgDat.msg =%s\n",(char*)msgDat.msg);
    }
    else
    {
        ret = -1;
        debug_msg("insertMsgToQue NULL==msgDat.msg\n");
		return ret;
    }

    ret = InsertQueue(pque,&msgDat);
	return ret;
}
int sendMsgToQue(int fd,socketCmd *cmd,socketMsg *msg)
{
    socketCmd skCmd;
    memset(&skCmd,0,sizeof(socketCmd));
    if(true == isQueFull(cmd->msqId,msg->size)){
        skCmd.sts        = FULLED;
    }
    else{
        if(-1 == insertMsgToQue(cmd->msqId,msg)){
            skCmd.sts        = ERROR;
        }
        else{
            skCmd.sts        = NORMAL;
        }
    }
    skCmd.key  = getKeyByQueid(cmd->msqId);
    skCmd.msqId= cmd->msqId;
    skCmd.cmd        = cmd->cmd;
    writeToSocket(fd,&skCmd,NULL);
    return 0;
}
static int getMsgByType(int msqid,socketMsg *__msgp,long int __msgtyp)//return -1:no Msg, return msg real size
{
	int ret = -1;
    MsgData msgDat;
    memset(&msgDat,0,sizeof(MsgData));
	MsgNodeQue *pque = findQueByQueid(msqid);
	if(NULL == pque)
	{
		return ret;
	}
	pMsgNode p = pque->Front->next;
	if(0 == __msgtyp)
	{
        ret	= DeleteQueue(pque,&msgDat);
	}
	else
	{
        debug_msg("getMsgByType p %lx\n",(long int)p);
		while(p != pque->Rear->next)
		{
			if(p->data.msgTyp == __msgtyp)
			{
                debug_msg("getMsgByType msgDat.msg %lx\n",(long int)(msgDat.msg));
                ret	= DeleteQueByNode(pque,p,&msgDat);
				break;
			}
			p=p->next;
		}
    }
    if(NULL!=msgDat.msg)
    {
        memcpy(__msgp->msg,msgDat.msg,msgDat.size);
        freeToPool(msqid,msgDat.msg);
        msgDat.msg = NULL;
    }
    __msgp->msgType = msgDat.msgTyp;
    __msgp->size = msgDat.size;
    debug_msg("getMsgByType __msgp->size = %d\n",__msgp->size);
	return ret;
}

static int getMsgExcType(int msqid,socketMsg *__msgp,long int __msgtyp)//return -1:no Msg, return msg real size
{
	int ret = -1;
    MsgData msgDat;
    memset(&msgDat,0,sizeof(MsgData));
	MsgNodeQue *pque = findQueByQueid(msqid);
    debug_msg("getMsgExcType\n");
	if(NULL == pque)
	{
		return ret;
	}
	pMsgNode p = pque->Front->next;
	if(0 == __msgtyp)
	{
        ret	= DeleteQueue(pque,&msgDat);
	}
	else
	{
		while(p != pque->Rear->next)
		{
			if(p->data.msgTyp != __msgtyp)
			{
                ret	= DeleteQueByNode(pque,p,&msgDat);
				break;
			}
			p=p->next;
		}
	}
    if(NULL!=msgDat.msg)
    {
        memcpy(__msgp->msg,msgDat.msg,msgDat.size);
        freeToPool(msqid,msgDat.msg);
        msgDat.msg = NULL;
    }
    __msgp->msgType = msgDat.msgTyp;
    __msgp->size = msgDat.size;
    debug_msg("getMsgExcType __msgp->size = %d\n",__msgp->size);
	return ret;
}

int getMsgFromQueByType(int fd,socketCmd *cmd)
{
    debug_msg("getMsgFromQueByType\n");
    socketCmd skCmd;
    socketMsg skMsg;
    memset(&skCmd,0,sizeof(socketCmd));
    memset(&skMsg,0,sizeof(socketMsg));
	int realSz = 0;	
    realSz = getMsgByType(cmd->msqId,&(skMsg),cmd->msgType);
    debugprintf(&skMsg);
	if(-1 == realSz)
	{
        skCmd.sts = EMPTY;
	}
    else
    {
        skCmd.sts = NORMAL;
    }
    skCmd.key  = getKeyByQueid(cmd->msqId);
    skCmd.msqId= cmd->msqId;
    skCmd.cmd        = cmd->cmd;
    writeToSocket(fd,&skCmd,&skMsg);
    debugprintf(&skMsg);
    return 0;
}

int getMsgFromQueExType(int fd,socketCmd *cmd)
{
    debug_msg("getMsgFromQueExType\n");
    socketCmd skCmd;
    socketMsg skMsg;
    memset(&skCmd,0,sizeof(socketCmd));
    memset(&skMsg,0,sizeof(socketMsg));
    int realSz = 0;

    realSz = getMsgExcType(cmd->msqId,&skMsg,cmd->msgType);
	if(-1 == realSz)
	{
        skCmd.sts = EMPTY;
	}
    else
    {
        skCmd.sts = NORMAL;
    }
    skCmd.key  = getKeyByQueid(cmd->msqId);
    skCmd.msqId= cmd->msqId;
    skCmd.cmd        = cmd->cmd;
    writeToSocket(fd,&skCmd,&skMsg);
    debugprintf(&skMsg);
    return 0;
}

void *acceptClient(void *para)
{
	int nready,i;
	int maxi,maxfd,connfd;
	fd_set				rset, allset;
    int socket_fd,client_fd;
    socklen_t client_len;
    struct sockaddr_un client_addr;

	
    socket_fd = *((int*)para);

	maxfd = socket_fd;			/* initialize */
	maxi = -1;					/* index into client[] array */


	FD_ZERO(&allset);
	FD_SET(socket_fd, &allset);
    LOGE("\n\nacceptClient server is listening socket\n");
	while(1)
	{
		rset = allset;
        usleep(20*1000);
        debug_msg("acceptClient socket_fd = %d\n",socket_fd);
		nready = select(socket_fd+1, &rset, NULL, NULL, NULL);
        debug_msg("acceptClient nready = %d\n",nready);
		if (FD_ISSET(socket_fd, &rset)) {	/* new client connection */
            debug_msg("acceptClient listening socket readable\n");
			client_len = sizeof(client_addr);
			connfd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
			//accept(server_sockfd,(struct sockaddr*)&client_addr, &client_len);
			if(connfd < 0)
               perror("acceptClient accept:");
            debug_msg("connfd = %d\n",connfd);
			for ( i = 0; i < FD_SETSIZE; i++)
                if (client_sockfd[i] < 0) {
                    client_sockfd[i] = connfd; /* save descriptor */
                    LOGE("acceptClient new client client_sockfd[%d] = %d\n",i,client_sockfd[i]);
					break;
				}
			if (i == FD_SETSIZE)
                debug_msg("acceptClient too many clients\n");
 
			//FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd; 		/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */
            debug_msg("acceptClient nready = %d\n",nready);
			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}
        debug_msg("acceptClient maxi = %d\n",maxi);
		for ( i = 0; i <= maxi; i++) {	/* check all clients for data */
            if ( (client_fd = client_sockfd[i]) < 0)
				continue;
            debug_msg("acceptClient wait read\n");
		}
	}
    return NULL;
}


void *recvMsg(void * para)
{
	fd_set				rset, allset;
    int nready,i,n,client_fd;
	struct timeval tmot = {0,1000};

    para = para;
	
    debug_msg("recvMsg\n");
	FD_ZERO(&allset);
	while(1)
	{
        usleep(20*1000);
		if(!checkExistClient())continue;
		for ( i = 0; i < FD_SETSIZE; i++)
		{
            if (-1 != client_sockfd[i]) {
                client_fd = client_sockfd[i];
				if (!FD_ISSET(client_fd, &rset))
				{
					FD_SET(client_fd, &allset);
				}
				rset = allset;
				nready = select(client_fd+1, &rset, NULL, NULL, &tmot);
				
				if((0 != nready)&&((-1 != nready)))
				{
                    debug_msg("recvMsg client_fd =%d, nready = %d\n",client_fd,nready);
                    if (FD_ISSET(client_fd, &rset)) {
                        //usleep(20*1000);
                        //socketData skdat;
                        socketCmd skCmd;
                        socketMsg skMsg;
                        memset(&skCmd,0,sizeof(socketCmd));
                        memset(&skMsg,0,sizeof(socketMsg));
                        n = readFromSocket(client_fd,&skCmd,&skMsg);
                        debug_msg("recvMsg size: %d\n",n);
                        if(-1 == n){
                            debug_msg("recvMsg error\n");
                        }
                        else if(0 == n){
                            debug_msg("recvMsg client_fd = %d ,connection closed by client \n",client_fd);
                            close(client_fd);
                            FD_CLR(client_fd, &allset);
                            clearFdInMap(client_fd);
                        }
                        else
                        {
                            debugprintf(&skMsg);
                            if((CMD_MSG_GET_CREATE == skCmd.cmd)||(CMD_MSG_GET_NOCRTE == skCmd.cmd))
                            {
                                msgQueIdGet(client_fd,&skCmd);
                            }
                            else if((CMD_MSG_SEND_NOWT== skCmd.cmd)||(CMD_MSG_SEND_WAIT== skCmd.cmd))
                            {
                                sendMsgToQue(client_fd,&skCmd,&skMsg);
                            }
                            else if((CMD_MSG_RECV_NOWT_BYTYPE== skCmd.cmd)||(CMD_MSG_RECV_WAIT_BYTYPE== skCmd.cmd))
                            {
                                getMsgFromQueByType(client_fd,&skCmd);
                            }
                            else if((CMD_MSG_RECV_NOWT_EXTYPE== skCmd.cmd)||(CMD_MSG_RECV_WAIT_EXTYPE== skCmd.cmd))
                            {
                                getMsgFromQueExType(client_fd,&skCmd);
                            }
                        }
                    }
				}
			}			
		}
    }
    return NULL;
}


int main()
{
    int server_sockfd;

    signal(SIGPIPE,SIG_IGN);
    initClientFdArr();
    initKeyQueArray();
    server_sockfd = server_create();
	int trdret = pthread_create(&pthrdsvListenCleint,NULL,acceptClient,(void*)(&server_sockfd));
	trdret = pthread_create(&pthrdsvRecieveMsg,NULL,recvMsg,NULL);
    trdret = trdret;
	while(1){
        sleep(10);
	};
    debug_msg("main\n");
    return 0;
}

