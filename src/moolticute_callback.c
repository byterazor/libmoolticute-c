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
* @file moolticute_callback.c
* @brief C Source file for the moolticute_callback function
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"
#include <json.h>

/**
* @brief function called by libwebsockets for some reasons including moolticute messages
* @param wsi - websocket information
* @param reason - reason why the callback has been called
* @param user - some user specific data structure
* @param in - buffer of incoming data
* @param len - size of the incoming data
*
*/
int callback_moolticute( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
  struct json_object *jobj;
  struct json_object *msg;
  const char *cmd;
  int i;
  int cb_found=0;

  switch( reason )
    {
      case LWS_CALLBACK_CLIENT_ESTABLISHED:
        mContext.tried=1;
        mContext.connected=1;
        lws_callback_on_writable( wsi );
        break;

      case LWS_CALLBACK_CLIENT_RECEIVE:
        jobj =json_tokener_parse((char *) in);
        json_object_object_get_ex(jobj, "msg", &msg);
        cmd=json_object_get_string(msg);

        // search for a callback for this command
        cb_found=0;
        for(i=0; i<mContext.cb_nr; i++)
        {
          if (strcmp(mContext.callbacks[i].cmd, cmd)==0 && mContext.callbacks[i].cb != NULL)
          {
            cb_found=1;
            mContext.callbacks[i].cb(jobj);
          }
        }

        // if no callback has been found, call the not_found callback
        if (cb_found==0)
        {
          mContext.callbacks[0].cb(jobj);
        }


        json_object_put(jobj);
        break;

      case LWS_CALLBACK_CLIENT_WRITEABLE:
        mContext.ready=1;
        break;
      case LWS_CALLBACK_CLOSED:
        break;
      case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        mContext.tried=1;
        mContext.connected=0;
        break;

      default:
        break;
    }
  return 0;
}
