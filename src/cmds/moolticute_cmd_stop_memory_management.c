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
int moolticute_stop_memory_management(int wait)
{
  const char *json_str;
  char *msg;
  struct json_object *jObj=json_object_new_object();

  if (mContext.connected == 0)
  {
    return M_ERROR_NOT_CONNECTED;
  }

  if (mContext.info.status.connected == 0)
  {
    return M_ERROR_NO_MOOLTIPASS_DEVICE;
  }

  json_object_object_add(jObj, "msg", json_object_new_string("exit_memorymgmt"));

  json_str=json_object_to_json_string(jObj);

  msg=malloc(strlen(json_str)+1);
  strncpy(msg, json_str, strlen(json_str)+1);
  mContext.transmit_message=msg;
  mContext.transmit_size=strlen(json_str);

  // send message to moolticuted
  lws_callback_on_writable(mContext.wsi);

  while(mContext.info.mm.enabled==1 && wait == 1)
  {
    usleep(100);
  }

  return 0;
}
