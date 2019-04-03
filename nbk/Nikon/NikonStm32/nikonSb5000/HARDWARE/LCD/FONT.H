#ifndef __FONT_H
#define __FONT_H 	   

//extern const char picScan_1[];
//extern const char picScan_2[];
//extern const char picScan_3[];
//extern const char picTest[];


typedef struct SNR_SUB_FONT
{
	char ch;
	char wt;
	char buf[24];
}SnrSubFont;

typedef struct SNR_SUB_SYMBOL
{
	char ch;
	char wt;
	char buf[24];
}SnrSubSymbol;

extern const char font_isoNum[];
extern const char font_apertNum[];
extern const char font_scaleNum[];
extern const char temp_warn[];
extern const char picStby[];
extern const char picBack[];
extern const char pic_mod_ttl[];
extern const char pic_mod_a[];
extern const char pic_mod_gn[];
extern const char pic_mod_m[];
extern const char pic_mod_rpt[];
extern const char font_mExpApart[];
extern const char font_exp2Num[];
extern const char picCmpAdd[];
extern const char picCmpSub[];
extern const char picExp2Dot[];
extern const char picEv[];
extern const char picIso[];
extern const char picScale_m[];
extern const char picZoom[];
extern const char picMm[];
extern const char picZoomMin[];
extern const char picZoomMax[]; 
extern const char picApert_F[];
extern const char picBtnZoom[];
extern const char picBtnCmp[];
extern const char picBtnMsc[];
extern const char picBtnFlash[];
extern const char picBtnSel[];
extern const char picBtn_M[];
extern const char picBtn_m[];
extern const char picBtnTimesHz[];
extern const char picBtnHead[];
extern const char picBtnFno[];
extern const char picBtnCh[];
extern const char picScale_ft[];
extern const char picGnScale_ft[];
extern const char picGnScale_m[];
extern const char picCurse[];
extern const char fontScaleDot[];
extern const char fontScale_m[];
extern const char fontScale_div[];
extern const char fontGn_Arrow_Left[];
extern const char fontGn_Arrow_Right[];
extern const char font_gnScaleSmNum[];
extern const char picGnSmDot[];
/*
extern const char picSnrNO1[];
extern const char picSnrNO2[];
extern const char picSnrNO3[];
extern const char picSnrNO4[];
extern const char picSnrNO5[];
extern const char picSnrNO6[];
extern const char picSnrNO7[];
extern const char picSnrNO8[];
extern const char picSnrNO9[];
extern const char picSnrNO10[];
extern const char picSnrNO11[];
extern const char picSnrNO12[];
extern const char picSnrNO13[];
extern const char picSnrNO14[];
extern const char picSnrNO15[];
extern const char picSnrNO16[];
extern const char picSnrNO17[];
extern const char picSnrNO18[];
extern const char picSnrNO19[];
extern const char picSnrMask[];
*/

extern const char picBtnPgUp[];
extern const char picBtnPgDn[];
//extern const char picBtnSnrSet[];
extern const char picBtnNext[];
/*
extern const char picSnrSub_2_1[];
extern const char picSnrSub_2_2[];
extern const char picSnrSub_2_3[];
extern const char picSnrSub_2_4[];
extern const char picSnrSub_2_5[];
extern const char picSnrSub_2_6[];
*/
extern const char picSnrSub_3_1[];
extern const char picSnrSub_3_2[];
extern const char picSnrSub_3_3[];
extern const char picSnrSub_3_4[];
/*extern const char picSnrSub_3_5[];*/
extern const char picSnrSub_5_1[];
extern const char picSnrSub_5_2[];
extern const char picSnrSub_7_1[];
extern const char picSnrSub_13_1[];
extern const char picSnrSub_13_2[];
extern const char picSnrSub_13_3[];
extern const char picSnrSub_13_4[];
extern const char picSnrSub_13_selected[];
extern const char picSnrSub_13_bk[];
extern const char picSnrSub_14_1[];
extern const char picSnrSub_14_2[];
//extern const char picSnrSub_16_1[];
extern const char picSnrSub_16_wpoff[];
extern const char picSnrSub_16_wpon[];
extern const char picSnrSub_16_zoom[];
extern const char picSnrSub_16_M[];
extern const char picSnrSub_17_1[];
extern const char picSnrSub_17_2[];
extern const char picSnrSub_17_3[];
extern const char picSnrSub_9_3[];
extern const char picSnrSubArrow[];
extern const char picSnrSub_2_bk[];
extern const char picSnrSub_2_msk[];
extern const char picSnrFx[];
extern const char picSnrDx[];
extern const char picSnrSub_8_mask[];
extern const char lightHead[];
extern const char picSnrSub_isoUP[];
extern const char picSnrSub_isoDN[];
extern const char picSnrSubMyMenu_sel[];
extern const char picSnrSubMyMenu_unsel[];
extern const char picSnrSubBack[];
extern const char picSnrSubMyMenuSel[];
extern const char picBtnSnrSet_My[];
extern const char picBtnSnrSet_ALL[];
extern const char picSnrSubArrowEx[];
extern const char picSnrSub_9_af[];
extern const char picSnrFrame[];
extern const char picGnFoc[];
extern const char picGnFocMask[];
extern const char picMultiTimes[];
extern const char picMultiHz[];
extern const char picMultiTmsHz[];
extern const char picRmtIcon[];
extern const char picMstIcon[];
extern const char picRmtTitle[];
extern const char picRmtGr[];
extern const char picRmtCh[];
extern const char picRmtDot[];
extern const char picRmtGrMsk[];
extern const char picRmtChMsk[];
extern const char picRmtLeft[];
extern const char picRmtRight[];
extern const char fontRmtGr[];
extern const char fontRmtGrSel[];
extern const char fontRmtCh[];
extern const char picMstFrame[];
extern const char picMstGrm[];
extern const char picMstGra[];
extern const char picMstGrb[];
extern const char picMstGrc[];
extern const char picMstModM[];
extern const char picMstModTtl[];
extern const char picMstModAuto[];
extern const char picMstModOff[];
extern const char picMstExpHead[];
extern const char picMstEv[];
extern const char fontMstManuExp[];
extern const char picMstRptTimes[];
extern const char picMstRptHz[];
extern const char picMstRptOn[];
extern const char picMstRptOff[];
extern const char fontSu4[];
extern const char picRmtSu4ModOff[];
extern const char picRmtSu4ModAuto[];
extern const char picChFrameMask[];
extern const char picChFrame[];
extern const char fontMstCh[];
extern const char picLightBody_up[];
extern const char picLightBody_un[];
extern const char picLightBody_bk[];
extern const char picLightBody_fr_parallel[];
extern const char picLightBody_fr_reflect[];
extern const char picLightBody_af[];
extern const char picTempratureWarn[];
extern const char picVoiceOff[];
extern const char picVoiceOn[];
extern const char picLowBat[];
extern const char picLinkCls[];
extern const char picLinkFla[];
extern const char pic_mod_bl[];
extern const char pic_mod_fp[];
extern const char pic_mod_ak[];
extern const char picBtnZoomBack[];
extern const char picBtnModeling[];
extern const char pic_mod_afill[];
extern const char pic_mod_rfOnly[];
extern const char pic_mod_Only[] ;
extern const char picScaleOver[];
extern const char picScaleUnder[];
extern const char picBtnLock[];
extern const char fontScale_ft[];
extern const char picBtnFlashMode[];
extern const char picHeadUp[];
extern const char picHeadDn[];
extern const char picZoomModDx[];
extern const char picZoomModFx[];
extern const char picZoomModZoom[];
extern const char picZoomModM[];
extern const char picReset[];
extern const char picZoomFixedFrame[];
extern const char picBtn_ft[];
extern const char picBtnFlashWeak[];
extern const char picIsoAuto[];
extern const char picMenuShadowLeft_1[];
extern const char picMenuShadowLeft_2[];
extern const char picMenuShadowRight_1[];
extern const char picMenuShadowRight_2[];
//extern const char picZoomSetaFrame[];
extern const char picSubModRpt[];
extern const char picSubModManu[];
extern const char picSubModGn[];
extern const char picSubModAuto[];
extern const char picSubModFunc[];

extern const SnrSubFont fontSnrSubChar[];

//########################################################
// SB5000 MENU
//########################################################
extern const char picMenuMode[];
extern const char picMenuAuto[];
extern const char picMenuFxDx[];
extern const char picMenuAf[];
extern const char picMenuIso[];
extern const char picMenuReset0[];
extern const char picMenuChan[];
extern const char picMenuLinkMode[];
extern const char picMenuPair[];
extern const char picMenuPin[];
extern const char picMenuName[];
extern const char picMenuLcd[];
extern const char picMenuReady[];
extern const char picMenuMsc[];
extern const char picMenuStby[];
extern const char picMenuLight[];
extern const char picMenuMft[];
extern const char picMenuVer[];
extern const char picMenuReset1[];
extern const char picMenuLabel[];
extern const char picMenuLabel_1[];
extern const char picMenuLabel_2[];
extern const char picMenuLabel_3[];
extern const char picMenuLabel_f1[];
extern const char picMenuLabel_f2[];
extern const char picMenuLabel_f3[];
extern const char picMenuLabel_s1[];
extern const char picMenuLabel_s2[];
extern const char picMenuLabel_s3[];
//extern const char picMenuLabel_c1[];
//extern const char picMenuLabel_c2[];
//extern const char picMenuLabel_c3[];

#define FONT_WT_APERT  8
#define FONT_WT_SCALE 10
#define FONT_WT_ISO      5

#define FONT_HT_N  16
#define FONT_HT_S  8

#define FONT_STYLE_ISO           1
#define FONT_STYLE_APERT       2
#define FONT_STYLE_SCALE       3

#define FONT_SNRSUB_LEN 53//32

#endif




/*###################ASCII#############################
  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
*/
