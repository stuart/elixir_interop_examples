# Elixir Interop Examples

Some simple examples of interoperation with C code from Elixir.

Shows how Ports, Port Drivers, NIFs and C nodes work with Elixir.

## Installation

Prerequisites for compiling the C code:

* GNU Scientific Library libgsl : http:gnu.org/software/gsl
* Postgresql C library libpq : http:postgresql.org/docs/10/static/libpq.html

## Compiling the Examples

Run `mix compile`
This should compile all the C code as well as the Elixir code.
To remove the C objects you can run `mix priv.clean`

## Running the tests

* Run the suite: `mix test`
* If the C node tests fail weirdly check that epmd is running as a daemon. (It should be started by test helper.)

You should see all green.

## Using the Examples

See the docs for each module as to how they are used.
None of them do anything very interesting, they are after all only
examples of how you can do these things.
