#ifndef _REMSGQUE_H_
#define _REMSGQUE_H_


typedef struct MsgData_s{
	long int msgTyp;
    int size;
	void *msg;
} MsgData;

typedef struct MsgNode_s{
	MsgData data;
	struct MsgNode_s *next;
} MsgNode,*pMsgNode;


typedef struct MsgNodeQue_s{
	pMsgNode Front;
	pMsgNode Rear;
	int msqid;
} MsgNodeQue,*pMsgNodeQue;


void printfQueue(pMsgNodeQue queue);
int InitQueue(pMsgNodeQue);    
bool IsEmptyQueue(pMsgNodeQue);    //
int InsertQueue(pMsgNodeQue, MsgData *data);
int DeleteQueue(pMsgNodeQue,MsgData* data);
int DeleteQueByNode(pMsgNodeQue queue,pMsgNode pNode,MsgData* data);
void DestroyQueue(pMsgNodeQue);
int TraverseQueue(pMsgNodeQue);    
void ClearQueue(pMsgNodeQue);    
int LengthQueue(pMsgNodeQue);    

#endif //_REMSGQUE_H_
