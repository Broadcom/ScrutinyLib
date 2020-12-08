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

/**
 *
 *  @method  expiPhyControl()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PhyOperation            Phy control operation 
 *
 *  @return  Status                  '0' for success and  non-zero for failure
 *
 *  @brief                           This method do some phy control operation on expander phy by SMP 
 *
 */

SCRUTINY_STATUS expiPhyControl (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __IN__ U8  PhyOperation)
{
    U32 						index;
    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    U32 						size = sizeof (SMP_PHY_CONTROL_RESPONSE);
    SMP_PHY_CONTROL_REQUEST 	smpPhyControlRequest;
    PTR_SMP_PHY_CONTROL_REQUEST ptrSmpPhyControlRequest;
    SMP_PHY_CONTROL_RESPONSE 	smpPhyControlResponse;


    gPtrLoggerExpanders->logiFunctionEntry ("expiPhyControl (PtrDevice=%x, PhyIdentifier=%x, PhyOperation=%x)", PtrDevice != NULL, PhyIdentifier, PhyOperation);



    sosiMemSet (&smpPhyControlRequest, 0, sizeof (SMP_PHY_CONTROL_REQUEST));
    sosiMemSet (&smpPhyControlResponse, 0, sizeof (SMP_PHY_CONTROL_RESPONSE));

    //let below parameters, others keep default value 0
    smpPhyControlRequest.SMPFrameType                  = SMP_FRAME_TYPE_SMP_REQUEST;
    smpPhyControlRequest.Function                      = SMP_FUNCTION_PHY_CONTROL;
    smpPhyControlRequest.PhyOperation                  = PhyOperation;
    smpPhyControlRequest.PhyIdentifier                 = PhyIdentifier;
    smpPhyControlRequest.RequestLength                 = 9;           // this is fixed for SMP_FUNCTION_PHY_CONTROL
    smpPhyControlRequest.AllocRespLength               = 0;           // if the value is not zero, it limit the data bytes that smp target can transfer,
                                                            // 0 means the SMP will transfer data as the function specify.

    smpPhyControlRequest.ExpanderChangeCount           = 0;           //When EX is greater than zero then if the value doesn't match the expander change count of the SMP target (i.e. the expander)
                                                            // when the request arrives then the target ignores the request and sets a function result of "invalid expander change count" in the response
                                                            //the default value is 0,that means the target will not take this as a condition.
    
    smpPhyControlRequest.ProgMaxPhysLinkRate           = 0;           // 0 means we don't change current max link rate setting
            
                                                            //   0  : no change

                                                            //   8  : 1.5 Gbps

                                                            //   9  : 3 Gbps

                                                            //   10 : 6 Gbps

                                                            //   11 : 12 Gbps 
    smpPhyControlRequest.ProgMinPhysLinkRate            = 0;           // 0 means we don't change current min lind rate setting
    
    smpPhyControlRequest.EnableSataPartial              = 0;
    
                                                            //set the Enable SATA Partial field to CO which is two bits wide.
                                                            //The defined values for CO are 0 for no change; 
                                                            //1 for manage partial phy power conditions; 
                                                            //2 for disable partial phy power conditions 
    
    smpPhyControlRequest.EnableSataSlumber              = 0;
                                                            //set the Enable SATA Slumber field to CO which is two bits wide.
                                                            // The defined values for CO are 0 for no change;
                                                            // 1 for manage slumber phy power conditions; 
                                                            // 2 for disable slumber phy power conditions.
    
    smpPhyControlRequest.EnableSasPartial               = 0;
    
                                                            //set the Enable SAS Partial field to CO which is two bits wide.
                                                            //The defined values for CO are 0 for no change; 
                                                            //1 for manage partial phy power conditions; 
                                                            //2 for disable partial phy power conditions 
    smpPhyControlRequest.EnableSasSlumber               = 0;
                                                            //set the Enable SAS Slumber field to CO which is two bits wide.
                                                            // The defined values for CO are 0 for no change;
                                                            // 1 for manage slumber phy power conditions; 
                                                            // 2 for disable slumber phy power conditions.

    smpPhyControlRequest.PwrDisControl                  = 0;

                                                            //where PDC is the power disable control value. 
                                                            //The default value is 0 which means no change. 
                                                            //The value of 1 is reserved; 2, if supported, instructs the management device server to negate the POWER DISABLE signal;
                                                              // while 3, if supported, instructs it to assert that signal. This applies to the phy given by ID (which defaults to 0).
    smpPhyControlRequest.UpdatePartialPathTimeoutValue  = 0;  // set to one to tell the phy to change the timeout value specified in PartialPathTimeoutValue.
                                                              // set to zero means ignore PartialPathTimeoutValue.


    /* We need to swap the bytes */
    //the structure defined as big endian, need change to little endian
    ptrSmpPhyControlRequest = &smpPhyControlRequest;
    
    for (index = 0; index < (U32) ((sizeof (SMP_PHY_CONTROL_REQUEST)) / 4); index++)
    {
        ((PU32) ptrSmpPhyControlRequest)[index] = swap32 (((PU32) ptrSmpPhyControlRequest)[index]);
    }
	
    size = sizeof (SMP_PHY_CONTROL_RESPONSE);

    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpPhyControlRequest, sizeof (SMP_PHY_CONTROL_REQUEST), (PU8) &smpPhyControlResponse, size);
    
    gPtrLoggerExpanders->logiDebug("smpPhyControlResponse Function = %x, FunctionResult = %x",smpPhyControlResponse.Header.Fields.Function, smpPhyControlResponse.Header.Fields.FunctionResult);
    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiPhyControl (SMPPassthrough Status=%x)", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiPhyControl  (Status = %x) ",status);
        return (status);
    }

    if ((smpPhyControlResponse.Header.Fields.Function != SMP_FUNCTION_PHY_CONTROL) || smpPhyControlResponse.Header.Fields.FunctionResult)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiPhyControl (SMP Function Failed=%x, Result=%x)", smpPhyControlResponse.Header.Fields.Function, smpPhyControlResponse.Header.Fields.FunctionResult);
		status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerExpanders->logiFunctionExit ("expiPhyControl  (Status = %x) ",status);
        return (status);
    }

    gPtrLoggerExpanders->logiFunctionExit ("expiPhyControl  (Status = %x) ",status);

    return (status);

}



/**
 *
 *  @method  expiGetTotalPhys()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrNumOfPhys            point to total number of expander Phy
 *
 *  @return  Status                  '0' for success and  non-zero for failure
 *
 *  @brief                           This method will total number of expander phy by SMP 
 *
 */

SCRUTINY_STATUS expiGetTotalPhys (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU8 PtrNumOfPhys)
{
    U32 						    index;
    SCRUTINY_STATUS 			    status = SCRUTINY_STATUS_FAILED;
    U32 						    size = sizeof (SMP_REPORT_GENERAL_RESPONSE);
    SMP_REPORT_GENERAL_REQUEST 	    smpReportGeneralRequest;
    PTR_SMP_REPORT_GENERAL_REQUEST  ptrSmpReportGeneralRequest;
    SMP_REPORT_GENERAL_RESPONSE 	smpReportGeneralResponse;
    
    gPtrLoggerExpanders->logiFunctionEntry ("expiGetTotalPhys (PtrDevice=%x, PtrNumOfPhys=%x)", PtrDevice != NULL, PtrNumOfPhys != NULL);
    if (PtrNumOfPhys == NULL)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerExpanders->logiFunctionExit ("expiGetTotalPhys  (Status = %x) ",status);
        return (status);

    }

    sosiMemSet (&smpReportGeneralRequest, 0, sizeof (SMP_REPORT_GENERAL_REQUEST));
    sosiMemSet (&smpReportGeneralResponse, 0, sizeof (SMP_REPORT_GENERAL_RESPONSE));

    //let below parameters, others keep default value 0
    smpReportGeneralRequest.SMPFrameType                  = SMP_FRAME_TYPE_SMP_REQUEST;
    smpReportGeneralRequest.Function                      = SMP_FUNCTION_REPORT_GENERAL;
    smpReportGeneralRequest.RequestLength                 = 0;           // this is fixed for SMP_FUNCTION_REPORT_GENERAL
    smpReportGeneralRequest.AllocRespLength               = 0;           // if the value is not zero, it limit the data bytes that smp target can transfer,
                                                                         // 0 means the SMP will transfer data as the function specify.


    /* We need to swap the bytes */
    //the structure defined as big endian, need change to little endian
    ptrSmpReportGeneralRequest = &smpReportGeneralRequest;
    
    for (index = 0; index < (U32) ((sizeof (SMP_REPORT_GENERAL_REQUEST)) / 4); index++)
    {
        ((PU32) ptrSmpReportGeneralRequest)[index] = swap32 (((PU32) ptrSmpReportGeneralRequest)[index]);
    }
	
    size = sizeof (SMP_REPORT_GENERAL_RESPONSE);

    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpReportGeneralRequest, sizeof (SMP_REPORT_GENERAL_REQUEST), (PU8) &smpReportGeneralResponse, size);

    gPtrLoggerExpanders->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", &smpReportGeneralResponse, size);  /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/
    gPtrLoggerExpanders->logiDumpMemoryInVerbose (((PU8) (&smpReportGeneralResponse)), size, gPtrLoggerExpanders);          /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/

    
    gPtrLoggerExpanders->logiDebug("smpReportGeneralResponse Function = %x, FunctionResult = %x",smpReportGeneralResponse.Header.Fields.Function, smpReportGeneralResponse.Header.Fields.FunctionResult);
    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiGetTotalPhys (SMPPassthrough Status=%x)", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiGetTotalPhys  (Status = %x) ",status);
        return (status);
    }

    if ((smpReportGeneralResponse.Header.Fields.Function != SMP_FUNCTION_REPORT_GENERAL) && smpReportGeneralResponse.Header.Fields.FunctionResult)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiGetTotalPhys (SMP Function Failed=%x, Result=%x)", smpReportGeneralResponse.Header.Fields.Function, smpReportGeneralResponse.Header.Fields.FunctionResult);
		status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerExpanders->logiFunctionExit ("expiGetTotalPhys  (Status = %x) ",status);
        return (status);
    }
    
    *PtrNumOfPhys = smpReportGeneralResponse.Response.Fields.NumberOfPhys;
    gPtrLoggerExpanders->logiFunctionExit ("expiGetTotalPhys  (Status = %x) ",status);

    return (status);

}

/**
 *
 *  @method  expiGetPhyErrCounters()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PtrSasPhyErrCounter     Get the  error counter 
 *
 *  @return  Status                  '0' for success and  non-zero for failure
 *
 *  @brief                           This method do some phy control operation on expander phy by SMP 
 *
 */

SCRUTINY_STATUS expiGetPhyErrCounters (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter)
{

    U32 						            index;
    SCRUTINY_STATUS 			            status = SCRUTINY_STATUS_FAILED;
    U32 						            size = sizeof (SMP_REPORT_PHY_ERROR_LOG_RESPONSE);
    SMP_REPORT_PHY_ERROR_LOG_REQUEST 	    smpReportPhyErrorLogRequest;
    PTR_SMP_REPORT_PHY_ERROR_LOG_REQUEST    ptrSmpReportPhyErrorLogRequest;
    SMP_REPORT_PHY_ERROR_LOG_RESPONSE 	    smpReportPhyErrorLogResponse;
    
    gPtrLoggerExpanders->logiFunctionEntry ("expiGetPhyErrCounters (PtrDevice=%x, PhyIdentifier, PtrSasPhyErrCounter=%x)", PtrDevice != NULL, PhyIdentifier, PtrSasPhyErrCounter != NULL);

    sosiMemSet (&smpReportPhyErrorLogRequest, 0, sizeof (SMP_REPORT_PHY_ERROR_LOG_REQUEST));
    sosiMemSet (&smpReportPhyErrorLogResponse, 0, sizeof (SMP_REPORT_PHY_ERROR_LOG_RESPONSE));

    //let below parameters, others keep default value 0
    smpReportPhyErrorLogRequest.SMPFrameType                  = SMP_FRAME_TYPE_SMP_REQUEST;
    smpReportPhyErrorLogRequest.Function                      = SMP_FUNCTION_REPORT_PHY_ERROR_LOG;
    smpReportPhyErrorLogRequest.RequestLength                 = 0x02;           // this is fixed for SMP_FUNCTION_REPORT_PHY_ERROR_LOG
    smpReportPhyErrorLogRequest.AllocRespLength               = 0;           // if the value is not zero, it limit the data bytes that smp target can transfer,
                                                                         // 0 means the SMP will transfer data as the function specify.
    smpReportPhyErrorLogRequest.PhyIdentifier                 = PhyIdentifier;

    /* We need to swap the bytes */
    //the structure defined as big endian, need change to little endian
    ptrSmpReportPhyErrorLogRequest = &smpReportPhyErrorLogRequest;
    
    for (index = 0; index < (U32) ((sizeof (SMP_REPORT_PHY_ERROR_LOG_REQUEST)) / 4); index++)
    {
        ((PU32) ptrSmpReportPhyErrorLogRequest)[index] = swap32 (((PU32) ptrSmpReportPhyErrorLogRequest)[index]);
    }
	
    size = sizeof (SMP_REPORT_PHY_ERROR_LOG_RESPONSE);

    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpReportPhyErrorLogRequest, sizeof (SMP_REPORT_PHY_ERROR_LOG_REQUEST), (PU8) &smpReportPhyErrorLogResponse, size);

    gPtrLoggerExpanders->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", &smpReportPhyErrorLogResponse, size);  /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/
    gPtrLoggerExpanders->logiDumpMemoryInVerbose (((PU8) (&smpReportPhyErrorLogResponse)), size, gPtrLoggerExpanders);          /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/

    
    gPtrLoggerExpanders->logiDebug("smpReportPhyErrorLogResponse Function = %x, FunctionResult = %x",smpReportPhyErrorLogResponse.Header.Fields.Function, smpReportPhyErrorLogResponse.Header.Fields.FunctionResult);
    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiGetPhyErrCounters (SMPPassthrough Status=%x)", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyErrCounters  (Status = %x) ",status);
        return (status);
    }

    if ((smpReportPhyErrorLogResponse.Header.Fields.Function != SMP_FUNCTION_REPORT_PHY_ERROR_LOG) || smpReportPhyErrorLogResponse.Header.Fields.FunctionResult)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiGetPhyErrCounters (SMP Function Failed=%x, Result=%x)", smpReportPhyErrorLogResponse.Header.Fields.Function, smpReportPhyErrorLogResponse.Header.Fields.FunctionResult);
		status = SCRUTINY_STATUS_IGNORE;
        gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyErrCounters  (Status = %x) ",status);
        return (status);
    }
    
    
    PtrSasPhyErrCounter->InvalidDwordCount = smpReportPhyErrorLogResponse.Response.Fields.InvalidDwordCount;
    PtrSasPhyErrCounter->LossDwordSynchCount = smpReportPhyErrorLogResponse.Response.Fields.LossOfDwordSynchronizationCount;
    PtrSasPhyErrCounter->PhyResetProblemCount = smpReportPhyErrorLogResponse.Response.Fields.PhyResetProblemCount;
    PtrSasPhyErrCounter->RunningDisparityErrorCount = smpReportPhyErrorLogResponse.Response.Fields.RunningDisparityErrorCount;
    
    gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyErrCounters  (Status = %x) ",status);

    return (status);


}

/**
 *
 *  @method  expiResetPhy()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PhyOperation            Phy control operation,=1 link reset, =2 hard reset =3 clear error counter 
 *
 *  @param   DoAllPhys               if the operation for single phy or all phys,=TRUE for all phys, =FALSE for single phy identified by  PhyIdentifier
 *
 *  @return  Status                  '0' for success and  non-zero for failure
 *
 *  @brief                           This method do some phy control operation on expander phy by SMP 
 *
 */


SCRUTINY_STATUS expiResetPhy (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __IN__ U8  PhyOperation, __IN__ BOOLEAN  DoAllPhys)
{
    U8 						         phyIndex;
    U8                               numberOfPhys;
    U8                               startPhy;
    U8                               endPhy;
    U8                               interalPhyOperation;
    SCRUTINY_STATUS 			     status = SCRUTINY_STATUS_FAILED;
    //SCRUTINY_SAS_ERROR_COUNTERS      sasPhyErrCounter;

    gPtrLoggerExpanders->logiFunctionEntry ("expiResetPhy (PtrDevice=%x, PhyIdentifier=%x, PhyOperation=%x, DoAllPhys=%x)", PtrDevice != NULL, PhyIdentifier, PhyOperation, DoAllPhys);

    status = expiGetTotalPhys (PtrDevice,  &numberOfPhys);
    gPtrLoggerExpanders->logiDebug ("expiResetPhy get total phy number numberOfPhys=%x", numberOfPhys);
    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiResetPhy get total phy number failed Status=%x", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiResetPhy  (Status = %x) ",status);
        return (status);
    }

    //decide the start and end phy.    
    if (DoAllPhys == TRUE)
    {
        startPhy = 0;
        endPhy =  startPhy + numberOfPhys - 1;
    }
    else
    {
        startPhy = PhyIdentifier;
        endPhy = startPhy;
    }

    //Transfer customer operation code to internal operation code
    switch (PhyOperation)
    {
        case 1:
        {
            interalPhyOperation = SMP_PHY_CONTROL_PHY_OPERATION_LINK_RESET;
            break; 
        }
        case 2:
        {
            interalPhyOperation = SMP_PHY_CONTROL_PHY_OPERATION_HARD_RESET;
            break; 

        }
        case 3:
        {
            interalPhyOperation = SMP_PHY_CONTROL_PHY_OPERATION_CLEAR_ERROR_LOG;
            break;
        }
        default:
        {
            status = SCRUTINY_STATUS_INVALID_PARAMETER;
            gPtrLoggerExpanders->logiDebug ("Invalid operation = %x", PhyOperation);
            gPtrLoggerExpanders->logiFunctionExit ("expiResetPhy  (Status = %x) ",status);
            return (status);

        }
    
    }

    //How to decide the status for all phys cases?so far no good solutions.
    //currently if only one phy ,then it can return the status.
    //for multi phys case , I can't just return if one of phys failed ,I will continue the left phys.    
    for (phyIndex = startPhy; phyIndex <= endPhy; phyIndex++)
    {   //below is for debug use, it is reading ,so it will not affect other operation
        //expiGetPhyErrCounters (PtrDevice, phyIndex, &sasPhyErrCounter);
        //gPtrLoggerExpanders->logiDebug ("Before phyIndex = %x, InvalidDwordCount = %x, LossDwordSynchCount = %x, PhyResetProblemCount = %x, RunningDisparityErrorCount = %x", phyIndex, sasPhyErrCounter.InvalidDwordCount, sasPhyErrCounter.LossDwordSynchCount, sasPhyErrCounter.PhyResetProblemCount, sasPhyErrCounter.RunningDisparityErrorCount);

        status = expiPhyControl (PtrDevice, phyIndex, interalPhyOperation);
        
        //below is for debug use, it is reading ,so it will not affect other operation
        //expiGetPhyErrCounters (PtrDevice, phyIndex, &sasPhyErrCounter);
        //gPtrLoggerExpanders->logiDebug ("After phyIndex = %x, InvalidDwordCount = %x, LossDwordSynchCount = %x, PhyResetProblemCount = %x, RunningDisparityErrorCount = %x", phyIndex, sasPhyErrCounter.InvalidDwordCount, sasPhyErrCounter.LossDwordSynchCount, sasPhyErrCounter.PhyResetProblemCount, sasPhyErrCounter.RunningDisparityErrorCount);

        gPtrLoggerExpanders->logiDebug ("phyIndex = %x, interalPhyOperation = %x, status = %x", phyIndex, interalPhyOperation, status);
    }

    gPtrLoggerExpanders->logiFunctionExit ("expiResetPhy  (Status = %x) ",status);

    return (status);
}