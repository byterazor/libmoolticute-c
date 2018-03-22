
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

#define MAX_CALLBACKS 30          /// defines the maximum number of supported callbacks
#define CARD_ID_SIZE 64           /// the size of the mooltipass card id in bytes
#define MOOLTICUTE_CMD_SIZE 30    /// the maximum size of moolticute websocket commands in bytes
#define MAX_MOOLTIPASS_SERVICE_NAME 250 /// the maximum length of a moolitpass service name
#define MAX_PASSWORD_LENGTH 500   /// Maximum number of chars per password, required for allocationg enough space
#define MOOLTICUTE_ERROR_BLOCK 10 /// Number of elements increased when error array is full
#define MOOLTICUTE_VALUE_BLOCK 10 /// Number of elements increased when value array is full


/*
* ERROR Codes
*/
#define M_ERROR_NOT_CONNECTED -1
#define M_ERROR_TIMEOUT -2
#define M_ERROR_NO_MOOLTIPASS_DEVICE -3
#define M_ERROR_DEVICE_LOCKED -4
#define M_ERROR_ALLOCATING_MEMORY -5
#define M_ERROR_SERVICE_NAME_TOO_LONG -6
#define M_ERROR_NO_SERVICE_NAME -7
#define M_ERROR_NO_MEMORY -8
#define M_ERROR_NO_SERVICE -9
#define M_ERROR_NO_CREDENTIAL -10
#define M_ERROR_NO_CARD -11
#define M_ERROR_PASSWORD_NOT_FOUND -12
#define M_ERROR_APPROVAL_REQUIRED -13
#define M_ERROR_UNKNOWN_ERROR -14

/**
* @brief all the available message types
*/
enum moolticute_message_types {
  PARAM_CHANGED,
  MP_CONNECTED,
  MP_DISCONNECTED,
  STATUS_CHANGED,
  VERSION_CHANGED,
  CARD_DB_METADATA,
  MEMORYMGMT_DATA,
  MEMORYMGMT_CHANGED,
  GET_APPLICATION_ID,
  PROGRESS_DETAILED,
  PROGRESS,
  START_MEMORYMGMT,
  ASK_PASSWORD
};

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
  int updating;                                 /// update process is running
};

/**
* @brief structure repesenting the login credentials for a service within the mooltipass device
*/
struct mooltipass_credential
{
  char address[2];          /// address of the element within the multipass device
  char date_created[11];    /// creation date of this element as a string
  char date_last_used[11];  /// date of the last use of this element as a string
  char *description;        /// pointer to a string describing this element, be sure to allocate enough memory
  int favourite;            /// is this elemnt a favourite ? 1=yes, 0=no
  char *login;              /// pointer to the string containing the login name
  char *password;           /// pointer to the string containing the password

  struct mooltipass_credential *pPrevious; /// previous element in the list
  struct mooltipass_credential *pNext;     /// next element in the list
};

/**
* @brief structure representing a service within the mooltipass device
*/
struct mooltipass_service
{
  char *name;               /// pointer to the service name
  int credentials;          /// the number of credentials

  struct mooltipass_credential  *pFirstCredential;  /// pointer to the first credential element
  struct mooltipass_credential  *pLastCredential;   /// pointer to the last credential element

  struct mooltipass_service *pPrevious;            /// pointer to the previous service element
  struct mooltipass_service *pNext;                /// pointer to the next service element
};

/**
* @brief structure representing the memory within the mooltipass device
*/
struct mooltipass_memory
{
  int services;                                 /// number of services with the moolipass device
  struct mooltipass_service *pFirstService;     /// pointer to the first service
  struct mooltipass_service *pLastService;      /// pointer to the last service
};

/**
* @brief structure for molticute application info
*/
struct moolticute_app_info
{
  char name[200];
  char version[200];
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
  struct mooltipass_mm mm;
  struct mooltipass_memory *memory;
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
* @brief structure representing an error received from moolticute daemon
*/
struct moolticute_error
{
  enum moolticute_message_types message_type;
  int error_code;
  char *error_msg;
};

/**
* @brief structure representing an return value from the mollticute daemon
*/
struct moolticute_value
{
  int message_type;
  void *value;
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
  pthread_mutex_t write_mutex;  /// mutex for controlling access to this structure from the writing phase

  // callbacks
  struct moolticute_cb callbacks[MAX_CALLBACKS];
	int cb_nr;
  char *transmit_message;
  int transmit_size;

  // error messages
  struct moolticute_error **errors;
  int errors_size;

  // return values
  struct moolticute_value **values;
  int values_size;

  // connection states
  int connected;                /// connection has been initiated
  int ready;                    /// context is ready for transmitting
  int finish;                   /// used for closing everything
  int finished;                 /// everything has been closed
  struct moolticute_app_info app; /// remote app information
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
extern struct lws_protocols protocols[];

/*
*   All available functions
*/
int moolipass_add_credential(struct mooltipass_service *service, const char address[2], const char *date_created, const char *date_used,
                            const char *description, int favourite, const char *login, const char *password);

int mooltipass_del_credential(struct mooltipass_service *service, struct mooltipass_credential *credential);
int mooltipass_delete_all_credentials(struct mooltipass_service *service);
struct mooltipass_service* mooltipass_add_service(struct mooltipass_memory *memory, const char *service_name);
int mooltipass_del_service(struct mooltipass_memory *memory, struct mooltipass_service *service);
int mooltipass_delete_all_services(struct mooltipass_memory *memory);
struct mooltipass_memory * mooltipass_new_memory();
void mooltipass_free_memory(struct mooltipass_memory *memory);

void moolticute_init_ctx();   /// initialize the context
int  moolticute_connect();    /// connect to the moolticuted and fetch initial information


int moolticute_request_random_number(); /// request random numbers from mooltipass device
int moolticute_request_device_uid(char key[32]);  /// request device uid from mooltipass device
int moolticute_start_memory_management(int want_data, int wait); /// initiate memory management mode on the device
int moolticute_stop_memory_management(int wait); /// stop memory management mode on the mooltipass device
int moolticute_request_password(const char *service, const char *login, char *password, int timeout);
int moolticute_get_application_id(char *name, char *version); /// request the application id from daemon
int moolticute_cmd_service_exist(char *service, int data_node);
#endif
