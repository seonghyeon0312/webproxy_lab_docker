#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

void doit(int fd);
void findDomainAndPortFromUri(const char* uri, char * url, char* server, char* serverPort);
void read_requesthdrs(rio_t *rp);

int main(int argc, char **argv)
{
  // printf("%s", user_agent_hdr);
  // return 0;
  int listenfd, connfd;
  char *host, *serverPort;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1)
  {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen); // line:netp:tiny:accept
    
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    
    doit(connfd);  // line:netp:tiny:doit
    Close(connfd); // line:netp:tiny:close
  }
}


/*
 * doit - handle one HTTP request/response transaction
 */
void doit(int fd)
{
  char buf[MAXLINE], method[MAXLINE], url[MAXLINE],version[MAXLINE];
  const char uri[MAXLINE];
  char server[MAXLINE], request[MAXLINE];
  char serverBuf[MAXLINE], serverPort[MAXLINE];
  rio_t rio, serverRio;

  Rio_readinitb(&rio,fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers: \n");
  printf("%s",buf);
  sscanf(buf, "%s %s %s", method, uri, version);
  read_requesthdrs(&rio);

  findDomainAndPortFromUri(uri, url, server, serverPort);
  sprintf(request, "%s %s %s\r\n", method, serverPort, "HTTP/1.0");
  sprintf(request+strlen(request),"Host: %s\r\n", server);
  sprintf(request+strlen(request),"User-Agent: %s\r\n",user_agent_hdr);
  sprintf(request+strlen(request),"Connection: close\r\n");
  sprintf(request+strlen(request),"Proxy-Connection: close\r\n");
  sprintf(request+strlen(request),"\r\n");
  printf("server domain : %s, server port: %s\n", server, serverPort);
  int clientfd = Open_clientfd(url, server);

  Rio_readinitb(&serverRio, clientfd);
  Rio_writen(clientfd, request, strlen(request));

  int result;
  while((result = Rio_readlineb(&serverRio, serverBuf,MAXLINE))>0){
    Rio_writen(fd,serverBuf, result);
  }
  Close(clientfd);
  return;
}

void read_requesthdrs(rio_t *rp){
  char buf[MAXLINE];
  Rio_readlineb(rp,buf,MAXLINE);
  while(strcmp(buf, "\r\n")){
    Rio_readlineb(rp,buf,MAXLINE);
    printf("%s",buf);
  }
  return;
}

void findDomainAndPortFromUri(const char* uri, char* host, char* port, char* path) {
    const char* host_start = strstr(uri, "://");
    host_start = host_start ? host_start + 3 : uri;

    const char* path_start = strchr(host_start, '/');
    if (path_start) {
        strcpy(path, path_start); // /부터 끝까지 경로 복사
    } else {
        strcpy(path, "/");
    }

    char host_port[256] = {0};
    int host_len = path_start ? path_start - host_start : strlen(host_start);
    if (host_len >= sizeof(host_port)) host_len = sizeof(host_port) - 1;
    strncpy(host_port, host_start, host_len);
    host_port[host_len] = '\0';

    char* colon = strchr(host_port, ':');
    if (colon) {
        *colon = '\0';
        strcpy(host, host_port);
        strcpy(port, colon + 1);
    } else {
        strcpy(host, host_port);
        strcpy(port, "80");
    }
}