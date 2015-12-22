#ifndef	_UPDATE_H_
#define _UPDATE_H_

#define DEMOCFG_SPI_FLASH_DEV_NAME		"spiflash:"
#define DEMOCFG_NEW_IMAGE_OFFSET      	(256 * 1024)    /* 256K */
#define DEMOCFG_NEW_IMAGE_AREA_LEN    	(512 * 1024)    /* 512K */
#define DEMOCFG_UPDATE_BUFFER_SIZE    	(1024 * 1)
#define DEMOCFG_SPI_FLASH_CLK_MODE    	SPI_CLK_POL_PHA_MODE0
#define DEMOCFG_SPI_FLASH_CLK_FREQ    	20000000
#define DEMOCFG_SPI_FLASH_CHANNEL     	"spi1:"

#ifndef min
    #define min(a, b)   ((a) < (b) ? (a) : (b))
#endif

#ifndef max
    #define max(a, b)   ((a) > (b) ? (a) : (b))
#endif


#endif