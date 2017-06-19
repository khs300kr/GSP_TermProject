#pragma once

void Init_DB(void);
void Close_DB(void);
//
void Client_Login(WCHAR id[], int ci);
void Client_Logout(WCHAR id[], WORD x, WORD y, BYTE Level, WORD Exp, WORD HP, WORD ATT, int gold, int ci);
