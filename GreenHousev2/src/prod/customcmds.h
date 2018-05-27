#ifndef __CUSTOMCMDS_H__
#define __CUSTOMCMDS_H__

#include <ghairdefs.h>


// Relay commands
#define AIR_CMD_RELAY_GET_STATE         AIR_CMD_IN_CUSTOM_10
#define AIR_CMD_RELAY_SET_STATE         AIR_CMD_IN_CUSTOM_11
#define AIR_CMD_RELAY_SET_MODE          AIR_CMD_IN_CUSTOM_12
#define AIR_CMD_RELAY_GET_MODE          AIR_CMD_IN_CUSTOM_13
#define AIR_CMD_RELAY_SET_MODE          AIR_CMD_IN_CUSTOM_14
#define AIR_CMD_RELAY_GET_OPERTIME      AIR_CMD_IN_CUSTOM_15
#define AIR_CMD_RELAY_SET_OPERTIME      AIR_CMD_IN_CUSTOM_16
// How many relays we have. Enumerate them in the GHRelay::GHRelay().


#endif // __CUSTOMCMDS_H__
