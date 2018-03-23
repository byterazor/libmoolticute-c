#include "../moolticute.h"
#include <json.h>
#include <stdio.h>

/**
* @brief callback for the get_application_id message
*/
void moolticute_cb_application_id(struct json_object *jObj)
{
  struct json_object *data;
  struct json_object *app_name;
  struct json_object *app_version;

  json_object_object_get_ex(jObj, "data", &data);
  json_object_object_get_ex(data, "application_name", &app_name);
  json_object_object_get_ex(data, "application_version", &app_version);

  pthread_mutex_lock (&mContext.write_mutex);
  strncpy(mContext.app.name, json_object_get_string(app_name), 200);
  strncpy(mContext.app.version, json_object_get_string(app_version), 200);
  pthread_mutex_unlock (&mContext.write_mutex);
}
