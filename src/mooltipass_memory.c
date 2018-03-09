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
* @file mooltipass_memory.c
* @brief C Source file for all multipass memory object operations
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*
*/
#include "moolticute.h"
#include <string.h>


/**
* @brief add a credential set, in most cases read from the mooltipass device to a service element
*
* @param service - the service to add the credential to
* @param address - the two byte address of the memory element within the mooltipass device
* @param date_created - the date this entry was created as a 10 byte string of the form yyyy-mm-dd
* @param date_used  - the date this entry was last used as a 10 byte string of the form yyyy-mm-dd
* @param description - a description of this entry as a string
* @param favourite - 1 if this element is a favourite, 0 if not
* @param login - login account/name as a string
* @param password - password as a string
*
* @return 0 - if everything went fine, <0 Error Codes
*/
int moolipass_add_credential(struct mooltipass_service *service, const char address[2], const char *date_created, const char *date_used,
                            const char *description, int favourite, const char *login, const char *password)
{

  struct mooltipass_credential *credential = malloc(sizeof(struct mooltipass_credential));
  int description_size=strlen(description);
  int login_size=strlen(login);
  int password_size=strlen(password);

  if (credential == NULL)
  {
    return M_ERROR_ALLOCATING_MEMORY;
  }

  credential->pPrevious=NULL;
  credential->pNext=NULL;
  memcpy(credential->address, address, 2);
  strncpy(credential->date_created, date_created, 11);
  strncpy(credential->date_last_used, date_used, 11);

  credential->description=malloc(description_size+1);
  if (credential->description == NULL)
  {
    free(credential);
    return M_ERROR_ALLOCATING_MEMORY;
  }
  strncpy(credential->description, description, description_size);

  credential->login=malloc(login_size+1);
  if (credential->login == NULL)
  {
    free(credential->description);
    free(credential);
    return M_ERROR_ALLOCATING_MEMORY;
  }
  strncpy(credential->login, login, login_size);

  credential->password=malloc(password_size+1);
  if (credential->password == NULL)
  {
    free(credential->description);
    free(credential->login);
    free(credential);
    return M_ERROR_ALLOCATING_MEMORY;
  }
  strncpy(credential->password, password, password_size);

  if (service->pLastCredential==NULL)
  {
    service->pLastCredential=credential;
    service->pFirstCredential=credential;
  }
  else
  {
    credential->pPrevious=service->pLastCredential;
    service->pLastCredential->pNext=credential;
    service->pLastCredential=credential;
  }

  service->credentials++;

  return 0;
}

/**
* @brief delete a credential entry from a service
*
* @param service - the service to delete the credential from
* @param credential - the credential to delete
*
* @return 0 = everything was fine, <0 Error Code
*/
int mooltipass_del_credential(struct mooltipass_service *service, struct mooltipass_credential *credential)
{

  if (service == NULL)
  {
    return M_ERROR_NO_SERVICE;
  }

  if (credential == NULL)
  {
    return M_ERROR_NO_CREDENTIAL;
  }

  if (credential->pPrevious != NULL)
  {
    credential->pPrevious->pNext=credential->pNext;
  }

  if (credential->pNext != NULL)
  {
    credential->pNext->pPrevious=credential->pPrevious;
  }

  if (service->pFirstCredential == credential)
  {
    service->pFirstCredential = credential->pNext;
  }

  if (service->pLastCredential == credential)
  {
    service->pLastCredential = credential->pPrevious;
  }

  service->credentials--;

  free(credential->description);
  free(credential->login);
  free(credential->password);
  free(credential);

  return 0;
}

/**
* @brief delete all credentials from a service structure
*
* @param service - the service from which all credential shall be deleted
*
* @return 0 = everything was fine, <0 Error Code
*/
int mooltipass_delete_all_credentials(struct mooltipass_service *service)
{
  if (service == NULL)
  {
    return M_ERROR_NO_SERVICE;
  }

  while(service->pFirstCredential != NULL)
  {
    mooltipass_del_credential(service, service->pFirstCredential);
  }

  return 0;
}


/**
* @brief adds a new service to the memory structure
*
* @param memory - the memory to add the service to
* @param service_name - the name of the service
*
* @return 0 - everything went fine , <0 Error Code
*/
struct mooltipass_service* mooltipass_add_service(struct mooltipass_memory *memory, const char *service_name)
{
  struct mooltipass_service *service = malloc(sizeof(struct mooltipass_service));
  int name_len = strlen(service_name);

  if (memory == NULL)
  {
    return NULL;
  }

  if (name_len == 0)
  {
    return NULL;
  }

  // error allocating memory
  if (service == NULL)
  {
    return NULL;
  }

  // check for service_name length
  if (name_len > MAX_MOOLTIPASS_SERVICE_NAME)
  {
    free(service);
    return NULL;
  }

  service->name=malloc(name_len+1);
  if (service->name == NULL)
  {
    free(service);
    return NULL;
  }
  memset(service->name,0,name_len+1);
  memcpy(service->name, service_name, name_len);
  service->credentials=0;
  service->pFirstCredential=NULL;
  service->pPrevious=NULL;
  service->pNext=NULL;

  if (memory->pLastService == NULL)
  {
    memory->pLastService=service;
    memory->pFirstService=service;
  }
  else
  {
    service->pPrevious=memory->pLastService;
    memory->pLastService->pNext=service;
    memory->pLastService=service;
  }

  memory->services++;

  return service;
}


/**
* @brief delete a service from the memories service list
*
* @param memory - the memory to delete the service from
* @param service - the service to delete
*
* @return 0 - everythings fine, <0 Error Code
*/
int mooltipass_del_service(struct mooltipass_memory *memory, struct mooltipass_service *service)
{

  if (memory == NULL)
  {
    return M_ERROR_NO_MEMORY;
  }

  if (service == NULL)
  {
    return M_ERROR_NO_SERVICE;
  }

  if (service->pPrevious != NULL)
  {
    service->pPrevious->pNext=service->pNext;
  }

  if (service->pNext != NULL)
  {
    service->pNext->pPrevious=service->pPrevious;
  }

  if (memory->pFirstService == service)
  {
    memory->pFirstService=service->pNext;
  }

  if (memory->pLastService == service)
  {
    memory->pLastService=service->pPrevious;
  }

  memory->services--;

  mooltipass_delete_all_credentials(service);

  free(service->name);
  free(service);

  return 0;
}


/**
* @brief delete all services of the given memory
*
* @param memory - pointer to the memory to delete all services from
*
* @return 0 - everything went well, <0 Error Code
*/
int mooltipass_delete_all_services(struct mooltipass_memory *memory)
{

  if (memory == NULL)
  {
    return M_ERROR_NO_MEMORY;
  }

  while(memory->pFirstService != NULL)
  {
    mooltipass_del_service(memory, memory->pFirstService);
  }

  return 0;
}


/**
* @brief create a new memory structure for holding all the mooltipass services
*
* @return NULL - something went wrong, otherwise a pointer to the memory structure
*/
struct mooltipass_memory * mooltipass_new_memory()
{
  struct mooltipass_memory *mem = malloc(sizeof(struct mooltipass_memory));

  // error allocating memory
  if (mem == NULL)
  {
    return NULL;
  }

  mem->services=0;
  mem->pFirstService=NULL;
  mem->pLastService=NULL;

  return mem;
}

/**
* @brief frees all the memory allocated by a moolipass memory structure
*
* @param memory - the memory structure to free
*/
void mooltipass_free_memory(struct mooltipass_memory *memory)
{
  mooltipass_delete_all_services(memory);
  free(memory);
}
