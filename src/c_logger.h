//c_logger.h
#ifndef LOGGER_C
#define LOGGER_C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define LOG_LEVEL 4

static const char LOGFILE[]                          = "logs/main_log.txt";
static const char ERROR_LOGFILE[]                    = "logs/error_log.txt";
static const char NODE_STATE_DATA_LOGFILE[]          = "logs/node_state_data.txt";

int logger(char* str, int level);
int error_logger(char* str, int level);
int node_state_data_logger(char* str);

#endif
