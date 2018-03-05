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
#include <json.h>
#include <libwebsockets.h>

#define MAX_RANDOM_NUMBERS 32

int mRandomNumbers[MAX_RANDOM_NUMBERS];   /// global variable for the requested random numbers
int mGotRandomNumbers;                    /// identifier signalling the RandomNumbers have been updated

/**
* @brief callback for the get_random_numbers message
*/
void moolticute_cb_get_random_numbers(struct json_object *jObj)
{
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
}

/**
* @brief request random numbers from the moolticute daemon
* @param randomNumbers - array for the random numbers as call by reference, size MAX_RANDOM_NUMBERS = 32
* @return 0 = everything ok, < 0 ERROR Codes according to moolticute.h
*/
int moolticute_request_random_number(int *randomNumbers)
{
  int i;
  int timeout=100;
  const char *json_str;
  char *msg;
  struct json_object *jObj=json_object_new_object();

  if (mContext.connected==0)
  {
    return M_ERROR_NOT_CONNECTED;
  }

  if (mContext.status.connected == 0)
  {
    return M_ERROR_NO_MOOLTIPASS_DEVICE;
  }

  json_object_object_add(jObj, "msg", json_object_new_string("get_random_numbers"));
  json_str=json_object_to_json_string(jObj);
  msg=malloc(strlen(json_str)+1);
  memcpy(msg, json_str, strlen(json_str)+1);

  // blank random number array
  memset(mRandomNumbers,0,MAX_RANDOM_NUMBERS);
  mGotRandomNumbers=0;

  //register the callback
  moolticute_register_cb("get_random_numbers", &moolticute_cb_get_random_numbers);
  lws_write(mContext.wsi, (unsigned char *)msg,strlen(msg), LWS_WRITE_TEXT);

  while(mGotRandomNumbers==0 && timeout >0)
  {
    usleep(100);
    timeout--;
  }

  if (timeout == 0)
  {
    return M_ERROR_TIMEOUT;
  }

  for(i=0; i<MAX_RANDOM_NUMBERS;i++)
  {
    randomNumbers[i]=mRandomNumbers[i];
  }


  return 0;
}
