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

#ifdef WIN32
void network_send(unsigned socket, const struct string_st *msg, char flag) {
    { // Header of packet
        char *header = skr_malloc(16);
        set_length(header, msg->size);
        header[0] = flag;
        send(socket, header, 16, 0);
        skr_free(header);
    }
    send(socket, msg->data, (int)msg->size, 0);
}
void network_read(unsigned socket, struct string_st *msg, char *flag) {
    { // Header of packet
        char *header = skr_malloc(16);
        recv(socket, header, 16, 0);
        *flag = header[0];
        string_resize(msg, get_length(header));
        skr_free(header);
    }
    recv(socket, msg->data, (int)msg->size, 0);
}
#else
void network_send(int socket, const struct string_st *msg, char flag) {
    { // Header of packet
        char *header = skr_malloc(16);
        set_length(header, msg->size);
        header[0] = flag;
        send(socket, header, 16, 0);
        skr_free(header);
    }
    send(socket, msg->data, msg->size, 0);
}
void network_read(int socket, struct string_st *msg, char *flag) {
    { // Header of packet
        char *header = skr_malloc(16);
        read(socket, header, 16);
        *flag = header[0];
        string_resize(msg, get_length(header));
        skr_free(header);
    }
    read(socket, msg->data, msg->size);
}
#endif
