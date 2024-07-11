#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>


void msg_show_attr(int msgid, struct msqid_ds info)
{
    sleep(1);
    int ret = msgctl(msgid, IPC_STAT, &info);
    if(ret == -1)
    {
        printf("消息属性获取失败\n");
        return;
    }
    printf("\n");
    printf("现在队列中的字节数： %ld\n", info.msg_cbytes);
    printf("队列中消息数数： %ld\n", info.msg_qnum);
    printf("最后发送消息的进程： %ld\n", info.msg_qbytes);
    printf("最后接收消息的进程： %d\n", info.msg_lspid);
    printf("最后发送消息的时间： %s\n", ctime(&(info.msg_stime)));
    printf("最后接收消息的时间： %s\n", ctime(&(info.msg_rtime)));
    printf("最后变化时间： %s\n", ctime(&(info.msg_ctime)));
    printf("消息UID是: %d\n", info.msg_perm.uid);
    printf("消息GID是: %d\n", info.msg_perm.gid);
}
int main()
{
    struct msgmbuf
    {
        int mtype;
        char mtext[10];
    }msg_mbuf;
    
    struct msqid_ds msg_info;
    
    char *msgpath = "/ipc/msg";
    key_t key = ftok(msgpath, 'b');
    if(key != -1)
    {
        printf("成功建立KEY\n");
    }
 
    int msg_flags = IPC_CREAT | IPC_EXCL;
    int msg_id = msgget(key, msg_flags | 0x0666);
    if(msg_id != -1)
    {
        printf("消息建立失败\n");
        return 0;
    }

    msg_show_attr(msg_id, msg_info)  //显示消息队列属性
    
    int msg_sendflags = IPC_NOWAIT;
    msg_mbuf.mtype = 10;
    memcpy(msg_mbuf.mtext, "测试消息", sizeof("测试消息"));
    int rets = msgsnd(msg_id, &msg_mbuf, sizeof("测试消息"), msg_sendflags);
    if(rets == -1)
    {
        printf("消息发送失败\n");
        return 0;
    }
    msg_show_attr(msg_id, msg_info);  //显示消息队列属性

    int msg_recvflags = IPC_NOWAIT | MSG_NOERROR;
    int retr  = msgrcv(msg_id, &msg_mbuf, sizeof("测试消息"), 10, msg_recvflags);
    if(retr == -1)
    {
        printf("消息接收失败\n");
        return 0;
    }
    else 
    {
        printf("消息接收成功,长度： %d\n", retr);
    }
    msg_show_attr(msg_id, msg_info);  //显示消息队列属性

    msg_info.msg_perm.uid = 8;
    msg_info.msg_perm.gid = 8;
    msg_info.msg_qbytes = 12345;
    int ret = msgctl(msg_id, IPC_SET, &msg_info);
    if(ret == -1)
    {
        printf("消息属性修改失败\n");
        return 0;
    }
    msg_show_attr(msg_id, msg_info);  //显示消息队列属性

    int ret = msgctl(msg_id, IPC_RMID, NULL);
    if(ret == -1)
    {
        printf("消息删除失败\n");
        return 0;
    }
    return 0;
}