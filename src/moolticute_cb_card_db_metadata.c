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
* @file moolticute_cb_card_db_metadata.c
* @brief C Source file for managing command card_db_metadata
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"
#include <json.h>

void moolticute_cb_card_db_metadata(struct json_object *jObj)
{
  struct json_object *data;
  struct json_object *cardIDobj;
  struct json_object *DbChangeNumberObj;
  struct json_object *credentialsDbChangeNumberObj;
  const char *cardID;

  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "cardId", &cardIDobj);
  json_object_object_get_ex(data, "dataDbChangeNumber", &DbChangeNumberObj);
  json_object_object_get_ex(data, "credentialsDbChangeNumber", &credentialsDbChangeNumberObj);

  cardID=json_object_get_string(cardIDobj);
  memcpy(mContext.info.card.id,cardID,64);

  mContext.info.card.dataDbChangeNumber=json_object_get_int(DbChangeNumberObj);
  mContext.info.card.credentialsDbChangeNumber=json_object_get_int(credentialsDbChangeNumberObj);


}
