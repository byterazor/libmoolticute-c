#ifndef __MOOLTICUTE_ERRORS_H__
#define __MOOLTICUTE_ERRORS_H__
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
* @file moolticute_errors.h
* @brief Header File for error management
* @author Dominik Meyer <dmeyer@federationhq.de>
* @copyright 2018 by Dominik Meyer
*/

int moolticute_errors_init(struct moolticute_ctx *ctx);   /// initialize error structure in context
int moolticute_error_add(struct moolticute_ctx *ctx, enum moolticute_message_types type, int error_code, const char *error_msg);
void moolticute_delete_error(struct moolticute_ctx *ctx, int index);
void moolticute_free_errors(struct moolticute_ctx *ctx);
int moolticute_error_search(struct moolticute_ctx *ctx, enum moolticute_message_types type);

#endif
