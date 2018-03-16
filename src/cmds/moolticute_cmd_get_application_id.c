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
* @file moolticute_cmd_get_application_id.c
* @brief C Source file for requesting the application id from the moolticute server
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>
#include <libwebsockets.h>

/**
* @brief return the application id from the moolticute server
*
* @param name   - the name of the application, max 200 chars
* @param version- the version of the application, max 200 chars
*
* @return 0 - everything is fine, <0 ERROR Codes
*/
int moolticute_get_application_id(char *name, char *version)
{
  const char *json_str;
  char *msg;
  struct json_object *jObj=json_object_new_object();

  pthread_mutex_lock(&mContext.write_mutex);
  if (mContext.connected == 0)
  {
    pthread_mutex_unlock(&mContext.write_mutex);
    return M_ERROR_NOT_CONNECTED;
  }

  if (mContext.info.status.connected == 0)
  {
    pthread_mutex_unlock(&mContext.write_mutex);
    return M_ERROR_NO_MOOLTIPASS_DEVICE;
  }

  pthread_mutex_unlock(&mContext.write_mutex);


  if (strlen(mContext.app.name)== 0)
  {

    json_object_object_add(jObj, "msg", json_object_new_string("get_application_id"));

    json_str=json_object_to_json_string(jObj);


    msg=malloc(LWS_PRE+strlen(json_str)+1);
    strncpy(msg+LWS_PRE, json_str, strlen(json_str)+1);

    pthread_mutex_lock (&mContext.write_mutex);
    mContext.transmit_message=msg+LWS_PRE;
    mContext.transmit_size=strlen(json_str);
    pthread_mutex_unlock (&mContext.write_mutex);

    // send message to moolticuted
    lws_callback_on_writable(mContext.wsi);
    while(strlen(mContext.app.name)==0 || strlen(mContext.app.version)==0)
    {
      usleep(100);
    }
  }

  pthread_mutex_lock(&mContext.write_mutex);
  strncpy(name, mContext.app.name,200);
  strncpy(version, mContext.app.version,200);
  pthread_mutex_unlock(&mContext.write_mutex);

  return 0;
}
