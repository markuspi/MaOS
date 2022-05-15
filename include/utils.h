#pragma once

#ifdef __cplusplus
#define CPP_GUARD_START extern "C" {
#define CPP_GUARD_END }
#else
#define CPP_GUARD_START
#define CPP_GUARD_END
#endif
