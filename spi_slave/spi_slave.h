#ifndef _SPI_SLAVE_H
#define _SPI_SLAVE_H

#include <stdint.h>

#define SLAVE_MAX_ADDR 15

typedef struct _slave_info
{
    uint32_t acces_reg;
    uint32_t reg_data[SLAVE_MAX_ADDR];
} slave_info_t;

void spi_master_init(void);
void spi_slave_init(void);
void spi_write_reg(uint32_t reg, uint32_t data);
uint32_t spi_read_reg(uint32_t reg);

#endif
