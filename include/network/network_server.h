#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include "network_conf.h"

struct network_server {
    struct network_conf *config;

    void (*_get)(const struct string_st *, struct string_st *);

    int (*_send)(const struct string_st *);

    int _socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    struct list_st *hosts;
};

struct network_server *network_server_new(
        struct network_conf *,
        void (*)(const struct string_st *, struct string_st *),
        int (*)(const struct string_st *));
void network_server_free(struct network_server *);


void network_server_start(struct network_server *);
void network_server_close();

void network_server_connected(struct network_server *);
void network_server_connect(struct network_server *);

void network_server_get(struct network_server *, const struct string_st *, char, struct string_st *);
void network_server_send(struct network_server *, const struct string_st *, char);


#endif //NETWORK_SERVER_H
