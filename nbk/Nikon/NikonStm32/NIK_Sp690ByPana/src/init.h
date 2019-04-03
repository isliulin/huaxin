#ifndef _OL_INIT_
#define _OL_INIT

extern unsigned int Count_2ms;
extern unsigned int Count_IRQ4_Level;
extern unsigned char Buf_PA;
extern void enable_irq(void);
extern void disable_irq(void);
extern void set_irq(void);
extern void TimersStart(void);
extern void TM0_stop(void);
extern void com_start(void);
extern unsigned char trans_ctr;
extern unsigned int BUF;
extern void Port_init(void);
extern void TurnOnInit(void);
extern void AD_init(unsigned char swInt);
extern void TurnOnInit();


#endif



