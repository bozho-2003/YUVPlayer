#ifndef INNER_MODULES_H
#define INNER_MODULES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
typedef int (*image_filter_get_interface_func)(void **ppIntf);
typedef int (*yuv_formater_get_interface_func)(void **ppIntf);


extern int inner_image_filters_num;
extern image_filter_get_interface_func inner_image_filters[];

extern int inner_formaters_num;
extern yuv_formater_get_interface_func inner_formaters[];



#endif // INNER_MODULES_H
