/**********************************************************
 * errorcodes.h
 *
 * Copyright (C) 2012-2013 Jason Gassel. All rights reserved.
 *
 * This file is part of the Jackal Mod Manager.
 *
 * Jackal Mod Manager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jackal Mod Manager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jackal Mod Manager.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Desc: Defines error codes for use in return values and
 *       error messages.
 ******************************************************************************/

#ifndef ERRORCODES_H
#define ERRORCODES_H

typedef int ErrorCode;

namespace Error
{
static const int UNKNOWN                        = -1;
static const int NO_ERROR                       = 0;
static const int INVALID_MOD_ID                 = 1;
static const int FAILED_OPEN_VERSION_FILE       = 2;
static const int FAILED_OPEN_PLUGINS_FILE       = 3;
static const int FAILED_OPEN_RESOURCES_FILE     = 4;
static const int MOD_GAME_DIR_INVALID           = 5;
static const int FAILED_OPEN_MOD_ARCHIVE        = 6;
static const int FAILED_OPEN_MOD_CONFIG_FILE    = 7;
static const int FAILED_DELETE_MOD              = 8;
static const int FAILED_DELETE_SCRIPT_CACHE     = 9;
static const int MODS_NOT_LOADED                = 10;
static const int CANT_REMOVE_ENABLED_MOD        = 11;
static const int DATA_DIR_INVALID               = 12;
static const int FAILED_PARSE_MOD_CONFIG        = 13;
static const int INVALID_MOD_ARCHIVE_CONFIG         = 14;
}

#endif // ERRORCODES_H
