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
* @file moolticute_cmd_request_password.c
* @brief C Source file for requesting a password from the mooltipass device
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>
#include <libwebsockets.h>


/**
* @brief callback for the ask_password message
*/
void moolticute_cb_ask_password(struct json_object *jObj)
{
  int i;
  struct json_object *data;
  struct json_object *password;


  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "password", &password);

  pthread_mutex_lock(&mContext.write_mutex);
  strncpy(mContext.password, json_object_get_string(password),500);
  mContext.ask_password_running=0;
  pthread_mutex_unlock(&mContext.write_mutex);

}


/**
* @brief request password from the mooltipass device
*
* @param service - name of the service for which to request the password
* @param login   - the login for which to request the password
* @param password- password buffer to return the password by call by reference, user has to make sure memory is allocated
* @param timeout - timeout in seconds to wait for the user to acknowledge the request
*
* @return 0 - everything is fine, <0 ERROR Codes
*/
int moolticute_request_password(const char *service, const char *login, char *password, int timeout)
{
  const char *json_str;
  char *msg;
  struct json_object *jObj=NULL;
  struct json_object *data=NULL;
  int tout=timeout*10000;

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

  if (mContext.info.status.card_inserted == 0)
  {
    pthread_mutex_unlock(&mContext.write_mutex);
    return M_ERROR_NO_CARD;
  }

  if (mContext.info.status.locked == 1)
  {
    pthread_mutex_unlock(&mContext.write_mutex);
    return M_ERROR_DEVICE_LOCKED;
  }
  pthread_mutex_unlock(&mContext.write_mutex);

  jObj = json_object_new_object();
  data = json_object_new_object();

  json_object_object_add(data, "service", json_object_new_string(service));
  json_object_object_add(data, "login", json_object_new_string(login));

  json_object_object_add(jObj, "msg", json_object_new_string("ask_password"));
  json_object_object_add(jObj, "data", data);

  json_str=json_object_to_json_string(jObj);

  msg=malloc(LWS_PRE+strlen(json_str)+1);
  strncpy(msg+LWS_PRE, json_str, strlen(json_str)+1);

  pthread_mutex_lock(&mContext.write_mutex);
  mContext.transmit_message=msg+LWS_PRE;
  mContext.transmit_size=strlen(json_str);
  pthread_mutex_unlock(&mContext.write_mutex);

  //register the callback
  moolticute_register_cb("ask_password", &moolticute_cb_ask_password);

  pthread_mutex_lock(&mContext.write_mutex);
  memset(mContext.password,0,500);
  mContext.ask_password_running=1;
  pthread_mutex_unlock(&mContext.write_mutex);

  // send message to moolticuted
  lws_callback_on_writable(mContext.wsi);

  while(mContext.ask_password_running==1 && tout>0)
  {
    usleep(100);
    tout--;
  }

  if (tout == 0)
  {
    return M_ERROR_TIMEOUT;
  }

  pthread_mutex_lock(&mContext.write_mutex);
  strcpy(password, mContext.password);
  pthread_mutex_unlock(&mContext.write_mutex);
  return 0;
}
