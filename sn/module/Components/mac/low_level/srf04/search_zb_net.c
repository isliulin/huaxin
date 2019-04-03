/******************** (C) COPYRIGHT 2012 XXXXX Elec.,LTD ***********************
* File Name         : search_zb_net.c
* Author            : huangshengbin
* Date First Issued : 
* Description       : 用于扫描指定信道中网络列表
********************************************************************************
* History:
*******************************************************************************/

#include "search_zb_net.h"


#define DEBUG_SW       0
#if  DEBUG_SW
#include "utils.h"
#define DEBUG_PRINTF(...)     kprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif



typedef struct nwkParam
{
  struct nwkParam *nextParam;
  uint8 logicalChannel;
  uint8 extendedPANID[8];
  uint16 PANID;
  uint16 chosenRouter;
  uint8 chosenRouterLinkQuality;
  uint8 chosenRouterDepth:4;
  uint8 routerCapacity:1;
  uint8 deviceCapacity:1;
} DiscoveredNwkParam_t;

#define CHANNELLIST_26    0x04000000  // 26 - 0x1A
#define CHANNELLIST_25    0x02000000  // 25 - 0x19
#define CHANNELLIST_24    0x01000000  // 24 - 0x18
#define CHANNELLIST_23    0x00800000  // 23 - 0x17
#define CHANNELLIST_22    0x00400000  // 22 - 0x16
#define CHANNELLIST_21    0x00200000  // 21 - 0x15
#define CHANNELLIST_20    0x00100000  // 20 - 0x14
#define CHANNELLIST_19    0x00080000  // 19 - 0x13
#define CHANNELLIST_18    0x00040000  // 18 - 0x12
#define CHANNELLIST_17    0x00020000  // 17 - 0x11
#define CHANNELLIST_16    0x00010000  // 16 - 0x10
#define CHANNELLIST_15    0x00008000  // 15 - 0x0F
#define CHANNELLIST_14    0x00004000  // 14 - 0x0E
#define CHANNELLIST_13    0x00002000  // 13 - 0x0D
#define CHANNELLIST_12    0x00001000  // 12 - 0x0C
#define CHANNELLIST_11    0x00000800  // 11 - 0x0B

//这里定义你想扫描的信道
#define  testChannelList   (CHANNELLIST_11 | CHANNELLIST_12 | CHANNELLIST_23 | CHANNELLIST_24)

//这里定义最大可保存的网络数量
#define MAX_NWK_PARAMLIST        20
DiscoveredNwkParam_t *pDiscoveredNwkParamList = NULL; //扫描结果保存在这里链表

static uint8 gNwkCnt = 0; //记录扫描到的网络个数

static void performNetworkDisc( uint32 scanChannelList );

static void freeNwkParamList( void );

static void *beaconIndCB ( void *param );

static void *nwkDiscoveryCnfCB ( void *param );


void scanNwkList(void)
{
	freeNwkParamList();
	
  NLME_ResetRequest();
  
  performNetworkDisc(testChannelList);
}


void performNetworkDisc( uint32 scanChannelList )
{
  NLME_ScanFields_t scan;
  
  devState = DEV_NWK_DISC;

  scan.channels = scanChannelList;
  scan.duration = BEACON_ORDER_240_MSEC;
  scan.scanType = ZMAC_ACTIVE_SCAN;
  scan.scanApp  = NLME_DISC_SCAN;

  if ( NLME_NwkDiscReq2( &scan ) == ZSuccess )
  {
    // Register ZDO callback to handle the network discovery confirm and
    // beacon notification confirm
    ZDO_RegisterForZdoCB( ZDO_NWK_DISCOVERY_CNF_CBID, nwkDiscoveryCnfCB );
    ZDO_RegisterForZdoCB( ZDO_BEACON_NOTIFY_IND_CBID, beaconIndCB );
  }
  else
  {
    NLME_NwkDiscTerm();
  }
}

void *beaconIndCB ( void *param )
{
  NLME_beaconInd_t *pBeacon = param;
  DiscoveredNwkParam_t *pParam = pDiscoveredNwkParamList;
  DiscoveredNwkParam_t *pLastParam;
  uint8 found = FALSE;
  
  if ( (pBeacon->permitJoining == TRUE) && 
  	 ((pBeacon->deviceCapacity || pBeacon->routerCapacity) || (pBeacon->LQI > gMIN_TREE_LINK_COST) ))
  {
    // Add the network parameter to the Network Parameter List
    while ( pParam != NULL )
    {
      if ( ( pParam->PANID == pBeacon->panID ) &&
          ( pParam->logicalChannel == pBeacon->logicalChannel ))
      {
        found = TRUE;
        break;
      }
      
      pLastParam = pParam;
      pParam = pParam->nextParam;
    }
    
    // If no existing parameter found, make a new one and add to the list
    if ( found == FALSE )
    {
      gNwkCnt ++; 
      
      if (gNwkCnt > MAX_NWK_PARAMLIST)
         return NULL;
      
      pParam = osal_mem_alloc( sizeof( DiscoveredNwkParam_t ) );
      if ( pParam == NULL )
      {
        // Memory alloc failed, discard this beacon
        return ( NULL );
      }
      
      // Clear the network descriptor
      osal_memset( pParam, 0, sizeof( DiscoveredNwkParam_t )  );

      // Initialize the descriptor
      pParam->chosenRouter = pBeacon->sourceAddr;
      pParam->chosenRouterLinkQuality = pBeacon->LQI;
      pParam->chosenRouterDepth = pBeacon->depth;
      
      // Save new entry into the descriptor list
      if ( pDiscoveredNwkParamList == NULL )
      {
        // First element in the list
        pDiscoveredNwkParamList = pParam;
      }
      else
      {
        // Last element in the list
        pLastParam->nextParam = pParam;
      }
    }

    // Update the descriptor with the incoming beacon
    pParam->logicalChannel = pBeacon->logicalChannel;
    pParam->PANID          = pBeacon->panID;
    
     // Save the extended PAN ID from the beacon payload only if 1.1 version network
    if ( pBeacon->protocolVersion != ZB_PROT_V1_0 )
    {
      osal_cpyExtAddr( pParam->extendedPANID, pBeacon->extendedPanID );
    }
    else
    {
      osal_memset( pParam->extendedPANID, 0xFF, Z_EXTADDR_LEN );
    }

    // check if this device is a better choice to join...
    // ...dont bother checking assocPermit flag is doing a rejoin
    if (  pBeacon->LQI > gMIN_TREE_LINK_COST  )
    {
      uint8 selected = FALSE;
      uint8 capacity = FALSE;

      if ( _NIB.nwkAddrAlloc == NWK_ADDRESSING_STOCHASTIC )
      {
        if ( ((pBeacon->LQI   > pParam->chosenRouterLinkQuality) &&
              (pBeacon->depth < MAX_NODE_DEPTH)) ||
            ((pBeacon->LQI   == pParam->chosenRouterLinkQuality) &&
             (pBeacon->depth < pParam->chosenRouterDepth)) )
        {
          selected = TRUE;
        }
      }
      else
      {
        if ( pBeacon->depth < pParam->chosenRouterDepth )
        {
          selected = TRUE;
        }
      }

      capacity = pBeacon->routerCapacity;

      if ( (capacity) && (selected) )
      {
        // this is the new chosen router for joining...
        pParam->chosenRouter            = pBeacon->sourceAddr;
        pParam->chosenRouterLinkQuality = pBeacon->LQI;
        pParam->chosenRouterDepth       = pBeacon->depth;
      }
    }
  }
  return ( NULL );
}

void *nwkDiscoveryCnfCB ( void *param )
{
  NLME_NwkDiscTerm();
  
  // Scan completed. De-register the callbacks with ZDO
  ZDO_DeregisterForZdoCB( ZDO_NWK_DISCOVERY_CNF_CBID );
  ZDO_DeregisterForZdoCB( ZDO_BEACON_NOTIFY_IND_CBID );
  
  if ( pDiscoveredNwkParamList != NULL )
  {
    #warning 这里扫描到结果
  }
  else
  {
    #warning 这里扫描不到网络
  }
  
  freeNwkParamList();

  return ( NULL );
}

void freeNwkParamList( void )
{
  DiscoveredNwkParam_t *pParam = pDiscoveredNwkParamList;
  DiscoveredNwkParam_t *pNextParam;

  // deallocate the pDiscoveredNwkParamList memory
  while ( pParam != NULL )
  {
    pNextParam = pParam->nextParam;

    osal_mem_free( pParam );

    pParam = pNextParam;
  }

  pDiscoveredNwkParamList = NULL;
  gNwkCnt = 0;
}