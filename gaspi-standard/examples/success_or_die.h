#ifndef _SUCCESS_OR_DIE_H
#define _SUCCESS_OR_DIE_H 1

void success_or_die ( const char* file, const int line
                    , const int ec
                    );

#ifndef NDEBUG
#define ASSERT(ec) success_or_die (__FILE__, __LINE__, ec)
#else
#define ASSERT(ec) ec
#endif

#endif
