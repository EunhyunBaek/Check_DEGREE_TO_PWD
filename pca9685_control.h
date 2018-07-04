#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <termio.h>
#include <signal.h>
#include <stdlib.h>
#include "pca9685_define.h"
int fd;
unsigned char buffer[3]={0};

int reg_read8(unsigned char addr)
{
	int length = 1;
	buffer[0] = addr;

	if(write(fd, buffer,length)!=length)
	{
		printf("Failed to write from the i2c bus\n");
	}
	
	if(read(fd,buffer, length) != length)
	{
		printf("Failed to read from the i2c bus\n");
	}
	//printf("addr[%d] = %d\n", addr, buffer[0]);
	
	return 0;
}

int reg_write8(unsigned char addr, unsigned char data)
{

	int length=2;
	
	buffer[0] = addr;
	buffer[1] = data;

	if(write(fd,buffer,length)!=length)
	{
		printf("Failed to write from the i2c bus\n");
		return -1;
	}
	
	return 0;
}

int reg_read16(unsigned char addr)
{
	unsigned short temp;
	reg_read8(addr);
	temp = 0xff & buffer[0];
	reg_read8(addr+1);
	temp |= (buffer[0] <<8);
	//printf("addr=0x%x, data=%d\n", addr, temp);
	 
	return 0;	
}

int reg_write16(unsigned char addr, unsigned short data)
{
	int length =2;
	reg_write8(addr, (data & 0xff));
	reg_write8(addr+1, (data>>8) & 0xff);
	return 0;
}
int pca9685_restart(void)
{
	int length;
	
	reg_write8(MODE1, 0x00);
	reg_write8(MODE2, 0x04);
	return 0;
}

int pca9685_freq()
{
	int length = 2, freq = 50;
	uint8_t pre_val = (CLOCK_FREQ / 4096 / freq) -1; 
	printf("prescale_val = %d\n", pre_val);
	 
	reg_write8(MODE1, 0x10);				// OP : OSC OFF
	reg_write8(PRE_SCALE, pre_val);	// OP : WRITE PRE_SCALE VALUE
	reg_write8(MODE1, 0x80);				// OP : RESTART
	reg_write8(MODE2, 0x04);				// OP : TOTEM POLE 
	return 0;
}

//FUNCTION pca9685_restart
int 
pca9685_resetart(void)
{
    reg_write8(MODE1,0x00);
    reg_write8(MODE2,0x04);
    
    return 0;
}

//FUNCTION INIT
int Initulize()
{
	wiringPiSetup();

	if((fd=open(I2C_DEV,O_RDWR))<0)
	{
		#ifdef DEBUG
		printf("Failed open ic2-1 bus\n");
		#endif
		return -1;
	}
	//PCADDR == i2c_slave addr
	//write a address 
	if(ioctl(fd,I2C_SLAVE,PCA_ADDR)<0)
	{
		printf("Failed to acquire bus access and/or talk to slave\n");
		return -1;
	}
    return 0;
}