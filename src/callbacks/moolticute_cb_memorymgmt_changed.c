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
* @file moolticute_cb_connected.c
* @brief C Source file for managing commands mp_connect and mp_disconnect
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"

void moolticute_cb_memorymgmt_changed(void *user, struct json_object *jObj)
{
  struct moolticute_ctx *ctx = (struct moolticute_ctx *) user;
  struct json_object *data;
  json_object_object_get_ex(jObj, "data", &data);

  if (strncmp(json_object_get_string(data),"true",4)==0)
  {
    pthread_mutex_lock (&ctx->write_mutex);
    ctx->info.mm.enabled=1;
    pthread_mutex_unlock (&ctx->write_mutex);
  }
  else
  {
    pthread_mutex_lock (&ctx->write_mutex);
    ctx->info.mm.enabled=0;
    if (ctx->info.memory != NULL)
    {
      mooltipass_free_memory(ctx->info.memory);
      ctx->info.memory=NULL;
    }
    pthread_mutex_unlock (&ctx->write_mutex);
  }
}
