#include "reque.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "log.h"

int queId = 1;

void printfQueue(pMsgNodeQue queue)
{
    pMsgNode P = queue->Front->next;
    debug_msg("sizeof(MsgNode) = %ld\n",sizeof(MsgNode));
    //debug_msg("queue->Front =%lx,queue->Rear =%lx,queue->Front->next =%lx,queue->Rear->next =%lx\n",queue->Front,queue->Rear,queue->Front->next,queue->Rear->next);
    while (P != NULL) {
        //debug_msg("printfQueue:P->data.msgTyp = %ld, P->data.size = %d, P->data.msg = %s\n",P->data.msgTyp,P->data.size,(char*)P->data.msg);
        P = P->next;
    }
}

int InitQueue(pMsgNodeQue queue)    //
{
	int ret = 0;
    debug_msg("InitQueue\n");
	queue->Front = queue->Rear = (pMsgNode)malloc(sizeof(MsgNode));    //
    if (queue->Front == NULL) {        //
		ret = -1;
        debug_msg("create queue,memory alloc fail\n");
		return ret;
    }
    queue->Rear->next = NULL;    //
	queue->msqid = queId++;
    debug_msg("creat queue success\n");
	ret = queue->msqid;
	return ret;
}
bool IsEmptyQueue(pMsgNodeQue queue)    //
{
	bool bret = false;
	if(queue->Front == queue->Rear){
		bret = true;
        debug_msg("queue is empty\n");
	}
	else{
		bret = false;
        debug_msg("queue is not empty\n");
	}
	return bret;
}
int InsertQueue(pMsgNodeQue queue, MsgData *data)    //
{
	int ret = 0;

	pMsgNode p = (pMsgNode)malloc(sizeof(MsgNode));    // 
    if(p == NULL){
		ret = -1;
        debug_msg("memory alloc fail,data = %lx\n",(long unsigned int)(&data));
		return ret;
	}

    p->data.msg = data->msg;
    debug_msg("p->data.msg=%lx,data.msg=%lx\n",(long int)p->data.msg,(long int)data->msg);
    debug_msg("p->data.msg=%s,data.msg=%s\n",(char *)(p->data.msg),(char *)(data->msg));
    p->data.size = data->size;
    p->data.msgTyp = data->msgTyp;
	p->next = NULL;
	queue->Rear->next = p;
	queue->Rear = p;
    debug_msg("inser data success\n");
    printfQueue(queue);
	return ret;

}

int DeleteQueue(pMsgNodeQue queue,MsgData* data)    //
{
    int ret = -1;

	if(IsEmptyQueue(queue)){
		ret = -1;
        debug_msg("DeleteQueue que is empty");
		return ret;
	}
	pMsgNode p = queue->Front->next;
    debug_msg("DeleteQueue msg->msg addr = %lx\n",(long int)(p->data.msg));
    data->msg = p->data.msg;
    ret = p->data.size;
    debug_msg("DeleteQueue ret =%d\n",ret);
    data->size = p->data.size;
    data->msgTyp = p->data.msgTyp;
    queue->Front->next = p->next;
    if(queue->Rear == p){
        queue->Rear = queue->Front;
    }
    free(p);
    p=NULL;
    debug_msg("DeleteQueue delete data success\n");
    printfQueue(queue);
	return ret;
}

int DeleteQueByNode(pMsgNodeQue queue,pMsgNode pNode,MsgData* data)    //
{
    int ret = -1;

	if(IsEmptyQueue(queue)){
		ret = -1;
        debug_msg("DeleteQueByNode que is empty");
		return ret;
	}
	pMsgNode p = queue->Front->next;
    pMsgNode ppre = queue->Front;
	while(p!=queue->Rear->next)
	{	
		if(p==pNode)
		{
            debug_msg("msg->msg addr = %lx\n",(long int)(p->data.msg));
            data->msg = p->data.msg;
            ret = p->data.size;
            debug_msg("DeleteQueByNode ret =%d\n",ret);

            data->size = p->data.size;
            data->msgTyp = p->data.msgTyp;
            ppre->next = p->next;
            if(queue->Rear == p){
                queue->Rear = queue->Front;
            }
            free(p);
            p=NULL;
            debug_msg("DeleteQueByNode delete data success\n");
            break;
		}
        else
        {
            ppre = p;
            p=p->next;
        }
	}
    printfQueue(queue);
	return ret;
}


void DestroyQueue(pMsgNodeQue queue)    //
{
	//
	while (queue->Front != NULL) {
		queue->Rear = queue->Front->next;
		free(queue->Front);
		queue->Front = queue->Rear;
	}
	queue->msqid = -1;
    debug_msg("destroy queue ok\n");

}
int TraverseQueue(pMsgNodeQue queue)    //
{
	int ret = 0;
    if (IsEmptyQueue(queue)) {
        ret = -1;
        debug_msg("queue is empty\n");
		return ret;
    }        
    pMsgNode p = queue->Front->next;    //
    debug_msg("traveral result\n");
    while (p != NULL) {
        debug_msg("msgTyp = %ld, msg =%s\n", p->data.msgTyp,(char*)(p->data.msg));
        p = p->next;
    }
	return ret;
}
void ClearQueue(pMsgNodeQue queue)
{
    pMsgNode P = queue->Front->next;
    pMsgNode Q = NULL;
    queue->Rear = queue->Front;
    queue->Front->next = NULL;
    while (P != NULL) {
        Q = P;
        P = P->next;
		free(Q->data.msg);
		Q->data.msg = NULL;
        free(Q);
    }
    debug_msg("clear queue ok\n");

}
int LengthQueue(pMsgNodeQue queue)
{
	int len = 0;
	pMsgNode P = queue->Front->next;

	while (P != NULL) {
		len ++;
        P = P->next;
    }
	return len;
}



