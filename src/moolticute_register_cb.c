#include "moolticute.h"
#include <string.h>

void moolticute_register_cb(struct moolticute_ctx *ctx,const char *cmd, void (*cb)(void *user,struct json_object *jObj))
{
  int i=0;
  int found=0;
  /*
  * first check if a callback is already registered
  */
  for(i=0; i<ctx->cb_nr; i++)
  {
    if (strcmp(ctx->callbacks[ctx->cb_nr].cmd, cmd) == 0)
    {
      found=1;
      break;
    }
  }

  if (found == 0)
  {
    strncpy(ctx->callbacks[ctx->cb_nr].cmd, cmd, strlen(cmd));
    ctx->callbacks[ctx->cb_nr].cb=cb;
    ctx->cb_nr++;
  }
}
