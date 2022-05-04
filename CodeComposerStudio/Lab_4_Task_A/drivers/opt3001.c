/**************************************************************************************************
 *  Filename:       opt3001.c
 *  Revised:        
 *  Revision:       
 *
 *  Description:    Driver for the Texas Instruments OP3001 Optical Sensor
 *
 *  Copyright (C) 2014 - 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "i2cOptDriver.h"
#include "opt3001.h"
#include "utils/uartstdio.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

/* Slave address */
#define OPT3001_I2C_ADDRESS             0x47

/* Register addresses */
#define REG_RESULT                      0x00
#define REG_CONFIGURATION               0x01
#define REG_LOW_LIMIT                   0x02
#define REG_HIGH_LIMIT                  0x03

#define REG_MANUFACTURER_ID             0x7E
#define REG_DEVICE_ID                   0x7F

/* Register values */
#define MANUFACTURER_ID                 0x5449  // TI
#define DEVICE_ID                       0x3001  // Opt 3001
#define CONFIG_RESET                    0xC810                   
#define CONFIG_TEST                     0xCC10
#define CONFIG_ENABLE                   0x10C4 //0x10C4
#define CONFIG_DISABLE                  0x10C0 // 0xC010   - 100 ms, shutdown

/* Bit values */
#define DATA_RDY_BIT                    0x0080  // Data ready

#define FLAG_HIGH_BIT                 0x0060 //0x0070
#define FLAG_LOW_BIT                  0x00// 0x0060



/* Register length */
#define REGISTER_LENGTH                 2

/* Sensor data size */
#define DATA_LENGTH                     2

/* ------------------------------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Public functions
 * -------------------------------------------------------------------------------------------------
 */


void SensorOpt3001SetLow()
{
    uint16_t val;
    val = 0xFFF;
    writeI2C(OPT3001_I2C_ADDRESS, REG_LOW_LIMIT, (uint8_t*)&val);
}
void SensorOpt3001SetHigh()
{
    uint16_t val;
    val = 0x789A;
    writeI2C(OPT3001_I2C_ADDRESS, REG_HIGH_LIMIT, (uint8_t*)&val);
}

bool isHighFlag(uint16_t *rawData)
{
    bool success;
    uint16_t val;

    sensorOpt3001Read(&val);

    success = ( (val >> 7 & 0x00FF) & FLAG_HIGH_BIT) == FLAG_HIGH_BIT;

    if (success){
        sensorOpt3001Read(&val);

        *rawData = (val << 7) | (val>>7 &0xFF);
    } else {
        *rawData = 0;
    }

    return success;
}

bool isLowFlag(uint16_t *rawData)
{
    bool success;
        uint16_t val;

        sensorOpt3001Read(&val);

        success = ( (val >> 6 & 0x00FF) & FLAG_LOW_BIT) == FLAG_LOW_BIT;

        if (success){
            sensorOpt3001Read(&val);
            *rawData = (val << 6) | (val>>6 &0xFF);
        } else {
            *rawData = 0;
        }

        return success;
}

/**************************************************************************************************
 * @fn          sensorOpt3001Init
 *
 * @brief       Initialize the light sensor driver
 *
 * @return      none
 **************************************************************************************************/
bool sensorOpt3001Init(void)
{
    sensorOpt3001Enable(false);
    SensorOpt3001SetLow();
    SensorOpt3001SetHigh();
    return (true);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Enable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001Enable(bool enable)
{
    uint16_t val;

    if (enable)
    {
        val = CONFIG_ENABLE;
    }
    else
    {
        val = CONFIG_DISABLE;
    }

    writeI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t*)&val);
}


void SensorOpt3001ReadResult(uint16_t *rawData)
{
    readI2C(OPT3001_I2C_ADDRESS, REG_RESULT, (uint8_t *)rawData);
}

/*
 * Reads CONFIGURATION register
 * Returns contents of register
 */
void SensorOpt3001ReadConfig(uint16_t *rawData)
{
    readI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t *)rawData);
}


/**************************************************************************************************
 * @fn          sensorOpt3001Read
 *
 * @brief       Read the result register
 *
 * @param       Buffer to store data in
 *
 * @return      TRUE if valid data
 **************************************************************************************************/
bool sensorOpt3001Read(uint16_t *rawData)
{
    bool success;
    uint16_t val;

    success = readI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t *)&val);


    if (success)
    {
        success = ((val>>8 &0xFF) & DATA_RDY_BIT) == DATA_RDY_BIT;
    }

    if (success)
    {
        success = readI2C(OPT3001_I2C_ADDRESS, REG_RESULT, (uint8_t *)&val);
    }

    if (success)
    {
        // Swap bytes
        *rawData = (val << 8) | (val>>8 &0xFF);
    }
    else
    {
        //sensorSetErrorData
    }

    return (success);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 **************************************************************************************************/
bool sensorOpt3001Test(void)
{
    uint16_t val;

    // Check manufacturer ID
    readI2C(OPT3001_I2C_ADDRESS, REG_MANUFACTURER_ID, (uint8_t *)&val);
    val = (val << 8) | (val>>8 &0xFF);

    if (val != MANUFACTURER_ID)
    {
        return (false);
    }

    UARTprintf("Manufacturer ID Correct: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);

    // Check device ID
    readI2C(OPT3001_I2C_ADDRESS, REG_DEVICE_ID, (uint8_t *)&val);
    val = (val << 8) | (val>>8 &0xFF);

    if (val != DEVICE_ID)
    {
        return (false);
    }

    UARTprintf("Device ID Correct: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);

    return (true);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Convert
 *
 * @brief       Convert raw data to object and ambience temperature
 *
 * @param       rawData - raw data from sensor
 *
 * @param       convertedLux - converted value (lux)
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001Convert(uint16_t rawData, float *convertedLux)
{
    uint16_t e, m;

    m = rawData & 0x0FFF;
    e = (rawData & 0xF000) >> 12;

    *convertedLux = m * (0.01 * exp2(e));
}
