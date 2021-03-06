/*
 * gesture_ligth_sensor.c
 *
 * Created: 05/04/2015 12:30:41 p.m.
 *  Author: Nick
 */ 

#include "./../include/gesture_ligth_sensor.h"



/**
 * @brief Configures I2C communications and initializes registers to defaults
 *
 * @return TRUE if initialized successfully. FALSE otherwise.
 */
int APDS9960_init(void)
{
	/*Variable initiation*/
	gesture_ud_delta_ = 0;
	gesture_lr_delta_ = 0;
	gesture_ud_count_ = 0;
	gesture_lr_count_ = 0;
	gesture_near_count_ = 0;
	gesture_far_count_ = 0;
	gesture_state_ = 0;
	gesture_motion_ = DIR_NONE;
	
    uint8_t id;

    /* Initialize I2C */
	twi_master_options_t opt = {
		.speed = 50000,
		.chip  = 0x20
	};
	twi_master_setup(TWI_PORT_AD, &opt);
     
    /* Read ID register and check against known values for APDS-9960 */
    if( !wireReadDataByte(APDS9960_ID, &id) ) {
        return FALSE;
    }
    if( !(id == APDS9960_ID_1 || id == APDS9960_ID_2) ) {
        return FALSE;
    }
     
    /* Set ENABLE register to 0 (disable all features) */
    if( !setMode(ALL, OFF) ) {
        return FALSE;
    }
    
    /* Set default values for ambient light and proximity registers */
    if( !wireWriteDataByte(APDS9960_ATIME, DEFAULT_ATIME) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_WTIME, DEFAULT_WTIME) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_PPULSE, DEFAULT_PROX_PPULSE) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_CONFIG1, DEFAULT_CONFIG1) ) {
        return FALSE;
    }
    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return FALSE;
    }
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return FALSE;
    }
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return FALSE;
    }
    if( !setProxIntLowThresh(DEFAULT_PILT) ) {
        return FALSE;
    }
    if( !setProxIntHighThresh(DEFAULT_PIHT) ) {
        return FALSE;
    }
    if( !setLightIntLowThreshold(DEFAULT_AILT) ) {
        return FALSE;
    }
    if( !setLightIntHighThreshold(DEFAULT_AIHT) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_PERS, DEFAULT_PERS) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_CONFIG2, DEFAULT_CONFIG2) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_CONFIG3, DEFAULT_CONFIG3) ) {
        return FALSE;
    }
    
    /* Set default values for gesture sense registers */
    if( !setGestureEnterThresh(DEFAULT_GPENTH) ) {
        return FALSE;
    }
    if( !setGestureExitThresh(DEFAULT_GEXTH) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GCONF1, DEFAULT_GCONF1) ) {
        return FALSE;
    }
    if( !setGestureGain(DEFAULT_GGAIN) ) {
        return FALSE;
    }
    if( !setGestureLEDDrive(DEFAULT_GLDRIVE) ) {
        return FALSE;
    }
    if( !setGestureWaitTime(DEFAULT_GWTIME) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_U, DEFAULT_GOFFSET) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_D, DEFAULT_GOFFSET) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_L, DEFAULT_GOFFSET) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GOFFSET_R, DEFAULT_GOFFSET) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GPULSE, DEFAULT_GPULSE) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_GCONF3, DEFAULT_GCONF3) ) {
        return FALSE;
    }
    if( !setGestureIntEnable(DEFAULT_GIEN) ) {
        return FALSE;
    }
    
#if 0
    /* Gesture config register dump */
    uint8_t reg;
    uint8_t val;
  
    for(reg = 0x80; reg <= 0xAF; reg++) {
        if( (reg != 0x82) && \
            (reg != 0x8A) && \
            (reg != 0x91) && \
            (reg != 0xA8) && \
            (reg != 0xAC) && \
            (reg != 0xAD) )
        {
            wireReadDataByte(reg, &val);
            printf(reg, HEX);
            printf(": 0x");
            printfln(val, HEX);
        }
    }
    for(reg = 0xE4; reg <= 0xE7; reg++) {
        wireReadDataByte(reg, &val);
        printf(reg, HEX);
        printf(": 0x");
        printfln(val, HEX);
    }
#endif

    return TRUE;
}

/*******************************************************************************
 * Public methods for controlling the APDS-9960
 ******************************************************************************/

/**
 * @brief Reads and returns the contents of the ENABLE register
 *
 * @return Contents of the ENABLE register. 0xFF if error.
 */
uint8_t getMode(void)
{
    uint8_t enable_value;
    
    /* Read current ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, &enable_value) ) {
        return ERROR;
    }
    
    return enable_value;
}

/**
 * @brief Enables or disables a feature in the APDS-9960
 *
 * @param[in] mode which feature to enable
 * @param[in] enable ON (1) or OFF (0)
 * @return TRUE if operation success. FALSE otherwise.
 */
int setMode(uint8_t mode, uint8_t enable)
{
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = getMode();
    if( reg_val == ERROR ) {
        return FALSE;
    }
    
    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if( mode >= 0 && mode <= 6 ) {
        if (enable) {
            reg_val |= (1 << mode);
        } else {
            reg_val &= ~(1 << mode);
        }
    } else if( mode == ALL ) {
        if (enable) {
            reg_val = 0x7F;
        } else {
            reg_val = 0x00;
        }
    }
        
    /* Write value back to ENABLE register */
    if( !wireWriteDataByte(APDS9960_ENABLE, reg_val) ) {
        return FALSE;
    }
        
    return TRUE;
}

/**
 * @brief Starts the light (R/G/B/Ambient) sensor on the APDS-9960
 *
 * @param[in] interrupts TRUE to enable hardware interrupt on high or low light
 * @return TRUE if sensor enabled correctly. FALSE on error.
 */
int enableLightSensor(int interrupts)
{
    
    /* Set default gain, interrupts, enable power, and enable sensor */
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return FALSE;
    }
    if( interrupts ) {
        if( !setAmbientLightIntEnable(1) ) {
            return FALSE;
        }
    } else {
        if( !setAmbientLightIntEnable(0) ) {
            return FALSE;
        }
    }
    if( !enablePower() ){
        return FALSE;
    }
    if( !setMode(AMBIENT_LIGHT, 1) ) {
        return FALSE;
    }
    
    return TRUE;

}

/**
 * @brief Ends the light sensor on the APDS-9960
 *
 * @return TRUE if sensor disabled correctly. FALSE on error.
 */
int disableLightSensor(void)
{
    if( !setAmbientLightIntEnable(0) ) {
        return FALSE;
    }
    if( !setMode(AMBIENT_LIGHT, 0) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Starts the proximity sensor on the APDS-9960
 *
 * @param[in] interrupts TRUE to enable hardware external interrupt on proximity
 * @return TRUE if sensor enabled correctly. FALSE on error.
 */
int enableProximitySensor(int interrupts)
{
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return FALSE;
    }
    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return FALSE;
    }
    if( interrupts ) {
        if( !setProximityIntEnable(1) ) {
            return FALSE;
        }
    } else {
        if( !setProximityIntEnable(0) ) {
            return FALSE;
        }
    }
    if( !enablePower() ){
        return FALSE;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Starts the gesture recognition engine on the APDS-9960
 *
 * @param[in] interrupts TRUE to enable hardware external interrupt on gesture
 * @return TRUE if engine enabled correctly. FALSE on error.
 */
int enableGestureSensor(int interrupts)
{
    
    /* Enable gesture mode
       Set ENABLE to 0 (power off)
       Set WTIME to 0xFF
       Set AUX to LED_BOOST_300
       Enable PON, WEN, PEN, GEN in ENABLE 
    */
    resetGestureParameters();
    if( !wireWriteDataByte(APDS9960_WTIME, 0xFF) ) {
        return FALSE;
    }
    if( !wireWriteDataByte(APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE) ) {
        return FALSE;
    }
    if( !setLEDBoost(LED_BOOST_300) ) {
        return FALSE;
    }
    if( interrupts ) {
        if( !setGestureIntEnable(1) ) {
            return FALSE;
        }
    } else {
        if( !setGestureIntEnable(0) ) {
            return FALSE;
        }
    }
    if( !setGestureMode(1) ) {
        return FALSE;
    }
    if( !enablePower() ){
        return FALSE;
    }
    if( !setMode(WAIT, 1) ) {
        return FALSE;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return FALSE;
    }
    if( !setMode(GESTURE, 1) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Ends the gesture recognition engine on the APDS-9960
 *
 * @return TRUE if engine disabled correctly. FALSE on error.
 */
int disableGestureSensor(void)
{
    resetGestureParameters();
    if( !setGestureIntEnable(0) ) {
        return FALSE;
    }
    if( !setGestureMode(0) ) {
        return FALSE;
    }
    if( !setMode(GESTURE, 0) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Determines if there is a gesture available for reading
 *
 * @return TRUE if gesture available. FALSE otherwise.
 */
int isGestureAvailable(void)
{
    uint8_t val;
    
    /* Read value from GSTATUS register */
    if( !wireReadDataByte(APDS9960_GSTATUS, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out GVALID bit */
    val &= APDS9960_GVALID;
    
    /* Return TRUE/FALSE based on GVALID bit */
    if( val == 1) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief Processes a gesture event and returns best guessed gesture
 *
 * @return Number corresponding to gesture. -1 on error.
 */
int readGesture()
{
    uint8_t fifo_level = 0;
    uint8_t bytes_read = 0;
    uint8_t fifo_data[128];
    uint8_t gstatus;
    int motion;
    int i;
    
    /* Make sure that power and gesture is on and data is valid */
    if( !isGestureAvailable() || !(getMode() & 0b01000001) ) {
        return DIR_NONE;
    }
    
    /* Keep looping as long as gesture data is valid */
    while(1) {
    
        /* Wait some time to collect next batch of FIFO data */
        delay_ms(FIFO_PAUSE_TIME);
        
        /* Get the contents of the STATUS register. Is data still valid? */
        if( !wireReadDataByte(APDS9960_GSTATUS, &gstatus) ) {
            return ERROR;
        }
        
        /* If we have valid data, read in FIFO */
        if( (gstatus & APDS9960_GVALID) == APDS9960_GVALID ) {
        
            /* Read the current FIFO level */
            if( !wireReadDataByte(APDS9960_GFLVL, &fifo_level) ) {
                return ERROR;
            }

#if DEBUG
            printf("FIFO Level: %d \n",fifo_level);
#endif

            /* If there's stuff in the FIFO, read it into our data block */
            if( fifo_level > 0) {
                bytes_read = wireReadDataBlock(APDS9960_GFIFO_U, fifo_data, (fifo_level * 4) );
                if( bytes_read == -1 ) {
                    return ERROR;
                }
#if DEBUG
                printf("FIFO Dump: ");
                for ( i = 0; i < bytes_read; i++ ) {
                    printf("%d ",fifo_data[i]);
                }
                printf("\n");
#endif

                /* If at least 1 set of data, sort the data into U/D/L/R */
                if( bytes_read >= 4 ) {
                    for( i = 0; i < bytes_read; i += 4 ) {
                        gesture_data_.u_data[gesture_data_.index] = fifo_data[i + 0];
                        gesture_data_.d_data[gesture_data_.index] = fifo_data[i + 1];
                        gesture_data_.l_data[gesture_data_.index] = fifo_data[i + 2];
                        gesture_data_.r_data[gesture_data_.index] = fifo_data[i + 3];
                        gesture_data_.index++;
                        gesture_data_.total_gestures++;
                    }
                    
#if DEBUG
               printf("Up Data: ");
                for ( i = 0; i < gesture_data_.total_gestures; i++ ) {
                    printf("%d",gesture_data_.u_data[i]);
                }
                printf("\n");
#endif

                    /* Filter and process gesture data. Decode near/far state */
                    if( processGestureData() ) {
                        if( decodeGesture() ) {
                            //***TODO: U-Turn Gestures
#if DEBUG
                            //printfln(gesture_motion_);
#endif
                        }
                    }
                    
                    /* Reset data */
                    gesture_data_.index = 0;
                    gesture_data_.total_gestures = 0;
                }
            }
        } else {
    
            /* Determine best guessed gesture and clean up */
            delay_ms(FIFO_PAUSE_TIME);
            decodeGesture();
            motion = gesture_motion_;
#if DEBUG
            printf("END: %d \n",gesture_motion_);
#endif
            resetGestureParameters();
            return motion;
        }
    }
}

/**
 * Turn the APDS-9960 on
 *
 * @return TRUE if operation successful. FALSE otherwise.
 */
int enablePower(void)
{
    if( !setMode(POWER, 1) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * Turn the APDS-9960 off
 *
 * @return TRUE if operation successful. FALSE otherwise.
 */
int disablePower(void)
{
    if( !setMode(POWER, 0) ) {
        return FALSE;
    }
    
    return TRUE;
}

/*******************************************************************************
 * Ambient light and color sensor controls
 ******************************************************************************/

/**
 * @brief Reads the ambient (clear) light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int readAmbientLight(uint16_t *val)
{
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_CDATAL, &val_byte) ) {
        return FALSE;
    }
    *val = val_byte;
    
    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_CDATAH, &val_byte) ) {
        return FALSE;
    }
    *val = *val + ((uint16_t)val_byte << 8);
    
    return TRUE;
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int readRedLight(uint16_t *val)
{
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_RDATAL, &val_byte) ) {
        return FALSE;
    }
    *val = val_byte;
    
    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_RDATAH, &val_byte) ) {
        return FALSE;
    }
    *val = *val + ((uint16_t)val_byte << 8);
    
    return TRUE;
}
 
/**
 * @brief Reads the green light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int readGreenLight(uint16_t *val)
{
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_GDATAL, &val_byte) ) {
        return FALSE;
    }
    *val = val_byte;
    
    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_GDATAH, &val_byte) ) {
        return FALSE;
    }
    *val = *val + ((uint16_t)val_byte << 8);
    
    return TRUE;
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int readBlueLight(uint16_t *val)
{
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from clear channel, low byte register */
    if( !wireReadDataByte(APDS9960_BDATAL, &val_byte) ) {
        return FALSE;
    }
    *val = val_byte;
    
    /* Read value from clear channel, high byte register */
    if( !wireReadDataByte(APDS9960_BDATAH, &val_byte) ) {
        return FALSE;
    }
    *val = *val + ((uint16_t)val_byte << 8);
    
    return TRUE;
}

/*******************************************************************************
 * Proximity sensor controls
 ******************************************************************************/

/**
 * @brief Reads the proximity level as an 8-bit value
 *
 * @param[out] val value of the proximity sensor.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int readProximity(uint8_t *val)
{
    *val = 0;
    
    /* Read value from proximity data register */
    if( !wireReadDataByte(APDS9960_PDATA, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/*******************************************************************************
 * High-level gesture controls
 ******************************************************************************/

/**
 * @brief Resets all the parameters in the gesture data member
 */
void resetGestureParameters(void)
{
    gesture_data_.index = 0;
    gesture_data_.total_gestures = 0;
    
    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;
    
    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;
    
    gesture_near_count_ = 0;
    gesture_far_count_ = 0;
    
    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;
}

/**
 * @brief Processes the raw gesture data to determine swipe direction
 *
 * @return TRUE if near or far state seen. FALSE otherwise.
 */
int processGestureData(void)
{
    uint8_t u_first = 0;
    uint8_t d_first = 0;
    uint8_t l_first = 0;
    uint8_t r_first = 0;
    uint8_t u_last = 0;
    uint8_t d_last = 0;
    uint8_t l_last = 0;
    uint8_t r_last = 0;
    int ud_ratio_first;
    int lr_ratio_first;
    int ud_ratio_last;
    int lr_ratio_last;
    int ud_delta;
    int lr_delta;
    int i;

    /* If we have less than 4 total gestures, that's not enough */
    if( gesture_data_.total_gestures <= 4 ) {
        return FALSE;
    }
    
    /* Check to make sure our data isn't out of bounds */
    if( (gesture_data_.total_gestures <= 32) && \
        (gesture_data_.total_gestures > 0) ) {
        
        /* Find the first value in U/D/L/R above the threshold */
        for( i = 0; i < gesture_data_.total_gestures; i++ ) {
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {
                
                u_first = gesture_data_.u_data[i];
                d_first = gesture_data_.d_data[i];
                l_first = gesture_data_.l_data[i];
                r_first = gesture_data_.r_data[i];
                break;
            }
        }
        
        /* If one of the _first values is 0, then there is no good data */
        if( (u_first == 0) || (d_first == 0) || \
            (l_first == 0) || (r_first == 0) ) {
            
            return FALSE;
        }
        /* Find the last value in U/D/L/R above the threshold */
        for( i = gesture_data_.total_gestures - 1; i >= 0; i-- ) {
#if DEBUG
            printf("Finding last: \n    U: %d \n   D: %d \n   L: %d \n   R: %d \n",gesture_data_.u_data[i],gesture_data_.d_data[i],gesture_data_.l_data[i],gesture_data_.r_data[i]);
#endif
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {
                
                u_last = gesture_data_.u_data[i];
                d_last = gesture_data_.d_data[i];
                l_last = gesture_data_.l_data[i];
                r_last = gesture_data_.r_data[i];
                break;
            }
        }
    }
    
    /* Calculate the first vs. last ratio of up/down and left/right */
    ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
    lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
    ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
    lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);
       
#if DEBUG
    printf("Last Values: \n");
    printf("    U:%i \n",u_last);
    printf("    D:%i \n",d_last);
    printf("    L:%i \n",l_last);
    printf("    R:%i \n",r_last);

    printf("Ratios: \n");
    printf("    UD Fi:%i \n ",ud_ratio_first);
    printf("    UD La:%i \n ",ud_ratio_last);
    printf("    LR Fi:%i \n ",lr_ratio_first);
    printf("    LR La:%i \n ",lr_ratio_last);
#endif
       
    /* Determine the difference between the first and last ratios */
    ud_delta = ud_ratio_last - ud_ratio_first;
    lr_delta = lr_ratio_last - lr_ratio_first;
    
#if DEBUG
    printf("Deltas: \n");
    printf("    UD:%i \n ",ud_delta);
    printf("    LR:%i \n ",lr_delta);
#endif

    /* Accumulate the UD and LR delta values */
    gesture_ud_delta_ += ud_delta;
    gesture_lr_delta_ += lr_delta;
    
#if DEBUG
    printf("Accumulations: \n");
    printf("    UD:%i \n ",gesture_ud_delta_);
    printf("    LR:%i \n ",gesture_lr_delta_);
#endif
    
    /* Determine U/D gesture */
    if( gesture_ud_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = 1;
    } else if( gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = -1;
    } else {
        gesture_ud_count_ = 0;
    }
    
    /* Determine L/R gesture */
    if( gesture_lr_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = 1;
    } else if( gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = -1;
    } else {
        gesture_lr_count_ = 0;
    }
    
    /* Determine Near/Far gesture */
    if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 0) ) {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {
            
            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            } else if( (ud_delta != 0) || (lr_delta != 0) ) {
                gesture_far_count_++;
            }
            
            if( (gesture_near_count_ >= 10) && (gesture_far_count_ >= 2) ) {
                if( (ud_delta == 0) && (lr_delta == 0) ) {
                    gesture_state_ = NEAR_STATE;
                } else if( (ud_delta != 0) && (lr_delta != 0) ) {
                    gesture_state_ = FAR_STATE;
                }
                return TRUE;
            }
        }
    } else {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {
                
            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            }
            
            if( gesture_near_count_ >= 10 ) {
                gesture_ud_count_ = 0;
                gesture_lr_count_ = 0;
                gesture_ud_delta_ = 0;
                gesture_lr_delta_ = 0;
            }
        }
    }
    
#if DEBUG
    printf("  UD_CT:%i \n ",gesture_ud_count_);
    printf("  LR_CT:%i \n ",gesture_lr_count_);
    printf("  NEAR_CT:%i \n ",gesture_near_count_);
    printf("  FAR_CT:%i \n ",gesture_far_count_);
    printf("---------- \n\n");
#endif
    
    return FALSE;
}

/**
 * @brief Determines swipe direction or near/far state
 *
 * @return TRUE if near/far event. FALSE otherwise.
 */
int decodeGesture(void)
{
    /* Return if near or far event is detected */
    if( gesture_state_ == NEAR_STATE ) {
        gesture_motion_ = DIR_NEAR;
        return TRUE;
    } else if ( gesture_state_ == FAR_STATE ) {
        gesture_motion_ = DIR_FAR;
        return TRUE;
    }
    
    /* Determine swipe direction */
    if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_UP;
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_DOWN;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 1) ) {
        gesture_motion_ = DIR_RIGHT;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == -1) ) {
        gesture_motion_ = DIR_LEFT;
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else {
        return FALSE;
    }
    
    return TRUE;
}

/*******************************************************************************
 * Getters and setters for register values
 ******************************************************************************/

/**
 * @brief Returns the lower threshold for proximity detection
 *
 * @return lower threshold
 */
uint8_t getProxIntLowThresh(void)
{
    uint8_t val;
    
    /* Read value from PILT register */
    if( !wireReadDataByte(APDS9960_PILT, &val) ) {
        val = 0;
    }
    
    return val;
}

/**
 * @brief Sets the lower threshold for proximity detection
 *
 * @param[in] threshold the lower proximity threshold
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProxIntLowThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_PILT, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Returns the high threshold for proximity detection
 *
 * @return high threshold
 */
uint8_t getProxIntHighThresh(void)
{
    uint8_t val;
    
    /* Read value from PIHT register */
    if( !wireReadDataByte(APDS9960_PIHT, &val) ) {
        val = 0;
    }
    
    return val;
}

/**
 * @brief Sets the high threshold for proximity detection
 *
 * @param[in] threshold the high proximity threshold
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProxIntHighThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_PIHT, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Returns LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the value of the LED drive strength. 0xFF on failure.
 */
uint8_t getLEDDrive(void)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out LED drive bits */
    val = (val >> 6) & 0b00000011;
    
    return val;
}

/**
 * @brief Sets the LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value (0-3) for the LED drive strength
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setLEDDrive(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 6;
    val &= 0b00111111;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9960_CONTROL, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Returns receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the value of the proximity gain. 0xFF on failure.
 */
uint8_t getProximityGain(void)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out PDRIVE bits */
    val = (val >> 2) & 0b00000011;
    
    return val;
}

/**
 * @brief Sets the receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProximityGain(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9960_CONTROL, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Returns receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @return the value of the ALS gain. 0xFF on failure.
 */
uint8_t getAmbientLightGain(void)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out ADRIVE bits */
    val &= 0b00000011;
    
    return val;
}

/**
 * @brief Sets the receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setAmbientLightGain(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9960_CONTROL, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    drive &= 0b00000011;
    val &= 0b11111100;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9960_CONTROL, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Get the current LED boost value
 * 
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @return The LED boost value. 0xFF on failure.
 */
uint8_t getLEDBoost(void)
{
    uint8_t val;
    
    /* Read value from CONFIG2 register */
    if( !wireReadDataByte(APDS9960_CONFIG2, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out LED_BOOST bits */
    val = (val >> 4) & 0b00000011;
    
    return val;
}

/**
 * @brief Sets the LED current boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @param[in] drive the value (0-3) for current boost (100-300%)
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setLEDBoost(uint8_t boost)
{
    uint8_t val;
    
    /* Read value from CONFIG2 register */
    if( !wireReadDataByte(APDS9960_CONFIG2, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    boost &= 0b00000011;
    boost = boost << 4;
    val &= 0b11001111;
    val |= boost;
    
    /* Write register value back into CONFIG2 register */
    if( !wireWriteDataByte(APDS9960_CONFIG2, &val) ) {
        return FALSE;
    }
    
    return TRUE;
}    
   
/**
 * @brief Gets proximity gain compensation enable
 *
 * @return 1 if compensation is enabled. 0 if not. 0xFF on error.
 */
uint8_t getProxGainCompEnable(void)
{
    uint8_t val;
    
    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out PCMP bits */
    val = (val >> 5) & 0b00000001;
    
    return val;
}

/**
 * @brief Sets the proximity gain compensation enable
 *
 * @param[in] enable 1 to enable compensation. 0 to disable compensation.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProxGainCompEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;
    
    /* Write register value back into CONFIG3 register */
    if( !wireWriteDataByte(APDS9960_CONFIG3, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the current mask for enabled/disabled proximity photodiodes
 *
 * 1 = disabled, 0 = enabled
 * Bit    Photodiode
 *  3       UP
 *  2       DOWN
 *  1       LEFT
 *  0       RIGHT
 *
 * @return Current proximity mask for photodiodes. 0xFF on error.
 */
uint8_t getProxPhotoMask(void)
{
    uint8_t val;
    
    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, &val) ) {
        return ERROR;
    }
    
    /* Mask out photodiode enable mask bits */
    val &= 0b00001111;
    
    return val;
}

/**
 * @brief Sets the mask for enabling/disabling proximity photodiodes
 *
 * 1 = disabled, 0 = enabled
 * Bit    Photodiode
 *  3       UP
 *  2       DOWN
 *  1       LEFT
 *  0       RIGHT
 *
 * @param[in] mask 4-bit mask value
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProxPhotoMask(uint8_t mask)
{
    uint8_t val;
    
    /* Read value from CONFIG3 register */
    if( !wireReadDataByte(APDS9960_CONFIG3, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    mask &= 0b00001111;
    val &= 0b11110000;
    val |= mask;
    
    /* Write register value back into CONFIG3 register */
    if( !wireWriteDataByte(APDS9960_CONFIG3, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the entry proximity threshold for gesture sensing
 *
 * @return Current entry proximity threshold.
 */
uint8_t getGestureEnterThresh(void)
{
    uint8_t val;
    
    /* Read value from GPENTH register */
    if( !wireReadDataByte(APDS9960_GPENTH, &val) ) {
        val = 0;
    }
    
    return val;
}

/**
 * @brief Sets the entry proximity threshold for gesture sensing
 *
 * @param[in] threshold proximity value needed to start gesture mode
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureEnterThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_GPENTH, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the exit proximity threshold for gesture sensing
 *
 * @return Current exit proximity threshold.
 */
uint8_t getGestureExitThresh(void)
{
    uint8_t val;
    
    /* Read value from GEXTH register */
    if( !wireReadDataByte(APDS9960_GEXTH, &val) ) {
        val = 0;
    }
    
    return val;
}

/**
 * @brief Sets the exit proximity threshold for gesture sensing
 *
 * @param[in] threshold proximity value needed to end gesture mode
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureExitThresh(uint8_t threshold)
{
    if( !wireWriteDataByte(APDS9960_GEXTH, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the gain of the photodiode during gesture mode
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the current photodiode gain. 0xFF on error.
 */
uint8_t getGestureGain(void)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out GGAIN bits */
    val = (val >> 5) & 0b00000011;
    
    return val;
}

/**
 * @brief Sets the gain of the photodiode during gesture mode
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] gain the value for the photodiode gain
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureGain(uint8_t gain)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    gain &= 0b00000011;
    gain = gain << 5;
    val &= 0b10011111;
    val |= gain;
    
    /* Write register value back into GCONF2 register */
    if( !wireWriteDataByte(APDS9960_GCONF2, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the drive current of the LED during gesture mode
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the LED drive current value. 0xFF on error.
 */
uint8_t getGestureLEDDrive(void)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out GLDRIVE bits */
    val = (val >> 3) & 0b00000011;
    
    return val;
}

/**
 * @brief Sets the LED drive current during gesture mode
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value for the LED drive current
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureLEDDrive(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 3;
    val &= 0b11100111;
    val |= drive;
    
    /* Write register value back into GCONF2 register */
    if( !wireWriteDataByte(APDS9960_GCONF2, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the time in low power mode between gesture detections
 *
 * Value    Wait time
 *   0          0 ms
 *   1          2.8 ms
 *   2          5.6 ms
 *   3          8.4 ms
 *   4         14.0 ms
 *   5         22.4 ms
 *   6         30.8 ms
 *   7         39.2 ms
 *
 * @return the current wait time between gestures. 0xFF on error.
 */
uint8_t getGestureWaitTime(void)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, &val) ) {
        return ERROR;
    }
    
    /* Mask out GWTIME bits */
    val &= 0b00000111;
    
    return val;
}

/**
 * @brief Sets the time in low power mode between gesture detections
 *
 * Value    Wait time
 *   0          0 ms
 *   1          2.8 ms
 *   2          5.6 ms
 *   3          8.4 ms
 *   4         14.0 ms
 *   5         22.4 ms
 *   6         30.8 ms
 *   7         39.2 ms
 *
 * @param[in] the value for the wait time
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureWaitTime(uint8_t time)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !wireReadDataByte(APDS9960_GCONF2, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    time &= 0b00000111;
    val &= 0b11111000;
    val |= time;
    
    /* Write register value back into GCONF2 register */
    if( !wireWriteDataByte(APDS9960_GCONF2, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the low threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return TRUE if operation successful. FALSE otherwise.
 */
int getLightIntLowThreshold(uint16_t *threshold)
{
    uint8_t val_byte;
    *threshold = 0;
    
    /* Read value from ambient light low threshold, low byte register */
    if( !wireReadDataByte(APDS9960_AILTL, &val_byte) ) {
        return FALSE;
    }
    *threshold = val_byte;
    
    /* Read value from ambient light low threshold, high byte register */
    if( !wireReadDataByte(APDS9960_AILTH, &val_byte) ) {
        return FALSE;
    }
    *threshold = *threshold + ((uint16_t)val_byte << 8);
    
    return TRUE;
}

/**
 * @brief Sets the low threshold for ambient light interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setLightIntLowThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;
    
    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;
    
    /* Write low byte */
    if( !wireWriteDataByte(APDS9960_AILTL, val_low) ) {
        return FALSE;
    }
    
    /* Write high byte */
    if( !wireWriteDataByte(APDS9960_AILTH, val_high) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the high threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return TRUE if operation successful. FALSE otherwise.
 */
int getLightIntHighThreshold(uint16_t *threshold)
{
    uint8_t val_byte;
    *threshold = 0;
    
    /* Read value from ambient light high threshold, low byte register */
    if( !wireReadDataByte(APDS9960_AIHTL, &val_byte) ) {
        return FALSE;
    }
    *threshold = val_byte;
    
    /* Read value from ambient light high threshold, high byte register */
    if( !wireReadDataByte(APDS9960_AIHTH, &val_byte) ) {
        return FALSE;
    }
    *threshold = *threshold + ((uint16_t)val_byte << 8);
    
    return TRUE;
}

/**
 * @brief Sets the high threshold for ambient light interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setLightIntHighThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;
    
    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;
    
    /* Write low byte */
    if( !wireWriteDataByte(APDS9960_AIHTL, val_low) ) {
        return FALSE;
    }
    
    /* Write high byte */
    if( !wireWriteDataByte(APDS9960_AIHTH, val_high) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets the low threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return TRUE if operation successful. FALSE otherwise.
 */
int getProximityIntLowThreshold(uint8_t *threshold)
{
    *threshold = 0;
    
    /* Read value from proximity low threshold register */
    if( !wireReadDataByte(APDS9960_PILT, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Sets the low threshold for proximity interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProximityIntLowThreshold(uint8_t threshold)
{
    
    /* Write threshold value to register */
    if( !wireWriteDataByte(APDS9960_PILT, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}
    
/**
 * @brief Gets the high threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return TRUE if operation successful. FALSE otherwise.
 */
int getProximityIntHighThreshold(uint8_t *threshold)
{
    *threshold = 0;
    
    /* Read value from proximity low threshold register */
    if( !wireReadDataByte(APDS9960_PIHT, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Sets the high threshold for proximity interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProximityIntHighThreshold(uint8_t threshold)
{
    
    /* Write threshold value to register */
    if( !wireWriteDataByte(APDS9960_PIHT, threshold) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets if ambient light interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getAmbientLightIntEnable(void)
{
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out AIEN bit */
    val = (val >> 4) & 0b00000001;
    
    return val;
}

/**
 * @brief Turns ambient light interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 4;
    val &= 0b11101111;
    val |= enable;
    
    /* Write register value back into ENABLE register */
    if( !wireWriteDataByte(APDS9960_ENABLE, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets if proximity interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getProximityIntEnable(void)
{
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out PIEN bit */
    val = (val >> 5) & 0b00000001;
    
    return val;
}

/**
 * @brief Turns proximity interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setProximityIntEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9960_ENABLE, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;
    
    /* Write register value back into ENABLE register */
    if( !wireWriteDataByte(APDS9960_ENABLE, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Gets if gesture interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getGestureIntEnable(void)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, &val) ) {
        return ERROR;
    }
    
    /* Shift and mask out GIEN bit */
    val = (val >> 1) & 0b00000001;
    
    return val;
}

/**
 * @brief Turns gesture-related interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureIntEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 1;
    val &= 0b11111101;
    val |= enable;
    
    /* Write register value back into GCONF4 register */
    if( !wireWriteDataByte(APDS9960_GCONF4, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Clears the ambient light interrupt
 *
 * @return TRUE if operation completed successfully. FALSE otherwise.
 */
int clearAmbientLightInt(void)
{
    uint8_t throwaway;
    if( !wireReadDataByte(APDS9960_AICLEAR, &throwaway) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Clears the proximity interrupt
 *
 * @return TRUE if operation completed successfully. FALSE otherwise.
 */
int clearProximityInt(void)
{
    uint8_t throwaway;
    if( !wireReadDataByte(APDS9960_PICLEAR, &throwaway) ) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Tells if the gesture state machine is currently running
 *
 * @return 1 if gesture state machine is running, 0 if not. 0xFF on error.
 */
uint8_t getGestureMode(void)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, &val) ) {
        return ERROR;
    }
    
    /* Mask out GMODE bit */
    val &= 0b00000001;
    
    return val;
}

/**
 * @brief Tells the state machine to either enter or exit gesture state machine
 *
 * @param[in] mode 1 to enter gesture state machine, 0 to exit.
 * @return TRUE if operation successful. FALSE otherwise.
 */
int setGestureMode(uint8_t mode)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !wireReadDataByte(APDS9960_GCONF4, &val) ) {
        return FALSE;
    }
    
    /* Set bits in register to given value */
    mode &= 0b00000001;
    val &= 0b11111110;
    val |= mode;
    
    /* Write register value back into GCONF4 register */
    if( !wireWriteDataByte(APDS9960_GCONF4, val) ) {
        return FALSE;
    }
    
    return TRUE;
}

/*******************************************************************************
 * Raw I2C Reads and Writes
 ******************************************************************************/

/**
 * @brief Writes a single byte to the I2C device (no register)
 *
 * @param[in] val the 1-byte value to write to the I2C device
 * @return TRUE if successful write operation. FALSE otherwise.
 */
// int wireWriteByte(uint8_t val)
// {
//     Wire.beginTransmission(APDS9960_I2C_ADDR);
//     Wire.write(val);
//     if( Wire.endTransmission() != 0 ) {
//         return FALSE;
//     }
//     
//     return TRUE;
// }

/**
 * @brief Writes a single byte to the I2C device and specified register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val the 1-byte value to write to the I2C device
 * @return TRUE if successful write operation. FALSE otherwise.
 */
int wireWriteDataByte(uint8_t reg, uint8_t val)
{
	int write_val=val;
   	twi_package_t packet_write = 
	{
	   	.addr         = reg,      // TWI slave memory address data
	   	.addr_length  = 1,    // TWI slave memory address data size
	   	.chip         = APDS9960_I2C_ADDR,      // TWI slave bus address
	   	.buffer       = &write_val,        // transfer data destination buffer
	   	.length       = 1                    // transfer data size (bytes)
   	};
   	
   	if(twi_master_write(TWI_PORT_AD, &packet_write) == TWI_SUCCESS)
   	{
	   	return TRUE;
   	}
	
	return FALSE;
}

/**
 * @brief Writes a block (array) of bytes to the I2C device and register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val pointer to the beginning of the data byte array
 * @param[in] len the length (in bytes) of the data to write
 * @return TRUE if successful write operation. FALSE otherwise.
 */
int wireWriteDataBlock(  uint8_t reg, uint8_t *val, unsigned int len)
{
   	twi_package_t packet_write = 
	{
	   	.addr         = reg,      // TWI slave memory address data
	   	.addr_length  = 1,    // TWI slave memory address data size
	   	.chip         = APDS9960_I2C_ADDR,      // TWI slave bus address
	   	.buffer       = val,        // transfer data destination buffer
	   	.length       = len                   // transfer data size (bytes)
   	};
   	
   	if(twi_master_write(TWI_PORT_AD, &packet_write) == TWI_SUCCESS)
   	{
	   	return TRUE;
   	}
   	
   	return FALSE; 
}

/**
 * @brief Reads a single byte from the I2C device and specified register
 *
 * @param[in] reg the register to read from
 * @param[out] the value returned from the register
 * @return TRUE if successful read operation. FALSE otherwise.
 */
int wireReadDataByte(uint8_t reg, uint8_t *val)
{
   	twi_package_t packet_read = {
		.addr         = reg,      // TWI slave memory address data
	   	.addr_length  = 1,    // TWI slave memory address data size
	   	.chip         = APDS9960_I2C_ADDR,      // TWI slave bus address
	   	.buffer       = val,        // transfer data destination buffer
	   	.length       = 1                    // transfer data size (bytes)
   	};
   	
   	if(twi_master_read(TWI_PORT_AD, &packet_read) == TWI_SUCCESS)
   	{
	   	return TRUE;
   	}
   	
   	return FALSE;
}

/**
 * @brief Reads a block (array) of bytes from the I2C device and register
 *
 * @param[in] reg the register to read from
 * @param[out] val pointer to the beginning of the data
 * @param[in] len number of bytes to read
 * @return Number of bytes read. -1 on read error.
 */
int wireReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len)
{
	twi_package_t packet_read = {
		.addr         = reg,      // TWI slave memory address data
		.addr_length  = 1,    // TWI slave memory address data size
		.chip         = APDS9960_I2C_ADDR,      // TWI slave bus address
		.buffer       = val,        // transfer data destination buffer
		.length       = len                    // transfer data size (bytes)
	};
		
	if(twi_master_read(TWI_PORT_AD, &packet_read) == TWI_SUCCESS)
	{
		return len;
	}
		
	return -1;
}


//organizaaaaaaaaaaaaaaaaaaaaar


int size(int *val)
{
	int n;
	for(n=0;*val!='\0';val++)
	{
		n++;
	}
	return n;
}

void gesture_enable_irq(void)
{
	ioport_set_pin_dir(PE2,IOPORT_DIR_INPUT);
	pmic_init();
	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);
	cpu_irq_disable();
	PORTE.INTCTRL |= 0x01;
	PORTE.INT0MASK = (1<<2);
	PORTE.PIN2CTRL |= (1<<1);
	cpu_irq_enable();
}
void gesture_disable_irq(void)
{
	cpu_irq_disable();
	PORTE.INTCTRL &=~(0x04);
	cpu_irq_enable();
}

void gesture_set_on(void)
{
	delay_s(2);
	if(APDS9960_init())
	{
		printf("Inicializacion Correcta\n");
	}
	else
	{
		printf("Error en inicio\n");
	}
	delay_s(1);
	if(enableGestureSensor(TRUE))
	{
		printf("Sensor de gestos activado\n");
	}
	else
	{
		printf("fallo en inicio sensor de gestos\n");
	}
	delay_s(1);
	if(setGestureIntEnable(TRUE))
	{
		printf("Interrupcion activado\n");
	}
	else
	{
		printf("fallo en inicio interrupcion\n");
	}
	
	
	
	
}

int return_gesture(void)
{
	if(isGestureAvailable())
	{
		return readGesture();
	}
}

void print_gesture(void)
{
	if(isGestureAvailable())
	{
		switch(readGesture())
		{
			case DIR_UP:
			printf("UP \n");
			break;
			case DIR_DOWN:
			printf("DOWN \n");
			break;
			case DIR_LEFT:
			printf("LEFT \n");
			break;
			case DIR_RIGHT:
			printf("RIGHT \n");
			break;
			case DIR_NEAR:
			printf("NEAR \n");
			break;
			case DIR_FAR:
			printf("FAR \n");
			break;
			default:
			printf("NONE \n");
		}
	}
	
}