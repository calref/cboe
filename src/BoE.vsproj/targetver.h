#pragma once

// This ensures functions unavailable in Windows XP will not be available at compile time

#define _WIN32_WINNT 0x0601
#include <SDKDDKVer.h>
