/*
 * Copyright 2008, Network Appliance Inc.
 * Author: Jason McMullan <mcmullan <at> netapp.com>
 * Licensed under the GPL-2 or later.
 */

#include "mqx.h"
#include "bsp.h"
#include "spi_flash.h"

struct winbond_spi_flash_params {
	uint16_t	id;
	uint16_t	nr_blocks;
	const char	*name;
};

static const struct winbond_spi_flash_params winbond_spi_flash_table[] = {
	{
		.id			= 0x4017,
		.nr_blocks		= 128,
		.name			= "W25Q64",
	},
};

int32_t spi_flash_winbond_init(MQX_FILE_PTR spifd, struct spi_flash *flash, uint8_t *idcode)
{
	const struct winbond_spi_flash_params *params;
	int32_t i;

	for (i = 0; i < ARRAY_SIZE(winbond_spi_flash_table); i++) {
		params = &winbond_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(winbond_spi_flash_table)) {
		printf("SF: Unsupported Winbond ID %02x%02x\n",
				idcode[1], idcode[2]);
		return 1;
	}

    flash->name = params->name;
    flash->page_size = 256;
    flash->sector_size = 4096;
    flash->size = 4096 * 16 * params->nr_blocks;

	return 0;
}
