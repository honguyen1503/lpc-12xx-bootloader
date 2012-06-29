/*
Copyright (c) 2012 Martin Koehler, metraTec GmbH (metraTec.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS",
WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "GlobalIncludes.h"

const u8 const sOK[]="OK!";

void SendErrorByCode(tErrorCode eErrorCode)
{
#if 1
	switch (eErrorCode)
	{
	case ecNoError:
		break;
	case ecAcknowledge:
		CommSendStrC13(sOK,3);
		break;
	case ecBrownOutDetected:
		sendArray13("BOD");
		break;
	case ecBufferOverflow:
		sendArray13("BOF");
		break;
	case ecCommunicationCRCError:
		sendArray13("CRE");
		break;
	case ecCommandReceiveTimeout:
		sendArray13("CRT");
		break;
	case ecDidNotSleep:
		sendArray13("DNS");
		break;
	case ecDecimalDecodeError:
		sendArray13("EDX");
		break;
	case ecHardwareFailure:
		sendArray13("EHF");
		break;
	case ecHexDecodeError:
		sendArray13("EHX");
		break;
	case ecNotInCNRMode:
		sendArray13("NCM");
		break;
	case ecNumberOutOfRage:
		sendArray13("NOR");
		break;
	case ecNotSupported:
		sendArray13("NOS");
		break;
	case ecNoRfFieldActive:
		sendArray13("NRF");
		break;
	case ecNoStandardSelected:
		sendArray13("NSS");
		break;
	case ecPhaseLockError:
		sendArray13("PLE");
		break;
	case ecWatchdogOccured:
		sendArray13("SRT");
		break;
	case ecUnknownCommand:
		sendArray13("UCO");
		break;
	case ecUnknownParameter:
		sendArray13("UPA");
		break;
	case ecUartReceiveError:
		sendArray13("URE");
		break;
	case ecWrongDataLength:
		sendArray13("WDL");
		break;
	case ecAntennaReflectivityToHigh:
		sendArray13("ARH");
		break;
	case ecRFIDCRCError:
		sendArray13("CER");
		break;
	case ecFifoLengthError:
		sendArray13("FLE");
		break;
	case ecErrorGettingHeaderbitErrorCode:
		sendArray13("HBE");
		break;
	case ecReadDataToLong:
		sendArray13("RDL");
		break;
	case ecResponseLengthNotAsExpedted:
		sendArray13("RXE");
		break;
	case ecTagCommunicationError:
		sendArray13("TCE");
		break;
	case ecTimeOutError:
		sendArray13("TOE");
		break;
	case ecTagOutOfRage:
		sendArray13("TOR");
		break;
	case ecTagNotResonding:
		sendArray13("TNR");
		break;
	case ecAccessError:
		sendArray13("AER");
		break;
	case ecPreambleDetectError:
		sendArray13("PDE");
		break;
	case ecPinHigh:
		sendArray13("HI!");
		break;
	case ecPinLow:
		sendArray13("LOW");
		break;
	case ecNoTagSelected:
		sendArray13("NTS");
		break;
	case ecWrongTagType:
		sendArray13("WTT");
		break;
	case ecBlockNotAuthenticated:
		sendArray13("BNA");
		break;
	case ecUnknownError:
		sendArray13("UER");
		break;
	default:
		sendArray("UER ");CommSendHex13(eErrorCode);
		break;
	}
#endif
}
