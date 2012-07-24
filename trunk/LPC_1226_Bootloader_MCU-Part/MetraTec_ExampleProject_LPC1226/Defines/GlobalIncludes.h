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

#ifndef GLOBALINCLUDES_H_
#define GLOBALINCLUDES_H_

#include "string.h"			/*standard CRC*/

#include "LPC122x.h"		/*hardware abstraction layer from NXP*/
#include "core_cm0.h"		/*hardware abstraction layer from NXP*/
#include "core_cmFunc.h"	/*hardware abstraction layer from NXP*/
#include "core_cmInstr.h"	/*hardware abstraction layer from NXP*/

#include "GlobalDefines.h"	/*contains some settings and #defines*/
#include "Typedefs.h"		/*contains all new defined types, mostly structs*/
#include "GlobalVariables.h"/*Contains the "extern" declaration of global variables*/

#include "../Support/ErrorHandling.h"	/*translates errors to interface*/
#include "../Support/uart.h"			/*gives UART interface, also support functions to decode and encode data to and from ascii*/
#include "../Support/Timer.h"			/*gives flexible software timer function (millisecondwise, as much timers as wanted)*/
#include "../Main/Parser.h"				/*contains the translation from received ascii commands to functions*/

#endif /* GLOBALINCLUDES_H_ */
