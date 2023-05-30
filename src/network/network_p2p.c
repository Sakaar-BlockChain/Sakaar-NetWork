#include "network.h"

struct network_p2p *network_p2p_new(
        struct network_conf config,
        int (*_get)(const struct string_st *str, struct string_st *res),
        int (*_send)(const struct string_st *str)) {
    struct network_p2p *res = skr_malloc(sizeof(struct network_p2p));
    res->config = config;
    res->server = network_server_new(&res->config, _get, _send);
    return res;
}
void network_p2p_free(struct network_p2p *res) {
    if (res == NULL) return;
    network_server_free(res->server);
    skr_free(res);
}


void network_p2p_start(struct network_p2p *res) {
    network_server_start(res->server);
}
void network_p2p_close(struct network_p2p *res) {
    network_server_close(res->server);
}

int network_p2p_set_hosts(struct network_p2p *res, const struct string_st *tlv) {
    return list_set_tlv_self(res->server->hosts, tlv, STRING_TYPE);
}
void network_p2p_get_hosts(const struct network_p2p *res, struct string_st *tlv) {
    list_get_tlv(res->server->hosts, tlv);
}
int network_p2p_connected(struct network_p2p *res) {
    return network_server_connected(res->server);
}

int network_p2p_get(struct network_p2p *res, const struct string_st *msg, struct string_st *res_msg) {
    return network_server_get(res->server, msg, NET_DATA, res_msg);
}
int network_p2p_send(struct network_p2p *res, const struct string_st *msg) {
    network_server_send(res->server, msg, NET_DATA);
    return ERR_SUCCESS;
}
