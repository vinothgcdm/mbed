/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: demo.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- System includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbed.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define SLAVE_ID 0x0A

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
DigitalOut led(PTB21);

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, SLAVE_ID, 0, 9600, MB_PAR_NONE );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );
    
    // Initialise some registers
    usRegInputBuf[1] = 0x1234;
    usRegInputBuf[2] = 0x5678;
    usRegInputBuf[3] = 0x9abc;        

    for( ;; )
    {
        ( void )eMBPoll(  );
                
        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    --usAddress;
    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    --usAddress;
    if (eMode == MB_REG_READ)
    {
        if( ( usAddress >= REG_INPUT_START )
            && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
        {
            iRegIndex = ( int )( usAddress - usRegInputStart );
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ =
                    ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ =
                    ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
        } else {
            eStatus = MB_ENOREG;
        }
    }

    if (eMode == MB_REG_WRITE)
    {
        if( ( usAddress >= REG_INPUT_START )
            && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
        {
            iRegIndex = ( int )( usAddress - usRegInputStart );
            while( usNRegs > 0 )
            {
                usRegInputBuf[iRegIndex] =  ((unsigned int) *pucRegBuffer << 8) | ((unsigned int) *(pucRegBuffer+1));
                pucRegBuffer+=2;
                iRegIndex++;
                usNRegs--;
            }
        } else {
            eStatus = MB_ENOREG;
        }
    }

    led = usRegInputBuf[1] & 0x01;
    return eStatus;
}


#define COIL_START 0
#define COIL_NREGS 1

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode eStatus = MB_ENOERR;

    --usAddress;
    if ((usAddress >= COIL_START) &&
        (usAddress + usNCoils) <= COIL_START + COIL_NREGS) {
        led = pucRegBuffer[0] & 0x01;
    } else {
        eStatus = MB_ENOREG;
    }
    
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

