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

#include <errno.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/file.h>


SCRUTINY_STATUS spGetComPortConfiguration (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig);
BOOLEAN spiIsHandleValid (__IN__ U32 FileDescriptor);
SCRUTINY_STATUS spReadData (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerial);
VOID *spReadThread (__IN__ VOID* PtrParam);

/**
 *
 * @name    spiOpenPort()
 *
 * @param   PtrConfig           Configuration for the SERIAL Port. This should be the generic
 *                              values defined in this header file.
 *
 * @param   PtrHandle           After opening the port successfully, Handle will be stored
 *                              in this arugment.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Opens the SERIAL with the params given in the argument. Handle will be
 *          stored in the Handle Ptr.
 *
 */

SCRUTINY_STATUS spiOpenPort (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrConfig, __OUT__ PTR_SCRUTINY_IAL_SERIAL_HANDLE *PtrHandle)
{

    int fdRead = -1;
    int fdWrite = -1;

    int mcs = 0;
    struct termios options;

    /* Posix - set baudrate to 0 and back */
    //struct termios tty;
    struct serial_struct serial;

    /*
     * Initialzing the Handle with foo value.
     *
     */

    PTR_SCRUTINY_IAL_SERIAL_HANDLE ptrSerial = NULL;

    ptrSerial = (PTR_SCRUTINY_IAL_SERIAL_HANDLE) malloc (sizeof (SCRUTINY_IAL_SERIAL_HANDLE));

    memset (ptrSerial, 0, sizeof (SCRUTINY_IAL_SERIAL_HANDLE));

    ptrSerial->ReadHandle = -1;
    ptrSerial->WriteHandle = -1;

    fdRead = open (PtrConfig->DeviceName, O_RDONLY | O_NOCTTY | O_NDELAY);
    fdWrite = open (PtrConfig->DeviceName, O_WRONLY | O_NOCTTY | O_NDELAY);

    ioctl(fdRead, TIOCGSERIAL, &serial);
    serial.flags |= 0x2000; //ASYNC_LOW_LATENCY; // (0x2000)
    ioctl(fdRead, TIOCSSERIAL, &serial);

    ioctl(fdWrite, TIOCGSERIAL, &serial);
    serial.flags |= 0x2000; //ASYNC_LOW_LATENCY; // (0x2000)
    ioctl(fdWrite, TIOCSSERIAL, &serial);

    if (fdRead < 0 || fdWrite < 0)
    {

        if (fdRead > 0)
        {
            close (fdRead);
        }

        if (fdWrite > 0)
        {
            close (fdWrite);
        }

        return (SCRUTINY_STATUS_FAILED);

    }

    #ifndef OS_SOLARIS

    if (flock (fdRead, LOCK_EX | LOCK_NB) == -1)
    {

        close (fdRead);
        close (fdWrite);

        return (SCRUTINY_STATUS_FAILED);

    }

    #endif

    tcgetattr (fdRead, &options);

    /*
     * Calling the Conversion or Setting params
     */

    if (spGetComPortConfiguration (PtrConfig, &options) != SCRUTINY_STATUS_SUCCESS) {

        if (fdRead > 0)
        {
            close (fdRead);
        }

        if (fdWrite > 0)
        {
            close (fdWrite);
        }

        return (SCRUTINY_STATUS_FAILED);

    }

    ioctl(fdRead, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(fdRead, TIOCMSET, &mcs);

    tcflush (fdRead, TCIOFLUSH);
    tcflush (fdWrite, TCIOFLUSH);

    tcsetattr (fdRead, TCSANOW, &options);
    tcsetattr (fdWrite, TCSANOW, &options);

    ptrSerial->ReadHandle = fdRead;
    ptrSerial->WriteHandle = fdWrite;

    ptrSerial->PtrSerialBuffer = (PTR_IAL_SERIAL_BUFFER) malloc (sizeof (IAL_SERIAL_BUFFER));

    wsbiInitializeBuffers (ptrSerial->PtrSerialBuffer);

    /*
     * We need to start the reading thread
     */

    if (pthread_create (&ptrSerial->ReadThread, NULL, spReadThread, (void*) ptrSerial))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    memcpy (&ptrSerial->SerialConfig, PtrConfig, sizeof (SCRUTINY_IAL_SERIAL_CONFIG));

    *PtrHandle = ptrSerial;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spiClosePort()
 *
 * @param   PtrSerialParams     Serial port handle for the native system call.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Close the SERIAL port which is corresponding to the given HANDLE.
 *
 */

SCRUTINY_STATUS spiClosePort (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerial)
{

    if (PtrSerial->ReadHandle != -1)
    {

        #ifndef OS_SOLARIS
        flock (PtrSerial->ReadHandle, LOCK_UN);
        #endif

        close (PtrSerial->ReadHandle);
        close (PtrSerial->WriteHandle);

    }

    sem_destroy (&PtrSerial->PtrSerialBuffer->CriticalSectionHandle);

    PtrSerial->ReadHandle = -1;
    PtrSerial->WriteHandle = -1;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spiFlushPort()
 *
 * @param   PtrSerial           Holds the handle for using it with the native
 *                              system.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Flushes the Port. Clears all errors.
 *
 */

SCRUTINY_STATUS spiFlushPort (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerial)
{

    if (PtrSerial->ReadHandle == -1)
    {
        /* We don't have to flush, not necessary to throw error or do we?*/

        return (SCRUTINY_STATUS_SUCCESS);

    }

    /*
     * Read whatever we have
     */

    wsbiFlush (PtrSerial->PtrSerialBuffer);

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spStartThreads()
 *
 * @param   PtrParam        VOID paramter which needs to be converted into the
 *                          PTR_SCRUTINY_IAL_SERIAL_HANDLE.
 *
 * @return  VOID*           However we are not returning anything.
 *
 * @brief   Starts the thread for reading and wirting the serial port. This will
 *          keep reading theport until the COM port is closed.
 *
 */


VOID *spReadThread (__IN__ VOID* PtrParam)
{

    PTR_SCRUTINY_IAL_SERIAL_HANDLE ptrSerial = (PTR_SCRUTINY_IAL_SERIAL_HANDLE) PtrParam;
    int available = 0;

    fd_set input;

    if (!spiIsHandleValid (ptrSerial->ReadHandle))
    {
        return (NULL);
    }

    FD_ZERO (&input);
    FD_SET (ptrSerial->ReadHandle, &input);


    while (TRUE)
    {

        if (!spiIsHandleValid (ptrSerial->ReadHandle))
        {
            break;
        }

        available = 0;

        /* Do the select */

        if (ioctl (ptrSerial->ReadHandle, FIONREAD, &available) < 0)
        {
            available = 0;
        }

        if (available)
        {
            spReadData (ptrSerial);
        }

    }

    return (NULL);

}

/**
 *
 * @name    spReadData()
 *
 * @param   PtrSerial           Serial port handle for the native system call.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Reads the available data on the serial port and stores them into the
 *          serial buffer. The method does not throw any error message.
 *
 */

SCRUTINY_STATUS spReadData (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerial)
{

    int numRead = 0;
    U8 data[1024 * 1024] = { 0 };

    numRead = read (PtrSerial->ReadHandle, data, 1024 * 1024);

    if (numRead < 1)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    wsbiLockBuffer (PtrSerial->PtrSerialBuffer);
    wsbiAddData (PtrSerial->PtrSerialBuffer, data, numRead);
    wsbiUnlockBuffer (PtrSerial->PtrSerialBuffer);

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spiReadData()
 *
 * @param   PtrSerialParams     Serial port handle for the native system call.
 *
 * @param   PtrData             Holds the pointer data on which the read values are stored.
 *
 * @param   PtrReadSize         Holds the Number of bytes read from the SERIAL.
 *
 * @param   MaxSize             Maximum number of size, which can be read by the program.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Reads the data from the SERIAL port and stores the results in PtrData.
 *
 */

SCRUTINY_STATUS spiReadData (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerial, __OUT__ U8* PtrData, __OUT__ U32* PtrReadSize, __IN__ U32 MaxSize)
{
    return (wsbiGetData (PtrSerial->PtrSerialBuffer, PtrData, PtrReadSize, MaxSize));
}

/**
 *
 * @name    spiWriteData()
 *
 * @param   PtrSerialParams     Handle for the serial port configuration.
 *
 * @param   PtrData             Data that needs to be written into the COM port.
 *
 * @param   Size                Size of the specifid input data which needs to
 *                              be written.
 *
 * @param   PtrWritten          Number of bytes which are written on the port. On the
 *                              successfull written, PtrWritten will match with the Size
 *                              variable.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Writes the given data on the SERIAL port.
 *
 */

SCRUTINY_STATUS spiWriteData (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerial, __IN__ U8* PtrData, __IN__ U32 Size, __OUT__ U32* PtrWritten)
{

    int val = 0;

    if (!spiIsHandleValid (PtrSerial->WriteHandle))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    val = write (PtrSerial->WriteHandle, PtrData, Size);

    if (val != Size)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrWritten = Size;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spSetConfigurationBaudRate()
 *
 * @param   PtrSerialConfig     COM port configuration parameters to convert from
 *                              Scrutiny based configuration to linux specific
 *                              configurations.
 *
 * @param   PtrTermConfig       COM port converted configurations which can be
 *                              directly used for Linux based system calls.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Converts the Scrutiny configuration of Baud rate into Termios based
 *          configuration of Baud Rate.
 *
 */

SCRUTINY_STATUS spSetConfigurationBaudRate (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig)
{

    /*
     * Setting up Baud Rates.
     *
     */

    switch (PtrSerialConfig->BaudRate)
    {

        case SCRUTINY_BAUD_RATE_230400:
        {
            cfsetospeed (PtrTermConfig, B230400);
            cfsetispeed (PtrTermConfig, B230400);

            break;
        }

        case SCRUTINY_BAUD_RATE_115200:
        {
            cfsetospeed (PtrTermConfig, B115200);
            cfsetispeed (PtrTermConfig, B115200);
            break;
        }

        case SCRUTINY_BAUD_RATE_38400:
        {
            cfsetospeed (PtrTermConfig, B38400);
            cfsetispeed (PtrTermConfig, B38400);

            break;
        }

        case SCRUTINY_BAUD_RATE_57600:
        {
            cfsetospeed (PtrTermConfig, B57600);
            cfsetispeed (PtrTermConfig, B57600);

            break;
        }

        case SCRUTINY_BAUD_RATE_19200:
        {
            cfsetospeed (PtrTermConfig, B19200);
            cfsetispeed (PtrTermConfig, B19200);

            break;
        }

        case SCRUTINY_BAUD_RATE_9600:
        {
            cfsetospeed (PtrTermConfig, B9600);
            cfsetispeed (PtrTermConfig, B9600);

            break;
        }

        case SCRUTINY_BAUD_RATE_4800:
        {
            cfsetospeed (PtrTermConfig, B4800);
            cfsetispeed (PtrTermConfig, B4800);

            break;
        }

        case SCRUTINY_BAUD_RATE_2400:
        {
            cfsetospeed (PtrTermConfig, B2400);
            cfsetispeed (PtrTermConfig, B2400);

            break;
        }

        case SCRUTINY_BAUD_RATE_1200:
        {
            cfsetospeed (PtrTermConfig, B1200);
            cfsetispeed (PtrTermConfig, B1200);

            break;
        }

        case SCRUTINY_BAUD_RATE_600:
        {
            cfsetospeed (PtrTermConfig, B600);
            cfsetispeed (PtrTermConfig, B600);

            break;
        }

        case SCRUTINY_BAUD_RATE_300:
        {
            cfsetospeed (PtrTermConfig, B300);
            cfsetispeed (PtrTermConfig, B300);

            break;
        }

        case SCRUTINY_BAUD_RATE_110:
        {
            cfsetospeed (PtrTermConfig, B110);
            cfsetispeed (PtrTermConfig, B110);

            break;
        }

        default:
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spSetConfigurationDataBits()
 *
 * @param   PtrSerialConfig     COM port configuration parameters to convert from
 *                              Scrutiny based configuration to linux specific
 *                              configurations.
 *
 * @param   PtrTermConfig       COM port converted configurations which can be
 *                              directly used for Linux based system calls.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Converts the Scrutiny configuration of Data Bits into Termios
 *          based configuration of Data Bits.
 *
 */

SCRUTINY_STATUS spSetConfigurationDataBits (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig)
{
    switch (PtrSerialConfig->DataBits)
    {
        case SCRUTINY_DATA_BITS_5:
        {
            PtrTermConfig->c_cflag = (PtrTermConfig->c_cflag & ~CSIZE) | CS5;
            break;
        }
        case SCRUTINY_DATA_BITS_6:
        {
            PtrTermConfig->c_cflag = (PtrTermConfig->c_cflag & ~CSIZE) | CS6;
            break;
        }
        case SCRUTINY_DATA_BITS_7:
        {
            PtrTermConfig->c_cflag = (PtrTermConfig->c_cflag & ~CSIZE) | CS7;
            break;
        }
        case SCRUTINY_DATA_BITS_8:
        {
            PtrTermConfig->c_cflag = (PtrTermConfig->c_cflag & ~CSIZE) |  CS8;
            break;
        }

        default:
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spSetConfigurationFlowControl()
 *
 * @param   PtrSerialConfig     COM port configuration parameters to convert from
 *                              Scrutiny based configuration to linux specific
 *                              configurations.
 *
 * @param   PtrTermConfig       COM port converted configurations which can be
 *                              directly used for Linux based system calls.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Converts the Scrutiny configuration of flow control into Termios
 *          based configuration of flow control.
 *
 */

SCRUTINY_STATUS spSetConfigurationFlowControl (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig)
{
    switch (PtrSerialConfig->FlowControl )
    {
        case SCRUTINY_FLOW_CONTROL_UNKNOWN:
        case SCRUTINY_FLOW_CONTROL_NONE:
        {
            PtrTermConfig->c_iflag &= ~(IXON | IXOFF | IXANY);
            PtrTermConfig->c_cflag &= ~CRTSCTS;
            break;
        }

        case SCRUTINY_FLOW_CONTROL_SOFTWARE:
        {
            PtrTermConfig->c_iflag |= (IXON | IXOFF);
            break;
        }

        case SCRUTINY_FLOW_CONTROL_HARDWARE:
        {
            PtrTermConfig->c_cflag |= CRTSCTS;
            break;
        }

        default:
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spSetConfigurationParity()
 *
 * @param   PtrSerialConfig     COM port configuration parameters to convert from
 *                              Scrutiny based configuration to linux specific
 *                              configurations.
 *
 * @param   PtrTermConfig       COM port converted configurations which can be
 *                              directly used for Linux based system calls.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Converts the Scrutiny configuration of parity value into Termios
 *          based configuration of parity.
 *
 */

SCRUTINY_STATUS spSetConfigurationParity (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig)
{
    /*
     * Setting up COM Configurations.
     *
     */

    switch (PtrSerialConfig->Parity)
    {
        case SCRUTINY_PARITY_NONE:
        {
            PtrTermConfig->c_iflag &= ~(INPCK);
            PtrTermConfig->c_cflag &= ~(PARENB);
            break;
        }

        case SCRUTINY_PARITY_ODD:
        {
            PtrTermConfig->c_iflag |= INPCK;
            PtrTermConfig->c_cflag |= (PARENB | PARODD);

            break;
        }

        case SCRUTINY_PARITY_EVEN:
        {
            PtrTermConfig->c_iflag |= INPCK;
            PtrTermConfig->c_cflag |= PARENB;

            break;
        }

        default:
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spSetConfigurationStopBits()
 *
 * @param   PtrSerialConfig     COM port configuration parameters to convert from
 *                              Scrutiny based configuration to linux specific
 *                              configurations.
 *
 * @param   PtrTermConfig       COM port converted configurations which can be
 *                              directly used for Linux based system calls.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Converts the Scrutiny configuration of Stop bits into Termios based
 *          configuration of stop bits.
 *
 */

SCRUTINY_STATUS spSetConfigurationStopBits (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig)
{

    switch (PtrSerialConfig->StopBits)
    {
        case SCRUTINY_STOP_BITS_1_0:
        {
            PtrTermConfig->c_cflag &= ~CSTOPB;
            break;
        }

        case SCRUTINY_STOP_BITS_1_5:
        case SCRUTINY_STOP_BITS_2_0:
        {
            PtrTermConfig->c_cflag |= CSTOPB;
            break;
        }

        default:
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spGetComPortConfiguration()
 *
 * @param   PtrSerialConfig     COM port configuration parameters to convert from
 *                              Scrutiny based configuration to linux specific
 *                              configurations.
 *
 * @param   PtrTermConfig       COM port converted configurations which can be
 *                              directly used for Linux based system calls.
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief   Converts the Scrutiny standard COM configuration to the windows
 *          based configurations.
 *
 */

SCRUTINY_STATUS spGetComPortConfiguration (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrSerialConfig, __OUT__ struct termios *PtrTermConfig)
{

    if (spSetConfigurationBaudRate (PtrSerialConfig, PtrTermConfig))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (spSetConfigurationDataBits (PtrSerialConfig, PtrTermConfig))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    PtrTermConfig->c_iflag = IGNBRK | IGNPAR;
    PtrTermConfig->c_oflag &= ~OPOST;
    PtrTermConfig->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    PtrTermConfig->c_cflag |= CLOCAL | CREAD;
    PtrTermConfig->c_cflag &= ~HUPCL;

    PtrTermConfig->c_cc[VMIN] = 0;
    PtrTermConfig->c_cc[VTIME] = 5;

    /* Setting up the Flow Controls. */
    if (spSetConfigurationFlowControl (PtrSerialConfig, PtrTermConfig))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Set up parity */
    if (spSetConfigurationParity (PtrSerialConfig, PtrTermConfig))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Set up stop bits */
    if (spSetConfigurationStopBits (PtrSerialConfig, PtrTermConfig))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    spiIsHandleValid()
 *
 * @param   FileDescriptor      Descriptor which needs to be checked if it is
 *                              valid.
 *
 * @return  BOOLEAN             TRUE if the descriptor is valid and FALSE if it
 *                              is invalid.
 *
 * @brief   Check if the descriptor is valid by calling the appropriate system
 *          call.
 *
 */

BOOLEAN spiIsHandleValid (__IN__ U32 FileDescriptor)
{
    return (BOOLEAN) (fcntl (FileDescriptor, F_GETFD) != -1 || errno != EBADF);
}


