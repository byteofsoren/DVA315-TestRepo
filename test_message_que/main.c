#include <string.h>
#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#define mq_name "/mq_name"
#define size_max 1024
#define SEC 100000
#define _GNU_SOURCE

void* printer(void* data);
void* sender(void* data);
char* itoa(int i, char b[]);

static struct mq_attr attr;
static mqd_t mq_messages;
// Keaping them as globals makes for less head ache.

int main(void)
{
    // init the message que.
    attr.mq_msgsize = size_max;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    attr.mq_maxmsg = 100;
    mode_t omask;
    omask = umask(0);
    mq_messages = mq_open(mq_name,(O_RDONLY| O_CREAT), (S_IRWXU | S_IRWXG | S_IRWXO), &attr);
    umask(omask);
    // Create the threads for senden and reciver.
    perror("mq_messages\n");
    pthread_t printThread;
    pthread_t senderThread;
    pthread_create(&printThread,NULL, printer, NULL);
    pthread_create(&senderThread, NULL, sender, NULL);
    int running = 1;
    while (running) {
        // basicly if you priss space folowed by enter the program terminates.
        char ch;
        ch = getchar();
        if(ch == ' '){
            running = 0;
        }
    }
    mq_close(mq_messages);
    //mq_unlink(mq_messages);
    return 0;
}

void* printer(void* data)
{
    printf("printer()i\n");
    char buffer[size_max + 1];
    // This is the reciver thread.
    int *d = (int*)data; // just stops the linter from complaining about unused variable.
    d = NULL;
    mqd_t mq_printer;
    mq_printer = mq_open(mq_name, O_RDONLY, 0644, &attr);
    while (1) {
        ssize_t temp;
        //if(attr.mq_curmsgs > 0){
            temp = mq_receive(mq_printer,buffer,size_max ,NULL);
            perror("mq_receive");
//          mq_close(mq_messages);
            //printf("Print: temp=%lu\tmessages=%s\tmq=%d\n", temp,  buffer , mq_messages);
       // }else{
            //printf("No messages to retreave!\n");
        //}
        usleep(3*SEC);
    }
}

void* sender(void* data)
{
    printf("sender()\n");
    int *d = (int*)data;
    d = NULL;

    int counter = 0;
    //mq_open(mq_name,O_WRONLY,  mq_messages);  // open mesage queue.
    mqd_t mq_sender;
    mq_sender = mq_open(mq_name, O_WRONLY , 0644, &attr);
    while (1) {
        char BUFFER[size_max + 1];
        strcpy(BUFFER, itoa(counter,BUFFER));
        printf("sender() BUFFER=%s\n", BUFFER);
        int temp = mq_send(mq_sender ,BUFFER ,size_max, 0);
        //perror("mq_send");
        printf("sender: mq_state=%d Sent_to_mq='%s\n'",temp, BUFFER );
        usleep(4*SEC);
        counter++;
     }
}


char* itoa(int i, char b[]){
    // Convrets int to string
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
