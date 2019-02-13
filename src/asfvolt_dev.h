#ifndef ASFVOLT_DEV_H
#define ASFVOLT_DEV_H
#include <linux/ioctl.h>

#define ASFVOLT_DEV_IOC_MAGIC 'k'

typedef struct
{
    unsigned char address;
    unsigned char reg;
    unsigned char value;
} cpld_data_arg_t;

#define CPLD_DATA_GET_VARIABLES _IOR(ASFVOLT_DEV_IOC_MAGIC, 1, cpld_data_arg_t *)
#define CPLD_DATA_SET_VARIABLES _IOW(ASFVOLT_DEV_IOC_MAGIC, 2, cpld_data_arg_t *)

#endif
