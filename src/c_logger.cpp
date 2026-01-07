//c_logger.c
#include "c_logger.h"

time_t timer;
struct tm* tm_info;
char time_stamp[26];

const char* levels[] = {"CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG", "NOTSET"};

int logger(char* str, int level){
    static int index = 0;
    int return_val = 0;
    FILE* fptr;
    if (level<=LOG_LEVEL){
        timer = time(NULL);
        tm_info = localtime(&timer);
        
        strftime(time_stamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        fptr = fopen(LOGFILE, "a");
        if (fptr==NULL){
            printf("File write error\n");
            return_val = -1;
        }else{
            fprintf(fptr, "%d [%s] %s: %s\n", index, time_stamp, levels[level], str);
            index++;
            //printf("write file: %s: %s\n", time_stamp, str);
        }
        fclose(fptr);
    }
    error_logger(str, level);
    return return_val;
}


int error_logger(char* str, int level){
    static int error_index = 0;
    int return_val = 0;
    FILE* fptr;
    if (level<=2){
        fptr = fopen(ERROR_LOGFILE, "a");
        if (fptr==NULL){
            printf("File write error\n");
            return_val = -1;
        }else{
            fprintf(fptr, "(%d) %s\n", error_index, str);
            error_index++;
        }
        fclose(fptr);
    }
    return return_val;
}


int node_state_data_logger(char* str){
    static int nsd_index = 0;
    int return_val = 0;
    FILE* fptr;
    fptr = fopen(NODE_STATE_DATA_LOGFILE, "a");
    if (fptr==NULL){
        printf("File write error\n");
        return_val = -1;
    }else{
        fprintf(fptr, "%s\n",  str);
        nsd_index++;
    }
    fclose(fptr);
    return return_val;
}   
