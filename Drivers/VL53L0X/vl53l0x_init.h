

#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include <stdint.h>
#include <stdbool.h>
VL53L0X_Error vl53l0x_init(VL53L0X_DEV pMyDevice, VL53L0X_Version_t *pVersion, VL53L0X_DeviceInfo_t *pDeviceInfo, uint8_t address);
VL53L0X_Error vl53l0x_GetRanging_now(VL53L0X_DEV pMyDevice, uint16_t *result);
VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev);
VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev);
VL53L0X_Error vl53l0x_GetRanging_last(VL53L0X_DEV pMyDevice, uint16_t *result);
