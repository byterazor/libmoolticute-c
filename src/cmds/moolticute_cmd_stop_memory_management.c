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
* @file moolticute_cmd_start_memory_management.c
* @brief C Source file for stopping memory mangement for the mooltipass device
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>
#include <libwebsockets.h>

/**
* @brief stop memory management on the mooltipass device
*
* @param wait - wait for the answer packet
*
* @return 0 - everything is fine, <0 ERROR Codes
*/
int moolticute_stop_memory_management(struct moolticute_ctx *ctx, int wait)
{
  const char *json_str;
  char *msg;
  struct json_object *jObj=json_object_new_object();

  pthread_mutex_lock(&ctx->write_mutex);
  if (ctx->connected == 0)
  {
    pthread_mutex_unlock(&ctx->write_mutex);
    return M_ERROR_NOT_CONNECTED;
  }

  if (ctx->info.status.connected == 0)
  {
    pthread_mutex_unlock(&ctx->write_mutex);
    return M_ERROR_NO_MOOLTIPASS_DEVICE;
  }

  if (ctx->info.status.card_inserted == 0)
  {
    pthread_mutex_unlock(&ctx->write_mutex);
    return M_ERROR_NO_CARD;
  }
  pthread_mutex_unlock(&ctx->write_mutex);

  json_object_object_add(jObj, "msg", json_object_new_string("exit_memorymgmt"));

  json_str=json_object_to_json_string(jObj);

  msg=malloc(LWS_PRE+strlen(json_str)+1);
  strncpy(msg+LWS_PRE, json_str, strlen(json_str)+1);

  pthread_mutex_lock(&ctx->write_mutex);
  ctx->transmit_message=msg+LWS_PRE;
  ctx->transmit_size=strlen(json_str);
  pthread_mutex_unlock(&ctx->write_mutex);

  // send message to moolticuted
  lws_callback_on_writable(ctx->wsi);

  while(ctx->info.mm.enabled==1 && wait == 1)
  {
    usleep(100);
  }

  return 0;
}
