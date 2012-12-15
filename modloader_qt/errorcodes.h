/**********************************************************
 * errorcodes.h
 *
 * Author: Jason Gassel
 * Desc: Defines error codes for use in return values and
 *       error messages.
 **********************************************************/

#ifndef ERRORCODES_H
#define ERRORCODES_H

namespace Error
{
static const int UNKNOWN                        = -1;
static const int NO_ERROR                       = 0;
static const int FAILED_OPEN_VERSION_FILE       = 1;
static const int FAILED_OPEN_PLUGINS_FILE       = 2;
static const int FAILED_OPEN_RESOURCES_FILE     = 3;
}

#endif // ERRORCODES_H
