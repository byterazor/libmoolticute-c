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
* @file moolticute_cb_param_changed.c
* @brief C Source file for managing command param_changed
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "../moolticute.h"
#include <json-c/json.h>

int isTrue(const char *value)
{
  if (strncmp(value, "true", 4)==0)
  {
      return 1;
  }
  else
  {
      return 0;
  }
}

void moolticute_cb_param_changed(void *user, struct json_object *jObj)
{
  struct moolticute_ctx *ctx = (struct moolticute_ctx *) user;
  struct json_object *data;
  struct json_object *paramObj;
  struct json_object *valueObj;
  const char *param;
  const char *value;

  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data,"parameter", &paramObj);
  json_object_object_get_ex(data,"value", &valueObj);
  param=json_object_get_string(paramObj);
  value=json_object_get_string(valueObj);

  pthread_mutex_lock (&ctx->write_mutex);
  if (strncmp(param, "delay_after_key", 15) == 0)
  {
    ctx->info.parameters.delay_after_key=strtol(value,NULL,10);
  }
  else if(strncmp(param, "delay_after_key_enabled", 23) == 0)
  {
    ctx->info.parameters.delay_after_key_enabled=isTrue(value);
  }
  else if(strncmp(param, "key_after_pass", 14) == 0)
  {
    ctx->info.parameters.key_after_pass=strtol(value,NULL,10);
  }
  else if(strncmp(param, "key_after_pass_enabled", 22) == 0)
  {
    ctx->info.parameters.key_after_pass_enabled=isTrue(value);
  }
  else if(strncmp(param, "key_after_login", 15) == 0)
  {
    ctx->info.parameters.key_after_login=strtol(value,NULL,10);
  }
  else if(strncmp(param, "key_after_login_enabled", 23) == 0)
  {
    ctx->info.parameters.key_after_login_enabled=isTrue(value);
  }
  else if(strncmp(param, "lock_unlock_mode", 16) == 0)
  {
    ctx->info.parameters.lock_unlock_mode=strtol(value,NULL,10);
  }
  else if(strncmp(param, "hash_display", 12) == 0)
  {
    ctx->info.parameters.hash_display=isTrue(value);
  }
  else if(strncmp(param, "random_starting_pin", 19) == 0)
  {
    ctx->info.parameters.random_starting_pin=isTrue(value);
  }
  else if(strncmp(param, "knock_enabled", 13) == 0)
  {
    ctx->info.parameters.knock_enabled=isTrue(value);
  }
  else if(strncmp(param, "knock_sensitivity", 17) == 0)
  {
    ctx->info.parameters.knock_sensitivity=strtol(value,NULL,10);
  }
  else if(strncmp(param, "screen_brightness", 17) == 0)
  {
    ctx->info.parameters.screen_brightness=strtol(value,NULL,10);
  }
  else if(strncmp(param, "user_interaction_timeout", 24) == 0)
  {
    ctx->info.parameters.user_interaction_timeout=strtol(value,NULL,10);
  }
  else if(strncmp(param, "tutorial_enabled", 16) == 0)
  {
    ctx->info.parameters.tutorial_enabled=isTrue(value);
  }
  else if(strncmp(param, "offline_mode", 12) == 0)
  {
    ctx->info.parameters.offline_mode=isTrue(value);
  }
  else if(strncmp(param, "flash_screen", 12) == 0)
  {
    ctx->info.parameters.flash_screen=isTrue(value);
  }
  else if(strncmp(param, "user_request_cancel", 19) == 0)
  {
    ctx->info.parameters.user_request_cancel=isTrue(value);
  }
  else if(strncmp(param, "screensaver", 11) == 0)
  {
    ctx->info.parameters.screensaver_enabled=isTrue(value);
  }
  else if(strncmp(param, "lock_timeout_enabled", 20) == 0)
  {
    ctx->info.parameters.lock_timeout_enabled=isTrue(value);
  }
  else if(strncmp(param, "lock_timeout", 12) == 0)
  {
    ctx->info.parameters.lock_timeout=strtol(value,NULL,10);
  }
  else if(strncmp(param, "keyboard_layout", 15) == 0)
  {
    ctx->info.parameters.keyboard_layout=strtol(value,NULL,10);
  }
  else
  {
    printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(data, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
  }
  pthread_mutex_unlock (&ctx->write_mutex);
}
