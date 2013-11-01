// HDIFD20B.cpp : Defines the entry point for the DLL application.
//
/************************************************************
	
Name:     HDIFD20B.cpp
Purpose:  For using Huada PCSC Driver
Designer: Chen Yue
First Code Date:     2004/2/26
Version : 1.0.4.1

//version updata:
1.0.4.2 : 更新HD_ResetCard热复位问题，完成HD_GetDescriptor，（1.0.4.1曾经提供给农行、格尔）
************************************************************/

#include "stdafx.h"
#include <stdio.h>

//	include Resource Manager definition
#include "winscard.h"

#include "HDIFD20B.h"
#include "HDIFD20Binc.h"

SCARD_IO_REQUEST	IO_Request;
SCARDCONTEXT		ContextHandle;

unsigned long		ret;
unsigned char		ReaderName[100][255];
unsigned long		ReaderID;
short Token;



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

void GetErrorCode(long ret)
{
	char MessageBuffer[255];

	switch (ret) {
		case SCARD_E_CANCELLED:
			MessageBox(NULL,"The action was cancelled by an SCardCancel request.","Err",MB_OK);
			break;

		case SCARD_E_CANT_DISPOSE:
			MessageBox(NULL,"The system could not dispose of the media in the requested manner.","Err",MB_OK);
			break;
		case SCARD_E_CARD_UNSUPPORTED:
			MessageBox(NULL,"The smart card does not meet minimal requirements for support.","Err",MB_OK);
			break;
		case SCARD_E_DUPLICATE_READER:
			MessageBox(NULL,"The reader driver didn't produce a unique reader name.","Err",MB_OK);
			break;
		case SCARD_E_INSUFFICIENT_BUFFER:
			MessageBox(NULL,"The data buffer to receive returned data is too small for the returned data.","Err",MB_OK);
			break;
		case SCARD_E_INVALID_ATR:
			MessageBox(NULL,"An ATR obtained from the registry is not a valid ATR string.","Err",MB_OK);
			break;
		case SCARD_E_INVALID_HANDLE:
			MessageBox(NULL,"The supplied handle was invalid.","Err",MB_OK);
			break;
		case SCARD_E_INVALID_PARAMETER:
			MessageBox(NULL,"One or more of the supplied parameters could not be properly interpreted.","Err",MB_OK);
			break;
		case SCARD_E_INVALID_TARGET:
			MessageBox(NULL,"Registry startup information is missing or invalid.","Err",MB_OK);
			break;
		case SCARD_E_INVALID_VALUE:
			MessageBox(NULL,"One or more of the supplied parameters� values could not be properly interpreted.","Err",MB_OK);
			break;
		case SCARD_E_NOT_READY:
			MessageBox(NULL,"The reader or card is not ready to accept commands.","Err",MB_OK);
			break;
		case SCARD_E_NOT_TRANSACTED:
			MessageBox(NULL,"An attempt was made to end a non-existent transaction.","Err",MB_OK);
			break;
		case SCARD_E_NO_MEMORY:
			MessageBox(NULL,"Not enough memory available to complete this command.","Err",MB_OK);
			break;
		case SCARD_E_NO_SERVICE:
			MessageBox(NULL,"The Smart card resource manager is not running.","Err",MB_OK);
			break;
		case SCARD_E_NO_SMARTCARD:
			MessageBox(NULL,"The operation requires a smart card but no smart card is currently in the device.","Err",MB_OK);
			break;
		case SCARD_E_PCI_TOO_SMALL:
			MessageBox(NULL,"The PCI Receive buffer was too small.","Err",MB_OK);
			break;
		case SCARD_E_PROTO_MISMATCH:
			MessageBox(NULL,"The requested protocols are incompatible with the protocol currently in use with the card.","Err",MB_OK);
			break;
		case SCARD_E_READER_UNAVAILABLE:
			MessageBox(NULL,"The specified reader is not currently available for use.","Err",MB_OK);
			break;
		case SCARD_E_READER_UNSUPPORTED:
			MessageBox(NULL,"The reader driver does not meet minimal requirements for support.","Err",MB_OK);
			break;
		case SCARD_E_SERVICE_STOPPED:
			MessageBox(NULL,"The Smart card resource manager has shut down.","Err",MB_OK);
			break;
		case SCARD_E_SHARING_VIOLATION:
			MessageBox(NULL,"The card cannot be accessed because of other connections outstanding.","Err",MB_OK);
			break;
		case SCARD_E_SYSTEM_CANCELLED:
			MessageBox(NULL,"The action was cancelled by the system presumably to log off or shut down.","Err",MB_OK);
			break;
		case SCARD_E_TIMEOUT:
			MessageBox(NULL,"The user-specified timeout value has expired.","Err",MB_OK);
			break;
		case SCARD_E_UNKNOWN_CARD:
			MessageBox(NULL,"The specified card name is not recognized.","Err",MB_OK);
			break;
		case SCARD_E_UNKNOWN_READER:
			MessageBox(NULL,"The specified reader name is not recognized.","Err",MB_OK);
			break;
		case SCARD_F_COMM_ERROR:
			MessageBox(NULL,"An internal communications error has been detected.","Err",MB_OK);
			break;
		case SCARD_F_INTERNAL_ERROR:
			MessageBox(NULL,"An internal consistency check failed.","Err",MB_OK);
			break;
		case SCARD_F_UNKNOWN_ERROR:
			MessageBox(NULL,"An internal error has been detected but the source is unknown.","Err",MB_OK);
			break;
		case SCARD_F_WAITED_TOO_LONG:
			MessageBox(NULL,"An internal consistency timer has expired.","Err",MB_OK);
			break;
		case SCARD_S_SUCCESS:
			MessageBox(NULL,"SCARD_S_SUCCESS","OK",MB_OK);
			break;
		case SCARD_W_REMOVED_CARD:
			MessageBox(NULL,"The card has been removed so that further communication is not possible.","Err",MB_OK);
			break;
		case SCARD_W_RESET_CARD:
			MessageBox(NULL,"The card has been reset so any shared state information is invalid.","Err",MB_OK);
			break;
		case SCARD_W_UNPOWERED_CARD:
			MessageBox(NULL,"Power has been removed from the card so that further communication is not possible.","Err",MB_OK);
			break;
		case SCARD_W_UNRESPONSIVE_CARD:
			MessageBox(NULL,"The card is not responding to a reset.","Err",MB_OK);
			break;
		case SCARD_W_UNSUPPORTED_CARD:
			MessageBox(NULL,"The reader cannot communicate with the card due to ATR configuration conflicts.","Err",MB_OK);
			break;
		default:
			sprintf(MessageBuffer,"Function returned 0x%X error code.", ret);
			MessageBox(NULL, MessageBuffer,"Err",MB_OK);
			break;
	}
}



//导出函数
/**********************************************************************************************
1.HD_GetReaderList
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_GetReaderList(
	unsigned long * ReaderNo,		//返回值，设备标识列表
	unsigned long * ReaderCount		//返回值，设备数量
)

参数说明
ReaderNo 		返回值，设备标识列表。	
ReaderCount		返回值，设备数量。

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_GetReaderList( int * ReaderNo, int * ReaderCount )
{
	SCARDHANDLE	CardHandle;
	unsigned char ResponseBuffer[4096];
	unsigned long ResponseLength;
	int i,j, StringLen;
	unsigned long ActiveProtocol;
	unsigned char ReaderNameTmp[255];

	Token = NULL;

	//
	//	Open a context which communication to the Resource Manager
	//
	ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &ContextHandle);

	if (ret != SCARD_S_SUCCESS) 
	{
//		GetErrorCode(ret);			
		return CER_PCSC_SCardEstablishContext ;
	}
	
	//
	//The SCardListCards function searches the smart card database 
	//and provides a list of named cards previously introduced to the system by the user.
	//
	//The caller specifies an ATR string, a set of interface identifiers (GUIDs), or both. 
	//If both an ATR string and an identifier array are supplied, the cards returned 
	//will match the ATR string supplied and support the interfaces specified.
	//

	ret = SCardListReaders(ContextHandle, NULL, NULL, &ResponseLength);
	if (ret != SCARD_S_SUCCESS) 
	{
//		GetErrorCode(ret);			
		return(CER_PCSC_SCardListReaders);
	}

	ret = SCardListReaders(ContextHandle, NULL, (char *)ResponseBuffer, &ResponseLength);
	if (ret != SCARD_S_SUCCESS) 
	{
//		GetErrorCode(ret);			
		return(CER_PCSC_SCardListReaders);
	}
/*
	ret = SCardListCards(ContextHandle, NULL, NULL, NULL, NULL, ResponseLength);
	if (ret != SCARD_S_SUCCESS) 
	{
		GetErrorCode(ret);			
		return(CER_PCSC_SCardListCards);
	}

	ret = SCardListCards(ContextHandle, NULL, NULL, NULL, ResponseBuffer, ResponseLength);
	if (ret != SCARD_S_SUCCESS) 
	{
		GetErrorCode(ret);			
		return(CER_PCSC_SCardListCards);
	}
*/

	ReaderID = 0;
	StringLen = 0;
	j=0;
	for(i=1;(unsigned long)i<ResponseLength+1;i++)
	{
		memcpy(ReaderNameTmp,ResponseBuffer+StringLen,i-StringLen);
		ret = SCardConnect(ContextHandle, (char *)ReaderNameTmp, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &CardHandle, &ActiveProtocol);
		if (ret == SCARD_S_SUCCESS)
		{
			memcpy(ReaderName[j], ReaderNameTmp,i);
			StringLen = i;
			j++;
			SCardDisconnect(CardHandle, SCARD_EJECT_CARD);
		}
	}

	*ReaderCount = j;
	ReaderID = j;

	for (i=0;i<j;i++)
		 ReaderNo[i] = i+1;
	
	SCardFreeMemory(ContextHandle,&ResponseLength);
	return EXCUTE_SUC ;	

}

/**********************************************************************************************
2.HD_ReleaseReaderList
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_GetReaderList(
)

参数说明

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_ReleaseReaderList()
{
	if( ContextHandle == NULL)
		return EXCUTE_SUC ;	
	//
	//The SCardReleaseContext function closes an established resource manager context, 
	//freeing any resources allocated under that context, 
	//including SCARDHANDLE objects and memory allocated using the SCARD_AUTOALLOCATE 
	//length designator.
	//

	ret = SCardReleaseContext(ContextHandle);

	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardReleaseContext;
	}

	ContextHandle = NULL;

	return EXCUTE_SUC ;	

}

/**********************************************************************************************
2.HD_OpenPort
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_OpenPort(
	short  			ivPortNo,		// 串口/USB 端口编号/PCSC设备编号
	unsigned long 	ivBaud,			// 通讯波特率	//USB接口不使用
	short  			ivParity,		// 校验方式		//USB接口不使用
	HANDLE 			*devNo				
)

参数说明
ivPortNo 		串口编号: Windows中可以是1，2，3或4。	
                USB 端口编号: 21, 22, 23, 24
				PCSC设备编号: 根据ReaderName对应选择
ivBaud			通讯波特率, 串口状态下有效;采用9600, 19200 , 38400, 115200 波特率通讯。
ivParity	    校验方式, 串口状态下有效;0无校验位(缺省);1奇校验;2偶校验;
devNo			返回的设备号

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_OpenPort(short ivPortNo,unsigned long ivBaud, short ivParity, HANDLE *devNo)
{
	SCARDHANDLE	CardHandle;
	unsigned long ActiveProtocol;

	if (((unsigned long)ivPortNo > ReaderID) || (ivPortNo < 0))
	{
		return CER_RANGE;
	}


	ret = SCardConnect(ContextHandle, (char *)ReaderName[ivPortNo-1], SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &CardHandle, &ActiveProtocol);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);			
		return CER_PCSC_SCardConnect ;
	}
/*
	if (Token != ivPortNo-1)
	{
		ret = SCardConnect(ContextHandle, (char *)ReaderName[ivPortNo-1], SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &CardHandle, &ActiveProtocol);
		if (ret != SCARD_S_SUCCESS)
		{
			GetErrorCode(ret);			
			return CER_PCSC_SCardConnect ;
		}
	}
*/
	*devNo =(HANDLE) CardHandle;
		
	return EXCUTE_SUC ;	

}

/**********************************************************************************************
3  HD_ResetCard 
对智能卡进行上电复位，取得复位应答信息。

unsigned short HD_ResetCard(		
	HANDLE 			devNo,
	char			*srATR,			
    short			*srATRLen,
	short 			ivCardSeat	//USB接口不使用
)

参数说明
devNo			    由HD_OpenPort返回的设备号
srATR            	存放收到的数据
srATRLen			存放接收到的数据的长度
ivCardSeat			卡座号。1：主卡座；2:副卡座

返回值
函数返回0x9000表示正确，此时srATR数据有效；其余值表示出错，错误信息见附后的“错误信息表”，此时srATR数据无效。

注意事项
该函数结束后，卡的RST管脚为高电平，卡进入工作状态。卡返回的复位应答字节存入srATR变量中。
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_ResetCard( HANDLE devNo, unsigned char	*srATR, short	*srATRLen, short ivCardSeat)
{

	DWORD ActiveProtocol;
	DWORD dwCardState=0;
	BYTE  AtrBuffer[32];
	DWORD AtrLen ;

	ret = SCardStatus((SCARDHANDLE)devNo, NULL, NULL, &dwCardState, &ActiveProtocol, NULL, &AtrLen);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardStatus;
	}

	switch ( dwCardState )
	{
		case SCARD_ABSENT:
	//		printf("Card absent.\n");
			return CER_NOCARD;

		case SCARD_SWALLOWED:
	//        printf("Card swallowed.\n");

			//
			//The SCardReconnect function reestablishes an existing connection between 
			//the calling application and a smart card. 
			//This function moves a card handle from direct access to general access, 
			//or acknowledges and clears an error condition that is preventing further 
			//access to the card.
			//

			//SCARD_LEAVE_CARD  Don't do anything special on reconnect. 
			//SCARD_RESET_CARD  Reset the card (Warm Reset). 
			//SCARD_UNPOWER_CARD  Power down the card and reset it (Cold Reset). 

			ret = SCardReconnect((SCARDHANDLE)devNo,SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 , SCARD_UNPOWER_CARD , &ActiveProtocol);
			if (ret != SCARD_S_SUCCESS)
			{
				GetErrorCode(ret);
				return CER_PCSC_SCardReconnect;
			}
			break;


		case SCARD_PRESENT:
	//        printf("Card present.\n");
		case SCARD_NEGOTIABLE:
	//        printf("Card reset and waiting PTS negotiation.\n");
		case SCARD_SPECIFIC:
	//        printf("Card has specific communication protocols set.\n");

			//
			//The SCardReconnect function reestablishes an existing connection between 
			//the calling application and a smart card. 
			//This function moves a card handle from direct access to general access, 
			//or acknowledges and clears an error condition that is preventing further 
			//access to the card.
			//

			//SCARD_LEAVE_CARD  Don't do anything special on reconnect. 
			//SCARD_RESET_CARD  Reset the card (Warm Reset). 
			//SCARD_UNPOWER_CARD  Power down the card and reset it (Cold Reset). 

			ret = SCardReconnect((SCARDHANDLE)devNo,SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 , SCARD_LEAVE_CARD , &ActiveProtocol);
			if (ret != SCARD_S_SUCCESS)
			{
				GetErrorCode(ret);
				return CER_PCSC_SCardReconnect;
			}
			break;

		case SCARD_POWERED:
	//        printf("Card has power.\n");

			//
			//The SCardReconnect function reestablishes an existing connection between 
			//the calling application and a smart card. 
			//This function moves a card handle from direct access to general access, 
			//or acknowledges and clears an error condition that is preventing further 
			//access to the card.
			//

			//SCARD_LEAVE_CARD  Don't do anything special on reconnect. 
			//SCARD_RESET_CARD  Reset the card (Warm Reset). 
			//SCARD_UNPOWER_CARD  Power down the card and reset it (Cold Reset). 

			ret = SCardReconnect((SCARDHANDLE)devNo,SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 , SCARD_RESET_CARD , &ActiveProtocol);
			if (ret != SCARD_S_SUCCESS)
			{
//				GetErrorCode(ret);
				return CER_PCSC_SCardReconnect;
			}
			break;

		default:
	//        printf("Unknown or unexpected card state.\n");
			return CER_UNKNOWN;
	}

	AtrLen = 0x20;		//Microsoft Define Atrlen<=0x20;
	ret = SCardStatus((SCARDHANDLE)devNo, NULL, NULL, &dwCardState, &ActiveProtocol, (LPBYTE)&AtrBuffer, &AtrLen);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardStatus;
	}

	*srATRLen = (short)AtrLen;
	memcpy(srATR, AtrBuffer, AtrLen);

	return EXCUTE_SUC ;	

}

/**********************************************************************************************
4  HD_ClosePort
关闭设备。该函数关闭由HD_OpenPort函数打开的设备。
unsigned short HD_ClosePort(
	HANDLE devNo
)

参数说明
devNo				由HD_OpenPort返回的设备号

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ClosePort(HANDLE devNo)
{
//	if( ContextHandle == NULL)
//		return EXCUTE_SUC ;	

	//
	//The SCardDisconnect function terminates a connection previously opened 
	//between the calling application and a smart card in the target reader.
	//

	//SCARD_LEAVE_CARD  Don't do anything special. 
	//SCARD_RESET_CARD  Reset the card. 
	//SCARD_UNPOWER_CARD  Power down the card. 
	//SCARD_EJECT_CARD  Eject the card. 

	ret = SCardDisconnect((SCARDHANDLE)devNo, SCARD_EJECT_CARD);
/*	
	if (ret != SCARD_S_SUCCESS)
	{
		GetErrorCode(ret);
		return CER_PCSC_SCardDisconnect;
	}
	

	ret = SCardCancel(ContextHandle);
	if (ret != SCARD_S_SUCCESS)
	{
		GetErrorCode(ret);
		return CER_PCSC_SCardCancel;
	}
*/

/*	//
	//The SCardReleaseContext function closes an established resource manager context, 
	//freeing any resources allocated under that context, 
	//including SCARDHANDLE objects and memory allocated using the SCARD_AUTOALLOCATE 
	//length designator.
	//

	ret = SCardReleaseContext(ContextHandle);

	if (ret != SCARD_S_SUCCESS)
	{
		GetErrorCode(ret);
		return CER_PCSC_SCardReleaseContext;
	}

	ContextHandle = NULL;
*/
	return EXCUTE_SUC ;	
}

/**********************************************************************************************
5  HD_PowerOn
给卡上电。上电结束后，卡的Vcc管脚为5V,CLK管脚提供一个3.684MHz的稳定时钟，RST管脚为低电平，I/O管脚为高电平。

unsigned short  HD_PowerOn(
HANDLE 		devNo,
short  		ivCardSeat	//USB接口不使用
)

参数说明
devNo			    由HD_OpenPort返回的设备号
ivCardSeat		    卡座号。1：主卡座；2:副卡座

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。

注意事项
该函数结束后，卡处在复位状态，即RST管脚为低电平。
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_PowerOn(HANDLE devNo, short ivCardSeat)
{
	DWORD ActiveProtocol;

	//
	//The SCardReconnect function reestablishes an existing connection between 
	//the calling application and a smart card. 
	//This function moves a card handle from direct access to general access, 
	//or acknowledges and clears an error condition that is preventing further 
	//access to the card.
	//

	//SCARD_LEAVE_CARD  Don't do anything special on reconnect. 
	//SCARD_RESET_CARD  Reset the card (Warm Reset). 
	//SCARD_UNPOWER_CARD  Power down the card and reset it (Cold Reset). 
	
	ret = SCardReconnect((SCARDHANDLE)devNo,SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 , SCARD_UNPOWER_CARD , &ActiveProtocol);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardReconnect;
	}

	return EXCUTE_SUC ;
}

/**********************************************************************************************
6  HD_PowerOff
给卡断电。该函数成功完成后，卡的各管脚与读卡器电隔离。

unsigned short HD_PowerOff(
	HANDLE 		devNo,
	short 		ivCardSeat	//USB接口不使用
)

参数说明
devNo			由HD_OpenPort返回的设备号
ivCardSeat		卡座号。1：主卡座；2:副卡座

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_PowerOff(	HANDLE 	devNo,short	ivCardSeat)
{
	//
	//The SCardDisconnect function terminates a connection previously opened 
	//between the calling application and a smart card in the target reader.
	//

	//SCARD_LEAVE_CARD  Don't do anything special. 
	//SCARD_RESET_CARD  Reset the card. 
	//SCARD_UNPOWER_CARD  Power down the card. 
	//SCARD_EJECT_CARD  Eject the card. 

	ret = SCardDisconnect((SCARDHANDLE)devNo, SCARD_UNPOWER_CARD);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardDisconnect;
	}

	return EXCUTE_SUC ;
}

/**********************************************************************************************
7 HD_ProbeCard
探测卡座中是否有卡存在。

unsigned short HD_ProbeCard(
HANDLE 		devNo,
short 		ivCardSeat	//USB接口不使用
)		

参数说明
devNo			 由HD_OpenPort返回的设备号
ivCardSeat	 	 卡座号。1：主卡座；2:副卡座

返回值
函数返回0x0000值表示无卡插入，0x9000表示有卡；其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ProbeCard(/*HANDLE devNo,*/short ivPortNo,  short ivCardSeat)	
{
	DWORD ActiveProtocol;
	DWORD dwCardState=0;
	DWORD AtrLen ;

	SCARDHANDLE	CardHandle;

/*	//
	//The SCardReconnect function reestablishes an existing connection between 
	//the calling application and a smart card. 
	//This function moves a card handle from direct access to general access, 
	//or acknowledges and clears an error condition that is preventing further 
	//access to the card.
	//

	//SCARD_LEAVE_CARD  Don't do anything special on reconnect. 
	//SCARD_RESET_CARD  Reset the card (Warm Reset). 
	//SCARD_UNPOWER_CARD  Power down the card and reset it (Cold Reset). 

	ret = SCardReconnect((SCARDHANDLE)devNo,SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 , SCARD_LEAVE_CARD , &ActiveProtocol);
	if (ret != SCARD_S_SUCCESS)
	{
		GetErrorCode(ret);
		return CER_PCSC_SCardReconnect;
	}

	ret = SCardStatus((SCARDHANDLE)devNo, NULL, NULL, &dwCardState, &ActiveProtocol, NULL, &AtrLen);
	if (ret != SCARD_S_SUCCESS)
	{
		GetErrorCode(ret);
		return CER_PCSC_SCardStatus;
	}

*/
	
	Token= ivPortNo-1;

	ret = SCardConnect(ContextHandle, (char *)ReaderName[ivPortNo-1], SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &CardHandle, &ActiveProtocol);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);			
		return CER_PCSC_SCardConnect ;
	}

	ret = SCardStatus(CardHandle, NULL, NULL, &dwCardState, &ActiveProtocol, NULL, &AtrLen);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardStatus;
	}
	
//	ret = SCardDisconnect(CardHandle, SCARD_EJECT_CARD);
	ret = SCardDisconnect(CardHandle, SCARD_UNPOWER_CARD);


	switch ( dwCardState )
	{
		case SCARD_ABSENT:
	//		printf("Card absent.\n");
			return CER_NOCARD;
		case SCARD_PRESENT:
	//        printf("Card present.\n");
			return EXCUTE_SUC;
		case SCARD_SWALLOWED:
	//        printf("Card swallowed.\n");
			return EXCUTE_SUC;
		case SCARD_POWERED:
	//        printf("Card has power.\n");
			return EXCUTE_SUC;
		case SCARD_NEGOTIABLE:
	//        printf("Card reset and waiting PTS negotiation.\n");
			return EXCUTE_SUC;
		case SCARD_SPECIFIC:
	//        printf("Card has specific communication protocols set.\n");
			return EXCUTE_SUC;
		default:
	//        printf("Unknown or unexpected card state.\n");
			return CER_NOCARD;
	}

}
/**********************************************************************************************
8  HD_ApduT0
卡命令处理函数,该命令接收APDU Case1 Case2 Case3 Case4格式的指令。

unsigned short HD_ApduT0(				
    HANDLE 				devNo,
	char 				*apduData,
	short				srAPDULen,
	char 				*respData,
	short				*respDataLen,
	short 				ivCardSeat	//USB接口不使用
)

参数说明
devNo				由HD_OpenPort返回的设备号
apduData			APDU命令(字节串)用CLA、INS、P1、P2以及一个可变长度的条件体来表示，条件体的定义为Lc、Data（Lc个字节）和Le。
					根据Lc和Le值的不同，共有四种命令情况，如下表所示：
					情况	结构
					1	CLA INS P1 P2
					2	CLA INS P1 P2 Le
					3	CLA INS P1 P2 Lc Data
					4	CLA INS P1 P2 Lc Data Le
srAPDULen			存放发送的APDU数据的长度
respData			卡响应数据
respDataLen			卡响应数据的长度
ivCardSeat	 		卡座号

返回值
函数返回0xffXX表示命令未成功执行，错误信息见附后的“错误信息表”；其余值为卡的响应。
当操作Case1和 Case2时，respData无意义。当操作Case3和 Case4时，函数返回0x9000，respData有意义；其余响应，respData无意义。
***********************************************************************************************/
unsigned short isobase2(HANDLE devNo,unsigned char *svAPDU,short svAPDULen,unsigned char *srResp,short *srRespLen, short ivCardSet)
{
	BYTE  ResponseBuffer[300];
	DWORD ResponseLength ;

	WORD retval = CER_PCSC_SCardTransmit_Data_LOST;

	*srRespLen = 0 ;
	*srResp = '\0' ;

	IO_Request.dwProtocol = SCARD_PROTOCOL_T0;
	IO_Request.cbPciLength = (DWORD) sizeof(SCARD_IO_REQUEST);


	ResponseLength=300;

	ret = SCardTransmit((SCARDHANDLE)devNo, &IO_Request, svAPDU, svAPDULen, 0, (BYTE *)&ResponseBuffer, &ResponseLength);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardTransmit;
	}


	if ( ResponseLength >= 2 )
	{

		retval = ((BYTE)ResponseBuffer[ResponseLength-2]) * 256 + (BYTE)ResponseBuffer[ResponseLength-1] ;

		*srRespLen = (short)ResponseLength - 2;
		memcpy(srResp,ResponseBuffer,ResponseLength-2);

	}
	else
		retval =  CER_PCSC_SCardTransmit_Data_LOST;

	return retval ;
}

unsigned short isobase3(HANDLE devNo,unsigned char *svAPDU,short svAPDULen,unsigned char *srResp,short *srRespLen, short ivCardSet)
{
	BYTE  ResponseBuffer[300];
	DWORD ResponseLength ;

	WORD retval = CER_PCSC_SCardTransmit_Data_LOST;


	IO_Request.dwProtocol = SCARD_PROTOCOL_T0;
	IO_Request.cbPciLength = (DWORD) sizeof(SCARD_IO_REQUEST);

	
	ResponseLength=300;

	ret = SCardTransmit((SCARDHANDLE)devNo, &IO_Request, svAPDU, svAPDULen, 0, (BYTE *)&ResponseBuffer, &ResponseLength);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardTransmit;
	}


	if ( ResponseLength >= 2 )
	{
		retval = ((BYTE)ResponseBuffer[ResponseLength-2]) * 256 + (BYTE)ResponseBuffer[ResponseLength-1] ;
	}
	else
		retval =  CER_PCSC_SCardTransmit_Data_LOST;

	return retval ;

}

HDIFD20B_API unsigned short CCONV HD_ApduT0( HANDLE devNo,  unsigned char *svAPDU, short srAPDULen, unsigned char *respData, short *respDataLen, short ivCardSeat )
{
	unsigned short retv;
	unsigned char getresp[12];
	unsigned char tmpTPDU[600];

	*respData = '\0';

	if( srAPDULen < 4 || srAPDULen > 261 )
		return CER_APDU;			// 非法APDU字节数

	memcpy( tmpTPDU, svAPDU,srAPDULen);
	
    // begin transaction locking
    // check that handle to card is valid
    ret = SCardBeginTransaction((SCARDHANDLE)devNo);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardBeginTransaction;
	}

	if( srAPDULen == 4 )
	{
		// isocase = 1
		tmpTPDU[4]=0x00;
		retv = isobase2(devNo,tmpTPDU,5, respData, respDataLen, ivCardSeat); 

		if(  HIBYTE(retv) == 0x61)
		{
			memcpy(getresp, "\x00\xC0\x00\x00",4);
			getresp[4]= LOBYTE(retv);

			retv = isobase2(devNo,getresp,5, respData, respDataLen, ivCardSeat);
		}
		else if(  HIBYTE(retv) == 0x6C)
		{
			tmpTPDU[4]= LOBYTE(retv);	// 补上新的Le字节
			retv = isobase2(devNo,tmpTPDU,5, respData, respDataLen, ivCardSeat); 
		}
	}

	else if( srAPDULen == 5 )
	{
		// isocase = 2
		retv = isobase2(devNo,tmpTPDU, 5,respData, respDataLen,ivCardSeat); 
		if(  HIBYTE(retv) == 0x61)
		{
			memcpy(getresp, "\x00\xC0\x00\x00",4);
			getresp[4]= LOBYTE(retv);

			retv = isobase2(devNo,getresp,5, respData, respDataLen, ivCardSeat);
		}
		else if(  HIBYTE(retv) == 0x6C)
		{
			tmpTPDU[4]= LOBYTE(retv);	// 补上新的Le字节
			retv = isobase2(devNo,tmpTPDU,5, respData, respDataLen, ivCardSeat); 
		}
	}

	else if( srAPDULen-5 == (unsigned char)svAPDU[4] )
	{
		// isocase = 3
		retv = isobase3(devNo,tmpTPDU,srAPDULen,respData, respDataLen, ivCardSeat); 

		if(  HIBYTE(retv) == 0x61)
		{
			memcpy(getresp, "\x00\xC0\x00\x00",4);
			getresp[4]= LOBYTE(retv);
			retv = isobase2(devNo,getresp,5, respData, respDataLen, ivCardSeat);
		}
		
	}
	else if( srAPDULen-6 == (unsigned char)svAPDU[4] )
	{
		// isocase = 4;
		retv = isobase3(devNo,tmpTPDU,srAPDULen-1,respData, respDataLen, ivCardSeat); // 去掉Le
		if(  HIBYTE(retv) == 0x61)
		{
			memcpy(getresp, "\x00\xC0\x00\x00",4);
			getresp[4]= LOBYTE(retv);
			retv = isobase2(devNo,getresp,5, respData, respDataLen, ivCardSeat);
		}
	}
	else
		return CER_APDU;			// 非法APDU Case结构

    //end transaction locking
    ret = SCardEndTransaction((SCARDHANDLE)devNo, SCARD_LEAVE_CARD);
	if (ret != SCARD_S_SUCCESS)
	{
		GetErrorCode(ret);
		return CER_PCSC_SCardEndTransaction;
	}

	return retv;
}

/**********************************************************************************************
9 HD_GetDescriptor
获取读卡设备描述
   
unsigned short HD_GetDescriptor(		
	HANDLE 		devNo,		
	char 		*descriptor
)

参数说明
devNo			由HD_OpenPort返回的设备号
descriptor		读卡设备信息,对于CI-4型双卡座读卡器，该信息意义为“CIDC CI-4-1.0-009600“(十六进制串)
				对于USBKey，硬件序列号是对CPU IC卡做FORMAT时写入卡内的历史字节

返回值
函数返回0x9000表示正确，此时descriptor有效。其余值表示出错，错误信息见附后的“错误信息表”，此时descriptor数据无效。
***********************************************************************************************/
/*
void byte2str_chen( char *svHex, unsigned char ivByte )
{
	char subs[5];

	sprintf( subs, "%02X", ivByte);
	strcat( svHex, subs );
	return;
}
*/
HDIFD20B_API unsigned short CCONV HD_GetDescriptor( HANDLE devNo, unsigned char *descriptor , short * descriptorlen)
{
	unsigned char tmpstr[100] ;
	unsigned long pcbAttrLen;
//	unsigned short i;

	//
	//The SCardGetAttrib function gets the current reader attributes for the given handle. 
	//It does not affect the state of the reader, driver, or card.
	//
	
	ret = SCardGetAttrib((SCARDHANDLE)devNo, SCARD_ATTR_DEVICE_FRIENDLY_NAME, NULL, &pcbAttrLen);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardGetAttrib;
	}

	ret = SCardGetAttrib((SCARDHANDLE)devNo, SCARD_ATTR_DEVICE_FRIENDLY_NAME, tmpstr, &pcbAttrLen);
	if (ret != SCARD_S_SUCCESS)
	{
//		GetErrorCode(ret);
		return CER_PCSC_SCardGetAttrib;
	}

	/*
	descriptor[0] = '\0' ;
	for( i=0 ; i < pcbAttrLen ; i++) 
		byte2str_chen(descriptor,tmpstr[i]) ;
	*/

	memcpy(descriptor,tmpstr,pcbAttrLen);
	* descriptorlen = (unsigned short)pcbAttrLen;
	SCardFreeMemory(ContextHandle,&pcbAttrLen);

	return EXCUTE_SUC ;

}