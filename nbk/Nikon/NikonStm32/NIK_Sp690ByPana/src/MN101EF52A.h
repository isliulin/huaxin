#ifndef _LED_TYPE_DEF_H
#define _LED_TYPE_DEF_H

/*************************************************************
 *															 *
 *************************************************************/
#define UINT8 unsigned char 

typedef struct {
	UINT8	bit0:1;
	UINT8	bit1:1;
	UINT8	bit2:1;
	UINT8	bit3:1;
	UINT8	bit4:1;
	UINT8	bit5:1;
	UINT8	bit6:1;
	UINT8	bit7:1;
} UB_BITS;

typedef struct {
	UINT8	bit10:2;
	UINT8	bit2:1;
	UINT8	bit3:1;
	UINT8	bit4:1;
	UINT8	bit5:1;
	UINT8	bit6:1;
	UINT8	bit7:1;
} UB_BITS2;

/*
 *	CHDRVSEL --- 0x03DF0
 * 	External Oscillation Control Register
 */
#define		DRVSOSC_SEL	((UB_BITS*)(&CHDRVSEL))->bit1
#define		DRVOSC_SEL	((UB_BITS*)(&CHDRVSEL))->bit0

/*
 *	PWMMDL --- 0x03E00
 * 	PWM Mode Contorl Register Lower 8 bits
 */
#define		PCRAEN		((UB_BITS*)(&PWMMDL))->bit7
#define		PCRBEN		((UB_BITS*)(&PWMMDL))->bit6
#define		INTAEN		((UB_BITS*)(&PWMMDL))->bit5
#define		INTBEN		((UB_BITS*)(&PWMMDL))->bit4
#define		DTEN		((UB_BITS*)(&PWMMDL))->bit3
#define		ORMD		((UB_BITS*)(&PWMMDL))->bit2
#define		TCEN		((UB_BITS*)(&PWMMDL))->bit1
#define		WAVEMD		((UB_BITS*)(&PWMMDL))->bit0

/*
 *	PWMMDH ---0x03E01
 * 	PWM Mode Contorl Register Upper 8 bits
 */
#define		INTCEN		((UB_BITS*)(&PWMMDH))->bit6
#define		SDSELA		((UB_BITS*)(&PWMMDH))->bit1
#define		SDSELB		((UB_BITS*)(&PWMMDH))->bit0

/*
 *	PWMSELL ---0x03E02
 * 	PWM0 Output Control Register Lower 8 bits
 */
#define		PSELN00		((UB_BITS*)(&PWMSELL))->bit7
#define		PSEL00		((UB_BITS*)(&PWMSELL))->bit6
#define		OTLVN02		((UB_BITS*)(&PWMSELL))->bit5
#define		OTLV02		((UB_BITS*)(&PWMSELL))->bit4
#define		OTLVN01		((UB_BITS*)(&PWMSELL))->bit3
#define		OTLV01		((UB_BITS*)(&PWMSELL))->bit2
#define		OTLVN00		((UB_BITS*)(&PWMSELL))->bit1
#define		OTLV00		((UB_BITS*)(&PWMSELL))->bit0

/*
 *	PWMSELH ---0x03E03
 * 	PWM0 Output Control Register Upper 8 bits
 */
#define		PSELN02		((UB_BITS*)(&PWMSELH))->bit3
#define		PSEL02		((UB_BITS*)(&PWMSELH))->bit2
#define		PSELN01		((UB_BITS*)(&PWMSELH))->bit1
#define		PSEL01		((UB_BITS*)(&PWMSELH))->bit0

/*
 *	PWMSETL ---0x03E04
 * 	PWM Cycle Setting Register Lower 8 bits
 */
#define		PMSET07		((UB_BITS*)(&PWMSETL))->bit7
#define		PMSET06		((UB_BITS*)(&PWMSETL))->bit6
#define		PMSET05		((UB_BITS*)(&PWMSETL))->bit5
#define		PMSET04		((UB_BITS*)(&PWMSETL))->bit4
#define		PMSET03		((UB_BITS*)(&PWMSETL))->bit3
#define		PMSET02		((UB_BITS*)(&PWMSETL))->bit2
#define		PMSET01		((UB_BITS*)(&PWMSETL))->bit1
#define		PMSET00		((UB_BITS*)(&PWMSETL))->bit0

/*
 *	PWMSETH ---0x03E05
 * 	PWM Cycle Setting Register Upper 8 bits
 */
#define		PMSET0F		((UB_BITS*)(&PWMSETH))->bit7
#define		PMSET0E		((UB_BITS*)(&PWMSETH))->bit6
#define		PMSET0D		((UB_BITS*)(&PWMSETH))->bit5
#define		PMSET0C		((UB_BITS*)(&PWMSETH))->bit4
#define		PMSET0B		((UB_BITS*)(&PWMSETH))->bit3
#define		PMSET0A		((UB_BITS*)(&PWMSETH))->bit2
#define		PMSET09		((UB_BITS*)(&PWMSETH))->bit1
#define		PMSET08		((UB_BITS*)(&PWMSETH))->bit0
/*
 *	TCMPAL ---0x03E06
 * 	PWM U-Phase Comparison Setting Register Lower 8 bits
 */
#define		TCPA07		((UB_BITS*)(&TCMPAL))->bit7
#define		TCPA06		((UB_BITS*)(&TCMPAL))->bit6
#define		TCPA05		((UB_BITS*)(&TCMPAL))->bit5
#define		TCPA04		((UB_BITS*)(&TCMPAL))->bit4
#define		TCPA03		((UB_BITS*)(&TCMPAL))->bit3
#define		TCPA02		((UB_BITS*)(&TCMPAL))->bit2
#define		TCPA01		((UB_BITS*)(&TCMPAL))->bit1
#define		TCPA00		((UB_BITS*)(&TCMPAL))->bit0

/*
 *	TCMPAH ---0x03E07
 * 	PWM U-Phase Comparison Setting Register Upper 8 bits
 */
#define		TCPA0F		((UB_BITS*)(&TCMPAH))->bit7
#define		TCPA0E		((UB_BITS*)(&TCMPAH))->bit6
#define		TCPA0D		((UB_BITS*)(&TCMPAH))->bit5
#define		TCPA0C		((UB_BITS*)(&TCMPAH))->bit4
#define		TCPA0B		((UB_BITS*)(&TCMPAH))->bit3
#define		TCPA0A		((UB_BITS*)(&TCMPAH))->bit2
#define		TCPA09		((UB_BITS*)(&TCMPAH))->bit1
#define		TCPA08		((UB_BITS*)(&TCMPAH))->bit0


/*
 *	TCMPBL ---0x03E08
 * 	PWM V-Phase Comparison Setting Register Lower 8 bits
 */
#define		TCPB07		((UB_BITS*)(&TCMPBL))->bit7
#define		TCPB06		((UB_BITS*)(&TCMPBL))->bit6
#define		TCPB05		((UB_BITS*)(&TCMPBL))->bit5
#define		TCPB04		((UB_BITS*)(&TCMPBL))->bit4
#define		TCPB03		((UB_BITS*)(&TCMPBL))->bit3
#define		TCPB02		((UB_BITS*)(&TCMPBL))->bit2
#define		TCPB01		((UB_BITS*)(&TCMPBL))->bit1
#define		TCPB00		((UB_BITS*)(&TCMPBL))->bit0


/*
 *	TCMPBH ---0x03E09
 * 	PWM V-Phase Comparison Setting Register Upper 8 bits
 */
#define		TCPB0F		((UB_BITS*)(&TCMPBH))->bit7
#define		TCPB0E		((UB_BITS*)(&TCMPBH))->bit6
#define		TCPB0D		((UB_BITS*)(&TCMPBH))->bit5
#define		TCPB0C		((UB_BITS*)(&TCMPBH))->bit4
#define		TCPB0B		((UB_BITS*)(&TCMPBH))->bit3
#define		TCPB0A		((UB_BITS*)(&TCMPBH))->bit2
#define		TCPB09		((UB_BITS*)(&TCMPBH))->bit1
#define		TCPB08		((UB_BITS*)(&TCMPBH))->bit0

/*
 *	TCMPCL ---0x03E0A
 * 	PWM W-Phase Comparison Setting Register Lower 8 bits
 */
#define		TCPC07		((UB_BITS*)(&TCMPCL))->bit7
#define		TCPC06		((UB_BITS*)(&TCMPCL))->bit6
#define		TCPC05		((UB_BITS*)(&TCMPCL))->bit5
#define		TCPC04		((UB_BITS*)(&TCMPCL))->bit4
#define		TCPC03		((UB_BITS*)(&TCMPCL))->bit3
#define		TCPC02		((UB_BITS*)(&TCMPCL))->bit2
#define		TCPC01		((UB_BITS*)(&TCMPCL))->bit1
#define		TCPC00		((UB_BITS*)(&TCMPCL))->bit0

/*
 *	TCMPCH ---0x03E0B
 * 	PWM W-Phase Comparison Setting Register Upper 8 bits
 */
#define		TCPC0F		((UB_BITS*)(&TCMPCH))->bit7
#define		TCPC0E		((UB_BITS*)(&TCMPCH))->bit6
#define		TCPC0D		((UB_BITS*)(&TCMPCH))->bit5
#define		TCPC0C		((UB_BITS*)(&TCMPCH))->bit4
#define		TCPC0B		((UB_BITS*)(&TCMPCH))->bit3
#define		TCPC0A		((UB_BITS*)(&TCMPCH))->bit2
#define		TCPC09		((UB_BITS*)(&TCMPCH))->bit1
#define		TCPC08		((UB_BITS*)(&TCMPCH))->bit0

/*
 *	OUTMD ---0x03E0C
 * 	PWM Output Polarity Control Register
 */
#define		PXDTNW		((UB_BITS*)(&OUTMD))->bit5
#define		PXDTW		((UB_BITS*)(&OUTMD))->bit4
#define		PXDTNV		((UB_BITS*)(&OUTMD))->bit3
#define		PXDTV		((UB_BITS*)(&OUTMD))->bit2
#define		PXDTNU		((UB_BITS*)(&OUTMD))->bit1
#define		PXDTU		((UB_BITS*)(&OUTMD))->bit0

/*
 *	DTMSET ---0x03E0D
 * 	PWM Dead Time Setting Register
 */
#define		DTST07		((UB_BITS*)(&DTMSET))->bit7
#define		DTST06		((UB_BITS*)(&DTMSET))->bit6
#define		DTST05		((UB_BITS*)(&DTMSET))->bit5
#define		DTST04		((UB_BITS*)(&DTMSET))->bit4
#define		DTST03		((UB_BITS*)(&DTMSET))->bit3
#define		DTST02		((UB_BITS*)(&DTMSET))->bit2
#define		DTST01		((UB_BITS*)(&DTMSET))->bit1
#define		DTST00		((UB_BITS*)(&DTMSET))->bit0


/*
 *	DTMSET1 ---0x03E0E
 * 	PWM Dead Time Setting Register 1
 */
#define		STIM17		((UB_BITS*)(&DTMSET1))->bit7
#define		STIM16		((UB_BITS*)(&DTMSET1))->bit6
#define		STIM15		((UB_BITS*)(&DTMSET1))->bit5
#define		STIM14		((UB_BITS*)(&DTMSET1))->bit4
#define		STIM13		((UB_BITS*)(&DTMSET1))->bit3
#define		STIM12		((UB_BITS*)(&DTMSET1))->bit2
#define		STIM11		((UB_BITS*)(&DTMSET1))->bit1
#define		STIM10		((UB_BITS*)(&DTMSET1))->bit0

/*
 *	PWMBCL ---0x03E10
 * 	PWM BC Value Read Register Lower 8 bits
 */
#define		PWMBC07		((UB_BITS*)(&PWMBCL))->bit7
#define		PWMBC06		((UB_BITS*)(&PWMBCL))->bit6
#define		PWMBC05		((UB_BITS*)(&PWMBCL))->bit5
#define		PWMBC04		((UB_BITS*)(&PWMBCL))->bit4
#define		PWMBC03		((UB_BITS*)(&PWMBCL))->bit3
#define		PWMBC02		((UB_BITS*)(&PWMBCL))->bit2
#define		PWMBC01		((UB_BITS*)(&PWMBCL))->bit1
#define		PWMBC00		((UB_BITS*)(&PWMBCL))->bit0

/*
 *	PWMBCH ---0x03E11
 * 	PWM BC Value Read Register Upper 8 bits
 */
#define		PWMBC15		((UB_BITS*)(&PWMBCH))->bit7
#define		PWMBC14		((UB_BITS*)(&PWMBCH))->bit6
#define		PWMBC13		((UB_BITS*)(&PWMBCH))->bit5
#define		PWMBC12		((UB_BITS*)(&PWMBCH))->bit4
#define		PWMBC11		((UB_BITS*)(&PWMBCH))->bit3
#define		PWMBC10		((UB_BITS*)(&PWMBCH))->bit2
#define		PWMBC09		((UB_BITS*)(&PWMBCH))->bit1
#define		PWMBC08		((UB_BITS*)(&PWMBCH))->bit0

/*
 *	BCSTR ---0x03E12
 * 	PWM BC Status Read Register
 */
#define		PWMSTR		((UB_BITS*)(&BCSTR))->bit0

/*
 *	PWMOFFL ---0x03E13
 * 	PWM Pin Protection Control Register Lower 8 bits
 */
#define		PRTANU1		((UB_BITS*)(&PWMOFFL))->bit7
#define		PRTANU0		((UB_BITS*)(&PWMOFFL))->bit6
#define		PRTAU1		((UB_BITS*)(&PWMOFFL))->bit5
#define		PRTAU0		((UB_BITS*)(&PWMOFFL))->bit4
#define		IRQSEL2		((UB_BITS*)(&PWMOFFL))->bit3
#define		IRQSEL1		((UB_BITS*)(&PWMOFFL))->bit2
#define		IRQSEL0		((UB_BITS*)(&PWMOFFL))->bit1
#define		OUTEN0		((UB_BITS*)(&PWMOFFL))->bit0


/*
 *	PWMOFFH ---0x03E14
 * 	PWM Pin Protection Control Register Upper 8 bits
 */

#define		PRTANW1		((UB_BITS*)(&PWMOFFH))->bit7
#define		PRTANW0		((UB_BITS*)(&PWMOFFH))->bit6
#define		PRTAW1		((UB_BITS*)(&PWMOFFH))->bit5
#define		PRTAW0		((UB_BITS*)(&PWMOFFH))->bit4
#define		PRTANV1		((UB_BITS*)(&PWMOFFH))->bit3
#define		PRTANV0		((UB_BITS*)(&PWMOFFH))->bit2
#define		PRTAV1		((UB_BITS*)(&PWMOFFH))->bit1
#define		PRTAV0		((UB_BITS*)(&PWMOFFH))->bit0


/*
 *	IRQCULL ---0x03E15
 * 	Interrupt Output Control Register
 */
#define		REGSELEDGE1	((UB_BITS*)(&IRQCULL))->bit7
#define		REGSELEDGE0	((UB_BITS*)(&IRQCULL))->bit6
#define		REGCULLCMP1	((UB_BITS*)(&IRQCULL))->bit5
#define		REGCULLCMP0	((UB_BITS*)(&IRQCULL))->bit4
#define		REGCULLUDF1	((UB_BITS*)(&IRQCULL))->bit3
#define		REGCULLUDF0	((UB_BITS*)(&IRQCULL))->bit2
#define		REGCULLOVF1	((UB_BITS*)(&IRQCULL))->bit1
#define		REGCULLOVF0	((UB_BITS*)(&IRQCULL))->bit0

/*
 *	PWMTMCNT ---0x03E16
 * 	PWM Timer Operation Control Register
 */
#define		PSCSEL1		((UB_BITS*)(&PWMTMCNT))->bit4
#define		PSCSEL0		((UB_BITS*)(&PWMTMCNT))->bit3
#define		PWMCKSEL1	((UB_BITS*)(&PWMTMCNT))->bit2
#define		PWMCKSEL0	((UB_BITS*)(&PWMTMCNT))->bit1
#define		SEL_PWM_TIM	((UB_BITS*)(&PWMTMCNT))->bit0


/*
 * RELCTR   ---0x03E17
 * 4 Phases PWM Mode Register
 */
#define		RELIRQA		((UB_BITS*)(&RELCTR))->bit7
#define		RELIRQB		((UB_BITS*)(&RELCTR))->bit6
#define		RELIRQC		((UB_BITS*)(&RELCTR))->bit5
#define		RELIRQD		((UB_BITS*)(&RELCTR))->bit4
#define		MD1CTR		((UB_BITS*)(&RELCTR))->bit2
#define		RELMD		((UB_BITS2*)(&RELCTR))->bit10

/*
 * PWMODR   ---0x03E18
 * 4 Phases PWM Output Order Control Register
 */
#define		ORDER		((UB_BITS*)(&PWMODR))->bit0

/*
 * RELSTAT  ---0x03E19
 * 4 Phases PWM Output Status Register
 */
#define		STAT1		((UB_BITS*)(&RELSTAT))->bit1
#define		STAT0		((UB_BITS*)(&RELSTAT))->bit0

/*
 * PWMCMP1  ---0x03E1A
 * PWM Cycle/Compare Collective Setting Register 1
 */
#define		PWMCMPL7		((UB_BITS*)(&PWMCMP1))->bit7
#define		PWMCMPL6		((UB_BITS*)(&PWMCMP1))->bit6
#define		PWMCMPL5		((UB_BITS*)(&PWMCMP1))->bit5
#define		PWMCMPL4		((UB_BITS*)(&PWMCMP1))->bit4
#define		PWMCMPL3		((UB_BITS*)(&PWMCMP1))->bit3
#define		PWMCMPL2		((UB_BITS*)(&PWMCMP1))->bit2
#define		PWMCMPL1		((UB_BITS*)(&PWMCMP1))->bit1
#define		PWMCMPL0		((UB_BITS*)(&PWMCMP1))->bit0

/*
 * PWMCMP2  ---0x03E1B
 * PWM Cycle/Compare Collective Setting Register 2
 */
#define		PWMCMPH7		((UB_BITS*)(&PWMCMP2))->bit7
#define		PWMCMPH6		((UB_BITS*)(&PWMCMP2))->bit6
#define		PWMCMPH5		((UB_BITS*)(&PWMCMP2))->bit5
#define		PWMCMPH4		((UB_BITS*)(&PWMCMP2))->bit4
#define		PWMCMPH3		((UB_BITS*)(&PWMCMP2))->bit3
#define		PWMCMPH2		((UB_BITS*)(&PWMCMP2))->bit2
#define		PWMCMPH1		((UB_BITS*)(&PWMCMP2))->bit1
#define		PWMCMPH0		((UB_BITS*)(&PWMCMP2))->bit0

/*
 * PRTKEY  ---0x03E50
 * Register protect control register
 */
#define		PRTK7			((UB_BITS*)(&PRTKEY))->bit7
#define		PRTK6			((UB_BITS*)(&PRTKEY))->bit6
#define		PRTK5			((UB_BITS*)(&PRTKEY))->bit5
#define		PRTK4			((UB_BITS*)(&PRTKEY))->bit4
#define		PRTK3			((UB_BITS*)(&PRTKEY))->bit3
#define		PRTK2			((UB_BITS*)(&PRTKEY))->bit2
#define		PRTK1			((UB_BITS*)(&PRTKEY))->bit1
#define		PRTK0			((UB_BITS*)(&PRTKEY))->bit0

/*
 * CKTRMD  ---0x03E51
 * Clock transition mode register
 */
#define		TRMD			((UB_BITS*)(&CKTRMD))->bit0

/*
 * CKMONEN  ---0x03E52
 * Clock monitoring enable register
 */
#define		MVCLK			((UB_BITS*)(&CKMONEN))->bit2
#define		DETIRQ			((UB_BITS*)(&CKMONEN))->bit1
#define		MONIEN			((UB_BITS*)(&CKMONEN))->bit0

/*
 * CKMONSETU  ---0x03E53
 * Clock monitoring frequency setting register (upper limit)
 */
#define		MNUSET7			((UB_BITS*)(&CKMONSETU))->bit7
#define		MNUSET6			((UB_BITS*)(&CKMONSETU))->bit6
#define		MNUSET5			((UB_BITS*)(&CKMONSETU))->bit5
#define		MNUSET4			((UB_BITS*)(&CKMONSETU))->bit4
#define		MNUSET3			((UB_BITS*)(&CKMONSETU))->bit3
#define		MNUSET2			((UB_BITS*)(&CKMONSETU))->bit2
#define		MNUSET1			((UB_BITS*)(&CKMONSETU))->bit1
#define		MNUSET0			((UB_BITS*)(&CKMONSETU))->bit0

/*
 * CKMONSETL  ---0x03E54
 * Clock monitoring frequency setting register (lower limit)
 */
#define		MNLSET7			((UB_BITS*)(&CKMONSETL))->bit7
#define		MNLSET6			((UB_BITS*)(&CKMONSETL))->bit6
#define		MNLSET5			((UB_BITS*)(&CKMONSETL))->bit5
#define		MNLSET4			((UB_BITS*)(&CKMONSETL))->bit4
#define		MNLSET3			((UB_BITS*)(&CKMONSETL))->bit3
#define		MNLSET2			((UB_BITS*)(&CKMONSETL))->bit2
#define		MNLSET1			((UB_BITS*)(&CKMONSETL))->bit1
#define		MNLSET0			((UB_BITS*)(&CKMONSETL))->bit0

/*
 * CKMONSTAT  ---0x03E55
 * Clock monitoring status register
 */
#define		PLLERR			((UB_BITS*)(&CKMONSTAT))->bit2
#define		XIERR			((UB_BITS*)(&CKMONSTAT))->bit1
#define		OSCERR			((UB_BITS*)(&CKMONSTAT))->bit0

/*
 * OSCLOCK  ---0x03E56
 * Oscillation Control Register Protect Register
 */
#define		LOCKEN			((UB_BITS*)(&OSCLOCK))->bit0

/*
 *	P0OUT --- 0x03F70        ----->    0x03E70
 *	Port 0 output data
 */
 
#define		P0OUT7		((UB_BITS*)(&P0OUT))->bit7
#define		P0OUT6		((UB_BITS*)(&P0OUT))->bit6
#define		P0OUT5		((UB_BITS*)(&P0OUT))->bit5
#define		P0OUT4		((UB_BITS*)(&P0OUT))->bit4
#define		P0OUT3		((UB_BITS*)(&P0OUT))->bit3
#define		P0OUT2		((UB_BITS*)(&P0OUT))->bit2
#define		P0OUT1		((UB_BITS*)(&P0OUT))->bit1
#define		P0OUT0		((UB_BITS*)(&P0OUT))->bit0


/*
 *	P2OUT --- 0x03F72        ----->     0x3E72
 *	Port 2 output data
 */

#define		P2OUT7		((UB_BITS*)(&P2OUT))->bit7
#define		P2OUT6		((UB_BITS*)(&P2OUT))->bit6
#define		P2OUT5		((UB_BITS*)(&P2OUT))->bit5
#define		P2OUT4		((UB_BITS*)(&P2OUT))->bit4
#define		P2OUT3		((UB_BITS*)(&P2OUT))->bit3
#define		P2OUT2		((UB_BITS*)(&P2OUT))->bit2
#define		P2OUT1		((UB_BITS*)(&P2OUT))->bit1
#define		P2OUT0		((UB_BITS*)(&P2OUT))->bit0


/*
 *	P3OUT --- 0x03E73       ----->  0x03E73
 *	Port 3 output data
 */
 
#define		P3OUT6		((UB_BITS*)(&P3OUT))->bit6
#define		P3OUT5		((UB_BITS*)(&P3OUT))->bit5
#define		P3OUT4		((UB_BITS*)(&P3OUT))->bit4
#define		P3OUT3		((UB_BITS*)(&P3OUT))->bit3
#define		P3OUT2		((UB_BITS*)(&P3OUT))->bit2
#define		P3OUT1		((UB_BITS*)(&P3OUT))->bit1
#define		P3OUT0		((UB_BITS*)(&P3OUT))->bit0


/*
 *	P4OUT --- 0x03E74
 * 	Port 4 output data
 */
#define		P4OUT5		((UB_BITS*)(&P4OUT))->bit5
#define		P4OUT4		((UB_BITS*)(&P4OUT))->bit4
#define		P4OUT3		((UB_BITS*)(&P4OUT))->bit3
#define		P4OUT2		((UB_BITS*)(&P4OUT))->bit2
#define		P4OUT1		((UB_BITS*)(&P4OUT))->bit1
#define		P4OUT0		((UB_BITS*)(&P4OUT))->bit0



/*
 *	PAOUT --- 0x03E7A
 * Port A output data
 */
#define		PAOUT7		((UB_BITS*)(&PAOUT))->bit7
#define		PAOUT6		((UB_BITS*)(&PAOUT))->bit6
#define		PAOUT5		((UB_BITS*)(&PAOUT))->bit5
#define		PAOUT4		((UB_BITS*)(&PAOUT))->bit4
#define		PAOUT3		((UB_BITS*)(&PAOUT))->bit3
#define		PAOUT2		((UB_BITS*)(&PAOUT))->bit2
#define		PAOUT1		((UB_BITS*)(&PAOUT))->bit1
#define		PAOUT0		((UB_BITS*)(&PAOUT))->bit0

/*
 *	P0IN --- 0x03E80
 *	Port 0 input data
 */
#define		P0IN7		((UB_BITS*)(&P0IN))->bit7
#define		P0IN6		((UB_BITS*)(&P0IN))->bit6
#define		P0IN5		((UB_BITS*)(&P0IN))->bit5
#define		P0IN4		((UB_BITS*)(&P0IN))->bit4
#define		P0IN3		((UB_BITS*)(&P0IN))->bit3
#define		P0IN2		((UB_BITS*)(&P0IN))->bit2
#define		P0IN1		((UB_BITS*)(&P0IN))->bit1
#define		P0IN0		((UB_BITS*)(&P0IN))->bit0

/*
 *	P2IN --- 0x03E82
 *	Port 2 input data
 */
 
#define		P2IN6		((UB_BITS*)(&P2IN))->bit6
#define		P2IN5		((UB_BITS*)(&P2IN))->bit5
#define		P2IN4		((UB_BITS*)(&P2IN))->bit4
#define		P2IN3		((UB_BITS*)(&P2IN))->bit3
#define		P2IN2		((UB_BITS*)(&P2IN))->bit2
#define		P2IN1		((UB_BITS*)(&P2IN))->bit1
#define		P2IN0		((UB_BITS*)(&P2IN))->bit0


/*
 *	P3IN --- 0x03E83
 *	Port 3 input data
 */
 
#define		P3IN6		((UB_BITS*)(&P3IN))->bit6
#define		P3IN5		((UB_BITS*)(&P3IN))->bit5
#define		P3IN4		((UB_BITS*)(&P3IN))->bit4
#define		P3IN3		((UB_BITS*)(&P3IN))->bit3
#define		P3IN2		((UB_BITS*)(&P3IN))->bit2
#define		P3IN1		((UB_BITS*)(&P3IN))->bit1
#define		P3IN0		((UB_BITS*)(&P3IN))->bit0


/*
 *	P4IN --- 0x03E84
 *	Port 4 input data
 */
#define		P4IN5		((UB_BITS*)(&P4IN))->bit5
#define		P4IN4		((UB_BITS*)(&P4IN))->bit4
#define		P4IN3		((UB_BITS*)(&P4IN))->bit3
#define		P4IN2		((UB_BITS*)(&P4IN))->bit2
#define		P4IN1		((UB_BITS*)(&P4IN))->bit1
#define		P4IN0		((UB_BITS*)(&P4IN))->bit0


/*
 *	PAIN --- 0x03E8A
 *	Port A input data
 */
#define		PAIN7		((UB_BITS*)(&PAIN))->bit7
#define		PAIN6		((UB_BITS*)(&PAIN))->bit6
#define		PAIN5		((UB_BITS*)(&PAIN))->bit5
#define		PAIN4		((UB_BITS*)(&PAIN))->bit4
#define		PAIN3		((UB_BITS*)(&PAIN))->bit3
#define		PAIN2		((UB_BITS*)(&PAIN))->bit2
#define		PAIN1		((UB_BITS*)(&PAIN))->bit1
#define		PAIN0		((UB_BITS*)(&PAIN))->bit0

/*
 *	SCHMIT1 --- 0x03E8F
 *	Port A Schmitt level control
 */
#define		SCHMIT22		((UB_BITS*)(&SCHMIT1))->bit2

/*
 *	P0DIR --- 0x03F90
 *	Port 0 I/O direction control
 */
#define		P0DIR7		((UB_BITS*)(&P0DIR))->bit7
#define		P0DIR6		((UB_BITS*)(&P0DIR))->bit6
#define		P0DIR5		((UB_BITS*)(&P0DIR))->bit5
#define		P0DIR4		((UB_BITS*)(&P0DIR))->bit4
#define		P0DIR3		((UB_BITS*)(&P0DIR))->bit3
#define		P0DIR2		((UB_BITS*)(&P0DIR))->bit2
#define		P0DIR1		((UB_BITS*)(&P0DIR))->bit1
#define		P0DIR0		((UB_BITS*)(&P0DIR))->bit0

/*
 *	P2DIR --- 0x03E92
 *	Port 2 I/O direction control
 */
 
#define		P2DIR6		((UB_BITS*)(&P2DIR))->bit6
#define		P2DIR5		((UB_BITS*)(&P2DIR))->bit5
#define		P2DIR4		((UB_BITS*)(&P2DIR))->bit4
#define		P2DIR3		((UB_BITS*)(&P2DIR))->bit3
#define		P2DIR2		((UB_BITS*)(&P2DIR))->bit2
#define		P2DIR1		((UB_BITS*)(&P2DIR))->bit1
#define		P2DIR0		((UB_BITS*)(&P2DIR))->bit0


/*
 *	P3DIR --- 0x03E93
 *	Port 3 I/O direction control
 */
 
#define		P3DIR6		((UB_BITS*)(&P3DIR))->bit6
#define		P3DIR5		((UB_BITS*)(&P3DIR))->bit5
#define		P3DIR4		((UB_BITS*)(&P3DIR))->bit4
#define		P3DIR3		((UB_BITS*)(&P3DIR))->bit3
#define		P3DIR2		((UB_BITS*)(&P3DIR))->bit2
#define		P3DIR1		((UB_BITS*)(&P3DIR))->bit1
#define		P3DIR0		((UB_BITS*)(&P3DIR))->bit0


/*
 *	P4DIR --- 0x03E94
 *	Port 4 I/O direction control
 */
#define		P4DIR5		((UB_BITS*)(&P4DIR))->bit5
#define		P4DIR4		((UB_BITS*)(&P4DIR))->bit4
#define		P4DIR3		((UB_BITS*)(&P4DIR))->bit3
#define		P4DIR2		((UB_BITS*)(&P4DIR))->bit2
#define		P4DIR1		((UB_BITS*)(&P4DIR))->bit1
#define		P4DIR0		((UB_BITS*)(&P4DIR))->bit0


/*
 *	PADIR --- 0x03E9A
 *	Port A I/O direction control
 */
#define		PADIR7		((UB_BITS*)(&PADIR))->bit7
#define		PADIR6		((UB_BITS*)(&PADIR))->bit6
#define		PADIR5		((UB_BITS*)(&PADIR))->bit5
#define		PADIR4		((UB_BITS*)(&PADIR))->bit4
#define		PADIR3		((UB_BITS*)(&PADIR))->bit3
#define		PADIR2		((UB_BITS*)(&PADIR))->bit2
#define		PADIR1		((UB_BITS*)(&PADIR))->bit1
#define		PADIR0		((UB_BITS*)(&PADIR))->bit0

/*
 *	P0PLU --- 0x03EA0
 *	Port 0 pull-up resistor additional selection
 */
#define		P0PLU7		((UB_BITS*)(&P0PLU))->bit7
#define		P0PLU6		((UB_BITS*)(&P0PLU))->bit6
#define		P0PLU5		((UB_BITS*)(&P0PLU))->bit5
#define		P0PLU4		((UB_BITS*)(&P0PLU))->bit4
#define		P0PLU3		((UB_BITS*)(&P0PLU))->bit3
#define		P0PLU2		((UB_BITS*)(&P0PLU))->bit2
#define		P0PLU1		((UB_BITS*)(&P0PLU))->bit1
#define		P0PLU0		((UB_BITS*)(&P0PLU))->bit0

/*
 *	P2PLU --- 0x03EA2
 *	Port 2 pull-up resistor additional selection
 */
 
#define		P2PLU6		((UB_BITS*)(&P2PLU))->bit6
#define		P2PLU5		((UB_BITS*)(&P2PLU))->bit5
#define		P2PLU4		((UB_BITS*)(&P2PLU))->bit4
#define		P2PLU3		((UB_BITS*)(&P2PLU))->bit3
#define		P2PLU2		((UB_BITS*)(&P2PLU))->bit2
#define		P2PLU1		((UB_BITS*)(&P2PLU))->bit1
#define		P2PLU0		((UB_BITS*)(&P2PLU))->bit0


/*
 *	P3PLU --- 0x03EA3
 *	Port 3 pull-up resistor additional selection
 */
 
#define		P3PLU6		((UB_BITS*)(&P3PLU))->bit6
#define		P3PLU5		((UB_BITS*)(&P3PLU))->bit5
#define		P3PLU4		((UB_BITS*)(&P3PLU))->bit4
#define		P3PLU3		((UB_BITS*)(&P3PLU))->bit3
#define		P3PLU2		((UB_BITS*)(&P3PLU))->bit2
#define		P3PLU1		((UB_BITS*)(&P3PLU))->bit1
#define		P3PLU0		((UB_BITS*)(&P3PLU))->bit0

/*
 *	P4PLUD --- 0x03EA4
 *	Port 4 pull-up resistor additional selection
 */
#define		P4PLUD5		((UB_BITS*)(&P4PLUD))->bit5
#define		P4PLUD4		((UB_BITS*)(&P4PLUD))->bit4
#define		P4PLUD3		((UB_BITS*)(&P4PLUD))->bit3
#define		P4PLUD2		((UB_BITS*)(&P4PLUD))->bit2
#define		P4PLUD1		((UB_BITS*)(&P4PLUD))->bit1
#define		P4PLUD0		((UB_BITS*)(&P4PLUD))->bit0


/*
 *	PAPLU --- 0x03EAA
 *	Port A pull-up resistor additional selection
 */
#define		PAPLU7		((UB_BITS*)(&PAPLU))->bit7
#define		PAPLU6		((UB_BITS*)(&PAPLU))->bit6
#define		PAPLU5		((UB_BITS*)(&PAPLU))->bit5
#define		PAPLU4		((UB_BITS*)(&PAPLU))->bit4
#define		PAPLU3		((UB_BITS*)(&PAPLU))->bit3
#define		PAPLU2		((UB_BITS*)(&PAPLU))->bit2
#define		PAPLU1		((UB_BITS*)(&PAPLU))->bit1
#define		PAPLU0		((UB_BITS*)(&PAPLU))->bit0

/*
 *	SELUD --- 0x03EAF
 *	Port 4 pull-up/pull-down selection
 */
 
#define		SELUD4		((UB_BITS*)(&SELUD))->bit4

/*
 *	P0OMD1 --- 0x03EB0
 * 	Port 0 output mode register 1
 */
#define		P0OMD15		((UB_BITS*)(&P0OMD1))->bit5
#define		P0OMD14		((UB_BITS*)(&P0OMD1))->bit4
#define		P0OMD13		((UB_BITS*)(&P0OMD1))->bit3
#define		P0OMD12		((UB_BITS*)(&P0OMD1))->bit2
#define		P0OMD11		((UB_BITS*)(&P0OMD1))->bit1
#define		P0OMD10		((UB_BITS*)(&P0OMD1))->bit0

/*
 *	P3OMD --- 0x03EB3
 * 	Port 3 output mode register 
 */
 
#define		P3OMD5		((UB_BITS*)(&P3OMD))->bit5
#define		P3OMD3		((UB_BITS*)(&P3OMD))->bit3
#define		P3OMD2		((UB_BITS*)(&P3OMD))->bit2


/*
 *	P4OMD --- 0x03EB4
 * 	Port 4 output mode register 
 */
 
#define		P4OMD4		((UB_BITS*)(&P4OMD))->bit4
#define		P4OMD3		((UB_BITS*)(&P4OMD))->bit3
#define		P4OMD2		((UB_BITS*)(&P4OMD))->bit2
#define		P4OMD1		((UB_BITS*)(&P4OMD))->bit1
#define		P4OMD0		((UB_BITS*)(&P4OMD))->bit0


/*
 *	PAOMD --- 0x03EBA
 * 	Port A output mode register
 */
//#define		PAOMD7		((UB_BITS*)(&PAOMD))->bit7
#define		PAOMD6		((UB_BITS*)(&PAOMD))->bit6
#define		PAOMD5		((UB_BITS*)(&PAOMD))->bit5
#define		PAOMD4		((UB_BITS*)(&PAOMD))->bit4

/*
 *	P0OMD2 --- 0x03EC0
 * 	Port 0 output mode register 2
 */
#define		P0OMD24		((UB_BITS*)(&P0OMD2))->bit4
#define		P0OMD23		((UB_BITS*)(&P0OMD2))->bit3
#define		P0OMD22		((UB_BITS*)(&P0OMD2))->bit2
#define		P0OMD21		((UB_BITS*)(&P0OMD2))->bit1
#define		P0OMD20		((UB_BITS*)(&P0OMD2))->bit0

/*
 *	P4IMD --- 0x03EC4
 *  Port 4 input mode register
 */
 
#define		P4IMD5		((UB_BITS*)(&P4IMD))->bit5
#define		P4IMD4		((UB_BITS*)(&P4IMD))->bit4
#define		P4IMD3		((UB_BITS*)(&P4IMD))->bit3
#define		P4IMD2		((UB_BITS*)(&P4IMD))->bit2


/*
 *	PAIMD --- 0x03ECA
 *  Port A input mode register
 */
#define		PAIMD7		((UB_BITS*)(&PAIMD))->bit7
#define		PAIMD6		((UB_BITS*)(&PAIMD))->bit6
#define		PAIMD5		((UB_BITS*)(&PAIMD))->bit5
#define		PAIMD4		((UB_BITS*)(&PAIMD))->bit4
#define		PAIMD3		((UB_BITS*)(&PAIMD))->bit3
#define		PAIMD2		((UB_BITS*)(&PAIMD))->bit2
#define		PAIMD1		((UB_BITS*)(&PAIMD))->bit1
#define		PAIMD0		((UB_BITS*)(&PAIMD))->bit0

/*
 *	P0LED --- 0x03EE0
 * 	Port 0 LED control register
 */
#define		P0LED7		((UB_BITS*)(&P0LED))->bit7
#define		P0LED6		((UB_BITS*)(&P0LED))->bit6
#define		P0LED5		((UB_BITS*)(&P0LED))->bit5
#define		P0LED4		((UB_BITS*)(&P0LED))->bit4
#define		P0LED3		((UB_BITS*)(&P0LED))->bit3
#define		P0LED2		((UB_BITS*)(&P0LED))->bit2
#define		P0LED1		((UB_BITS*)(&P0LED))->bit1
#define		P0LED0		((UB_BITS*)(&P0LED))->bit0

/*
 *	P0ODC --- 0x03EF0
 * 	Port 0 Nch open-drain control register
 */
#define		P0ODC7		((UB_BITS*)(&P0ODC))->bit7
#define		P0ODC6		((UB_BITS*)(&P0ODC))->bit6


/*
 *	P2ODC --- 0x03EF2
 * Port 2 Nch open-drain control register
 */
#define		P2ODC3		((UB_BITS*)(&P2ODC))->bit3
#define		P2ODC1		((UB_BITS*)(&P2ODC))->bit1

/*
 *	P3ODC --- 0x03EF3
 * 	Port 3 Nch open-drain control register
 */
#define		P3ODC6		((UB_BITS*)(&P3ODC))->bit6
#define		P3ODC4		((UB_BITS*)(&P3ODC))->bit4

/*
 *	PAODC --- 0x03EFA
 * 	Port A Nch open-drain control register
 */
#define		PAODC3		((UB_BITS*)(&PAODC))->bit3
#define		PAODC1		((UB_BITS*)(&PAODC))->bit1

/*
 *	CPUM --- 0x03F00
 *	CPU mode control register
 */
#define		OSCSEL1		((UB_BITS*)(&CPUM))->bit6
#define		OSCSEL0		((UB_BITS*)(&CPUM))->bit5
#define		OSCDBL		((UB_BITS*)(&CPUM))->bit4
#define		STOP		((UB_BITS*)(&CPUM))->bit3
#define		HALT		((UB_BITS*)(&CPUM))->bit2
#define		OSC1		((UB_BITS*)(&CPUM))->bit1
#define		OSC0		((UB_BITS*)(&CPUM))->bit0

/*
 *	MEMCTR --- 0x03F01
 *	Memory Control Register
 */
#define		IOW1		((UB_BITS*)(&MEMCTR))->bit7
#define		IOW0		((UB_BITS*)(&MEMCTR))->bit6
#define		IVBM		((UB_BITS*)(&MEMCTR))->bit5
#define		IRWE		((UB_BITS*)(&MEMCTR))->bit2

/*
 *	WDCTR --- 0x03F02
 *	Watchdog timer control register
 */
#define		WDTS1		((UB_BITS*)(&WDCTR))->bit2
#define		WDTS0		((UB_BITS*)(&WDCTR))->bit1
#define		WDEN		((UB_BITS*)(&WDCTR))->bit0

/*
 *	DLYCTR --- 0x03F03
 *	Oscillation stabilization wait time control register
 */
#define		BUZOE		((UB_BITS*)(&DLYCTR))->bit7
#define		BUZS2		((UB_BITS*)(&DLYCTR))->bit6
#define		BUZS1		((UB_BITS*)(&DLYCTR))->bit5
#define		BUZS0		((UB_BITS*)(&DLYCTR))->bit4
#define		DLYS1		((UB_BITS*)(&DLYCTR))->bit3
#define		DLYS0		((UB_BITS*)(&DLYCTR))->bit2

/*
 *	WD2CTR --- 0x03F05
 *	Watchdog timer 2 control register
 */
#define		WD2TS3		((UB_BITS*)(&WD2CTR))->bit3
#define		WD2TS2		((UB_BITS*)(&WD2CTR))->bit2
#define		WD2TS1		((UB_BITS*)(&WD2CTR))->bit1
#define		WD2TS0		((UB_BITS*)(&WD2CTR))->bit0


/*
 *	HANDSHAKE --- 0x03F06
 *	Internal ROM access method control register
 */
 
#define		ROMHND		((UB_BITS*)(&HANDSHAKE))->bit2

/*
 *	AUCTR --- 0x03F07
 *	Expanded Calculation Control Register
 */
#define		AUDIVU		((UB_BITS*)(&AUCTR))->bit2
#define		AUMUL		((UB_BITS*)(&AUCTR))->bit1
#define		AUMULU		((UB_BITS*)(&AUCTR))->bit0

/*
 *	SBNKR --- 0x03F0A
 *	Bank Register For Source Address
 */
#define		SBA3		((UB_BITS*)(&SBNKR))->bit3
#define		SBA2		((UB_BITS*)(&SBNKR))->bit2
#define		SBA1		((UB_BITS*)(&SBNKR))->bit1
#define		SBA0		((UB_BITS*)(&SBNKR))->bit0

/*
 *	DBNKR --- 0x03F0B
 *	Bank Register For Destination Address
 */
#define		DBA3		((UB_BITS*)(&DBNKR))->bit3
#define		DBA2		((UB_BITS*)(&DBNKR))->bit2
#define		DBA1		((UB_BITS*)(&DBNKR))->bit1
#define		DBA0		((UB_BITS*)(&DBNKR))->bit0

/*
 *	SC0SEL  --- 0x03F10
 *	シリアル0入出力端子切換え制御レジスタ
 */
#define		SBO0SEL		((UB_BITS*)(&SC0SEL))->bit7
#define		SC0BRP2		((UB_BITS*)(&SC0SEL))->bit6
#define		SC0BRP1		((UB_BITS*)(&SC0SEL))->bit5
#define		SC0BRP0		((UB_BITS*)(&SC0SEL))->bit4
#define		OSL0		((UB_BITS*)(&SC0SEL))->bit3
#define		SC0SEL2		((UB_BITS*)(&SC0SEL))->bit2
#define		SC0SEL1		((UB_BITS*)(&SC0SEL))->bit1
#define		SC0SEL0		((UB_BITS*)(&SC0SEL))->bit0

/*
 *	SC0MD0 --- 0x03F11
 *	Serial interface 0 mode register 0
 */
#define		SC0CE1		((UB_BITS*)(&SC0MD0))->bit7
#define		SC0DIR		((UB_BITS*)(&SC0MD0))->bit4
#define		SC0STE		((UB_BITS*)(&SC0MD0))->bit3
#define		SC0LNG2		((UB_BITS*)(&SC0MD0))->bit2
#define		SC0LNG1		((UB_BITS*)(&SC0MD0))->bit1
#define		SC0LNG0		((UB_BITS*)(&SC0MD0))->bit0

/*
 *	SC0MD1--- 0x03F12
 *	Serial interface 0 mode register 1
 */
#define		SC0IOM		((UB_BITS*)(&SC0MD1))->bit7
#define		SC0SBTS		((UB_BITS*)(&SC0MD1))->bit6
#define		SC0SBIS		((UB_BITS*)(&SC0MD1))->bit5
#define		SC0SBOS		((UB_BITS*)(&SC0MD1))->bit4
#define		SC0CKM		((UB_BITS*)(&SC0MD1))->bit3
#define		SC0MST		((UB_BITS*)(&SC0MD1))->bit2
#define		SC0DIV		((UB_BITS*)(&SC0MD1))->bit1
#define		SC0CMD		((UB_BITS*)(&SC0MD1))->bit0

/*
 *	SC0MD2 --- 0x03F13
 *	Serial interface 0 mode register 2
 */
#define		SC0FM1		((UB_BITS*)(&SC0MD2))->bit7
#define		SC0FM0		((UB_BITS*)(&SC0MD2))->bit6
#define		SC0PM1		((UB_BITS*)(&SC0MD2))->bit5
#define		SC0PM0		((UB_BITS*)(&SC0MD2))->bit4
#define		SC0NPE		((UB_BITS*)(&SC0MD2))->bit3
#define		SC0BRKF		((UB_BITS*)(&SC0MD2))->bit1
#define		SC0BRKE		((UB_BITS*)(&SC0MD2))->bit0

/*
 *	SC0MD3 --- 0x03F14
 *	Serial interface 0 mode register 3
 */
#define		SC0FDC1		((UB_BITS*)(&SC0MD3))->bit7
#define		SC0FDC0		((UB_BITS*)(&SC0MD3))->bit6
#define		SC0PSCE		((UB_BITS*)(&SC0MD3))->bit3
#define		SC0PSC2		((UB_BITS*)(&SC0MD3))->bit2
#define		SC0PSC1		((UB_BITS*)(&SC0MD3))->bit1
#define		SC0PSC0		((UB_BITS*)(&SC0MD3))->bit0

/*
 *	SC0STR --- 0x03F15
 *	Serial interface 0 status register
 */
#define		SC0TBSY		((UB_BITS*)(&SC0STR))->bit7
#define		SC0RBSY		((UB_BITS*)(&SC0STR))->bit6
#define		SC0TEMP		((UB_BITS*)(&SC0STR))->bit5
#define		SC0REMP		((UB_BITS*)(&SC0STR))->bit4
#define		SC0FEF		((UB_BITS*)(&SC0STR))->bit3
#define		SC0PEK		((UB_BITS*)(&SC0STR))->bit2
#define		SC0ORE		((UB_BITS*)(&SC0STR))->bit1
#define		SC0ERE		((UB_BITS*)(&SC0STR))->bit0

/*
 *	RXBUF0 --- 0x03F16
 *	Serial interface 0 reception data buffer
 */
#define		RXBUF07		((UB_BITS*)(&RXBUF0))->bit7
#define		RXBUF06		((UB_BITS*)(&RXBUF0))->bit6
#define		RXBUF05		((UB_BITS*)(&RXBUF0))->bit5
#define		RXBUF04		((UB_BITS*)(&RXBUF0))->bit4
#define		RXBUF03		((UB_BITS*)(&RXBUF0))->bit3
#define		RXBUF02		((UB_BITS*)(&RXBUF0))->bit2
#define		RXBUF01		((UB_BITS*)(&RXBUF0))->bit1
#define		RXBUF00		((UB_BITS*)(&RXBUF0))->bit0

/*
 *	TXBUF0 --- 0x03F17
 *	Serial interface 0 transmission data buffer
 */
#define		TXBUF07		((UB_BITS*)(&TXBUF0))->bit7
#define		TXBUF06		((UB_BITS*)(&TXBUF0))->bit6
#define		TXBUF05		((UB_BITS*)(&TXBUF0))->bit5
#define		TXBUF04		((UB_BITS*)(&TXBUF0))->bit4
#define		TXBUF03		((UB_BITS*)(&TXBUF0))->bit3
#define		TXBUF02		((UB_BITS*)(&TXBUF0))->bit2
#define		TXBUF01		((UB_BITS*)(&TXBUF0))->bit1
#define		TXBUF00		((UB_BITS*)(&TXBUF0))->bit0

/*
 *	LINCTR --- 0x03F18
 *	LIN control register
 */
#define		SFBSY		((UB_BITS*)(&LINCTR))->bit7
#define		SBFBSY		((UB_BITS*)(&LINCTR))->bit6
#define		SFREC		((UB_BITS*)(&LINCTR))->bit5
#define		SBFREC		((UB_BITS*)(&LINCTR))->bit4
#define		SBFEN		((UB_BITS*)(&LINCTR))->bit3
#define		SBFSEL2		((UB_BITS*)(&LINCTR))->bit2
#define		SBFSEL1		((UB_BITS*)(&LINCTR))->bit1
#define		SBFSEL0		((UB_BITS*)(&LINCTR))->bit0


/*
 *	RSTFACT --- 0x03F19
 *	Reset Factor Determination register
 */
#define		RSTMON		((UB_BITS*)(&RSTFACT))->bit0

/*
 *	OSCCNT --- 0x03F1A
 *	External high-speed oscillation control register
 */
 
#define		HOSCDIVSEL2		((UB_BITS*)(&OSCCNT))->bit7
#define		HOSCDIVSEL1		((UB_BITS*)(&OSCCNT))->bit6
#define		HOSCDIVSEL0		((UB_BITS*)(&OSCCNT))->bit5
#define		HOSCDIVSTOP		((UB_BITS*)(&OSCCNT))->bit4
#define		HOSCSEL			((UB_BITS*)(&OSCCNT))->bit1
#define		HOSCCNT			((UB_BITS*)(&OSCCNT))->bit0



/*
 *	RCCNT --- 0x03F1B
 *	Internal high-speed oscillation control register
 */
#define		RCFSEL		((UB_BITS*)(&RCCNT))->bit1
#define		RCON		((UB_BITS*)(&RCCNT))->bit0

/*
 *	OSCSCNT --- 0x03F1C
 *	External low-speed oscillation control register
 */
#define		SOSCSEL		((UB_BITS*)(&OSCSCNT))->bit1
#define		SOSCCNT		((UB_BITS*)(&OSCSCNT))->bit0

/*
 *	RCSCNT --- 0x03F1D
 *	Internal low-speed oscillation control register
 */
#define		RCSON			((UB_BITS*)(&RCSCNT))->bit0

/*
 *	EDGDT --- 0x03F1E
 *	Both edges interrupt control register
 */
#define		EDGSEL7		((UB_BITS*)(&EDGDT))->bit7
#define		EDGSEL5		((UB_BITS*)(&EDGDT))->bit5
#define		EDGSEL3		((UB_BITS*)(&EDGDT))->bit3
#define		EDGSEL2		((UB_BITS*)(&EDGDT))->bit2
#define		EDGSEL0		((UB_BITS*)(&EDGDT))->bit0


/*
 *	PLLCNT --- 0x03F1F
 *	Clock multiplication circuit control register
 */
#define		PLLCK3		((UB_BITS*)(&PLLCNT))->bit7
#define		PLLCK2		((UB_BITS*)(&PLLCNT))->bit6
#define		PLLCK1		((UB_BITS*)(&PLLCNT))->bit5
#define		PLLCK0		((UB_BITS*)(&PLLCNT))->bit4
#define		PLLEN		((UB_BITS*)(&PLLCNT))->bit1
#define		PLLSTART	((UB_BITS*)(&PLLCNT))->bit0

/*
 *	RXBUF1 --- 0x03F26
 *	Serial interface 1 reception data buffer
 */
#define		RXBUF17		((UB_BITS*)(&RXBUF1))->bit7
#define		RXBUF16		((UB_BITS*)(&RXBUF1))->bit6
#define		RXBUF15		((UB_BITS*)(&RXBUF1))->bit5
#define		RXBUF14		((UB_BITS*)(&RXBUF1))->bit4
#define		RXBUF13		((UB_BITS*)(&RXBUF1))->bit3
#define		RXBUF12		((UB_BITS*)(&RXBUF1))->bit2
#define		RXBUF11		((UB_BITS*)(&RXBUF1))->bit1
#define		RXBUF10		((UB_BITS*)(&RXBUF1))->bit0

/*
 *	TXBUF1 --- 0x03F27
 *	Serial interface 1 transmission data buffer
 */
#define		TXBUF17		((UB_BITS*)(&TXBUF1))->bit7
#define		TXBUF16		((UB_BITS*)(&TXBUF1))->bit6
#define		TXBUF15		((UB_BITS*)(&TXBUF1))->bit5
#define		TXBUF14		((UB_BITS*)(&TXBUF1))->bit4
#define		TXBUF13		((UB_BITS*)(&TXBUF1))->bit3
#define		TXBUF12		((UB_BITS*)(&TXBUF1))->bit2
#define		TXBUF11		((UB_BITS*)(&TXBUF1))->bit1
#define		TXBUF10		((UB_BITS*)(&TXBUF1))->bit0

/*
 *	SCINTSEL --- 0x03F28
 *  Clock synchronous serial reception interrupt control register
 */
#define		SC1IGC		((UB_BITS*)(&SCINTSEL))->bit1
#define		SC0IGC		((UB_BITS*)(&SCINTSEL))->bit0


/*
 *	LVIMD --- 0x03F2F
 *	Power Supply Detection Control Registers
 */
#define		LVIMOD			((UB_BITS*)(&LVIMD ))->bit7
#define		LVICTR2			((UB_BITS*)(&LVIMD ))->bit6
#define		LVICTR1			((UB_BITS*)(&LVIMD ))->bit5
#define		LVICTR0			((UB_BITS*)(&LVIMD ))->bit4
#define		LVIMON			((UB_BITS*)(&LVIMD ))->bit1
#define		LVION			((UB_BITS*)(&LVIMD ))->bit0

/*
 *	KEYT3_1IMD --- 0x03F3E
 *	Key interrupt control register 1
 */
#define		KEYT3SEL	((UB_BITS*)(&KEYT3_1IMD))->bit7
#define		KEYT3_1EN3	((UB_BITS*)(&KEYT3_1IMD))->bit3
#define		KEYT3_1EN2	((UB_BITS*)(&KEYT3_1IMD))->bit2
#define		KEYT3_1EN1	((UB_BITS*)(&KEYT3_1IMD))->bit1
#define		KEYT3_1EN0	((UB_BITS*)(&KEYT3_1IMD))->bit0

/*
 *	KEYT3_2IMD --- 0x03F3F
 *	Key interrupt control register 2
 */
#define		KEYT3_2EN3	((UB_BITS*)(&KEYT3_2IMD))->bit3
#define		KEYT3_2EN2	((UB_BITS*)(&KEYT3_2IMD))->bit2
#define		KEYT3_2EN1	((UB_BITS*)(&KEYT3_2IMD))->bit1
#define		KEYT3_2EN0	((UB_BITS*)(&KEYT3_2IMD))->bit0

/*
 *	WD2MDSEL --- 0x03F4C
 *	Watchdog timer 2 operation mode setup register
 */
#define		WD2MDSEL7	((UB_BITS*)(&WD2MDSEL))->bit7
#define		WD2MDSEL6	((UB_BITS*)(&WD2MDSEL))->bit6
#define		WD2MDSEL5	((UB_BITS*)(&WD2MDSEL))->bit5
#define		WD2MDSEL4	((UB_BITS*)(&WD2MDSEL))->bit4
#define		WD2MDSEL3	((UB_BITS*)(&WD2MDSEL))->bit3
#define		WD2MDSEL2	((UB_BITS*)(&WD2MDSEL))->bit2
#define		WD2MDSEL1	((UB_BITS*)(&WD2MDSEL))->bit1
#define		WD2MDSEL0	((UB_BITS*)(&WD2MDSEL))->bit0

/*
 *	WD2CLR --- 0x03F4D
 *	Watchdog timer 2 clear register
 */
#define		WD2CLR7		((UB_BITS*)(&WD2CLR))->bit7
#define		WD2CLR6		((UB_BITS*)(&WD2CLR))->bit6
#define		WD2CLR5		((UB_BITS*)(&WD2CLR))->bit5
#define		WD2CLR4		((UB_BITS*)(&WD2CLR))->bit4
#define		WD2CLR3		((UB_BITS*)(&WD2CLR))->bit3
#define		WD2CLR2		((UB_BITS*)(&WD2CLR))->bit2
#define		WD2CLR1		((UB_BITS*)(&WD2CLR))->bit1
#define		WD2CLR0		((UB_BITS*)(&WD2CLR))->bit0



/*
 *	SC4MD0 --- 0x03F50
 *	Serial interface 4 mode register 0
 */
#define		SC4CE1		((UB_BITS*)(&SC4MD0))->bit7
#define		SC4DIR		((UB_BITS*)(&SC4MD0))->bit4
#define		SC4STE		((UB_BITS*)(&SC4MD0))->bit3
#define		SC4LNG2		((UB_BITS*)(&SC4MD0))->bit2
#define		SC4LNG1		((UB_BITS*)(&SC4MD0))->bit1
#define		SC4LNG0		((UB_BITS*)(&SC4MD0))->bit0

/*
 *	SC4MD1 --- 0x03F51
 *	Serial interface 4 mode register 1
 */
#define		SC4IOM		((UB_BITS*)(&SC4MD1))->bit7
#define		SC4SBTS		((UB_BITS*)(&SC4MD1))->bit6
#define		SC4SBIS		((UB_BITS*)(&SC4MD1))->bit5
#define		SC4SBOS		((UB_BITS*)(&SC4MD1))->bit4
#define		SC4MST		((UB_BITS*)(&SC4MD1))->bit2

/*
 *	SC4MD2 --- 0x03F52
 *	Serial interface 4 mode register 2
 */
#define		SC4FDC1		((UB_BITS*)(&SC4MD2))->bit7
#define		SC4FDC0		((UB_BITS*)(&SC4MD2))->bit6
#define		SC4PSCE		((UB_BITS*)(&SC4MD2))->bit3
#define		SC4PSC2		((UB_BITS*)(&SC4MD2))->bit2
#define		SC4PSC1		((UB_BITS*)(&SC4MD2))->bit1
#define		SC4PSC0		((UB_BITS*)(&SC4MD2))->bit0

/*
 *	SC4MD3 --- 0x03F53
 *	Serial interface 4 mode register 3
 */

//#define		SC4SMB		((UB_BITS*)(&SC4MD3))->bit7
#define		SC4ADM		((UB_BITS*)(&SC4MD3))->bit6
#define		SC4STPC		((UB_BITS*)(&SC4MD3))->bit5
#define		SC4TMD		((UB_BITS*)(&SC4MD3))->bit4
#define		SC4REX		((UB_BITS*)(&SC4MD3))->bit3
#define		SC4CMD		((UB_BITS*)(&SC4MD3))->bit2
#define		SC4ACKS		((UB_BITS*)(&SC4MD3))->bit1
#define		SC4ACK0		((UB_BITS*)(&SC4MD3))->bit0


/*
 *	SC4AD0 --- 0x03F54
 *	Serial interface 4 address setting register 0
 */

#define		SC4ADR7		((UB_BITS*)(&SC4AD0))->bit7
#define		SC4ADR6		((UB_BITS*)(&SC4AD0))->bit6
#define		SC4ADR5		((UB_BITS*)(&SC4AD0))->bit5
#define		SC4ADR4		((UB_BITS*)(&SC4AD0))->bit4
#define		SC4ADR3		((UB_BITS*)(&SC4AD0))->bit3
#define		SC4ADR2		((UB_BITS*)(&SC4AD0))->bit2
#define		SC4ADR1		((UB_BITS*)(&SC4AD0))->bit1
#define		SC4ADR0		((UB_BITS*)(&SC4AD0))->bit0

/*
 *	SC4AD1 --- 0x03F55
 *	
 */
 
#define		SC4ADR9		((UB_BITS*)(&SC4AD1))->bit1
#define		SC4ADR8		((UB_BITS*)(&SC4AD1))->bit0


/*
 *	SC4STR0 --- 0x03F56
 *	Serial interface 4 status register 0
 */
#define		SC4BSY		((UB_BITS*)(&SC4STR0))->bit7
#define		SC4TEMP		((UB_BITS*)(&SC4STR0))->bit5
#define		SC4REMP		((UB_BITS*)(&SC4STR0))->bit4
#define		SC4ORE		((UB_BITS*)(&SC4STR0))->bit0

/*
 *	SC4STR1 --- 0x03F57
 *	Serial interface 4 status register 1
 */
#define		SC4WSR		((UB_BITS*)(&SC4STR1))->bit7
#define		SC4ABT_LST	((UB_BITS*)(&SC4STR1))->bit6
#define		SC4ADD_ACC	((UB_BITS*)(&SC4STR1))->bit5
#define		SC4STRT		((UB_BITS*)(&SC4STR1))->bit4
#define		SC4BUSBSY	((UB_BITS*)(&SC4STR1))->bit3
#define		SC4IICBSY	((UB_BITS*)(&SC4STR1))->bit2
#define		SC4GCALL	((UB_BITS*)(&SC4STR1))->bit1
#define		SC4DATA_ERR	((UB_BITS*)(&SC4STR1))->bit0

/*
 *	RXBUF4 --- 0x03F58
 *	Serial interface 4 reception data buffer
 */
#define		RXBUF47		((UB_BITS*)(&RXBUF4))->bit7
#define		RXBUF46		((UB_BITS*)(&RXBUF4))->bit6
#define		RXBUF45		((UB_BITS*)(&RXBUF4))->bit5
#define		RXBUF44		((UB_BITS*)(&RXBUF4))->bit4
#define		RXBUF43		((UB_BITS*)(&RXBUF4))->bit3
#define		RXBUF42		((UB_BITS*)(&RXBUF4))->bit2
#define		RXBUF41		((UB_BITS*)(&RXBUF4))->bit1
#define		RXBUF40		((UB_BITS*)(&RXBUF4))->bit0

/*
 *	TXBUF4 --- 0x03F59
 *	Serial interface 4 transmission data buffer
 */
#define		TXBUF47		((UB_BITS*)(&TXBUF4))->bit7
#define		TXBUF46		((UB_BITS*)(&TXBUF4))->bit6
#define		TXBUF45		((UB_BITS*)(&TXBUF4))->bit5
#define		TXBUF44		((UB_BITS*)(&TXBUF4))->bit4
#define		TXBUF43		((UB_BITS*)(&TXBUF4))->bit3
#define		TXBUF42		((UB_BITS*)(&TXBUF4))->bit2
#define		TXBUF41		((UB_BITS*)(&TXBUF4))->bit1
#define		TXBUF40		((UB_BITS*)(&TXBUF4))->bit0

/*
 *	SC4SEL --- 0x03F5A
 *	Serial 4 I/O pin switching control register
 */
#define		SC4BRP2		((UB_BITS*)(&SC4SEL))->bit6
#define		SC4BRP1		((UB_BITS*)(&SC4SEL))->bit5
#define		SC4BRP0		((UB_BITS*)(&SC4SEL))->bit4
//#define		OSL4		((UB_BITS*)(&SC4SEL))->bit3
#define		SC4SEL2		((UB_BITS*)(&SC4SEL))->bit2
#define		SC4SEL1		((UB_BITS*)(&SC4SEL))->bit1
#define		SC4SEL0		((UB_BITS*)(&SC4SEL))->bit0

/*
 *	STB_EDG --- 0x03F5E
 *	Edge interrupt in standby control register
 */
 
#define		STB_EDG_IRQ4	((UB_BITS*)(&STB_EDG))->bit4
#define		STB_EDG_IRQ3	((UB_BITS*)(&STB_EDG))->bit3
#define		STB_EDG_IRQ2	((UB_BITS*)(&STB_EDG))->bit2
#define		STB_EDG_IRQ1	((UB_BITS*)(&STB_EDG))->bit1
#define		STB_EDG_IRQ0	((UB_BITS*)(&STB_EDG))->bit0


/*
 *	STB_MSK --- 0x03F5F
 *	Interrupt in standby control register
 */
 
#define		STB_MSK_IRQ4		((UB_BITS*)(&STB_MSK))->bit4
#define		STB_MSK_IRQ3		((UB_BITS*)(&STB_MSK))->bit3
#define		STB_MSK_IRQ2		((UB_BITS*)(&STB_MSK))->bit2
#define		STB_MSK_IRQ1		((UB_BITS*)(&STB_MSK))->bit1
#define		STB_MSK_IRQ0		((UB_BITS*)(&STB_MSK))->bit0


/*
 *	TM0BC --- 0x03F60
 *	Timer 0 binary counter
 */
#define		TM0BC7		((UB_BITS*)(&TM0BC))->bit7
#define		TM0BC6		((UB_BITS*)(&TM0BC))->bit6
#define		TM0BC5		((UB_BITS*)(&TM0BC))->bit5
#define		TM0BC4		((UB_BITS*)(&TM0BC))->bit4
#define		TM0BC3		((UB_BITS*)(&TM0BC))->bit3
#define		TM0BC2		((UB_BITS*)(&TM0BC))->bit2
#define		TM0BC1		((UB_BITS*)(&TM0BC))->bit1
#define		TM0BC0		((UB_BITS*)(&TM0BC))->bit0

/*
 *	TM1BC --- 0x03F61
 *	Timer 1 binary counter
 */
#define		TM1BC7		((UB_BITS*)(&TM1BC))->bit7
#define		TM1BC6		((UB_BITS*)(&TM1BC))->bit6
#define		TM1BC5		((UB_BITS*)(&TM1BC))->bit5
#define		TM1BC4		((UB_BITS*)(&TM1BC))->bit4
#define		TM1BC3		((UB_BITS*)(&TM1BC))->bit3
#define		TM1BC2		((UB_BITS*)(&TM1BC))->bit2
#define		TM1BC1		((UB_BITS*)(&TM1BC))->bit1
#define		TM1BC0		((UB_BITS*)(&TM1BC))->bit0

/*
 *	TM0OC --- 0x03F62
 *	Timer 0 compare register
 */
#define		TM0OC7		((UB_BITS*)(&TM0OC))->bit7
#define		TM0OC6		((UB_BITS*)(&TM0OC))->bit6
#define		TM0OC5		((UB_BITS*)(&TM0OC))->bit5
#define		TM0OC4		((UB_BITS*)(&TM0OC))->bit4
#define		TM0OC3		((UB_BITS*)(&TM0OC))->bit3
#define		TM0OC2		((UB_BITS*)(&TM0OC))->bit2
#define		TM0OC1		((UB_BITS*)(&TM0OC))->bit1
#define		TM0OC0		((UB_BITS*)(&TM0OC))->bit0

/*
 *	TM1OC --- 0x03F63
 *	Timer 1 compare register
 */
#define		TM1OC7		((UB_BITS*)(&TM1OC))->bit7
#define		TM1OC6		((UB_BITS*)(&TM1OC))->bit6
#define		TM1OC5		((UB_BITS*)(&TM1OC))->bit5
#define		TM1OC4		((UB_BITS*)(&TM1OC))->bit4
#define		TM1OC3		((UB_BITS*)(&TM1OC))->bit3
#define		TM1OC2		((UB_BITS*)(&TM1OC))->bit2
#define		TM1OC1		((UB_BITS*)(&TM1OC))->bit1
#define		TM1OC0		((UB_BITS*)(&TM1OC))->bit0

/*
 *	TM0MD --- 0x03F64
 *	Timer 0 mode register
 */
#define		TM0POP		((UB_BITS*)(&TM0MD))->bit6
#define		TM0MOD		((UB_BITS*)(&TM0MD))->bit5
#define		TM0PWM		((UB_BITS*)(&TM0MD))->bit4
#define		TM0EN		((UB_BITS*)(&TM0MD))->bit3
#define		TM0CK2		((UB_BITS*)(&TM0MD))->bit2
#define		TM0CK1		((UB_BITS*)(&TM0MD))->bit1
#define		TM0CK0		((UB_BITS*)(&TM0MD))->bit0

/*
 *	TM1MD --- 0x03F65
 *	Timer 1 mode register
 */
#define		TM1CAS1		((UB_BITS*)(&TM1MD))->bit5
#define		TM1CAS0		((UB_BITS*)(&TM1MD))->bit4
#define		TM1EN		((UB_BITS*)(&TM1MD))->bit3
#define		TM1CK2		((UB_BITS*)(&TM1MD))->bit2
#define		TM1CK1		((UB_BITS*)(&TM1MD))->bit1
#define		TM1CK0		((UB_BITS*)(&TM1MD))->bit0

/*
 *	CK0MD --- 0x03F66
 *	Timer 0 prescaler selection register
 */
#define		TM0ADD1		((UB_BITS*)(&CK0MD))->bit6
#define		TM0ADD0		((UB_BITS*)(&CK0MD))->bit5
#define		TM0ADDEN	((UB_BITS*)(&CK0MD))->bit4
#define		TM0PSC2		((UB_BITS*)(&CK0MD))->bit3
#define		TM0PSC1		((UB_BITS*)(&CK0MD))->bit2
#define		TM0PSC0		((UB_BITS*)(&CK0MD))->bit1
#define		TM0BAS		((UB_BITS*)(&CK0MD))->bit0

/*
 *	CK1MD --- 0x03F67
 *	Timer 1 prescaler selection register
 */
#define		TM1PSC2		((UB_BITS*)(&CK1MD))->bit3
#define		TM1PSC1		((UB_BITS*)(&CK1MD))->bit2
#define		TM1PSC0		((UB_BITS*)(&CK1MD))->bit1
#define		TM1BAS		((UB_BITS*)(&CK1MD))->bit0

/*
 *	TM2BC --- 0x03F68
 *	Timer 2 binary counter
 */
#define		TM2BC7		((UB_BITS*)(&TM2BC))->bit7
#define		TM2BC6		((UB_BITS*)(&TM2BC))->bit6
#define		TM2BC5		((UB_BITS*)(&TM2BC))->bit5
#define		TM2BC4		((UB_BITS*)(&TM2BC))->bit4
#define		TM2BC3		((UB_BITS*)(&TM2BC))->bit3
#define		TM2BC2		((UB_BITS*)(&TM2BC))->bit2
#define		TM2BC1		((UB_BITS*)(&TM2BC))->bit1
#define		TM2BC0		((UB_BITS*)(&TM2BC))->bit0

/*
 *	TM2OC --- 0x03F6A
 *	Timer 2 compare register
 */
#define		TM2OC7		((UB_BITS*)(&TM2OC))->bit7
#define		TM2OC6		((UB_BITS*)(&TM2OC))->bit6
#define		TM2OC5		((UB_BITS*)(&TM2OC))->bit5
#define		TM2OC4		((UB_BITS*)(&TM2OC))->bit4
#define		TM2OC3		((UB_BITS*)(&TM2OC))->bit3
#define		TM2OC2		((UB_BITS*)(&TM2OC))->bit2
#define		TM2OC1		((UB_BITS*)(&TM2OC))->bit1
#define		TM2OC0		((UB_BITS*)(&TM2OC))->bit0

/*
 *	TM2MD --- 0x03F6C
 *	Timer 2 mode register
 */
#define		TM2POP		((UB_BITS*)(&TM2MD))->bit6
#define		TM2MOD		((UB_BITS*)(&TM2MD))->bit5
#define		TM2PWM		((UB_BITS*)(&TM2MD))->bit4
#define		TM2EN		((UB_BITS*)(&TM2MD))->bit3
#define		TM2CK2		((UB_BITS*)(&TM2MD))->bit2
#define		TM2CK1		((UB_BITS*)(&TM2MD))->bit1
#define		TM2CK0		((UB_BITS*)(&TM2MD))->bit0

/*
 *	CK2MD --- 0x03F6E
 *	Timer 2 prescaler selection register
 */
#define		TM2ADD1		((UB_BITS*)(&CK2MD))->bit6
#define		TM2ADD0		((UB_BITS*)(&CK2MD))->bit5
#define		TM2ADDEN	((UB_BITS*)(&CK2MD))->bit4
#define		TM2PSC2		((UB_BITS*)(&CK2MD))->bit3
#define		TM2PSC1		((UB_BITS*)(&CK2MD))->bit2
#define		TM2PSC0		((UB_BITS*)(&CK2MD))->bit1
#define		TM2BAS		((UB_BITS*)(&CK2MD))->bit0

/*
 *	TMABC --- 0x03F71
 *	Timer A binary counter
 */
#define		TMABC7		((UB_BITS*)(&TMABC))->bit7
#define		TMABC6		((UB_BITS*)(&TMABC))->bit6
#define		TMABC5		((UB_BITS*)(&TMABC))->bit5
#define		TMABC4		((UB_BITS*)(&TMABC))->bit4
#define		TMABC3		((UB_BITS*)(&TMABC))->bit3
#define		TMABC2		((UB_BITS*)(&TMABC))->bit2
#define		TMABC1		((UB_BITS*)(&TMABC))->bit1
#define		TMABC0		((UB_BITS*)(&TMABC))->bit0

/*
 *	TMAOC --- 0x03F73
 *	Timer A compare register
 */
#define		TMAOC7		((UB_BITS*)(&TMAOC))->bit7
#define		TMAOC6		((UB_BITS*)(&TMAOC))->bit6
#define		TMAOC5		((UB_BITS*)(&TMAOC))->bit5
#define		TMAOC4		((UB_BITS*)(&TMAOC))->bit4
#define		TMAOC3		((UB_BITS*)(&TMAOC))->bit3
#define		TMAOC2		((UB_BITS*)(&TMAOC))->bit2
#define		TMAOC1		((UB_BITS*)(&TMAOC))->bit1
#define		TMAOC0		((UB_BITS*)(&TMAOC))->bit0

/*
 *	TMAMD1 --- 0x03F75
 *	Timer A mode register 1
 */
#define		TMAEN		((UB_BITS*)(&TMAMD1))->bit3
#define		TMACK2		((UB_BITS*)(&TMAMD1))->bit2
#define		TMACK1		((UB_BITS*)(&TMAMD1))->bit1
#define		TMACK0		((UB_BITS*)(&TMAMD1))->bit0

/*
 *	TMAMD2 --- 0x03F77
 *	Timer A mode register 2
 */
#define		PSCEN		((UB_BITS*)(&TMAMD2))->bit6

/*
 *	TM6BC --- 0x03F78
 *	Timer 6 binary counter
 */
#define		TM6BC7		((UB_BITS*)(&TM6BC))->bit7
#define		TM6BC6		((UB_BITS*)(&TM6BC))->bit6
#define		TM6BC5		((UB_BITS*)(&TM6BC))->bit5
#define		TM6BC4		((UB_BITS*)(&TM6BC))->bit4
#define		TM6BC3		((UB_BITS*)(&TM6BC))->bit3
#define		TM6BC2		((UB_BITS*)(&TM6BC))->bit2
#define		TM6BC1		((UB_BITS*)(&TM6BC))->bit1
#define		TM6BC0		((UB_BITS*)(&TM6BC))->bit0

/*
 *	TM6OC --- 0x03F79
 *	Timer 6 compare register
 */
#define		TM6OC7		((UB_BITS*)(&TM6OC))->bit7
#define		TM6OC6		((UB_BITS*)(&TM6OC))->bit6
#define		TM6OC5		((UB_BITS*)(&TM6OC))->bit5
#define		TM6OC4		((UB_BITS*)(&TM6OC))->bit4
#define		TM6OC3		((UB_BITS*)(&TM6OC))->bit3
#define		TM6OC2		((UB_BITS*)(&TM6OC))->bit2
#define		TM6OC1		((UB_BITS*)(&TM6OC))->bit1
#define		TM6OC0		((UB_BITS*)(&TM6OC))->bit0

/*
 *	TM6MD --- 0x03F7A
 *	Timer 6 mode register
 */
#define		TM6CLRS		((UB_BITS*)(&TM6MD))->bit7
#define		TM6IR2		((UB_BITS*)(&TM6MD))->bit6
#define		TM6IR1		((UB_BITS*)(&TM6MD))->bit5
#define		TM6IR0		((UB_BITS*)(&TM6MD))->bit4
#define		TM6CK3		((UB_BITS*)(&TM6MD))->bit3
#define		TM6CK2		((UB_BITS*)(&TM6MD))->bit2
#define		TM6CK1		((UB_BITS*)(&TM6MD))->bit1
#define		TM6CK0		((UB_BITS*)(&TM6MD))->bit0

/*
 *	TBCLR --- 0x03F7B
 *	Time base timer clear control register
 */
#define		TBCLR7		((UB_BITS*)(&TBCLR))->bit7
#define		TBCLR6		((UB_BITS*)(&TBCLR))->bit6
#define		TBCLR5		((UB_BITS*)(&TBCLR))->bit5
#define		TBCLR4		((UB_BITS*)(&TBCLR))->bit4
#define		TBCLR3		((UB_BITS*)(&TBCLR))->bit3
#define		TBCLR2		((UB_BITS*)(&TBCLR))->bit2
#define		TBCLR1		((UB_BITS*)(&TBCLR))->bit1
#define		TBCLR0		((UB_BITS*)(&TBCLR))->bit0

/*
 *	TM6BEN --- 0x03F7C
 *	Timer 6 enable register
 */
#define		TBEN		((UB_BITS*)(&TM6BEN))->bit1
#define		TM6EN		((UB_BITS*)(&TM6BEN))->bit0

/*
 *	RMCTR --- 0x03F7D
 *	Remote control carrier output control register
 */
#define		TM0RMC		((UB_BITS*)(&RMCTR))->bit7
#define		TM0RMB		((UB_BITS*)(&RMCTR))->bit6
#define		TM0RMA		((UB_BITS*)(&RMCTR))->bit4
#define		RMOEN		((UB_BITS*)(&RMCTR))->bit3
#define		RMDTY1		((UB_BITS*)(&RMCTR))->bit2
#define		RMDTY0		((UB_BITS*)(&RMCTR))->bit1
//#define		RMBTMS		((UB_BITS*)(&RMCTR))->bit0

/*
 *	TM7MD4 --- 0x03F7E
 *	Timer 7 mode register 4
 */
#define		T7ONESHOT	((UB_BITS*)(&TM7MD4))->bit4
#define		T7ICT2		((UB_BITS*)(&TM7MD4))->bit1
#define		T7CAPCLR	((UB_BITS*)(&TM7MD4))->bit0

/*
 *	TM7BCL --- 0x03F80
 * 	Timer 7 binary counter (lower 8 bits)
 */
#define		TM7BCL7		((UB_BITS*)(&TM7BCL))->bit7
#define		TM7BCL6		((UB_BITS*)(&TM7BCL))->bit6
#define		TM7BCL5		((UB_BITS*)(&TM7BCL))->bit5
#define		TM7BCL4		((UB_BITS*)(&TM7BCL))->bit4
#define		TM7BCL3		((UB_BITS*)(&TM7BCL))->bit3
#define		TM7BCL2		((UB_BITS*)(&TM7BCL))->bit2
#define		TM7BCL1		((UB_BITS*)(&TM7BCL))->bit1
#define		TM7BCL0		((UB_BITS*)(&TM7BCL))->bit0

/*
 *	TM7BCH --- 0x03F81
 * 	Timer 7 binary counter (upper 8 bits)
 */
#define		TM7BCH7		((UB_BITS*)(&TM7BCH))->bit7
#define		TM7BCH6		((UB_BITS*)(&TM7BCH))->bit6
#define		TM7BCH5		((UB_BITS*)(&TM7BCH))->bit5
#define		TM7BCH4		((UB_BITS*)(&TM7BCH))->bit4
#define		TM7BCH3		((UB_BITS*)(&TM7BCH))->bit3
#define		TM7BCH2		((UB_BITS*)(&TM7BCH))->bit2
#define		TM7BCH1		((UB_BITS*)(&TM7BCH))->bit1
#define		TM7BCH0		((UB_BITS*)(&TM7BCH))->bit0

/*
 *	TM7OC1L --- 0x03F82
 * 	Timer 7 compare register 1 (lower 8 bits)
 */
#define		TM7OC1L7	((UB_BITS*)(&TM7OC1L))->bit7
#define		TM7OC1L6	((UB_BITS*)(&TM7OC1L))->bit6
#define		TM7OC1L5	((UB_BITS*)(&TM7OC1L))->bit5
#define		TM7OC1L4	((UB_BITS*)(&TM7OC1L))->bit4
#define		TM7OC1L3	((UB_BITS*)(&TM7OC1L))->bit3
#define		TM7OC1L2	((UB_BITS*)(&TM7OC1L))->bit2
#define		TM7OC1L1	((UB_BITS*)(&TM7OC1L))->bit1
#define		TM7OC1L0	((UB_BITS*)(&TM7OC1L))->bit0

/*
 *	TM7OC1H --- 0x03F83
 * 	Timer 7 compare register 1 (upper 8 bits)
 */
#define		TM7OC1H7	((UB_BITS*)(&TM7OC1H))->bit7
#define		TM7OC1H6	((UB_BITS*)(&TM7OC1H))->bit6
#define		TM7OC1H5	((UB_BITS*)(&TM7OC1H))->bit5
#define		TM7OC1H4	((UB_BITS*)(&TM7OC1H))->bit4
#define		TM7OC1H3	((UB_BITS*)(&TM7OC1H))->bit3
#define		TM7OC1H2	((UB_BITS*)(&TM7OC1H))->bit2
#define		TM7OC1H1	((UB_BITS*)(&TM7OC1H))->bit1
#define		TM7OC1H0	((UB_BITS*)(&TM7OC1H))->bit0

/*
 *	TM7PR1L --- 0x03F84
 * 	Timer 7 preset register 1 (lower 8 bits)
 */
#define		TM7PR1L7	((UB_BITS*)(&TM7PR1L))->bit7
#define		TM7PR1L6	((UB_BITS*)(&TM7PR1L))->bit6
#define		TM7PR1L5	((UB_BITS*)(&TM7PR1L))->bit5
#define		TM7PR1L4	((UB_BITS*)(&TM7PR1L))->bit4
#define		TM7PR1L3	((UB_BITS*)(&TM7PR1L))->bit3
#define		TM7PR1L2	((UB_BITS*)(&TM7PR1L))->bit2
#define		TM7PR1L1	((UB_BITS*)(&TM7PR1L))->bit1
#define		TM7PR1L0	((UB_BITS*)(&TM7PR1L))->bit0

/*
 *	TM7PR1H --- 0x03F85
 * 	Timer 7 preset register 1 (upper 8 bits)
 */
#define		TM7PR1H7	((UB_BITS*)(&TM7PR1H))->bit7
#define		TM7PR1H6	((UB_BITS*)(&TM7PR1H))->bit6
#define		TM7PR1H5	((UB_BITS*)(&TM7PR1H))->bit5
#define		TM7PR1H4	((UB_BITS*)(&TM7PR1H))->bit4
#define		TM7PR1H3	((UB_BITS*)(&TM7PR1H))->bit3
#define		TM7PR1H2	((UB_BITS*)(&TM7PR1H))->bit2
#define		TM7PR1H1	((UB_BITS*)(&TM7PR1H))->bit1
#define		TM7PR1H0	((UB_BITS*)(&TM7PR1H))->bit0

/*
 *	TM7ICL --- 0x03F86
 * 	Timer 7 input capture register (lower 8 bits)
 */
#define		TM7ICL7		((UB_BITS*)(&TM7ICL))->bit7
#define		TM7ICL6		((UB_BITS*)(&TM7ICL))->bit6
#define		TM7ICL5		((UB_BITS*)(&TM7ICL))->bit5
#define		TM7ICL4		((UB_BITS*)(&TM7ICL))->bit4
#define		TM7ICL3		((UB_BITS*)(&TM7ICL))->bit3
#define		TM7ICL2		((UB_BITS*)(&TM7ICL))->bit2
#define		TM7ICL1		((UB_BITS*)(&TM7ICL))->bit1
#define		TM7ICL0		((UB_BITS*)(&TM7ICL))->bit0

/*
 *	TM7ICH --- 0x03F87
 * 	Timer 7 input capture register (upper 8 bits)
 */
#define		TM7ICH7		((UB_BITS*)(&TM7ICH))->bit7
#define		TM7ICH6		((UB_BITS*)(&TM7ICH))->bit6
#define		TM7ICH5		((UB_BITS*)(&TM7ICH))->bit5
#define		TM7ICH4		((UB_BITS*)(&TM7ICH))->bit4
#define		TM7ICH3		((UB_BITS*)(&TM7ICH))->bit3
#define		TM7ICH2		((UB_BITS*)(&TM7ICH))->bit2
#define		TM7ICH1		((UB_BITS*)(&TM7ICH))->bit1
#define		TM7ICH0		((UB_BITS*)(&TM7ICH))->bit0

/*
 *	TM7MD1 --- 0x03F88
 *	Timer 7 mode register 1
 */
#define		T7ICEDG1	((UB_BITS*)(&TM7MD1))->bit6
#define		TM7CL		((UB_BITS*)(&TM7MD1))->bit5
#define		TM7EN		((UB_BITS*)(&TM7MD1))->bit4
#define		TM7PS1		((UB_BITS*)(&TM7MD1))->bit3
#define		TM7PS0		((UB_BITS*)(&TM7MD1))->bit2
#define		TM7CK1		((UB_BITS*)(&TM7MD1))->bit1
#define		TM7CK0		((UB_BITS*)(&TM7MD1))->bit0

/*
 *	TM7MD2 --- 0x03F89
 *	Timer 7 mode register 2
 */
#define		T7ICEDG0	((UB_BITS*)(&TM7MD2))->bit7
#define		T7PWMSL		((UB_BITS*)(&TM7MD2))->bit6
#define		TM7BCR		((UB_BITS*)(&TM7MD2))->bit5
#define		TM7PWM		((UB_BITS*)(&TM7MD2))->bit4
#define		TM7IRS1		((UB_BITS*)(&TM7MD2))->bit3
#define		T7ICEN		((UB_BITS*)(&TM7MD2))->bit2
#define		T7ICT1		((UB_BITS*)(&TM7MD2))->bit1
#define		T7ICT0		((UB_BITS*)(&TM7MD2))->bit0

/*
 *	TM7OC2L --- 0x03F8A
 * 	Timer 7 compare register 2 (lower 8 bits)
 */
#define		TM7OC2L7	((UB_BITS*)(&TM7OC2L))->bit7
#define		TM7OC2L6	((UB_BITS*)(&TM7OC2L))->bit6
#define		TM7OC2L5	((UB_BITS*)(&TM7OC2L))->bit5
#define		TM7OC2L4	((UB_BITS*)(&TM7OC2L))->bit4
#define		TM7OC2L3	((UB_BITS*)(&TM7OC2L))->bit3
#define		TM7OC2L2	((UB_BITS*)(&TM7OC2L))->bit2
#define		TM7OC2L1	((UB_BITS*)(&TM7OC2L))->bit1
#define		TM7OC2L0	((UB_BITS*)(&TM7OC2L))->bit0

/*
 *	TM7OC2H --- 0x03F8B
 * 	Timer 7 compare register 2 (upper 8 bits)
 */
#define		TM7OC2H7	((UB_BITS*)(&TM7OC2H))->bit7
#define		TM7OC2H6	((UB_BITS*)(&TM7OC2H))->bit6
#define		TM7OC2H5	((UB_BITS*)(&TM7OC2H))->bit5
#define		TM7OC2H4	((UB_BITS*)(&TM7OC2H))->bit4
#define		TM7OC2H3	((UB_BITS*)(&TM7OC2H))->bit3
#define		TM7OC2H2	((UB_BITS*)(&TM7OC2H))->bit2
#define		TM7OC2H1	((UB_BITS*)(&TM7OC2H))->bit1
#define		TM7OC2H0	((UB_BITS*)(&TM7OC2H))->bit0

/*
 *	TM7PR2L --- 0x03F8C
 * 	Timer 7 preset register 2 (lower 8 bits)
 */
#define		TM7PR2L7	((UB_BITS*)(&TM7PR2L))->bit7
#define		TM7PR2L6	((UB_BITS*)(&TM7PR2L))->bit6
#define		TM7PR2L5	((UB_BITS*)(&TM7PR2L))->bit5
#define		TM7PR2L4	((UB_BITS*)(&TM7PR2L))->bit4
#define		TM7PR2L3	((UB_BITS*)(&TM7PR2L))->bit3
#define		TM7PR2L2	((UB_BITS*)(&TM7PR2L))->bit2
#define		TM7PR2L1	((UB_BITS*)(&TM7PR2L))->bit1
#define		TM7PR2L0	((UB_BITS*)(&TM7PR2L))->bit0

/*
 *	TM7PR2H --- 0x03F8D
 * 	Timer 7 preset register 2 (upper 8 bits)
 */
#define		TM7PR2H7	((UB_BITS*)(&TM7PR2H))->bit7
#define		TM7PR2H6	((UB_BITS*)(&TM7PR2H))->bit6
#define		TM7PR2H5	((UB_BITS*)(&TM7PR2H))->bit5
#define		TM7PR2H4	((UB_BITS*)(&TM7PR2H))->bit4
#define		TM7PR2H3	((UB_BITS*)(&TM7PR2H))->bit3
#define		TM7PR2H2	((UB_BITS*)(&TM7PR2H))->bit2
#define		TM7PR2H1	((UB_BITS*)(&TM7PR2H))->bit1
#define		TM7PR2H0	((UB_BITS*)(&TM7PR2H))->bit0

/*
 *	TM7DPR1 --- 0x03F8E
 *	the dead time preset register 1
 */
#define		TM7DPR17	((UB_BITS*)(&TM7DPR1))->bit7
#define		TM7DPR16	((UB_BITS*)(&TM7DPR1))->bit6
#define		TM7DPR15	((UB_BITS*)(&TM7DPR1))->bit5
#define		TM7DPR14	((UB_BITS*)(&TM7DPR1))->bit4
#define		TM7DPR13	((UB_BITS*)(&TM7DPR1))->bit3
#define		TM7DPR12	((UB_BITS*)(&TM7DPR1))->bit2
#define		TM7DPR11	((UB_BITS*)(&TM7DPR1))->bit1
#define		TM7DPR10	((UB_BITS*)(&TM7DPR1))->bit0

/*
 *	TM7DPR2 --- 0x03F8F
 *	the dead time preset register 2
 */
#define		TM7DPR27	((UB_BITS*)(&TM7DPR2))->bit7
#define		TM7DPR26	((UB_BITS*)(&TM7DPR2))->bit6
#define		TM7DPR25	((UB_BITS*)(&TM7DPR2))->bit5
#define		TM7DPR24	((UB_BITS*)(&TM7DPR2))->bit4
#define		TM7DPR23	((UB_BITS*)(&TM7DPR2))->bit3
#define		TM7DPR22	((UB_BITS*)(&TM7DPR2))->bit2
#define		TM7DPR21	((UB_BITS*)(&TM7DPR2))->bit1
#define		TM7DPR20	((UB_BITS*)(&TM7DPR2))->bit0

/*
 *	TM7MD3 --- 0x03F9E
 *	Timer 7 mode register 3
 */
#define		TM7CKSMP	((UB_BITS*)(&TM7MD3))->bit7
#define		TM7BUFSEL	((UB_BITS*)(&TM7MD3))->bit6
#define		TM7CKEDG	((UB_BITS*)(&TM7MD3))->bit5
/*
 *	TMCKSEL1 --- 0x03FB0
 * 	Timer clock selection register 1
 */
#define		TM2IOSEL	((UB_BITS*)(&TMCKSEL1))->bit2
#define		TM1IOSEL	((UB_BITS*)(&TMCKSEL1))->bit1
#define		TM0IOSEL	((UB_BITS*)(&TMCKSEL1))->bit0

/*
 *	TMCKSEL2 --- 0x03FB1
 * 	Timer clock selection register 2
 */
#define		TM7IOSEL2	((UB_BITS*)(&TMCKSEL2))->bit4
#define		TM9IOSEL	((UB_BITS*)(&TMCKSEL2))->bit2
#define		TM7IOSEL	((UB_BITS*)(&TMCKSEL2))->bit0


/*
 *	TMINSEL1 --- 0x03FB2
 * 	Timer input selection register 1
 */
#define		TMINSEL15	((UB_BITS*)(&TMINSEL1))->bit5
#define		TMINSEL14	((UB_BITS*)(&TMINSEL1))->bit4
#define		TMINSEL13	((UB_BITS*)(&TMINSEL1))->bit3
#define		TMINSEL12	((UB_BITS*)(&TMINSEL1))->bit2
#define		TMINSEL11	((UB_BITS*)(&TMINSEL1))->bit1
#define		TMINSEL10	((UB_BITS*)(&TMINSEL1))->bit0

/*
 *	TMINSEL2 --- 0x03FB3
 * 	Timer input selection register 2
 */
#define		TMINSEL27	((UB_BITS*)(&TMINSEL2))->bit7
#define		TMINSEL26	((UB_BITS*)(&TMINSEL2))->bit6
#define		TMINSEL23	((UB_BITS*)(&TMINSEL2))->bit3
#define		TMINSEL22	((UB_BITS*)(&TMINSEL2))->bit2



/*
 *	FBEWER1 --- 0x03FBD
 *	On-board writing enable
 */
#define		BEW7		((UB_BITS*)(&FBEWER1))->bit7
#define		BEW6		((UB_BITS*)(&FBEWER1))->bit6
#define		BEW5		((UB_BITS*)(&FBEWER1))->bit5
#define		BEW4		((UB_BITS*)(&FBEWER1))->bit4
#define		BEW3		((UB_BITS*)(&FBEWER1))->bit3
#define		BEW2		((UB_BITS*)(&FBEWER1))->bit2
#define		BEW1		((UB_BITS*)(&FBEWER1))->bit1
#define		BEW0		((UB_BITS*)(&FBEWER1))->bit0


/*
 *	FEWSPD --- 0x03FBF
 *	Internal flash control register
 */
#define 	NSTOP		((UB_BITS*)(&FEWSPD))->bit3


/*
 *	ANCTR0 --- 0x03FC5
 *	A/D converter control register 0
 */
#define		ANSH1		((UB_BITS*)(&ANCTR0))->bit7
#define		ANSH0		((UB_BITS*)(&ANCTR0))->bit6
#define		ANCK1		((UB_BITS*)(&ANCTR0))->bit5
#define		ANCK0		((UB_BITS*)(&ANCTR0))->bit4
#define		ANLADE		((UB_BITS*)(&ANCTR0))->bit3
#define		ANCK2		((UB_BITS*)(&ANCTR0))->bit2

/*
 *	ANCTR1 --- 0x03FC6
 *	A/D converter control register 1
 */
#define		ANCHS3		((UB_BITS*)(&ANCTR1))->bit3
#define		ANCHS2		((UB_BITS*)(&ANCTR1))->bit2
#define		ANCHS1		((UB_BITS*)(&ANCTR1))->bit1
#define		ANCHS0		((UB_BITS*)(&ANCTR1))->bit0

/*
 *	ANCTR2 --- 0x03FC7
 *  A/D converter control register 0
 */
#define		ANST		((UB_BITS*)(&ANCTR2))->bit7
#define		ANSTSEL1	((UB_BITS*)(&ANCTR2))->bit6
#define		ANSTSEL0	((UB_BITS*)(&ANCTR2))->bit5

/*
 *	ANBUF0 --- 0x03FC8
 *	A/D converter data storage buffer 0
 */
#define		ANBUF07		((UB_BITS*)(&ANBUF0))->bit7
#define		ANBUF06		((UB_BITS*)(&ANBUF0))->bit6

/*
 *	ANBUF1 --- 0x03FC9
 *	A/D converter data storage buffer 1
 */
#define		ANBUF17		((UB_BITS*)(&ANBUF1))->bit7
#define		ANBUF16		((UB_BITS*)(&ANBUF1))->bit6
#define		ANBUF15		((UB_BITS*)(&ANBUF1))->bit5
#define		ANBUF14		((UB_BITS*)(&ANBUF1))->bit4
#define		ANBUF13		((UB_BITS*)(&ANBUF1))->bit3
#define		ANBUF12		((UB_BITS*)(&ANBUF1))->bit2
#define		ANBUF11		((UB_BITS*)(&ANBUF1))->bit1
#define		ANBUF10		((UB_BITS*)(&ANBUF1))->bit0

/*
 *	IRQCNT --- 0x03FD0
 *	External interrupt pin setting register
 */
#define		P24EN		((UB_BITS*)(&IRQCNT))->bit4
#define		P23EN		((UB_BITS*)(&IRQCNT))->bit3
#define		P22EN		((UB_BITS*)(&IRQCNT))->bit2
#define		P21EN		((UB_BITS*)(&IRQCNT))->bit1
#define		P20EN		((UB_BITS*)(&IRQCNT))->bit0

/*
 *	NF0CTR --- 0x03FD1
 * 	Noise filter 0 control register
 */
#define		NF0SCK2		((UB_BITS*)(&NF0CTR))->bit7
#define		NF0SCK1		((UB_BITS*)(&NF0CTR))->bit6
#define		NF0SCK0		((UB_BITS*)(&NF0CTR))->bit5
#define		NF0EN1		((UB_BITS*)(&NF0CTR))->bit4

/*
 *	NF1CTR --- 0x03FD2
 * 	Noise filter 1 control register
 */
#define		NF1SCK2		((UB_BITS*)(&NF1CTR))->bit7
#define		NF1SCK1		((UB_BITS*)(&NF1CTR))->bit6
#define		NF1SCK0		((UB_BITS*)(&NF1CTR))->bit5
#define		NF1EN1		((UB_BITS*)(&NF1CTR))->bit4

/*
 *	NF2CTR --- 0x03FD3
 * 	Noise filter 2 control register
 */
#define		NF2SCK2		((UB_BITS*)(&NF2CTR))->bit7
#define		NF2SCK1		((UB_BITS*)(&NF2CTR))->bit6
#define		NF2SCK0		((UB_BITS*)(&NF2CTR))->bit5
#define		NF2EN1		((UB_BITS*)(&NF2CTR))->bit4

/*
 *	NF3CTR --- 0x03FD4
 * 	Noise filter 3 control register
 */
#define		NF3SCK2		((UB_BITS*)(&NF3CTR))->bit7
#define		NF3SCK1		((UB_BITS*)(&NF3CTR))->bit6
#define		NF3SCK0		((UB_BITS*)(&NF3CTR))->bit5
#define		NF3EN1		((UB_BITS*)(&NF3CTR))->bit4

/*
 *	NF4CTR --- 0x03FD5
 * 	Noise filter 4 control register
 */
#define		NF4SCK2		((UB_BITS*)(&NF4CTR))->bit7
#define		NF4SCK1		((UB_BITS*)(&NF4CTR))->bit6
#define		NF4SCK0		((UB_BITS*)(&NF4CTR))->bit5
#define		NF4EN1		((UB_BITS*)(&NF4CTR))->bit4

/*
 *	NF5CTR --- 0x03FD6
 * 	Noise filter 5 control register
 */
#define		NF5SCK2		((UB_BITS*)(&NF5CTR))->bit7
#define		NF5SCK1		((UB_BITS*)(&NF5CTR))->bit6
#define		NF5SCK0		((UB_BITS*)(&NF5CTR))->bit5
#define		NF5EN1		((UB_BITS*)(&NF5CTR))->bit4

/*
 *	LVLMD --- 0x03FD7
 *	External interrupt valid input switch control register
 */
#define		EXLVL4		((UB_BITS*)(&LVLMD))->bit5
#define		LVLEN4		((UB_BITS*)(&LVLMD))->bit4
#define		EXLVL3		((UB_BITS*)(&LVLMD))->bit3
#define		LVLEN3		((UB_BITS*)(&LVLMD))->bit2
#define		EXLVL2		((UB_BITS*)(&LVLMD))->bit1
#define		LVLEN2		((UB_BITS*)(&LVLMD))->bit0

/*
 *	NMICR --- 0x03FE1
 *	Non - maskable interrupt control register
 */
#define		IRQNPG		((UB_BITS*)(&NMICR))->bit2
#define		IRQNWDG		((UB_BITS*)(&NMICR))->bit1
#define		IRQNPRI		((UB_BITS*)(&NMICR))->bit0

/*
 *	IRQ0ICR --- 0x03FE2
 *	External interrupt 0 control register
 */
#define		IRQ0LV1		((UB_BITS*)(&IRQ0ICR))->bit7
#define		IRQ0LV0		((UB_BITS*)(&IRQ0ICR))->bit6
#define		REDG0		((UB_BITS*)(&IRQ0ICR))->bit5
#define		IRQ0IE		((UB_BITS*)(&IRQ0ICR))->bit1
#define		IRQ0IR		((UB_BITS*)(&IRQ0ICR))->bit0

/*
 *	IRQ1ICR --- 0x03FE3
 *	External interrupt 1 control register
 */
#define		IRQ1LV1		((UB_BITS*)(&IRQ1ICR))->bit7
#define		IRQ1LV0		((UB_BITS*)(&IRQ1ICR))->bit6
#define		REDG1		((UB_BITS*)(&IRQ1ICR))->bit5
#define		IRQ1IE		((UB_BITS*)(&IRQ1ICR))->bit1
#define		IRQ1IR		((UB_BITS*)(&IRQ1ICR))->bit0

/*
 *	IRQ2ICR --- 0x03FE4
 *	External interrupt 2 control register
 */
#define		IRQ2LV1		((UB_BITS*)(&IRQ2ICR))->bit7
#define		IRQ2LV0		((UB_BITS*)(&IRQ2ICR))->bit6
#define		REDG2		((UB_BITS*)(&IRQ2ICR))->bit5
#define		IRQ2IE		((UB_BITS*)(&IRQ2ICR))->bit1
#define		IRQ2IR		((UB_BITS*)(&IRQ2ICR))->bit0

/*
 *	IRQ3ICR --- 0x03FE5
 *	External interrupt 3 control register
 */
#define		IRQ3LV1		((UB_BITS*)(&IRQ3ICR))->bit7
#define		IRQ3LV0		((UB_BITS*)(&IRQ3ICR))->bit6
#define		REDG3		((UB_BITS*)(&IRQ3ICR))->bit5
#define		IRQ3IE		((UB_BITS*)(&IRQ3ICR))->bit1
#define		IRQ3IR		((UB_BITS*)(&IRQ3ICR))->bit0

/*
 *	IRQ4ICR --- 0x03FE6
 *	External interrupt 4 control register
 */
#define		IRQ4LV1		((UB_BITS*)(&IRQ4ICR))->bit7
#define		IRQ4LV0		((UB_BITS*)(&IRQ4ICR))->bit6
#define		REDG4		((UB_BITS*)(&IRQ4ICR))->bit5
#define		IRQ4IE		((UB_BITS*)(&IRQ4ICR))->bit1
#define		IRQ4IR		((UB_BITS*)(&IRQ4ICR))->bit0

/*
 *	LIN1ICR --- 0x03FE7
 *	LIN interrupt control register
 */
#define		LINLV1		((UB_BITS*)(&LINICR))->bit7
#define		LINLV0		((UB_BITS*)(&LINICR))->bit6
#define		LINREDG		((UB_BITS*)(&LINICR))->bit5
#define		LINIE		((UB_BITS*)(&LINICR))->bit1
#define		LINIR		((UB_BITS*)(&LINICR))->bit0


/*
 *	LVIICR --- 0x03FE8
 *	Power supply detection interrupt control register
 */
#define		LVILV1		((UB_BITS*)(&LVIICR))->bit7
#define		LVILV0		((UB_BITS*)(&LVIICR))->bit6
#define		LVIIE		((UB_BITS*)(&LVIICR))->bit1
#define		LVIIR		((UB_BITS*)(&LVIICR))->bit0

/*
 *	TM0ICR --- 0x03FE9
 *	Timer 0 interrupt control register
 */
#define		TM0LV1		((UB_BITS*)(&TM0ICR))->bit7
#define		TM0LV0		((UB_BITS*)(&TM0ICR))->bit6
#define		TM0IE		((UB_BITS*)(&TM0ICR))->bit1
#define		TM0IR		((UB_BITS*)(&TM0ICR))->bit0

/*
 *	TM1ICR --- 0x03FEA
 *	Timer 1 interrupt control register
 */
#define		TM1LV1		((UB_BITS*)(&TM1ICR))->bit7
#define		TM1LV0		((UB_BITS*)(&TM1ICR))->bit6
#define		TM1IE		((UB_BITS*)(&TM1ICR))->bit1
#define		TM1IR		((UB_BITS*)(&TM1ICR))->bit0

/*
 *	TM2ICR --- 0x03FEB
 *	Timer 2 interrupt control register
 */
#define		TM2LV1		((UB_BITS*)(&TM2ICR))->bit7
#define		TM2LV0		((UB_BITS*)(&TM2ICR))->bit6
#define		TM2IE		((UB_BITS*)(&TM2ICR))->bit1
#define		TM2IR		((UB_BITS*)(&TM2ICR))->bit0

/*
 *	IE1ICR --- 0x03FEC
 *	Timer 3 interrupt control register
 */
#define		IE1LV1		((UB_BITS*)(&IE1ICR))->bit7
#define		IE1LV0		((UB_BITS*)(&IE1ICR))->bit6
#define		IE1IE		((UB_BITS*)(&IE1ICR))->bit1
#define		IE1IR		((UB_BITS*)(&IE1ICR))->bit0

/*
 *	IE2ICR --- 0x03FED
 *	Timer 4 interrupt control register
 */
#define		IE2LV1		((UB_BITS*)(&IE2ICR))->bit7
#define		IE2LV0		((UB_BITS*)(&IE2ICR))->bit6
#define		IE2IE		((UB_BITS*)(&IE2ICR))->bit1
#define		IE2IR		((UB_BITS*)(&IE2ICR))->bit0

/*
 *	TM6ICR --- 0x03FEE
 *	Timer 6 interrupt control register
 */
#define		TM6LV1		((UB_BITS*)(&TM6ICR))->bit7
#define		TM6LV0		((UB_BITS*)(&TM6ICR))->bit6
#define		TM6IE		((UB_BITS*)(&TM6ICR))->bit1
#define		TM6IR		((UB_BITS*)(&TM6ICR))->bit0

/*
 *	TBICR --- 0x03FEF
 *	Time base interrupt control register
 */
#define		TBLV1		((UB_BITS*)(&TBICR))->bit7
#define		TBLV0		((UB_BITS*)(&TBICR))->bit6
#define		TBIE		((UB_BITS*)(&TBICR))->bit1
#define		TBIR		((UB_BITS*)(&TBICR))->bit0

/*
 *	TM7ICR --- 0x03FF0
 *	Timer 7 interrupt control register
 */
#define		TM7LV1		((UB_BITS*)(&TM7ICR))->bit7
#define		TM7LV0		((UB_BITS*)(&TM7ICR))->bit6
#define		TM7IE		((UB_BITS*)(&TM7ICR))->bit1
#define		TM7IR		((UB_BITS*)(&TM7ICR))->bit0

/*
 *	T7OC2ICR --- 0x03FF1
 *	Timer 7 compare 2-match interrupt control register
 */
#define		TM7OC2LV1	((UB_BITS*)(&T7OC2ICR))->bit7
#define		TM7OC2LV0	((UB_BITS*)(&T7OC2ICR))->bit6
#define		TM7OC2IE	((UB_BITS*)(&T7OC2ICR))->bit1
#define		TM7OC2IR	((UB_BITS*)(&T7OC2ICR))->bit0


/*
 *	PWMOVICR --- 0x03FF4
 *	Timer 9 overflow interrupt control register
 */
#define		PWMOVLV1	((UB_BITS*)(&PWMOVICR))->bit7
#define		PWMOVLV0	((UB_BITS*)(&PWMOVICR))->bit6
#define		PWMOVIE		((UB_BITS*)(&PWMOVICR))->bit1
#define		PWMOVIR		((UB_BITS*)(&PWMOVICR))->bit0

/*
 *	PWMUDICR --- 0x03FF5
 *	Timer 9 underflow interrupt control register
 */
#define		PWMUDLV1	((UB_BITS*)(&PWMUDICR))->bit7
#define		PWMUDLV0	((UB_BITS*)(&PWMUDICR))->bit6
#define		PWMUDIE		((UB_BITS*)(&PWMUDICR))->bit1
#define		PWMUDIR		((UB_BITS*)(&PWMUDICR))->bit0

/*
 *	T9OC2ICR --- 0x03FF6
 *	Timer 9 TCMPA compare match interrupt
 */
#define		TM9OC2LV1	((UB_BITS*)(&TM9OC2ICR))->bit7
#define		TM9OC2LV0	((UB_BITS*)(&TM9OC2ICR))->bit6
#define		TM9OC2IE	((UB_BITS*)(&TM9OC2ICR))->bit1
#define		TM9OC2IR	((UB_BITS*)(&TM9OC2ICR))->bit0

/*
 *	SC0TICR --- 0x03FF7
 *	Serial 0 transmission interrupt control register
 */
#define		SC0TLV1		((UB_BITS*)(&SC0TICR))->bit7
#define		SC0TLV0		((UB_BITS*)(&SC0TICR))->bit6
#define		SC0TIE		((UB_BITS*)(&SC0TICR))->bit1
#define		SC0TIR		((UB_BITS*)(&SC0TICR))->bit0

/*
 *	SC0RICR --- 0x03FF9
 *	Serial 1 transmission interrupt control register
 */
#define		SC0RLV1		((UB_BITS*)(&SC0RICR))->bit7
#define		SC0RLV0		((UB_BITS*)(&SC0RICR))->bit6
#define		SC0RIE		((UB_BITS*)(&SC0RICR))->bit1
#define		SC0RIR		((UB_BITS*)(&SC0RICR))->bit0

/*
 *	SC4STPCICR --- 0x03FFB
 *	Serial 4 stop condition interrupt control register
 */
#define		SC4STPCLV1		((UB_BITS*)(&SC4STPCICR))->bit7
#define		SC4STPCLV0		((UB_BITS*)(&SC4STPCICR))->bit6
#define		SC4STPCIE		((UB_BITS*)(&SC4STPCICR))->bit1
#define		SC4STPCIR		((UB_BITS*)(&SC4STPCICR))->bit0

/*
 *	SC4ICR --- 0x03FFC
 *	Serial 4 interrupt control register
 */
#define		SC4LV1		((UB_BITS*)(&SC4ICR))->bit7
#define		SC4LV0		((UB_BITS*)(&SC4ICR))->bit6
#define		SC4IE		((UB_BITS*)(&SC4ICR))->bit1
#define		SC4IR		((UB_BITS*)(&SC4ICR))->bit0

/*
 *	adicr --- 0x03FFD
 *	A/D conversion interrupt control register
 */
#define		ADLV1		((UB_BITS*)(&ADICR))->bit7
#define		ADLV0		((UB_BITS*)(&ADICR))->bit6
#define		ADIE		((UB_BITS*)(&ADICR))->bit1
#define		ADIR		((UB_BITS*)(&ADICR))->bit0


#endif /* _LED_TYPE_DEF_H */
