#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include "network_conf.h"

struct network_client {
    struct network_conf *config;
    int8_t connected;
    socket_t socket;
};


void network_client_data_init(struct network_client *);
void network_client_data_free(struct network_client *);

void network_client_set_config(struct network_client *, struct network_conf *);
void network_client_connect(struct network_client *, struct string_st *);
void network_client_get(struct network_client *, const struct string_st *, int8_t, struct string_st *, int8_t *);
void network_client_send(struct network_client *, const struct string_st *, int8_t, int8_t *);
void network_client_close(struct network_client *);

#endif //NETWORK_CLIENT_H
