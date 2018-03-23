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
* @file moolticute_cb_memorymgmt_data.c
* @brief C Source file for managing memory changes
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json.h>

int mooltipass_penc_to_str(struct json_object *password_enc, char *password)
{
  int password_length=0;
  int i=0;

  if(password_enc==NULL || password == NULL)
  {
    return -1;
  }

  password_length=json_object_array_length(password_enc);
  memset(password,0,password_length+1);

  for(i=0; i<password_length; i++)
  {
    password[i]=json_object_get_int(json_object_array_get_idx(password_enc, i));
  }

  return password_length;
}

void moolticute_cb_memorymgmt_data(struct json_object *jObj)
{
  struct json_object *data=NULL;
  struct json_object *login_data=NULL;
  struct json_object *service=NULL;
  struct json_object *service_nameObj=NULL;
  struct json_object *childrenObj=NULL;
  struct json_object *credentialObj=NULL;
  struct json_object *addressObj=NULL;
  struct json_object *descriptionObj=NULL;
  struct json_object *dateCreatedObj=NULL;
  struct json_object *dateUsedObj=NULL;
  struct json_object *favouriteObj=NULL;
  struct json_object *loginObj=NULL;
  struct json_object *passwordObj=NULL;

  struct mooltipass_service *mService;
  const char *service_name;
  char address[2];
  char *password;


  int services=0;
  int credentials=0;
  int i=0;
  int r=0;

  if (jObj == NULL)
  {
    return;
  }

  json_object_object_get_ex(jObj, "data", &data);
  if (data == NULL)
  {
    return;
  }
  json_object_object_get_ex(data, "login_nodes", &login_data);
  if(login_data == NULL)
  {
    return;
  }

  services=json_object_array_length(login_data);

  if (services == 0)
  {
    return;
  }
  pthread_mutex_lock (&mContext.write_mutex);
  mContext.info.mm.updating=1;

  if (mContext.info.memory != NULL)
  {
    mooltipass_free_memory(mContext.info.memory);
    mContext.info.memory=NULL;
  }

  mContext.info.memory=mooltipass_new_memory();

  for(i=0; i<services; i++)
  {
    service = json_object_array_get_idx(login_data, i);
    json_object_object_get_ex(service, "service", &service_nameObj);
    service_name=json_object_get_string(service_nameObj);
    mService=mooltipass_add_service(mContext.info.memory, service_name);

    json_object_object_get_ex(service, "childs", &childrenObj);
    if(childrenObj != NULL)
    {
      credentials=json_object_array_length(childrenObj);
      if (credentials > 0)
      {
        for(r=0; r<credentials; r++)
        {
          credentialObj=json_object_array_get_idx(childrenObj, r);
          json_object_object_get_ex(credentialObj, "address", &addressObj);
          address[0]=json_object_get_int(json_object_array_get_idx(addressObj, 0));
          address[1]=json_object_get_int(json_object_array_get_idx(addressObj, 1));

          json_object_object_get_ex(credentialObj, "date_created", &dateCreatedObj);
          json_object_object_get_ex(credentialObj, "date_last_used", &dateUsedObj);
          json_object_object_get_ex(credentialObj, "description", &descriptionObj);
          json_object_object_get_ex(credentialObj, "favorite", &favouriteObj);
          json_object_object_get_ex(credentialObj, "login", &loginObj);
          json_object_object_get_ex(credentialObj, "password_enc", &passwordObj);
          password=malloc(json_object_array_length(passwordObj));
          mooltipass_penc_to_str(passwordObj, password);

          moolipass_add_credential(mService, address, json_object_get_string(dateCreatedObj),
                                   json_object_get_string(dateUsedObj),
                                   json_object_get_string(descriptionObj),
                                   json_object_get_int(favouriteObj),
                                   json_object_get_string(loginObj),
                                   password
        );

        }
      }
    }

  }

  mContext.info.mm.updating=0;
  pthread_mutex_unlock (&mContext.write_mutex);
}
