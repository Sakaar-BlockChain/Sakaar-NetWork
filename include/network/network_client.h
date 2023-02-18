#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include "network_conf.h"

struct network_client {
    struct network_conf *config;
    int connected;
    socket_t socket;
};


struct network_client *network_client_new();
void network_client_free(struct network_client *);

void network_client_set_config(struct network_client *, struct network_conf *);
void network_client_connect(struct network_client *, struct string_st *);
void network_client_get(struct network_client *, const struct string_st *, char, struct string_st *, char *);
void network_client_send(struct network_client *, const struct string_st *, char);
void network_client_close(struct network_client *);

#endif //NETWORK_CLIENT_H
