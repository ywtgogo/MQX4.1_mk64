#include "global.h"
#include "task.h"
#include <shell.h>
#include "rf_type_cmd.h"

extern void reset_cpu();
extern void reset_rf();
extern void reset_wifi();
int32_t Shell_reboot(int32_t argc, char *argv[] )
{ /* Body */
    bool                print_usage, shorthelp = FALSE;
    int32_t             return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

    if (!print_usage)
    {
		if (argc == 2)
		{
			if (strcmp (argv[1], "cpu") == 0)
			{
				reset_cpu() ;
			}
			else if (strcmp (argv[1], "wifi") == 0)
			{
				reset_wifi();
			}
			else if (strcmp (argv[1], "rf") == 0)
			{
				reset_rf();
			}
			else if (strcmp (argv[1], "rs485") == 0)
			{
				;
			}
			else
			{
				printf ("Unknown reboot command!\n");
				return_code = SHELL_EXIT_ERROR;
			}
        }else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
    }

	if (print_usage)  {
		if (shorthelp)  {
			printf("%s [<cpu|wifi|rf|rs485>] \n", argv[0]);
		} else  {
			printf("Usage: %s [<cpu|wifi|rf|rs485>] \n", argv[0]);
		}
	}
	return return_code;
}

extern uint32_t systime_get(void);
int32_t Shell_systime(int32_t argc, char *argv[])
{
    bool                print_usage, shorthelp = FALSE;
    int32_t             return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

	if (!print_usage)  {
		if (argc >  1) {
			 printf("Error, invalid number of parameters\n");
			 return_code = SHELL_EXIT_ERROR;
			 print_usage=TRUE;
		}else {
			systime_get();
		}
	}
	if (print_usage)  {
		if (shorthelp)  {
			printf("%s \n", argv[0]);
		} else  {
			printf("Usage: %s \n", argv[0]);
		}
	}
	return return_code;
}

int32_t Shell_block(int32_t argc, char *argv[]) 
{
    bool                print_usage, shorthelp = FALSE;
    int32_t             return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

	if (!print_usage)  {
		if (argc >  1) {
			 printf("Error, invalid number of parameters\n");
			 return_code = SHELL_EXIT_ERROR;
			 print_usage=TRUE;
		}else {
			/* 创建 DBLK_AT_TASK */
			user_task_abort("port_scan_task");
			_time_delay(500);
			_task_create(0, DBLK_AT_TASK, 0);
			user_task_abort("Shell");
			//_task_block();
			return_code = SHELL_EXIT_SUCCESS;
		}
	}
	if (print_usage)  {
		if (shorthelp)  {
			printf("%s \n", argv[0]);
		} else  {
			printf("Usage: %s \n", argv[0]);
		}
	}
	return return_code;
}

extern bool wifi_cfg_set(void);
int32_t Shell_wifi_cfgset(int32_t argc, char *argv[])
{
    bool                print_usage, shorthelp = FALSE;
    int32_t             return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

	if (!print_usage)  {
		if (argc >  1) {
			 printf("Error, invalid number of parameters\n");
			 return_code = SHELL_EXIT_ERROR;
			 print_usage=TRUE;
		}else {
    		wifi_cfg_set();
		}
	}
	if (print_usage)  {
		if (shorthelp)  {
			printf("%s \n", argv[0]);
		} else  {
			printf("Usage: %s \n", argv[0]);
		}
	}
	return return_code;
}

int32_t Shell_rf(int32_t argc, char *argv[]) 
{
    bool              print_usage, shorthelp = FALSE;
    int32_t           return_code = SHELL_EXIT_SUCCESS;	
//test by ywt
	static	int8_t	tstatus=TRUE;
	
	uint16_t	param;
	uint16_t 	targid = 0x0002;
	uint8_t 	cmd_apn = 0x51;
	_task_id	task_id;
//			user_task_abort("port_scan_task");
//			tstatus = FALSE;
//			if (argc == 1) return SHELL_EXIT_SUCCESS;
//			//task_id = _task_get_id_from_name("heart_beat_task");
//			//_task_destroy(task_id);		
	uchar ffdid[2];
	uchar rfdid[2];
	uchar reg0000[]= {0x00,0x00};
	uchar reg1000[]= {0x10,0x00};
	uchar reg2000[]= {0x20,0x00};
	uchar reg3000[]= {0x30,0x00};
	uchar reg4000[]= {0x40,0x00};
	uchar reg5000[]= {0x50,0x00};

	uchar addrparam[2]= {0};
	uchar regparam[2] = {0};
	
	print_usage = Shell_check_help_request(argc, argv, &shorthelp );	
	if(!print_usage) {
		if (argc > 6 || argc == 1) {
			 printf("Error, invalid number of parameters\n");
			 return_code = SHELL_EXIT_ERROR;
			 print_usage=TRUE;
		}else {
			user_task_abort("port_scan_task");
			
			if (argc >= 3){
				StrToHex(argv[2], ffdid);
				StrToHex(argv[3], rfdid);
			}
			if (!strcasecmp(argv[1],"scan")){
				user_task_create("port_scan_task");
			}
			else if (!strcasecmp(argv[1],"auto_setffdroute")){//cmd_setffdroute
				//cmd_setffdroute(ffdid, routetab);
				auto_setffdroute();
			}
			else if (!strcasecmp(argv[1],"auto_setrfdtab")){
				//cmd_setrfdtab(ffdid, rfdtab1);
				auto_setrfdtab();
			}
			else if (!strcasecmp(argv[1],"auto_setrfdbind")){
				auto_setrfdbind();
			}
			else if (!strcasecmp(argv[1],"setpoweroff")){
				cmd_setpvpoweroff(ffdid, rfdid);
			}
			else if (!strcasecmp(argv[1],"setpoweron")){
				cmd_setpvpoweron(ffdid, rfdid);
			}	
			else if (!strcasecmp(argv[1],"sync")){
				; 
			}
			else if (!strcasecmp(argv[1],"setreg")){
				if (argc == 6){
					StrToHex(argv[4], addrparam);
					StrToHex(argv[5], regparam);
					cmd_setreg(ffdid, rfdid, addrparam, regparam);
				}else{
					printf("\n参数输入错误");
				}
			}	
			else if (!strcasecmp(argv[1],"getreg")){
				if (argc == 5){
					if (!strcasecmp(argv[4],"ffff")){//自动查询gateway ID 并判断是否已经死机
						//check_gateway();
					}else if (!strcasecmp(argv[4],"0000")){
						cmd_getreg(ffdid, rfdid, reg0000);
					}else if (!strcasecmp(argv[4],"1000")){
						cmd_getreg(ffdid, rfdid, reg1000);
					}else if (!strcasecmp(argv[4],"2000")){
						cmd_getreg(ffdid, rfdid, reg2000);
					}else if (!strcasecmp(argv[4],"3000")){
						cmd_getreg(ffdid, rfdid, reg3000);
					}else{
						printf("\n请输入正确的地址参数");
					}
				}else{
					printf("\n请输入正确的地址参数");
				}
			}
			else{
				print_usage = TRUE;
			}
		}
	}
	
	if (print_usage)  {
		if (shorthelp)  {
			printf("%s <scan|setreg|getreg|auto_setffdroute|auto_setrfdtab|auto_rfdbind> \n", argv[0]);
		} else  {
			printf("\nUsage: rf <command> [<val>] ...\n");
			printf("  Commands: \n");
			printf("    scan \n");
			printf("        poll start \n");
			printf("    setreg <ffd> <rfd> <regaddr> <regval> \n");
			printf("        set rf reg \n");	
			printf("    getreg <ffd> <rfd> <regaddr> \n");
			printf("        read rf reg \n");				
			printf("    auto_setffdroute \n");
			printf("        set rf route by auto \n");	
			printf("    auto_setrfdtab \n");
			printf("        set rf rfdtab by auto \n");	
			printf("    auto_setrfdbind \n");
			printf("        set rf rfd bind to ffd by auto \n");
//			printf("  Parameters: \n");
		}
	}
	
	return return_code;
}
