#include "remsg.h"
#include "log.h"
#include <stdio.h>
#include "msgsocket.h"
#include <string.h>
#if 0
int msgctl (int __msqid, int __cmd, struct msqid_ds *__buf)
{
	int ret = -1;

	if( IPC_STAT == __cmd){
        ret = clientGetMsgqueAttr(__msqid,__buf);
        if(-1 != ret ){
            debug_msg("get message queue header success\n");
		}
		else{
            debug_msg("get message queue header fail\n");
		}
	}
	else if( IPC_SET == __cmd){
        ret = clientSetMsgqueAttr(__msqid,__buf);
        if(-1 != ret ){
            debug_msg("set message queue attr success\n");
		}
		else{
            debug_msg("set message queue attr fail\n");
		}
	}
	else if( IPC_RMID == __cmd){
        debug_msg("msgctl remove que\n");
        clientMsgCtrlRemv(__msqid);
	}
	else{
        debug_msg("msgctl else\n");
	}

	return ret;
}
#endif
int skmsgget (key_t __key, int __msgflg)
{
	int ret = -1;
    bool bExist = false;


    debug_msg("so,msgget __msgflg =%x\n",__msgflg);
	if( IPC_PRIVATE == __key ){
        debug_msg("creat new message queue\n");
        ret = clientMsgGetCreate(__key,bExist);
        debug_msg("return queue id\n");
	}
	else{
		if( 0 == __msgflg){
            debug_msg("get queue id\n");
            ret = clientMsgGetNoCrte(__key,bExist);
            debug_msg("return queue id\n");
		}
        else if(__msgflg&IPC_CREAT){
            debug_msg("__msgflg&IPC_CREAT = %x\n",__msgflg&IPC_CREAT);
            if(__msgflg&IPC_EXCL){
                debug_msg("__msgflg&IPC_EXCL = %x\n",__msgflg&IPC_EXCL);
                ret = clientMsgGetCreate(__key,bExist);
                if(true == bExist)
                {
                    ret = -1;
                    debug_msg("que exist ,return error\n");
                }
            }
            else{
                ret = clientMsgGetCreate(__key,bExist);
            }
        }
		else{
            debug_msg("unknow error\n");
		}
	}

	return ret;
}
ssize_t skmsgrcv (int __msqid, void *__msgp, size_t __msgsz,
		       long int __msgtyp, int __msgflg)
{
	int ret = -1;
    debug_msg("msgrcv __msgtyp=%ld __msgflg = %d\n",__msgtyp,__msgflg);
	if(0 == __msgflg){
        debug_msg("recive msg, if there isn't a message which type is msgtyp,hold and wait the msg coming\n");
        ret = clientMsgRcvWaitBytype(__msqid,__msgtyp,__msgp,__msgsz,false);
	}
	else
	{
		if(IPC_NOWAIT&__msgflg){
            debug_msg("recive msg, if que is empty, return immediately\n");
            if(MSG_EXCEPT& __msgflg){
                debug_msg("recive msg, return the first msg which type is not msgtyp\n");
                if(MSG_NOERROR& __msgflg){
                    ret = clientMsgRcvNowaitExtype(__msqid,__msgtyp,__msgp,__msgsz,true);
                }
                else
                {
                    ret = clientMsgRcvNowaitExtype(__msqid,__msgtyp,__msgp,__msgsz,false);
                }

			}
			else{
                if(MSG_NOERROR& __msgflg){
                    ret = clientMsgRcvNowaitBytype(__msqid,__msgtyp,__msgp,__msgsz,true);
                }
                else
                {
                    ret = clientMsgRcvNowaitBytype(__msqid,__msgtyp,__msgp,__msgsz,false);
                }
			}
						
		}
		else
		{
            if(MSG_EXCEPT& __msgflg){
                if(MSG_NOERROR& __msgflg){
                    ret = clientMsgRcvWaitExtype(__msqid,__msgtyp,__msgp,__msgsz,true);
                }
                else
                {
                    ret = clientMsgRcvWaitExtype(__msqid,__msgtyp,__msgp,__msgsz,false);
                }
			}
			else{
                if(MSG_NOERROR& __msgflg){
                    ret = clientMsgRcvWaitBytype(__msqid,__msgtyp,__msgp,__msgsz,true);
                }
                else
                {
                    ret = clientMsgRcvWaitBytype(__msqid,__msgtyp,__msgp,__msgsz,false);
                }
			}
		}
	}
	return ret;
}
int skmsgsnd (int __msqid, const void *__msgp, size_t __msgsz,
		   int __msgflg)
{
    int ret = -1;
    debug_msg("msgsnd __msgflg = %d\n",__msgflg);
	if(0 == __msgflg){
        debug_msg("send msg, if que is full, hold and wait the msg can be send to que\n");
        ret = clientMsgSendWaitExt(__msqid,__msgp,__msgsz);
	}
	else{
		if(IPC_NOWAIT & __msgflg){
            debug_msg("send msg, if que is full, return immediately\n");
            ret = clientMsgSendNoWaitExt(__msqid,__msgp,__msgsz);
		}
		else{
            ret = clientMsgSendWaitExt(__msqid,__msgp,__msgsz);
		}
	}
	return ret;
}

