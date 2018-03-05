#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include "erl_interface.h"
#include "ei.h"

#define BUFSIZE 1000
int my_listen(int port);

int main(int argc, char **argv) {
        int port;                          /* Listen port number */
        int listen;                        /* Listen socket */
        int fd;                            /* fd to Erlang node */
        ErlConnect conn;                   /* Connection data */
        ei_cnode ec;

        int loop = 1;                      /* Loop flag */
        int got;

        erlang_msg emsg;                   /* Incoming message */
        ei_x_buff msg;                     /* Incoming message buffer */

        ei_set_tracelevel(5);

        if(argc < 2){
          printf("Usage: cnode <port>\n");
          return(-1);
        }
        port = atoi(argv[1]);

        printf("Init\n");
        erl_init(NULL, 0);

        printf("Connecting\n");
        if (ei_connect_init(&ec, "cnode", "chocolatecookie", 1) < 0)
                erl_err_quit("Error on connect_init.");

        /* Make a listen socket */
        printf("Listening\n");
        if ((listen = my_listen(port)) <= 0)
                erl_err_quit("Error on listen.");

        printf("Publishing\n");
        if (ei_publish(&ec, port) == -1)
                erl_err_quit("Error on erl_publish.");

        printf("Accepting\n");
        if ((fd = ei_accept(&ec, listen, &conn)) == ERL_ERROR)
                erl_err_quit("Error on erl_accept");

        fprintf(stderr, "Connected to %s\n\r", conn.nodename);
        ei_x_new(&msg);
        while (loop) {
                got = ei_receive_msg(fd, &emsg, &msg);
                printf("Got Message: ");
                if (got == ERL_TICK) {
                        /* ignore */
                } else if (got == ERL_ERROR) {
                        loop = 0;
                } else {
                        if (emsg.msgtype == ERL_REG_SEND) {
                                ETERM *e = erl_mk_int(5);
                                ei_send(fd, &emsg.from, e, erl_size(e));
                                erl_free_term(e);
                        }
                }
        } /* while */
        ei_x_free(&msg);
}


int my_listen(int port) {
        int listen_fd;
        struct sockaddr_in addr;
        int on = 1;

        if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                return (-1);

        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        memset((void*) &addr, 0, (size_t) sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listen_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
                return (-1);

        listen(listen_fd, 5);
        return listen_fd;
}
