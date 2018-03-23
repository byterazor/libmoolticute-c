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
* @file moolticute_cb_status_changed.c
* @brief C Source file for managing command status_changed
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>

void moolticute_cb_status_changed(struct json_object *jObj)
{
  struct json_object *data;
  const char *status;

  json_object_object_get_ex(jObj, "data", &data);
  status=json_object_get_string(data);

  pthread_mutex_lock (&mContext.write_mutex);
  if (strncmp(status, "Locked", 6) == 0)
  {
    mContext.info.status.locked=1;
  }
  else if (strncmp(status,"Unlocked",8) == 0)
  {
    mContext.info.status.locked=0;
  }
  else if (strncmp(status,"NoCardInserted",14)==0 )
  {
    mContext.info.status.card_inserted=0;
  }
  pthread_mutex_unlock (&mContext.write_mutex);
}
