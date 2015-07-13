// gpio_i2c.h

#ifndef _EDRV_GPIO_I2C_H_
#define _EDRV_GPIO_I2C_H_


typedef enum
{
    GPIO_I2C_BPS_80K,
    GPIO_I2C_BPS_160K

} GPIO_I2C_BPS_T;

/*
if you want get a stop condition befort restart condition , do following  
1, call gpio_i2c_write_data(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen); 
the datalen == 0,get a stop condition
2, call gpio_i2c_read_data(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen); 
the addrlen== 0
 
 for example:
        gpio_i2c_write_data(gSensorInfo.i2caddress,&Addr,1, (unsigned char *)&data, 0);
    return(gpio_i2c_read_data(gSensorInfo.i2caddress,&Addr,0, data, 1));
*/





BOOL gpio_i2c_open(VOID);
BOOL gpio_i2c_close(VOID);

//BOOL gpio_i2c_init(VOID);

VOID gpio_i2c_Start(VOID);

VOID gpio_i2c_Stop(VOID);

UINT8 gpio_i2c_TxByte(UINT8 data);

VOID gpio_i2c_RxByte(UINT8 *data, UINT8 ack);

VOID gpio_i2c_write_data(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen);

UINT8 gpio_i2c_read_data(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen);


#endif



