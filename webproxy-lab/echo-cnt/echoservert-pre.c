#include "csapp.h"
#include "sbuf.h"
#define NTHREADS 4
#define SBUFSIZE 16

void echo_cnt(int connfd);
void *thread(void* vargp);

sbuf_t sbuf;

int main(int argc, char **argv){
    int i,listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n",argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);

    sbuf_init(&sbuf, SBUFSIZE);
    for(i = 0;i<NTHREADS;i++){
        Pthread_create(&tid, NULL, thread, NULL);
    }

    while(1){
        clientlen= sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        Getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE,
                    client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n",client_hostname, client_port);
        // sbuf_insert(&sbuf, connfd);
        printf("🔼 inserting connfd %d\n", connfd);
        sbuf_insert(&sbuf, connfd);
        printf("✅ inserted connfd %d\n", connfd);
    }
}

void *thread(void *vargp){
    Pthread_detach(pthread_self());
    while(1){
        // int connfd = sbuf_remove(&sbuf);
        int connfd = sbuf_remove(&sbuf);
        printf("removed connfd %d\n", connfd);
        echo_cnt(connfd);
        Close(connfd);
    }
}