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
 *  @method  expiEnableDisablePhy()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PhyOperation            Phy enable or disable operation 
 *
 *  @return  Status                  '0' for success and  non-zero for failure
 *
 *  @brief                      This method will enable or disable expander phy by SMP 
 *
 */

SCRUTINY_STATUS expiEnableDisablePhy (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __IN__ U8  PhyOperation)
{
    U32 						index;
    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    U32 						size = sizeof (SMP_PHY_CONTROL_RESPONSE);
    SMP_PHY_CONTROL_REQUEST 	smpPhyControlRequest;
    PTR_SMP_PHY_CONTROL_REQUEST ptrsmpPhyControlRequest;
    SMP_PHY_CONTROL_RESPONSE 	smpPhyControlResponse;

    gPtrLoggerExpanders->logiFunctionEntry ("expiEnableDisablePhy (PtrDevice=%x, PhyIdentifier=%x, PhyOperation=%x)", PtrDevice != NULL, PhyIdentifier, PhyOperation);

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
    ptrsmpPhyControlRequest = &smpPhyControlRequest;
    
    for (index = 0; index < (U32) ((sizeof (SMP_PHY_CONTROL_REQUEST)) / 4); index++)
    {
        ((PU32) ptrsmpPhyControlRequest)[index] = swap32 (((PU32) ptrsmpPhyControlRequest)[index]);
    }
	
    size = sizeof (SMP_PHY_CONTROL_RESPONSE);

    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpPhyControlRequest, sizeof (SMP_PHY_CONTROL_REQUEST), (PU8) &smpPhyControlResponse, size);
    
    gPtrLoggerExpanders->logiDebug("smpPhyControlResponse Function = %x, FunctionResult = %x",smpPhyControlResponse.Header.Fields.Function, smpPhyControlResponse.Header.Fields.FunctionResult);
    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiEnableDisablePhy (SMPPassthrough Status=%x)", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiEnableDisablePhy  (Status = %x) ",status);
        return (status);
    }

    if (!smpPhyControlResponse.Header.Fields.Function || smpPhyControlResponse.Header.Fields.FunctionResult)
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiEnableDisablePhy (SMP Function Failed=%x, Result=%x)", smpPhyControlResponse.Header.Fields.Function, smpPhyControlResponse.Header.Fields.FunctionResult);
		gPtrLoggerExpanders->logiFunctionExit ("expiEnableDisablePhy  (Status = %x) ",status);
        return (status);
    }

    gPtrLoggerExpanders->logiFunctionExit ("expiEnableDisablePhy  (Status = %x) ",status);

    return (status);

}

