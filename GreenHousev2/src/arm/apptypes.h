#ifndef __APPTYPES_H__
#define __APPTYPES_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <ghair.h>

typedef unordered_map<string, string> UserArgs_t;
typedef vector<string> URLParams_t;
typedef bool(*CallbackDevice_t)(GHAir *air, const UserArgs_t &args, string *output);
typedef void(*CallbackProxy_t)(const UserArgs_t &args, string *output);


#endif // __APPTYPES_H__

