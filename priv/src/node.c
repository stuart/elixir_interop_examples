#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "erl_interface.h"
#include "ei.h"

#define BUFSIZE 1000
int my_listen(int port);
int run_cnode(int port);

int main(int argc, char **argv) {
  int port;
  int pid;
  int fd;

  if(argc < 2){
    printf("Usage: cnode <port>\n");
    return(-1);
  }
  port = atoi(argv[1]);

  if((pid = fork()) != 0)
    exit(0);

  setsid();
  chdir("/");
  umask(0);
  if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
    ioctl(fd, TIOCNOTTY, 0);
    close(fd);
  }

  return run_cnode(port);
}

int run_cnode(int port){
        int listen;                        /* Listen socket */
        int fd;                            /* fd to Erlang node */
        ErlConnect conn;                   /* Connection data */
        ei_cnode ec;

        int loop = 1;                      /* Loop flag */
        int got;

        ErlMessage emsg;
        unsigned char message[BUFSIZE];     /* Incoming message buffer */

        ETERM *fromp, *tuplep, *fnp, *argp, *resp;
        int res;

        printf("Init\n");

        ei_set_tracelevel(5);
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
        while (loop) {
                got = erl_receive_msg(fd, message, BUFSIZE, &emsg);
                if (got == ERL_TICK) {
                        /* ignore */
                } else if (got == ERL_ERROR) {
                        loop = 0;
                } else {
                        if (emsg.type == ERL_REG_SEND) {
                            fromp = erl_element(2, emsg.msg);
                            tuplep = erl_element(3, emsg.msg);
                            fnp = erl_element(1, tuplep);
                            argp = erl_element(2, tuplep);
                            if(strncmp(ERL_ATOM_PTR(fnp), "cube", 4) == 0){
                              res = ERL_INT_VALUE(argp) * ERL_INT_VALUE(argp) * ERL_INT_VALUE(argp);
                            }
                            if(strncmp(ERL_ATOM_PTR(fnp), "square", 6) == 0){
                              res = ERL_INT_VALUE(argp) * ERL_INT_VALUE(argp);
                            }

                            resp = erl_format("{cnode, ~i}", res);
                            erl_send(fd, fromp, resp);

                        }

                        erl_free_term(fromp);
                        erl_free_term(fnp);
                        erl_free_term(argp);
                        erl_free_term(tuplep);
                        erl_free_term(resp);
                        erl_free_term(emsg.from);
                        erl_free_term(emsg.msg);
                }

        } /* while */
    return 0;
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
