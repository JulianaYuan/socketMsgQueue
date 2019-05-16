#include "remsg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define debug_msg(fmt,...) printf("%s[%d]:"fmt,__FILE__,__LINE__,##__VA_ARGS__)

#define TEST 'a'
#define TEST1 'b'

#define MAX_TEXT 16//212994//8192//2147479552  //9612

pthread_t pthrdsend;
pthread_t pthrdrecv;

/*这个结构体是自己定义的，但是有一点通信双方都要一样*/
struct tMsgText
{
    int bStart;
    unsigned char lvlLF;
    unsigned char lvlRF;
    unsigned char lvlMLR;
    unsigned char lvlLR;
    unsigned char lvlRR;
    unsigned char flash;
    unsigned char isShow;
    unsigned char lvlDistance;
    unsigned int steering_angle;
    unsigned char isAUXLineShow;
};

typedef struct stMsgBlock
{
    long id;
    struct tMsgText text;
}tMsgBlock;

struct MsgText
{
    int i;
    int j;
};

typedef struct MsgBlock_s
{
    long id;
    char text[MAX_TEXT];//struct MsgText text;
}MsgBlock;

int testGetQueId(int key_a)
{

    int mid;
    key_t key = key_a;


    //key = ftok("../", TEST);
    //debug_msg("key =%x,IPC_PRIVATE = %x,IPC_CREAT=%x,IPC_EXCL=%x,IPC_CREAT|IPC_EXCL=%x\n",key,IPC_PRIVATE,IPC_CREAT,IPC_EXCL,IPC_CREAT|IPC_EXCL);
    //debug_msg("MSGMAX=%d,MSGMNB=%d,MSGMNI=%d\n",MSGMAX,MSGMNB,MSGMNI);
    //key = ftok("../../", TEST1);
    debug_msg("key =%x\n",key);
    if(key == -1) {
        perror("ftok error");
        exit(1);
    }
    mid = skmsgget(key,IPC_EXCL);
    if(mid < 0){
        debug_msg("mid = %d\n",mid);
        mid = skmsgget(key, IPC_CREAT|0777);
        if(mid == -1) {
            perror("msgget error");
            exit(2);
            }
    }
    return mid;
}

int testSendMsg(int mid, int i,int j)
{
    MsgBlock data;
    char buf[MAX_TEXT] = {0};
    int ret =-1;

    data.id = (long int)j;
    //data.text.i=i;
    //data.text.j=j;
    sprintf(buf,"aa%02d%02dbb",i,j);
    memcpy(data.text, buf, MAX_TEXT);
    //debug_msg("sizeof(struct tMsgText) = %ld\n",MAX_TEXT);
    ret = skmsgsnd(mid, (void *)&data, MAX_TEXT, 0);
    debug_msg("testSendMsg type:%ld %s\n",data.id,data.text);
    //debug_msg("skmsgsnd:type =%ld data.text = (%d,%d))\n",data.id,data.text.i,data.text.j);
    if(ret == -1) {
        debug_msg("msgsnd error\n");
        exit(0);
    }
    return 0;
}

int testRecvMsg(int mid,int typ)
{
    long int type;
    MsgBlock data;
    int ret =-1;
    debug_msg("testRecvMsg\n");
    type = typ;
    ret = skmsgrcv(mid, (void *)&data, MAX_TEXT, type, IPC_NOWAIT);
    if(-1 != ret)
        debug_msg("msgrcv:ret =%d data.text = (%s))\n",ret,data.text);
        //debug_msg("msgrcv:ret =%d data.text = (%d,%d))\n",ret,data.text.i,data.text.j);
    else
        debug_msg("recieve msg error\n");
    return 0;
}
#if 0
int testRemoveQue(int mid)
{
    msgctl(mid,IPC_RMID,0); //删除消息队列
    return 0;
}
#endif
void *sendMsg(void *para)
{
    int qid;
    qid = *((int*)para);
    int i = 0;
    int j = 0;
    while(-1!=qid)
    {
        j = i%100+1;
        testSendMsg(qid,i,j);
        i++;
        usleep(10);
    }
    return NULL;
}
void *recvMsg(void *para)
{
    int qid;
    qid = *((int*)para);
    int i = 0;
    int j = 0;
    while(-1!=qid)
    {
        j = i%100+1;
        testRecvMsg(qid,j);
        i++;
        usleep(10);
    }
    return NULL;
}
int main(int argc, char **argv)
{
	int qid = -1;
#if 0
    qid = testGetQueId(1235);
    int trdret = 0;
    if(argc!=2)
    {
        debug_msg("wrong parameter,1:sendMsg;2:recvMsg\n");
        exit(0);
    }
    if(**argv = 1)
    {
        trdret = pthread_create(&pthrdsend,NULL,sendMsg,(void*)(&qid));
    }else if(**argv = 2)
    {
        trdret = pthread_create(&pthrdrecv,NULL,recvMsg,(void*)(&qid));
    }

    while(1){
        sleep(10);
    };

	//communicate with server socket
#else
	while(1)
	{
		int cmd = 0;
        debug_msg("please enter cmd:\n[1]:get que id\n[2]: send msg to que; \n[3]: receive msg;\n");
        setbuf(stdin, NULL);
        if(scanf("%d",&cmd)!=1)
        {
            debug_msg("error\n");
        }
        debug_msg("while %d\n",cmd);
		switch(cmd)
		{
            case 1: //get msg que
                qid = testGetQueId(1235);
				break;
            case 2://send msg to que
				{
                    debug_msg("send msg to que\n");
                    if(-1 == qid){debug_msg("que is not exist or create fail!\n");break;}
                    int i=1,j=1;
                    while(1)
                    {
                        debug_msg("i=%d\n",i);
                        testSendMsg(qid,i,j);
                        i++;
                        j++;
                        //usleep(20*1000);
                    }
				}
				break;
                case 3://recieve msg from que
                {
                    debug_msg("recieve msg from que\n");
                    if(-1 == qid){debug_msg("que is not exist or create fail!\n");break;}
                    int m = 1,n = 1;
                    n=n;
                    while(1)
                    {
                        testRecvMsg(qid,m);
                        m++;
                        //usleep(20*1000);
                    }
                }
                break;
#if 0
            case 4://delete que
                testRemoveQue(qid);
                qid = -1;
				break;
#endif
			default:
                debug_msg("wrong cmd\n");
				break;

		}
	}
#endif
	exit(0);
}

