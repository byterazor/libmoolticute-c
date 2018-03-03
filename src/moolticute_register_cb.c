#include "moolticute.h"
#include <string.h>

void moolticute_register_cb(const char *cmd, void (*cb)(struct json_object *jObj))
{
  strncpy(mContext.callbacks[mContext.cb_nr].cmd, cmd, strlen(cmd));
  mContext.callbacks[mContext.cb_nr].cb=cb;
  mContext.cb_nr++;
}
