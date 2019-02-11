#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
 
#include "asfvolt_dev.h"
 
void show_cpld_Data(cpld_data_arg_t *pData)
{
    printf("Show CPLD Data: address=%02X, reg=%02X  value=%02X\n",pData->address,pData->reg,pData->value);	
}
 
void get_cpld_data(int fd,cpld_data_arg_t *pdata)
{
    cpld_data_arg_t data;
    data.address=pdata->address;
    data.reg=pdata->reg;
    if (ioctl(fd, CPLD_DATA_GET_VARIABLES, &data) == -1)
    {
        printf(" Fail to get_cpld_data !!");
    }
    else
    {
        printf("OK to get_cpld_data !!\n");
		pdata->value=data.value;
		show_cpld_Data(pdata);
    }
}

void set_cpld_data(int fd,cpld_data_arg_t *pdata)
{
    cpld_data_arg_t data;
    data.address=pdata->address;
    data.reg=pdata->reg;	
	data.value=pdata->value;	
    if (ioctl(fd, CPLD_DATA_SET_VARIABLES, &data) == -1)
    {
        printf(" Fail to set_cpld_data !!");
    }
    else
    {
        printf("OK to set_cpld_data !!\n");
		show_cpld_Data(&data);
		memcpy(pdata,&data,sizeof(cpld_data_arg_t));
    }
}

void print_uasge(char *cmd)
{
 printf("Usage: %s \n", cmd);
 printf("     option: ");
 printf("           -h  : help message\n");
 printf("           -1 [ <address>  ] :  \n");
 printf("           -2 [ <reg>  ] :  \n");
 printf("           -3 [ < value>  ] :  \n"); 
 printf("           -g  :  cpld get\n");
 printf("           -s  :  cpld set\n"); 
 printf("     Examole:\n"); 
 printf("      To Get CPLD: %s  -g -1 <address> -2 <reg> \n", cmd);
 printf("      To Set CPLD: %s  -s -1 <address> -2 <reg> -3 <value> \n", cmd);
 printf("\n") ;
}

enum{
    o_help,
	o_ioctl_get,
	o_ioctl_set,
	o_read,
	o_write
    } option;


	

int main (int argc, char **argv)
{
  char *file_name = "/dev/dev_asfvolt";
  int fd;	
  int rc=0,result;
  long ret_value;
  opterr = 0;    
  cpld_data_arg_t opt_data={0x01,0x02,0x00};
  option=o_ioctl_get;

    
  if (argc == 1)
  {
		print_uasge( argv[0]);
		goto addr_end;
  }
  else
  {
	  
    while( (result = getopt(argc, argv, "gs1:2:3:")) != -1 )
    {
           switch(result)
          {
               case '1':
                   printf("option=1, optopt=%c, optarg=%s\n", optopt, optarg);
				   opt_data.address=(unsigned char)strtol(optarg, NULL, 16); 				   
                   break;
              case '2':
                   printf("option=2, optopt=%c, optarg=%s\n", optopt, optarg);
				   opt_data.reg=(unsigned char)strtol(optarg, NULL, 16); 				   
                   break;
              case '3':
                   printf("option=1, optopt=%c, optarg=%s\n", optopt, optarg);
				   opt_data.value=(unsigned char)strtol(optarg, NULL, 16); 
                   break;
              case 'g':
                    printf("result=g, optopt=%c, optarg=%s\n", optopt, optarg);
					option=o_ioctl_get;
                    break;
              case 's':
                    printf("result=s, optopt=%c, optarg=%s\n", optopt, optarg);
					option=o_ioctl_set;
                    break;
			default:
                   printf("default, result=%c\n",result);
                   break;
           }
        printf("argv[%d]=%s\n", optind, argv[optind]);
    }	  
	  
  }	 

  printf("Show before call commmand: option: %d\n",option); 
  show_cpld_Data(&opt_data);
  
  fd = open(file_name, O_RDWR);
  if (fd == -1)
    {
        printf("[%s]:fail to open",argv[0]);
        rc=2;
		goto addr_end;
    }  

  switch (option)
  {
	case o_ioctl_get:
            get_cpld_data(fd,&opt_data);
            break;
	case o_ioctl_set:
            set_cpld_data(fd,&opt_data);
            break;
    default:
            break;
    }	
	
  close (fd);
	
addr_end: 
  return rc;
  
  
}

