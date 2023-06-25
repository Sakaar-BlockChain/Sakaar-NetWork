#include "network.h"

void network_p2p_data_init(struct network_p2p *res, struct network_conf config,
        int (*_get)(const struct string_st *str, struct string_st *res),
        int (*_send)(const struct string_st *str)) {
    res->config = config;
    network_server_data_init(&res->server, &res->config, _get, _send);
}
void network_p2p_data_free(struct network_p2p *res) {
    if (res == NULL) return;
    network_server_data_free(&res->server);
}


void network_p2p_start(struct network_p2p *res) {
    network_server_start(&res->server);
}
void network_p2p_close(struct network_p2p *res) {
    network_server_close(&res->server);
}

int network_p2p_set_hosts(struct network_p2p *res, const struct string_st *tlv) {
    return address_list_set_tlv(&res->server.hosts, tlv);
}
void network_p2p_get_hosts(const struct network_p2p *res, struct string_st *tlv) {
    address_list_get_tlv(&res->server.hosts, tlv);
}
int network_p2p_connected(struct network_p2p *res) {
    return network_server_connected(&res->server);
}

int network_p2p_get(struct network_p2p *res, const struct string_st *msg, struct string_st *res_msg) {
    return network_server_get(&res->server, msg, NET_DATA, res_msg);
}
int network_p2p_send(struct network_p2p *res, const struct string_st *msg) {
    network_server_send(&res->server, msg, NET_DATA);
    return ERR_SUCCESS;
}
