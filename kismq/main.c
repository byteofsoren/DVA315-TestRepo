#include <string.h>
#include <stdio.h>
#include <mqueue.h>
#include <errno.h>

#define NAME "/acer1"
#define MESSAGE "Hello"
#define nrm 5

int main(int argc, char *argv[])
{
    mqd_t my_mq;
    struct mq_attr attr;
    attr.mq_flags=0;
    attr.mq_msgsize=33;
    attr.mq_curmsgs=0;
    attr.mq_maxmsg=nrm;

    my_mq = mq_open(NAME, O_CREAT | O_RDWR,0644, &attr);
    perror("mq_open");
    mq_close(my_mq);
    my_mq = mq_open(NAME, O_WRONLY,&attr);
    for (int i = 0; i < nrm; ++i) {
        mq_send(my_mq, MESSAGE, sizeof(MESSAGE) + 1, 1);
        perror("mq_send");
    }
    mq_close(my_mq);
    my_mq = mq_open(NAME,O_RDONLY, &attr);
    for (int i = 0; i < nrm ; ++i) {
        ssize_t bytesread = 0;
        char buffer[50];
        //unsigned int prio = 0;
        bytesread = mq_receive(my_mq,buffer, sizeof(buffer), NULL);
        perror("mq_receive");
        printf("message: %s\n", buffer);
    }
    mq_close(my_mq);
    mq_unlink(NAME);
    return 0;
}
