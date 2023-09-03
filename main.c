#include <stdio.h>
#include "network.h"

int8_t config_request_get(const struct string_st *data, struct string_st *response) {
    if (response == NULL) return 1;
    string_clear(response);
    if (string_is_null(data)) return 0;
    printf("data get : %s\n", data->data);
    string_set_str(response, "321", 3);
    return 0;
}
int8_t config_request_send(const struct string_st *data) {
    if (string_is_null(data)) return 0;
    printf("data send : %s\n", data->data);
    return 1;
}

int main() {
    struct network_conf config = (struct network_conf) {AF_INET, SOCK_STREAM, 0, INADDR_ANY, 1240, 20};
    struct network_p2p network;

    struct string_st str;
    struct string_st res;

    network_p2p_data_init(&network, config, &config_request_get, &config_request_send);
    string_data_init(&str);
    string_data_init(&res);

//    network_p2p_start(&network);
    network_server_connect(&network.server);
    string_set_str(&str, "123", 3);
    while(1) {
        network_p2p_get(&network, &str, &res);
        printf("res : %s\n", res.data);
    }

    string_data_free(&res);
    string_data_free(&str);
    network_p2p_data_free(&network);
}