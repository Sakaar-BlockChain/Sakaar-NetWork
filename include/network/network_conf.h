#ifndef NETWORK_CONF_H
#define NETWORK_CONF_H

#include "struct.h"
#include "unistd.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "pthread.h"

#define NET_CONNECTIONS     0b00000001
#define NET_DATA            0b00000010
#define NET_REQUEST         0b00000100
#define NET_RESPONSE        0b00001000
#define NET_SEND            0b00010000
#define NET_GET             0b00100000
#define NET_ERROR           0b01000000

struct network_conf {
    int domain;
    int service;
    int protocol;
    unsigned long interface;
    int port;
    int backlog;
};

void network_send(int, const struct string_st *, char);
void network_read(int, struct string_st *, char *);

#endif //NETWORK_CONF_H
