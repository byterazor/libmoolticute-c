/*
libmoolticute-c
Copyright (C) 2018  Dominik Meyer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file moolticute_connect.c
* @brief C Source file for the moolticute_connect function
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"

/*
* @brief websocket threat, calling the lws_service function every 250msecs
*/
void *websocket(void *data)
{
  struct moolticute_ctx *ctx = (struct moolticute_ctx *) data;

  while(ctx->finish == 0)
  {
    lws_service( ctx->context, 250 );
  }
  pthread_mutex_lock (&ctx->write_mutex);
  ctx->finished=1;
  pthread_mutex_unlock (&ctx->write_mutex);
  return NULL;
}

/// protocol enumeration for libwebsockets
enum moolticute_protocols
{
	PROTOCOL_MOOLTICUTE = 0,
	PROTOCOL_COUNT
};

/**
* @brief connect to the moolticute daemon
*
* @param ctx - the context in which to connect
* @return 0 = everyhting is fine || -1 not connected to daemon
*/
int moolticute_connect(struct moolticute_ctx *ctx)
{
  struct lws_client_connect_info ccinfo;
  int timeout=(1e+6/100)*10;
  memset(&ccinfo,0,sizeof(ccinfo));

  ccinfo.context = ctx->context;
	ccinfo.address = "localhost";
	ccinfo.port = 30035;
	ccinfo.path = "/";
	ccinfo.host = lws_canonical_hostname( ctx->context );
	ccinfo.origin = "origin";
	ccinfo.protocol = protocols[PROTOCOL_MOOLTICUTE].name;
  ccinfo.userdata = (void *) ctx;
  ctx->wsi=lws_client_connect_via_info(&ccinfo);
  pthread_mutex_init (&ctx->write_mutex, NULL);
  pthread_create(&ctx->thread, NULL, websocket, (void *) ctx);

  // wait until connection attempt has been made
  while(ctx->connected==0 && timeout >0)
  {
    usleep(10000);
    timeout--;
  }

  if (timeout == 0)
  {
    return M_ERROR_TIMEOUT;
  }
  return 0;
}
