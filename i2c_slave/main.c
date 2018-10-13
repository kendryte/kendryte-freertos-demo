#include <stdio.h>
#include "i2c_slave.h"
#include "project_cfg.h"

int main(void)
{
    int result = 0;
    uint8_t index, data_buf[SLAVE_MAX_ADDR];
    printf(" \n test i2c slave mode \n");

    i2c_slave_init();
    i2c_master_init();

    printf("write data\n");
    for (index = 0; index < SLAVE_MAX_ADDR; index++)
        data_buf[index] = index+30;
    i2c_write_reg(0, &data_buf[0], 7);
    i2c_write_reg(7, &data_buf[7], 7);

    printf("read data\n");
    for (index = 0; index < SLAVE_MAX_ADDR; index++)
        data_buf[index] = 0;
    i2c_read_reg(0, &data_buf[0], 7);
    i2c_read_reg(7, &data_buf[7], 7);
    for (index = 0; index < SLAVE_MAX_ADDR; index++)
    {
        if (data_buf[index] != index + 30)
        {
            printf("write: %x      read: %x\n", index, data_buf[index]);
            result++;
        }
    }
    if(result)
    {
        printf("i2c slave test error\n");
        printf("_TEST_FAIL_\n");
        return -1;
    }
    printf("i2c slave test ok\n");
    printf("_TEST_PASS_\n");
    while(1)
        ;
    return 0;
}
