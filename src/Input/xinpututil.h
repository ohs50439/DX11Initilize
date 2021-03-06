#pragma once
#include	<xinput.h>
#include <Windows.h>


#define MAX_CONTROLLERS 1  // ＸＢＯＸコントローラはＭＡＸ４つまで 
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

// XINPUT コントローラ情報
struct CONTROLLER_STATE
{
    XINPUT_STATE state;							// XINPUTコントローラの状態
    bool bConnected;							// 接続されているか否か
};

void InitXinputState();							// XINPUT 初期化;
HRESULT UpdateXinputControllerState();			// XINPUT UPDATE
WORD GetXinputButton(int i);					// ＸＢＯＸコントローラのボタン情報をＧＥＴ
int  GetXinputLeftTrigger(int idx);				// ＸＢＯＸコントローラの左トリガー値（０−２５５）をＧＥＴ
int  GetXinputRightTrigger(int idx);		    // ＸＢＯＸコントローラの右トリガー値（０−２５５）をＧＥＴ
int  GetXinputThumbLX(int idx);					// ＸＢＯＸコントローラの左アナログコントローラＸ値をＧＥＴ
int  GetXinputThumbLY(int idx);					// ＸＢＯＸコントローラの左アナログコントローラＹ値をＧＥＴ
int  GetXinputThumbRX(int idx);					// ＸＢＯＸコントローラの右アナログコントローラＸ値をＧＥＴ
int  GetXinputThumbRY(int idx);					// ＸＢＯＸコントローラの右アナログコントローラＹ値をＧＥＴ
void XinputSetDeadZone(int idx);				// 不正な値補正	
bool XinputConnected(int i);					// ＸＢＯＸコントローラが接続されているか？

bool GetXinputTriggerButton_A(int idx);
bool GetXinputTriggerButton_B(int idx);
bool GetXinputTriggerButton_Y(int idx);
bool GetXinputTriggerButton_X(int idx);
bool GetXinputTriggerR_SHOULDER(int idx);
bool GetXinputTriggerL_SHOULDER(int idx);
bool GetXinputTriggerSTART(int idx);
bool GetXinputTriggerBACK(int idx);