#ifndef NETWORK_CONF_H
#define NETWORK_CONF_H

#include "basic.h"
#include "unistd.h"

#define NET_CONNECTIONS     0b00000001
#define NET_DATA            0b00000010
#define NET_REQUEST         0b00000100
#define NET_RESPONSE        0b00001000
#define NET_SEND            0b00010000
#define NET_GET             0b00100000
#define NET_ERROR           0b01000000

#ifdef WIN32
#include "winsock2.h"
#include "windows.h"
#define socket_t unsigned
#else
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "pthread.h"
#define socket_t int
#endif

struct network_conf {

    short domain;
    int service;
    int protocol;
    unsigned long _interface;
    int port;
    int backlog;
};

void network_send(socket_t, const struct string_st *, char, char *);
void network_read(socket_t, struct string_st *, char *);


#endif //NETWORK_CONF_H
