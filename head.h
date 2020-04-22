//
//  head.h
//  5_4server
//
//  Created by FuWenbo on 2020/4/21.
//  Copyright © 2020 FuWenbo. All rights reserved.
//

#ifndef head_h
#define head_h
#define SA  struct sockaddr
#define MAXLINE 4096
#define LISTENQ 1024
#define SERV_PORT 9877
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
    char    buf[MAXLINE];
    vsnprintf(buf, MAXLINE, fmt, ap);
    if (errnoflag)
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",strerror(error));
    strcat(buf, "\n");
    fflush(stdout);     /* in case stdout and stderr are the same */
    //必须立刻刷新stdout让其输出
    fputs(buf, stderr);
    fflush(NULL);       /* flushes all stdio output streams */
}


/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
err_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}
int Socket(int family,int type,int protocol){
    int n;
    if( (n =socket(family, type, protocol)) < 0)
       err_sys("socket error");
    return n;
}
void Bind(int fd,const struct sockaddr *sa,socklen_t salen){
    if(bind(fd,sa,salen) < 0)
        err_sys("bind error");
}

void Listen(int fd, int backlog){
    char    *ptr;
    /*4can override 2nd argument with environment variable */
    if ( (ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);

    if (listen(fd, backlog) < 0)
        err_sys("listen error");
}
int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr){
    int n;

again:
    if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef    EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            goto again;
        else
            err_sys("accept error");
    }
    return(n);
}
pid_t Fork(void){
    pid_t pid;

    if ( (pid = fork()) == -1)
        err_sys("fork error");
    return(pid);
}
void Close(int fd){
    if (close(fd) == -1)
        err_sys("close error");
}
ssize_t writen(int fd,const void *vptr,size_t n){
    size_t nleft;
    ssize_t nwritten;
    const char* ptr;
    ptr = vptr;
    nleft = n ;
    while(nleft>0){
        if((nwritten = write(fd, ptr, nleft))<=0){
            if(nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }
        nleft -= nwritten;
        ptr+=nwritten;
        printf("这次取了读：%d，还剩余:%d\n",nwritten,nleft);
    }
    return n;
}
void
Writen(int fd, void *ptr, size_t nbytes)
{
    if (writen(fd, ptr, nbytes) != nbytes)
        err_sys("writen error");
}

void client_echo(int sockfd){
    ssize_t n;
    char buf[MAXLINE];
again:
    while((n = read(sockfd,buf,MAXLINE)) > 0)
        Writen(sockfd, buf, n);
    if (n<0 && errno == EINTR)
        goto again;
    else if (n<0)
        err_sys("str_echo: read error!");
}

#endif /* head_h */
