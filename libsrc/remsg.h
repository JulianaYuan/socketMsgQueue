#ifndef _MSG_QUE_H_
#define _MSG_QUE_H_
#include <stddef.h>
#include <sys/types.h>
//#include <sys/ipc.h>

#ifndef __key_t_defined
typedef int key_t;
#define __key_t_defined
#endif

#define IPC_PRIVATE  0
#define MODEBITS 9

#define IPC_EXCL  (0x01<<(1+MODEBITS))
#define IPC_CREAT  (0x01<<(2+MODEBITS))

#define IPC_NOWAIT  (0x01<<(0+MODEBITS))
#define MSG_EXCEPT  (0x01<<(3+MODEBITS))
#define MSG_NOERROR (0x01<<(4+MODEBITS))


//int msgctl (int __msqid, int __cmd, struct msqid_ds *__buf);
int skmsgget (key_t __key, int __msgflg);
ssize_t skmsgrcv (int __msqid, void *__msgp, size_t __msgsz,
		       long int __msgtyp, int __msgflg);
int skmsgsnd (int __msqid, const void *__msgp, size_t __msgsz,
		   int __msgflg);


#endif //_MSG_QUE_H_
