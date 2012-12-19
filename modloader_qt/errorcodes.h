/**********************************************************
 * errorcodes.h
 *
 * Author: Jason Gassel
 * Desc: Defines error codes for use in return values and
 *       error messages.
 **********************************************************/

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
}

#endif // ERRORCODES_H
