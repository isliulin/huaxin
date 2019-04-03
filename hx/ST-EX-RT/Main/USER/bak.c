#if 0
void KeyLeft(signed char mode)
{
#ifdef _MOTO_
	if(SETTING_MOTO==g_SettingIndex)
	{
		if(motoSet.moving != 0)return;
#ifdef MOTO_REFLECT_DETECT
		if(REF_BOARD_DETECT) return;
#endif
		motoSet.focIndex--;
		if(motoSet.focIndex<=0)
		{
			motoSet.focIndex = 0;
			*motoSet.dest_location = focLenScape[0];
			motoSet.destRsv = *motoSet.dest_location;
			MotoReset();
			LCD_MMOTO_OFF;
			*motoSet.mode = MOTO_MODE_AUTO;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_AUTO;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_AUTO &  motoSet.focIndex);
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//g_paraChange = 1;
			//SetFocLen(0,0);
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_REVERSE); 
			ParaPreserve(EEPROM_INDEX_MOTOMODE,(MASK_MOTOMODE_AUTO&motoSet.focIndex));
#ifdef _CANON_
			//while(uart_tran_flag);
			//SendUartData(UART_ENCODE_FOCLEN,MOTO_MODE_AUTO,*motoSet.dest_location);
			DecodeUartData(UART_ENCODE_FOCLEN,MOTO_MODE_AUTO,*motoSet.dest_location);
#endif
		}else
		{
			LCD_MMOTO_ON;
/*
			if(motoSet.current_location<105)
			{
				if(!MOTOLOCA)
				{
					MotoReset();
				}
			}else
			{
				if(MOTOLOCA)
				{
					MotoReset();
				}
			}
*/
			*motoSet.mode = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = motoSet.focIndex;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_MANU | motoSet.focIndex);
			ParaPreserve(EEPROM_INDEX_MOTOMODE,(MASK_MOTOMODE_MANU|motoSet.focIndex));
			*motoSet.dest_location = focLenScape[motoSet.focIndex];
			motoSet.destRsv = *motoSet.dest_location;
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			ParaPreserve(EEPROM_INDEX_LOCATION,*motoSet.dest_location);
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_REVERSE); 
			//SetFocLen(motoSet.focIndex,0);	
			//g_paraChange = 1;
			//SendUartData(UART_ENCODE_FOCLEN,*motoSet.dest_location,0);
#ifdef _CANON_
			//while(uart_tran_flag);
			//SendUartData(UART_ENCODE_FOCLEN,MOTO_MODE_AUTO,*motoSet.dest_location);
			DecodeUartData(UART_ENCODE_FOCLEN,motoSet.focIndex,*motoSet.dest_location);
#endif
		}
		return;
	}
#endif
	switch(mode)
	{
		case WORKINGMODE_TTL:
			if(0==g_SettingIndex)
			{
#ifdef _CANON_
				if(WLM_ON == *wlSet.is_masterOn)
				{
					return;
				}
#endif
				if((-3==mstPara->cmp1)&&(0==mstPara->cmp2))
				{
					return;
				}
				mstPara->cmp2--;
				if(mstPara->cmp2<0)
				{
					mstPara->cmp2+=3;
					mstPara->cmp1--;
				}
				//wlSet.m_arrPara[WLM_ARR_A].cmp1 = mstPara->cmp1;
				//wlSet.m_arrPara[WLM_ARR_A].cmp2 = mstPara->cmp2 ;
				SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0);

				ParaPreserve(EEPROM_INDEX_TTLCMP,((mstPara->cmp1+3)<<4)|mstPara->cmp2);				
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_TTLCOM,mstPara->cmp1,mstPara->cmp2);
			}
#ifdef _CANON_
			else if(SETTING_SYNMODE==g_SettingIndex)
			{
				mainPara.synMode--;
				if(mainPara.synMode<SYNC_MODE_FRON)
				{
					mainPara.synMode = SYNC_MODE_FP;
				}
				if((SYNC_MODE_REAR==mainPara.synMode)&&(WLM_ON==*wlSet.is_masterOn))
				{
					mainPara.synMode=SYNC_MODE_FRON;
				}
				CanSynMode(mode,mainPara.synMode,DSP_REVERSE);
				ParaPreserve(EEPROM_INDEX_MODE,mainPara.workMode|(mainPara.synMode<<6));
				
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_SYNC,mainPara.synMode);
			}else if(SETTING_TTLFEB==g_SettingIndex)
			{
				if((!flaPara.ttl_FEB2)&&(!flaPara.ttl_FEB1))
					return;
				flaPara.ttl_FEB2--;
				if(flaPara.ttl_FEB2<0)
				{
					flaPara.ttl_FEB1--;
					flaPara.ttl_FEB2+=3;					
				}
					
				//SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
				CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
				
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_TTLFEB,flaPara.ttl_FEB1,flaPara.ttl_FEB2);
			}
#if 0
			else if(WLM_SETTING_RATION==g_SettingIndex)
			{
				wlSet.arr_mode--;
				if(wlSet.arr_mode<WLM_MODE_ALL)
					wlSet.arr_mode = WLM_MODE_RES;
				DisplayCanMstHead(1,mainPara.workMode,&wlSet);
				
				//ParaPreserve(EEPROM_CANON_RATIO,(wlSet.is_masterOn<<7|wlSet.arr_mode));	
				ParaPreserve(EEPROM_CANON_RATIO,((wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(wlSet.arr_mode&0x03)));
				//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(1,mainPara.workMode);
				
			}
#endif
			else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if((-3==wlSet.m_arrPara[WLM_ARR_A].cmp1)&&(0==wlSet.m_arrPara[WLM_ARR_A].cmp2))
				{
					return;
				}
				wlSet.m_arrPara[WLM_ARR_A].cmp2--;
				if(wlSet.m_arrPara[WLM_ARR_A].cmp2<0)
				{
					wlSet.m_arrPara[WLM_ARR_A].cmp2+=3;
					wlSet.m_arrPara[WLM_ARR_A].cmp1--;					
				}
				
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);				
				
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);
/*************************************************************************************************************/
//in the canon mode, the wlm array A has the parameter same as master
/*************************************************************************************************************/
				mstPara->cmp1 = wlSet.m_arrPara[WLM_ARR_A].cmp1;
				mstPara->cmp2 = wlSet.m_arrPara[WLM_ARR_A].cmp2;
				SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0);	

				//ParaPreserve(EEPROM_CANON_CMPA,((wlSet.m_arrPara[WLM_ARR_A].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_A].cmp2));
				////while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,((mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRA),((wlSet.m_arrPara[WLM_ARR_A].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_A].cmp2));

				
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				wlSet.m_arrPara[WLM_ARR_B].cmp1--;
				if(wlSet.m_arrPara[WLM_ARR_B].cmp1<0)
				{
					wlSet.m_arrPara[WLM_ARR_B].cmp1 = 0;
				}
				//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(3,mainPara.workMode,&wlSet);
				
				//ParaPreserve(EEPROM_CANON_CMPB,((wlSet.m_arrPara[WLM_ARR_B].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_B].cmp2));
				//ParaPreserve(EEPROM_CANON_CMPB,wlSet.m_arrPara[WLM_ARR_B].cmp1);
				ParaPreserve(EEPROM_CANON_ABR,wlSet.m_arrPara[WLM_ARR_B].cmp1);
				
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,((mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRB),((wlSet.m_arrPara[WLM_ARR_B].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_B].cmp2));
				//DecodeUartData(UART_ENCODE_WLMSET,((mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRB),wlSet.m_arrPara[WLM_ARR_B].cmp1);
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode|(UART_ENCODE_WLMSET_ARRB<<4)),wlSet.m_arrPara[WLM_ARR_B].cmp1);
				
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				if((-3==wlSet.m_arrPara[WLM_ARR_C].cmp1)&&(0==wlSet.m_arrPara[WLM_ARR_C].cmp2))
				{
					return;
				}
				wlSet.m_arrPara[WLM_ARR_C].cmp2--;
				if(wlSet.m_arrPara[WLM_ARR_C].cmp2<0)
				{
					wlSet.m_arrPara[WLM_ARR_C].cmp2+=3;
					wlSet.m_arrPara[WLM_ARR_C].cmp1--;					
				}
				//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(4,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_CMPC,((wlSet.m_arrPara[WLM_ARR_C].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_C].cmp2));
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,((mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRC),((wlSet.m_arrPara[WLM_ARR_C].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_C].cmp2));
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode|(UART_ENCODE_WLMSET_ARRC<<4)),((wlSet.m_arrPara[WLM_ARR_C].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_C].cmp2));
			}else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				*wlSet.m_chan--;
				if(*wlSet.m_chan<WL_CHANNEL_ONE)
				{
					*wlSet.m_chan = WL_CHANNEL_FOUR;
				}
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN, /*g_paraBakBuf[EEPROM_CANON_RATIO]*/((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
			}
#endif
		break;

		case WORKINGMODE_MANU:
			if(0==g_SettingIndex)
			{
#ifdef _CANON_
				if(WLM_ON == *wlSet.is_masterOn)
				{
					return;
				}
#endif
				if((1==mstPara->output1)&&(0==mstPara->output2))
				{
					mstPara->output1 = 8;
					mstPara->output2 = 0;
				}else
				{
					mstPara->output2--;
					if(mstPara->output2<0)
					{
						mstPara->output2+=3;
						mstPara->output1--;
					}
					
				}
				SetManuPara(mstPara->output1,mstPara->output2);

				ParaPreserve(EEPROM_INDEX_EXP,(mstPara->output1<<4)|mstPara->output2);		
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_OUTPUT,mstPara->output1,mstPara->output2);				
			}
#ifdef _CANON_
			else if(SETTING_SYNMODE==g_SettingIndex)
			{
				mainPara.synMode--;
				if(mainPara.synMode<SYNC_MODE_FRON)
				{
					mainPara.synMode = SYNC_MODE_FP;
				}
				if((SYNC_MODE_REAR==mainPara.synMode)&&(WLM_ON==*wlSet.is_masterOn))
				{
					mainPara.synMode=SYNC_MODE_FRON;
				}
				CanSynMode(mode,mainPara.synMode,DSP_REVERSE);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_SYNC,mainPara.synMode);
			}else if(WLM_SETTING_RATION==g_SettingIndex)
			{
				*wlSet.arr_mode--;
				if(*wlSet.arr_mode<WLM_MODE_ALL)
					*wlSet.arr_mode = WLM_MODE_RES;
				DisplayCanMstHead(1,mainPara.workMode,&wlSet);

				//ParaPreserve(EEPROM_CANON_RATIO,(wlSet.is_masterOn<<7|wlSet.arr_mode));	
				ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
				//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(1,mainPara.workMode);				
			}
#if 0
			else if((WLM_SETTING_ARRA<=g_SettingIndex)&&(WLM_SETTING_ARRC>=g_SettingIndex))
			{
				if((1==wlSet.m_arrPara[g_SettingIndex-3].output1)&&(0==wlSet.m_arrPara[g_SettingIndex-3].output2))
				{
					wlSet.m_arrPara[g_SettingIndex-3].output1 = 8;
					wlSet.m_arrPara[g_SettingIndex-3].output2 = 0;
				}else
				{
					wlSet.m_arrPara[g_SettingIndex-3].output2--;
					if(wlSet.m_arrPara[g_SettingIndex-3].output2<0)
					{
						wlSet.m_arrPara[g_SettingIndex-3].output2+=3;
						wlSet.m_arrPara[g_SettingIndex-3].output1--;
					}
				}
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(g_SettingIndex-2,mainPara.workMode,&wlSet);
				
				ParaPreserve((EEPROM_CANON_ARRAEXP+g_SettingIndex-4),(wlSet.m_arrPara[g_SettingIndex-3].output1<<4)|wlSet.m_arrPara[g_SettingIndex-3].output2 );
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|(EEPROM_CANON_ARRAEXP+g_SettingIndex-4),(wlSet.m_arrPara[g_SettingIndex-3].output1<<4)|wlSet.m_arrPara[g_SettingIndex-3].output2 );
			}
#endif
#if 1
			else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if((1==wlSet.m_arrPara[WLM_ARR_A].output1)&&(0==wlSet.m_arrPara[WLM_ARR_A].output2))
				{
					wlSet.m_arrPara[WLM_ARR_A].output1 = 8;
					wlSet.m_arrPara[WLM_ARR_A].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WLM_ARR_A].output2--;
					if(wlSet.m_arrPara[WLM_ARR_A].output2<0)
					{
						wlSet.m_arrPara[WLM_ARR_A].output2+=3;
						wlSet.m_arrPara[WLM_ARR_A].output1--;
					}
				}
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);

				mstPara->output1 = wlSet.m_arrPara[WLM_ARR_A].output1;
				mstPara->output2 = wlSet.m_arrPara[WLM_ARR_A].output2;
				SetManuPara(mstPara->output1,mstPara->output2);				
				
				ParaPreserve(EEPROM_CANON_ARRAEXP,(wlSet.m_arrPara[WLM_ARR_A].output1<<4)|wlSet.m_arrPara[WLM_ARR_A].output2 );
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode)|(UART_ENCODE_WLMSET_ARRA<<4),(wlSet.m_arrPara[WLM_ARR_A].output1<<4)|wlSet.m_arrPara[WLM_ARR_A].output2 );
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				if((1==wlSet.m_arrPara[WLM_ARR_B].output1)&&(0==wlSet.m_arrPara[WLM_ARR_B].output2))
				{
					wlSet.m_arrPara[WLM_ARR_B].output1 = 8;
					wlSet.m_arrPara[WLM_ARR_B].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WLM_ARR_B].output2--;
					if(wlSet.m_arrPara[WLM_ARR_B].output2<0)
					{
						wlSet.m_arrPara[WLM_ARR_B].output2+=3;
						wlSet.m_arrPara[WLM_ARR_B].output1--;
					}
				}
				//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(3,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_ARRBEXP,(wlSet.m_arrPara[WLM_ARR_B].output1<<4)|wlSet.m_arrPara[WLM_ARR_B].output2 );
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode)|(UART_ENCODE_WLMSET_ARRB<<4),(wlSet.m_arrPara[WLM_ARR_B].output1<<4)|wlSet.m_arrPara[WLM_ARR_B].output2 );
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				if((1==wlSet.m_arrPara[WLM_ARR_C].output1)&&(0==wlSet.m_arrPara[WLM_ARR_C].output2))
				{
					wlSet.m_arrPara[WLM_ARR_C].output1 = 8;
					wlSet.m_arrPara[WLM_ARR_C].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WLM_ARR_C].output2--;
					if(wlSet.m_arrPara[WLM_ARR_C].output2<0)
					{
						wlSet.m_arrPara[WLM_ARR_C].output2+=3;
						wlSet.m_arrPara[WLM_ARR_C].output1--;
					}
				}
				//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(4,mainPara.workMode,&wlSet);
				
				ParaPreserve(EEPROM_CANON_ARRCEXP,(wlSet.m_arrPara[WLM_ARR_C].output1<<4)|wlSet.m_arrPara[WLM_ARR_C].output2 );
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRC<<4),(wlSet.m_arrPara[WLM_ARR_C].output1<<4)|wlSet.m_arrPara[WLM_ARR_C].output2 );
			}
#endif
			else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				*wlSet.m_chan--;
				if(*wlSet.m_chan<WL_CHANNEL_ONE)
				{
					*wlSet.m_chan = WL_CHANNEL_FOUR;
				}
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN, /*g_paraBakBuf[EEPROM_CANON_RATIO]*/((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
				//DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
			}
#endif
			
		break;

#ifdef _SPEED_LIGHT_
		case WORKINGMODE_F1:
		case WORKINGMODE_F2:
			if((1==mstPara->output1)&&(0==mstPara->output2))
			{
				mstPara->output1 = 8;
				mstPara->output2 = 0;
			}else
			{
				mstPara->output2--;
				if(mstPara->output2<0)
				{
					mstPara->output2+=3;
					mstPara->output1--;
				}
				
			}
			SetRmtPara(mstPara->output1,mstPara->output2);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_OUTPUT,mstPara->output1,mstPara->output2);		
		break;
#endif

		case WORKINGMODE_MULTI:
			if(0==g_SettingIndex)
			{
#ifdef _CANON_
				if(WLM_ON == *wlSet.is_masterOn)
				{
					return;
				}
				if(0x10==mainPara.modGr[MASTER].multi_exp)	
				{
					mainPara.modGr[MASTER].multi_exp=0x60;
					MultiCalMaxTime();					
				}else
				{
					mainPara.modGr[MASTER].multi_exp-=0x10;
				}

				wlSet.m_arrPara[WLM_ARR_A].multi_exp= mainPara.modGr[MASTER].multi_exp;
				
				SetMultiPara(mainPara.modGr[MASTER].multi_exp,mainPara.multi_times,mainPara.multi_Frq);

				ParaPreserve(EEPROM_INDEX_MULTI_EXPOSE,mainPara.modGr[MASTER].multi_exp);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTI,UART_ENCODE_MULTI_EXPOSE,(mainPara.modGr[MASTER].multi_exp>>4));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
#else
				if(0x10==mainPara.modGr[MASTER].multi_exp)
				{
					mainPara.modGr[MASTER].multi_exp = 0x50;
					MultiCalMaxTime();
				}else
				{
					if(0==(mainPara.modGr[MASTER].multi_exp&0x0f))
					{
						mainPara.modGr[MASTER].multi_exp -=0x10;
						mainPara.modGr[MASTER].multi_exp += 2;
					}else
					{
						mainPara.modGr[MASTER].multi_exp --;
					}
				}
				SetMultiPara(mainPara.modGr[MASTER].multi_exp,mainPara.multi_times,mainPara.multi_Frq);
				
				ParaPreserve(EEPROM_INDEX_MULTI_EXPOSE,mainPara.modGr[MASTER].multi_exp);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTI,UART_ENCODE_MULTI_EXPOSE,mainPara.modGr[MASTER].multi_exp);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);

#endif
			}else if(1==g_SettingIndex)
			{
#ifdef _NIKON_
				if(mainPara.multi_times<=10)
				{
					mainPara.multi_times--;
				}else 
				{
					mainPara.multi_times-=2;
				}

				if(mainPara.multi_times<1)
				{
					mainPara.multi_times = 1;
				}
#else
				if(mainPara.multi_times<=20)
				{
					mainPara.multi_times--;
				}else if(mainPara.multi_times<=50)
				{
					mainPara.multi_times-=5;
				}else
				{
					mainPara.multi_times-=10;
				}

				if(mainPara.multi_times<1)
				{
					mainPara.multi_times = 1;
				}
#endif
				LCD_MULTIME_RVS(mainPara.multi_times);
				ParaPreserve(EEPROM_INDEX_MULTI_TIMES,mainPara.multi_times);

				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
				
			}else if(2==g_SettingIndex)
			{
#ifdef _NIKON_
				if(mainPara.multi_Frq<=10)
				{
					mainPara.multi_Frq--;
				}else if(mainPara.multi_Frq<=100)
				{
					mainPara.multi_Frq-=10;
				}
#else
				if(mainPara.multi_Frq<=20)
				{
					mainPara.multi_Frq--;
				}else if(mainPara.multi_Frq<=50)
				{
					mainPara.multi_Frq-=5;
				}else if(mainPara.multi_Frq<=190)
				{
					mainPara.multi_Frq-=10;
				}else
				{
					mainPara.multi_Frq = 190;
				}
#endif
				if(mainPara.multi_Frq<1)
				{
					mainPara.multi_Frq = 1;
				}
				LCD_MULFRQ_RVS(mainPara.multi_Frq);
				ParaPreserve(EEPROM_INDEX_MULTI_FRQ,mainPara.multi_Frq);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
				
			}
#ifdef _CANON_
			if(WLM_SETTING_RATION==g_SettingIndex)
			{
				*wlSet.arr_mode--;
				if(*wlSet.arr_mode<WLM_MODE_ALL)
					*wlSet.arr_mode = WLM_MODE_RES;
				//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(1,mainPara.workMode,&wlSet);

				//ParaPreserve(EEPROM_CANON_RATIO,(wlSet.is_masterOn<<7|wlSet.arr_mode));	
				ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
				//DisplayCanMstHead(1,mainPara.workMode);
				
			}
#if 0
			else if((WLM_SETTING_ARRA<=g_SettingIndex)&&(WLM_SETTING_ARRC>=g_SettingIndex))
			{
				if(0x10==(wlSet.m_arrPara[g_SettingIndex-3].mulOtp&0xf0))
				{
					wlSet.m_arrPara[g_SettingIndex-3].mulOtp = 0x60;
				}else
				{
					wlSet.m_arrPara[g_SettingIndex-3].mulOtp -= 0x10;
				}
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(g_SettingIndex-3,mainPara.workMode,&wlSet);

				ParaPreserve((EEPROM_CANON_MULARRA+g_SettingIndex-4),wlSet.m_arrPara[g_SettingIndex-3].mulOtp);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|(g_SettingIndex-4),wlSet.m_arrPara[g_SettingIndex-3].mulOtp);
				
			}
#endif
#if 1
			else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if(0x10==(wlSet.m_arrPara[WLM_ARR_A].multi_exp&0xf0))
				{
					wlSet.m_arrPara[WLM_ARR_A].multi_exp = 0x60;
					mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].multi_exp;
					MultiCalMaxTime();
				}else
				{
					wlSet.m_arrPara[WLM_ARR_A].multi_exp -= 0x10;
					mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].multi_exp;
				}
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);

				//mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].mulOtp;
				SetMultiPara(mainPara.modGr[MASTER].multi_exp,mainPara.multi_times,mainPara.multi_Frq);				

				ParaPreserve(EEPROM_CANON_MULARRA,wlSet.m_arrPara[WLM_ARR_A].multi_exp);
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRA,wlSet.m_arrPara[WLM_ARR_A].mulOtp);
				DecodeUartData(UART_ENCODE_WLMSET,(UART_ENCODE_WLMSET_ARRA<<4)|mainPara.workMode,wlSet.m_arrPara[WLM_ARR_A].multi_exp);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
				
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				if(0x10==(wlSet.m_arrPara[WLM_ARR_B].multi_exp&0xf0))
				{
					wlSet.m_arrPara[WLM_ARR_B].multi_exp = 0x60;
				}else
				{
					wlSet.m_arrPara[WLM_ARR_B].multi_exp -= 0x10;
				}
				//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(3,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_MULARRB,wlSet.m_arrPara[WLM_ARR_B].multi_exp);
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRB,wlSet.m_arrPara[WLM_ARR_B].mulOtp);
				DecodeUartData(UART_ENCODE_WLMSET,(UART_ENCODE_WLMSET_ARRB<<4)|mainPara.workMode,wlSet.m_arrPara[WLM_ARR_B].multi_exp);
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				if(0x10==(wlSet.m_arrPara[WLM_ARR_C].multi_exp)&0xf0)
				{
					wlSet.m_arrPara[WLM_ARR_C].multi_exp = 0x60;
				}else
				{
					wlSet.m_arrPara[WLM_ARR_C].multi_exp -= 0x10;
				}
				//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(4,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_MULARRC,wlSet.m_arrPara[WLM_ARR_C].multi_exp);
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRC,wlSet.m_arrPara[WLM_ARR_C].mulOtp);
				DecodeUartData(UART_ENCODE_WLMSET,(UART_ENCODE_WLMSET_ARRC<<4)|mainPara.workMode,wlSet.m_arrPara[WLM_ARR_C].multi_exp);
			}
#endif
			else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				*wlSet.m_chan--;
				if(*wlSet.m_chan<WL_CHANNEL_ONE)
				{
					*wlSet.m_chan = WL_CHANNEL_FOUR;
				}
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN, /*g_paraBakBuf[EEPROM_CANON_RATIO]*/((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
				//DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
			}
#endif
		break;
		
#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			if((g_SettingIndex<5)&&(g_SettingIndex>0))
			{
				WlmArrLeft();
				SetMastPara(*wlSet.m_chan,wlSet.m_arrPara);
			}else if(5==g_SettingIndex)
			{
				*wlSet.m_chan--;
				if(*wlSet.m_chan<WL_CHANNEL_ONE)
				{
					*wlSet.m_chan = WL_CHANNEL_FOUR;
				}
				SetMstChan(*wlSet.m_chan,DSP_REVERSE);

				ParaPreserve(EEPROM_INDEX_SWL_CHANARR,((*wlSet.m_chan<<6)|(wlSet.s_chan<<4) | wlSet.s_array));	
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
			}
		break;
#endif
		case WORKINGMODE_RTC:
		case WORKINGMODE_RTN:
			if(1==g_SettingIndex)
			{
				//wlSet.s_array--;
				//if(wlSet.s_array<0)	wlSet.s_array = 2;
				*wlSlave.s_array--;
				if(*wlSlave.s_array<WL_ARRARY_A)
				{
					*wlSlave.s_array = WL_ARRARY_C;
				}
				//while(uart_tran_flag);
#ifdef _CANON_
				DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_ARR,(*wlSlave.s_array-1));
#endif
#ifdef _NIKON_
				DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_ARR,*wlSlave.s_array);		
#endif
				//ParaPreserve(EEPROM_INDEX_SWL_CHANARR,((*wlSet.m_chan<<6)|(wlSet.s_chan<<4) | wlSet.s_array));
				//DisplayRmt(wlSet.s_array,wlSet.s_chan,g_SettingIndex);
			}else if(2==g_SettingIndex)
			{
				*wlSlave.s_chan--;
				if(*wlSlave.s_chan<WL_CHANNEL_ONE)
					*wlSlave.s_chan = WL_CHANNEL_FOUR;
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_CHAN,*wlSlave.s_chan);
				//ParaPreserve(EEPROM_INDEX_SWL_CHANARR,((*wlSet.m_chan<<6)|(wlSet.s_chan<<4) | wlSet.s_array));
				//DisplayRmt(wlSet.s_array,wlSet.s_chan,g_SettingIndex);
			}
			DisplayRmt(*wlSlave.s_array,*wlSlave.s_chan,g_SettingIndex);
			
			ParaPreserve(EEPROM_INDEX_SWL_CHANARR,((*wlSet.m_chan<<6)|(*wlSlave.s_chan<<4) | *wlSlave.s_array));
			////while(uart_tran_flag);
			//DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_CHAN,wlSet.s_chan);
			
		break;

	}
}
#endif

#if 0
void KeyRight(signed char mode)
{
#ifdef _MOTO_
	if(SETTING_MOTO==g_SettingIndex)
	{
		if(motoSet.moving != 0)return;
#ifdef MOTO_REFLECT_DETECT
		if(REF_BOARD_DETECT) return;
#endif
		LCD_MMOTO_ON
		motoSet.focIndex++;

		if(motoSet.focIndex>10)
		{
			motoSet.focIndex=10;
			MotoReset();
		}

		*motoSet.dest_location = focLenScape[motoSet.focIndex];
		motoSet.destRsv = *motoSet.dest_location;
		*motoSet.mode = MOTO_MODE_MANU;
		DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_REVERSE); 
		ParaPreserve(EEPROM_INDEX_MOTOMODE,(MASK_MOTOMODE_MANU|motoSet.focIndex));
		ParaPreserve(EEPROM_INDEX_LOCATION,*motoSet.dest_location);
		
#ifdef _CANON_
		//while(uart_tran_flag);
		//SendUartData(UART_ENCODE_FOCLEN,MOTO_MODE_AUTO,*motoSet.dest_location);
		DecodeUartData(UART_ENCODE_FOCLEN,motoSet.focIndex,*motoSet.dest_location);
#endif
		
		return;
	}
#endif

	switch(mode)
	{
		case WORKINGMODE_TTL:
			if(0==g_SettingIndex)
			{
				if(3==mstPara->cmp1)
				{
					return;
				}
#ifdef  _CANON_
				if(WLM_ON == *wlSet.is_masterOn)
				{
					return;
				}
#endif
				mstPara->cmp2++;
				if(mstPara->cmp2>2)
				{
					mstPara->cmp1++;
					mstPara->cmp2 = 0;
				}
				wlSet.m_arrPara[WLM_ARR_A].cmp1 = mstPara->cmp1;
				wlSet.m_arrPara[WLM_ARR_A].cmp2 = mstPara->cmp2 ;
				
				SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0);

				ParaPreserve(EEPROM_INDEX_TTLCMP,((mstPara->cmp1+3)<<4)|mstPara->cmp2);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_TTLCOM,mstPara->cmp1,mstPara->cmp2);
			}
#ifdef _CANON_
			if(SETTING_SYNMODE==g_SettingIndex)
			{
				mainPara.synMode++;
				if(mainPara.synMode>SYNC_MODE_FP)
				{
					mainPara.synMode = SYNC_MODE_FRON;
				}
				if((SYNC_MODE_REAR==mainPara.synMode)&&(WLM_ON==*wlSet.is_masterOn))
				{
					mainPara.synMode=SYNC_MODE_FP;
				}
				CanSynMode(mode,mainPara.synMode,DSP_REVERSE);
				ParaPreserve(EEPROM_INDEX_MODE,mainPara.workMode|(mainPara.synMode<<6));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_SYNC,mainPara.synMode);
			}else if(SETTING_TTLFEB==g_SettingIndex)
			{
				if(flaPara.ttl_FEB1>=3)
				{
					return;
				}
				flaPara.ttl_FEB2++;
				if(flaPara.ttl_FEB2>=3)
				{
					flaPara.ttl_FEB1++;
					flaPara.ttl_FEB2-=3;
					
				}
				//SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
				CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_TTLFEB,flaPara.ttl_FEB1,flaPara.ttl_FEB2);
				
			}
#if 0
			else if(WLM_SETTING_RATION==g_SettingIndex)
			{
				wlSet.arr_mode++;
				if(wlSet.arr_mode>WLM_MODE_RES)
				{
					wlSet.arr_mode = WLM_MODE_ALL;
				}
				//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(1,mainPara.workMode,&wlSet);
				
				//ParaPreserve(EEPROM_CANON_RATIO,(wlSet.is_masterOn<<7|wlSet.arr_mode));	
				ParaPreserve(EEPROM_CANON_RATIO,((wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(wlSet.arr_mode&0x03)));
			}
#endif
			else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if(3==wlSet.m_arrPara[WLM_ARR_A].cmp1)
				{
					return;
				}
				wlSet.m_arrPara[WLM_ARR_A].cmp2++;
				if(wlSet.m_arrPara[WLM_ARR_A].cmp2>2)
				{
					wlSet.m_arrPara[WLM_ARR_A].cmp1++;
					wlSet.m_arrPara[WLM_ARR_A].cmp2 = 0;
				}
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);

				mstPara->cmp1 = wlSet.m_arrPara[WLM_ARR_A].cmp1;
				mstPara->cmp2 = wlSet.m_arrPara[WLM_ARR_A].cmp2;
				SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0);	

				ParaPreserve(EEPROM_CANON_CMPA,((wlSet.m_arrPara[WLM_ARR_A].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_A].cmp2));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRA,((wlSet.m_arrPara[WLM_ARR_A].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_A].cmp2));
				
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				wlSet.m_arrPara[WLM_ARR_B].cmp1++;
				if(wlSet.m_arrPara[WLM_ARR_B].cmp1>12)
				{
					wlSet.m_arrPara[WLM_ARR_B].cmp1 = 12;
				}
				//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(3,mainPara.workMode,&wlSet);

				//ParaPreserve(EEPROM_CANON_CMPB,((wlSet.m_arrPara[WLM_ARR_B].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_B].cmp2));
				//ParaPreserve(EEPROM_CANON_CMPB,wlSet.m_arrPara[WLM_ARR_B].cmp1);
				ParaPreserve(EEPROM_CANON_ABR,wlSet.m_arrPara[WLM_ARR_B].cmp1);
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|UART_ENCODE_WLMSET_ARRB,((wlSet.m_arrPara[WLM_ARR_B].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_B].cmp2));
				DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRB<<4),wlSet.m_arrPara[WLM_ARR_B].cmp1);
				
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				if(3==wlSet.m_arrPara[WLM_ARR_C].cmp1)
				{
					return;
				}
				wlSet.m_arrPara[WLM_ARR_C].cmp2++;
				if(wlSet.m_arrPara[WLM_ARR_C].cmp2>2)
				{
					wlSet.m_arrPara[WLM_ARR_C].cmp1++;
					wlSet.m_arrPara[WLM_ARR_C].cmp2 = 0;
				}
				DisplayCanMstHead(4,mainPara.workMode,&wlSet);

				ParaPreserve(EEPROM_CANON_CMPC,((wlSet.m_arrPara[WLM_ARR_C].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_C].cmp2));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRC<<4),((wlSet.m_arrPara[WLM_ARR_C].cmp1+3)<<4|wlSet.m_arrPara[WLM_ARR_C].cmp2));
				//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
			}else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				*wlSet.m_chan++;
				if(*wlSet.m_chan>WL_CHANNEL_FOUR)
				{
					*wlSet.m_chan = WL_CHANNEL_ONE;
				}
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);
				
				ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN, /*g_paraBakBuf[EEPROM_CANON_RATIO]*/((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
				//DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
			}
#endif
		break;

		case WORKINGMODE_MANU:
			if(0==g_SettingIndex)
			{
#ifdef _CANON_
				if(WLM_ON == *wlSet.is_masterOn)
				{
					return;
				}
#endif
				if(8==mstPara->output1)
				{
					mstPara->output1 = 1;
					mstPara->output2 = 0;
				}else
				{
					mstPara->output2++;
					if(mstPara->output2>2)
					{
						mstPara->output1++;
						mstPara->output2=0;
					}
					
				}
				SetManuPara(mstPara->output1,mstPara->output2);

				ParaPreserve(EEPROM_INDEX_EXP,(mstPara->output1<<4)|mstPara->output2);
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_OUTPUT,mstPara->output1,mstPara->output2);				
			}
#ifdef _CANON_
		else if(SETTING_SYNMODE==g_SettingIndex)
		{
			mainPara.synMode++;
			if(mainPara.synMode>SYNC_MODE_FP)
			{
				mainPara.synMode = SYNC_MODE_FRON;
			}
			if((SYNC_MODE_REAR==mainPara.synMode)&&(WLM_ON==*wlSet.is_masterOn))
			{
				//mainPara.synMode=SYNC_MODE_FRON;
				mainPara.synMode=SYNC_MODE_FP;
				
			}
			CanSynMode(mode,mainPara.synMode,DSP_REVERSE);
			
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_SYNC,mainPara.synMode);
		}else if(WLM_SETTING_RATION==g_SettingIndex)
		{
			*wlSet.arr_mode++;
			if(*wlSet.arr_mode>WLM_MODE_RES)
			{
				*wlSet.arr_mode = WLM_MODE_ALL;
			}
			DisplayCanMstHead(1,mainPara.workMode,&wlSet);

			//ParaPreserve(EEPROM_CANON_RATIO,(wlSet.is_masterOn<<7|wlSet.arr_mode));	
			ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
			//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
		}
#if 0
		else if((WLM_SETTING_ARRA<=g_SettingIndex)&&(WLM_SETTING_ARRC>=g_SettingIndex))
		{
			if(8==wlSet.m_arrPara[g_SettingIndex-3].output1)
			{
				wlSet.m_arrPara[g_SettingIndex-3].output1 = 1;
				wlSet.m_arrPara[g_SettingIndex-3].output2 = 0;
			}else
			{
				wlSet.m_arrPara[g_SettingIndex-3].output2++;
				if(wlSet.m_arrPara[g_SettingIndex-3].output2>2)
				{
					wlSet.m_arrPara[g_SettingIndex-3].output1++;
					wlSet.m_arrPara[g_SettingIndex-3].output2=0;
				}
			}
			DisplayCanMstHead(g_SettingIndex-2,mainPara.workMode,&wlSet);
			
			ParaPreserve(EEPROM_CANON_ARRAEXP+(g_SettingIndex-4),(wlSet.m_arrPara[g_SettingIndex-3].output1<<4)|wlSet.m_arrPara[g_SettingIndex-3].output2 );
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|(g_SettingIndex-4),(wlSet.m_arrPara[g_SettingIndex-3].output1<<4)|wlSet.m_arrPara[g_SettingIndex-3].output2 );
			//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
		}
#endif
#if 1
		else if(WLM_SETTING_ARRA==g_SettingIndex)
		{
			if(8==wlSet.m_arrPara[WLM_ARR_A].output1)
			{
				wlSet.m_arrPara[WLM_ARR_A].output1 = 1;
				wlSet.m_arrPara[WLM_ARR_A].output2 = 0;
			}else
			{
				wlSet.m_arrPara[WLM_ARR_A].output2++;
				if(wlSet.m_arrPara[WLM_ARR_A].output2>2)
				{
					wlSet.m_arrPara[WLM_ARR_A].output1++;
					wlSet.m_arrPara[WLM_ARR_A].output2=0;
				}
			}
			DisplayCanMstHead(2,mainPara.workMode,&wlSet);

			mstPara->output1 = wlSet.m_arrPara[WLM_ARR_A].output1;
			mstPara->output2 = wlSet.m_arrPara[WLM_ARR_A].output2;
			SetManuPara(mstPara->output1,mstPara->output2);		
			
			ParaPreserve(EEPROM_CANON_ARRAEXP,(wlSet.m_arrPara[WLM_ARR_A].output1<<4)|wlSet.m_arrPara[WLM_ARR_A].output2 );
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode )|(UART_ENCODE_WLMSET_ARRA<<4),(wlSet.m_arrPara[WLM_ARR_A].output1<<4)|wlSet.m_arrPara[WLM_ARR_A].output2 );
			//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
		}else if(WLM_SETTING_ARRB==g_SettingIndex)
		{
			if(8==wlSet.m_arrPara[WLM_ARR_B].output1)
			{
				wlSet.m_arrPara[WLM_ARR_B].output1 = 1;
				wlSet.m_arrPara[WLM_ARR_B].output2 = 0;
			}else
			{
				wlSet.m_arrPara[WLM_ARR_B].output2++;
				if(wlSet.m_arrPara[WLM_ARR_B].output2>2)
				{
					wlSet.m_arrPara[WLM_ARR_B].output1++;
					wlSet.m_arrPara[WLM_ARR_B].output2=0;
				}
			}
			DisplayCanMstHead(3,mainPara.workMode,&wlSet);

			ParaPreserve(EEPROM_CANON_ARRBEXP,(wlSet.m_arrPara[WLM_ARR_B].output1<<4)|wlSet.m_arrPara[WLM_ARR_B].output2 );
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRB<<4),(wlSet.m_arrPara[WLM_ARR_B].output1<<4)|wlSet.m_arrPara[WLM_ARR_B].output2 );
			//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
		}else if(WLM_SETTING_ARRC==g_SettingIndex)
		{
			if(8==wlSet.m_arrPara[WLM_ARR_C].output1)
			{
				wlSet.m_arrPara[WLM_ARR_C].output1 = 1;
				wlSet.m_arrPara[WLM_ARR_C].output2 = 0;
			}else
			{
				wlSet.m_arrPara[WLM_ARR_C].output2++;
				if(wlSet.m_arrPara[WLM_ARR_C].output2>2)
				{
					wlSet.m_arrPara[WLM_ARR_C].output1++;
					wlSet.m_arrPara[WLM_ARR_C].output2=0;
				}
			}
			DisplayCanMstHead(4,mainPara.workMode,&wlSet);

			ParaPreserve(EEPROM_CANON_ARRCEXP,(wlSet.m_arrPara[WLM_ARR_C].output1<<4)|wlSet.m_arrPara[WLM_ARR_C].output2 );
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRC<<4),(wlSet.m_arrPara[WLM_ARR_C].output1<<4)|wlSet.m_arrPara[WLM_ARR_C].output2 );
			//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
		}
#endif
		else if(WLM_SETTING_CHAN==g_SettingIndex)
		{
			*wlSet.m_chan++;
			if(*wlSet.m_chan>WL_CHANNEL_FOUR)
			{
				*wlSet.m_chan = WL_CHANNEL_ONE;
			}
			DisplayCanMstHead(5,mainPara.workMode,&wlSet);

			ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN, /*g_paraBakBuf[EEPROM_CANON_RATIO]*/((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
			//DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
			//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
		}
#endif
		break;

#ifdef _SPEED_LIGHT_
		case WORKINGMODE_F1:
		case WORKINGMODE_F2:
			if(8==mstPara->output1)
			{
				mstPara->output1 = 1;
				mstPara->output2 = 0;
			}else
			{
				mstPara->output2++;
				if(mstPara->output2>2)
				{
					mstPara->output1++;
					mstPara->output2=0;
				}
				
			}
			SetRmtPara(mstPara->output1,mstPara->output2);
			
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_OUTPUT,mstPara->output1,mstPara->output2);		
		break;
#endif
		

		case WORKINGMODE_MULTI:
		if(0==g_SettingIndex)
		{
#ifdef _CANON_
			if(WLM_ON == *wlSet.is_masterOn)
			{
				return;
			}
			if(0x60==mainPara.modGr[MASTER].multi_exp)	
			{
				mainPara.modGr[MASTER].multi_exp=0x10;
			}else
			{
				mainPara.modGr[MASTER].multi_exp+=0x10;
			}

			wlSet.m_arrPara[WLM_ARR_A].multi_exp = mainPara.modGr[MASTER].multi_exp;
			
			MultiCalMaxTime();
			SetMultiPara(mainPara.modGr[MASTER].multi_exp,mainPara.multi_times,mainPara.multi_Frq);

			ParaPreserve(EEPROM_INDEX_MULTI_EXPOSE,mainPara.modGr[MASTER].multi_exp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTI,UART_ENCODE_MULTI_EXPOSE,(mainPara.modGr[MASTER].multi_exp>>4));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
#else
			if(0x50==mainPara.modGr[MASTER].multi_exp)
			{
				mainPara.modGr[MASTER].multi_exp = 0x10;
			}else
			{
				if(2==(mainPara.modGr[MASTER].multi_exp&0x0f))
				{
					mainPara.modGr[MASTER].multi_exp +=0x10;
					mainPara.modGr[MASTER].multi_exp &= 0xf0;
				}else
				{
					mainPara.modGr[MASTER].multi_exp ++;
				}
			}
			MultiCalMaxTime();
			SetMultiPara(mainPara.modGr[MASTER].multi_exp,mainPara.multi_times,mainPara.multi_Frq);

			ParaPreserve(EEPROM_INDEX_MULTI_EXPOSE,mainPara.modGr[MASTER].multi_exp);
			ParaPreserve(EEPROM_INDEX_MULTI_TIMES,mainPara.multi_times);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTI,UART_ENCODE_MULTI_EXPOSE,mainPara.modGr[MASTER].multi_exp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);

#endif
		}else if(1==g_SettingIndex)
		{
#ifdef _NIKON_
			if(mainPara.multi_times<10)
			{
				mainPara.multi_times++;
			}else
			{
				mainPara.multi_times+=2;
			}

			if(mainPara.multi_times>90)
			{
				mainPara.multi_times = 90;
			}
#else
			if(mainPara.multi_times<20)
			{
				mainPara.multi_times++;
			}else if(mainPara.multi_times<50)
			{
				mainPara.multi_times+=5;
			}else
			{
				mainPara.multi_times+=10;
			}
#endif
			MultiCalMaxTime();

			LCD_MULTIME_RVS(mainPara.multi_times);
			MULTI_SET_FRQ(mainPara.multi_Frq);			

			ParaPreserve(EEPROM_INDEX_MULTI_TIMES,mainPara.multi_times);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
			
		}else if(2==g_SettingIndex)
		{
#ifdef _NIKON_
			if(mainPara.multi_Frq<10)
			{
				mainPara.multi_Frq++;
			}else 
			{
				mainPara.multi_Frq+=10;
			}
			if(mainPara.multi_Frq>100)
			{
				mainPara.multi_Frq = 100;
			}

#else
			if(mainPara.multi_Frq<20)
			{
				mainPara.multi_Frq++;
			}else if(mainPara.multi_Frq<50)
			{
				mainPara.multi_Frq+=5;
			}else
			{
				mainPara.multi_Frq+=10;
				if(mainPara.multi_Frq==200)
					mainPara.multi_Frq = 199;
			}
			if(mainPara.multi_Frq>199)
				mainPara.multi_Frq = 199;
#endif

			MultiCalMaxTime();
			MULTI_SET_ETIMES(mainPara.multi_times);
			LCD_MULFRQ_RVS(mainPara.multi_Frq);	

			ParaPreserve(EEPROM_INDEX_MULTI_FRQ,mainPara.multi_Frq);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
		}
#ifdef _CANON_
		else if(WLM_SETTING_RATION==g_SettingIndex)
		{
			*wlSet.arr_mode++;
			if(*wlSet.arr_mode>WLM_MODE_RES)
			{
				*wlSet.arr_mode = WLM_MODE_ALL;
			}
			DisplayCanMstHead(1,mainPara.workMode,&wlSet);

			//ParaPreserve(EEPROM_CANON_RATIO,(wlSet.is_masterOn<<7|wlSet.arr_mode));	
			ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
			//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
		}/*else if((WLM_SETTING_ARRA<=g_SettingIndex)&&(WLM_SETTING_ARRC>=g_SettingIndex))
		{
			if(0x60==(wlSet.m_arrPara[g_SettingIndex-3].mulOtp&0xf0))
			{
				wlSet.m_arrPara[g_SettingIndex-3].mulOtp=0x10;
			}else 
			{
				wlSet.m_arrPara[g_SettingIndex-3].mulOtp+=0x10;
			}
			//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
			//DisplayCanMstHead(2,mainPara.workMode,&wlSet);
			DisplayCanMstHead(g_SettingIndex-2,mainPara.workMode,&wlSet);

			ParaPreserve(g_SettingIndex+14,wlSet.m_arrPara[g_SettingIndex-3].mulOtp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,(mainPara.workMode<<4)|(g_SettingIndex-4),wlSet.m_arrPara[WLM_ARR_A].mulOtp);
		}*/
		else if(WLM_SETTING_ARRA==g_SettingIndex)
		{
			if(0x60==(wlSet.m_arrPara[WLM_ARR_A].multi_exp&0xf0))
			{
				wlSet.m_arrPara[WLM_ARR_A].multi_exp=0x10;
			}else 
			{
				wlSet.m_arrPara[WLM_ARR_A].multi_exp+=0x10;
			}
			//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
			DisplayCanMstHead(2,mainPara.workMode,&wlSet);

			mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].multi_exp;

			MultiCalMaxTime();
			//MULTI_SET_ETIMES(mainPara.multi_times);
			//LCD_MULFRQ_RVS(mainPara.multi_Frq);	
			
			SetMultiPara(mainPara.modGr[MASTER].multi_exp,mainPara.multi_times,mainPara.multi_Frq);	

			ParaPreserve(EEPROM_CANON_MULARRA,wlSet.m_arrPara[WLM_ARR_A].multi_exp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRA<<4),wlSet.m_arrPara[WLM_ARR_A].multi_exp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_MULTMFRQ,mainPara.multi_times,mainPara.multi_Frq);
		}else if(WLM_SETTING_ARRB==g_SettingIndex)
		{
			if(0x60==(wlSet.m_arrPara[WLM_ARR_B].multi_exp&0xf0))
			{
				wlSet.m_arrPara[WLM_ARR_B].multi_exp=0x10;
			}else 
			{
				wlSet.m_arrPara[WLM_ARR_B].multi_exp+=0x10;
			}
			//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
			DisplayCanMstHead(3,mainPara.workMode,&wlSet);

			ParaPreserve(EEPROM_CANON_MULARRB,wlSet.m_arrPara[WLM_ARR_B].multi_exp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRB<<4),wlSet.m_arrPara[WLM_ARR_B].multi_exp);
		}else if(WLM_SETTING_ARRC==g_SettingIndex)
		{
			if(0x60==(wlSet.m_arrPara[WLM_ARR_C].multi_exp&0xf0))
			{
				wlSet.m_arrPara[WLM_ARR_C].multi_exp=0x10;
			}else 
			{
				wlSet.m_arrPara[WLM_ARR_C].multi_exp+=0x10;
			}
			//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
			DisplayCanMstHead(4,mainPara.workMode,&wlSet);

			ParaPreserve(EEPROM_CANON_MULARRC,wlSet.m_arrPara[WLM_ARR_C].multi_exp);
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,mainPara.workMode|(UART_ENCODE_WLMSET_ARRC<<4),wlSet.m_arrPara[WLM_ARR_C].multi_exp);
		}else if(WLM_SETTING_CHAN==g_SettingIndex)
		{
			*wlSet.m_chan++;
			if(*wlSet.m_chan>WL_CHANNEL_FOUR)
			{
				*wlSet.m_chan = WL_CHANNEL_ONE;
			}
			//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
			DisplayCanMstHead(5,mainPara.workMode,&wlSet);

			ParaPreserve(EEPROM_CANON_RATIO,((*wlSet.is_masterOn<<7)|(*wlSet.m_chan<<4)|(*wlSet.arr_mode&0x03)));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_OPEN,((*wlSet.is_masterOn<<7)|((*wlSet.m_chan-1)<<4)|(*wlSet.arr_mode&0x03)));
			//DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
		}
#endif
		break;

#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			if((g_SettingIndex<5)&&(g_SettingIndex>0))
			{
				WlmArrRight();
				SetMastPara(*wlSet.m_chan,wlSet.m_arrPara);
			}else if(5==g_SettingIndex)
			{
				*wlSet.m_chan++;
				if(*wlSet.m_chan>WL_CHANNEL_FOUR)
				{
					*wlSet.m_chan = WL_CHANNEL_ONE;
				}
				SetMstChan(*wlSet.m_chan,DSP_REVERSE);

				ParaPreserve(EEPROM_INDEX_SWL_CHANARR,((*wlSet.m_chan<<6)|(wlSet.s_chan<<4) | wlSet.s_array));	
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WLMSET,UART_ENCODE_WLMSET_CHAN<<4,*wlSet.m_chan);
			}
		break;
#endif
		case WORKINGMODE_RTC:
		case WORKINGMODE_RTN:
			if(1==g_SettingIndex)
			{
				*wlSlave.s_array++;
				if(*wlSlave.s_array>WL_ARRARY_C)	
					*wlSlave.s_array = WL_ARRARY_A;
				//while(uart_tran_flag);
#ifdef _CANON_
				DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_ARR,(*wlSlave.s_array-1));
#endif
#ifdef _NIKON_
				DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_ARR,*wlSlave.s_array);
#endif
				//DisplayRmt(wlSet.s_array,wlSet.s_chan,g_SettingIndex);
			}else if(2==g_SettingIndex)
			{
				//wlSet.s_chan++;
				//if(wlSet.s_chan>3)
				//	wlSet.s_chan = 0;
				*wlSlave.s_chan++;
				if(*wlSlave.s_chan>WL_CHANNEL_FOUR)
				{
					*wlSlave.s_chan = WL_CHANNEL_ONE;
				}
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_CHAN,*wlSlave.s_chan);

				//DisplayRmt(wlSet.s_array,wlSet.s_chan,g_SettingIndex);
			}
			DisplayRmt(*wlSlave.s_array,*wlSlave.s_chan,g_SettingIndex);
			
			ParaPreserve(EEPROM_INDEX_SWL_CHANARR,((*wlSet.m_chan<<6)|(*wlSlave.s_chan<<4) | *wlSlave.s_array));
			////while(uart_tran_flag);
			//DecodeUartData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_CHAN,wlSet.s_chan);
		break;
		
	}
}
#endif
