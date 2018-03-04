#include "moolticute.h"
#include <string.h>

void moolticute_register_cb(const char *cmd, void (*cb)(struct json_object *jObj))
{
  int i=0;
  int found=0;
  /*
  * first check if a callback is already registered
  */
  for(i=0; i<mContext.cb_nr; i++)
  {
    if (strcmp(mContext.callbacks[mContext.cb_nr].cmd, cmd) == 0)
    {
      found=1;
      break;
    }
  }

  if (found == 0)
  {
    strncpy(mContext.callbacks[mContext.cb_nr].cmd, cmd, strlen(cmd));
    mContext.callbacks[mContext.cb_nr].cb=cb;
    mContext.cb_nr++;
  }
}
