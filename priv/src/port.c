/* port.c */
#include <stdio.h>
#include <string.h>

#include <gsl/gsl_sf_bessel.h>
#include "erl_comm.h"

int main() {
        int ref, n;
        double x, res;

        char buf[16];

        while (read_cmd((unsigned char*)buf) > 0) {
                ref = (buf[0] << 8) | buf[1];
                n = (buf[3] << 8) | buf[2];
                x = *(double*)(buf + 4);

                // fprintf(stderr, "ref: %d, n: %d, x: %lf, res: %lf\n", ref, n, x, res);
                res = gsl_sf_bessel_Jn(n,x);

                memset(&buf[0], 0, sizeof(buf));
                memcpy(&buf[0], &ref, sizeof(int));
                memcpy(&buf[2], &res, sizeof(double));

                write_cmd((unsigned char*)buf, 10);
        }
        return(0);
}
