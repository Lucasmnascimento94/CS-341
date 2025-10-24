#ifndef CONF_H
#define CONF_H
#include "ftd2xx.h"

enum default_conf{
    stop_bits       = FT_STOP_BITS_2,
    data_length     = FT_BITS_8,
    parity          = FT_PARITY_NONE,
    baud_rate       = FT_BAUD_9600,
    time_out        = 100,
    flow_control    = FT_FLOW_NONE, 
};

#endif