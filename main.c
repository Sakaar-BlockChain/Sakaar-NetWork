#include <stdio.h>
#include "network.h"

void config_request_get(const struct string_st *data, struct string_st *response) {
    if (response == NULL) return;
    if (string_is_null(data)) return string_clear(response);
    printf("data get : %s\n", data->data);
    string_set_str(response, "321", 3);
}
int config_request_send(const struct string_st *data) {
    if (string_is_null(data)) return 0;
    printf("data send : %s\n", data->data);
    return 1;
}

int main(){
    struct network_conf config = (struct network_conf) {AF_INET, SOCK_STREAM, 0, INADDR_ANY, 1240, 20};
    struct network_p2p *network;
    network = network_p2p_new(config, &config_request_get, &config_request_send);


    network_p2p_start(network);

    struct string_st *str = string_new();
    struct string_st *res = string_new();
    string_set_str(str, "123", 3);
    while(1){
        network_p2p_get(network, str, res);
        printf("res : %s\n", res->data);
    }

}