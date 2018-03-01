/* port.c */
#include <stdio.h>
#include <string.h>

#include <gsl/gsl_poly.h>
#include "erl_comm.h"
#include "erl_interface.h"
#include "ei.h"

typedef unsigned char byte;

int main(int argc, char** argv) {
        ETERM *tuplep, *xp;
        ETERM *list, *fp;
        ETERM *element;

        int len;
        double res;
        byte buf[256];
        double *coeffs;

        erl_init(NULL, 0);

        while (read_cmd(buf) > 0) {
                tuplep = erl_decode(buf);
                list = erl_element(1, tuplep);
                xp = erl_element(2, tuplep);
                len =  erl_length(list);

                coeffs = malloc(sizeof(double) * len);
                if(coeffs == NULL) {
                        fprintf(stderr, "Could not allocate coefficients.");
                        break;
                }

                int i = 0;
                while (list && (element = erl_hd(list))) {
                        coeffs[i] = ERL_FLOAT_VALUE(element);
                        i++;
                        list = erl_tl(list);
                }

                res = gsl_poly_eval(coeffs, len, ERL_FLOAT_VALUE(xp));

                fp = erl_mk_float(res);
                erl_encode(fp, buf);
                write_cmd(buf, erl_term_len(fp));

                erl_free_compound(tuplep);
                erl_free_compound(list);
                erl_free_term(xp);
                erl_free_term(fp);
                free(coeffs);
        }
        return(0);
}
