#include <string.h>
#include <stdio.h>
#include <mqueue.h>
#include <errno.h>

#define NAME "/acer1"
#define MESSAGE "Hello"
#define nrm 5

int main(int argc, char *argv[])
{
    // create mq  and attribute struct.
    mqd_t my_mq;
    struct mq_attr attr;
    
    // sett attribute
    attr.mq_flags=0;
    attr.mq_msgsize=33; // <<< This on is very important for the size of the massages you send.
    // DO NOT send messages that is larger then 33 bye and DO NOT sett a buffer
    // that is less then 40byte
    attr.mq_curmsgs=0;
    attr.mq_maxmsg=nrm;

    my_mq = mq_open(NAME, O_CREAT | O_RDWR,0644, &attr);            // Create the queue the number is Linux file prop.
    perror("mq_open");
    mq_close(my_mq);                                                // Now we have created the mq close the mq
    my_mq = mq_open(NAME, O_WRONLY,&attr);                          // re open with other attributes.
    for (int i = 0; i < nrm; ++i) {
        // This is the sender passe
        // Verry important that the sise of the message that is going in to the
        // mq is  less then mq_msgsize + 1.
        mq_send(my_mq, MESSAGE, sizeof(MESSAGE) + 1, 1);
       // perror("mq_send");  // uncomment this if you have problem.
    }
    // Now we are finched with the sender phase and there for closes the mq.
    mq_close(my_mq);
    // Re open the mq with readonly attribute.
    my_mq = mq_open(NAME,O_RDONLY, &attr);
    for (int i = 0; i < nrm ; ++i) {
        ssize_t bytesread = 0;      // This variable stores how many bytes that have bean reed from the mq
        char buffer[50];            // This is where the program stores the message, the size of this buffer must be larger then mq_msgsize.
        bytesread = mq_receive(my_mq,buffer, sizeof(buffer), NULL);
        perror("mq_receive");
        printf("message: %s\n", buffer);
    }
    // At he end of the program _ALWAYS_ remember to close and unlink the mq.
    // Other wise you get some funny problems.
    mq_close(my_mq);
    mq_unlink(NAME);
    return 0;
}
