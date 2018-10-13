#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

#include <stdint.h>

#define SLAVE_MAX_ADDR  14
#define SLAVE_ADDRESS   0x32

struct slave_info_t
{
    uint8_t acces_reg;
    uint8_t reg_data[SLAVE_MAX_ADDR];
};

void i2c_slave_init(void);
void i2c_master_init(void);
uint8_t i2c_write_reg(uint8_t reg, uint8_t *data_buf, size_t length);
uint8_t i2c_read_reg(uint8_t reg, uint8_t *data_buf, size_t length);

#endif
