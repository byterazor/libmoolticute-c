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
* @file moolticute_cb_failed_memorymgmt.c
* @brief Source File for the moolticute_cb_failed_memorymgmt callback
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"
#include "moolticute_errors.h"
#include <json.h>
#include <stdio.h>

void moolticute_cb_failed_memorymgmt(struct json_object *jObj)
{
  struct json_object *data;
  struct json_object *failed;
  struct json_object *code;
  struct json_object *msg;

  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "failed", &failed);

  if (failed != NULL)
  {
    if (strncmp(json_object_get_string(failed),"true",4)==0)
    {
      json_object_object_get_ex(data, "error_code", &code);
      json_object_object_get_ex(data, "error_message", &msg);

      moolticute_error_add(&mContext, START_MEMORYMGMT, json_object_get_int(code), json_object_get_string(msg));
      printf("mm failed\n");
    }
    return;
  }

}
