#include <stdio.h>
#include "spi_slave.h"
#include "project_cfg.h"

int main(void)
{
    uint8_t index;
    spi_master_init();
    spi_slave_init();

    printf("spi slave test\n");

    printf("spi write\n");
    for (index = 0; index < SLAVE_MAX_ADDR; index++)
        spi_write_reg(index, index+0x10000);
    printf("spi read\n");
    for (index = 0; index < SLAVE_MAX_ADDR; index++)
    {
        if (index+0x10000 != spi_read_reg(index))
        {
            printf("_TEST_FAIL_\n");
            return 0;
        }
    }
    printf("spi slave test ok\n");
    printf("_TEST_PASS_\n");
    while(1)
        ;
    return 0;
}
