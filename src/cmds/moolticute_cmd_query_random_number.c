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
* @file moolticute_query_random_number.c
* @brief C Source file for requesting a random number from mooltipass device
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json-c/json.h>
#include <libwebsockets.h>
#include <stdint.h>

#define MAX_RANDOM_NUMBERS 32

uint32_t mRandomNumbers[MAX_RANDOM_NUMBERS];   /// global variable for the requested random numbers
int mGotRandomNumbers;                    /// identifier signalling the RandomNumbers have been updated

/**
* @brief callback for the get_random_numbers message
*/
void moolticute_cb_get_random_numbers(void *user , struct json_object *jObj)
{
  struct moolticute_ctx *ctx=(struct moolticute_ctx *) user;
  int i;
  struct json_object *data;
  struct json_object *value;

  json_object_object_get_ex(jObj, "data", &data);

  for(i=0; i<MAX_RANDOM_NUMBERS; i++)
  {
    value=json_object_array_get_idx(data,i);
    mRandomNumbers[i]=json_object_get_int(value);
  }
  mGotRandomNumbers=1;
  ctx->ready=1;
}

/**
* @brief request random numbers from the moolticute daemon
* @param randomNumbers - array for the random numbers as call by reference, size MAX_RANDOM_NUMBERS = 32
* @return 0 = everything ok, < 0 ERROR Codes according to moolticute.h
*/
int moolticute_request_random_number(struct moolticute_ctx *ctx, int *randomNumbers)
{
  int i;
  int timeout=10000000;
  const char *json_str;
  char *msg;
  struct json_object *jObj=json_object_new_object();

  pthread_mutex_lock(&ctx->write_mutex);
  if (ctx->connected==0)
  {
    pthread_mutex_unlock(&ctx->write_mutex);
    return M_ERROR_NOT_CONNECTED;
  }

  if (ctx->info.status.connected == 0)
  {
    pthread_mutex_unlock(&ctx->write_mutex);
    return M_ERROR_NO_MOOLTIPASS_DEVICE;
  }
  pthread_mutex_unlock(&ctx->write_mutex);

  json_object_object_add(jObj, "msg", json_object_new_string("get_random_numbers"));
  json_str=json_object_to_json_string(jObj);
  msg=malloc(LWS_PRE+strlen(json_str)+1);
  memcpy(msg+LWS_PRE, json_str, strlen(json_str)+1);

  pthread_mutex_lock (&ctx->write_mutex);
  ctx->transmit_message=msg+LWS_PRE;
  ctx->transmit_size=strlen(json_str);
  pthread_mutex_unlock (&ctx->write_mutex);

  // blank random number array
  memset(mRandomNumbers,0,MAX_RANDOM_NUMBERS);
  mGotRandomNumbers=0;

  //register the callback
  moolticute_register_cb(ctx, "get_random_numbers", &moolticute_cb_get_random_numbers);

  lws_callback_on_writable(ctx->wsi);

  while(mGotRandomNumbers==0 && timeout >0)
  {
    usleep(500);
    timeout--;
  }

  if (timeout == 0)
  {
    return M_ERROR_TIMEOUT;
  }

  if (randomNumbers==NULL)
  {
    return 0;
  }
  for(i=0; i<MAX_RANDOM_NUMBERS;i++)
  {
    randomNumbers[i]=mRandomNumbers[i];
  }


  return 0;
}
