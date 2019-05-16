#include <stdlib.h>
#include "remsgque.h"


int initMempool(pMEMPOOL pmpool)
{
    void *p = malloc(MEMPOOL_SIZE);
    debug_msg("initMempool p =%lx  sizeof(memNode) =%ld\n",(long unsigned int)p,sizeof(memNode));
    pmemNode pNode = (pmemNode)malloc(sizeof(memNode));
    pNode->pAddr = p;
    pNode->size = MEMPOOL_SIZE;
    pNode->next = NULL;
    pNode->pre = NULL;
    pmpool->freeMem.front = pmpool->freeMem.rear = pNode;
    pmpool->usedMem.front = pmpool->usedMem.rear = NULL;
    debug_msg("pmpool->freeMem.front = %lx,pmpool->freeMem.rear = %lx\n",(long unsigned int)pmpool->freeMem.front,(long unsigned int)pmpool->freeMem.rear);
    debug_msg("pmpool->usedMem.front = %lx,pmpool->usedMem.rear = %lx\n",(long unsigned int)pmpool->usedMem.front,(long unsigned int)pmpool->usedMem.rear);
    return 0;
}
int tidyMemFragment(int id)
{
    int ret = -1;
    pMEMPOOL pmpool =findmemPoolByQid(id);
    if(NULL == pmpool)
    {
        debug_msg("no this pool\n");
        return ret;
    }
    pmemNode p = pmpool->freeMem.front;
    debug_msg("in\n");
    while (NULL!=p) {
        pmemNode q = pmpool->freeMem.front;
        while(NULL!=q)
         {
             if((void *)((char *)p->pAddr+p->size) == q->pAddr)
             {
                 pmemNode qd = q;
                 q=q->next;
                 p->size = p->size+qd->size;
                 if(NULL!=qd->pre)qd->pre->next = qd->next;
                 if(NULL!=qd->next)qd->next->pre = qd->pre;
                 if(pmpool->freeMem.front == qd)
                 {
                     pmpool->freeMem.front = qd->next;
                 }
                 if(pmpool->freeMem.rear == qd)
                 {
                     pmpool->freeMem.rear = qd->pre;
                 }
                 free(qd);
                 qd=NULL;
                 ret = 0;
             }
             else
             {
                 q=q->next;
             }
        }
         p=p->next;
    }
    return ret;
}
void *mallocFromPool(int id,int size)
{
   void *pret = NULL;
   pMEMPOOL pmpool =findmemPoolByQid(id);
   if(NULL==pmpool)
   {
        debug_msg("NULL==pmpool\n");
        return pret;
   }
   pmemNode p = pmpool->freeMem.front;
   while ((NULL!=p)&&(p->size<size)) {
        p=p->next;
   }
   debug_msg("id=%d,size=%d in\n",id,size);
   if(NULL != p)
   {
      debug_msg("p=%lx\n",(long unsigned int)p);
      pmemNode pNode = (pmemNode)malloc(sizeof(memNode));
      debug_msg("mallocFromPool\n");
      pNode->pAddr = p->pAddr;
      debug_msg("mallocFromPool\n");
      pNode->size = size;
      pNode->next = NULL;
      pNode->pre = NULL;
      debug_msg("mallocFromPool\n");
      if(size == p->size)
      {
          debug_msg("mallocFromPool\n");
         if(NULL != p->pre)p->pre->next = p->next;
         if(NULL != p->next)p->next->pre = p->pre;
         debug_msg("mallocFromPool\n");
         if(pmpool->freeMem.front == p)
         {
             pmpool->freeMem.front = p->next;
         }
         debug_msg("mallocFromPool\n");
         if(pmpool->freeMem.rear == p)
         {
             pmpool->freeMem.rear = p->pre;
         }
         debug_msg("mallocFromPool\n");
         free(p);
         p=NULL;
      }
      else
      {
        debug_msg("mallocFromPool\n");
        p->pAddr = (void *)((char *)p->pAddr+size);
        debug_msg("mallocFromPool\n");
        p->size = p->size-size;
      }
      debug_msg("mallocFromPool\n");
      if(NULL !=pmpool->usedMem.rear)
      {
          pmpool->usedMem.rear->next = pNode;
          pNode->pre = pmpool->usedMem.rear;
          debug_msg("mallocFromPool\n");
      }
      else
      {
          pmpool->usedMem.front=pNode;
      }
      pmpool->usedMem.rear=pNode;
      debug_msg("mallocFromPool\n");
      pret = pNode->pAddr;
      debug_msg("p=%lx\n",(long unsigned int)pret);
   }
   else
   {
        if(-1 != tidyMemFragment(id))
        {
            pret = mallocFromPool(id,size);
        }
        else
        {
            debug_msg("mempool is full P==NULL\n");
        }
   }
   printMemPool(id);
   return pret;
}
int freeToPool(int id,void *p)
{
    int ret =0;

    pMEMPOOL pmpool =findmemPoolByQid(id);
    if(NULL == pmpool)
    {
        debug_msg("no this pool\n");
        return ret;
    }
    pmemNode pNode = pmpool->usedMem.front;

    while ((NULL!=pNode)&&(pNode->pAddr != p)) {
         pNode=pNode->next;
    }
    debug_msg("in\n");
    if(NULL != pNode)
    {
        pmemNode qd = pNode;
        if(NULL!=qd->pre)qd->pre->next = qd->next;
        if(NULL!=qd->next)qd->next->pre = qd->pre;
        if(pmpool->usedMem.front == qd)
        {
            pmpool->usedMem.front = qd->next;
        }
        if(pmpool->usedMem.rear == qd)
        {
            pmpool->usedMem.rear = qd->pre;
        }
        if((NULL == pmpool->freeMem.rear)&&(NULL == pmpool->freeMem.front))
        {
            debug_msg("NULL == pmpool->freeMem.rear\n");
            qd->next = NULL;
            qd->pre = NULL;
            pmpool->freeMem.front = pmpool->freeMem.rear = qd;

        }
        else
        {
            pmpool->freeMem.rear->next = qd;
            qd->pre = pmpool->freeMem.rear;
            pmpool->freeMem.rear=qd;
            pmpool->freeMem.rear->next = NULL;
        }
        ret = 0;
    }
    else
    {
        debug_msg("error:maybe used a wrong function to free this memory!!!!!\n");
    }
    printMemPool(id);
    return ret;
}

int destroyMempool(pMEMPOOL pmpool)
{
    //free real memory which for malloc()
    pmemNode pfreeMinAddrNode = pmpool->freeMem.front;
    pmemNode pusedMinAddrNode = pmpool->usedMem.front;
    while(pfreeMinAddrNode->pAddr>pfreeMinAddrNode->next->pAddr)
    {
        pfreeMinAddrNode = pfreeMinAddrNode->next;
    }
    while(pusedMinAddrNode->pAddr>pusedMinAddrNode->next->pAddr)
    {
        pusedMinAddrNode = pusedMinAddrNode->next;
    }
    if(pusedMinAddrNode->pAddr>pfreeMinAddrNode->pAddr)
    {
        free(pfreeMinAddrNode->pAddr);
    }
    else
    {
        free(pusedMinAddrNode->pAddr);
    }
    //free the 2 que
    while (pmpool->freeMem.front!=NULL) {
        pmpool->freeMem.rear = pmpool->freeMem.front->next;
        free(pmpool->freeMem.front);
        pmpool->freeMem.front = pmpool->freeMem.rear;
    }
    while (pmpool->usedMem.front!=NULL) {
        pmpool->usedMem.rear = pmpool->usedMem.front->next;
        free(pmpool->usedMem.front);
        pmpool->usedMem.front = pmpool->usedMem.rear;
    }
    return 0;
}
bool isMempoolFull(int __msqid,int size)
{
    bool ret = false;
    pMEMPOOL pmpool =findmemPoolByQid(__msqid);
    if(NULL == pmpool)
    {
        debug_msg("no this pool\n");
        return ret;
    }
    pmemNode p = pmpool->freeMem.front;
    while ((NULL!=p)&&(p->size<size)) {
         p=p->next;
    }
    debug_msg("id=%d,size=%d in\n",__msqid,size);
    if(NULL == p)
    {
         if(-1 != tidyMemFragment(__msqid))
         {
             ret = isMempoolFull(__msqid,size);
         }
         else
         {
             ret = true;
             debug_msg("mempool is full P==NULL\n");
         }
    }
    printMemPool(__msqid);
    return ret;
}

void printMemPool(int id)
{
    pMEMPOOL pmpool =findmemPoolByQid(id);
    //pmemNode p = pmpool->freeMem.front;
    debug_msg("pmpool->freeMem.front = %lx,pmpool->freeMem.front->pAddr=%lx,pmpool->freeMem.front->size=%d\n\
              pmpool->freeMem.rear = %lx,pmpool->freeMem.rear->pAddr=%lx,pmpool->freeMem.rear->size=%d\n",
              (long unsigned int)pmpool->freeMem.front,(NULL==pmpool->freeMem.front)?0:(long unsigned int)pmpool->freeMem.front->pAddr,(NULL==pmpool->freeMem.front)?0:pmpool->freeMem.front->size,
              (long unsigned int)pmpool->freeMem.rear,(NULL==pmpool->freeMem.rear)?0:(long unsigned int)pmpool->freeMem.rear->pAddr,(NULL==pmpool->freeMem.rear)?0:pmpool->freeMem.rear->size);
    debug_msg("pmpool->usedMem.front = %lx,pmpool->usedMem.front->pAddr=%lx,pmpool->usedMem.front->size=%d\n\
               pmpool->usedMem.rear = %lx,pmpool->usedMem.rear->pAddr=%lx,pmpool->usedMem.rear->size=%d\n",
              (long unsigned int)pmpool->usedMem.front,(NULL==pmpool->usedMem.front)?0:(long unsigned int)pmpool->usedMem.front->pAddr,(NULL==pmpool->usedMem.front)?0:pmpool->usedMem.front->size,
              (long unsigned int)pmpool->usedMem.rear,(NULL==pmpool->usedMem.rear)?0:(long unsigned int)pmpool->usedMem.rear->pAddr,(NULL==pmpool->usedMem.rear)?0:pmpool->usedMem.rear->size);
    /*while (NULL!=p) {
        if(NULL!=p->pre)printf("p->pre->pAddr = %lx  ",(long unsigned int)p->pre->pAddr);
        else debug_msg("freeMem:\np->pre = NULL  ");
        printf("p->pAddr = %lx  ",(long unsigned int)p->pAddr);
        if(NULL!=p->next)printf("p->next->pAddr = %lx\n",(long unsigned int)p->next->pAddr);
        else debug_msg("p->next = NULL\n");
         p=p->next;
    }
    p = pmpool->usedMem.front;
    while (NULL!=p) {
        if(NULL!=p->pre)printf("p->pre->pAddr = %lx  ",(long unsigned int)p->pre->pAddr);
        else debug_msg("usedMem:\np->pre = NULL  ");
        printf("p->pAddr = %lx  ",(long unsigned int)p->pAddr);
        if(NULL!=p->next)printf("p->next->pAddr = %lx\n",(long unsigned int)p->next->pAddr);
        else debug_msg("p->next = NULL\n");
         p=p->next;
    }*/
}
