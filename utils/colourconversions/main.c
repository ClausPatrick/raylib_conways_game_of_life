//main.c
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

const char header[] = "#include <cstdint>";


static const char RGB_TABLE_FILENAME[]                 = "rgb_table.cpp";
static const char TABLE_NAME[]                         = "rgb_values";

int log_values(char* str, size_t size){
    static int index = 0;
    int return_val = 0;
    FILE* fptr;
    fptr = fopen(RGB_TABLE_FILENAME, "a");
    if (fptr==NULL){
        printf("File write error\n");
        return_val = -1;
    }else{
        if (index == 0){
            fprintf(fptr, "%s\n\n", header);
            fprintf(fptr, "const uint8_t %s[%d][6] =\n\t{{%s},\n", TABLE_NAME, size, str);
        }else if (index == size-1){
            fprintf(fptr, "\t{%s}};\n", str);
        }else{
            fprintf(fptr, "\t{%s},\n", str);
        }

        index++;
    }
    fclose(fptr);
    return return_val;
}


 
double max(double a, double b, double c){
    double max =  a > b ? a : b;
    return max > c ? max : c;
}

double min(double a, double b, double c){
    double min =  a < b ? a : b;
    return min < c ? min : c;
}

int rgb_to_hsv(double* h, double* s, double* v, double r, double g, double b){
    double c_max = max(r, g, b);
    double c_min = min(r, g, b);

    double delta = c_max - c_min;
    *v = c_max;
    if (delta < 0.00001){
        *s = 0;
        *h = 0;
        return 0;
    }
    if (c_max > 0.0){
        *s = (delta / c_max);
    }else{
        *s = 0.0;
        *h = -1;
        return 1;
    }
    if (r >= c_max){
        *h = (g - b) / delta;
    }else if(g >= c_max){
        *h = 2.0 + (b - r) / delta;
    }else{
        *h = 4.0 + (r - g) / delta;
    }
    *h *= 60;
    if (*h < 0.0){
        *h += 360;
    }
}

int hsv_to_rgb(double* r, double* g, double* b, double h, double s, double v){
    int h_flr;
    double fact, p, q, t;
    if (s < 0.00001){
        *r = v;
        *g = v;
        *b = v;
        return 0;
    }

    h /= 60.0;
    h_flr = floor(h);
    fact = h - h_flr;
    p = v * (1 - s);
    q = v * (1 - s * fact);
    t = v * (1 - s * (1 - fact));

    switch(h_flr){
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
    return 0;
    
}

int main(){

    double r, g, b, h, s, v, ret_val;
    char rgb_values[255];
    size_t size = 360;

    for (int i=0; i<size; i++){
        h = (double) i;
        s = 1;
        v = 255;
        ret_val = hsv_to_rgb(&r, &g, &b, h, s, v);
        ret_val = rgb_to_hsv(&h, &s, &v, r, g, b);
        sprintf(rgb_values, "%.0f, %.0f, %.0f", r, g, b);
        log_values(rgb_values, size);
    }

    return 0;
}

