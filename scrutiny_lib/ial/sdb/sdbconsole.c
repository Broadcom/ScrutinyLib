/*
 *
 * Copyright (C) 2019 - 2020 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1.Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 *
 * 2.Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * 3.Neither the name of the copyright holder nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "libincludes.h"
#include "sdbconsole.h"



/**
 *
 *
 * @brief   When the serial port is reading any address via SDB, we can store
 *          the last read address. Thus, we don't have to repeat the entire
 *          command for reading the next data. This will save time. Hence this
 *          should be static and initialized once.
 *
 *          The Binary address has a condition that, only read works.
 *
 */

static U32 sSdbLastBinaryAddress = 0;


SCRUTINY_STATUS sdbiConnectSdb (__IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrFilters)
{

    SDB_SERIAL_STATE state;
    SCRUTINY_STATUS status;
    PTR_SCRUTINY_IAL_SERIAL_HANDLE ptrSerialHandle = NULL;

    state = SDB_DISCOVERY_SERIAL_STATE_CONNECTION_ESTABLISH;
    status = SCRUTINY_STATUS_RETRY;

    while (TRUE)
    {

        gPtrLoggerGeneric->logiDebug ("SDB enumerating state is %x", state);

        status = sdbEnumerateConnectionStates (PtrFilters, &state, &ptrSerialHandle);

        if (status != SCRUTINY_STATUS_RETRY)
        {
            break;
        }

    }

    return (status);

}

SCRUTINY_STATUS sdbEnumerateConnectionStates (__IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrFilters, __INOUT__ SDB_SERIAL_STATE* PtrCurrentState, __INOUT__ PTR_SCRUTINY_IAL_SERIAL_HANDLE *PtrSerialHandle)
{

    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    switch (*PtrCurrentState)
    {

        case SDB_DISCOVERY_SERIAL_STATE_CONNECTION_ESTABLISH:
        {

            status = sdbiSerialConnectionEstablish (PtrFilters, PtrSerialHandle);

            if (SCRUTINY_STATUS_SUCCESS == status)
            {
                *PtrCurrentState = SDB_DISCOVERY_SERIAL_STATE_CONNECTION_POST;
                status = SCRUTINY_STATUS_RETRY;

            }

            else
            {

                /*
                 * We are failed with the connection establish. There is nothing to destroy
                 * as well. Hence just return failure.
                 */

            }

            break;

        }

        case SDB_DISCOVERY_SERIAL_STATE_CONNECTION_POST:
        {
            /* We have now established the connection. Hence execute the next state */
            *PtrCurrentState = SDB_DISCOVERY_SERIAL_STATE_SDB_CHECK;
            status = SCRUTINY_STATUS_RETRY;

            break;

        }

        case SDB_DISCOVERY_SERIAL_STATE_CONNECTION_DESTROY:
        {
            spiClosePort (*PtrSerialHandle);
            status = SCRUTINY_STATUS_FAILED;

            break;
        }

        case SDB_DISCOVERY_SERIAL_STATE_SDB_CHECK:
        {

            if (sdbiCheckConsole (PtrFilters, *PtrSerialHandle))
            {
                /* We don't have any good connection. Hence destry and fail. */
                *PtrCurrentState = SDB_DISCOVERY_SERIAL_STATE_CONNECTION_DESTROY;
                status = SCRUTINY_STATUS_RETRY;
            }

            else
            {

                if (SCRUTINY_DISCOVERY_TYPE_SERIAL_DEBUG == gPtrScrutinyDeviceManager->DiscoveryFlag)
                {
                    /* We just have to post connect */
                    *PtrCurrentState = SDB_DISCOVERY_SERIAL_STATE_SDB_POST_CONNECT;
                    status = SCRUTINY_STATUS_RETRY;
                }

                else
                {
                    /* We don't have any good connection. Hence destry and fail. */
                    *PtrCurrentState = SDB_DISCOVERY_SERIAL_STATE_CONNECTION_DESTROY;
                    status = SCRUTINY_STATUS_RETRY;
                }

            }

            break;

        }

        case SDB_DISCOVERY_SERIAL_STATE_SDB_POST_CONNECT:
        {
            if (sdbExpanderSerialInterfacePostConnect (PtrFilters, *PtrSerialHandle))
            {

                /* We are failed on the connection of SDB Post connect. */
                status = SCRUTINY_STATUS_FAILED;
            }

            else
            {
                status = SCRUTINY_STATUS_SUCCESS;
            }

            break;

        }

        default:
        {
            status = SCRUTINY_STATUS_FAILED;
            break;
        }

    }

    return (status);

}

SCRUTINY_STATUS sdbExpanderSerialInterfacePostConnect (__IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrFilter, __IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;
    SCRUTINY_STATUS status;

    ptrDevice = (PTR_SCRUTINY_DEVICE) sosiMemAlloc (sizeof (SCRUTINY_DEVICE));

    sosiMemSet (ptrDevice, 0, sizeof (SCRUTINY_DEVICE));

    ptrDevice->HandleType = SCRUTINY_HANDLE_TYPE_SDB;
    ptrDevice->DeviceInfo.HandleType = ptrDevice->HandleType;

    sosiMemCopy (&ptrDevice->Handle.SdbHandle, PtrSerialHandle, sizeof (SCRUTINY_IAL_SERIAL_HANDLE));

    /*
     * Populate the device info structure.
     */

    ptrDevice->ProductFamily = SCRUTINY_PRODUCT_FAMILY_EXPANDER;

    /*
     * We will reset the expander current address.
     */

    status = bsdiQualifyBroadcomScsiDevice (ptrDevice);

    /*
     * If the status is good, then return otherwise destroy all the references.
     */

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sdbiSerialConnectionClose (ptrDevice);
        sosiMemFree (ptrDevice);

        return (status);
    }

    /* We will now have to add it to the global device list */

    return (ldmiAddScrutinyDevice (gPtrScrutinyDeviceManager, ptrDevice));

}

/**
 *
 * @method  sdbYield()
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS if the operation is
 *                              successful or SCRUTINY_STATUS_FAILED.
 *
 * @brief   For SDB we need to yield the read parallel thread.
 *
 */

SCRUTINY_STATUS sdbYield()
{

    #if defined(OS_WINDOWS)

        U32 index = 0;

        SwitchToThread();

        /*
         * I want to sleep less than 1 milliseconds. There is no direct option in windows.
         * thus making this ugly hack.
         */

        for (index = 0; index < 2000; index++)
        {
            sosiSleep (0);
        }

    #elif defined (OS_UEFI)


    #else


    sosiSleep (1);

    #endif

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 * @method  sdbiSerialConnectionClose()
 *
 * @param   PtrDevice           Device which has the serial connection to be
 *                              closed.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when the device is
 *          successfully closed, otherwise returns SCRUTINY_STATUS_FAILED
 *
 * @brief   This method will close the Serial port connection
 *
 */

SCRUTINY_STATUS sdbiSerialConnectionClose (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    return (spiClosePort (&PtrDevice->Handle.SdbHandle));
}

/**
 * @method sdbiReestablishConnection()
 *
 */


SCRUTINY_STATUS sdbiReestablishConnection (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    PTR_SCRUTINY_IAL_SERIAL_HANDLE ptrSerialHandle = NULL;

    SCRUTINY_IAL_SERIAL_CONFIG serialConfig;

    sosiMemSet (&serialConfig, 0, sizeof (SCRUTINY_IAL_SERIAL_CONFIG));

    sosiMemCopy (&serialConfig, &PtrDevice->Handle.SdbHandle.SerialConfig, sizeof (SCRUTINY_IAL_SERIAL_CONFIG));

    if (sdbiSerialConnectionClose (PtrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_PORT);
    }

    sosiMemSet (&PtrDevice->Handle.SdbHandle, 0, sizeof (SCRUTINY_IAL_SERIAL_HANDLE));

    if (spiOpenPort (&serialConfig, &ptrSerialHandle))
    {
        return (SCRUTINY_STATUS_INVALID_PORT);
    }

    sosiMemCopy (&PtrDevice->Handle.SdbHandle, ptrSerialHandle, sizeof (SCRUTINY_IAL_SERIAL_HANDLE));

    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @method  sdbiSerialConnectionEstablish()
 *
 * @param   PtrFilters          Filter parameters which has the configuration details
 *                              for the Serial port.
 *
 * @param   PtrSerialHandle     Pointer to a Pointer serial configuration where
 *                              the Serial handle will be stored upon the
 *                              successfull connection to the serial port.
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Based on the configuration specified from the filters (or user
 *          input) the Serial connetion will be established. The established
 *          connection handles will then be stored in othe PtrSerialHandle which
 *          ia s pointer to pointer variable.
 *
 */

SCRUTINY_STATUS sdbiSerialConnectionEstablish (__IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrFilters, __INOUT__ PTR_SCRUTINY_IAL_SERIAL_HANDLE *PtrSerialHandle)
{

    /*
     * Setup the PtrFilters to have the default params for the SDB
     */

    PtrFilters->u.SerialConfig.BaudRate       = SCRUTINY_BAUD_RATE_115200;
    PtrFilters->u.SerialConfig.DataBits       = SCRUTINY_DATA_BITS_8;
    PtrFilters->u.SerialConfig.FlowControl    = SCRUTINY_FLOW_CONTROL_NONE;
    PtrFilters->u.SerialConfig.Parity         = SCRUTINY_PARITY_NONE;
    PtrFilters->u.SerialConfig.StopBits       = SCRUTINY_STOP_BITS_1_0;

    /*
     * Check if we have the COM port data.
     */

    if (sosiStringLength (PtrFilters->u.SerialConfig.DeviceName) < 1)
    {
        return (SCRUTINY_STATUS_INVALID_PORT);
    }

    if (spiOpenPort (&PtrFilters->u.SerialConfig, PtrSerialHandle))
    {
        return (SCRUTINY_STATUS_INVALID_PORT);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiCheckConsole()
 *
 * @param   PtrFilters          Filter parameters which has the configuration details
 *                              for the Serial port.
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Peform the SDB check by issuing a dummy read. If the read is
 *          successful, we know that we have a device which is responding for
 *          Serial port commands. At this point of a time, we don't know if the
 *          device attached is Expander or Switch. As the Expanders and Switch
 *          shares the same SDB hardware responses are same.
 *
 */

SCRUTINY_STATUS sdbiCheckConsole (__IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrFilter, __IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle)
{

    U8 data[1024] = { "\r" };
    U32 dword = 0, readSize;

    sSdbLastBinaryAddress = 0;

    /* Flush the existing values. */
    spiFlushPort (PtrSerialHandle);

    /* Perform dummy write to clear the buffer */
    if (spiWriteData (PtrSerialHandle, data, 1, &readSize))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Flush the existing values. */
    spiFlushPort (PtrSerialHandle);

    sosiSleep (10);

    /* Do a valid memory read. */
    if (sdbiMemoryRead32 (PtrSerialHandle, 0x10000000, &dword))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiMemoryRead8()
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @param   Address             Address value to perform the memory operation
 *
 * @param   PtrData             Memory read data from the SDB
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Performs the memory read 'Byte' operation on the SDB
 *
 */

SCRUTINY_STATUS sdbiMemoryRead8 (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle, __IN__ U32 Address, __OUT__ PU8 PtrData)
{

    SCRUTINY_STATUS status;
    U8 cmd[7];
    U32 cmdSize = 7;
    U32 sizeOp = 0;
    U8 data;
    U32 retryCount = 6;

    while (retryCount--)
    {

    sSdbLastBinaryAddress = 0;

    spiFlushPort (PtrSerialHandle);

    cmd[0] = 'G';
    cmd[1] = 1;
    cmd[2] = (U8) ((Address >> 24) & 0xFF);
    cmd[3] = (U8) ((Address >> 16) & 0xFF);
    cmd[4] = (U8) ((Address >> 8) & 0xFF);
    cmd[5] = (U8) (Address & 0xFF);
    cmd[6] = '\r';

    /* Write the data into the serial port */

    status = spiWriteData (PtrSerialHandle, cmd, cmdSize, &sizeOp);

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    sdbYield();

    data = 0;
    sizeOp = 1;

    status = spiReadData (PtrSerialHandle, (U8 *) &data, &sizeOp, 1);

    if (status || sizeOp != 1)
    {

        if (retryCount == 0)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        continue;
    }

    *PtrData = data;

    spiReadData (PtrSerialHandle, cmd, &sizeOp, 1);

    break;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiMemoryRead16()
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @param   Address             Address value to perform the memory operation
 *
 * @param   PtrData             Memory read data from the SDB
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Performs the memory read 'WORD' operation on the SDB
 *
 */

SCRUTINY_STATUS sdbiMemoryRead16 (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle, __IN__ U32 Address, __OUT__ PU16 PtrData)
{

    SCRUTINY_STATUS status;
    U8 cmd[7];
    U32 cmdSize = 7;
    U32 sizeOp = 0;
    U16 data;
    U32 retryCount = 6;

    sSdbLastBinaryAddress = 0;

    while (retryCount--)
    {

    spiFlushPort (PtrSerialHandle);

    cmd[0] = 'G';
    cmd[1] = 2;
    cmd[2] = (U8) ((Address >> 24) & 0xFF);
    cmd[3] = (U8) ((Address >> 16) & 0xFF);
    cmd[4] = (U8) ((Address >> 8) & 0xFF);
    cmd[5] = (U8) (Address & 0xFF);
    cmd[6] = '\r';

    /* Write the data into the serial port */

    status = spiWriteData (PtrSerialHandle, cmd, cmdSize, &sizeOp);

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    sdbYield();

    data = 0;
    sizeOp = 2;

    status = spiReadData (PtrSerialHandle, (U8 *) &data, &sizeOp, 2);

    if (status || sizeOp != 1)
    {
        if (retryCount == 0)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        continue;
    }

    data = ((data >> 8) & 0xFF) | (data & 0xFF) << 8;
    *PtrData = data;

    spiReadData (PtrSerialHandle, cmd, &sizeOp, 1);

    break;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiMemoryRead32()
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @param   Address             Address value to perform the memory operation
 *
 * @param   PtrData             Memory read data from the SDB
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Performs the memory read 'DWORD' operation on the SDB
 *
 */

SCRUTINY_STATUS sdbiMemoryRead32 (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle, __IN__ U32 Address, __OUT__ PU32 PtrData)
{

    SCRUTINY_STATUS status;
    U8 cmd[7];
    U32 cmdSize = 7;
    U32 sizeOp = 0;
    U32 data;
    U32 retryCount = 6;

    while (retryCount--)
    {

    spiFlushPort (PtrSerialHandle);

    if (sSdbLastBinaryAddress == (Address - 4))
    {

        sizeOp = 2;
        cmdSize = 2;

        cmd[0] = 'n';
        cmd[1] = '\r';

    }

    else
    {
        cmd[0] = 'G';
        cmd[1] = 4;
        cmd[2] = (U8) ((Address >> 24) & 0xFF);
        cmd[3] = (U8) ((Address >> 16) & 0xFF);
        cmd[4] = (U8) ((Address >> 8) & 0xFF);
        cmd[5] = (U8) (Address & 0xFF);
        cmd[6] = '\r';
    }

    /* Write the data into the serial port */
    status = spiWriteData (PtrSerialHandle, cmd, cmdSize, &sizeOp);

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    sdbYield();

    data = 0;
    sizeOp = 4;

    status = spiReadData (PtrSerialHandle, (U8 *) &data, &sizeOp, 4);

    if (status || sizeOp != 4)
    {

        if (retryCount == 0)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        continue;
    }

    data = (((data & 0x000000FF) << 24)  +
            ((data & 0x0000FF00) << 8)  +
            ((data & 0x00FF0000) >> 8)  +
            ((data & 0xFF000000) >> 24));

    *PtrData = data;

    spiReadData (PtrSerialHandle, cmd, &sizeOp, 1);

    sSdbLastBinaryAddress = Address;

    break;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiMemoryWrite8()
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @param   Address             Address value to perform the memory operation
 *
 * @param   Data                Data that needs to be written
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Performs the memory write 'BYTE' operation over the SDB
 *
 */

SCRUTINY_STATUS sdbiMemoryWrite8 (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle, __IN__ U32 Address, __IN__ U8 Data)
{

    SCRUTINY_STATUS status;
    U8 cmd[8];
    U32 cmdSize = 8;
    U32 sizeOp = 0;

    while (TRUE)
    {

    sSdbLastBinaryAddress = 0;

    spiFlushPort (PtrSerialHandle);

    cmd[0] = 'P';
    cmd[1] = 1;
    cmd[2] = (U8) ((Address >> 24) & 0xFF);
    cmd[3] = (U8) ((Address >> 16) & 0xFF);
    cmd[4] = (U8) ((Address >> 8) & 0xFF);
    cmd[5] = (U8) (Address & 0xFF);
    cmd[6] = (U8) (Data & 0xFF);
    cmd[7] = '\r';

    /* Write the data into the serial port */

    status = spiWriteData (PtrSerialHandle, cmd, cmdSize, &sizeOp);

    sdbYield();

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    spiReadData (PtrSerialHandle, cmd, &sizeOp, 1);

    break;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiMemoryWrite16()
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @param   Address             Address value to perform the memory operation
 *
 * @param   Data                Data that needs to be written
 *
 * @return  SCRUTINY_STATUS              SCRUTINY_STATUS_SUCCESS if the operation is successful or
 *                              SCRUTINY_STATUS_FAILED.
 *
 * @brief   Performs the memory write 'WORD' operation over the SDB
 *
 */

SCRUTINY_STATUS sdbiMemoryWrite16 (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle, __IN__ U32 Address, __IN__ U16 Data)
{

    SCRUTINY_STATUS status;
    U8 cmd[9];
    U32 cmdSize = 9;
    U32 sizeOp = 0;

    sSdbLastBinaryAddress = 0;

    while (TRUE)
    {

    spiFlushPort (PtrSerialHandle);

    cmd[0] = 'P';
    cmd[1] = 2;
    cmd[2] = (U8) ((Address >> 24) & 0xFF);
    cmd[3] = (U8) ((Address >> 16) & 0xFF);
    cmd[4] = (U8) ((Address >> 8) & 0xFF);
    cmd[5] = (U8) (Address & 0xFF);
    cmd[6] = (U8) ((Data >>  8) & 0xFF);
    cmd[7] = (U8) (Data & 0xFF);
    cmd[8] = '\r';

    /* Write the data into the serial port */

    status = spiWriteData (PtrSerialHandle, cmd, cmdSize, &sizeOp);

    sdbYield();

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    spiReadData (PtrSerialHandle, cmd, &sizeOp, 1);

    break;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sdbiMemoryWrite32()
 *
 * @param   PtrSerialHandle     Serial handle where the connection details are
 *                              stored.
 *
 * @param   Address             Address value to perform the memory operation
 *
 * @param   Data                Data that needs to be written
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS if the operation is
 *                              successful or SCRUTINY_STATUS_FAILED.
 *
 * @brief   Performs the memory write 'DWORD' operation over the SDB
 *
 */

SCRUTINY_STATUS sdbiMemoryWrite32 (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialHandle, __IN__ U32 Address, __IN__ U32 Data)
{

    SCRUTINY_STATUS status;
    U8 cmd[11];
    U32 cmdSize = 11;
    U32 sizeOp = 0;

    while (TRUE)
    {

    sSdbLastBinaryAddress = 0;

    spiFlushPort (PtrSerialHandle);

    cmd[0] = 'P';
    cmd[1] = 4;
    cmd[2] = (U8) ((Address >> 24) & 0xFF);
    cmd[3] = (U8) ((Address >> 16) & 0xFF);
    cmd[4] = (U8) ((Address >> 8) & 0xFF);
    cmd[5] = (U8) (Address & 0xFF);
    cmd[6] = (U8) ((Data >> 24) & 0xFF);
    cmd[7] = (U8) ((Data >> 16) & 0xFF);
    cmd[8] = (U8) ((Data >>  8) & 0xFF);
    cmd[9] = (U8) (Data & 0xFF);
    cmd[10] = '\r';

    /* Write the data into the serial port */
    status = spiWriteData (PtrSerialHandle, cmd, cmdSize, &sizeOp);

    sdbYield();

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    spiReadData (PtrSerialHandle, cmd, &sizeOp, 1);

    break;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

