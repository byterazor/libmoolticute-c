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
* @file moolticute_cmd_service_exist.c
* @brief C Source file for checking a service exist on the mooltipass device
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json-c/json.h>
#include <libwebsockets.h>


/**
* @brief callback for the credential_exist/data_node_exist message
*/
void moolticute_cb_credential_exist(void *user, struct json_object *jObj)
{
  struct moolticute_ctx *ctx = (struct moolticute_ctx *) user;
  int i;
  struct json_object *data;
  struct json_object *value;

  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "exists", &value);

  if (strncmp(json_object_get_string(value),"true",4)==0)
  {
    ctx->credential_exist=1;
  }
  else
  {
    ctx->credential_exist=0;
  }
}


/**
* @brief check if the given service exist on the moolipass device
*
* @param service   - the name of the service to check
* @param data      - check for a data node or credential node
*
* @return 0 - not found, 1 - exist, <0 ERROR Codes
*/
int moolticute_cmd_service_exist(struct moolticute_ctx *ctx, char *service, int data_node)
{
  const char *json_str;
  char *msg;
  struct json_object *jObj;
  struct json_object *data;

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

  jObj=json_object_new_object();
  data=json_object_new_object();

  json_object_object_add(jObj, "msg", data_node==0 ? json_object_new_string("credential_exists") :  json_object_new_string("data_node_exists"));
  json_object_object_add(data, "service", json_object_new_string(service));
  json_object_object_add(jObj, "data", data);

  json_str=json_object_to_json_string(jObj);
  msg=malloc(LWS_PRE+strlen(json_str)+1);
  strncpy(msg+LWS_PRE, json_str, strlen(json_str)+1);

  pthread_mutex_lock (&ctx->write_mutex);
  ctx->transmit_message=msg+LWS_PRE;
  ctx->transmit_size=strlen(json_str);
  ctx->credential_exist=-1;
  pthread_mutex_unlock (&ctx->write_mutex);

  //register the callback
  moolticute_register_cb(ctx, "credential_exists", &moolticute_cb_credential_exist);
  moolticute_register_cb(ctx, "data_node_exists", &moolticute_cb_credential_exist);


  // send message to moolticuted
  lws_callback_on_writable(ctx->wsi);

  while(ctx->credential_exist==-1)
  {
    usleep(100);
  }


  return ctx->credential_exist;
}
