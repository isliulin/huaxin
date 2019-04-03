;/**********************************************************************/
;/**********************************************************************/
#ifdef DOC

;TITLE:  スタートアップ(MN101EF51A)

;TITDOC: スタートアップルーチン

;DATE:   2008/11/7

#endif
;/**********************************************************************/
;/**********************************************************************/
#define STACKTOP 0x0400			;1kByte		/* @016YS[10/07/28]change 0x03ff -> 0x0400 */

;CPU制御レジスタ
#define	CPUM		0x3F00

;高速系制御レジスタ
#define	HANDSHAKE	0x3F06		;ハンドシェーク設定レジスタ 10MHz以上で必要
#define OSCCNT		0x3F1A		;高速発振制御レジスタ　　外部/内部の切換えと周辺への分周
#define	RCCNT		0x3F1B		;内蔵高速発振制御レジスタ
#define	OSCSCNT 	0x3F1C		;低速発振制御レジスタ
#define	RCSCNT 		0x3F1D		;内蔵低速発振制御レジスタ

#define CHDRVSEL	0x3DF0		;外部発振制御レジスタ

#define	PLLCNT		0x3F1F		;クロック逓倍回路制御レジスタ

#define WDCTR2		0x3F05		;ウォッチドッグタイマ２制御レジスタ

#define WD2MDSEL	0x3F4C		;ウォッチドッグタイマ２モード制御レジスタ
#define PRTKEY		0x3E50		;レジスタプロテクト制御レジスタ

;DEFINE定義
#define	SC_SLECT	1		;0:外部発振	1:内蔵発振

#define	BP_SLECT	1		;0:ICE/FLASH	1:デバッグプローブ使用時

#define	WAIT_EXT	30		; 1/2.5MHz * (8 + 2(HANDSHAKE分岐ロス)) * (30 + 1) = 約124us	/* @016YS[10/07/28]add */
							; (MURATA製CSTCR4M00G53の安定待ちは40usだが、なぜか64usでも切り替え後動作速度が安定しない)
#define	WAIT_TIME	20		; 外部発振10MHz時 1/1.25MHz * 7cycle * 20 = 112us
							; 外部発振 8MHz時 1/1.00MHz * 7cycle * 11 = 110us
							; 外部発振 4MHz時 1/0.50MHz * 7cycle *  8 = 112us

_STEXT		SECTION	CODE,PUBLIC,0
_CONST		SECTION	CODE,PUBLIC,1
_GCONST		SECTION	CODE,PUBLIC,1
_DATA		SECTION	DATA,PUBLIC,1
_GDATA		SECTION	DATA,PUBLIC,1
_ROMDATA	SECTION	CODE,PUBLIC,1
_GROMDATA	SECTION	CODE,PUBLIC,1
_ROMDATAEND	SECTION	CODE,PUBLIC,1
_ENTRY		SECTION CODE,PUBLIC,0
_TEXT		SECTION	CODE,PUBLIC,0
_BSS		SECTION	DATA,PUBLIC,1
_GBSS		SECTION	DATA,PUBLIC,1
_BSSEND		SECTION	DATA,PUBLIC,1

	GLOBAL	_enable_irq, _disable_irq, _vRAM_NMI

_STEXT		SECTION		; リンカで -T_STEXT=4000 を指定
	da	A(Reset)		;  0:リセットベクタのセット
	da	A(NoIRQ)	;  1:ノンマスカブル割込み		ウオッチドック（割込み）
;;	*			da	A(_int_IRQ0)	;  2:外部割込み０(PWM回転数入力)	/* @031YSx[10/10/01]delete */
	da	A(_int_Ext0_SCKINT)		;  2:外部割込み０
	da	A(_int_Ext1_FLASHINT)	;  3:外部割込み１(モータ センサ1)	(Stop_Wake_up)
	da	A(_int_Ext2_FlwFlash)	;  4:外部割込み２(モータ センサ2)	(Hook_Wake_up)
	da	A(_int_Ext3_WakeUp)	;  5:外部割込み３(モータ センサ3)	(Batt_Out)
	da	A(NoIRQ)	;  6:外部割込み４(モータ IPM Fo)	(Key_wake_up)
	da	A(NoIRQ)		;  7:LIN割込み
	da	A(NoIRQ)		;  8:電源電圧検知割込み
	da	A(_int_TM0)		;  9:タイマ０割込み		_int_TM0 set as send and receive clock		(DTMF_output)
	da	A(_int_TM1)	; 10:タイマ１割込み(500μs)			(Hold_Music)
	da	A(NoIRQ)		; 11:タイマ２割込み(800μs)
	da	A(NoIRQ)		; 12:
	da	A(NoIRQ)		; 13:
	da	A(NoIRQ)		; 14:タイマ６割込み
	da	A(NoIRQ)		; 15:タイムベース割り込み			(500ms Interrupt)
	da	A(NoIRQ)		; 16:タイマ７割込み		_int_TM7
	da	A(NoIRQ)		; 17:タイマ７ コンペア２一致割込み	(2ms Interrupt)
	da	A(NoIRQ)		; 18:
	da	A(NoIRQ)		; 19:
	da	A(NoIRQ)		; 20:タイマ９(MPWM) オーバーフロー割込み
	da	A(NoIRQ)	; 21:タイマ９(MPWM) アンダーフロー割込み
	da	A(NoIRQ)		; 22:タイマ９(MPWM) TCMPAコンペア一致割込み
;;	*			da	A(_int_SC0t)	; 23:シリアル０ UART送信割込み	/* @031YSx[10/10/01]delete */
	da	A(_int_SC0t)		; 23:
	da	A(NoIRQ)		; 24:シリアル１ UART送信割込み_IRQ_ClkSyn
;;	*			da	A(_int_SC0r)	; 25:シリアル０ UART受信割込み	/* @031YSx[10/10/01]delete */
	da	A(_int_SC0r)		; 25:
	da	A(NoIRQ)		; 26:シリアル１ UART受信割込み
	da	A(NoIRQ)		; 27:シリアル４ ストップコンディション割込み
	da	A(_IRQ_ClkSyn)		; 28:シリアル４割り込み
	da	A(_int_AD)		; 29:A/D変換割込み
	da	A(NoIRQ)		; 30:

#if BP_SLECT	;/* デバッグプローブ用 */
	da	A(MonInterrupt)	; 31: MonInterruptを定義 Don't Change!
#else
	da	A(NoIRQ)		; 31:
#endif



	org	0xc0		; _STEXT + 0xc0 (0x40c0)  フラッシュオプション領域

	dc	0x05		; Flashオプション WD2停止 STOP時低速発振停止可能

	org	0xd0		; _STEXT + 0xd0 (0x40d0) 番地へ。


_TEXT		SECTION
NoIRQ:				; 使用しない割り込み
	rti

	align 1

; リセットスタート時 内蔵発振20MHz   fs = 2.5MHzで動作 (CPUMの初期値が0x20の為、8分周される)

Reset:
	movw	STACKTOP,A0	; スタックポインタの初期化
	movw	A0,SP

	mov	0x30,D0		;
	mov	D0,PSW		; 全割込み禁止、割り込みマスクレベル３

; ハンドシェーク回路設定

	mov	0x04,(HANDSHAKE); ハンドシェーク回路ON	 以下の設定でfs=10MHz以上にする場合はONにする必要あり	

; 高速発振設定         注意：外部高速発振を選択する場合は2段階設定が必要 ⇒ 外部端子切換えと外部発振選択を同時に行わない事
#if SC_SLECT
	;内蔵高速発振を使用する場合
	mov	0x10,(OSCCNT)	; 内蔵高速発振選択、fpll-div停止
	mov	0x80,(RCCNT)	; 20MHz選択 16MHz選択時は"0x82"
				; この切り替えはfs=10MHz以下の時に行う事
#else
	;外部高速発振を使用する場合
	mov	0x11,(OSCCNT)	; P25/P26 機能選択 外部高速発振を選択、fpll-div停止
	mov	0,D0			; D0初期化、以後発振子安定待ち（発振子依存）	/* B@016YS[10/07/28]add */
ext_wait:
	add	1,D0			; 2cycle
	cmp	WAIT_EXT,D0		; 2cycle
	bls	ext_wait		; 4cycle WAIT_EXT >= D0 で ext_waitへ

	mov	0x00,(CHDRVSEL) ; 外部高速発振能力通常、最大設定時は"0x01"		/* E@016YS[10/07/28]add */
				; 低電圧動作時に最大にする必要あり、5V動作時は通常でOK

	bset	(OSCCNT)1	; 外部高速発振へ切換え
	
		; ここからfoscが外部発振に依存
		; 外部発振10MHzの場合、fs = 1.25MHzで動作

	nop					;内蔵発振停止前にnopが2つ必要		/* @016YS[10/07/28]add */
	nop					;					/* @016YS[10/07/28]add */

	mov	0x81,(RCCNT)	; 内蔵高速発振停止					/* @016YS[10/07/28]change 0x01 -> 0x81 */
				; 外部高速発振へ切り替えたあとに停止する事
#endif

; ウオッチドッグタイマ2設定
	mov	0x88,(PRTKEY)	; ウオッチドッグタイマ2設定レジスタ書き込み許可
	mov	0x00,(WD2MDSEL)	; WDT2停止
	mov	0x44,(PRTKEY)	; 以降のウオッチドッグタイマ設定レジスタ書き込み許可

; 低速発振設定
	mov	0x02,(OSCSCNT)	; 内部低速発振選択 、ポート選択
	mov	0x81,(RCSCNT)	; 内蔵低速発振停止

; PLL設定
#if SC_SLECT
	;内蔵高速発振を使用する場合
	mov	0x00,(PLLCNT)	; PLL未使用　この時点でfosc=20MHz or 16MHz
				; 
#else
	;外部高速発振を使用する場合
	mov	0x10,(PLLCNT)	; PLL使用 2逓倍を選択

	bset	(PLLCNT)0	; PLL動作開始

	mov	0,D0		; D0初期化
pll_wait:
	add	1,D0		; 2cycle
	cmp	WAIT_TIME,D0	; 2cycle
	bls	pll_wait	; 3cycle WAIT_TIME >= D0 で pll_waitへ

	bset	(PLLCNT)1	; PLLクロック動作開始
				; PLL使用　この時点でfosc=20MHz (外部発振10MHzの時) fs = 2.5MHz
#endif

; fs設定
	mov	0x10,(CPUM)	; fs = 1/1 fpll(frc) = 20MHz
		; 以降 fs = 20MHz 動作

; fpll-div設定
	mov	(OSCCNT),D0
	or	0x00,D0		; 
	mov	D0,(OSCCNT)	; 周辺機能用高速発振(fpll/div)分周比を選択

	bclr	(OSCCNT)4	; fpll-div動作開始


				; 静的変数の初期化
	movw	0x0000,A0	; _BSS 領域の先頭アドレス -> A0
	sub	D0,D0		; D0 = 0
clear1:
	mov	D0,(A0)
	addw	0x1,A0
	cmpw	_BSSEND,A0	; RAM 領域の最後か？
	blt	clear1		; _BSSEND > A0 で clear1 へ

				; 初期値付き静的変数の初期化
raminit:
	movw	_ROMDATA,A0
	movw	_DATA,A1
	cmpw	_GROMDATA,A0
	beq	next1
init1:				; ROMDATA(A0) -> DATA(A1)
	mov	(A0),D0
	mov	D0,(A1)
	addw	0x1,A0
	addw	0x1,A1
	cmpw	_GROMDATA,A0
	bne	init1

next1:
	movw	_GROMDATA,A0
	movw	_GDATA,A1
	cmpw	_ROMDATAEND,A0
	beq	next2
init2:				; GROMDATA(A0) -> GDATA(A1)
	mov	(A0),D0
	mov	D0,(A1)
	addw	0x1,A0
	addw	0x1,A1
	cmpw	_ROMDATAEND,A0
	bne	init2

next2:				; レジスタの初期化
	subw	DW0,DW0
	movw	DW0,DW1
	movw	DW0,A0
	movw	DW0,A1
				; メイン関数の呼び出し
_loop1:

	jsr	_main

	bra	_loop1

;
; 割込み制御関数
;

_enable_irq:			;全割込み許可関数
	or	0x40,PSW
	rts

_disable_irq:			;全割込み禁止関数
	and	0xBF,PSW
	rts

#if BP_SLECT					; モニタを利用しない場合は0（デバッグプローブ用）

;;;;; ここからモニタプログラムです。変更しないで下さい。
;;;;; ブレークポイントを貼らないでください。

#define DEBUG_CMDFLG	0		; CMDFLGクリア不具合対策
#define OCDMONVER       0x0100
#define OCDRESERVE1     0x55AA
#define OCDRESERVE2     0x33CC
#define MONCT           0x3F0C
#define MONDT           0x3F0D
#define MONCTxx         0x3FFE

ChangeMonitorModeFlag   equ  0x20        ;Monitor Mode ON Flag
ChangeDataFlag          equ  0x40        ;
ChangeCommandFlag       equ  0x80        ;
ClearFlag               equ  0x02        ;CommandFlag and DataFlag  ClearFlag
CheckMonitorModeFlag    equ  0x40        ;Monitor Mode Check Flag
CheckCommandFlag        equ  0x20        ;Monitor and Checkcommand Flag
CheckDataFlag           equ  0x10        ;Monitor and Cheack Data Flag
CheckESCBreak           equ  0x02        ;ESC Break Flag  
CheckRomEvent           equ  0x04        ;ROM Event Flag
CheckRamEvent           equ  0x08        ;RAM Event Flag

	GLOBAL   MonInterrupt

_STEXT2      SECTION CODE,PUBLIC,0
_STEXT2      SECTION
	align 1

OCDMonitorReserve:                       ; Monitor-code
    DW  OCDRESERVE1                      ; Monitor-code
    DW  OCDRESERVE2                      ; Monitor-code
    DW  OCDMONVER                        ; Monitor-version
    DW  OCDMonitorEnd - MonInterrupt     ; Monitor-code size

	align 1
MonInterrupt:                            ; DW0,DW1,A0,A1 to SP
    addw    -8,SP                        ; Monitor-code
    movw    dw0,(6,SP)                   ; Monitor-code
    movw    dw1,(4,SP)                   ; Monitor-code
    movw    a0,(2,SP)                    ; Monitor-code
    movw    a1,(0,SP)                    ; Monitor-code

OCDSetMonitorModeFlag:                   ; SET MONITOR
    mov     (MONCT),d0                   ; Monitor-code
    btst    0x40,d0                      ; Monitor-code
    bne     OCDWaitDataFlag              ; Monitor-code
    mov     ChangeMonitorModeFlag,d0     ; Monitor-code
    mov     d0,(MONCT)                   ; Monitor-code
    
OCDWaitDataFlag:                         ; CHECK DataFlag
#if DEBUG_CMDFLG
    mov     0,(MONCTxx)
#endif
    mov     (MONCT),d0                   ; Monitor-code
    btst    CheckDataFlag,d0             ; Monitor-code
    beq     OCDWaitDataFlag              ; Monitor-code

OCDSetSPLow:                             ; SET SP_L
    movw    SP,a0                        ; Monitor-code
    movw    a0,dw0                       ; Monitor-code
    mov     d0,(MONDT)                   ; Monitor-code

OCDClearDataFlag:                        ; CLR DATA and COMMAND
    mov     ClearFlag,d0                 ; Monitor-code
    mov     d0,(MONCT)                   ; Monitor-code

OCDWaitDataFlag2:                        ; CHECK DataFlag
#if DEBUG_CMDFLG
    mov     0,(MONCTxx)
#endif
    mov     (MONCT),d0                   ; Monitor-code
    btst    CheckDataFlag,d0             ; Monitor-code
    beq     OCDWaitDataFlag2             ; Monitor-code

OCDSetSPHi:                              ; SET SP_H
    mov     d1,(MONDT)                   ; Monitor-code

OCDClearDataAndCommandFlag:              ; CLR DATA and COMMAND
    mov ClearFlag,d0                     ; Monitor-code
    mov d0,(MONCT)                       ; Monitor-code

OCDCommandLoop:                          ; LOOP MON
#if DEBUG_CMDFLG
    mov     0,(MONCTxx)
#endif
    mov     (MONCT),d0                   ; Monitor-code
    btst    CheckMonitorModeFlag,d0      ; Monitor-code
    beq     OCDGoProgram                 ; Monitor-code
    btst    CheckCommandFlag,d0          ; Monitor-code
    beq     OCDCommandLoop               ; Monitor-code

    nop                                  ; for debug

OCDChangeSP:                             ; CHANGE SP
    movw    (6,SP),a0                    ; Monitor-code
    movw    a0,sp                        ; Monitor-code
    bra     OCDClearDataAndCommandFlag   ; Monitor-code
                                         ; Monitor-code
OCDGoProgram:
    movw    (6,SP),dw0                   ; Monitor-code
    movw    (4,SP),dw1                   ; Monitor-code
    movw    (2,SP),a0                    ; Monitor-code
    movw    (0,SP),a1                    ; Monitor-code
    addw    8,SP                         ; Monitor-code
    rti                                  ; Monitor-code
	align 1
OCDMonitorEnd:

#endif


	END
