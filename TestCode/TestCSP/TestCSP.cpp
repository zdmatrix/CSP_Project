// TestCSP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "HED_CSP.h"
#include "cspdk.h"

int main(int argc, char* argv[])
{
	HCRYPTPROV hProv;
    HCRYPTKEY hKey;
    DWORD dwParam;
    BYTE *pbData;
    DWORD dwFlags;

	hProv = (HCRYPTPROV)malloc(sizeof(HCRYPTPROV));
	hKey = (HCRYPTKEY)malloc(sizeof(HCRYPTKEY));
	dwParam = KP_SALT;
	pbData = (BYTE*)malloc(sizeof(BYTE));
	dwFlags = 0x35;

	CPSetKeyParam(hProv, hKey, dwParam, pbData, dwFlags);
	printf("Hello World!\n");
	return 0;
}

