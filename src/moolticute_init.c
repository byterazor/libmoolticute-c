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
* @file moolticute_init.c
* @brief C Source file for the moolticute_init_ctx function
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"
#include "callbacks/moolticute_cb_not_found.h"
#include "callbacks/moolticute_cb_connected.h"
#include "callbacks/moolticute_cb_status_changed.h"
#include "callbacks/moolticute_cb_param_changed.h"
#include "callbacks/moolticute_cb_version_changed.h"
#include "callbacks/moolticute_cb_card_db_metadata.h"
#include "callbacks/moolticute_cb_memorymgmt_data.h"
#include "callbacks/moolticute_cb_memorymgmt_changed.h"
#include "callbacks/moolticute_cb_application_id.h"
#include "callbacks/moolticute_cb_progress.h"
#include "callbacks/moolticute_cb_failed_memorymgmt.h"
#include "moolticute_array.h"
#include <libwebsockets.h>


struct lws_protocols protocols[] =
{
	{
		"moolticute-protocol",
		callback_moolticute,
		0,
		2000,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

/**
* @brief initialize the moolticute context
*
* This function *has* to be called before any other function of the libmoolticute
* library.
*
*/
struct moolticute_ctx *moolticute_init_ctx()
{
  struct lws_context_creation_info info;
	struct moolticute_ctx *ctx = calloc(sizeof(struct moolticute_ctx),1);

  // blank everything before using it
  memset(&info, 0, sizeof(info));

  info.port = CONTEXT_PORT_NO_LISTEN;
  info.protocols = protocols;
  info.gid = -1;
  info.uid = -1;
	info.user= (void *) ctx;

  ctx->context=lws_create_context( &info );

  // initialize error structure in context
  moolticute_array_init(ctx);

  // register all callbacks here
  moolticute_register_cb(ctx,"",&moolticute_cb_not_found);
  moolticute_register_cb(ctx,"mp_connected", &moolticute_cb_connect);
  moolticute_register_cb(ctx,"mp_disconnected", &moolticute_cb_disconnect);
  moolticute_register_cb(ctx,"status_changed", &moolticute_cb_status_changed);
  moolticute_register_cb(ctx,"param_changed", &moolticute_cb_param_changed);
  moolticute_register_cb(ctx,"version_changed", &moolticute_cb_version_changed);
  moolticute_register_cb(ctx,"card_db_metadata", &moolticute_cb_card_db_metadata);
  moolticute_register_cb(ctx,"memorymgmt_data", &moolticute_cb_memorymgmt_data);
  moolticute_register_cb(ctx,"memorymgmt_changed", &moolticute_cb_memorymgmt_changed);
  moolticute_register_cb(ctx,"get_application_id", &moolticute_cb_application_id);
  moolticute_register_cb(ctx,"progress_detailed", &moolticute_cb_progress);
  moolticute_register_cb(ctx,"progress", &moolticute_cb_progress);
  moolticute_register_cb(ctx,"failed_memorymgmt", &moolticute_cb_failed_memorymgmt);

	return ctx;
}
