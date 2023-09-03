#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include "network_conf.h"
struct network_server {
    struct network_conf *config;

    int8_t (*_get)(const struct string_st *, struct string_st *);

    int8_t (*_send)(const struct string_st *);

#ifdef WIN32
    WSADATA wsa;
#else
    pthread_mutex_t *mutex;
#endif
    socket_t _socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    struct address_list_st hosts;

    pthread_t thread;
};

void network_server_data_init(struct network_server *, struct network_conf *,
                              int8_t (*)(const struct string_st *, struct string_st *),
                              int8_t (*)(const struct string_st *));
void network_server_data_free(struct network_server *);


void network_server_start(struct network_server *);
void network_server_close(struct network_server *);

int8_t network_server_connected(struct network_server *);
void network_server_connect(struct network_server *);

int8_t network_server_get(struct network_server *, const struct string_st *, int8_t, struct string_st *);
int8_t network_server_send(struct network_server *, const struct string_st *, int8_t);

#endif //NETWORK_SERVER_H
