#ifndef LOGGING_H_
#define LOGGING_H_

#define DEBUG // Comment out to remove debug logs

#if defined DEBUG

#define LOG_DEBUG_ARGS(message, ...) \
  { printf("D %s: %s(): %d: ", __FILE__, __func__, __LINE__); printf(message, __VA_ARGS__); printf("\n"); }

#define LOG_DEBUG(message) \
  printf("D %s: %s(): %d: %s\n", __FILE__, __func__, __LINE__, message);

#else
#define LOG_DEBUG_ARGS(message, ...) \
  // Do nothing

#define LOG_DEBUG(message) \
  // Also do nothing

#endif

#endif