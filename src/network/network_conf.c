#include <stdio.h>
#include "network.h"

void set_length(char *_msg, size_t length) {
    for (int8_t i = 8; i >= 1; i--) {
        _msg[i] = (char) (((unsigned) length) % 256);
        length >>= 8;
    }
}
size_t get_length(const char *_msg) {
    size_t length = 0;
    for (int8_t i = 1; i <= 8; i++) {
        length <<= 8;
        length += (unsigned char) _msg[i];
    }
    return length;
}

void network_send(socket_t socket, const struct string_st *msg, int8_t flag, int8_t *res_flag) {
    { // Header of packet
        char header[16];
        set_length(header, msg->size);
        header[0] = flag;
        if (send(socket, header, 16, 0) != 16) {
            if (res_flag != NULL) *res_flag = NET_ERROR;
            return;
        }
    }
#ifdef WIN32
    size_t size = 0, res;
    while (size != msg->size) {
        res = send(socket, msg->data + size, (int)(msg->size - size), 0);
        if (res <= 0) {
            if (res_flag != NULL) *res_flag = NET_ERROR;
            return;
        }
        size += res;
    }
#else
    size_t size = 0, res;
    while (size != msg->size) {
        res = send(socket, msg->data + size, msg->size - size, 0);
        if (res <= 0) {
            if (res_flag != NULL) *res_flag = NET_ERROR;
            return;
        }
        size += res;
    }
#endif
}
void network_read(socket_t socket, struct string_st *msg, int8_t *flag) {
    { // Header of packet
        char header[16];
#ifdef WIN32
        if (recv(socket, header, 16, 0) != 16) {
            *flag = NET_ERROR;
            return;
        }
#else
        if (recv(socket, header, 16, 0) != 16) {
            *flag = NET_ERROR;
            return;
        }
#endif
        *flag = header[0];
        string_resize(msg, get_length(header));
    }
#ifdef WIN32
    size_t size = 0, res;
    while (size != msg->size) {
        res = recv(socket, msg->data + size, (int)(msg->size - size), 0);
        if (res <= 0) {
            *flag = NET_ERROR;
            return;
        }
        size += res;
    }
#else
    size_t size = 0, res;
    while (size != msg->size) {
        res = recv(socket, msg->data + size, msg->size - size, 0);
        if (res <= 0) {
            *flag = NET_ERROR;
            return;
        }
        size += res;
    }
#endif
}
