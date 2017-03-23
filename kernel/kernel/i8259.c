#include "x86.h"
#include "common.h"

#define PORT_PIC_MASTER 0x20
#define PORT_PIC_SLAVE  0xA0
#define IRQ_SLAVE       2

#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

/* 初始化8259中断控制器：
 * 硬件中断IRQ从32号开始，自动发送EOI */
void
init_intr(void) {
	out_byte(PORT_PIC_MASTER + 1, 0xFF);
	out_byte(PORT_PIC_SLAVE + 1 , 0xFF);
	out_byte(PORT_PIC_MASTER, 0x11);
	out_byte(PORT_PIC_MASTER + 1, 32);
	out_byte(PORT_PIC_MASTER + 1, 1 << 2);
	out_byte(PORT_PIC_MASTER + 1, 0x3);
	out_byte(PORT_PIC_SLAVE, 0x11);
	out_byte(PORT_PIC_SLAVE + 1, 32 + 8);
	out_byte(PORT_PIC_SLAVE + 1, 2);
	out_byte(PORT_PIC_SLAVE + 1, 0x3);

	out_byte(PORT_PIC_MASTER, 0x68);
	out_byte(PORT_PIC_MASTER, 0x0A);
	out_byte(PORT_PIC_SLAVE, 0x68);
	out_byte(PORT_PIC_SLAVE, 0x0A);
}

void init_timer(void)
{
	int counter = FREQ_8253 / HZ;
	out_byte(TIMER_PORT+3,0x34);
	out_byte(TIMER_PORT+0,counter%256);
	out_byte(TIMER_PORT+0,counter/256);
}
