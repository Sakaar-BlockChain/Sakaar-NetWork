#include <stdio.h>
#include "network.h"

void network_client_data_init(struct network_client *res) {
    res->config = NULL;
    res->connected = 0;
    res->socket = 0;
}
void network_client_data_free(struct network_client *res) {
    if (res->connected) network_client_close(res);
}

void network_client_set_config(struct network_client *res, struct network_conf *config) {
    if (res == NULL || config == NULL) return;
    res->config = config;
    res->socket = socket(config->domain, config->service, config->protocol);
#ifdef WIN32
    DWORD timeout = timeout_in_seconds * 1000;
    setsockopt(res->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(DWORD));
#else
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(res->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));
#endif
}
void network_client_connect(struct network_client *res, struct string_st *address) {
    struct sockaddr_in server_address = {};
    server_address.sin_family = res->config->domain;
    server_address.sin_port = htons(res->config->port);
#ifdef WIN32
    server_address.sin_addr.s_addr = inet_addr(address->data);
#else
    server_address.sin_addr.s_addr = (int) res->config->_interface;
    inet_pton(res->config->domain, address->data, &server_address.sin_addr);
#endif
    int _con = connect(res->socket, (struct sockaddr *) &server_address, sizeof(server_address));
    res->connected = (int8_t) (_con == 0);
}
void network_client_get(struct network_client *res, const struct string_st *msg, int8_t flag, struct string_st *res_msg, int8_t *res_flag) {
    if (!res->connected) {
        *res_flag |= NET_ERROR;
        return;
    }
    *res_flag = 0;
    network_send(res->socket, msg, NET_REQUEST | NET_GET | flag, res_flag);
    if (*res_flag & NET_ERROR) return;
    network_read(res->socket, res_msg, res_flag);
}
void network_client_send(struct network_client *res, const struct string_st *msg, int8_t flag, int8_t *res_flag) {
    if (!res->connected) return;
    network_send(res->socket, msg, NET_REQUEST | NET_SEND | flag, res_flag);
}
void network_client_close(struct network_client *res) {
    if (!res->connected) return;
#ifdef WIN32
    closesocket(res->socket);
#else
    close(res->socket);
#endif
    res->connected = 0;
}