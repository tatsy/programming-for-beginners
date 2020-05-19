#pragma once

#ifdef WIN32
#define __FUNC_NAME__ __FUNCTION__
#else
#define __FUNC_NAME__ __func__
#endif

#define NOT_IMPL_ERROR()                                        \
    do {                                                        \
        fprintf(stderr, "%s: not implemented! (%s, L%d)", __FUNC_NAME__, __FILE__, __LINE__); \
        abort();                                                \
    } while (1);
