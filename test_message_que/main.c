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
    mq_open(mq_name, O_CREAT | O_RDWR, &attr);
    // Create the threads for senden and reciver.
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
    // This is the reciver thread.
    int *d = (int*)data; // just stops the linter from complaining about unused variable.
    d = NULL;
    mq_open(mq_name,O_RDONLY, mq_messages); // open message queue.
    while (1) {
        char* messages;
        messages = (char *) calloc(sizeof(char*),size_max); // Alloc a place for the message
        mq_receive(mq_messages, messages, size_max,0);
        printf("Print %s\n", messages);
        free(messages);
        usleep(300000);
    }
}

void* sender(void* data)
{
    int *d = (int*)data;
    d = NULL;
     int counter = 0;
     mq_open(mq_name,O_WRONLY,  mq_messages);  // open mesage queue.
     while (1) {
         char* str;
         str = (char*)calloc(sizeof(char),size_max);
         str = itoa(counter, str);      // convert to string.
         printf("send: '%s'\n",str );
         mq_send(mq_messages, str, size_max,0);
         usleep(100000);
         counter++;
     }
}


char* itoa(int i, char b[]){
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
