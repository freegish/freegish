#ifndef GISH_CONFIG_H
#define GISH_CONFIG_H

#ifdef __linux__
 #define LINUX 1
#endif

#ifdef _WIN32
  #define WINDOWS 1
#endif

#ifdef __APPLE__
  #define MAC 1
#endif

#ifdef MAC
  #define __MACOSX__
#endif

#ifdef WINDOWS
  #define INLINE _inline
#else
  #define INLINE inline
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#if !defined(min) && !defined(_WIN32)
#define min(x1,x2) ((x1) > (x2) ? (x2):(x1))
#endif

#if !defined(max) && !defined(_WIN32)
#define max(x1,x2) ((x1) > (x2) ? (x1):(x2))
#endif

typedef enum {FALSE = 0, TRUE = 1} bool;

#endif /* GISH_CONFIG_H */
