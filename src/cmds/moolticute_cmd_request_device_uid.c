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
* @file moolticute_cmd_request_device_uid.c
* @brief C Source file for requesting the device uid
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>
#include <libwebsockets.h>

// void moolticute_cb_request_device_uid(struct json_object *jObj)
// {
// }

/**
* @brief request the uid from the mooltipass device
*
* a uid can be requested from the mooltipass device to identify modifications
* inside the device. In a first step a key has to be requested from the
* mooltipass supplier.
*
* @todo this function is not finished yet, because i could not verify its
* correct working. I have to request a key for my device.
*
* @param key - the key received from the moolitpass suppliert
* @return 0 - everything is fine, <0 ERROR Codes
*/
int moolticute_request_device_uid(struct moolticute_ctx *ctx, char key[32])
{
  const char *json_str;
  char *msg;
  char key_str[33];
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
  pthread_mutex_lock(&ctx->write_mutex);

  data=json_object_new_object();
  jObj=json_object_new_object();

  // create a string from the char array
  memset(key_str,0,33);
  memcpy(key_str,key,32);

  // create json object
  json_object_object_add(data, "key", json_object_new_string(key_str));
  json_object_object_add(jObj, "msg", json_object_new_string("request_device_uid"));
  json_object_object_add(jObj, "data", data);

  json_str=json_object_to_json_string(jObj);

  msg=malloc(LWS_PRE+strlen(json_str)+1);
  memcpy(msg+LWS_PRE, json_str, strlen(json_str)+1);

  pthread_mutex_lock(&ctx->write_mutex);
  ctx->transmit_message=msg+LWS_PRE;
  ctx->transmit_size=strlen(json_str);
  pthread_mutex_lock(&ctx->write_mutex);

  // send message to moolticuted
  lws_callback_on_writable(ctx->wsi);

  return 0;
}
