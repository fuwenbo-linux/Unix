//
//  main.c
//  5_4server
//
//  
//  Created by FuWenbo on 2020/4/21.
//  Copyright © 2020 FuWenbo. All rights reserved.
//

#include "head.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    //监听描述符号与连接描述符
    //监听是是服务器对外部端口，连接描述符号是建立tcp通路使用
    int listenfd,confd;
    //子进程的pid
    pid_t childpid;
    //等同与uint32_t
    socklen_t client;
    struct sockaddr_in cliaddr,seraddr;
    //拼接一下描述符
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&seraddr, sizeof(seraddr));
    //服务器类型
    seraddr.sin_family = AF_INET;
    //ipv4的地址
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //对应端口号
    seraddr.sin_port = htons(SERV_PORT);
    
    //绑定端口与描述符号
    Bind(listenfd, (SA*) &seraddr, sizeof(seraddr));
    //开始监听
    Listen(listenfd,LISTENQ);
    printf("服务器开始监听!\n");
    for(;;){
        client = sizeof(cliaddr);
        confd = Accept(listenfd, (SA *) &cliaddr, &client);
        if((childpid = Fork()) == 0){
            //关闭父节点与客户端的通路
            Close(listenfd);
            client_echo(confd);
            exit(0);
        }
        Close(confd);
    }
    
    
    return 0;
}
