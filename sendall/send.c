#include <sys/types.h>
#include <sys/socket.h>

int sendall(int sockfd, char *buf, int *len) {
    int sent = 0;
    int bytesleft = *len;
    int n;

    while (sent < *len) {
        n = send(sockfd, buf+total, bytesleft, 0);
        if (n <= 0) break;
        sent += n;
        bytesleft -= n;
    }

    *len = total;
     return n == -1 ? -1 : 0;
}
