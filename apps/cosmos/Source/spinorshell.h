#ifndef	_SPINORSHELL_H_
#define _SPINORSHELL_H_

//#define FSCFG_ENABLE_NFTL		1
#define NF_DEVICE	"nftl:" 

int32_t Shell_fs_open(int32_t argc, char_ptr argv[] );
int32_t Shell_fs_close(int32_t argc, char_ptr argv[] );
int32_t Shell_spinor_erase_chip(int32_t argc, char_ptr argv[] );

int32_t  Shell_test_bigfile(int32_t argc, char_ptr argv[] );
int32_t  Shell_test_smallfiles(int32_t argc, char_ptr argv[] );
int32_t  Shell_drivertest(int32_t argc, char_ptr argv[] );
int32_t  Shell_nftl_test(int32_t argc, char_ptr argv[] );

int32_t  nftl_init();

#endif