#include "../moolticute.h"
#include <json.h>
#include <stdio.h>

void moolticute_cb_not_found(void *user, struct json_object *jObj)
{
  printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jObj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}
