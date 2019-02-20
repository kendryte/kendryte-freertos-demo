#include <devices.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include "spi_slave.h"
#include "fpioa.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define CHIP_SELECT  1
handle_t spi0;
uintptr_t spi_stand;
handle_t spi_slave;

slave_info_t spi_slave_device;
spi_slave_handler_t spi_slave_handler;

void spi_slave_receive(uint32_t data)
{
    if(data & 0x40000000)
    {
        if (spi_slave_device.acces_reg < SLAVE_MAX_ADDR)
            spi_slave_device.reg_data[spi_slave_device.acces_reg] = data & 0x3FFFFFFF;
        spi_slave_device.acces_reg = SLAVE_MAX_ADDR;
    }
    else
    {
        spi_slave_device.acces_reg = (data & 0x3FFFFFFF);
    }
}

uint32_t spi_slave_transmit(uint32_t data)
{
    uint32_t ret = 0;
    spi_slave_device.acces_reg = (data & 0x3FFFFFFF);
    if (spi_slave_device.acces_reg < SLAVE_MAX_ADDR)
        ret = spi_slave_device.reg_data[spi_slave_device.acces_reg];
    else
        ret = 0xFF;
    spi_slave_device.acces_reg = SLAVE_MAX_ADDR;
    return ret;
}

spi_slave_event_t spi_slave_event(uint32_t data)
{
    if(data & 0x80000000)
        return SPI_EV_RECV;
    else
        return SPI_EV_TRANS;
}

void spi_slave_init(void)
{
    spi_slave = io_open("/dev/spi_slave");
    configASSERT(spi_slave);
    spi_slave_handler.on_event = spi_slave_event,
    spi_slave_handler.on_receive = spi_slave_receive,
    spi_slave_handler.on_transmit = spi_slave_transmit,

    spi_slave_config(spi_slave, 32, &spi_slave_handler);
}

void spi_master_init(void)
{
    spi0 = io_open("/dev/spi0");
    configASSERT(spi0);
    spi_stand = spi_get_device(spi0, SPI_MODE_0, SPI_FF_STANDARD, CHIP_SELECT, 32);
    spi_dev_set_clock_rate(spi_stand, 23000000);
}

void spi_write_reg(uint32_t reg, uint32_t data)
{
    uint32_t reg_value = reg | 0x80000000;
    uint32_t data_value = data | 0xc0000000;
    uint8_t* tmp_buf = malloc(4 + 4);
    memcpy(tmp_buf, &reg_value, 4);
    memcpy(tmp_buf + 4, &data_value, 4);
    taskENTER_CRITICAL();
    io_write(spi_stand, (uint8_t *)tmp_buf, 4 + 4);
    taskEXIT_CRITICAL();
    free(tmp_buf);
}

uint32_t spi_read_reg(uint32_t reg)
{
    uint32_t value = 0;

    uint32_t reg_value = reg & 0x7FFFFFFF;
    taskENTER_CRITICAL();
    io_write(spi_stand, (uint8_t *)&reg_value, 4);
    taskEXIT_CRITICAL();

    fpioa_set_function(36, FUNC_SPI0_D1);
    fpioa_set_function(38, FUNC_SPI0_D0);
    taskENTER_CRITICAL();
    io_read(spi_stand, (uint8_t *)&value, 4);
    taskEXIT_CRITICAL();
    fpioa_set_function(36, FUNC_SPI0_D0);
    fpioa_set_function(38, FUNC_SPI0_D1);
	return value;
}


