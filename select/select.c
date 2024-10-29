#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master; // master file descriptor list
    fd_set read_fds;
    int fdmax;

    int listener;
    int newfd
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[256];
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master); // initializes to the null set
    FD_ZERO(&read_fds);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, 'selectserver: %s\n', gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("socket");
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if ((bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            perror("bind";)
            close(listener);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
    }

    freaddrinfo(ai);

    if ((listen(listener, 10) == -1)) {
        perror("listen");
        exit(3);
    }

    FD_SET(listener, &master);

    fdmax = listener;
    

}
