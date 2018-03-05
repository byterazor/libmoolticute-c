#ifndef __MOOLTICUTE_H__
#define __MOOLTICUTE_H__
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
* @file moolticute.h
* @brief Header File for the libmoolticute-c library
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
* All defines and structures can be found here
*/
#include <json.h>
#include <libwebsockets.h>
#include <pthread.h>

#define MAX_CALLBACKS 20          /// defines the maximum number of supported callbacks
#define CARD_ID_SIZE 64           /// the size of the mooltipass card id in bytes
#define MOOLTICUTE_CMD_SIZE 30    /// the maximum size of moolticute websocket commands in bytes


/*
* ERROR Codes
*/
#define M_ERROR_NOT_CONNECTED -1
#define M_ERROR_TIMEOUT -2
#define M_ERROR_NO_MOOLTIPASS_DEVICE -3


/**
* @brief Mooltipass Device specific information
*/
struct mooltipass_dev_version
{
  int flash_size;           /// the size of the devices flash
  int hw_serial;            /// hardware serial number of the device
  char hw_version[30];      /// the hardware version of the device
};

/**
* @brief Mooltipass Device status information
*/
struct mooltipass_status
{
  int connected;        /// is a device connected to the moolticute daemon
  int locked;           /// is the moolticute device locked or unlocked
  int card_inserted;    /// is a card inserted in the moolticute device
};

/**
* @brief Mooltipass Card information
*/
struct mooltipass_card
{
  char id[CARD_ID_SIZE];
  int credentialsDbChangeNumber;
  int dataDbChangeNumber;
};

/**
* @brief Mooltipass Device parameters
*/
struct mooltipass_parameters
{
  int keyboard_layout;
  int lock_timeout_enabled;
  int lock_timeout;
  int screensaver_enabled;
  int user_request_cancel;
  int user_interaction_timeout;
  int flash_screen;
  int offline_mode;
  int tutorial_enabled;
  int screen_brightness;
  int knock_enabled;
  int knock_sensitivity;
  int random_starting_pin;
  int hash_display;
  int lock_unlock_mode;
  int key_after_login_enabled;
  int key_after_login;
  int key_after_pass_enabled;
  int key_after_pass;
  int delay_after_key_enabled;
  int delay_after_key;
};

/**
* @brief Mooltipass memory management structure
*/
struct mooltipass_mm
{
  int enabled;
};

/**
* @brief Mooltipass Info Structure
*
* Combines dev_version, status and parameters in one structure
*/
struct mooltipass_info
{
  struct mooltipass_dev_version device;
  struct mooltipass_status status;
  struct mooltipass_card card;
  struct mooltipass_parameters parameters;
  struct moolitpass_mm mm;
};


/**
* @brief structure for finding the correct callback for the multicute websocket service
*/
struct moolticute_cb
{
  char cmd[MOOLTICUTE_CMD_SIZE];
  void (*cb)(struct json_object *jObj);
};

/**
* @brief context for connecting to the moolticuted service
*/
struct moolticute_ctx
{
  struct mooltipass_info info;
  struct lws_context *context;  /// libwebsockets context
  struct lws *wsi;              /// libwebsockets connection
  pthread_t thread;             /// threat for managing data reception from websocket service


  // callbacks
  struct moolticute_cb callbacks[MAX_CALLBACKS];
	int cb_nr;

  // connection states
  int tried;                    /// connection has been tried to establish
  int connected;                /// connection has been initiated
  int ready;                    /// context is ready for transmitting
  int finish;                   /// used for closing everything
  int finished;                 /// everything has been closed
};


extern struct moolticute_ctx mContext;   /// only one multipass device is supported, so the moolticute context can be hardcoded, do not access directly


/*
* All internal functions
*/
int callback_moolticute( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
void moolticute_register_cb(const char *cmd, void (*cb)(struct json_object *jObj));

/**
* protocol structure required by libwebsockets
*/
static struct lws_protocols protocols[] =
{
	{
		"moolticute-protocol",
		callback_moolticute,
		0,
		200,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};


/*
*   All available functions
*/
void moolticute_init_ctx();   /// initialize the context
int  moolticute_connect();    /// connect to the moolticuted and fetch initial information


int moolticute_request_random_number(); /// request random numbers from mooltipass device
int moolticute_request_device_uid(char key[32]);  /// request device uid from mooltipass device

#endif
