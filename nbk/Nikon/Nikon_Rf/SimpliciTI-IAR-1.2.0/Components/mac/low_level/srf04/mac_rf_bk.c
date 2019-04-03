#include "mac_api.h"
#include "mac_rf.h"
#include "mac_radio_defs.h"
#include "mac_mem.h"
#include "comdef.h"
#include "osal.h"
#include "hal_types.h"
#include "AF.h"
#include "mac_low_level.h"
#include "mac_rx_onoff.h"
#include "hal_uart.h"
#include "mac_rx.h"
#include <stdio.h>

#ifdef IO_INDEX_TEST
#include "ioCC2531.h"
#endif

extern void osal_sleep_timers(void);
extern void halSleep( uint16 osal_timeout );
extern void macTxDoneCallback(void);
//-C $PROJ_DIR$\..\..\..\Libraries\TI2530DB\bin\Security.lib
//-f $PROJ_DIR$\..\..\..\Tools\CC2530DB\f8wConfig.cfg
#define MODELING_STYLE_APERT 	0
#define MODELING_STYLE_STEX	1
#define MODELING_STYLE_BOTH	2
#define MODELING_STYLE_OFF		3

#define	MAX_UART_DATA_LEN	64

#define DEVICE_ATTRIBUTE_SLAVE		0
#define DEVICE_ATTRIBUTE_MAST 		1
#define DEVICE_ATTRIBUTE_LINKSHOT	2

#define TI_EXTADDR  PXREG( 0x780C )  /* Pointer to Start of Flash Information Page          */
#define MACEX_READ_IEEE_ADDR(p)                macMemReadRam((macRam_t *) &TI_EXTADDR, p, 8)
static uint8 txMpdu[BASIC_RF_MAX_PAYLOAD_SIZE+BASIC_RF_PACKET_OVERHEAD_SIZE+1];
static uint8 g_extAddr[8] ;//= {0x38,0x15,0xC6,0xFE,0xFF,0x17,0x87,0x88};
uint8 g_modelingStyle = 1;
//static uint8 pilotCommand[] = {0x17,0x00,0xFF,0x47,0x00,0x00,0x48,0x0A,0x06,0x98,0x07,0x06,0x98,0x07,0x06,0x98,0x07,0x06,0x98,0x07,0x06,0x98,0x07};
uint8 preCmdBuf[] = 	{0x17,0x00,0xFF,0x47,0x75,0x30,0x60,0x04,0x01,0xA0,0x04,0x01,0xA0,0x04,0x01,0xA0,0x04,0x01,0x00,0x00,0x01,0x00,0x00};
uint8 flashCmd[] = 	{0x17,0x00,0xFF,0x47,0x75,0x30,0x60,0x04,0x01,0x9E,0x03,0x01,0x9E,0x03,0x01,0x9E,0x03,0x01,0x00,0x00,0x01,0x00,0x00};
uint8 multiCmd[] = {0x0F,0x00,0xFF,0x48};
unsigned char rcvBuf[UART_RCVBUF_LEN];
basicRfCfg_t* pConfig;
static basicRfCfg_t basicRfConfig;
static basicRfTxState_t txState=  { 0x00 }; // initialised and distinct
uint8 macEx_taskId;
uint8 g_quireFlag=0;
//uint8 g_linkedMode = 0;
//uint8 g_isMaster = CMD_MODE_RF_MST;
//uint8 g_isMaster = MSTDEV_MODE_DEV;
uint8 g_isMaster = CMD_MODE_OFF;
//uint8 g_isMaster = CMD_MODE_RFLS_SLA;
uint8 g_mstHold = 0;				
//DeviceInfo deviceInfo[128];
basicRfRxInfo_t  rxi={ 0xFF };
uint8 g_sendLen = 0;
//static uint8 g_netCheckFlag=0;
DeviceInfo * pCurrentDev=NULL;
int16 g_msTick=0;
uint8 g_slaLinkSta=0;
static int8 s_rxFlag=0;
static int8 s_ready=0; 
uint8 g_netStart=0;
uint8 g_expireCounter = 0;
uint8 g_initCheck=0;
uint8 g_swMode=0;
uint8 g_swRfChan=0;
uint16 g_swRfId=0;
uint8 g_grInfo = 0;
IpReserve ipRsv={0x0E,0x00};

unsigned char gBUartTotalByte=0;
unsigned char   gBUartLen=0;
unsigned char	gBUartBuf[MAX_UART_DATA_LEN];
unsigned char uart_tran_flag=0;

//static uint8 pRxData[APP_PAYLOAD_LENGTH];
static uint8 pTxData[APP_PAYLOAD_LENGTH]={0};
//void (*veriPckSendCallback)();
static void    (* veriPckSendCallback)(macRx_t*);
macRx_t  *callbackPara;
DevCheck g_devCheck={0};
RadioSet radioSet={0};

#ifdef DEBUG_TRACE
macTx_t macDataTx;
#endif

#ifdef SEND_OBEY_ZSTACK
macTx_t macDataTx;
#endif

MAC_DATAREQ_API static void macRollPolingProc(macRx_t* pck);
#ifndef SEND_UNIFIED
MAC_DATAREQ_API static void VeriResponse(macRx_t* pck);
MAC_DATAREQ_API static void LinkedModeShortAddrAlloc(macRx_t* pck);
MAC_DATAREQ_API static void LinkedModeRollPoling(macRx_t* pck);
#endif
MAC_DATAREQ_API void PreFlashInfo(macRx_t* pck);
MAC_DATAREQ_API void CountDown(macRx_t* pck);
MAC_DATAREQ_API void FlashConfirm(macRx_t* pck);
MAC_DATAREQ_API void InqureConfirm(macRx_t* pck);
MAC_DATAREQ_API void ReConfirm(macRx_t* pck);
MAC_DATAREQ_API void ReadyInfo(macRx_t* pck);
MAC_DATAREQ_API void Pilot(macRx_t* pck);
MAC_DATAREQ_API void LinkedShotRel(macRx_t* pck);
MAC_INTERNAL_API void SetSleepMode(macRx_t *pMsg);
MAC_DATAREQ_API void ChanScan(macRx_t* pck);
MAC_DATAREQ_API void RflsStart(macRx_t* pck);
MAC_DATAREQ_API static void VeriAndAllocID(macRx_t* pck);
MAC_DATAREQ_API void CmdModeSwitch(macRx_t* pck);
MAC_DATAREQ_API void CmdModeSwitchEx(macRx_t* pck);
MAC_DATAREQ_API static void RfMstExit(macRx_t* pck);
MAC_DATAREQ_API static void ExitAndSetChan_exit(macRx_t* pck);
MAC_DATAREQ_API static void ExitAndSetChan_setCh(macRx_t* pck);
MAC_DATAREQ_API static void ExitAndSetRfId(macRx_t* pck);
MAC_DATAREQ_API void MstBridgeInfoInqure(macRx_t* pck);
MAC_DATAREQ_API static void BridgedMstReadyRes(macRx_t* pck);
LINKMODE_DEFINE void linkedNoneCoordinatorProc(macRx_t *payload);
void BuildPilotInfoData(uint8 * buf, uint8 * len);
static uint8 UartSctSend(unsigned char id, unsigned char pa1, unsigned char pa2);
static uint16 NoneUsedShortAddrSearch(uint8 attr);
static uint8 LoadUartSendBuf(unsigned char id, unsigned char pa1, unsigned char pa2);
static void TrigerUartSend(void);

static void macExRxEnable(void);
static void macExRxDisable(void);
static void CalcEachRssi(int8 *valBuf, uint16 tms, uint16 dly);

static void macExUartInit (void);
static void macExUartRcvHandle(uint8 *rcv, uint8 len);
static void RfModCheck(uint8 mod);

extern void macWaitSendOk(void);

#ifdef DEBUG_TRACE
void DeviceInfoPrint(void);
void BufferPrint(uint8* buf, uint16 size);
int Printf(const char *fmt, ...);
#endif

//CurrentVeriDev currentVeriDev;


static void halRfReceiveOn(void)
{
	MAC_ISFLUSHRX();     // Making sure that the TX FIFO is empty.
	MAC_ISRXON();
}

static DeviceInfo* SearchForDevice(uint16 shortAddr)
{
	DeviceInfo *pDev;
	for(pDev=pConfig->devHead;pDev!=NULL;pDev=pDev->pNext)
	{
		if(pDev->shortAddr==shortAddr)
		{
			return pDev;
		}
	}
	return NULL;
}

static uint8 RemoveCurrent(void)
{
	DeviceInfo *tmpDev;
	tmpDev = pCurrentDev;
	pCurrentDev = pCurrentDev->pFront;
	osal_mem_free(tmpDev);
	
}

static uint8 RemoveDevice(uint16 shortAddr)
{
	DeviceInfo *nowDev;
	DeviceInfo *preDev;
	uint8 rtAddr;
	nowDev = preDev = pConfig->devHead;
#ifdef DEBUG_TRACE
	Printf("RmDev: %x\r\n", shortAddr);
#endif
	while(nowDev!=NULL)
	{
		if(nowDev->shortAddr==shortAddr)
		{
			if(pConfig->totalDev>0)
			{
				pConfig->totalDev--;
			}
			if(DEVICE_ATTRIBUTE_MAST==nowDev->atribute)
			{
				if(pConfig->mstRcd>0)
				{
					pConfig->mstRcd--;
				}
				if(shortAddr>ipRsv.mst)
				{
					ipRsv.mst= shortAddr;
				}
				//return (0x0E-nowDev->shortAddr);
			}else
			{
				//if(pConfig->arrCnt[nowDev->group]>0)
				//{
				//	pConfig->arrCnt[nowDev->group]--;
				//}
#ifdef DEBUG_TRACE
				Printf("remove a device\r\n");
#endif
				if(pConfig->arrCnt[pConfig->idInGroup[shortAddr]]>0)
				{
					pConfig->arrCnt[pConfig->idInGroup[shortAddr]]--;
				}
				UartSctSend(UART_CC_DEV,pConfig->idInGroup[shortAddr],pConfig->arrCnt[pConfig->idInGroup[shortAddr]]);
				pConfig->idInGroup[shortAddr]=0;
				//pConfig->isIdOccupied[shortAddr]=0;
#ifdef DEBUG_TRACE
				Printf("slave %d\r\n",pConfig->slvRcd);
#endif
				if(pConfig->slvRcd>0)
				{
					pConfig->slvRcd--;
				}
				if(shortAddr<ipRsv.sla)
				{
					ipRsv.sla = shortAddr;
				}
				//return nowDev->shortAddr;
			}
			if(nowDev==pCurrentDev)
			{
#ifdef DEBUG_TRACE
				Printf("remove current\r\n");
#endif
				if(nowDev==pConfig->devHead)
				{
#ifdef DEBUG_TRACE
					Printf("remove head\r\n");
					Printf("nowDev: %x\r\n",nowDev);
#endif
					pCurrentDev = NULL;
				}else
				{
#ifdef DEBUG_TRACE
					Printf("pCurrentDev: %x\r\n",pCurrentDev);
					Printf("nowDev->pFront: %x\r\n",nowDev->pFront);
#endif
					pCurrentDev = nowDev->pFront;
				}
			}
			if(nowDev==pConfig->devHead)
			{
				//pConfig->devHead = NULL;
#ifdef DEBUG_TRACE
				Printf("devHead->pNext: %x\r\n",pConfig->devHead->pNext);
				Printf("devHead->pNext->pNext: %x\r\n",pConfig->devHead->pNext->pNext);
				Printf("remove: %x\r\n",nowDev);
#endif
				pConfig->devHead = pConfig->devHead->pNext;
				//pCurrentDev = pConfig->devHead;
				rtAddr = nowDev->shortAddr;
				osal_mem_free(nowDev);
				
			}else
			{
#ifdef DEBUG_TRACE
				Printf("remove: %x\r\n",nowDev);
#endif
				preDev->pNext = nowDev ->pNext;
				nowDev->pNext->pFront = preDev;
				rtAddr = nowDev->shortAddr;
				osal_mem_free(nowDev);
			}
			
			pConfig->isIdOccupied[shortAddr]=0;
			
			if(pConfig->totalDev<=0)
			{
				LED_INS_UNLINK();
			}

#ifdef DEBUG_TRACE
			Printf("slvRcd: %d\r\n",pConfig->slvRcd);
			Printf("mstRcd: %d\r\n",pConfig->mstRcd);
			Printf("totalDev: %d\r\n; ",pConfig->totalDev);
			Printf("arrCnt: ");
			BufferPrint(pConfig->arrCnt, 5);
			Printf("idInGroup: ");
			BufferPrint(pConfig->idInGroup, 16);
			DeviceInfoPrint();
#endif

			return rtAddr;
			
		}
		preDev = nowDev;
		nowDev = preDev->pNext;
	}
	return 0x0;
}

static uint16 NoneUsedShortAddrSearch(uint8 attr)
{
#if 0
	uint16 st=0;
	DeviceInfo *nowDev;
	DeviceInfo *preDev;
	nowDev = preDev = pConfig->devHead;
	st = (DEVICE_ATTRIBUTE_MAST==attr)? 0x0E:0;
	while(st<16)
	{
		while(nowDev!=NULL)
		{
			if(st==nowDev->shortAddr)
			{
				st=(DEVICE_ATTRIBUTE_MAST==attr)?(st-1):(st+1);
				nowDev = pConfig->devHead;
			}else
			{
				preDev = nowDev;
				nowDev = preDev->pNext;
			}
		}
		return st;
	}
        return 0xFF;
#endif
	uint16 rtAddr=0;
	if(DEVICE_ATTRIBUTE_MAST==attr)
	{
		rtAddr = ipRsv.mst;
		ipRsv.mst--;
		while(ipRsv.mst>0)
		{
			if(!SearchForDevice(ipRsv.mst))
			{
				//HAL_ASSERT(ipRsv.mst>ipRsv.sla);
				pConfig->isIdOccupied[rtAddr] = 1;
				return rtAddr;
			}
			ipRsv.mst--;
		}
	}else
	{
		rtAddr = ipRsv.sla;
		ipRsv.sla++;
		while(ipRsv.sla<0x0E)
		{
			if(!SearchForDevice(ipRsv.sla))
			{
				//HAL_ASSERT(ipRsv.mst>ipRsv.sla);
				pConfig->isIdOccupied[rtAddr] = 1;
				return rtAddr;
			}
			ipRsv.sla++;
		}
	}

	HAL_ASSERT(0);
	return rtAddr;
}



//void halRfWaitTransceiverReady()
//{
//	while(MAC_RADIO_TRANS_NOREADY);
//}

static void macRfReceiveOn(void)
{
    txState.receiveOn = TRUE;
    halRfReceiveOn();
}

#if 0
static void macRvsBuf(uint8* pBuf, uint8 length)
{
	uint8 temp;
	uint8* pBufLast = (pBuf + length - 1);

	while(pBufLast > pBuf)
	{
		temp = *pBuf;
		*pBuf++ = *pBufLast;
		*pBufLast-- = temp;	
	}
}
#endif

static uint8 basicRfBuildHeader(uint8* buffer, uint16 destAddr, uint8 payloadLength)
{
    basicRfPktHdr_t *pHdr;
    uint16 fcf;

    pHdr= (basicRfPktHdr_t*)buffer;

    // Populate packet header
#ifndef SEND_OBEY_ZSTACK
    pHdr->packetLength = payloadLength + BASIC_RF_PACKET_OVERHEAD_SIZE;
#endif
    //pHdr->frameControlField = pConfig->ackRequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;
   // fcf= pConfig->ackRequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;
    fcf = pConfig->frameHead;
    pHdr->fcf0 = LO_UINT16(fcf);
    pHdr->fcf1 = HI_UINT16(fcf);
    pHdr->seqNumber= txState.txSeqNumber;
    pHdr->panId= pConfig->panId;
    pHdr->destAddr= destAddr;
    pHdr->srcAddr= pConfig->myAddr;

#ifdef SECURITY_CCM

    // Add security to FCF, length and security header
    pHdr->fcf0 |= BASIC_RF_SEC_ENABLED_FCF_BM_L;
    pHdr->packetLength += PKT_LEN_MIC;
    pHdr->packetLength += BASIC_RF_AUX_HDR_LENGTH;

    pHdr->securityControl= SECURITY_CONTROL;
    pHdr->frameCounter[0]=   LO_UINT16(LO_UINT32(txState.frameCounter));
    pHdr->frameCounter[1]=   HI_UINT16(LO_UINT32(txState.frameCounter));
    pHdr->frameCounter[2]=   LO_UINT16(HI_UINT32(txState.frameCounter));
    pHdr->frameCounter[3]=   HI_UINT16(HI_UINT32(txState.frameCounter));
	
#endif

    // Make sure bytefields are network byte order
    //UINT16_HTON(pHdr->panId);
   // UINT16_HTON(pHdr->destAddr);
    //UINT16_HTON(pHdr->srcAddr);
#ifdef SEND_OBEY_ZSTACK
    return BASIC_RF_HDR_SIZE-1;
#else
    return BASIC_RF_HDR_SIZE;
#endif
}

static uint8 LongAddrBuildHeader(uint8* buffer, uint8 *extAddr, uint8 payloadLength)
{
	extRfPktHdr_t *pHdr;
	uint16 fcf;

	pHdr= (extRfPktHdr_t*)buffer;

	// Populate packet header
#ifndef SEND_OBEY_ZSTACK
	pHdr->packetLength = payloadLength + 19;
#endif
	//pHdr->frameControlField = pConfig->ackRequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;
	//fcf= pConfig->ackRequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;
	fcf = pConfig->frameHead;
	pHdr->fcf0 = LO_UINT16(fcf);
	pHdr->fcf1 = HI_UINT16(fcf);
	pHdr->seqNumber= txState.txSeqNumber;
	pHdr->panId= 0xFFFF;
    	osal_memcpy(pHdr->extAddr,extAddr,8);
    	//for(i=0;i<8;i++) pHdr->extAddr[i] = pConfig->extAddr[i];
    	pHdr->srcPan= pConfig->panId;
	pHdr->srcAddr = pConfig->myAddr;

#ifdef SEND_OBEY_ZSTACK
       return (2+1+2+8+2+2);
#else
	return (2+1+2+8+2+2+1);
#endif
}

static void macWriteTxBuf(uint8* pData, uint8 length)
{
	MAC_ISFLUSHTX();
	MAC_CLEAR_TX_INT();
	macMemWriteTxFifo(pData,length);	
}

static uint8 macBuildMpdu(uint16 destAddr, uint8* pPayload, uint8 payloadLength,uint8 *extAddr,uint8 buMode)
{
	uint8 hdrLength, n;
	
	if(0==buMode)
	{
		hdrLength = basicRfBuildHeader(txMpdu, destAddr, payloadLength);
	}else
	{
		hdrLength = LongAddrBuildHeader(txMpdu, extAddr,payloadLength);
	}

	for(n=0;n<payloadLength;n++)
	{
		txMpdu[hdrLength+n] = pPayload[n];
	}
	return hdrLength + payloadLength; // total mpdu length
}

static uint8 halRfTransmit(void)
{
	uint8 status;

	MAC_ISTXON(); // Sending

	// Waiting for transmission to finish
	txState.txDone = TXSTATE_TXDONE_PENDING;
#ifndef RF_SEND_INT
	while(!(RFIRQF1 & IRQ_TXDONE) );
	RFIRQF1 = ~IRQ_TXDONE;
#endif

	status= SUCCESS;

	return status;
}

uint8 macRfSendPacket(uint16 destAddr, uint8* pPayload, uint8 length,uint8 addrMod)
{
	uint8 mpduLength;
	uint8 status;


	// Turn on receiver if its not on
/*
	if(!txState.receiveOn) 
	{
		halRfReceiveOn();
	}
*/

	if(TXSTATE_TXDONE_PENDING==txState.txDone)
	{
		//while(!(RFIRQF1 & IRQ_TXDONE) );					//wait till the previous send completed
		txState.txDone = TXSTATE_TXDONE_CLEAR;
		//return FAILURE;
	}

#ifdef _RT_AP_

	AP_TXEN = 1;

#endif

	// Check packet length
	length = min(length, BASIC_RF_MAX_PAYLOAD_SIZE);

	// Wait until the transceiver is idle
	halRfWaitTransceiverReady();

	// Turn off RX frame done interrupt to avoid interference on the SPI interface
	macDisableRxInterrupt();

	//mpduLength = macBuildMpdu(destAddr, pPayload, length,pConfig->devHead[pConfig->pCurrentDev].extAddr,addrMod);
	mpduLength = macBuildMpdu(destAddr, pPayload, length,pCurrentDev->extAddr,addrMod);


#ifdef SECURITY_CCM
	halRfWriteTxBufSecure(txMpdu, mpduLength, length, BASIC_RF_LEN_AUTH, BASIC_RF_SECURITY_M);
	txState.frameCounter++;     // Increment frame counter field
#else

#ifdef SEND_OBEY_ZSTACK
	pMacDataTx = &macDataTx;
	pMacDataTx->msdu.p = txMpdu;
	pMacDataTx->msdu.len = mpduLength;
	//macTxFrame(MAC_TX_TYPE_SLOTTED_CSMA);
	macTxFrame(MAC_TX_TYPE_UNSLOTTED_CSMA);
	//macWaitSendOk();
#else
	macWriteTxBuf(txMpdu, mpduLength);
#endif
	//pMacDataTx->msdu.p = txMpdu;
	//pMacDataTx->msdu.len = mpduLength;
	//macTxFrame(MAC_TX_TYPE_SLOTTED_CSMA);
#endif


	// Turn on RX frame done interrupt for ACK reception
	macEnableRxInterrupt();

	//FINGER2 = 0;
	// Send frame with CCA. return FAILED if not successful
	//its takes the most time
#ifdef SEND_OBEY_ZSTACK
	//macTxDoneCallback();
	status = SUCCESS;
#else
	if(halRfTransmit() != SUCCESS) 
	{
		status = FAILURE;
	}else
	{
		status = SUCCESS;
	}
#endif
	//FINGER2 = 1;

	// Wait for the acknowledge to be received, if any
#if 0					//skip waitting for ack for debug
	if (pConfig->ackRequest) {
	txState.ackReceived = FALSE;

	// We'll enter RX automatically, so just wait until we can be sure that the ack reception should have finished
	// The timeout consists of a 12-symbol turnaround time, the ack packet duration, and a small margin
	halMcuWaitUs((12 * BASIC_RF_SYMBOL_DURATION) + (BASIC_RF_ACK_DURATION) + (2 * BASIC_RF_SYMBOL_DURATION) + 10);

	// If an acknowledgment has been received (by RxFrmDoneIsr), the ackReceived flag should be set
	status = txState.ackReceived ? SUCCESS : FAILED;

	} else {
	status = SUCCESS;
	}
#endif 

	// Turn off the receiver if it should not continue to be enabled
/*
	if (!txState.receiveOn) 
	{
		macReceiveOff();
	}
*/

	if(status == SUCCESS) 
	{
		txState.txSeqNumber++;
	}

#ifdef SECURITY_CCM
	halRfIncNonceTx();          // Increment nonce value
#endif


	return status;

}

static void macParameterSet(void)
{
	MAC_INT_OFF();

	// Set channel
	if(RF_CHAN_AUTO==pConfig->channel)
	{
		//MAC_RADIO_SET_CHANNEL(RF_CHANNEL);
		MAC_RADIO_SET_CHANNEL(pConfig->autoChan);
	}else
	{
		MAC_RADIO_SET_CHANNEL(pConfig->channel);
	}

	// Write the short address and the PAN ID to the CC2520 RAM
	MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
	MAC_RADIO_SET_PAN_ID(pConfig->panId);

	//MAC_RADIO_SET_IEEE_ADDR(pConfig->extAddr);	

	MAC_MCU_OR_RFIRQM1(IRQ_TXDONE);			//Enable TxDone interrupt
	MAC_MCU_OR_RFIRQM1(IRQ_TXACKDONE);
	MAC_INT_ON();
}

MAC_DATAREQ_API static void macNetCheck(macRx_t* pck)
{
	static uint8 netChk = 0;
	static uint8 chanRsv = 0;
	//static uint8 chkCnt=0;
	//static uint8 netStart=0;

	//if(MSTDEV_MODE_HOLD==g_isMaster)
	if((CMD_MODE_RF_MST!=g_isMaster)&&(CMD_MODE_RF_SLA!=g_isMaster)&&
		(CMD_MODE_RFLS_MST!=g_isMaster)&&(CMD_MODE_RFLS_SLA!=g_isMaster))
	{
		return;
	}

	if(RF_CHAN_AUTO==pConfig->channel)
	{
/*
		if(!g_netStart)
		{
			chanRsv = pConfig->autoChan;
			pConfig->autoChan = RF_CHANNEL;
			g_mstHold = 0;
			g_netStart = 1;
		}else if(g_mstHold)
		{
			g_netStart = 0;
		}
*/
		if(!netChk)
		{
			netChk++;
			chanRsv = pConfig->autoChan;
			pConfig->autoChan = RF_CHANNEL;
		}
		
		//if((pConfig->autoChan<(RF_CHANNEL+15))&&(!g_mstHold)&&(netChk<2))
		if(netChk<3)
		{
			macParameterSet();
			pConfig->autoChan++;
			pConfig->frameHead = 0xc801;
			pConfig->destAddr = 0x000f;
			pConfig->myAddr = 0xFFFF;
			osal_memcpy(pTxData,g_extAddr,8);
			//pConfig->myAddr = ADDR;
			pTxData[8]=0x91;
			//pTxData[9]=0x00;
			//pTxData[10]=0x30;
			//pTxData[11]=0x00;
			//pTxData[10]=(uint8)((pConfig->panId)>>8);
			//pTxData[11]=(pConfig->panId)&0xFF;
			pTxData[9]=(pConfig->panId)&0xFF;
			pTxData[10]=(uint8)((pConfig->panId)>>8);
			pTxData[11]=0x01;
			g_sendLen = 12;
			macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
			pConfig->myAddr = 0x000F;
			veriPckSendCallback = &macNetCheck;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 50);
			if(pConfig->autoChan>=(RF_CHANNEL+15))
			{
				netChk++;
				pConfig->autoChan = RF_CHANNEL;
				
			}
			//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 450);
		}//else if(pConfig->autoChan>=(RF_CHANNEL+15))
		else
		{
			netChk = 0;
			pConfig ->autoChan = chanRsv;
			macParameterSet();
			veriPckSendCallback = &macRollPolingProc;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 80);	
		}
		
	}else
	{
		if(netChk++<2)
		{
			pConfig->frameHead = 0xc801;
			pConfig->destAddr = 0x000f;
			pConfig->myAddr = 0xFFFF;
			osal_memcpy(pTxData,g_extAddr,8);
			//pConfig->myAddr = ADDR;
			pTxData[8]=0x91;
			//pTxData[9]=0x00;
			//pTxData[10]=0x30;
			//pTxData[11]=0x00;
			//pTxData[10]=(uint8)((pConfig->panId)>>8);
			//pTxData[11]=(pConfig->panId)&0xFF;
			pTxData[9]=(pConfig->panId)&0xFF;
			pTxData[10]=(uint8)((pConfig->panId)>>8);
			pTxData[11]=0x00;
			g_sendLen = 12;
			macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
			pConfig->myAddr = 0x000F;
			veriPckSendCallback = &macNetCheck;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 30);		
		}else
		{
			netChk = 0;
			veriPckSendCallback = &macRollPolingProc;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 80);	
			
		}
	}
	
}

MAC_DATAREQ_API void RflsStart(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//arrcpy(pTxData,preCmdBuf,sizeof(preCmdBuf));
	pTxData[0] = 0x04;
	pTxData[1] = 0x00;//0x01;
	pTxData[2] = 0xFF;
	pTxData[3] = 0x4B;
	g_sendLen = 4;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);

#if 0
	if(RFLS_COORDINATOR_M==pConfig->isRflsCoordinator)
	{
#ifdef SEND_UNIFIED
		veriPckSendCallback=&macRollPolingProc;
#else
		veriPckSendCallback=&LinkedModeRollPoling;
#endif
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);
	}else
	{
		veriPckSendCallback=NULL;
	}
#endif
	
}

MAC_DATAREQ_API static void macRollPolingProc(macRx_t* pck)
{
	uint8 sdCnt=0;
	uint8 cn=0;
	switch(g_isMaster)
	{
		//case MSTDEV_MODE_DEV:
		case CMD_MODE_RF_SLA:
		basicRfConfig.ackRequest = 0;
		pConfig->frameHead = 0xc801;
		pConfig->destAddr = 0x000f;
		pConfig->myAddr = 0xFFFF;
		osal_cpyExtAddr(pTxData,g_extAddr);
		//pConfig->myAddr = ADDR;
		pTxData[8]=0x91;
		//pTxData[9]=0x00;
		//pTxData[10]=0x30;
		//pTxData[11]=0x00;
		//pTxData[10]=(uint8)((pConfig->panId)>>8);
		//pTxData[11]=(pConfig->panId)&0xFF;
		pTxData[9]=(pConfig->panId)&0xFF;
		pTxData[10]=(uint8)((pConfig->panId)>>8);
		pTxData[11]=0x00;
		g_sendLen = 12;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);	
		veriPckSendCallback = &macRollPolingProc;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);
		break;

		//case MSTDEV_MODE_MST:
		//case MSTDEV_MODE_MLINK:
		//case MSTDEV_MODE_SLINK:
		case CMD_MODE_RF_MST:
		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//pConfig->myAddr = 0x000F;
		pTxData[sdCnt++] = 0x9A;
		pTxData[sdCnt++] = pConfig->evtCnt;//0x01;
		pTxData[sdCnt++] = pConfig->totalDev;//0x01;
		pTxData[sdCnt++] = pConfig->mstRcd+1;//0x01;
		pTxData[sdCnt++] = ((1<<pConfig->slvRcd)-1)&0xFF;//(1<<pTxData[2])-1;//0x01;
		pTxData[sdCnt++] = 0x00;
		pTxData[sdCnt++] = ((1<<pConfig->slvRcd)-1)&0xFF;//pConfig->slvRcd;
		pTxData[sdCnt++] = (0xFF00>>(pConfig->mstRcd+1))&0xFF;
		for(cn=0;cn<pConfig->mstRcd;cn++)
		{
			pTxData[sdCnt++] = 0x0E-cn;
		}
		//pTxData[sdCnt++] = 0x80;
		//g_sendLen = 8;
		macRfSendPacket(pConfig->destAddr, pTxData, sdCnt,0);
		veriPckSendCallback = &macRollPolingProc;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);
		break;

		case CMD_MODE_OFF:
		break;

/*
		default:
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//pConfig->myAddr = 0x000F;
		pTxData[0] = 0x9A;
		pTxData[1] = pConfig->evtCnt;//0x01;
		pTxData[2] = pConfig->totalDev;//0x01;
		pTxData[3] = 0x01;
		pTxData[4] = (1<<pTxData[2])-1;//0x01;
		pTxData[5] = 0x00;
		pTxData[6] = pTxData[4];
		pTxData[7] = 0x80;
		g_sendLen = 8;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		veriPckSendCallback = &macRollPolingProc;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);
		break;
*/
	}
#ifdef DEBUG_TRACE	
	DeviceInfoPrint();
#endif
/*
	if(MSTDEV_MODE_DEV==g_isMaster)
	{
		basicRfConfig.ackRequest = 0;
		pConfig->frameHead = 0xc801;
		pConfig->destAddr = 0x000f;
		pConfig->myAddr = 0xFFFF;
		osal_cpyExtAddr(pTxData,g_extAddr);
		//pConfig->myAddr = ADDR;
		pTxData[8]=0x91;
		pTxData[9]=0x00;
		//pTxData[10]=0x30;
		//pTxData[11]=0x00;
		pTxData[10]=(uint8)((pConfig->panId)>>8);
		pTxData[11]=(pConfig->panId)&0xFF;
		g_sendLen = 12;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);	
	}else
	{
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//pConfig->myAddr = 0x000F;
		pTxData[0] = 0x9A;
		pTxData[1] = pConfig->evtCnt;//0x01;
		pTxData[2] = pConfig->totalDev;//0x01;
		pTxData[3] = 0x01;
		pTxData[4] = (1<<pTxData[2])-1;//0x01;
		pTxData[5] = 0x00;
		pTxData[6] = pTxData[4];
		pTxData[7] = 0x80;
		g_sendLen = 8;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		
	} 
	veriPckSendCallback = &macRollPolingProc;
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);
*/
}

#ifndef SEND_UNIFIED
MAC_DATAREQ_API static void LinkedModeRollPoling(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x000F;
	pTxData[0] = 0x9A;
	pTxData[1] = pConfig->evtCnt;//0x01;
	pTxData[2] = pConfig->totalDev;//0x01;
	pTxData[3] = 0x02;
	pTxData[4] = pConfig->mstRcd;//pConfig->mstRcd;//0x00;//0x01;
	pTxData[5] = 0x00;
	pTxData[6] = pConfig->totalDev-pConfig->mstRcd;//0x00;
	pTxData[7] = 0xC0;
	pTxData[8] = 0x0E;
	g_sendLen = 9;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	veriPckSendCallback = &LinkedModeRollPoling;
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);

}
#endif

static void PortInit(void)
{
	LED_RED_ON();
#ifdef LED_TOGGLE_DEBUG
	osal_start_timerEx( macEx_taskId, MACEX_LED_FLASH, 500);
#endif

}

static void macExRxEnable(void)
{
	//static int8 rxFlag=0;
	//macRxEnable(1);

	if(!s_rxFlag)
	{
		macRxEnable(1);
		s_rxFlag = 1;
	}
}

static void macExRxDisable(void)
{
	//macRxDisable(1);

	if(s_rxFlag)
	{
		//macRxEnable(1);
		//S1CON = 0x00;
		macRxDisable(1);
		s_rxFlag = 0;
	}
}

//extern macTx_t *pMacDataTx;
void macTaskInitEx(uint8 taskId)
{
	//uint8 buf[8];
	uint8 rssiBuf[16];

	macEx_taskId = taskId;
	pConfig = &basicRfConfig;
	
#ifdef DEBUG_TRACE
	macDataTx.msdu.p = pTxData;
	pMacDataTx = &macDataTx;
	
#endif
	
	//MAC_AUTO_CRC_ACK();
	//MAC_RECOMMEND_SETTING();
	//macEnableRxInterrupt();
	macLowLevelInit();
#ifdef DEBUG_TRACE
	g_isMaster = CMD_MODE_RF_MST;
	macRxEnable(1);
#endif

#ifdef DEBUG_NOTRACE
	g_isMaster = CMD_MODE_RF_MST;
	macRxEnable(1);
#endif

	macExUartInit();
	PortInit();
	//macRadioSetTxPower(0xF5);
	//macRadioSetTxPower(0x05);
#ifdef HAL_MAC_USE_REGISTER_POWER_VALUES
	macRadioSetTxPower(0xf5);
#endif

	//FRMCTRL0 |= 0x10;

	//CalcEachRssi(rssiBuf,1,0);
	//pConfig->autoChan = RF_CHANNEL;
	macMemReadRam((macRam_t *) TI_EXTADDR, g_extAddr, 8);
#ifdef DEBUG_TRACE
	Printf("ExtAddr: ");
	BufferPrint(g_extAddr, 8);
#endif
	MAC_RADIO_SET_IEEE_ADDR(g_extAddr);


/*
	HalFlashInit();
	HalFlashErase(20);
	HalFlashWrite(20, g_extAddr, 8);
	HalFlashRead(20,0,buf,8);*/
	//macRxOn();

	//g_linkedMode = 1;
	//g_linkedMode = 0;				//linked mode
	// Config basicRF
	//if((MSTDEV_MODE_MLINK==g_isMaster)||(MSTDEV_MODE_SLINK==g_isMaster))

	basicRfConfig.panId = 0x3000;

	basicRfConfig.channel = RF_CHANNEL;
	//basicRfConfig.ackRequest = 0;//TRUE;
	if(CMD_MODE_RF_SLA==g_isMaster)
	{
		//basicRfConfig.myAddr = ADDR; 
		//basicRfConfig.myAddr = 0xFFFF; 
		basicRfConfig.myAddr = 0x00FF; 
	}else
	{
		basicRfConfig.myAddr = ADDR; 
	}
	basicRfConfig.extAddr = g_extAddr;
	pCurrentDev = (void *)NULL;
	//basicRfConfig.devHead = deviceInfo;
	basicRfConfig.totalDev = 0;
	basicRfConfig.evtCnt = 0;
	basicRfConfig.mstRcd = 0;
	basicRfConfig.slvRcd = 0;
	osal_memset(basicRfConfig.idInGroup, 0x00, 16);
	osal_memset(basicRfConfig.arrCnt, 0x00, 5);
	osal_memset(basicRfConfig.isIdOccupied,0x00,16);
	//basicRfConfig.group = 1;

	//basicRfConfig.panId = 6666;

	//RfModCheck(g_isMaster);

	txState.txDone = 0;

	//pConfig = &basicRfConfig;
	rxi.pPayload   = NULL;
	txState.receiveOn = TRUE;
	txState.frameCounter = 0;

	ipRsv.mst = 0x0E;
	ipRsv.sla = 0x00;

	macParameterSet();

	//pConfig->autoChan = RF_CHANNEL;

	CalcEachRssi(rssiBuf,50,10);
	//macExRxEnable();

	//macRfReceiveOn();
	g_initCheck = 0;
	
#ifndef DEBUG_NOTRACE
	UartSctSend(UART_CC_INIT,1,0);
	osal_start_timerEx(macEx_taskId, MACEX_INIT_CHECK, 1000);
#endif

#ifdef LED_TOGGLE_DEBUG
	osal_start_timerEx( macEx_taskId, MACEX_LED_FLASH, 500);
#endif	

	//macNetCheck();
	
	//osal_start_timerEx(macEx_taskId,MACEX_ROLL_POLING_EVENT,30);
	macNetCheck(callbackPara);
	
	
}

MAC_DATAREQ_API static void VerfiInqure(macRx_t* pck)
{
	static uint8 veriCnt=0;
	if(veriCnt++<2)
	{
		pConfig->frameHead = 0x8C21;
		pConfig->destAddr = 0xffff;
		pTxData[0]=0x92;
		pTxData[1]=((pConfig->panId)&0xFF);//0x00;
		pTxData[2]=((pConfig->panId)>>8);
		//pTxData[2]=0x30;
		/*
		if(g_linkedMode)
		{
			pTxData[2]=((pConfig->panId)>>8);
		}else
		{
			pTxData[2]=((pConfig->panId)>>8);
		}*/
		if(RF_CHAN_AUTO==pConfig->channel)
		{
			pTxData[3]=0x01;
		}else
		{
			pTxData[3]=0x00;
		}
		g_sendLen = 4;
		//macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,1);
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,1);
		veriPckSendCallback = &VerfiInqure;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,10);
	}else
	{
		veriCnt = 0;
		g_quireFlag = 0;
		RemoveCurrent();
		veriPckSendCallback = &macRollPolingProc;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,500);
	}
	
}

static void macMallocNewDev(uint8* extAddr)
{
	//MSGpkt = (afIncomingMSGPacket_t *)osal_msg_allocate( len );
	DeviceInfo *newDevice;
	newDevice = (DeviceInfo*)osal_mem_alloc(sizeof(DeviceInfo));
	newDevice ->pNext= NULL;
	newDevice->pFront = NULL;
	newDevice->veriFlag = DEVICE_UNVERIFIED;
	if(NULL!=extAddr)
	{
		osal_cpyExtAddr(newDevice->extAddr,extAddr);
	}
#ifdef DEBUG_TRACE
		Printf("pCurrentDev %x\r\n",pCurrentDev);
		Printf("newDevice %x\r\n",newDevice);
#endif
	//if(NULL==pCurrentDev)
	if(!pConfig->totalDev)
	{
		pCurrentDev = newDevice;
		pConfig->devHead = newDevice;
		newDevice->pFront = NULL;
#ifdef DEBUG_TRACE
	Printf("first dev %x\r\n",pCurrentDev);
#endif
		//pConfig->devRear = newDevice;
	}else
	{
		//pConfig->pCurrentDev->pNext= newDevice;
		//newDevice->pFront = pConfig->pCurrentDev;
		//(pConfig->devRear) ->pNext = pConfig->pCurrentDev;
#ifdef DEBUG_TRACE
		Printf("pCurrentDev %x\r\n",pCurrentDev);
		Printf("newDevice %x\r\n",newDevice);
#endif
		newDevice ->pFront = pCurrentDev;
		pCurrentDev->pNext = newDevice;
		pCurrentDev = newDevice;
	}
#ifdef DEBUG_TRACE
	Printf("pCurrentDev %x\r\n",pCurrentDev);
	Printf("pCurrentDev->pNext %x\r\n",pCurrentDev->pNext);
#endif
	
}

static void macDeleteAllDevice(void)
{
	DeviceInfo *fDev;
	DeviceInfo *bDev;
	fDev = pConfig->devHead;
	bDev = fDev;
	if(fDev!=NULL)
	{
		while(fDev->pNext!=NULL)
		{
			bDev = fDev;
			fDev = fDev->pNext;
			osal_mem_free(bDev);
		}
	}
	if(fDev!=NULL)
	{
		osal_mem_free(fDev);
	}
	/*
	if(pConfig->devHead!=NULL)
	{
		while(pDev!=NULL)
		{
			pCurrentDev = pDev->pFront;
			osal_mem_free(pDev);
			pDev = pCurrentDev;
		}
		pConfig->devHead=NULL;
	}*/

	pCurrentDev = pConfig->devHead=NULL;
	pConfig->totalDev = 0;
	pConfig->slvRcd = 0;
	pConfig->mstRcd = 0;
	//pConfig->isRflsCoordinator = 0;
	pConfig->evtCnt = 0;

	ipRsv.mst = 0x0E;
	ipRsv.sla = 0x00;

	s_ready = 3;

	osal_memset(pConfig->arrCnt,0,5);
	osal_memset(pConfig->isIdOccupied, 0, 16);
	LED_INS_UNLINK();
	
}

static void VeriFailedProc(void)
{
}

MAC_DATAREQ_API static void DevReadyInfo(macRx_t* pck)
{
	static uint8 sr=2;

	if(sr!=s_ready)
	{
		UartSctSend(UART_CC_READY,s_ready,0);
		sr = s_ready;
	}

#ifdef SEND_UNIFIED
	veriPckSendCallback = &macRollPolingProc;
#else
	if(0X01==pCurrentDev->group)
	{
		veriPckSendCallback = &LinkedModeRollPoling;
	}else
	{
		veriPckSendCallback = &macRollPolingProc;
	}
#endif

	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,10);
	
}

MAC_DATAREQ_API static void BridgedMstReadyRes(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;	
	//pConfig->myAddr = 0x000F;
	pTxData[0] = 0x05;
	pTxData[1] = 0x00;
	pTxData[2] = 0xFF;
	pTxData[3] = 0x32;
	pTxData[4] = 0x01;//0x01;//0xAA;//0x01;
	g_sendLen = 5;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	veriPckSendCallback = &macRollPolingProc;
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,8+(osal_rand()&0x07));
	

}

MAC_DATAREQ_API static void DeviceReadyRespond(macRx_t* pck)
{
	static uint8 rspMk=0;
	DeviceInfo *pDevice;
	pDevice = SearchForDevice(pck->mac.srcAddr.addr.shortAddr);
	pDevice->ready = pck->msdu.p[4];

	if(pDevice->ready)
	{
		rspMk = 0;
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;	
		//pConfig->myAddr = 0x000F;
		pTxData[0] = 0x05;
		pTxData[1] = 0x00;
		pTxData[2] = 0xFF;
		pTxData[3] = 0x32;
		pTxData[4] = pDevice->ready;//0x01;//0xAA;//0x01;
		g_sendLen = 5;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	}else if(!rspMk)
	{
		rspMk = 1;
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;	
		//pConfig->myAddr = 0x000F;
		pTxData[0] = 0x05;
		pTxData[1] = 0x00;
		pTxData[2] = 0xFF;
		pTxData[3] = 0x32;
		pTxData[4] = pDevice->ready;//0x01;//0xAA;//0x01;
		g_sendLen = 5;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	}

	//while(!(RFIRQF1 & IRQ_TXDONE) );				//failed
	//UartSctSend(UART_CC_READY,pDevice->ready,0);
	s_ready = pDevice->ready;
	veriPckSendCallback = &DevReadyInfo;

/*
	if(0X01==pCurrentDev->group)
	{
		veriPckSendCallback = &LinkedModeRollPoling;

	}else
	{
		veriPckSendCallback = &macRollPolingProc;

	}
*/

	
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,4+(osal_rand()&0x03));

	
}

//uint16 devCnt=0;
static uint16 FirstCheckAndAlloc(void)
{
	pConfig->evtCnt++;
	pConfig->totalDev++;
	return 0x0000;
}


MAC_DATAREQ_API static void CheckEachDeviceAlloc(macRx_t* pck)
{
	if(g_devCheck.checkCount++<MACEX_DEVICE_CHECK_NUM)
	{
		pConfig->frameHead = 0x8861;
		//pConfig->destAddr = (DEVICE_ATTRIBUTE_MAST==g_devCheck.pChk->atribute)?(0x0E-g_devCheck.checkTurn):g_devCheck.checkTurn;
		pConfig->destAddr = g_devCheck.pChk->shortAddr;//(DEVICE_ATTRIBUTE_SLAVE==g_devCheck.pChk->atribute)?g_devCheck.checkTurn:(0x0E-g_devCheck.checkTurn);
		pTxData[0] = 0x9B;
		g_sendLen = 1;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		veriPckSendCallback = &CheckEachDeviceAlloc;
		//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,8);	
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,8+(osal_rand()&0x07));
	}else
	{
#if 0
		if(g_devCheck.checkCount>=MACEX_DEVICE_CHECK_NUM)
		{
			RemoveDevice(g_devCheck.checkTurn);
			pCurrentDev.shortAddr = g_devCheck.checkTurn;			
			VeriResponse();
			g_devCheck.checkCount = 0;
			g_devCheck.checkTurn = 0;
		}else
		{
			g_devCheck.checkTurn++;
			g_devCheck.checkCount = 0;
			if(g_devCheck.checkTurn>=pConfig->totalDev)
			{
				pConfig->evtCnt++;
				pConfig->totalDev++;	
				pCurrentDev.shortAddr = g_devCheck.checkTurn;
				VeriResponse();
				g_devCheck.checkTurn = 0;				
			}else
			{
				veriPckSendCallback = &CheckEachDeviceAlloc;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);
			}
			
		}
#endif

		pConfig->evtCnt++;
		//g_devCheck.checkTurn++;
		//if(g_devCheck.checkTurn>=pConfig->totalDev)
		//if(NULL==g_devCheck.pChk->pNext)
		if(pCurrentDev==g_devCheck.pChk->pNext)
		{
			//pCurrentDev->shortAddr = g_devCheck.checkTurn;
			RemoveDevice(g_devCheck.pChk->shortAddr);
			pConfig->totalDev++;
/*
			if(DEVICE_ATTRIBUTE_MAST==pCurrentDev->atribute)
			{
				pCurrentDev->shortAddr=0x0E-(pConfig->mstRcd-1);
			}else
			{
				pCurrentDev->shortAddr=(pConfig->slvRcd-1);
			}
*/			pCurrentDev->shortAddr = NoneUsedShortAddrSearch(pCurrentDev->atribute);
			VeriAndAllocID(pck);
			LED_INS_LINKED();
		}else
		{
			//pCurrentDev->shortAddr=RemoveDevice(g_devCheck.checkTurn);
#if 0
			if(g_devCheck.pChk->atribute==pCurrentDev->atribute)
			{
				pCurrentDev->shortAddr = RemoveDevice(g_devCheck.pChk->shortAddr);
				
				VeriAndAllocID(pck);
				g_devCheck.checkCount = 0;
				g_devCheck.checkTurn = 0;
				pConfig->totalDev++;

				LED_INS_LINKED();

				veriPckSendCallback = &macRollPolingProc;

				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);		

			}else
			{
				RemoveDevice(g_devCheck.pChk->shortAddr);
				g_devCheck.checkCount = 0;
				g_devCheck.pChk = g_devCheck.pChk->pNext;
				veriPckSendCallback = &CheckEachDeviceAlloc;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
				//pCurrentDev->shortAddr = NoneUsedShortAddrSearch(pCurrentDev->atribute);
			}
#else
			RemoveDevice(g_devCheck.pChk->shortAddr);
			g_devCheck.checkCount = 0;
			g_devCheck.pChk = g_devCheck.pChk->pNext;
			veriPckSendCallback = &CheckEachDeviceAlloc;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
#endif
/*
			if(DEVICE_ATTRIBUTE_MAST==pCurrentDev->atribute)
			{
				pCurrentDev->shortAddr=0x0E-RemoveDevice(g_devCheck.checkTurn);
			}else
			{
				pCurrentDev->shortAddr=RemoveDevice(g_devCheck.checkTurn);
			}
*/
			//pConfig->totalDev++;
		}
		//pCurrentDev->shortAddr = g_devCheck.checkTurn;
		//pCurrentDev->shortAddr=pConfig->destAddr;
		//pConfig->totalDev++;
		
		
		//if((MSTDEV_MODE_MLINK==g_isMaster)||
		//	(MSTDEV_MODE_SLINK==g_isMaster))

/*
		if(g_linkedMode)
		{
			VeriResponse(pck);
		}else
		{
			LinkedModeShortAddrAlloc(pck);
		}*/
		//g_devCheck.checkCount = 0;
		//g_devCheck.checkTurn = 0;
		//veriPckSendCallback = &macRollPolingProc;
		//if((MSTDEV_MODE_MLINK==g_isMaster)||
		//	(MSTDEV_MODE_SLINK==g_isMaster))

		
		
	}
	
}

#ifndef SEND_UNIFIED
MAC_DATAREQ_API static void VeriResponse(macRx_t* pck)
{
	pConfig->frameHead = 0x8C21;
	pConfig->destAddr = 0xffff;
	pTxData[0]=0x96;
	pTxData[1]=0x00;
	pTxData[2]=(pCurrentDev->shortAddr&0xFF);
	pTxData[3]=pTxData[2];
	pTxData[4]=0x00;
	pTxData[5]=pTxData[2]+1;
	pTxData[6] = pTxData[5];
	pTxData[7] = 0x01;
	pTxData[8] = (1<<pTxData[6]) - 1;
	pTxData[9]=0x00;
	pTxData[10]= pTxData[8];
	pTxData[11]=0x80;	
	g_sendLen = 12;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,1);
#ifdef SEND_UNIFIED
	veriPckSendCallback = &macRollPolingProc;
#else
	if(pConfig->mstRcd>0)
	{
		veriPckSendCallback = &LinkedModeRollPoling;
	}else
	{
		veriPckSendCallback = &macRollPolingProc;
	}
#endif
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);
}
#endif


MAC_DATAREQ_API static void VeriAndAllocID(macRx_t* pck)
{
	uint8 sdCnt=0;
	uint8 cn=0;
	pConfig->frameHead = 0x8C21;
	pConfig->destAddr = 0xffff;
	pTxData[sdCnt++]=0x96;
	pTxData[sdCnt++]=0x00;
	pTxData[sdCnt++]=(pCurrentDev->shortAddr&0xFF);
	pTxData[sdCnt++]=(pCurrentDev->shortAddr&0xFF);
	pTxData[sdCnt++]=0x00;
	pTxData[sdCnt++]=pConfig->evtCnt;
	pTxData[sdCnt++]=(pConfig->totalDev>0)?pConfig->totalDev:1;
	pTxData[sdCnt++]=pConfig->mstRcd+1;
	pTxData[sdCnt++]=((1<<pConfig->slvRcd)-1)&0xFF;
	pTxData[sdCnt++]=0x00;
	pTxData[sdCnt++]=pConfig->slvRcd;
	pTxData[sdCnt++]=(0xFF00>>(pConfig->mstRcd+1))&0xFF;	
	for(cn=0;cn<pConfig->mstRcd;cn++)
	{
		pTxData[sdCnt++]=0x0E-cn;
	}	
	//g_sendLen = 12;
	macRfSendPacket(pConfig->destAddr, pTxData, sdCnt,1);
	veriPckSendCallback = &macRollPolingProc;
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);
}


#ifndef SEND_UNIFIED
MAC_DATAREQ_API static void LinkedModeShortAddrAlloc(macRx_t* pck)
{
	pConfig->frameHead = 0x8C21;
	pConfig->destAddr = 0xffff;
	pTxData[0]=0x96;
	pTxData[1]=0x00;
	pTxData[2]=0x0E-(pCurrentDev->shortAddr&0xFF);
	pTxData[3]=pTxData[2];
	pTxData[4]=0x00;
	pTxData[5]=(pCurrentDev->shortAddr&0xFF)+1;
	pTxData[6] = pTxData[5];
	pTxData[7] = 0x02;
	pTxData[8] = 0x00;
	pTxData[9]=0x00;
	pTxData[10]= 0x00;
	pTxData[11]=0xC0;	
	pTxData[12]=0x0E;
	g_sendLen = 13;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,1);
#ifdef SEND_UNIFIED
	veriPckSendCallback = &macRollPolingProc;
#else
	veriPckSendCallback = &LinkedModeRollPoling;
#endif
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);
}
#endif

#ifndef SEND_UNIFIED
MAC_DATAREQ_API static void LinkModeResp(macRx_t* pck)
{
	pConfig->frameHead = 0xc821;
	pConfig->destAddr = 0x000f;
	pConfig->myAddr = 0xFFFF;//ADDR;			aviod wrong receive
	//halRfSetShortAddr(pConfig->myAddr);
	osal_cpyExtAddr(pTxData,g_extAddr);
	pTxData[8]=0x95;
	//pTxData[9]=0x02;
	//pTxData[9]=basicRfConfig.group+2;
	pTxData[9]=0x01;
	g_sendLen = 10;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	pConfig->myAddr = 0xFFFE;//ADDR;			aviod wrong receive
	MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
	veriPckSendCallback = NULL;
	osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);	
}
#endif

//pRxBuf->mac.dstAddr.addr.extAddr
static void MstVerifyProc(macRx_t *payload)
{
	static uint8 relCmdCnt=0; 
	static uint8 slaCmdRel=0;
	static uint8 slaCmdModel=0;
	static uint8 slaCmdTest=0;
	static uint8 veriFlag=0;
	//static uint8 quireFlag=0;
	static uint16 rsvAddr;
	static uint8 readyFlag=0;
	DeviceInfo *pDev;

	if((veriFlag>0)&&((payload->msdu.p[0]!=0x9c)||(rsvAddr!=payload->mac.srcAddr.addr.shortAddr)))
	{
		veriFlag = 0;
	}

	//if((quireFlag>0)&&(payload->msdu.p[0]!=0x91))
	//{
	//	quireFlag = 0;
	//}
	if((g_quireFlag>0)&&(payload->msdu.p[0]!=0x91))
	{
		g_quireFlag = 0;
	}

	rsvAddr = payload->mac.srcAddr.addr.shortAddr;

	switch(payload->msdu.p[0])
	{
		case 0x91:
		//if((pConfig->panId+0x3000)==MAKEWORD(payload->msdu.p[2], payload->msdu.p[1]))
		//if(!quireFlag)
		if(!g_quireFlag)
		{
			//quireFlag = 1;
			g_quireFlag = 1;
			macMallocNewDev(payload->mac.srcAddr.addr.extAddr);
			veriPckSendCallback = &VerfiInqure;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
			//VerfiInqure();
		}
		break;

		case 0x95:
		if(TRUE==osal_ExtAddrEqual(pCurrentDev->extAddr,payload->mac.srcAddr.addr.extAddr))
		{
			//pCurrentDev->group = payload->msdu.p[1]-2;
			
			if(pConfig->totalDev>0)
			{
				g_devCheck.checkCount = 0;
				g_devCheck.checkTurn = 0;
				g_devCheck.pChk = pConfig->devHead;
#ifdef DEBUG_TRACE
				Printf("CHK DEV: %d\r\n",pConfig->devHead->shortAddr);
#endif
				veriPckSendCallback = &CheckEachDeviceAlloc;

				//if(0x01==pCurrentDev->group)
				if(0x01== payload->msdu.p[1])
				{
					pCurrentDev->atribute = DEVICE_ATTRIBUTE_MAST;
					pConfig->mstRcd++;
					pCurrentDev->group = 0x0F;
				}else
				{
					pCurrentDev->atribute = DEVICE_ATTRIBUTE_SLAVE;
					//pConfig->idInGroup[pCurrentDev->shortAddr] = payload->msdu.p[1]-2;    //2014/10/29
					//pConfig->arrCnt[payload->msdu.p[1]-2]++;							//2014/10/29
					pConfig->slvRcd++;
					g_grInfo = 1;
					//pCurrentDev->group = payload->msdu.p[1]-2;						//2014/10/29
					//UartSctSend(UART_CC_DEV,pCurrentDev->group,pConfig->arrCnt[pCurrentDev->group]);  //2014/10/29
				}
				
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
			}else
			{

				//pCurrentDev->shortAddr = FirstCheckAndAlloc();
#ifdef SEND_UNIFIED
				if(0x01==payload->msdu.p[1])				//the linking device is a master
				{
					//veriPckSendCallback = &LinkedModeShortAddrAlloc;
					pCurrentDev->atribute = DEVICE_ATTRIBUTE_MAST;
					pConfig->mstRcd++;
					pCurrentDev->group = 0x0F;
					//pCurrentDev->shortAddr = 0x0E-FirstCheckAndAlloc();
					pCurrentDev->shortAddr = ipRsv.mst;
					pConfig->isIdOccupied[ipRsv.mst] = 1;
					ipRsv.mst--;
					pConfig->evtCnt++;
					pConfig->totalDev++;
				}else
				{
					//veriPckSendCallback = &VeriResponse;
					//pCurrentDev->shortAddr = FirstCheckAndAlloc();
					pCurrentDev->shortAddr = ipRsv.sla;
					pConfig->isIdOccupied[ipRsv.sla] = 1; 
					ipRsv.sla++;
					pConfig->evtCnt++;
					pConfig->totalDev++;
					pCurrentDev->atribute = DEVICE_ATTRIBUTE_SLAVE;
					pConfig->slvRcd++;
					g_grInfo = 1;
					//pConfig->idInGroup[pCurrentDev->shortAddr]=payload->msdu.p[1]-2;
					//pConfig->arrCnt[payload->msdu.p[1]-2]++;
					//pCurrentDev->group = payload->msdu.p[1]-2;
					//UartSctSend(UART_CC_DEV,pCurrentDev->group,pConfig->arrCnt[pCurrentDev->group]);
				}
				LED_INS_LINKED();
				veriPckSendCallback = &VeriAndAllocID;
#else
				//if(0x01==pCurrentDev->group)				//the linking device is a master
				if(0x01== payload->msdu.p[1])
				{
					veriPckSendCallback = &LinkedModeShortAddrAlloc;
					pCurrentDev->atribute = DEVICE_ATTRIBUTE_MAST;
					pConfig->mstRcd++;
					pCurrentDev->group = 0x0F;
				}else
				{
					veriPckSendCallback = &VeriResponse;
					pCurrentDev->atribute = DEVICE_ATTRIBUTE_SLAVE;
					//pConfig->mstRcd++;
					pConfig->idInGroup[pCurrentDev->shortAddr]=payload->msdu.p[1]-2;
					pConfig->arrCnt[payload->msdu.p[1]-2]++;
					pCurrentDev->group = payload->msdu.p[1]-2;
					UartSctSend(UART_CC_DEV,pCurrentDev->group,pConfig->arrCnt[pCurrentDev->group]);
				}
#endif		
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
				
			}
		}else
		{
			VeriFailedProc();
		}
		break;

		case 0x05:
		switch(MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]))
		//if(g_linkedMode)
		{
			case 0x00FF:
				veriPckSendCallback = &BridgedMstReadyRes;
				callbackPara = payload;
				//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 6+(osal_rand()&0x07));
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 60+(osal_rand()&0x0F));
			break;
			
			case 0x0020:
			//pConfig->devHead->ready = payload->msdu.p[4];	
			//pCurrentDev->ready = payload->msdu.p[4];
			if(g_grInfo>0)
			{
				g_grInfo = 0;
				pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr] = payload->msdu.p[3];    //2014/10/29
				pConfig->arrCnt[payload->msdu.p[3]]++;							//2014/10/29
				pConfig->isIdOccupied[payload->mac.srcAddr.addr.shortAddr]= 1;
			//		pConfig->slvRcd++;
				//pCurrentDev->group = payload->msdu.p[1];						//2014/10/29
				//UartSctSend(UART_CC_DEV,pCurrentDev->group,pConfig->arrCnt[pCurrentDev->group]);  //2014/10/29
				UartSctSend(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[payload->msdu.p[3]]);
			}
			if(pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]!=payload->msdu.p[3])
			{
				if(pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]>0)
				{
					pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]--;
					UartSctSend(UART_CC_DEV, pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr], pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]);
				}
				
				pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr] = payload->msdu.p[3];
				pConfig->arrCnt[payload->msdu.p[3]]++;
				UartSctSend(UART_CC_DEV, payload->msdu.p[3], pConfig->arrCnt[payload->msdu.p[3]]);
				
			}
			//if(readyFlag !=payload->msdu.p[4])
			//{
			//	readyFlag = payload->msdu.p[4];
			//	UartSctSend(UART_CC_READY,readyFlag,0);
			//}			
			veriPckSendCallback = &DeviceReadyRespond;
			callbackPara = payload;
			//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 60+(osal_rand()&0x0F));
			break;

			case 0x0049:
				switch(payload->msdu.p[3])
				{
					case 0x01:				//slave REL
					//veriPckSendCallback = NULL;	
					if((++slaCmdRel)>=9)
					{
						//UartSctSend(UART_CC_REL,0,0);
						UartSctSend(UART_CC_REL,UART_CC_REL_FLA,0);
						if(BRIDGE_MST_NORMAL==g_mstHold)
						{
							veriPckSendCallback = &macRollPolingProc;
						}else
						{
							osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
						}
						slaCmdRel = 0;
					}
					//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 500);
					break;

					case 0x02:				//slave MODEL
					//veriPckSendCallback = NULL;
					if((++slaCmdModel)>=9)
					{
						UartSctSend(UART_CC_SLACMD,UART_CC_SLACMD_MODEL,0);
						if(BRIDGE_MST_NORMAL==g_mstHold)
						{
							veriPckSendCallback = &macRollPolingProc;
						}else
						{
							osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
						}
						slaCmdModel = 0;
					}
					//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 500);
					break;

					case 0x03:				//slave TEST
					//veriPckSendCallback = NULL;
					if((++slaCmdTest)>=9)
					{
						UartSctSend(UART_CC_SLACMD,UART_CC_SLACMD_TEST,0);
						if(BRIDGE_MST_NORMAL==g_mstHold)
						{
							veriPckSendCallback = &macRollPolingProc;
						}else
						{
							osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
						}
						slaCmdTest = 0;
					}
					//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 500);
					break;
				}
			break;

			
		}
		break;

		case 0x04:				//slave switch the group
		if(CMD_MODE_RF_MST==g_isMaster)
		{
			//pConfig->arrCnt[pConfig->idInGroup[payload->msdu.p[1]]]--; //assumed the msdu.p[1] is the short address
#ifdef DEBUG_TRACE
			Printf("\r\n");
			Printf("GR %x to %x\r\n", pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],payload->msdu.p[3]);
			Printf("arrCnt: ");
			BufferPrint(pConfig->arrCnt, 5);
#endif
			pDev = SearchForDevice(payload->mac.srcAddr.addr.shortAddr);
			pDev->group = payload->msdu.p[3];
			if(pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]>0)
			{
				pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]--;
			}
			//LoadUartSendBuf(UART_CC_DEV,pConfig->idInGroup[payload->msdu.p[1]],pConfig->arrCnt[pConfig->idInGroup[payload->msdu.p[1]]]);
			LoadUartSendBuf(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]);
			pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr] = payload->msdu.p[3];
			pConfig->arrCnt[payload->msdu.p[3]]++;
			//Printf("GR %x to %x\r\n", pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],payload->msdu.p[3]);
#ifdef DEBUG_TRACE
			Printf("totalDev: %d; ",pConfig->totalDev);
			Printf("arrCnt: ");
			BufferPrint(pConfig->arrCnt, 5);
			Printf("idInGroup: ");
			BufferPrint(pConfig->idInGroup, 16);
#endif
			LoadUartSendBuf(UART_CC_DEV,payload->msdu.p[3],pConfig->arrCnt[payload->msdu.p[3]]);
			TrigerUartSend();
		}else if(CMD_MODE_RFLS_MST==g_isMaster)//linked shot mode switch
		{
			if(0xFF4B==MAKEWORD(payload->msdu.p[2],payload->msdu.p[3]))
			{
				g_isMaster = CMD_MODE_RFLS_SLA;
				UartSctSend(UART_CC_MSTSW,0,0);
			}
		}
		break;

		case 0x06:
		//veriPckSendCallback = NULL;	
		if((++relCmdCnt)>=9)
		{
			//UartSctSend(UART_CC_REL,0,0);
			UartSctSend(UART_CC_REL,UART_CC_REL_FLA,0);
			if(BRIDGE_MST_NORMAL==g_mstHold)
			{
				veriPckSendCallback = &macRollPolingProc;
			}else
			{
				osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
			}
			relCmdCnt = 0;
		}
		//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 500);
		break;

		case 0x9C:
			if(!veriFlag)
			{
				g_devCheck.checkTurn++;
				g_devCheck.checkCount=0;
				//g_devCheck.pChk = g_devCheck.pChk->pNext;
				//if(g_devCheck.checkTurn>=pConfig->totalDev)
				//if(NULL==g_devCheck.pChk->pNext)
				if(pCurrentDev==g_devCheck.pChk->pNext)
				{
					//pCurrentDev->shortAddr = g_devCheck.checkTurn;
					//pCurrentDev->shortAddr = (DEVICE_ATTRIBUTE_SLAVE==g_devCheck.pChk->atribute)?g_devCheck.checkTurn:(0x0E-g_devCheck.checkTurn);
					pCurrentDev->shortAddr = NoneUsedShortAddrSearch(pCurrentDev->atribute);//(DEVICE_ATTRIBUTE_SLAVE==pCurrentDev->atribute)?(pConfig->slvRcd-1):(0x0E-(pConfig->mstRcd-1));
					pConfig->totalDev++;
					pConfig->evtCnt++;
#ifdef SEND_UNIFIED	
					veriPckSendCallback = &VeriAndAllocID;
#else
					veriPckSendCallback = &VeriResponse;
#endif
					LED_INS_LINKED();
					osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
				}else
				{
					//g_devCheck.pChk = pConfig->devHead;
					g_devCheck.pChk = g_devCheck.pChk->pNext;
					veriPckSendCallback = &CheckEachDeviceAlloc;
					osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
				}
				veriFlag = 1;
			}
		break;

		case 0x97:
			//if(0x02==payload->msdu.p[2])
			{
				//UartSctSend(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]);
				RemoveDevice(payload->mac.srcAddr.addr.shortAddr);
				//pConfig->totalDev--;
				pConfig->evtCnt++;
				//if(pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]>0)
				//{
				//	pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]--;
				//}
				//UartSctSend(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]);
				
			}
		break;

		case 0x92:			//already have a master
			//if(pConfig->panId==MAKEWORD(payload->msdu.p[2],payload->msdu.p[3]))
			{
				veriPckSendCallback = &InqureConfirm;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
				osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
				pConfig->isRflsCoordinator = RFLS_COORDINATOR_S;
				g_mstHold = 1;
				g_netStart = 0;
				if(CMD_MODE_RF_MST==g_isMaster)
				{
					LED_INS_BRIGDE();
				}else
				{
					LED_INS_LINKED();
				}
			}				
		break;

/*
		case 0x96:
			pConfig->myAddr = MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]);
			MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
			veriPckSendCallback = NULL;
			//veriPckSendCallback=&ReadyInfo;
			//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);	
			//osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
			//g_mstHold = 1;
		break;
*/

		case 0x9A:
			if(1==g_mstHold)
			{
				osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
			}
		break;

		case 0x9B:
			veriPckSendCallback = &ReConfirm;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);	
		break;

			
	}
}

static void MstBridgeHandle(macRx_t *payload)
{

	static uint8 relCmdCnt=0; 
	uint16 tm;
	DeviceInfo *pDev;
	
	switch(payload->msdu.p[0])
	{
		case 0x9B:
			veriPckSendCallback = &ReConfirm;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);	
		break;

		case 0x05:
			switch(MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]))
			{
				case 0x0020:
					//UartSctSend(payload->mac.srcAddr.addr.shortAddr,payload->msdu.p[3],pConfig->arrCnt[payload->msdu.p[3]]);
					if(!pConfig->isIdOccupied[payload->mac.srcAddr.addr.shortAddr])
					{
#ifdef DEBUG_TRACE
						Printf("BRG %x,%x",payload->mac.srcAddr.addr.shortAddr,payload->msdu.p[3]);
#endif
						//UartSctSend(payload->mac.srcAddr.addr.shortAddr,payload->msdu.p[3],pConfig->arrCnt[payload->msdu.p[3]]);
						pConfig->isIdOccupied[payload->mac.srcAddr.addr.shortAddr]= 1;
						macMallocNewDev(NULL);
						//UartSctSend(payload->mac.srcAddr.addr.shortAddr,payload->msdu.p[3],pConfig->arrCnt[payload->msdu.p[3]]);
						pCurrentDev->shortAddr = payload->mac.srcAddr.addr.shortAddr;
						pCurrentDev->veriFlag = DEVICE_VERIFIED;
						pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr] = payload->msdu.p[3];    //2014/10/29
						pConfig->arrCnt[payload->msdu.p[3]]++;							//2014/10/29
						pConfig->slvRcd++;
						pConfig->totalDev++;
						//pCurrentDev->group = payload->msdu.p[1];						//2014/10/29
						//UartSctSend(UART_CC_DEV,pCurrentDev->group,pConfig->arrCnt[pCurrentDev->group]);  //2014/10/29
						UartSctSend(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[payload->msdu.p[3]]);
						//UartSctSend(payload->mac.srcAddr.addr.shortAddr,payload->msdu.p[3],pConfig->arrCnt[payload->msdu.p[3]]);
						//veriPckSendCallback=&ReadyInfo;
						//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);	
					}
					UartSctSend(UART_CC_READY,payload->msdu.p[4],0);
					veriPckSendCallback=&ReadyInfo;
					//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);	
					osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10+(osal_rand()&0x07));	
				break;
			}/*
			if(0x3200==MAKEWORD(payload->msdu.p[3],payload->msdu.p[4]))
			{
				if(!pConfig->isIdOccupied[payload->mac.srcAddr.addr.shortAddr])
				{
					macMallocNewDev(payload->mac.srcAddr.addr.extAddr);
					pCurrentDev->shortAddr = payload->mac.srcAddr.addr.shortAddr;
					pCurrentDev->veriFlag = DEVICE_VERIFIED;
					pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr] = payload->msdu.p[3];    //2014/10/29
					pConfig->arrCnt[payload->msdu.p[3]]++;							//2014/10/29
					pConfig->slvRcd++;
					pConfig->totalDev++;
					pConfig->isIdOccupied[payload->mac.srcAddr.addr.shortAddr]= 1;
					//pCurrentDev->group = payload->msdu.p[1];						//2014/10/29
					//UartSctSend(UART_CC_DEV,pCurrentDev->group,pConfig->arrCnt[pCurrentDev->group]);  //2014/10/29
					UartSctSend(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[payload->msdu.p[3]]);
				}
				//veriPckSendCallback=&ReadyInfo;
				//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);	
			}*/
		break;

		case 0x06:
		veriPckSendCallback = NULL;
		if((++relCmdCnt)>=9)
		{
			//UartSctSend(UART_CC_REL,0,0);
			UartSctSend(UART_CC_REL,UART_CC_REL_FLA,0);
			relCmdCnt = 0;
		}
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 500);
		break;

		case 0x9A:
			if(1==g_mstHold)
			{
				if(payload->msdu.p[2]>0)
				{
					osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
				}else
				{
					if(payload->mac.srcAddr.addr.shortAddr>pConfig->myAddr)
					{
						tm = 50+(payload->mac.srcAddr.addr.shortAddr-pConfig->myAddr-1)*1000;
					}else
					{
						tm = 50;
					}
					osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,tm);
				}
			}
		break;		

		case 0x96:
			pConfig->myAddr = MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]);
			MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
			//veriPckSendCallback = NULL;
			veriPckSendCallback=&MstBridgeInfoInqure;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5+(osal_rand()&0x03));	
			//osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
			//g_mstHold = 1;
		break;

		case 0x97:
			if(0x0FFE==MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]))
			{
				if(payload->mac.srcAddr.addr.shortAddr>pConfig->myAddr)
				{
					tm = 50+(payload->mac.srcAddr.addr.shortAddr-pConfig->myAddr-1)*1000;
				}else
				{
					tm = 50;
				}
				osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,tm);
			}else
			{
				RemoveDevice(payload->mac.srcAddr.addr.shortAddr);
				//pConfig->totalDev--;
				pConfig->evtCnt++;
			}
		break;

		case 0x04:
		if(CMD_MODE_RF_MST==g_isMaster)
		{
			//pConfig->arrCnt[pConfig->idInGroup[payload->msdu.p[1]]]--; //assumed the msdu.p[1] is the short address
#ifdef DEBUG_TRACE
			Printf("\r\n");
			Printf("GR %x to %x\r\n", pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],payload->msdu.p[3]);
			Printf("arrCnt: ");
			BufferPrint(pConfig->arrCnt, 5);
#endif
			pDev = SearchForDevice(payload->mac.srcAddr.addr.shortAddr);
			pDev->group = payload->msdu.p[3];
			if(pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]>0)
			{
				pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]--;
			}
			//LoadUartSendBuf(UART_CC_DEV,pConfig->idInGroup[payload->msdu.p[1]],pConfig->arrCnt[pConfig->idInGroup[payload->msdu.p[1]]]);
			LoadUartSendBuf(UART_CC_DEV,pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],pConfig->arrCnt[pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr]]);
			pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr] = payload->msdu.p[3];
			pConfig->arrCnt[payload->msdu.p[3]]++;
			//Printf("GR %x to %x\r\n", pConfig->idInGroup[payload->mac.srcAddr.addr.shortAddr],payload->msdu.p[3]);
#ifdef DEBUG_TRACE
			Printf("totalDev: %d; ",pConfig->totalDev);
			Printf("arrCnt: ");
			BufferPrint(pConfig->arrCnt, 5);
			Printf("idInGroup: ");
			BufferPrint(pConfig->idInGroup, 16);
#endif
			LoadUartSendBuf(UART_CC_DEV,payload->msdu.p[3],pConfig->arrCnt[payload->msdu.p[3]]);
			TrigerUartSend();
		}		else if(CMD_MODE_RFLS_MST==g_isMaster)//linked shot mode switch
		{
			if(0xFF4B==MAKEWORD(payload->msdu.p[2],payload->msdu.p[3]))
			{
				g_isMaster = CMD_MODE_RFLS_SLA;
				UartSctSend(UART_CC_MSTSW,0,0);
			}
		}
		break;

		
	}
}

DEVICE_DEFINE static void devJoinNetProc(macRx_t *payload)
{
	switch(payload->msdu.p[0])
	{
		case 0x92:
			veriPckSendCallback = &InqureConfirm;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);		
		break;

		case 0x96:
			//pConfig->myAddr = MAKEWORD(mpdu[1],mpdu[2]);
			pConfig->myAddr = MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]);
			MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
			veriPckSendCallback=&ReadyInfo;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);
			//osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,2000);
		break;

		case 0x9B:
			veriPckSendCallback = &ReConfirm;
			osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);	
		break;

		case 0x05:
			switch(MAKEWORD(payload->msdu.p[3],payload->msdu.p[4]))
			{
				case 0x3200:
				case 0x3300:
				veriPckSendCallback=&ReadyInfo;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);
				break;
				
			}
			/*
			if(0x3200==MAKEWORD(payload->msdu.p[3],payload->msdu.p[4]))
			{
				veriPckSendCallback=&ReadyInfo;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 10);	
			}*/
		break;

		case 0x9A:
			//osal_stop_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT);
			osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
			//g_expireCounter = 0;
			//HalUARTWrite(0,&payload->msdu.p[0],1);
		break;

		case 0x97:
			if(0x0FFE==MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]))
			{
				veriPckSendCallback=&macRollPolingProc;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 200);	
			}
		break;

		
	}
}

#ifndef SEND_UNIFIED
LINKMODE_DEFINE static void linkVeriProc(macRx_t *payload)
{
	static uint8 relCmdCnt=0; 
		
	switch(payload->msdu.p[0])
	{

		case 0x91:
		macMallocNewDev(payload->mac.srcAddr.addr.extAddr);
		veriPckSendCallback = &VerfiInqure;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
		break;

		case 0x95:
		if(TRUE==osal_ExtAddrEqual(pCurrentDev->extAddr,payload->mac.srcAddr.addr.extAddr))
		{
			pCurrentDev->group = payload->msdu.p[1];
#ifdef SEND_UNIFIED
			if(pConfig->totalDev>0)
			{
				g_devCheck.checkCount = 0;
				g_devCheck.checkTurn = 0;
				g_devCheck.pChk = pConfig->devHead;
				veriPckSendCallback = &CheckEachDeviceAlloc;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
			}else
			{

				pCurrentDev->shortAddr = 0x0E-FirstCheckAndAlloc();
				veriPckSendCallback = &VeriAndAllocID;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
			}
#else
			if(pConfig->totalDev>0)
			{
				g_devCheck.checkCount = 0;
				g_devCheck.checkTurn = 0;
				g_devCheck.pChk = pConfig->devHead;
				veriPckSendCallback = &CheckEachDeviceAlloc;
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
			}else
			{

				pCurrentDev->shortAddr = FirstCheckAndAlloc();
				veriPckSendCallback = &LinkedModeShortAddrAlloc;		
				osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
			}
#endif
			
		}else
		{
			VeriFailedProc();
		}
		break;
	
		case 0x92:
		veriPckSendCallback = &LinkModeResp;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);	
		pConfig->isRflsCoordinator = RFLS_COORDINATOR_S;
		break;

		case 0x96:
		pConfig->myAddr = MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]);
		MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
		veriPckSendCallback = NULL;
		
		break;

		case 0x9B:
		veriPckSendCallback = &ReConfirm;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
		break;

		case 0x9A:
		veriPckSendCallback = NULL;	
		osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);		
		break;

		case 0x06:
		if((++relCmdCnt)>=9)
		{
			UartSctSend(UART_CC_REL,0,0);
			veriPckSendCallback = NULL;	
			relCmdCnt = 0;
		}
		break;
	}

}
#endif

#ifndef SEND_UNIFIED
LINKMODE_DEFINE void linkedNoneCoordinatorProc(macRx_t *payload)
{
	static uint8 relCmdCnt=0; 
		
	switch(payload->msdu.p[0])
	{
	
		case 0x92:
		veriPckSendCallback = &LinkModeResp;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);	
		pConfig->isRflsCoordinator = RFLS_COORDINATOR_S;
		break;

		case 0x96:
		pConfig->myAddr = MAKEWORD(payload->msdu.p[1],payload->msdu.p[2]);
		MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
		veriPckSendCallback = NULL;
		
		break;

		case 0x9B:
		veriPckSendCallback = &ReConfirm;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 5);
		break;

		case 0x9A:
		veriPckSendCallback = NULL;	
		osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);		
		break;

		case 0x06:
		if((++relCmdCnt)>=9)
		{
			UartSctSend(UART_CC_REL,0,0);
			veriPckSendCallback = NULL;	
			relCmdCnt = 0;
		}
		break;
	}

}
//macMsg_t *MsgPkt;
#endif

MAC_INTERNAL_API void macRxCompleteCallbackEx(macRx_t *pMsg)
{
	macMsg_t *MsgPkt;
	MsgPkt = (macMsg_t *)osal_msg_allocate( sizeof(macRx_t));
	MsgPkt ->hdEvt.event= MAC_INCOMING_MSG_CMD;
	MsgPkt->macPayload = pMsg;
	if(pMsg->msdu.len)
	{
		osal_msg_send( macEx_taskId, (uint8 *)MsgPkt );		
	}
	//macRxOn();
	//macRxOnRequest();
}

static unsigned char CalcSum(uint8 *rcv, uint8 len)
{
	uint8 i=0,sum=0;
	for(i=0;i<len;i++)
	{
		sum+=rcv[i];
	}
	return sum;
}

static void SetWakeupInt(void)
{
	P0IEN |= 0X20;  // P11 设置为中断方式 
	PICTL |= 0X01; // 下降沿触发   
	IEN1 |= 0X20;   // 允许P0口中断;
	P0   |= 0X20;
	P0IFG = 0x00;   // 初始化中断标志位
	EA = 1;
	
}

static void ClearWakeupInt(void)
{
	IEN1 &= ~0X20; 
}

MAC_INTERNAL_API void SetSleepMode(macRx_t *pMsg)
{
	//SetWakeupInt();
	//SET_POWER_MODE(3);
	halSleep(0);
	ClearWakeupInt();
	asm("LJMP 0x800\n");
	//
}

void LoadGrPara(RfGroup_ArrPara *rfg, unsigned char mode, unsigned char para)
{
	switch(mode)
	{
		case WORKINGMODE_TTL:
			rfg->ttlExpVal = para;
		break;

		case WORKINGMODE_MANU:
			rfg->optCode = para;
		break;

		case WORKINGMODE_MULTI:
			rfg->mulOtpVal = para;
		break;

		case WORKINGMODE_EXTA:
			rfg->extaTmp = para;
		break;

	}
}


static void RfModCheck(uint8 mod)
{
#if 0
	switch(g_isMaster)
	{
		case MSTDEV_MODE_DEV:
		case MSTDEV_MODE_MST:
		case MSTDEV_MODE_HOLD:
			basicRfConfig.panId -= MST_PAN_ID;
		break;

		case MSTDEV_MODE_MLINK:
		case MSTDEV_MODE_SLINK:
			basicRfConfig.panId -= LINKED_PAN_ID;
		break;
	}
#endif
	if(basicRfConfig.panId>=LINKED_PAN_ID)
	{
		basicRfConfig.panId-=LINKED_PAN_ID;
	}else if(basicRfConfig.panId>=LINKED_PAN_ID_AUTO)
	{
		basicRfConfig.panId-=LINKED_PAN_ID_AUTO;
	}else if(basicRfConfig.panId>=MST_PAN_ID)
	{
		basicRfConfig.panId-=MST_PAN_ID;
	}else
	{
		basicRfConfig.panId-=MST_PAN_ID_AUTO;
	}

	switch(mod)
	{

		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
		case CMD_MODE_OFF:
			basicRfConfig.panId += (RF_CHAN_AUTO==pConfig->channel)?MST_PAN_ID_AUTO:MST_PAN_ID;
			macExRxDisable();
		break;
		
		case CMD_MODE_RF_SLA://case MSTDEV_MODE_DEV:
		case CMD_MODE_RF_MST://case MSTDEV_MODE_MST:
		//case MSTDEV_MODE_HOLD:
			basicRfConfig.panId += (RF_CHAN_AUTO==pConfig->channel)?MST_PAN_ID_AUTO:MST_PAN_ID;
			macExRxEnable();
		break;

		//case MSTDEV_MODE_MLINK:
		//case MSTDEV_MODE_SLINK:
		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			basicRfConfig.panId +=(RF_CHAN_AUTO==pConfig->channel)?LINKED_PAN_ID_AUTO:LINKED_PAN_ID;
			macExRxEnable();
		break;
	}

	
}

MAC_DATAREQ_API static void RfModExit(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x000F;
	pTxData[0] = 0x97;
	pTxData[1] = pConfig->myAddr;//0x01;
	if(((CMD_MODE_RF_MST==g_isMaster)||(CMD_MODE_RFLS_MST==g_isMaster)||(CMD_MODE_RFLS_SLA==g_isMaster)||
		(CMD_MODE_RF_SLA==g_isMaster))&&(BRIDGE_MST_NORMAL==g_mstHold))
	{
		pTxData[2] = 0xFE;
	}else
	{
		pTxData[2] = 0x01;
	}
	
	//pTxData[2] = pConfig->totalDev;//0x01;
	g_sendLen = 3;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	//veriPckSendCallback = &LinkedModeRollPoling;
	//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);	
}

MAC_DATAREQ_API static void RfMstExit(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x000F;
	pTxData[0] = 0x97;
	pTxData[1] = pConfig->myAddr;//0x01;
	pTxData[2] = 0xFE;	
	//pTxData[2] = pConfig->totalDev;//0x01;
	g_sendLen = 3;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	veriPckSendCallback = NULL;
	//veriPckSendCallback = &LinkedModeRollPoling;
	//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);	
}

MAC_DATAREQ_API static void ExitAndSetChan_exit(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x000F;
	pTxData[0] = 0x97;
	pTxData[1] = pConfig->myAddr;//0x01;
	pTxData[2] = 0xFE;	
	//pTxData[2] = pConfig->totalDev;//0x01;
	g_sendLen = 3;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);

	//WAIT_RF_SEND_OK();
/*
	if(g_swRfChan==0)
	{
		pConfig->channel = RF_CHAN_AUTO;
	}else
	{
		pConfig->channel = g_swRfChan -1+RF_CHANNEL;
	}

	RfModCheck(g_isMaster);
	macParameterSet();
	veriPckSendCallback = &macNetCheck;
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);
*/

	veriPckSendCallback = &ExitAndSetChan_setCh;
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,1);
	
}

MAC_DATAREQ_API static void ExitAndSetChan_setCh(macRx_t* pck)
{
	if(g_swRfChan==0)
	{
		pConfig->channel = RF_CHAN_AUTO;
	}else
	{
		pConfig->channel = g_swRfChan -1+RF_CHANNEL;
	}
	//osal_memset(pConfig->arrCnt,0,5);
	//LED_INS_UNLINK();
	RfModCheck(g_isMaster);
	macParameterSet();
	veriPckSendCallback = &macNetCheck;
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);

}

MAC_DATAREQ_API static void ExitAndSetRfId(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x000F;
	pTxData[0] = 0x97;
	pTxData[1] = pConfig->myAddr;//0x01;
	pTxData[2] = 0xFE;	
	//pTxData[2] = pConfig->totalDev;//0x01;
	g_sendLen = 3;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);

	if((CMD_MODE_RFLS_MST==g_isMaster)||(CMD_MODE_RFLS_SLA==g_isMaster))
	{
		if(RF_CHAN_AUTO==pConfig->channel)
		{
			basicRfConfig.panId = LINKED_PAN_ID_AUTO+g_swRfId;
		}else
		{
			basicRfConfig.panId = LINKED_PAN_ID+g_swRfId;
		}
	}else
	{
		if(RF_CHAN_AUTO==pConfig->channel)
		{
			basicRfConfig.panId = MST_PAN_ID_AUTO+g_swRfId;
		}else
		{
			basicRfConfig.panId = MST_PAN_ID+g_swRfId;
		}
	}
	macParameterSet();

	//osal_memset(pConfig->arrCnt,0,5);
	//LED_INS_UNLINK();
       //osal_memset(pConfig->idInGroup, 0, int len)

	veriPckSendCallback = &macNetCheck;
	osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,100);
}

static void macDelay(unsigned int de)
{
	//while(de--);
	 uint16 i,j;
	 for(i=de;i>0;i--)
	   for(j=587;j>0;j--);
}

MAC_DATAREQ_API void CmdModeSwitchEx(macRx_t* pck)
{
	g_isMaster = g_swMode;
	//RfModCheck(g_isMaster);
	//macDeleteAllDevice();
	//macParameterSet();
	if((CMD_MODE_RF_MST==g_isMaster)||(CMD_MODE_RF_SLA==g_isMaster)||(CMD_MODE_RFLS_SLA==g_isMaster))
	{
		RfModCheck(g_isMaster);
		macDeleteAllDevice();
		macParameterSet();
		g_netStart = 0;
		pConfig->isRflsCoordinator = RFLS_COORDINATOR_M;
		veriPckSendCallback = &macNetCheck;
	}else if(CMD_MODE_RFLS_MST==g_isMaster)
	{
		//macParameterSet();
		//if(RFLS_COORDINATOR_M==pConfig->isRflsCoordinator)
		if(!g_mstHold)
		{
#ifdef SEND_UNIFIED
			veriPckSendCallback=&macRollPolingProc;
#else
			veriPckSendCallback=&LinkedModeRollPoling;
#endif
		}else
		{
			veriPckSendCallback=NULL;
		}
	}
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 100);
}

MAC_DATAREQ_API void CmdModeSwitch(macRx_t* pck)
{
	if((CMD_MODE_RF_MST==g_isMaster)||(CMD_MODE_RF_SLA==g_isMaster)||(CMD_MODE_RFLS_MST==g_isMaster))
	{
		RfModExit(callbackPara);
	}else if(CMD_MODE_RFLS_SLA==g_isMaster)
	{
		RflsStart(callbackPara);
	}
	veriPckSendCallback = &CmdModeSwitchEx;
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 2+(osal_rand()&0x03));
	//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 50);

#if 0
	//macDelay(20);
	//while(!(RFIRQF1 & IRQ_TXDONE) );
	g_isMaster = g_swMode;
	RfModCheck(g_isMaster);
	macDeleteAllDevice();
	macParameterSet();
	if((CMD_MODE_RF_MST==g_isMaster)||(CMD_MODE_RF_SLA==g_isMaster)||(CMD_MODE_RFLS_SLA==g_isMaster))
	{
		g_netStart = 0;
		pConfig->isRflsCoordinator = RFLS_COORDINATOR_M;
		veriPckSendCallback = &macNetCheck;
	}else if(CMD_MODE_RFLS_MST==g_isMaster)
	{
		if(RFLS_COORDINATOR_M==pConfig->isRflsCoordinator)
		{
#ifdef SEND_UNIFIED
			veriPckSendCallback=&macRollPolingProc;
#else
			veriPckSendCallback=&LinkedModeRollPoling;
#endif
		}else
		{
			veriPckSendCallback=NULL;
		}
	}
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 100);
#endif
}

MAC_INTERNAL_API static void macExUartRcvHandle(uint8 *rcv,uint8 len)
{
	unsigned char veri=0;
	unsigned char i=0;
	veri = CalcSum(rcv,len-1);
	//FingerIndex();
	//uint8 *p;
	//veri=rcv[0]+rcv[1]+rcv[2];
	//if(veri!=rcv[3])
	if(veri!=rcv[len-1])
	{
		veri = 0;
		return;
	}

	while(i<(len-1))
	{
		switch(rcv[i])
		{
			case UART_ENCODE_RFID:
				//pConfig->panId = MAKEWORD(rcv[i+1], rcv[i+2]);
				//if((MSTDEV_MODE_MLINK==g_isMaster)||(MSTDEV_MODE_SLINK==g_isMaster))
				if((CMD_MODE_RFLS_MST==g_isMaster)||(CMD_MODE_RFLS_SLA==g_isMaster))
				{
					if(RF_CHAN_AUTO==pConfig->channel)
					{
						basicRfConfig.panId = LINKED_PAN_ID_AUTO+MAKEWORD(rcv[i+1], rcv[i+2]);
					}else
					{
						basicRfConfig.panId = LINKED_PAN_ID+MAKEWORD(rcv[i+1], rcv[i+2]);
					}
				}else
				{
					if(RF_CHAN_AUTO==pConfig->channel)
					{
						basicRfConfig.panId = MST_PAN_ID_AUTO+MAKEWORD(rcv[i+1], rcv[i+2]);
					}else
					{
						basicRfConfig.panId = MST_PAN_ID+MAKEWORD(rcv[i+1], rcv[i+2]);
					}
				}
				macParameterSet();
				i+=3;
			break;

			case UART_ENCODE_RFIDEX:
				g_swRfId = MAKEWORD(rcv[i+1], rcv[i+2]);
				veriPckSendCallback = &ExitAndSetRfId;
				//osal_memset(pConfig->arrCnt,0,5);
				//LED_INS_UNLINK();
				macDeleteAllDevice();
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);	
				i+=3;
			break;

			case UART_ENCODE_RFMOD:
/*
				//g_isMaster = rcv[i+1];
				RfModCheck(rcv[i+1]);
				g_isMaster = rcv[i+1];
				macDeleteAllDevice();
				macParameterSet();
				if(CMD_MODE_RFLS_MST==g_isMaster)
				{
					veriPckSendCallback = &RflsStart;
				}else
				{
					g_netStart = 0;
					pConfig->isRflsCoordinator = RFLS_COORDINATOR_M;
					veriPckSendCallback = &macNetCheck;
				}
*/
				if(!g_initCheck)
				{
					g_isMaster = rcv[i+1];;
					RfModCheck(g_isMaster);
					macDeleteAllDevice();
					macParameterSet();
					veriPckSendCallback = &macNetCheck;
					g_initCheck = 1;
				}else
				{
					g_swMode = rcv[i+1];
					veriPckSendCallback = &CmdModeSwitch;
				}
/*
				if(CMD_MODE_RFLS_SLA==g_isMaster)
				{
					pConfig->isRflsCoordinator = RFLS_COORDINATOR_M;
				}
				if(CMD_MODE_RFLS_MST==g_isMaster)
				{
					veriPckSendCallback = &RflsStart;	
				}else
				{				
					veriPckSendCallback = &macNetCheck;		
				}				
*/
				//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,20);	
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,3);
				//macNetCheck(callbackPara);
				i+=3;
			break;

			case UART_ENCODE_RFCH:
				if(EXIT_BEFORE_SET==rcv[i+2])
				{
					g_swRfChan = rcv[i+1];
					//osal_memset(pConfig->arrCnt,0,5);
					//LED_INS_UNLINK();
					macDeleteAllDevice();
					veriPckSendCallback = &ExitAndSetChan_exit;
					//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,3);
				}else
				{
					if(rcv[i+1]==0)
					{
						pConfig->channel = RF_CHAN_AUTO;
					}else
					{
						pConfig->channel = rcv[i+1]-1+RF_CHANNEL;
					}
					//osal_memset(pConfig->arrCnt,0,5);
					//LED_INS_UNLINK();
					macDeleteAllDevice();
					RfModCheck(g_isMaster);
					macParameterSet();
					veriPckSendCallback = &macNetCheck;
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,3);
					//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);
				}
				i+=3;
			break;

			case UART_ENCODE_RFADDR:
				//osal_memcpy(pConfig->extAddr, &rcv[i+1], 8);
				//macParameterSet();
				i+=9;
			break;
		
			case UART_ENCODE_APERTSHUTER:
				radioSet.shutter = rcv[i+1];
				radioSet.apert = rcv[i+2];
				i+=3;
			break;

			case UART_ENCODE_ISOCMP:
				radioSet.isoCmp = rcv[i+1];
				radioSet.adept = rcv[i+2];
				i+=3;
			break;

			case UART_ENCODE_RFPREARR:
				radioSet.workMode = (rcv[i+1]&0x07);			//(mainPara.synMode<<3)|(mainPara.workMode&0x07)
				radioSet.synMod = (rcv[i+1]>>3)&0x03;
				radioSet.preflashIndex = rcv[i+2];
				veriPckSendCallback = &PreFlashInfo;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				i+=3;
			break;

			case UART_ENCODE_RFCOUNTDOWN:
				g_msTick = MAKEWORD(rcv[i+1], rcv[i+2]);
				//veriPckSendCallback = &CountDown;
				CountDown(NULL);
				//veriPckSendCallback = &CountDown;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,300);	
				//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,1000);	
				i+=3;
			break;

			case UART_ENCODE_RFRECOVER:
				if(g_mstHold == BRIDGE_MST_NORMAL)
				{
					veriPckSendCallback = &macRollPolingProc;
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,900);	
				}
				i+=3;
			break;

#if 0
			case UART_ENCODE_RFa:
				radioSet.rfGr_arrPara[RF_GROUP_A].mode = rcv[i+1];
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_A],rcv[i+1], rcv[i+2]);
				i+=3;
			break;

			case UART_ENCODE_RFb:
				radioSet.rfGr_arrPara[RF_GROUP_B].mode = rcv[i+1];
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_B],rcv[i+1], rcv[i+2]);
				i+=3;
			break;

			case UART_ENCODE_RFc:
				radioSet.rfGr_arrPara[RF_GROUP_C].mode = rcv[i+1];
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_C],rcv[i+1], rcv[i+2]);
				i+=3;
			break;

			case UART_ENCODE_RFd:
				radioSet.rfGr_arrPara[RF_GROUP_D].mode = rcv[i+1];
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_D],rcv[i+1], rcv[i+2]);
				i+=3;
			break;

			case UART_ENCODE_RFe:
				radioSet.rfGr_arrPara[RF_GROUP_E].mode = rcv[i+1];
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_E],rcv[i+1], rcv[i+2]);
				i+=3;
			break;
#endif

			case UART_ENCODE_GRM:
				radioSet.rfGr_arrPara[RF_GROUP_A].mode = (rcv[i+1]>>4);
				radioSet.rfGr_arrPara[RF_GROUP_B].mode = (rcv[i+1]&0x0F);
				radioSet.rfGr_arrPara[RF_GROUP_C].mode = (rcv[i+2]>>4);
				radioSet.rfGr_arrPara[RF_GROUP_D].mode = (rcv[i+2]&0x0F);
				radioSet.rfGr_arrPara[RF_GROUP_E].mode = (rcv[i+3]>>4);
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_A],radioSet.rfGr_arrPara[RF_GROUP_A].mode, rcv[i+4]);
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_B],radioSet.rfGr_arrPara[RF_GROUP_B].mode, rcv[i+5]);
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_C],radioSet.rfGr_arrPara[RF_GROUP_C].mode, rcv[i+6]);
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_D],radioSet.rfGr_arrPara[RF_GROUP_D].mode, rcv[i+7]);
				LoadGrPara(&radioSet.rfGr_arrPara[RF_GROUP_E],radioSet.rfGr_arrPara[RF_GROUP_E].mode, rcv[i+8]);
				i+=9;
			break;

			case UART_ENCODE_RFCMD:
				switch(rcv[i+1])
				{
					case UART_ENCODE_RFCMD_FLA:
						veriPckSendCallback = &FlashConfirm;
						radioSet.workMode = rcv[i+2]&0x07;
						radioSet.synMod = (rcv[i+2]>>3)&0x03;
						radioSet.hpFlash = (rcv[i+2]>>5)&0x01;
						radioSet.fPilot = (rcv[i+2]>>6)&0x01;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
					break;

					case UART_ENCODE_RFCMD_PILOT:
						//veriPckSendCallback = &Pilot;
						//radioSet.workMode = rcv[i+2];
						//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);	
					break;

					case UART_ENCODE_RFCMD_REL:
						veriPckSendCallback = &LinkedShotRel;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,30);	
					break;

					case UART_ENCODE_RFCMD_SLEEP:
						//SetSleepMode();
						veriPckSendCallback=&SetSleepMode;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,5);
					break;
				}
				i+=3;
			break;

			case UART_ENCODE_RFMUL:
				radioSet.mul_times = rcv[i+1];
				radioSet.mul_frq = rcv[i+2];
				i+=3;
			break;

			case UART_ENCODE_EXIT:
				if((CMD_MODE_RF_MST==g_isMaster)||(CMD_MODE_RFLS_MST==g_isMaster)||(CMD_MODE_RFLS_SLA==g_isMaster)||
				(CMD_MODE_RF_SLA==g_isMaster))
				{
				g_isMaster = CMD_MODE_OFF;
				veriPckSendCallback = &RfMstExit;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,(osal_rand()&0x07));
				}
				i+=3;
			break;

			case UART_ENCODE_PILOT:
#if 1
				if(rcv[i+1])
				{
					veriPckSendCallback = &Pilot;
					radioSet.workMode = rcv[i+2];
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);	
				}
				i+=3;
#else
				g_msTick = 7000;
				veriPckSendCallback = &CountDown;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,30);
				i+=3;
#endif
			break;

			case UART_ENCODE_CHSCAN:
				veriPckSendCallback = &ChanScan;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,50);
				i+=3;
			break;

			case UART_ENCODE_RFFLA:
				radioSet.adept = rcv[i+1];
				veriPckSendCallback = &FlashConfirm;
				radioSet.workMode = rcv[i+2]&0x07;
				radioSet.synMod = (rcv[i+2]>>3)&0x03;
				radioSet.hpFlash = (rcv[i+2]>>5)&0x01;
				radioSet.fPilot = (rcv[i+2]>>6)&0x01;
				osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				i+=3;
			break;

			

			


#if 0
			case UART_ENCODE_RADIO:
			radioSet.ratio = (rcv[i+1]>>4)&0x0f;	
			radioSet.cmdSta = (rcv[i+1]&0x0f);
			switch(radioSet.cmdSta)
			{
				case RADIO_STATUE_PRE:
					//FingerIndex();
					if(!rcv[i+2])
					{
						veriPckSendCallback = &PreFlashInfo;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
					}else
					{
						veriPckSendCallback = &CountDown;
						g_msTick = 7000;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
					}
				break;

				case RADIO_STATUE_FLACFM:
					radioSet.arrExpVal[0] = rcv[i+2];
					flashCmd[9] = flashCmd[12] = flashCmd[15] = rcv[i+2];
					veriPckSendCallback = &FlashConfirm;
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				break;

				case RADIO_STATUE_FLASH:
					g_msTick = 6000;
					veriPckSendCallback = &CountDown;		
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				break;

				case RADIO_STATUE_RECOVER:
					veriPckSendCallback = &macRollPolingProc;
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				break;
			}
			break;
#endif

			default:
			i++;				//this keep the loop jump out
			break; 
		}		
		//i+=3;
	}

#if 0	
	switch(rcv[0])
	{
		case UART_ENCODE_RADIO:
			radioSet.ratio = (rcv[1]>>4)&0x0f;	
			radioSet.cmdSta = (rcv[1]&0x0f);
			switch(radioSet.cmdSta)
			{
				case RADIO_STATUE_PRE:
					FingerIndex();
					if(!rcv[2])
					{
						veriPckSendCallback = &PreFlashInfo;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
					}else
					{
						veriPckSendCallback = &CountDown;
						g_msTick = 7000;
						osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
					}
				break;

				case RADIO_STATUE_FLACFM:
					radioSet.arrExpVal[0] = rcv[2];
					flashCmd[9] = flashCmd[12] = flashCmd[15] = rcv[2];
					veriPckSendCallback = &FlashConfirm;
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				break;

				case RADIO_STATUE_FLASH:
					g_msTick = 6000;
					veriPckSendCallback = &CountDown;		
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				break;

				case RADIO_STATUE_RECOVER:
					veriPckSendCallback = &macRollPolingProc;
					osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
				break;
			}
		break;

		default:
		break;
	}
#endif
}

//unsigned char veriSum=0;
static void macEx_UartProcessData(uint8 port, uint8 event)
{
	uint8  ch;
	static uint8 loaden=0;
	static uint8 rcvLen=0;
	static uint16 bufIndex=0;

	//HalUARTRead (port, &ch, 1);

#if((defined HAL_UART_DMA)&&(HAL_UART_DMA==1))
	//uint16 rxBufLen  = Hal_UART_RxBufLen(0);
	//static uint8 rcvLen=0;
	uint16 rxBufLen;
	if(!loaden)
	{
		if(Hal_UART_RxBufLen(0)<2)
		{
			return;
		}
		HalUARTRead (port, &ch, 1);
		if(0xaa==ch)
		{
			//loaden = 1;
			HalUARTRead(port, &rcvLen, 1);
			rxBufLen = Hal_UART_RxBufLen(0);
			if(rcvLen>UART_RCVBUF_LEN)
			{
				return;
			}else if(rcvLen<=rxBufLen)
			{
				HalUARTRead(port,rcvBuf,rcvLen);
				macExUartRcvHandle(rcvBuf,rcvLen);
			}else
			{
				HalUARTRead(port,rcvBuf,rxBufLen);
				rcvLen-=rxBufLen;
				bufIndex+=rxBufLen;
				loaden = 1;
			}
			
		}else if(0xAB==ch)
		{
			g_msTick = 0;
			CountDown(NULL);
		}
	}else
	{
		rxBufLen = Hal_UART_RxBufLen(0);
		if(rcvLen<=rxBufLen)
		{
			HalUARTRead(port,(rcvBuf+bufIndex),rcvLen);
			macExUartRcvHandle(rcvBuf,rcvLen+bufIndex);
			bufIndex=0;
			rcvLen=0;
			loaden=0;
		}else
		{
			HalUARTRead(port,(rcvBuf+bufIndex),rxBufLen);
			rcvLen-=rxBufLen;
			bufIndex+=rxBufLen;
		}
	}
#else
	//static uint8 rcvLen=0;
	HalUARTRead (port, &ch, 1);
	//FingerIndex_2();

	if(!loaden)
	{
		if(0xaa==ch)
		{
			loaden = 1;
			//FingerIndex_1();
		}else if(0xAB==ch)
		{
			g_msTick = 0;
			CountDown(NULL);
		}
	}else if(1==loaden)
	{
		rcvLen = ch;
              bufIndex = 0;
		if(rcvLen>UART_RCVBUF_LEN)
		{
			rcvLen = 0;
			loaden = 0;
			return;
		}
		loaden = 2;
		
	}else if(2==loaden)
	{
		if(bufIndex<(rcvLen-1))
		{
			rcvBuf[bufIndex++] = ch;
			//FingerIndex_2();
		}else
		{
			rcvBuf[bufIndex++] = ch;
			macExUartRcvHandle(rcvBuf,rcvLen);
			rcvLen = 0;
			loaden = 0;
			bufIndex = 0;
			//FingerIndex_1();
		}
	}
#endif
	
/*
	if(loaden)
	{
		rcvBuf[bufIndex++] =  ch;
		if(bufIndex>=4)
		{
			//veriSum = (rcvBuf[0]+rcvBuf[1]+rcvBuf[2]);
			macExUartRcvHandle(rcvBuf);
			bufIndex = 0;
			loaden = 0;
			//return;
		}
	}else if(0xaa==ch)
	{
		loaden = 1;
	}
*/

}

static void macExUartInit (void)
{
	halUARTCfg_t uartConfig;

	/* Initialize APP ID */
	//App_TaskID = 0;

	/* UART Configuration */
	uartConfig.configured           = TRUE;
	uartConfig.baudRate             = MACEX_UART_DEFAULT_BAUDRATE;
	uartConfig.flowControl          = MACEX_UART_DEFAULT_OVERFLOW;
	uartConfig.flowControlThreshold = MACEX_UART_DEFAULT_THRESHOLD;
	uartConfig.rx.maxBufSize        = MACEX_UART_DEFAULT_MAX_RX_BUFF;
	uartConfig.tx.maxBufSize        = MACEX_UART_DEFAULT_MAX_TX_BUFF;
	uartConfig.idleTimeout          = MACEX_UART_DEFAULT_IDLE_TIMEOUT;
	uartConfig.intEnable            = TRUE;
	uartConfig.callBackFunc         = macEx_UartProcessData;

	/* Start UART */
	HalUARTOpen (MACEX_PORT, &uartConfig);

#ifdef IO_INDEX_TEST
	P1DIR |= 0x0C;
	FINGER1 = 0;
	FINGER2 = 0;
#endif
	//HalUARTWrite(0,"UART START\n",sizeof("UART START\n"));	
	//HalUARTWrite(0,"lucky day 20130806\r\n",sizeof("lucky day 20130806\r\n"));
       //HalUARTWrite(0,"xiao xin, dad love you\r\n",sizeof("xiao xin, dad love you\r\n"));	
	//HalUARTWrite(0,"power20140218\r\n",sizeof("power20140218\r\n"));
	//HalUARTWrite(0,"Boot test 0720-1\r\n",sizeof("Boot test 0720-1\r\n"));
	HalUARTWrite(0,"module test\r\n",sizeof("module test\r\n"));
	
}

static uint8 LoadUartSendBuf(unsigned char id, unsigned char pa1, unsigned char pa2)
{
	if(0==gBUartTotalByte)
	{
		gBUartBuf[gBUartTotalByte++] = 0xAA;
		gBUartBuf[gBUartTotalByte++] = 0;			//len
	}
	gBUartBuf[gBUartTotalByte++] = id;
	gBUartBuf[gBUartTotalByte++] = pa1;
	gBUartBuf[gBUartTotalByte++] = pa2;
#ifdef DEBUG_TRACE
	Printf("\r\n");
#endif
	return gBUartTotalByte;		
}

static uint8 LoadUartArray(uint8 id, uint8 *pBuf, uint8 len)
{
	if(0==gBUartTotalByte)
	{
		gBUartBuf[gBUartTotalByte++] = 0xAA;
		gBUartBuf[gBUartTotalByte++] = len;			//len
	}
	gBUartBuf[gBUartTotalByte++] = id;
	osal_memcpy(gBUartBuf+gBUartTotalByte,pBuf,len);
	gBUartTotalByte+=len;
	return gBUartTotalByte;
	
}

static void TrigerUartSend(void)
{
	char i=0;
	unsigned char sum=0;
	for(i=2;i<gBUartTotalByte;i++)
	{
		sum+= gBUartBuf[i];
	}
	gBUartBuf[gBUartTotalByte++] = sum;
	gBUartBuf[1] = (gBUartTotalByte-2);
	uart_tran_flag = 1;

	HalUARTWrite(0,gBUartBuf,gBUartTotalByte);

	uart_tran_flag = 0;

	gBUartTotalByte = 0;
	
	
}

static uint8 UartSctSend(unsigned char id, unsigned char pa1, unsigned char pa2)
{
	while(uart_tran_flag);
	LoadUartSendBuf(id, pa1,pa2);
	TrigerUartSend();	
        return 1;
}



#define PARA_GR_A 8
#define PARA_GR_B (PARA_GR_A+3)
#define PARA_GR_C (PARA_GR_B+3)
#define PARA_GR_D (PARA_GR_C+3)
#define PARA_GR_E (PARA_GR_D+3)
void BuildPreFlashInfoData(uint8 *buf, uint8 *len)
{
	uint8 loadIndex=0;
	uint8 i=0;
	//g_sendLen = 0;
	osal_memcpy(buf,preCmdBuf,4);					//the fixed head datas 0x17 00 ff 47
	loadIndex+= 4;
	buf[loadIndex++] = radioSet.shutter;
	buf[loadIndex++] = radioSet.apert;
	buf[loadIndex++] = radioSet.isoCmp;
	buf[loadIndex++] = 0x04;							//fixed?
	for(i=0;i<5;i++)
	{
		buf[loadIndex++] = 0x01;
		buf[loadIndex++] = 0x00;
		buf[loadIndex++] = 0x00;
	}
	switch(radioSet.preflashIndex)
	{
		case 0:
			for(i=0;i<3;i++)
			{
				buf[PARA_GR_A+i*3] = 0x01;
				buf[PARA_GR_A+i*3+1] = 0xA0;
				buf[PARA_GR_A+i*3+2] = 0x04;
			}
		break;

		case 1:
			buf[PARA_GR_A] = 0x01;
			buf[PARA_GR_A+1] = 0xA0;
			buf[PARA_GR_A+2] = 0x04;
		break;

		case 2:
			buf[PARA_GR_B] = 0x01;
			buf[PARA_GR_B+1] = 0xA0;
			buf[PARA_GR_B+2] = 0x04;
		break;

		case 3:
			buf[PARA_GR_C] = 0x01;
			buf[PARA_GR_C+1] = 0xA0;
			buf[PARA_GR_C+2] = 0x04;
		break;

		case 4:
			buf[PARA_GR_D] = 0x01;
			buf[PARA_GR_D+1] = 0xA0;
			buf[PARA_GR_D+2] = 0x04;
		break;

		case 5:
			buf[PARA_GR_E] = 0x01;
			buf[PARA_GR_E+1] = 0xA0;
			buf[PARA_GR_E+2] = 0x04;
		break;
	}
	 *len = loadIndex;	
	
}

unsigned char RfMcodeCovert(unsigned char src, uint8 hpMod)
{
	if(HP_FLASHMODE_OFF==hpMod)
	{
		src = 0x38 -src;
		return (src + 0x88);
	}else
	{
		src = 0x38 -src;
		return (src + 0x68);
	}
}

void BuildFlashInfoData(uint8 *buf, uint8 *len)
{
	uint8 loadIndex=0; 
	uint8 i=0;
	
	if(WORKINGMODE_MULTI==radioSet.workMode)
	{
		osal_memcpy(buf,multiCmd,4);
		loadIndex+= 4;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.shutter:0xFF;
		//buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.shutter:0x38;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.apert:0;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.isoCmp:0x48;

		for(i=0;i<3;i++)
		{
			if(WORKINGMODE_OFF==radioSet.rfGr_arrPara[i].mode)
			{
				buf[loadIndex++] = 0x00;
				buf[loadIndex++] = 0x00;
			}else
			{
				buf[loadIndex++] = RfMcodeCovert(radioSet.rfGr_arrPara[i].mulOtpVal,radioSet.hpFlash);
				buf[loadIndex++] = 0x02;
			}
		}
		//buf[loadIndex++] = radioSet.mul_times;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.mul_times:(radioSet.mul_times>0?radioSet.mul_times:0x18);
		buf[loadIndex++] = radioSet.mul_frq;
		
	}else
	{
		osal_memcpy(buf,preCmdBuf,4);
		loadIndex+= 4;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.shutter:0;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.apert:0;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.isoCmp:0x48;
		//buf[loadIndex++] = radioSet.synMod==SYNC_MODE_FP?0x48:0x04;//0x04;
		buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?((radioSet.hpFlash==HP_FLASHMODE_ON)?0x48:0x04):0x0A;
		
		for(i=0;i<RF_GROUP_TOTAL;i++)
		{
			switch(radioSet.rfGr_arrPara[i].mode)
			{
				case WORKINGMODE_TTL:
					buf[loadIndex++] = 0x01;
					buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?radioSet.rfGr_arrPara[i].ttlExpVal:0x98;
					//buf[loadIndex++] = radioSet.synMod==SYNC_MODE_FP?0x06:0x03;
					buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?((radioSet.hpFlash==HP_FLASHMODE_ON)?0x06:0x03):0x08;
				break;

				case WORKINGMODE_MANU:
					buf[loadIndex++] = 0x02;
					buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?RfMcodeCovert(radioSet.rfGr_arrPara[i].optCode,radioSet.hpFlash):RfMcodeCovert(radioSet.rfGr_arrPara[i].optCode,HP_FLASHMODE_OFF);//radioSet.rfGr_arrPara[i].optCode;
					//buf[loadIndex++] =  radioSet.synMod==SYNC_MODE_FP?0x05:0x02;
					buf[loadIndex++] = (FLASH_PILOT_OFF==radioSet.fPilot)?((radioSet.hpFlash==HP_FLASHMODE_ON)?0x05:0x02):0x02;
				break;

				case WORKINGMODE_EXTA:
					buf[loadIndex++] = 0x08;
					buf[loadIndex++] = radioSet.rfGr_arrPara[i].extaTmp;
					buf[loadIndex++] = 0x02;
				break;

				case WORKINGMODE_OFF:
					if(WORKINGMODE_GR==radioSet.workMode)
					{
						buf[loadIndex++] =0x00;
						buf[loadIndex++] =0x00;
						buf[loadIndex++] =0x00;
					}else if(WORKINGMODE_TTL==radioSet.workMode)
					{
						buf[loadIndex++] =0x01;
						buf[loadIndex++] =0x00;
						buf[loadIndex++] =0x00;
					}else if(WORKINGMODE_MANU==radioSet.workMode)
					{
						buf[loadIndex++] =0x01;
						buf[loadIndex++] =0x00;
						buf[loadIndex++] =0x00;
					}
				break;
			}
		}

	}
	
	*len = loadIndex;	
	
}

void BuildPilotInfoData(uint8 * buf, uint8 * len)
{
	uint8 loadIndex=0; 
	uint8 i=0;
	osal_memcpy(buf,preCmdBuf,4);
	loadIndex+=4;
	buf[loadIndex++]=0;
	buf[loadIndex++]=0;
	buf[loadIndex++]=0x48;
	buf[loadIndex++]=0x0A;
	
	switch(g_modelingStyle)
	{
		case MODELING_STYLE_APERT:
			if(radioSet.workMode==WORKINGMODE_GR)
			{
				for(i=0;i<RF_GROUP_TOTAL;i++)
				{
					buf[loadIndex++] = 0x01;
					buf[loadIndex++] = 0x98;
					buf[loadIndex++] = 0x08;					
				}
			}else
			{
				for(i=0;i<3;i++)
				{
					buf[loadIndex++] = 0x01;
					buf[loadIndex++] = 0x98;
					buf[loadIndex++] = 0x08;					
				}
				osal_memset(buf+loadIndex,0x00,6);
			}
		break;

		case MODELING_STYLE_STEX:
		case MODELING_STYLE_BOTH:
			if(radioSet.workMode==WORKINGMODE_GR)
			{
				for(i=0;i<RF_GROUP_TOTAL;i++)
				{
					buf[loadIndex++] = 0x06;
					buf[loadIndex++] = 0x98;
					buf[loadIndex++] = 0x07;					
				}
			}else
			{
				for(i=0;i<RF_GROUP_TOTAL;i++)
				{
					buf[loadIndex++] = 0x06;
					buf[loadIndex++] = i>2?0x00:0x98;
					buf[loadIndex++] = i>2?0x00:0x07;					
				}
				
			}
		break;

		case MODELING_STYLE_OFF:			//inspire it later, should analysize respectively
			if(radioSet.workMode==WORKINGMODE_GR)
			{
				for(i=0;i<RF_GROUP_TOTAL;i++)
				{
					buf[loadIndex++] = 0x01;
					buf[loadIndex++] = 0x98;
					buf[loadIndex++] = 0x08;					
				}
			}else
			{
				for(i=0;i<3;i++)
				{
					buf[loadIndex++] = 0x01;
					buf[loadIndex++] = 0x98;
					buf[loadIndex++] = 0x08;					
				}
				osal_memset(buf+loadIndex,0x00,6);
			}
		break;
	}

	*len = loadIndex;	

}



MAC_DATAREQ_API void PreFlashInfo(macRx_t* pck)
{
	static uint8 flaCmdCnt=0;
	//if(flaCmdCnt++<8)
	if(flaCmdCnt++<radioSet.adept)
	{
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//osal_memcpy(pTxData,preCmdBuf,sizeof(preCmdBuf));
		//g_sendLen = sizeof(preCmdBuf);
		BuildPreFlashInfoData(pTxData,&g_sendLen);
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		veriPckSendCallback = &PreFlashInfo;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,2);		
	}else
	{
		flaCmdCnt = 0;
		veriPckSendCallback = NULL;
	}
}

MAC_DATAREQ_API void CountDown(macRx_t* pck)
{
#if 0
	if(g_msTick>=0)
	{
		//
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//arrcpy(pTxData,preCmdBuf,sizeof(preCmdBuf));
		pTxData[0] = 0x90;
		pTxData[1] = 0x00;//0x01;
		pTxData[2] = g_msTick>>8;
		pTxData[3] = g_msTick&0xFF;
		g_sendLen = 4;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		g_msTick-=1000;
		veriPckSendCallback = &CountDown;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,1);	
	}else
	{
		g_msTick = 0;
		 veriPckSendCallback=&macRollPolingProc;
		 osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 1000);
		//veriPckSendCallback = NULL;
		
	}
#endif
	while(g_msTick>=0)
	{
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//arrcpy(pTxData,preCmdBuf,sizeof(preCmdBuf));
		pTxData[0] = 0x90;
		pTxData[1] = 0x00;//0x01;
		pTxData[2] = g_msTick>>8;
		pTxData[3] = g_msTick&0xFF;
		g_sendLen = 4;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
#if 1
		if(g_msTick>0)
		{
			txState.txSeqNumber--;
		}
#endif

		g_msTick-=1000;

		//veriPckSendCallback = NULL;
		//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,0);	
	}
	if(BRIDGE_MST_NORMAL==g_mstHold)
	{
#ifdef SEND_UNIFIED
		veriPckSendCallback=&macRollPolingProc;
#else
		if(pConfig->mstRcd>0)
		{
			veriPckSendCallback=&LinkedModeRollPoling;
		}else
		{
			veriPckSendCallback=&macRollPolingProc;
		}
#endif
	}else
	{
		veriPckSendCallback = NULL;
	}
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 300);
	//osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 2000);
	
	
}

MAC_DATAREQ_API void FlashConfirm(macRx_t* pck)
{
	static uint8 flaCmdCnt=0;
	//if(flaCmdCnt++<8)
	if(flaCmdCnt++<radioSet.adept)
	{
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//osal_memcpy(pTxData,flashCmd,sizeof(flashCmd));
		//g_sendLen = sizeof(flashCmd);
		BuildFlashInfoData(pTxData, &g_sendLen);
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		veriPckSendCallback = &FlashConfirm;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,2);	
	}else
	{
		flaCmdCnt = 0;
		veriPckSendCallback = NULL;
	}
}

MAC_DATAREQ_API void Pilot(macRx_t* pck)
{
	static uint8 pilotCmdCnt=0;
	if(pilotCmdCnt++<2)
	{
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		BuildPilotInfoData(pTxData, &g_sendLen);
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		veriPckSendCallback = &Pilot;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,3);	
	}else
	{
		pilotCmdCnt = 0;
		g_msTick = 1000;
		veriPckSendCallback = &CountDown;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,10);	
	}
}

static void ScanDelay(unsigned int de)
{
	while(de--);
}

static uint8 InqureAutoChan(uint8 *rssiVal)
{
	uint8 ich;
	uint8 rtCh=0;
	for(ich=0;ich<15;ich++)
	{
		if(rssiVal[rtCh]<=rssiVal[ich])
		{
			rtCh = ich;
		}
	}
	return (rtCh+RF_CHANNEL);
}

static void ChanScanRollPoling(void)
{
	uint8 sdCnt=0;
	uint8 cn=0;
	switch(g_isMaster)
	{
		//case MSTDEV_MODE_DEV:
		case CMD_MODE_RF_SLA:
		break;

		case CMD_MODE_RF_MST:
		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//pConfig->myAddr = 0x000F;
		pTxData[sdCnt++] = 0x9A;
		pTxData[sdCnt++] = pConfig->evtCnt;//0x01;
		pTxData[sdCnt++] = pConfig->totalDev;//0x01;
		pTxData[sdCnt++] = pConfig->mstRcd+1;//0x01;
		pTxData[sdCnt++] = ((1<<pConfig->slvRcd)-1)&0xFF;//(1<<pTxData[2])-1;//0x01;
		pTxData[sdCnt++] = 0x00;
		pTxData[sdCnt++] = ((1<<pConfig->slvRcd)-1)&0xFF;//pConfig->slvRcd;
		pTxData[sdCnt++] = (0xFF00>>(pConfig->mstRcd+1))&0xFF;
		for(cn=0;cn<pConfig->mstRcd;cn++)
		{
			pTxData[sdCnt++] = 0x0E-cn;
		}
		//pTxData[sdCnt++] = 0x80;
		//g_sendLen = 8;
		macRfSendPacket(pConfig->destAddr, pTxData, sdCnt,0);
		break;

		case CMD_MODE_OFF:
		break;

	}

}


static void CalcEachRssi(int8 *valBuf, uint16 tms, uint16 dly)
{
	uint16 chRsv;
	int8 rssiSum=0;
	uint16 panIdRsv=0;
	uint16 tm=0;
	int8 rssiTmp=0;
	//int16 randVer=0;
	uint8 bufIndex=0;
	//uint8 rssiDbm;
	uint8 rollCnt=0;
	uint16 rollChRsv=0;

	//ChanScanRollPoling();

	chRsv = pConfig->channel;
	panIdRsv = pConfig->panId;
	
	pConfig->channel = RF_CHANNEL;
	pConfig->panId = 0xffff;

	while(pConfig->channel<(RF_CHANNEL+15))
	{
		macExRxDisable();
		macParameterSet();
		macExRxEnable();

		rssiSum=0;
		ScanDelay(dly);		

		//NLME_EDScanRequest(1,10);

		for(tm=0;tm<tms;tm++)
		{
#if 0
			while(!(RSSISTAT&0x01));
			rssiTmp = RSSI;
			if(rssiTmp>rssiSum) rssiSum = rssiTmp;
			//rssiSum+=RSSI;
			ScanDelay(dly);	
#else
			macRadioEnergyDetectStart();
			ScanDelay(dly);
			rssiTmp = macRadioEnergyDetectStop();
			//rssiSum = (rssiTmp>rssiSum)?rssiTmp:rssiSum;
			if(rssiTmp>rssiSum) rssiSum = rssiTmp;
#endif
		}

		//rssiDbm = rssiSum + MAC_RADIO_RSSI_OFFSET;
		//MAC_RADIO_RSSI_LNA_OFFSET(rssiDbm);
  		//valBuf[bufIndex++] = radioComputeEDEx(rssiDbm);
		//valBuf[bufIndex++] = (rssiSum/tms);
		/*
		randVer = rssiSum+((int8)MAC_RADIO_RANDOM_BYTE())/10;
		if((randVer>0)&&(randVer<0xFF))
		{
			valBuf[bufIndex++] = 0xFF-randVer;
		}else
		{
			valBuf[bufIndex++] = 0xFF-rssiSum;
		}*/
		if(rssiSum<76)
		{
			valBuf[bufIndex++] = 0xFF-rssiSum*3;			
		}else
		{
			valBuf[bufIndex++] = 25;
		}		
/*
		if(rollCnt++>3)
		{
			rollCnt = 0;
			rollChRsv = pConfig->channel;
			pConfig->channel = chRsv;
			pConfig->panId = panIdRsv;
			//macExRxDisable();
			macParameterSet();
			//macExRxEnable();

			ScanDelay(dly);

			ChanScanRollPoling();

			pConfig->channel = rollChRsv;
			pConfig->panId = 0xFFFF;
			
		}
*/
		
		pConfig->channel++;
		if(BRIDGE_MST_HOLD==g_mstHold)
		{
			osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
		}
		
	}

	pConfig->autoChan = InqureAutoChan(valBuf);

	pConfig->channel = chRsv;
	pConfig->panId = panIdRsv;

	macParameterSet();
	

}

MAC_DATAREQ_API void ChanScan(macRx_t* pck)
{
	//uint16 chRsv;
	//uint16 rssiSum=0;
	//uint8 tm=0;
	int8 rssiBuf[16];
	uint8 regRsv=0;
	//uint8 bufIndex=0;

/*	
	chRsv = pConfig->channel;

	pConfig->channel = RF_CHANNEL;

	while(pConfig->channel<(RF_CHANNEL+15))
	{
		macExRxDisable();
		macParameterSet(pConfig);
		macExRxEnable();

		rssiSum=0;
		ScanDelay(5000);		

		for(tm=0;tm<50;tm++)
		{
			while(!(RSSISTAT&0x01));
			rssiSum+=RSSI;
		}

		prtBuf[bufIndex++] = (rssiSum/50);
		pConfig->channel++;
	}
*/
	//FRMCTRL0 |= 0x10;
	regRsv = FRMCTRL0;
	FRMCTRL0 |= 0x10;
	CalcEachRssi(rssiBuf,300,800);
	 FRMCTRL0 = regRsv;

	//FRMCTRL0 &= 0xEF;

	//pConfig->autoChan = InqureAutoChan(rssiBuf);

	LoadUartArray(UART_CC_RTSCAN,rssiBuf,15);
	TrigerUartSend();

	//macParameterSet();

	//if(RFLS_COORDINATOR_M==pConfig->isRflsCoordinator)
	if(BRIDGE_MST_NORMAL==g_mstHold)
	{
		veriPckSendCallback=&macRollPolingProc;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 50);
	}else
	{
		//osal_clear_event(macEx_taskId,MACEX_LINK_TIMEOUT_EVENT);
		osal_start_timerEx(macEx_taskId, MACEX_LINK_TIMEOUT_EVENT,5000);
		veriPckSendCallback = NULL;
	}
	

	
}

MAC_DATAREQ_API void LinkedShotRel(macRx_t* pck)
{
	static uint8 relCnt=0;
	static uint8 linkRelCnt = 0;
	if(relCnt++<9)
	{
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//pConfig->myAddr = 0x000F;
		pTxData[0] = 0x06;
		pTxData[1] = 0x00;//0x01;
		pTxData[2] = 0xFF;//0x01;
		pTxData[3] = 0x4A;
		pTxData[4] = 0x01;//0x01;
		pTxData[5] = linkRelCnt+1;//0x07;
		g_sendLen = 6;
		macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		//relCnt++;
		osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,7);		
	}else
	{
		relCnt = 0;
		//veriPckSendCallback=&macRollPolingProc;
		linkRelCnt++;
		if(RFLS_COORDINATOR_M==pConfig->isRflsCoordinator)
		{
#ifdef SEND_UNIFIED
			veriPckSendCallback = &macRollPolingProc;
#else
			veriPckSendCallback = &LinkedModeRollPoling;
#endif
		}else
		{
			veriPckSendCallback = NULL;
		}
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 200);	
		
	}
}

MAC_DATAREQ_API void InqureConfirm(macRx_t* pck)
{
	basicRfConfig.ackRequest = 1;
	pConfig->frameHead = 0xc821;
	pConfig->destAddr = 0x000f;
	pConfig->myAddr = 0xFFFF;
	//pConfig->myAddr = ADDR;
	osal_cpyExtAddr(pTxData,g_extAddr);
	pTxData[8]=0x95;
	//pTxData[9]=0x02;
	//if(MSTDEV_MODE_DEV==g_isMaster)
	if(CMD_MODE_RF_SLA==g_isMaster)
	{
		pTxData[9]=basicRfConfig.group+2;
	}else
	{
		pTxData[9]=0x01;
	}
	g_sendLen = 10;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	veriPckSendCallback = NULL;
}

MAC_DATAREQ_API void ReConfirm(macRx_t* pck)
{
	pConfig->frameHead = 0x8861;
	pConfig->destAddr = 0x000f;
	//pConfig->myAddr = ADDR;
	pTxData[0] = 0x9C;
	g_sendLen = 1;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);	
	veriPckSendCallback = NULL;
}

MAC_DATAREQ_API void ReadyInfo(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x0000;
	pTxData[0] = 0x05;
	pTxData[1] = 0x00;
	//if(MSTDEV_MODE_DEV==g_isMaster)
	if(CMD_MODE_RF_SLA==g_isMaster)
	{
		pTxData[2] = 0x20;
		pTxData[3] = pConfig->group;//0x03;
	}else
	{
		pTxData[2] = 0xFF;
		pTxData[3] = 0x33;//0x03;
	}
	pTxData[4] = 0x01;			//this indicate the 
	g_sendLen = 5;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	veriPckSendCallback = NULL;
	//osal_start_timerEx(macEx_taskId, MACEX_VERI_SEND_INQURE,2);	
}

MAC_DATAREQ_API void MstBridgeInfoInqure(macRx_t* pck)
{
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	//pConfig->myAddr = 0x0000;
	pTxData[0] = 0x05;
	pTxData[1] = 0x00;
	//if(MSTDEV_MODE_DEV==g_isMaster)
	if(CMD_MODE_RF_SLA==g_isMaster)
	{
		pTxData[2] = 0x20;
		pTxData[3] = pConfig->group;//0x03;
	}else
	{
		pTxData[2] = 0xFF;
		pTxData[3] = 0x33;//0x03;
	}
	//pTxData[4] = 0x00;			//this indicate the 
	//pTxData[4] = 0x01;
	pTxData[4] = 0x00;
	g_sendLen = 5;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	veriPckSendCallback = NULL;
}

MAC_DATAREQ_API static void devChangeGroup()
{
	pConfig->group++;
	if(pConfig->group>4)
	{
		pConfig->group = 0;
	}
	pConfig->frameHead = 0x8841;
	pConfig->destAddr = 0xFFFF;
	pTxData[0] = 0x04;
	pTxData[1] = 0x00;
	pTxData[2] = 0x31;
	pTxData[3] = pConfig->group;
	g_sendLen = 4;
	macRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);	
	veriPckSendCallback=&ReadyInfo;
	osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 50);	
}

static void macExProcessOSALMsg( macMsg_t *msgPtr )
{
	switch (msgPtr->hdEvt.event)
	{

		case MAC_INCOMING_MSG_CMD:
			switch(g_isMaster)
			{
				//case MSTDEV_MODE_MST:
				case CMD_MODE_RF_MST:
				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					if(BRIDGE_MST_NORMAL==g_mstHold)
					{
						MstVerifyProc(msgPtr->macPayload);
					}else
					{
						MstBridgeHandle(msgPtr->macPayload);
					}
				break;

				//case MSTDEV_MODE_DEV:
				case CMD_MODE_RF_SLA:
					devJoinNetProc(msgPtr->macPayload);
				break;

				//case MSTDEV_MODE_MLINK:
				//case MSTDEV_MODE_SLINK:
				//case CMD_MODE_RFLS_MST:
				//case CMD_MODE_RFLS_SLA:
					//if(RFLS_COORDINATOR_M==pConfig->isRflsCoordinator)
					//{
					//	linkVeriProc(msgPtr->macPayload);
					//}else
					//{
					//	linkedNoneCoordinatorProc(msgPtr->macPayload);
					//}
				//break;

				//case MSTDEV_MODE_HOLD:
                                case CMD_MODE_OFF:
				break;
			}
		break;

		default:
		break;
	}
	MAC_RADIO_FLUSH_RX_FIFO();
}


uint16 macEventLoopEx(uint8 taskId, uint16 events)
{
	  macMsg_t *MSGpkt;
  	  (void)taskId;  // Intentionally unreferenced parameter
  	 // static uint16 tick=0;

	if ( events & SYS_EVENT_MSG )
	{
		MSGpkt = (macMsg_t*)osal_msg_receive(macEx_taskId);
		while ( MSGpkt )
		{
			macExProcessOSALMsg(MSGpkt);

			// Release the memory
			osal_msg_deallocate( (uint8 *)MSGpkt );

			// Next - if one is available
			MSGpkt = (macMsg_t *)osal_msg_receive( macEx_taskId );
		}

		// return unprocessed events
		return (events ^ SYS_EVENT_MSG);
	}

		// Send a message out - This event is generated by a timer
		//  (setup in SampleApp_Init()).
/*
	if ( events & MACEX_ROLL_POLING_EVENT )
	{
		// Send the periodic message
		if(g_netCheckFlag<2)
		{
			macNetCheck();
			g_netCheckFlag++;
			osal_start_timerEx( macEx_taskId, MACEX_ROLL_POLING_EVENT, 30);
		}else
		{
			macRollPolingProc();
		}

		// Setup to send message again in normal periodb 

		// return unprocessed events
		return (events ^ MACEX_ROLL_POLING_EVENT);
	}
*/

	if(events & MACEX_VERI_SEND_INQURE)
	{
		if(NULL!=veriPckSendCallback)
		{
			veriPckSendCallback(callbackPara);
		}
		return (events ^ MACEX_VERI_SEND_INQURE);
	}

	if(events & MACEX_LINK_TIMEOUT_EVENT)
	{
		g_mstHold = BRIDGE_MST_NORMAL;
		pConfig->isRflsCoordinator = RFLS_COORDINATOR_M;
		pConfig->myAddr = 0x000F;
		pConfig->evtCnt++;
		if(pConfig->totalDev>0)
		{
			LED_INS_LINKED();
		}else
		{
			LED_INS_UNLINK();
		}
		MAC_RADIO_SET_SHORT_ADDR(pConfig->myAddr);
		veriPckSendCallback=&macRollPolingProc;
		osal_start_timerEx( macEx_taskId, MACEX_VERI_SEND_INQURE, 0);
		return (events ^ MACEX_LINK_TIMEOUT_EVENT);
	}

#ifdef LED_TOGGLE_DEBUG
	if(events & MACEX_LED_FLASH)
	{
		HAL_TOGGLE_LED2();
		osal_start_timerEx( macEx_taskId, MACEX_LED_FLASH, 500);
		return (events ^ MACEX_LED_FLASH);
	}
#endif

#ifdef DEBUG_TRACE
	if(events & MACEX_INIT_CHECK)
	{
		return (events^MACEX_INIT_CHECK);
	}
#else
	if(events & MACEX_INIT_CHECK)
	{
		if(!g_initCheck)
		{
			UartSctSend(UART_CC_INIT,1,0);
			osal_start_timerEx( macEx_taskId, MACEX_INIT_CHECK, 500);
		}
		return (events^MACEX_INIT_CHECK);
	}
#endif
	//if(tick++>10000)
	//{
	//	LED_RED = !LED_RED;
	//	tick = 0;
	//}

	// Discard unknown events
	return 0;
}



void macExTxDoneCallback(void)
{
	txState.txDone = TXSTATE_TXDONE_CLEAR;
#ifdef _RT_AP_
	AP_TXEN = 0;
#endif
	return;
}

void macExTxAckDoneCallback(void)
{
	txState.ackReceived= TXSTATE_TXACKDONE_RCVD;
#ifdef _RT_AP_
	AP_TXEN = 0;
#endif
	return;
}


#ifdef IO_INDEX_TEST
void FingerIndex_1(void)
{
	FINGER1 = ~FINGER1;
}

void FingerIndex_2(void)
{
	FINGER2 = ~FINGER2;
}
#endif


#if 0
void pilot()
{
	if(g_linkedMode)
	{
		//basicRfConfig.ackRequest = 0;
		pConfig->frameHead = 0x8841;
		pConfig->destAddr = 0xFFFF;
		//pConfig->myAddr = 0x000F;
		arrcpy(pTxData,pilotCommand,sizeof(pilotCommand));
		g_sendLen = sizeof(pilotCommand);
		//g_sendFlag = 1;
		basicRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		halMcuWaitMs(2);
		basicRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		halMcuWaitMs(2);
		pTxData[0] = 0x90;
		pTxData[1] = 0x00;
		pTxData[2] = 0x03;
		pTxData[3] = 0xe8;
		g_sendLen = 4;
		basicRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
		halMcuWaitMs(2);
		pTxData[0] = 0x90;
		pTxData[1] = 0x00;
		pTxData[2] = 0x00;
		pTxData[3] = 0x00;
		g_sendLen = 4;
		basicRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
	}else
	{
		//s_veriState = 7;
		if(linkedRelCnt<9)
		{
			pConfig->frameHead = 0x8841;
			pConfig->destAddr = 0xFFFF;
			//pConfig->myAddr = 0x000F;
			pTxData[0] = 0x06;
			pTxData[1] = 0x00;//0x01;
			pTxData[2] = 0xFF;//0x01;
			pTxData[3] = 0x4A;
			pTxData[4] = 0x01;//0x01;
			pTxData[5] = g_linkRelCnt;//0x07;
			g_sendLen = 6;
			basicRfSendPacket(pConfig->destAddr, pTxData, g_sendLen,0);
			linkedRelCnt++;
			halMcuWaitMs(7);
		}else
		{
			linkedRelCnt = 0;
			s_veriState = 0xFF;
			g_linkRelCnt ++;
		}
	}
}
#endif

/*
#pragma vector = P0INT_VECTOR    //格式：#pragma vector = 中断向量，紧接着是中断处理程序
  __interrupt void P0_ISR(void) 
{ 
  P0IFG = 0;             //清中断标志 
  P0IF = 0;              //清中断标志 
}*/

HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
{
   P0IFG = 0;             //清中断标志 
   P0IF = 0;              //清中断标志 
}


#ifdef DEBUG_TRACE
static void PutChar(const char Tbyte)
{
	U0DBUF = Tbyte; 
	while(UTX0IF == 0);   //发送完成标志位
	UTX0IF = 0;  
}

static void putsStr(const char *str)
{
	while(*str!='0')
		PutChar(*str++);
	//HalUARTWrite(0, (unsigned char*)str++, 1);
}

static char *mac_itoa(long n, int base)
 {
  	char *p;
 	int minus;
 	static char buf[16];

//	puts_("enter _itoa\n");
//	put_hex(n);

 	p = &buf[16];
 	*--p =  0;
 	if (n < 0) 
	{
  		minus = 1;
  		n = -n;
 	}
 	else
  		minus = 0;


 	if (n == 0)
	{
  		*--p = '0';
	}
 	else
  		while (n > 0) 
		{  
	//		puts_("_itoa ");
   			*--p = "0123456789abcdef"[n % base];
	//		putchar(*p);
   			n /= base;
  		}

 	if (minus)
  		*--p = '-';

 	return p;
}



int Printf(const char *fmt, ...)
{
	int i;
	const char *s;
 	int d;
	char buf[128];
	char *p = buf;
//	char buf[16];
//	char *p = buf;
 	va_list ap;

    va_start(ap, fmt);
	i = 16;

    while (*fmt) 
	{
        if (*fmt != '%') 
		{
            PutChar(*fmt++);
            continue;
        }
		
		switch(*++fmt)
		{
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
		 {
                   PutChar(*s);
                }
                break;
            case 'd':			
                d = va_arg(ap, int);
				//d>>=8;
                p = mac_itoa(d, 10);
                for (s = p; *s; s++) 
				{
                    PutChar(*s);
                }

				break;

            case 'x':
			case 'X':
                d = va_arg(ap, int);
                p = mac_itoa(d, 16);
                for (s = p; *s; s++) 
				{
                    PutChar(*s);
                }
                break;
             //Add other specifiers here...              
            default:  
                putsStr(fmt);
                break;
        } 
        fmt++;
    }
    va_end(ap);

    return 1;   /* Dummy return value */
}

void DeviceInfoPrint(void)
{
	DeviceInfo* dev;
	dev = pConfig->devHead;
	if(NULL==dev)
	{
		Printf("No device linked\r\n");
	}
	Printf("total device %d\r\n",pConfig->totalDev);
	while(dev!=NULL)
	{
		Printf("addr: %d;",dev->shortAddr);
		Printf("group: %d;", dev->group);
		Printf("attribute: %d;",dev->atribute);
		Printf("dev: %x;",dev);
		Printf("next: %x;",dev->pNext);
		Printf("front: %x;",dev->pFront);
		Printf("\r\n");
		dev = dev->pNext;
	}
}

void BufferPrint(uint8* buf, uint16 size)
{
	uint16 i;
	for(i=0;i<size;i++)
	{
		Printf("%x ",buf[i]);
	}
	Printf("\r\n");
}
#endif



