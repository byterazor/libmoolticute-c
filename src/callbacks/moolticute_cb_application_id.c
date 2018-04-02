#include "../moolticute.h"
#include <json-c/json.h>
#include <stdio.h>

/**
* @brief callback for the get_application_id message
*/
void moolticute_cb_application_id(void *user, struct json_object *jObj)
{
  struct moolticute_ctx *ctx=(struct moolticute_ctx *) user;
  struct json_object *data;
  struct json_object *app_name;
  struct json_object *app_version;


  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "application_name", &app_name);
  json_object_object_get_ex(data, "application_version", &app_version);

  pthread_mutex_lock (&ctx->write_mutex);
  strncpy(ctx->app.name, json_object_get_string(app_name), 200);
  strncpy(ctx->app.version, json_object_get_string(app_version), 200);
  pthread_mutex_unlock (&ctx->write_mutex);
}
