#include <erl_nif.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

ErlNifResourceType *complex_type;

int load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
	complex_type = enif_open_resource_type(env, NULL, "complex_type", NULL, ERL_NIF_RT_CREATE, NULL);

	if(!complex_type){
		return -1;
	}

	return 0;
}

static ERL_NIF_TERM complex_rect_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
		double a, b;
		gsl_complex *complex;

		if(!enif_get_double(env, argv[0], &a)) {
			return enif_make_badarg(env);
		}
		if(!enif_get_double(env, argv[1], &b)) {
			return enif_make_badarg(env);
		}

		complex = (gsl_complex*)enif_alloc_resource(complex_type, sizeof(gsl_complex));
		GSL_SET_COMPLEX(complex, a, b);

		return enif_make_resource(env, complex);
}

static ERL_NIF_TERM to_tuple_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
		gsl_complex *complex;
		ERL_NIF_TERM a, b;

		if(!enif_get_resource(env, argv[0], complex_type, (void**)&complex)){
			return enif_make_badarg(env);
		}

		a = enif_make_double(env, GSL_REAL(*complex));
		b = enif_make_double(env, GSL_IMAG(*complex));

		return enif_make_tuple2(env, a, b);
}

static ERL_NIF_TERM complex_arg_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
		gsl_complex *complex;
		double arg;

		if(!enif_get_resource(env, argv[0], complex_type, (void**)&complex)){
			return enif_make_badarg(env);
		}

		arg = gsl_complex_arg(*complex);

		return enif_make_double(env, arg);
}

static ERL_NIF_TERM complex_abs_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
		gsl_complex *complex;
		double mag;

		if(!enif_get_resource(env, argv[0], complex_type, (void**)&complex)){
			return enif_make_badarg(env);
		}

		mag = gsl_complex_abs(*complex);

		return enif_make_double(env, mag);
}

static ErlNifFunc nif_funcs[] = {
    {"new", 2, complex_rect_nif},
		{"to_tuple", 1, to_tuple_nif},
		{"arg", 1, complex_arg_nif},
		{"abs", 1, complex_abs_nif},
};

ERL_NIF_INIT(Elixir.Complex, nif_funcs, load, NULL, NULL, NULL)
