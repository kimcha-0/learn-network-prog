#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>


#define PORT "9034"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener; // Listening socket descriptor
    //
    // break;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
            close(listener);
            perror("listener: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(ai);

    if (listen(listener, 10) == -1) {
        perror("listen socket");
        return -1;
    }
    return listener;

}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // double the size
        *pfds = realloc(*pfds, sizeof (**pfds) * (*fd_size));
    }
    (*pfds)[*fd_count].fd = new_fd;
    (*pfds)[*fd_count].events = POLLIN;

    (*fd_count)++;

}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    pfds[i] = pfds[*fd_count - 1];
    (*fd_count)--;
}

int main()
{
    int listener; // listener socket

    int new fd; // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[256]; // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    int fd_count = 0;
    fd_size = 10;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listener socket\n");
        exit(1);
    }

    // Add the listener to the set
    add_topfds(&pfds, listener, &fd_count, &fd_size);

    // Main loop
    for (;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }
        
        // look through connections and look for data to read
        for (int i = 0; i < fd_count; i++) {

            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == listener) {

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);
                    if (newfd == -1)
                        perror("accept");
                    else {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on" "socket %d\n", 
                               inet_ntop(remoteaddr.ss_family, get_inaddr(
                               (struct sockaddr *)&remoteaddr),remoteIP, INET6_ADDRSTRLEN), 
                               newfd);
                    }
                } else {
                    // if not listener we are just a regular client
                }
            }
        }

    }

}
