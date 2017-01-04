#include "inner_modules.h"

#include "format_i420.h"



#include "format_y8.h"
#include "format_y16.h"
#include "format_i420.h"
#include "format_yv12.h"
#include "format_i422.h"
#include "format_yv16.h"
#include "format_yuyv.h"
#include "format_uyvy.h"
#include "format_yuv444.h"
#include "format_rgb24.h"

#include "filter_test.h"
#include "filter_mirror.h"
#include "filter_conv.h"

int inner_formaters_num = 10;
yuv_formater_get_interface_func inner_formaters[] = {
    formater_i420_get_interface,
    formater_i422_get_interface,
    formater_yv16_get_interface,
    formater_yuyv_get_interface,
    formater_uyvy_get_interface,
    formater_yv12_get_interface,
    formater_y8_get_interface,
    formater_y16_get_interface,
    formater_yuv444_get_interface,
    formater_rgb24_get_interface,
};

int inner_image_filters_num = 3;
image_filter_get_interface_func inner_image_filters[] = {
    filter_test_get_interface,
    filter_mirror_get_interface,
    filter_conv_get_interface
};

