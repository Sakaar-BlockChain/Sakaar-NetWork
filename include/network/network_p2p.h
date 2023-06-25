#ifndef NETWORK_P2P_H
#define NETWORK_P2P_H

#include "network_conf.h"
#include "network_server.h"

struct network_p2p {
    struct network_conf config;
    struct network_server server;
};

void network_p2p_data_init(struct network_p2p *, struct network_conf,
        int (*)(const struct string_st *, struct string_st *),
        int (*)(const struct string_st *));
void network_p2p_data_free(struct network_p2p *);

void network_p2p_start(struct network_p2p *);
void network_p2p_close(struct network_p2p *);

int network_p2p_set_hosts(struct network_p2p *, const struct string_st *);
void network_p2p_get_hosts(const struct network_p2p *, struct string_st *);
int network_p2p_connected(struct network_p2p *);

int network_p2p_get(struct network_p2p *, const struct string_st *, struct string_st *);
int network_p2p_send(struct network_p2p *, const struct string_st *);


#endif //NETWORK_P2P_H
