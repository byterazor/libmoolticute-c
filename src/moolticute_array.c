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
* @file moolticute_errors.c
* @brief Source File for dynamic array management of moolticute client
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*/
#include <pthread.h>
#include "moolticute.h"

/**
* @brief initialize the error and value structure in context
*
* @param ctx - the moolticute context
*
* @return 0 = everything is fine, -1 error
*/
int moolticute_array_init(struct moolticute_ctx *ctx)
{
  ctx->errors=calloc(sizeof(struct moolticute_error *), MOOLTICUTE_ERROR_BLOCK);
  if (ctx->errors==NULL)
  {
    return -1;
  }

  ctx->errors_size=MOOLTICUTE_ERROR_BLOCK;

  ctx->values=calloc(sizeof(struct moolticute_value *), MOOLTICUTE_VALUE_BLOCK);
  if (ctx->values==NULL)
  {
    free(ctx->errors);
    return -1;
  }

  ctx->values_size=MOOLTICUTE_VALUE_BLOCK;


  return 0;
}

/**
* @brief add a new error to the array
*
* @param ctx - the moolticute context to use
* @param type - the message type producing the error
* @param error_code - the error code of the failure message
* @param error_msg - the error message of the failure message
*
* @return 0 = everything is fine, -1 error
*/
int moolticute_error_add(struct moolticute_ctx *ctx, enum moolticute_message_types type, int error_code, const char *error_msg)
{
  struct moolticute_error *error;
  int i=0;
  pthread_mutex_lock (&ctx->write_mutex);
  while(ctx->errors[i]!=NULL && i < ctx->errors_size)
  {
    i++;
  }

  // check if we have to increase array size
  if (i == ctx->errors_size-1)
  {
    ctx->errors=realloc(ctx->errors, ctx->errors_size+MOOLTICUTE_ERROR_BLOCK);
    ctx->errors_size+=MOOLTICUTE_ERROR_BLOCK;
    i++;
  }

  // create a new error element/**
  error=malloc(sizeof(struct moolticute_error));
  if (error == NULL)
  {
    return -1;
  }
  error->message_type=type;
  printf("type: %d\n",type);
  error->error_code=error_code;
  error->error_msg=calloc(1,strlen(error_msg)+1);
  if (error->error_msg==NULL)
  {
    free(error);
    return -1;
  }
  strcpy(error->error_msg, error_msg);
  ctx->errors[i]=error;

  pthread_mutex_unlock (&ctx->write_mutex);

  return 0;
}

/**
* @brief add a new value to the array
*
* @param ctx - the moolticute context to use
* @param type - the message type producing the error
* @param data - the return value
*
* @return 0 = everything is fine, -1 error
*/
int moolticute_value_add(struct moolticute_ctx *ctx, enum moolticute_message_types type, void *data)
{
  struct moolticute_value *value;
  int i=0;
  pthread_mutex_lock (&ctx->write_mutex);
  while(ctx->values[i]!=NULL && i < ctx->values_size)
  {
    i++;
  }

  // check if we have to increase array size
  {
    ctx->values=realloc(ctx->values, ctx->values_size+MOOLTICUTE_VALUE_BLOCK);
    ctx->values_size+=MOOLTICUTE_VALUE_BLOCK;
    if (i == ctx->values_size-1)
    i++;
  }

  // create a new error element/**
  value=malloc(sizeof(struct moolticute_value));
  if (value == NULL)
  {
    return -1;
  }
  value->message_type=type;
  value->value=data;

  ctx->values[i]=value;

  pthread_mutex_unlock (&ctx->write_mutex);

  return 0;
}


/**
* @brief delete an error element from the error array in ctx
*
* @param ctx - the context of the error array
* @param index - the index to delete
*/
void moolticute_delete_error(struct moolticute_ctx *ctx, int index)
{
  if (ctx == NULL)
  {
    return;
  }

  if (ctx->errors_size==0)
  {
    return;
  }

  if (ctx->errors[index]==NULL)
  {
    return;
  }

  if (ctx->errors[index]->error_msg!= NULL)
  {
    free(ctx->errors[index]->error_msg);
  }
  free(ctx->errors[index]);

  ctx->errors[index]=NULL;

}

/**
* @brief delete a value element from the values array in ctx
*
* @param ctx - the context of the value array
* @param index - the index to delete
*/
void moolticute_delete_value(struct moolticute_ctx *ctx, int index)
{
  if (ctx == NULL)
  {
    return;
  }

  if (ctx->values_size==0)
  {
    return;
  }

  if (ctx->values[index]==NULL)
  {
    return;
  }

  if (ctx->values[index]->value!= NULL)
  {
    free(ctx->values[index]->value);
  }
  free(ctx->values[index]);

  ctx->values[index]=NULL;

}


/**
* @brief delete the whole errors array
*
* @param ctx - the context of the error array
*/
void moolticute_free_errors(struct moolticute_ctx *ctx)
{
  int i=0;
  pthread_mutex_lock(&ctx->write_mutex);
  for(i=0; i<ctx->errors_size; i++)
  {
    moolticute_delete_error(ctx, i);
  }

  free(ctx->errors);
  ctx->errors=NULL;
  ctx->errors_size=0;
  pthread_mutex_unlock(&ctx->write_mutex);
}

/**
* @brief find an error to a given message type
*
* @param ctx - the context of the error array
* @param type - the message type
*
* @return index of the error message
* @return -1 if not found
*/
int moolticute_error_search(struct moolticute_ctx *ctx, enum moolticute_message_types type)
{
  int i=0;

  if (ctx == NULL)
  {
    return -1;
  }

  if (ctx->errors==NULL)
  {
    return -1;
  }
  for(i=0; i< ctx->errors_size; i++)
  {
    if (ctx->errors[i]!= NULL)
    {
      if (ctx->errors[i]->message_type==type)
      {
        return i;
      }
    }
  }
  return -1;
}

/**
* @brief find the value to a given message type
*
* @param ctx - the context of the error array
* @param type - the message type
*
* @return index of the value
* @return -1 if not found
*/
int moolticute_value_search(struct moolticute_ctx *ctx, enum moolticute_message_types type)
{
  int i=0;

  if (ctx == NULL)
  {
    return -1;
  }

  if (ctx->values==NULL)
  {
    return -1;
  }
  for(i=0; i< ctx->values_size; i++)
  {
    if (ctx->values[i]!= NULL)
    {
      if (ctx->values[i]->message_type==type)
      {
        return i;
      }
    }
  }
  return -1;
}
