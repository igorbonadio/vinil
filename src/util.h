/** 
 *  @file	    util.h
 *  @brief    Some comuns defines and functions are defined here
 *  @author   Igor Bonadio
 */

#ifndef VINIL_UTIL_H_
#define VINIL_UTIL_H_

#define TRUE  1
#define FALSE 0

#ifndef _WIN32
  #define VINILAPI
#else
  #ifdef __cplusplus
    #define VINILAPI extern "C" __declspec(dllexport)
  #else
    #define VINILAPI __declspec(dllexport)
  #endif
#endif

#endif
