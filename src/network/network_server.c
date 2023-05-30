#include <stdio.h>
#include "network.h"

int is_running = 0;
struct network_server *network_server_new(
        struct network_conf *config,
        int (*_get)(const struct string_st *str, struct string_st *res),
        int (*_send)(const struct string_st *str)) {
    struct network_server *res = skr_malloc(sizeof(struct network_server));

    res->config = config;
    res->_get = _get;
    res->_send = _send;

#ifdef WIN32
    if (WSAStartup(MAKEWORD(2,2),&res->wsa) != 0) {
        perror("Failed to start WSA...\n");
    }
#endif

    res->_socket = socket(config->domain, config->service, config->protocol);
#ifdef WIN32
    char option = 1;
#else
    res->mutex = skr_malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(res->mutex, NULL);
    int option = 1;
#endif
    setsockopt(res->_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    res->server_address = (struct sockaddr_in) {};
    res->client_address = (struct sockaddr_in) {};
    res->hosts = list_new();

    if (res->_socket == 0) {
        perror("Failed to connect socket...\n");
        exit(1);
    }
    res->server_address.sin_family = config->domain;
    res->server_address.sin_addr.s_addr = htonl(config->_interface);
    res->server_address.sin_port = htons(config->port);
    if ((bind(res->_socket, (struct sockaddr *) &res->server_address, sizeof(res->server_address))) < 0) {
        perror("Failed to bind socket...\n");
        exit(1);
    }
    if ((listen(res->_socket, config->backlog)) < 0) {
        perror("Failed to start listening...\n");
        exit(1);
    }
    return res;
}
void network_server_free(struct network_server *res) {
    if (res == NULL) return;
    if (is_running) is_running = 0;
    list_free(res->hosts);
#ifndef WIN32
    pthread_mutex_destroy(res->mutex);
    skr_free(res->mutex);
    pthread_mutex_init(res->mutex, NULL);
#endif
    skr_free(res);
}

void network_server_accept(socket_t client_socket, struct network_server *server) {
    char flag = 0;
    char flag_res = 0;
    int send_next = 0;
    struct string_st *msg = string_new();
    struct string_st *res_msg = string_new();

    network_read(client_socket, msg, &flag);
    if (flag & NET_CONNECTIONS) {
        flag_res |= NET_CONNECTIONS;
        if (flag & NET_SEND) {
            if (msg->size == 0) {
                char *data = inet_ntoa(server->client_address.sin_addr);
                string_resize(msg, strlen(data));
                memcpy(msg->data, data, msg->size);
            }
            send_next = 1;
            for (size_t i = 0; i < server->hosts->size; i++) {
                if (string_cmp(msg, server->hosts->data[i]->data) == 0) {
                    send_next = 0;
                    break;
                }
            }
            if (send_next) {
                list_add_new(server->hosts, STRING_TYPE);
                string_set(server->hosts->data[server->hosts->size - 1]->data, msg);
            }
        }
        if (flag & NET_GET) {
            list_get_tlv(server->hosts, res_msg);
        }
    }

    if (flag & NET_DATA) {
        flag_res |= NET_DATA;
        if (flag & NET_SEND) {
            if (server->_send != NULL) send_next = server->_send(msg);
        }
        if (flag & NET_GET) {
            if (server->_get != NULL && server->_get(msg, res_msg))
                flag_res |= NET_ERROR;
        }
    }
    if (flag & NET_GET) {
        flag_res |= NET_GET;
        flag_res |= NET_RESPONSE;
        network_send(client_socket, res_msg, flag_res);
    }
#ifdef WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
    if ((flag & NET_SEND) && send_next) {
        network_server_send(server, msg, flag);
    }
    string_free(msg);
    string_free(res_msg);
}

#ifdef WIN32
DWORD WINAPI network_server_init(void *arg) {
    struct network_server *server = arg;
    int address_length = sizeof(server->client_address);
    while (is_running) {
        unsigned client_socket = accept(server->_socket, (struct sockaddr *) &server->client_address, &address_length);
        network_server_accept(client_socket, server);
    }
    return 0;
}
#else
void *network_server_init(void *arg) {
    struct network_server *server = arg;
    socklen_t address_length = sizeof(server->client_address);
    while (1) {
        pthread_mutex_lock(server->mutex);
        if (!is_running) break;
        pthread_mutex_unlock(server->mutex);
        int client_socket = accept(server->_socket, (struct sockaddr *) &server->client_address, &address_length);
        network_server_accept(client_socket, server);
    }
    return NULL;
}
#endif

void network_server_start(struct network_server *res) {
    if (is_running) return;
    is_running = 1;
#ifdef WIN32
    HANDLE server_thread = CreateThread(NULL, 0, network_server_init, res, 0, NULL);
    if(server_thread){
        network_server_connect(res);
    }
#else
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, network_server_init, res);
    network_server_connect(res);
#endif
}
void network_server_close(struct network_server *res) {
#ifndef WIN32
    pthread_mutex_lock(res->mutex);
#endif
    is_running = 0;
#ifndef WIN32
    pthread_mutex_unlock(res->mutex);
#endif
}

int network_server_connected(struct network_server *res) {
    struct string_st *msg = string_new();
    struct string_st *res_msg = string_new();
    int result = 0;

    if (network_server_get(res, msg, NET_CONNECTIONS, res_msg) == 0)
        result = list_set_tlv_self(res->hosts, res_msg, STRING_TYPE);
    string_free(msg);
    string_free(res_msg);
    return result;
}
void network_server_connect(struct network_server *res) {
    list_add_new(res->hosts, STRING_TYPE);
    string_set_str(res->hosts->data[res->hosts->size - 1]->data, "127.0.0.1", 9);

    struct string_st *msg = string_new();
    network_server_send(res, msg, NET_CONNECTIONS);
    string_free(msg);
}

int network_server_get(struct network_server *res, const struct string_st *msg, char flag, struct string_st *res_msg) {
    struct network_client *client = network_client_new();
    char res_flag = NET_ERROR;

    network_client_set_config(client, res->config);
    for (size_t i = 0; i < res->hosts->size; i++) {
        res_flag = 0;
        network_client_connect(client, res->hosts->data[i]->data);
        network_client_get(client, msg, flag, res_msg, &res_flag);
        network_client_close(client);
        if ((res_flag & NET_ERROR) == 0) break;
    }
    if (res_flag & NET_ERROR) string_clear(res_msg);
    network_client_free(client);
    return (res_flag & NET_ERROR) != 0;
}
void network_server_send(struct network_server *res, const struct string_st *msg, char flag) {
    struct network_client *client = network_client_new();
    network_client_set_config(client, res->config);
    for (size_t i = 0; i < res->hosts->size; i++) {
        network_client_connect(client, res->hosts->data[i]->data);
        network_client_send(client, msg, flag);
        network_client_close(client);
    }
    network_client_free(client);
}



