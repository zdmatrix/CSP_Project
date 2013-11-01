// HDIFD20B.cpp : Defines the entry point for the DLL application.
//
/************************************************************
	
Name:     HDIFD20B.cpp
Purpose:  For using Huada PCSC Driver
Designer: Chen Yue
First Code Date:     2004/2/26
Version : 1.0.4.1

//version updata:
1.0.4.2 : ¸üÐÂHD_ResetCardÈÈ¸´Î»ÎÊÌâ£¬Íê³ÉHD_GetDescriptor£¬£¨1.0.4.1Ôø¾­Ìá¹©¸øÅ©ÐÐ¡¢¸ñ¶û£©
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
			MessageBox(NULL,"One or more of the supplied parameters’ values could not be properly interpreted.","Err",MB_OK);
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



//µ¼³öº¯Êý
/**********************************************************************************************
1.HD_GetReaderList
Á¬½ÓÉè±¸¡£Óë¶Á¿¨Æ÷Í¨/USB KEY Í¨Ñ¶Ç°±ØÐëÏÈ´ò¿ªÉè±¸¡£
unsigned short HD_GetReaderList(
	unsigned long * ReaderNo,		//·µ»ØÖµ£¬Éè±¸±êÊ¶ÁÐ±í
	unsigned long * ReaderCount		//·µ»ØÖµ£¬Éè±¸ÊýÁ¿
)

²ÎÊýËµÃ÷
ReaderNo 		·µ»ØÖµ£¬Éè±¸±êÊ¶ÁÐ±í¡£	
ReaderCount		·µ»ØÖµ£¬Éè±¸ÊýÁ¿¡£

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£¬´ËÊ±devNoÓÐÐ§£»ÆäËüÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£
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
Á¬½ÓÉè±¸¡£Óë¶Á¿¨Æ÷Í¨/USB KEY Í¨Ñ¶Ç°±ØÐëÏÈ´ò¿ªÉè±¸¡£
unsigned short HD_GetReaderList(
)

²ÎÊýËµÃ÷

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£¬´ËÊ±devNoÓÐÐ§£»ÆäËüÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£
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
Á¬½ÓÉè±¸¡£Óë¶Á¿¨Æ÷Í¨/USB KEY Í¨Ñ¶Ç°±ØÐëÏÈ´ò¿ªÉè±¸¡£
unsigned short HD_OpenPort(
	short  			ivPortNo,		// ´®¿Ú/USB ¶Ë¿Ú±àºÅ/PCSCÉè±¸±àºÅ
	unsigned long 	ivBaud,			// Í¨Ñ¶²¨ÌØÂÊ	//USB½Ó¿Ú²»Ê¹ÓÃ
	short  			ivParity,		// Ð£Ñé·½Ê½		//USB½Ó¿Ú²»Ê¹ÓÃ
	HANDLE 			*devNo				
)

²ÎÊýËµÃ÷
ivPortNo 		´®¿Ú±àºÅ: WindowsÖÐ¿ÉÒÔÊÇ1£¬2£¬3»ò4¡£	
                USB ¶Ë¿Ú±àºÅ: 21, 22, 23, 24
				PCSCÉè±¸±àºÅ: ¸ù¾ÝReaderName¶ÔÓ¦Ñ¡Ôñ
ivBaud			Í¨Ñ¶²¨ÌØÂÊ, ´®¿Ú×´Ì¬ÏÂÓÐÐ§;²ÉÓÃ9600, 19200 , 38400, 115200 ²¨ÌØÂÊÍ¨Ñ¶¡£
ivParity	    Ð£Ñé·½Ê½, ´®¿Ú×´Ì¬ÏÂÓÐÐ§;0ÎÞÐ£ÑéÎ»(È±Ê¡);1ÆæÐ£Ñé;2Å¼Ð£Ñé;
devNo			·µ»ØµÄÉè±¸ºÅ

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£¬´ËÊ±devNoÓÐÐ§£»ÆäËüÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£
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
¶ÔÖÇÄÜ¿¨½øÐÐÉÏµç¸´Î»£¬È¡µÃ¸´Î»Ó¦´ðÐÅÏ¢¡£

unsigned short HD_ResetCard(		
	HANDLE 			devNo,
	char			*srATR,			
    short			*srATRLen,
	short 			ivCardSeat	//USB½Ó¿Ú²»Ê¹ÓÃ
)

²ÎÊýËµÃ÷
devNo			    ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ
srATR            	´æ·ÅÊÕµ½µÄÊý¾Ý
srATRLen			´æ·Å½ÓÊÕµ½µÄÊý¾ÝµÄ³¤¶È
ivCardSeat			¿¨×ùºÅ¡£1£ºÖ÷¿¨×ù£»2:¸±¿¨×ù

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£¬´ËÊ±srATRÊý¾ÝÓÐÐ§£»ÆäÓàÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±£¬´ËÊ±srATRÊý¾ÝÎÞÐ§¡£

×¢ÒâÊÂÏî
¸Ãº¯Êý½áÊøºó£¬¿¨µÄRST¹Ü½ÅÎª¸ßµçÆ½£¬¿¨½øÈë¹¤×÷×´Ì¬¡£¿¨·µ»ØµÄ¸´Î»Ó¦´ð×Ö½Ú´æÈësrATR±äÁ¿ÖÐ¡£
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
¹Ø±ÕÉè±¸¡£¸Ãº¯Êý¹Ø±ÕÓÉHD_OpenPortº¯Êý´ò¿ªµÄÉè±¸¡£
unsigned short HD_ClosePort(
	HANDLE devNo
)

²ÎÊýËµÃ÷
devNo				ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£» ÆäÓàÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£
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
¸ø¿¨ÉÏµç¡£ÉÏµç½áÊøºó£¬¿¨µÄVcc¹Ü½ÅÎª5V,CLK¹Ü½ÅÌá¹©Ò»¸ö3.684MHzµÄÎÈ¶¨Ê±ÖÓ£¬RST¹Ü½ÅÎªµÍµçÆ½£¬I/O¹Ü½ÅÎª¸ßµçÆ½¡£

unsigned short  HD_PowerOn(
HANDLE 		devNo,
short  		ivCardSeat	//USB½Ó¿Ú²»Ê¹ÓÃ
)

²ÎÊýËµÃ÷
devNo			    ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ
ivCardSeat		    ¿¨×ùºÅ¡£1£ºÖ÷¿¨×ù£»2:¸±¿¨×ù

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£» ÆäÓàÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£

×¢ÒâÊÂÏî
¸Ãº¯Êý½áÊøºó£¬¿¨´¦ÔÚ¸´Î»×´Ì¬£¬¼´RST¹Ü½ÅÎªµÍµçÆ½¡£
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
¸ø¿¨¶Ïµç¡£¸Ãº¯Êý³É¹¦Íê³Éºó£¬¿¨µÄ¸÷¹Ü½ÅÓë¶Á¿¨Æ÷µç¸ôÀë¡£

unsigned short HD_PowerOff(
	HANDLE 		devNo,
	short 		ivCardSeat	//USB½Ó¿Ú²»Ê¹ÓÃ
)

²ÎÊýËµÃ÷
devNo			ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ
ivCardSeat		¿¨×ùºÅ¡£1£ºÖ÷¿¨×ù£»2:¸±¿¨×ù

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£» ÆäÓàÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£
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
Ì½²â¿¨×ùÖÐÊÇ·ñÓÐ¿¨´æÔÚ¡£

unsigned short HD_ProbeCard(
HANDLE 		devNo,
short 		ivCardSeat	//USB½Ó¿Ú²»Ê¹ÓÃ
)		

²ÎÊýËµÃ÷
devNo			 ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ
ivCardSeat	 	 ¿¨×ùºÅ¡£1£ºÖ÷¿¨×ù£»2:¸±¿¨×ù

·µ»ØÖµ
º¯Êý·µ»Ø0x0000Öµ±íÊ¾ÎÞ¿¨²åÈë£¬0x9000±íÊ¾ÓÐ¿¨£»ÆäÓàÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±¡£
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
¿¨ÃüÁî´¦Àíº¯Êý,¸ÃÃüÁî½ÓÊÕAPDU Case1 Case2 Case3 Case4¸ñÊ½µÄÖ¸Áî¡£

unsigned short HD_ApduT0(				
    HANDLE 				devNo,
	char 				*apduData,
	short				srAPDULen,
	char 				*respData,
	short				*respDataLen,
	short 				ivCardSeat	//USB½Ó¿Ú²»Ê¹ÓÃ
)

²ÎÊýËµÃ÷
devNo				ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ
apduData			APDUÃüÁî(×Ö½Ú´®)ÓÃCLA¡¢INS¡¢P1¡¢P2ÒÔ¼°Ò»¸ö¿É±ä³¤¶ÈµÄÌõ¼þÌåÀ´±íÊ¾£¬Ìõ¼þÌåµÄ¶¨ÒåÎªLc¡¢Data£¨Lc¸ö×Ö½Ú£©ºÍLe¡£
					¸ù¾ÝLcºÍLeÖµµÄ²»Í¬£¬¹²ÓÐËÄÖÖÃüÁîÇé¿ö£¬ÈçÏÂ±íËùÊ¾£º
					Çé¿ö	½á¹¹
					1	CLA INS P1 P2
					2	CLA INS P1 P2 Le
					3	CLA INS P1 P2 Lc Data
					4	CLA INS P1 P2 Lc Data Le
srAPDULen			´æ·Å·¢ËÍµÄAPDUÊý¾ÝµÄ³¤¶È
respData			¿¨ÏìÓ¦Êý¾Ý
respDataLen			¿¨ÏìÓ¦Êý¾ÝµÄ³¤¶È
ivCardSeat	 		¿¨×ùºÅ

·µ»ØÖµ
º¯Êý·µ»Ø0xffXX±íÊ¾ÃüÁîÎ´³É¹¦Ö´ÐÐ£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±£»ÆäÓàÖµÎª¿¨µÄÏìÓ¦¡£
µ±²Ù×÷Case1ºÍ Case2Ê±£¬respDataÎÞÒâÒå¡£µ±²Ù×÷Case3ºÍ Case4Ê±£¬º¯Êý·µ»Ø0x9000£¬respDataÓÐÒâÒå£»ÆäÓàÏìÓ¦£¬respDataÎÞÒâÒå¡£
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
		return CER_APDU;			// ·Ç·¨APDU×Ö½ÚÊý

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
			tmpTPDU[4]= LOBYTE(retv);	// ²¹ÉÏÐÂµÄLe×Ö½Ú
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
			tmpTPDU[4]= LOBYTE(retv);	// ²¹ÉÏÐÂµÄLe×Ö½Ú
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
		retv = isobase3(devNo,tmpTPDU,srAPDULen-1,respData, respDataLen, ivCardSeat); // È¥µôLe
		if(  HIBYTE(retv) == 0x61)
		{
			memcpy(getresp, "\x00\xC0\x00\x00",4);
			getresp[4]= LOBYTE(retv);
			retv = isobase2(devNo,getresp,5, respData, respDataLen, ivCardSeat);
		}
	}
	else
		return CER_APDU;			// ·Ç·¨APDU Case½á¹¹

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
»ñÈ¡¶Á¿¨Éè±¸ÃèÊö
   
unsigned short HD_GetDescriptor(		
	HANDLE 		devNo,		
	char 		*descriptor
)

²ÎÊýËµÃ÷
devNo			ÓÉHD_OpenPort·µ»ØµÄÉè±¸ºÅ
descriptor		¶Á¿¨Éè±¸ÐÅÏ¢,¶ÔÓÚCI-4ÐÍË«¿¨×ù¶Á¿¨Æ÷£¬¸ÃÐÅÏ¢ÒâÒåÎª¡°CIDC CI-4-1.0-009600¡°(Ê®Áù½øÖÆ´®)
				¶ÔÓÚUSBKey£¬Ó²¼þÐòÁÐºÅÊÇ¶ÔCPU IC¿¨×öFORMATÊ±Ð´Èë¿¨ÄÚµÄÀúÊ·×Ö½Ú

·µ»ØÖµ
º¯Êý·µ»Ø0x9000±íÊ¾ÕýÈ·£¬´ËÊ±descriptorÓÐÐ§¡£ÆäÓàÖµ±íÊ¾³ö´í£¬´íÎóÐÅÏ¢¼û¸½ºóµÄ¡°´íÎóÐÅÏ¢±í¡±£¬´ËÊ±descriptorÊý¾ÝÎÞÐ§¡£
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