#include "vl53l0x_init.h"

//ContinuousRanging
VL53L0X_Error vl53l0x_init(VL53L0X_DEV pMyDevice, VL53L0X_Version_t *pVersion, VL53L0X_DeviceInfo_t *pDeviceInfo, uint8_t address){
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    
	int32_t status_int;

    status_int = VL53L0X_GetVersion(pVersion);
    if (status_int != 0){
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    HAL_Delay(10);


    if(Status == VL53L0X_ERROR_NONE){
    	Status = VL53L0X_DataInit(pMyDevice); // Data initialization
    }
    HAL_Delay(10);

    if(Status == VL53L0X_ERROR_NONE){
    	HAL_Delay(10);
    	Status = VL53L0X_SetDeviceAddress(pMyDevice, address);
    	pMyDevice->I2cDevAddr = address;
    }
    HAL_Delay(10);

    if(Status == VL53L0X_ERROR_NONE){
    	Status = VL53L0X_GetDeviceInfo(pMyDevice, pDeviceInfo);
    }
    HAL_Delay(10);

    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
    }
    HAL_Delay(10);

    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

    if(Status == VL53L0X_ERROR_NONE){
    	Status = VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings, &PhaseCal); // Device Initialization
    }
    HAL_Delay(10);

    if(Status == VL53L0X_ERROR_NONE){
    	Status = VL53L0X_PerformRefSpadManagement(pMyDevice, &refSpadCount, &isApertureSpads); // Device Initialization
    }
    HAL_Delay(10);

    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING); // Setup in single ranging mode
    }
    if (Status == VL53L0X_ERROR_NONE) {
            Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
            		VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    }
    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
        		VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
    }
    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
        		VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
        		(FixPoint1616_t)(0.25*65536));
    }
    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
        		VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
        		(FixPoint1616_t)(32*65536));
    }
    if(Status == VL53L0X_ERROR_NONE){
    	Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice, 30000);
    }
    HAL_Delay(10);
    if(Status == VL53L0X_ERROR_NONE){
    	Status = VL53L0X_StartMeasurement(pMyDevice);
    }
    return Status;
}

VL53L0X_Error vl53l0x_GetRanging_now(VL53L0X_DEV pMyDevice, uint16_t *result) {
	VL53L0X_RangingMeasurementData_t RangingMeasurementData;
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData = &RangingMeasurementData;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	Status = WaitMeasurementDataReady(pMyDevice);
	if(Status == VL53L0X_ERROR_NONE){
		Status = VL53L0X_GetRangingMeasurementData(pMyDevice, pRangingMeasurementData);
	    *result = pRangingMeasurementData->RangeMilliMeter;
		VL53L0X_ClearInterruptMask(pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
	    VL53L0X_PollingDelay(pMyDevice);
	}
	return Status;
}

VL53L0X_Error vl53l0x_GetRanging_last(VL53L0X_DEV pMyDevice, uint16_t *result){
	VL53L0X_RangingMeasurementData_t RangingMeasurementData;
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData = &RangingMeasurementData;
	VL53L0X_Error Status = VL53L0X_GetRangingMeasurementData(pMyDevice, pRangingMeasurementData);
	*result = pRangingMeasurementData->RangeMilliMeter;
	VL53L0X_ClearInterruptMask(pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
	VL53L0X_PollingDelay(pMyDevice);
	return Status;
}

VL53L0X_Error vl53l0x_GetRanging2_now(VL53L0X_DEV pMyDevice1, VL53L0X_DEV pMyDevice2,
												uint16_t *result1, uint16_t *result2){
	uint8_t NewDatReady1 = 0;
	uint8_t NewDatReady2 = 0;
	uint32_t LoopNb;
	uint8_t flag1 = 1, flag2 = 1;
	VL53L0X_Error Status1 = VL53L0X_ERROR_NONE;
	VL53L0X_Error Status2 = VL53L0X_ERROR_NONE;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;

	VL53L0X_RangingMeasurementData_t RangingMeasurementData1,
										RangingMeasurementData2;
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData1 = &RangingMeasurementData1,
										*pRangingMeasurementData2 = &RangingMeasurementData2;
	do {
		if(flag1){
			Status1 = VL53L0X_GetMeasurementDataReady(pMyDevice1, &NewDatReady1);
			if ((NewDatReady1 == 0x01) || Status1 != VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetRangingMeasurementData(pMyDevice1, pRangingMeasurementData1);
				*result1 = pRangingMeasurementData1->RangeMilliMeter;
				VL53L0X_ClearInterruptMask(pMyDevice1, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				flag1 = 0;
			}
		}
		if(flag2){
			Status2 = VL53L0X_GetMeasurementDataReady(pMyDevice2, &NewDatReady2);
			if ((NewDatReady2 == 0x01) || Status2 != VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetRangingMeasurementData(pMyDevice2, pRangingMeasurementData2);
				*result2 = pRangingMeasurementData2->RangeMilliMeter;
				VL53L0X_ClearInterruptMask(pMyDevice2, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				flag2 = 0;
			}
		}
		if(!flag1 && !flag2){
			break;
		}
		LoopNb = LoopNb + 1;
		VL53L0X_PollingDelay(pMyDevice1);
	} while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);
	return Status;
}

VL53L0X_Error vl53l0x_GetRanging4_now(VL53L0X_DEV pMyDevice1, VL53L0X_DEV pMyDevice2,
											VL53L0X_DEV pMyDevice3, VL53L0X_DEV pMyDevice4,
												uint16_t *result1, uint16_t *result2,
													uint16_t *result3, uint16_t *result4){
	uint8_t NewDatReady1 = 0;
	uint8_t NewDatReady2 = 0;
	uint8_t NewDatReady3 = 0;
	uint8_t NewDatReady4 = 0;
	uint32_t LoopNb;
	uint8_t flag1 = 1,
			 flag2 = 1,
			 flag3 = 1,
			 flag4 = 1;
	VL53L0X_Error Status1 = VL53L0X_ERROR_NONE,
					Status2 = VL53L0X_ERROR_NONE,
					Status3 = VL53L0X_ERROR_NONE,
					Status4 = VL53L0X_ERROR_NONE;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	VL53L0X_RangingMeasurementData_t RangingMeasurementData1,
										RangingMeasurementData2,
										RangingMeasurementData3,
										RangingMeasurementData4;
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData1 = &RangingMeasurementData1,
										*pRangingMeasurementData2 = &RangingMeasurementData2,
										*pRangingMeasurementData3 = &RangingMeasurementData3,
										*pRangingMeasurementData4 = &RangingMeasurementData4;
	do {
		if(flag1){
			Status1 = VL53L0X_GetMeasurementDataReady(pMyDevice1, &NewDatReady1);
			if ((NewDatReady1 == 0x01) || Status1 != VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetRangingMeasurementData(pMyDevice1, pRangingMeasurementData1);
				*result1 = pRangingMeasurementData1->RangeMilliMeter;
				VL53L0X_ClearInterruptMask(pMyDevice1, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				flag1 = 0;
			}
		}
		if(flag2){
			Status2 = VL53L0X_GetMeasurementDataReady(pMyDevice2, &NewDatReady2);
			if ((NewDatReady2 == 0x01) || Status2 != VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetRangingMeasurementData(pMyDevice2, pRangingMeasurementData2);
				*result2 = pRangingMeasurementData2->RangeMilliMeter;
				VL53L0X_ClearInterruptMask(pMyDevice2, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				flag2 = 0;
			}
		}
		if(flag3){
			Status3 = VL53L0X_GetMeasurementDataReady(pMyDevice3, &NewDatReady3);
			if ((NewDatReady3 == 0x01) || Status3 != VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetRangingMeasurementData(pMyDevice3, pRangingMeasurementData3);
				*result3 = pRangingMeasurementData3->RangeMilliMeter;
				VL53L0X_ClearInterruptMask(pMyDevice3, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				flag3 = 0;
			}
		}
		if(flag4){
			Status4 = VL53L0X_GetMeasurementDataReady(pMyDevice4, &NewDatReady4);
			if ((NewDatReady4 == 0x01) || Status4 != VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetRangingMeasurementData(pMyDevice4, pRangingMeasurementData4);
				*result4 = pRangingMeasurementData4->RangeMilliMeter;
				VL53L0X_ClearInterruptMask(pMyDevice4, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				flag4 = 0;
			}
		}

		if(!flag1 && !flag2 && !flag3 && !flag4){
			break;
		}
		LoopNb = LoopNb + 1;
		VL53L0X_PollingDelay(pMyDevice1);
	} while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);
	return Status;
}

VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint32_t StopCompleted=0;
    uint32_t LoopNb;

    // Wait until it finished
    // use timeout to avoid deadlock
    if (Status == VL53L0X_ERROR_NONE) {
        LoopNb = 0;
        do {
            Status = VL53L0X_GetStopCompletedStatus(Dev, &StopCompleted);
            if ((StopCompleted == 0x00) || Status != VL53L0X_ERROR_NONE) {
                break;
            }
            LoopNb = LoopNb + 1;
            VL53L0X_PollingDelay(Dev);
        } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

        if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
            Status = VL53L0X_ERROR_TIME_OUT;
        }

    }

    return Status;
}

VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t NewDatReady=0;
    uint32_t LoopNb;

    // Wait until it finished
    // use timeout to avoid deadlock
    if (Status == VL53L0X_ERROR_NONE) {
        LoopNb = 0;
        do {
            Status = VL53L0X_GetMeasurementDataReady(Dev, &NewDatReady);
            if ((NewDatReady == 0x01) || Status != VL53L0X_ERROR_NONE) {
                break;
            }
            LoopNb = LoopNb + 1;
            VL53L0X_PollingDelay(Dev);
        } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

        if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
            Status = VL53L0X_ERROR_TIME_OUT;
        }
    }

    return Status;
}

