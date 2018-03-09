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
* @brief C Source file for starting memory mangement for the mooltipass device
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>
#include <libwebsockets.h>

/**
* @brief start memory management on the mooltipass device
*
* to manage the credentials on the mooltipass device the credential
* management mode has to be initiated and acknowledged
* this function transmit the correct message and the device answers
* with a memorymgmt_data message.
*
* @param want_data - should data be requested from the device
* @param wait - wait for the memorymgmt_data message processing to finish
*
* @return 0 - everything is fine, <0 ERROR Codes
*/
int moolticute_start_memory_management(int want_data, int wait)
{
  const char *json_str;
  char *msg;
  struct json_object *data=json_object_new_object();
  struct json_object *jObj=json_object_new_object();

  if (mContext.connected == 0)
  {
    return M_ERROR_NOT_CONNECTED;
  }

  if (mContext.info.status.connected == 0)
  {
    return M_ERROR_NO_MOOLTIPASS_DEVICE;
  }

  if (mContext.info.status.locked==1)
  {
    return M_ERROR_DEVICE_LOCKED;
  }

  json_object_object_add(data, "want_data", json_object_new_string(want_data == 0 ? "false" : "true"));
  json_object_object_add(jObj, "msg", json_object_new_string("start_memorymgmt"));
  json_object_object_add(jObj, "data", data);


  json_str=json_object_to_json_string(jObj);

  msg=malloc(strlen(json_str)+1);
  strncpy(msg, json_str, strlen(json_str)+1);

  mContext.info.mm.updating=1;
  // send message to moolticuted
  lws_write(mContext.wsi, (unsigned char *)msg,strlen(msg), LWS_WRITE_TEXT);

  while(mContext.info.mm.updating==1 && wait == 1)
  {
    usleep(100);
  }

  return 0;
}
