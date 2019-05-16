#ifndef MEMPOOL_H
#define MEMPOOL_H

#define MEMPOOL_SIZE 16384


typedef struct memNode_s
{
    void *pAddr;
    int size;
    struct memNode_s *pre;
    struct memNode_s *next;

}memNode,*pmemNode;

typedef struct memQue_s
{
    pmemNode front;
    pmemNode rear;
}memQue,*pmemQue;

typedef struct MEMPOOL_s
{
    memQue usedMem;
    memQue freeMem;
}MEMPOOL,*pMEMPOOL;


int initMempool(pMEMPOOL pmpool);
void *mallocFromPool(int id,int size);
int freeToPool(int id,void *p);
int destroyMempool(pMEMPOOL pmpool);
bool isMempoolFull(int __msqid,int size);
void printMemPool(int id);
#endif // MEMPOOL_H
