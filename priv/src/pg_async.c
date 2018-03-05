/*
 * %CopyrightBegin%
 *
 * Copyright Ericsson AB 2006-2016. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * %CopyrightEnd%
 */
/*
 * Purpose: A driver using libpq to connect to Postgres
 * from erlang, a sample for the driver documentation
 */

#include <erl_driver.h>
#include <libpq-fe.h>
#include <ei.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pg_encode.h"

/* Driver interface declarations */
static ErlDrvData start(ErlDrvPort port, char *command);
static void stop(ErlDrvData drv_data);
static ErlDrvSSizeT control(ErlDrvData drv_data, unsigned int command, char *buf,
																												ErlDrvSizeT len, char **rbuf, ErlDrvSizeT rlen);
static void ready_io(ErlDrvData drv_data, ErlDrvEvent event);

static ErlDrvEntry pq_driver_entry = {
								NULL, /* F_PTR init, called when driver is loaded */
								start, /* L_PTR start, called when port is opened */
								stop, /* F_PTR stop, called when port is closed */
								NULL, /* F_PTR output, called when erlang has sent */
								ready_io, /* F_PTR ready_input, called when input descriptor ready */
								ready_io, /* F_PTR ready_output, called when output descriptor ready */
								"pg_async", /* char *driver_name, the argument to open_port */
								NULL, /* F_PTR finish, called when unloaded */
								NULL, /* void *handle, Reserved by VM */
								control, /* F_PTR control, port_command callback */
								NULL, /* F_PTR timeout, reserved */
								NULL, /* F_PTR outputv, reserved */
								NULL, /* F_PTR ready_async, only for async drivers */
								NULL, /* F_PTR flush, called when port is about
	                to be closed, but there is data in driver
	                queue */
								NULL, /* F_PTR call, much like control, sync call
	                to driver */
								NULL, /* F_PTR event, called when an event selected
	                by driver_event() occurs. */
								ERL_DRV_EXTENDED_MARKER, /* int extended marker, Should always be
	                                   set to indicate driver versioning */
								ERL_DRV_EXTENDED_MAJOR_VERSION, /* int major_version, should always be
	                                          set to this value */
								ERL_DRV_EXTENDED_MINOR_VERSION, /* int minor_version, should always be
	                                          set to this value */
								0,                   /* int driver_flags, see documentation */
								NULL,                /* void *handle2, reserved for VM use */
								NULL,                /* F_PTR process_exit, called when a
	                               monitored process dies */
								NULL                 /* F_PTR stop_select, called to close an
	                               event object */
};

typedef struct our_data_t {
								PGconn* conn;
								ErlDrvPort port;
								int socket;
								int connecting;
} our_data_t;

/* Keep the following definitions in alignment with the FUNC_LIST
 * in erl_pq_sync.erl
 */

#define DRV_CONNECT             'C'
#define DRV_DISCONNECT          'D'
#define DRV_SELECT              'S'

/* #define L     fprintf(stderr, "%d\r\n", __LINE__) */

/* INITIALIZATION AFTER LOADING */

/*
 * This is the init function called after this driver has been loaded.
 * It must *not* be declared static. Must return the address to
 * the driver entry.
 */
DRIVER_INIT(pq_drv)
{
								return &pq_driver_entry;
}

static char* get_s(const char* buf, int len);
static int do_connect(const char *s, our_data_t* data);
static int do_disconnect(our_data_t* data);
static int do_select(const char* s, our_data_t* data);

/* DRIVER INTERFACE */
static ErlDrvData start(ErlDrvPort port, char *command)
{
								our_data_t* data = driver_alloc(sizeof(our_data_t));
								data->port = port;
								data->conn = NULL;
								return (ErlDrvData)data;
}

static void stop(ErlDrvData drv_data)
{
								do_disconnect((our_data_t*)drv_data);
}

static ErlDrvSSizeT control(ErlDrvData drv_data, unsigned int command, char *buf,
																												ErlDrvSizeT len, char **rbuf, ErlDrvSizeT rlen)
{
								int r;
								char* s = get_s(buf, len);
								our_data_t* data = (our_data_t*)drv_data;
								switch (command) {
								case DRV_CONNECT:     r = do_connect(s, data);  break;
								case DRV_DISCONNECT:  r = do_disconnect(data);  break;
								case DRV_SELECT:      r = do_select(s, data);   break;
								default:              r = -1; break;
								}
								driver_free(s);
								return r;
}

static int do_connect(const char *s, our_data_t* data)
{
								PGconn* conn = PQconnectStart(s);
								if (PQstatus(conn) == CONNECTION_BAD) {
																ei_x_buff x;
																ei_x_new_with_version(&x);
																encode_error(&x, conn);
																PQfinish(conn);
																conn = NULL;
																driver_output(data->port, x.buff, x.index);
																ei_x_free(&x);
								}
								PQconnectPoll(conn);
								int socket = PQsocket(conn);
								data->socket = socket;
								driver_select(data->port, (ErlDrvEvent)(uintptr_t)socket, DO_READ, 1);
								driver_select(data->port, (ErlDrvEvent)(uintptr_t)socket, DO_WRITE, 1);
								data->conn = conn;
								data->connecting = 1;
								return 0;
}

static int do_disconnect(our_data_t* data)
{
								ei_x_buff x;
								driver_select(data->port, (ErlDrvEvent)(uintptr_t)data->socket, DO_READ, 0);
								driver_select(data->port, (ErlDrvEvent)(uintptr_t)data->socket, DO_WRITE, 0);
								PQfinish(data->conn);
								data->conn = NULL;
								ei_x_new_with_version(&x);
								encode_ok(&x);
								driver_output(data->port, x.buff, x.index);
								ei_x_free(&x);
								return 0;
}

static int do_select(const char* s, our_data_t* data)
{
								data->connecting = 0;
								PGconn* conn = data->conn;
								/* if there's an error return it now */
								if (PQsendQuery(conn, s) == 0) {
																ei_x_buff x;
																ei_x_new_with_version(&x);
																encode_error(&x, conn);
																driver_output(data->port, x.buff, x.index);
																ei_x_free(&x);
								}
								/* else wait for ready_output to get results */
								return 0;
}

static void ready_io(ErlDrvData drv_data, ErlDrvEvent event)
{
								PGresult* res = NULL;
								our_data_t* data = (our_data_t*)drv_data;
								PGconn* conn = data->conn;
								ei_x_buff x;
								ei_x_new_with_version(&x);
								if (data->connecting) {
																ConnStatusType status;
																PQconnectPoll(conn);
																status = PQstatus(conn);
																if (status == CONNECTION_OK)
																								encode_ok(&x);
																else if (status == CONNECTION_BAD)
																								encode_error(&x, conn);
								} else {
																PQconsumeInput(conn);
																if (PQisBusy(conn))
																								return;
																res = PQgetResult(conn);
																encode_result(&x, res, conn);
																PQclear(res);
																for (;;) {
																								res = PQgetResult(conn);
																								if (res == NULL)
																																break;
																								PQclear(res);
																}
								}
								if (x.index > 1) {
																driver_output(data->port, x.buff, x.index);
																if (data->connecting)
																								driver_select(data->port, (ErlDrvEvent)(uintptr_t)data->socket, DO_WRITE, 0);
								}
								ei_x_free(&x);
}

/* utilities */
static char* get_s(const char* buf, int len)
{
								char* result;
								if (len < 1 || len > 1000) return NULL;
								result = driver_alloc(len+1);
								memcpy(result, buf, len);
								result[len] = '\0';
								return result;
}
