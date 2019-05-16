#ifndef REMSGQUE_H
#define REMSGQUE_H
#include "reque.h"
#include "mempool.h"
#include "log.h"

#define QUE_MAX 32000

typedef struct keyQue_s
{
    int key;
    MsgNodeQue que;
    MEMPOOL mempool;
}keyQue,*pkeyQue;

extern keyQue gKeyQue[QUE_MAX];


void initKeyQueArray();
int getQidByKey(int key);
int getKeyByQueid(int queid);
int createQueue(int key);
MsgNodeQue *findQueByQueid(int qid);
void DestroyQueByQueid(int qid);
bool isQueFull(int __msqid,int size);
pMEMPOOL findmemPoolByQid(int id);


#endif // REMSGQUE_H
