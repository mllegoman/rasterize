#include <stdint.h>

#define s_int int128_t
#define s_uint uint128_t
#define s_size 16
#ifndef INT128_MAX
#define s_int int64_t
#define s_uint uint64_t
#define s_size 8
#ifndef INT64_MAX
#define s_int int32_t
#define s_uint uint32_t
#define s_size 4
#ifndef INT32_MAX
#define s_int int16_t
#define s_uint uint16_t
#define s_size 2
#ifndef INT16_MAX
#define s_int int8_t
#define s_uint uint8_t
#define s_size 1
#endif
#endif
#endif
#endif

#include <stdlib.h>
#include <stdio.h>
