#include <stdio.h>
#include "network.h"

void set_length(char *_msg, size_t length) {
    for (int i = 8; i >= 1; i--) {
        _msg[i] = (char) (((unsigned) length) % 256);
        length >>= 8;
    }
}
size_t get_length(const char *_msg) {
    size_t length = 0;
    for (int i = 1; i <= 8; i++) {
        length <<= 8;
        length += (unsigned char) _msg[i];
    }
    return length;
}

void network_send(socket_t socket, const struct string_st *msg, char flag) {
    { // Header of packet
        char *header = skr_malloc(16);
        set_length(header, msg->size);
        header[0] = flag;
        send(socket, header, 16, 0);
        skr_free(header);
    }
#ifdef WIN32
    send(socket, msg->data, (int) msg->size, 0);
#else
    send(socket, msg->data, msg->size, 0);
#endif
}
void network_read(socket_t socket, struct string_st *msg, char *flag) {
    { // Header of packet
        char *header = skr_malloc(16);
#ifdef WIN32
        recv(socket, header, 16, 0);
#else
        read(socket, header, 16);
#endif
        *flag = header[0];
        string_resize(msg, get_length(header));
        skr_free(header);
    }
#ifdef WIN32
    recv(socket, msg->data, (int) msg->size, 0);
#else
    read(socket, msg->data, msg->size);
#endif
}
