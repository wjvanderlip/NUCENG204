#include "SIS3150.h"
#include "sis3150usb_vme_calls.h"
#include "stdio.h"
#include <stdlib.h>

/*
Author: Cameron Bates cameron.r.bates@gmail.com
Source code based on Struck Innovative Systeme vme routines from gamma_system code

Source code for SIS3302 readout using SIS3150 modules.
Primary code with methods relevant to the framework is located in this file.
Methods specific to configuring the 3302's and communication to the
vme are located in SIS3302Configuration.cpp and VMECalls.cpp respectively


  */

int connectToDAQ(){
    //cout << "Initializing SIS3150 Libraries..." << endl;

      //  cout << "connectToDAQ" << endl;
        // originally static (global class variables)/////////////

        UCHAR    gl_cDName[128] ;    //DriverNames
        unsigned int gl_nof_usbdevices   ;
        unsigned int gl_usb_index   ;
        /////////////////////END//////////////////////////////////
        // initialize variables
        gl_USBDevice = NULL;
        //gl_hXDev = NULL; - DELETED

        int error=0;
        int return_code ;
        unsigned int i ;
        char messages_buffer[80] ;

        // -- USB VME Interface Setup
        FindAll_SIS3150USB_Devices(gl_sis3150_device_information_struct, &gl_nof_usbdevices, MAX_USB_DEV_NUMBER)  ;


        if (gl_nof_usbdevices == 0) {
            sprintf (messages_buffer, "No SIS3150 Device found\n");
            write_system_messages (messages_buffer, PRINT_DATE_YES)	   ;
            //exit(-1) ;
            gl_usb_index = 0 ;
            gl_USBDevice = NULL ;
        }


        else {
            printf("number of SIS3150 devices = %d \n\n",gl_nof_usbdevices);
            write_system_messages (messages_buffer, PRINT_DATE_YES)	   ;



            //sprintf (messages_buffer, "Device    Vendor   Product   Ser.No. \n");
            //write_system_messages (messages_buffer, PRINT_DATE_NO)	   ;
            for (i=0;i<gl_nof_usbdevices;i++) {
                printf("NameString: %s\n", gl_sis3150_device_information_struct[i].cDName);
                printf("idVendor:   %04X\n", gl_sis3150_device_information_struct[i].idVendor);
                printf("idProduct:  %04X\n", gl_sis3150_device_information_struct[i].idProduct);
                printf("idSerNo:    %d\n", gl_sis3150_device_information_struct[i].idSerNo);
                printf("WrapperRev: %d\n\n", gl_sis3150_device_information_struct[i].idFirmwareVersion);

                //write_system_messages (messages_buffer, PRINT_DATE_NO)	   ;
            }

            // use first device
            gl_usb_index = 1 ;


            //sprintf ((char*)gl_cDName, "%s", gl_sis3150_device_information_struct[gl_usb_index-1].cDName);
            int ret = Sis3150usb_OpenDriver_And_Download_FX2_Setup((char*)gl_sis3150_device_information_struct[0].cDName, &gl_USBDevice);
            //write_system_messages (messages_buffer, PRINT_DATE_NO)	   ;
            //return_code = SIS3150USB_OpenDriver_And_Download_FX2_Setup ((char*)gl_cDName, &gl_USBDevice ); // gl_cDName is a UCHAR
            //cout << ret << endl;
            if(ret != 0) {
                gl_USBDevice = NULL ;
                sprintf (messages_buffer, "%s\n", "ERROR Loading BIX File");
                write_system_messages (messages_buffer, PRINT_DATE_NO)	   ;
            }


            if ((error = sis3150Usb_Register_Single_Write(gl_USBDevice, SIS3150USB_VME_MASTER_CONTROL_STATUS, 0xe000 )) != 0) {
                sisVME_ErrorHandling (error, SIS3150USB_VME_MASTER_CONTROL_STATUS, "SIS3150USB_Register_Single_Write");
                return -1;
            }
        }
    
    gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY;
    return 0;
}



int loadConfiguration( char* configFilename, char* outputFilename, 
                       int saveData )
{
    int tempErr ;
    char gl_char_FileNameConf[MAX_FILENAME_LENGTH];
    tempErr = ReadConfiguration ( configFilename )  ;

    if (tempErr == FILE_ACCESS_OK) printf("Loaded config file successfully \n");
    if (tempErr != FILE_ACCESS_OK) {
        printf("Config Failure \n");
        return 1;
    }

    // Modules
    CopyModuleAddressesToTable();
    copy_sis3302_sample_energy_CommonParameters_to_inbox()  ;
    gl_uint_system_status =  SYSTEM_STATUS_MODULES_READY ;

    TauFactor_to_DecayExpCalculation()  ;
    
    //NEW - open file for binary dump	-RWB
    if ( saveData )
    {
      gl_uint_RunCheckWrDataToFileFlag = 1;	//Default = Write data
      if (gl_uint_RunCheckWrDataToFileFlag == 1)
      {
        OpenEventDataFile( outputFilename );
      }
    }
    return 0;
}


int initialize(){
    gl_uint_RunCheckWrDataToFileFlag = WRITE_DATA_TO_FILE; // can let the user choose this
    return 0;
}


int startDataAcquisition(){

    //cout << "startDataAcquisition" << endl;
    
    //gl_uint_FirEnergyLength = this->shapingtime;
    //gl_uint_FirEnergyGap = gap;
    //gl_uint_FirDecimationMode = decimation;

    unsigned int i;
    int run_index, error ;
    unsigned int addr, data;

    if(gl_uint_SIS3820EnableConf != 0) { // wird Configuration
        Configuration_SIS3820Clock();
    } // if enabled

    // 1. reset all modules and define
    run_index = 0 ;


    for (i=0;i<MAX_NO_OF_MODULES;i++) {
        if  (gl_uint_ModEnableConf[i] != 0) {  // enabled
            addr = gl_uint_ModAddrConf[i] + SIS3302_KEY_RESET  ;
            if ((error = vme_A32D32_write(gl_USBDevice,addr,0x0 )) != 0) {
                sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_write")   ;
                gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
                return -1 ;
            }
            gl_uint_ModConfIndexRun[run_index] = i ;
            gl_uint_ModAddrRun[run_index] = gl_uint_ModAddrConf[i] ;
            //sleep(1);
            addr = gl_uint_ModAddrConf[i] + SIS3302_MODID  ;
            //printf("%x \n",addr);
            //sub_vme_A32D32_read(gl_uint_ModAddrConf[i],&data );
            //cout << data << endl;

            if ((error = vme_A32D32_read(gl_USBDevice,addr,&data )) != 0) {
                sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_read")   ;
                gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
                //cout << data << endl;
                return -1 ;

            }




            run_index++;
        } // if enabled
    } /* for loop */


    gl_uint_NoOfModulesRun =  run_index ;


    // configure all defined SIS3302 modules
    if (gl_uint_NoOfModulesRun != 0) {
        for (i=0;i<gl_uint_NoOfModulesRun;i++) {
            Configuration_SIS3302ADC(gl_uint_ModAddrRun[i], gl_uint_ModConfIndexRun[i]) ;
        }
    }


    gl_uint_system_status = SYSTEM_STATUS_MODULES_RUNNING ;

    gl_uint_RunStopStatusFlag = 1 ;

    //gl_uint_SIS3302_BroadcastAddrConf = ADC_MODULE_CONF_DEFAULT_BROADCAST_ADDRESS;

    // Clear Timestamp via KEY Address
    addr = gl_uint_SIS3302_BroadcastAddrConf + SIS3302_KEY_TIMESTAMP_CLEAR ;

    if ((error = vme_A32D32_write(gl_USBDevice,addr,0 )) != 0) {
        sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_write")   ;
        gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
        //cout << gl_uint_system_status << endl;
        return -1 ;
    }

    // Clear Timestamp
    if (gl_uint_SIS3820EnableConf == 1)  {
        if ((error = vme_A32D32_write(gl_USBDevice,gl_uint_SIS3820AddrConf + SIS3820CLOCK_KEY_CLR_TIMESTAMP,0x0 )) != 0) {
            sisVME_ErrorHandling (error, gl_uint_SIS3820AddrConf + SIS3820CLOCK_KEY_CLR_TIMESTAMP, "sub_vme_A32D32_write");
            return -1;
        }
    }
    //cout << addr << endl;

    // end
    unsigned int  address_threshold_value ;
    unsigned int module_index;
    unsigned int temp2;
    int temp;//, data_rd ;  Declared as below to avoid segfaults.

    unsigned int  nof_energy_values ;

    //	signed int  sign_integral ;
    unsigned int  find_from_index1 ;
    unsigned int  find_from_index2;

    char file_gl_messages_buffer[128];

    find_from_index1 = DEFAULT_FIND_FROM_INDEX1; // replaced by DEFAULT_FIND_FROM_INDEX1 in code
    find_from_index2 = DEFAULT_FIND_FROM_INDEX2; // replaced by DEFAULT_FIND_FROM_INDEX2

    adc_buffer_reg_offset[0] = SIS3302_ADC1_OFFSET  ;
    adc_buffer_reg_offset[1] = SIS3302_ADC2_OFFSET  ;
    adc_buffer_reg_offset[2] = SIS3302_ADC3_OFFSET  ;
    adc_buffer_reg_offset[3] = SIS3302_ADC4_OFFSET  ;
    adc_buffer_reg_offset[4] = SIS3302_ADC5_OFFSET  ;
    adc_buffer_reg_offset[5] = SIS3302_ADC6_OFFSET  ;
    adc_buffer_reg_offset[6] = SIS3302_ADC7_OFFSET  ;
    adc_buffer_reg_offset[7] = SIS3302_ADC8_OFFSET  ;

    for (i=0;i<16;i++) {
        gl_uint_CountOfPileupsTriggerCountBasedArray[i] = 0;
    }

    nof_raw_data_lwords = (gl_uint_RawSampleLength>>1) ; //
    raw_data_index  = 2 ;

    nof_energy_values = 0 ; //
    switch(gl_uint_EnergySampleMode) {
    case 0: //  Energy Trapezoidal 510 values + MAX and MIN
        nof_energy_values = 510  ;
        break;
    case 1: // no energy
        nof_energy_values = 0 ; // Z: Default Configuration
        break;
    case 2: // 3 parts of Energy Trapezoidal (3x170=510 values) + min/max
        nof_energy_values = 3*gl_uint_EnergySampleLength ; //
        break;

    case 3: // programmable ,  // only valid if only one start address is definded
        nof_energy_values = gl_uint_EnergySampleLength ; //
        break;
    } // end switch
    if (nof_energy_values > 510) {
        sprintf (file_gl_messages_buffer, "number of energy values is too high (max = 510) ; actual = %d \n\n",nof_energy_values);
        sis_AnyErrorHandling (file_gl_messages_buffer)	   ;
        gl_uint_RunStopStatusFlag = AQC_STOP_STATUS ;
        return -1 ;
    }


    energy_max_index  = 2 + nof_energy_values + nof_raw_data_lwords ;

    event_length_lwords	 = 2 + 4  ; // Timestamp/Header, MAX, MIN, Trigger-FLags, Trailer
    event_length_lwords	 = event_length_lwords + nof_raw_data_lwords  ; //
    event_length_lwords	 = event_length_lwords + nof_energy_values  ; //


    // calculation of max events to use only one 8MByte Sample Buffer (only one page)
    // 8 MByte = 0x800000 Bytes = 0x400000 samples
    max_events = 0x200000 / (event_length_lwords) ;  // max events to fill 4 Mbyte (half page !)

    //check if max flag is enabled

    //GetCtrlVal (Panels[RUN_CTRL_MENUE],RUN_CTRL_CHECK_NOF_EVENT_MANUA,&temp);
    temp = DEFAULT_MAX_EVENTS_ENABLED_FLAG; // enable user input later

    if (temp == 1) {

        //GetCtrlVal (Panels[RUN_CTRL_MENUE],RUN_CTRL_RUN_EVENT_MAX_NUMBER2, &temp2);
        temp2 = 1; //DEFAULT_MAX_EVENTS; // enable user input later

        if ( max_events > temp2) {
            max_events = temp2 * 2;  //

        }
    }


    address_threshold_value = (max_events * event_length_lwords) ; // valid if halfful !!

    // address threshold flag
    data = address_threshold_value ;		//
    for (module_index=0;module_index<gl_uint_NoOfModulesRun;module_index++) {

        addr = gl_uint_ModAddrRun[module_index] + SIS3302_END_ADDRESS_THRESHOLD_ALL_ADC ;
        if ((error = vme_A32D32_write(gl_USBDevice,addr,data )) != 0) {
            sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_write")   ;
            gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
            return -1 ;
        }
    }


    // Start Readout Loop  */
    addr = gl_uint_SIS3302_BroadcastAddrConf + SIS3302_KEY_DISARM_AND_ARM_BANK1 ; 	   // start all modules at the same time
    if ((error = vme_A32D32_write(gl_USBDevice,addr,0x0 )) != 0) {
        sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_write")   ;
        gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
        return -1 ;
    }


    previous_bank_sample_address_reg_offset[0] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC1  ;
    previous_bank_sample_address_reg_offset[1] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC2  ;
    previous_bank_sample_address_reg_offset[2] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC3  ;
    previous_bank_sample_address_reg_offset[3] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC4  ;
    previous_bank_sample_address_reg_offset[4] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC5  ;
    previous_bank_sample_address_reg_offset[5] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC6  ;
    previous_bank_sample_address_reg_offset[6] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC7  ;
    previous_bank_sample_address_reg_offset[7] = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC8  ;

    bank1_armed_flag  = 1;
    buffer_switch_counter  = 0;

    // disable stop buttom / enable start button (neccessary if stoped with MaxEventNumber
    gl_uint_RunStopStatusFlag = 0 ;
    gl_uint_system_status = SYSTEM_STATUS_MODULES_CONFIGURED ;

    //cout << "Multi-event-finished" << endl;
    //sleep(5);
    return 0;
}

int stopDataAcquisition(){
    int error;
    unsigned int addr = gl_uint_ModAddrRun[0] + SIS3302_KEY_DISARM ;
    if ((error = vme_A32D32_write(gl_USBDevice,addr,0x0 )) != 0) {
        sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_write")   ;
        gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
        return -1 ;
    }
    
    //Close data file
//    if (gl_uint_RunCheckWrDataToFileFlag == 1)
//    {
//      CloseEventDataFile();
//    }

    return 0;
}


int acquireData( struct SISdata **outdata,unsigned int *len, int saveData )
{
    //cout << "aquireData" << endl;
    unsigned int  dma_request_no_of_words; // don't know whether local or instance
    // LOCAL VARIABLES ////////////////////////////
    unsigned int module_index, data;
    int channel_index ;
    int nof_channel ;
    int error;
    unsigned int addr;
    unsigned int data_rd;
    unsigned int dma_got_no_of_words ;

    unsigned int  uint_max_event_use , uint_max_event_saved ;
    ///////////////////////////////////////////////

        // poll now on ADDRESS Threshold valid

        // wait for address threshold flag (IRQ 6) . Poll on an internal Flag in the SIS3150 module
        addr = SIS3150USB_VME_INTERRUPT_STATUS ;
        do {
            if ((error = sis3150Usb_Register_Single_Read(gl_USBDevice, addr, &data_rd )) != 0) {
                sisVME_ErrorHandling (error, addr, "SIS3150USB_Register_Single_Read");
                return -1;
            }

        } while ((data_rd & 0xFE) == 0x00000)  ;   // loop until IRQ




        if (bank1_armed_flag == 1) {
            addr = gl_uint_SIS3302_BroadcastAddrConf + SIS3302_KEY_DISARM_AND_ARM_BANK2 ;
            bank1_armed_flag = 0; // bank 2 is armed
        }
        else {
            addr = gl_uint_SIS3302_BroadcastAddrConf + SIS3302_KEY_DISARM_AND_ARM_BANK1 ;
            bank1_armed_flag = 1; // bank 1 is armed
        }


        if ((error = vme_A32D32_write(gl_USBDevice,addr,0x0 )) != 0) {
            sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_write")   ;
            gl_uint_system_status = SYSTEM_STATUS_MODULES_NOT_READY ;
            return -1 ;
        }



        // loop over modules
        for (module_index=0;module_index<gl_uint_NoOfModulesRun;module_index++) {


            // set Buffer page
            data = 0x0 ; //Bank2 is armed and Bank1 (page 0) has to be readout
            if (bank1_armed_flag == 1) { // Bank1 is armed and Bank2 (page 4) has to be readout
                data = 0x4 ;
            }
            addr = gl_uint_ModAddrRun[module_index] + SIS3302_ADC_MEMORY_PAGE_REGISTER ;
            if ((error = vme_A32D32_write(gl_USBDevice,addr,data )) != 0) {
                sisVME_ErrorHandling (error, gl_uint_ModAddrRun[module_index], "sub_vme_A32D32_write");
                return -1;
            }

            nof_channel = 4 ;
            gl_uint_end_sample_address[0] = 0 ;
            gl_uint_end_sample_address[1] = 0 ;
            gl_uint_end_sample_address[2] = 0 ;
            gl_uint_end_sample_address[3] = 0 ;
            gl_uint_end_sample_address[4] = 0 ;
            gl_uint_end_sample_address[5] = 0 ;
            gl_uint_end_sample_address[6] = 0 ;
            gl_uint_end_sample_address[7] = 0 ;
            // if not four_channel
            if(gl_uint_SIS3302_FourChannelFlag == 0x0) { //
                nof_channel = 8 ;
            }


            int poll_counter = 0 ;
            addr = gl_uint_ModAddrRun[module_index] + SIS3302_ACQUISITION_CONTROL ;
            int poll_loop_valid = 1;
            do {
                    if ((error = vme_A32D32_read(gl_USBDevice,addr,&data_rd )) != 0) {
                            sisVME_ErrorHandling (error, gl_uint_ModAddrRun[module_index], "sub_vme_A32D32_read");
                    }
                    poll_counter++;
                    if (bank1_armed_flag == 1) { // Bank1 is armed and Bank2 (page 4) has to be readout
                            if ((data_rd & 0x10000) == 0x10000)  { poll_loop_valid = 0; } // is swapped
                    }
                    else {
                            if ((data_rd & 0x20000) == 0x20000)  { poll_loop_valid = 0; } // is swapped
                    }

                    if (poll_counter > 500) { // max length = eventlength (raw samples * 10ns) + energy Length max= 100us
                            poll_loop_valid = 0;
                    }
            } while (poll_loop_valid==1)  ;




            for (channel_index=0;channel_index<nof_channel;channel_index++) {
                // read stop sample address

                addr = gl_uint_ModAddrRun[module_index] + previous_bank_sample_address_reg_offset[channel_index] ;
                //msleep(100);
                if ((error = vme_A32D32_read(gl_USBDevice,addr,&gl_uint_end_sample_address[channel_index] )) != 0) {
                    sisVME_ErrorHandling (error, addr, "sub_vme_A32D32_read");
                    return -1;
                }


                // check if bank address flag is valid
                if ( ((gl_uint_end_sample_address[channel_index] >> 24) & 0x1) != bank1_armed_flag) {   //
                    sisVME_ErrorHandling (gl_uint_end_sample_address[channel_index], gl_uint_end_sample_address[channel_index], "bank address flag ")   ;
                    // in this case -> poll right arm flag or implement a delay
                }

                // check buffer address
                gl_uint_end_sample_address[channel_index] = gl_uint_end_sample_address[channel_index] & 0xffffff ; // mask bank2 address bit (bit 24)

                if (gl_uint_end_sample_address[channel_index] > 0x3fffff) {   // more than 1 page memory buffer is used
                    uint_max_event_saved =  gl_uint_end_sample_address[channel_index] / event_length_lwords ;

                    gl_uint_end_sample_address[channel_index] = 2 * ((max_events-1) * event_length_lwords) ; // max 8Mbyte (inside one page)
                    uint_max_event_use =  gl_uint_end_sample_address[channel_index] / event_length_lwords ;
                }


                // readout

                if (gl_uint_end_sample_address[channel_index]  >= 0x400000) {
                    dma_request_no_of_words = 0x200000 ;
                }
                else {
                    dma_request_no_of_words = (gl_uint_end_sample_address[channel_index] & 0x3ffffc)>>1 ;
                }

                if (gl_uint_end_sample_address[channel_index] != 0) {
                    addr = gl_uint_ModAddrRun[module_index] + adc_buffer_reg_offset[channel_index]  ;
                    error = vme_A32MBLT64_read(gl_USBDevice,(uint32)addr, &gl_dma_rd_buffer[0],  (gl_uint_end_sample_address[channel_index] & 0x3ffffc)>>1, &dma_got_no_of_words) ;
                    if (error != 0x0) { // vme error
                        sisVME_ErrorHandling (error, addr, "sub_vme_A32MBLT64_read")   ;
                    }
                }
                // temp =  gl_uint_end_sample_address[channel_index]  ;


                // write to File
                if ( saveData ){ gl_uint_DataEvent_OpenFlag = 1; }
                else { gl_uint_DataEvent_OpenFlag = 0; }
                //assert(gl_uint_end_sample_address[0] != 0);
                if (gl_uint_end_sample_address[channel_index] != 0) {
                    //From original Struck software - for writing data to file - RWB
                    if (gl_uint_DataEvent_OpenFlag == 1) 
                    {
//                        #ifdef STD_DEBUG
//                            fprintf(gl_stdDebugFilePointer, "Writing to data file\n");
//                        #endif
                        WriteBufferHeaderCounterNofChannelToDataFile (buffer_switch_counter, 0x0 /*nof_events*/, dma_got_no_of_words /*event_length_lwords*/) ;
                        WriteTS_EventsToDataFile (gl_dma_rd_buffer, dma_got_no_of_words);
                    }



                    WriteParseBuffer(gl_dma_rd_buffer, dma_got_no_of_words,outdata,len);  // Parse and post data
                }

            }   // channel_index


        }   //   module_index

        buffer_switch_counter++ ;
        return 0;
}


/************************************************************************************************/
/************************************************************************************************/




int WriteParseBuffer (unsigned int* memory_data_array, unsigned int nof_write_length_lwords,struct SISdata **data, unsigned int *len)
{
    long long timestamp;
    int energy;
    int maxEnergy;
    int firstEnergy;
    unsigned short *raw_data_ptr ;
    unsigned int Cl = 6 + floor(gl_uint_RawSampleLength/2 + 0.5) + gl_uint_DataEvent_RunFile_NOF_Energy;
    unsigned int NumRec = nof_write_length_lwords/Cl;
    //qint64 * qTS = new qint64[NumRec];
    //SISdata *unpackeddata = new SISdata[NumRec];
    int oldlen = *len;
    *len += NumRec;
    if (oldlen == 0) {
        *data = malloc(NumRec*sizeof(struct SISdata));
    }
    else {
        *data = realloc(*data,(*len)*sizeof(struct SISdata));
    }
    //printf("%d events\n",NumRec); 
    unsigned int cntr = 0;

//    printf( "gl_uint_RawSampleLength = %d\n", gl_uint_RawSampleLength );
    for(unsigned int i = 0; i <NumRec; i++){
        unsigned int ADCID = memory_data_array[cntr] & 65535;
        unsigned int Mod = ADCID >> 11;//
        unsigned int ID = (((Mod-1) << 3) + (ADCID & 7) ) -56 ;

        timestamp = ((long long)(memory_data_array[cntr] & 0xffff0000) << 16) + (long long)memory_data_array[cntr+1];

        cntr = cntr + 2;
        if (gl_uint_RawSampleLength != 0)
        {
            raw_data_ptr = (unsigned short*) &(memory_data_array[cntr]);	//Changed to "&" - RWB
//            for (int k=0;k<gl_uint_RawSampleLength;k++){
//                //data[i].rawdata[k] = raw_data_ptr[k];
//            }
         }

        cntr = cntr + gl_uint_RawSampleLength/2 + gl_uint_DataEvent_RunFile_NOF_Energy;
        maxEnergy = memory_data_array[cntr++];
        firstEnergy = memory_data_array[cntr++];
        int trigger = (memory_data_array[cntr] >> 24 ) & 7;	//RWB
        int nneighborm1 = (memory_data_array[cntr] >> 28) & 1;
        int nneighborp1 = (memory_data_array[cntr]>> 29) & 1;
        int pileup = (memory_data_array[cntr] >> 30) & 1 ;
        //unsigned int trailer = memory_data_array[cntr+1]; this reads the trailer which should be 3735928559 in decimal
//        energy = maxEnergy - firstEnergy;
        energy = maxEnergy;					//RWB
        cntr += 2;
        //printf("energy: %u adcid: %u \n",energy,ID);
        int n = i +oldlen;
        (*data)[n].adcid = ID;
        (*data)[n].energy = energy;
        (*data)[n].timestamp = timestamp;
        (*data)[n].trigger = trigger;				//RWB
        (*data)[n].neighborminus = nneighborm1;
        (*data)[n].neighborplus = nneighborp1;
        (*data)[n].pileup = pileup;        
        // Raw Data  - RWB
        int k;
        for( k=0; k < gl_uint_RawSampleLength; k++ )
        {
          (*data)[n].rawdata[k] = raw_data_ptr[k];
        }
    }
    return 0;
}


/************************************************************************************************************/
// FUNCTIONS ADDED FOR WRITING BINARY DATA TO FILE (FROM ORIGINAL SIS SOFTWARE) - RWB
int WriteBufferHeaderCounterNofChannelToDataFile (unsigned int buffer_no,unsigned int nof_events, unsigned int event_length)
{
    int written ;
    int data ;
    //header
    data = FILE_FORMAT_EVENT_HEADER ;
    written=fwrite(&data,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;
    //event counter
    written=fwrite(&buffer_no,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;
    //nof channels
    written=fwrite(&nof_events,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;
    //event length
    written=fwrite(&event_length,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;


    return 0;

}

int WriteTS_EventsToDataFile (ULONG* memory_data_array, unsigned int nof_write_length_lwords)
{
    int nof_write_elements ;
    int written ;
    char messages_buffer[256] ;

    // gl_uint_DataEvent_RunFile_EvenSize : length

    nof_write_elements = nof_write_length_lwords ;
    written=fwrite(memory_data_array,0x4,nof_write_elements,gl_FILE_DataEvenFilePointer); // write 3 uint value
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written  ;
    if(nof_write_elements != written) {
        sprintf (messages_buffer, "Data File Write Error in  WriteEventToDataFile()  \n");
        write_system_messages (messages_buffer, PRINT_DATE_YES)    ;
    }

    return 0;

}

int OpenEventDataFile (char* filename)
{
    int written ;
    unsigned int dataFormat ;
    unsigned int temp ;
//    SetDefaultDataFileName();
    // Made up default filename:
//    char *filename = "BINARY_DATA_OUT.dat";
//    gl_FILE_DataEvenFilePointer=fopen(gl_char_FileNameData,"wb");
    gl_FILE_DataEvenFilePointer=fopen(filename,"wb");

    // File Number
    gl_uint_DataEvent_FileCounter = 0x1 ;
    written=fwrite(&gl_uint_DataEvent_FileCounter,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = written ;

    // Data Format Mode
    dataFormat = 0x1406 ;        // changed 18.03.2009
    written=fwrite(&dataFormat,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // Number of modules
    written=fwrite(&gl_uint_NoOfModulesRun,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;


    // sampling parameters (clock)
    temp =   (gl_uint_SIS3820ClockModeConf << 16) +  (gl_uint_SIS3302ClockModeConf) ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // Raw data
    temp =   (gl_uint_RawSampleStart_Index << 16) +  (gl_uint_RawSampleLength) ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;


    // changed !!!!
    // MWD parameters
    temp =     ((gl_uint_FirDecimationMode << 28) & 0xf0000000)            //
               +  ((gl_uint_FirEnergyGap  & 0x0fff) << 12)         //
               +  (gl_uint_FirEnergyLength & 0x0fff);;     //

    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;


    // nof of Energy value
    gl_uint_DataEvent_RunFile_NOF_Energy = 2 ; // dummy
//    gl_uint_OfflineEnergyModeLength =  gl_uint_EnergySampleMode ;
    switch(gl_uint_EnergySampleMode) {
    case 0: //  Energy Trapezoidal 510 values
        gl_uint_DataEvent_RunFile_NOF_Energy = 510 ;   //   ;
        break;
    case 1: // no energy
        gl_uint_DataEvent_RunFile_NOF_Energy = 0 ;  //
        break;

    case 2: // 3 parts of Energy Trapezoidal (3x170=510 values) + min/max
        gl_uint_DataEvent_RunFile_NOF_Energy = 3*gl_uint_EnergySampleLength ; //
        break;

    case 3: // programmable ,  // only valid if only one start address is definded
        gl_uint_DataEvent_RunFile_NOF_Energy = 0 ; // MAX and MIN
        if (gl_uint_EnergySampleStart_Index1 != 0) {gl_uint_DataEvent_RunFile_NOF_Energy = gl_uint_DataEvent_RunFile_NOF_Energy + gl_uint_EnergySampleLength ; }
        if (gl_uint_EnergySampleStart_Index2 != 0) {gl_uint_DataEvent_RunFile_NOF_Energy = gl_uint_DataEvent_RunFile_NOF_Energy + gl_uint_EnergySampleLength ; }
        if (gl_uint_EnergySampleStart_Index3 != 0) {gl_uint_DataEvent_RunFile_NOF_Energy = gl_uint_DataEvent_RunFile_NOF_Energy + gl_uint_EnergySampleLength ; }
        break;

    } // end switch


    temp = gl_uint_EnergySampleMode;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;


    //           OFFLI_CTRL_FILE_READOUT_MODE
    written=fwrite(&gl_uint_RunReadoutMode,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;


    // begin new 19.01.2008
    // Energy Sample Mode 2: Addr2 and Addr1
    temp =   (gl_uint_EnergySampleStart_Index2 << 16) +  (gl_uint_EnergySampleStart_Index1) ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // Energy Sample Mode 2: Addr2 and Addr1
    temp =   (gl_uint_DataEvent_RunFile_NOF_Energy << 16) +  (gl_uint_EnergySampleStart_Index3) ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // gl_uint_EnergyGateLength
    temp =   gl_uint_EnergyGateLength ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // gl_uint_TriggerGateLength
    temp =   gl_uint_TriggerGateLength ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // gl_uint_PreTriggerDelay
    temp =   gl_uint_PreTriggerDelay ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // reserved 1
    temp =   0 ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // reserved 2
    temp =   0 ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // reserved 3
    temp =   0 ;
    written=fwrite(&temp,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;

    // end new 26.01.2006


    gl_uint_DataEvent_OpenFlag    = 0   ; // Open

    return 0;

}


// DONT FORGET TO ADD FILE-CLOSING TO STOPACQUISITION
int CloseEventDataFile (void)
{
    int written ;
    int data ;

    data = FILE_FORMAT_EOF_TRAILER ;
    written=fwrite(&data,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written ;
    fclose(gl_FILE_DataEvenFilePointer);
    gl_uint_DataEvent_OpenFlag    = 0   ; // Close
    return 0;
}

