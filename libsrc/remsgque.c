#include "remsgque.h"

keyQue gKeyQue[QUE_MAX];

void initKeyQueArray()
{
    int i;
    debug_msg("initKeyQueArray\n");
    for( i=0;i<QUE_MAX;i++)
    {
        gKeyQue[i].key = -1;
        gKeyQue[i].que.msqid = -1;
        gKeyQue[i].que.Front = NULL;
        gKeyQue[i].que.Rear = NULL;
        gKeyQue[i].mempool.freeMem.front = NULL;
        gKeyQue[i].mempool.freeMem.rear = NULL;
        gKeyQue[i].mempool.usedMem.front = NULL;
        gKeyQue[i].mempool.usedMem.rear = NULL;
    }
}


int getQidByKey(int key)
{
    int ret = -1;
    int j;
    if(-1 != key)
    {
        for( j=0;j<QUE_MAX;j++)
        {
            if(gKeyQue[j].key == key)
            {
                ret = gKeyQue[j].que.msqid;
                debug_msg("getQidByKey ret=%d\n",ret);
                printfQueue(&gKeyQue[j].que);
                break;
            }
        }
    }
    return ret;
}

int getKeyByQueid(int queid)
{
    int key = -1;
    debug_msg("getKeyByQueid queid =%d\n",queid);
    if(-1 != queid)
    {
        for(int i =0;i<QUE_MAX;i++){
            if(gKeyQue[i].que.msqid == queid)
            {
                key = gKeyQue[i].key;
                break;
            }
        }
    }
    return key;

}

int createQueue(int key)
{
    int queId = -1;
    debug_msg("createQueue key =%d\n",key);

    for(int i =0;i<QUE_MAX;i++){
        debug_msg("gKeyQue[%d].key=%d\n",i,gKeyQue[i].key);
        if(gKeyQue[i].key == -1)//the que is unused
        {
            gKeyQue[i].key = key;
            pMsgNodeQue pque = &gKeyQue[i].que;
            pMEMPOOL pmp = &gKeyQue[i].mempool;
            initMempool(pmp);
            queId = InitQueue(pque);
            if(-1 != queId)
            {
                debug_msg("createQueue key =%d queId=%d success\n",key,queId);
                //setMsgqueAttr(gKeyQue[i].que.msqid,(struct msqid_ds *)0);
            }
            break;
        }
    }
    return queId;

}

MsgNodeQue *findQueByQueid(int qid)
{
    MsgNodeQue *pque = NULL;
    if(-1 != qid)
    {
        for(int i =0;i<QUE_MAX;i++){
            if(gKeyQue[i].que.msqid == qid)
            {
                pque = &gKeyQue[i].que;
                debug_msg("findQueByQueid :%d\n",qid);
                break;
            }
        }
    }
    return pque;
}

void DestroyQueByQueid(int qid)
{
    for(int i =0;i<QUE_MAX;i++){
        if(gKeyQue[i].que.msqid == qid)
        {
            pMsgNodeQue pque = &gKeyQue[i].que;
            pMEMPOOL pmpool = &gKeyQue[i].mempool;
            DestroyQueue(pque);
            destroyMempool(pmpool);
            gKeyQue[i].key = -1;
            debug_msg("findQueByQueid :%d\n",qid);
            break;
        }
    }
}

bool isQueFull(int __msqid,int size)
{
    bool bret = false;
    bret = isMempoolFull(__msqid,size);
    return bret;
}

pMEMPOOL findmemPoolByQid(int id)
{
    pMEMPOOL pmpool=NULL;
    int j;
    if(-1 != id)
    {
        for( j=0;j<QUE_MAX;j++)
        {
            if(gKeyQue[j].que.msqid == id)
            {
                pmpool = &gKeyQue[j].mempool;
                debug_msg("findmemPoolByQid gKeyQue[%d].que.msqid=%d\n",j,gKeyQue[j].que.msqid);
                break;
            }
        }
    }
    return pmpool;
}
