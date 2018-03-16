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
* @file moolticute_cb_version_changed.c
* @brief C Source file for managing command version_changed
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"
#include <json.h>

void moolticute_cb_version_changed(struct json_object *jObj)
{
  struct json_object *data;
  struct json_object *flashObj;
  struct json_object *serialObj;
  struct json_object *versionObj;
  const char *flash_size;
  const char *serial;
  const char *version;

  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "flash_size", &flashObj);
  json_object_object_get_ex(data, "hw_serial", &serialObj);
  json_object_object_get_ex(data, "hw_version", &versionObj);

  flash_size=json_object_get_string(flashObj);
  serial=json_object_get_string(serialObj);
  version=json_object_get_string(versionObj);

  pthread_mutex_lock (&mContext.write_mutex);
  mContext.info.device.flash_size=strtol(flash_size, NULL, 10);
  mContext.info.device.hw_serial=strtol(serial, NULL, 10);
  strncpy(mContext.info.device.hw_version,version, strlen(version) < 30 ? strlen(version) : 30);
  pthread_mutex_unlock (&mContext.write_mutex);
}
