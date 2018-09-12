#ifndef SIS3150USBCCI2_H
#define SIS3150USBCCI2_H

#include <stdio.h>	//for FILE ptr
#include <math.h>
#include "SISDefines.h"
#include "sis3150usb_vme_calls.h"

/**********************************************/
/**********************************************/

    int loadConfiguration( char* configFilename, char* outputFilename, 
                           int saveData );
    int connectToDAQ();
    int initialize(); //{cout << "initialize inline" << endl; return 0;}
    int startDataAcquisition();
    int stopDataAcquisition();
    int acquireData( struct SISdata **data, unsigned int *len, int saveData );

    //Added for writing binary data to file - RWB
    int WriteTS_EventsToDataFile(ULONG* memory_data_array, unsigned int nof_write_length_lwords);
    int WriteBufferHeaderCounterNofChannelToDataFile(unsigned int buffer_no, unsigned int nof_events, unsigned int event_length);
    int OpenEventDataFile( char* filename  );
    int CloseEventDataFile( void );

    void* getusbptr();


    void InitLibraries(void);
    int  ReadConfiguration (char *path_file);
    int  Configuration_SIS3820Clock (void);
    int  Configuration_SIS3302ADC (unsigned int module_addr, unsigned int module_conf_index );
    int  sis3302_write_dac_offset(unsigned int module_addr, unsigned int *offset_value_array);
    int sub_sis3302_fastTriggerOut_setup(unsigned int module_addr, unsigned int module_conf_index);
    int  DefineRunEnergyFactor (void);
    int WriteParseBuffer (unsigned int* memory_data_array, unsigned int nof_write_length_lwords,struct SISdata **data, unsigned int *len);
    int  CopyModuleAddressesToTable (void);
    int  copy_sis3302_sample_energy_CommonParameters_to_inbox (void);
    int  calculate_energy_trigger_windows_and_parameters (void);
    int  Get_ADC_sample_clock_for_DecayExpCalculation(unsigned int* sample_clock);
    int  TauFactor_to_DecayExpCalculation(void);
    int sisVME_ErrorHandling (unsigned int prot_error, unsigned int vme_addr, const char *err_messages);
    int write_system_messages (char *system_messages, int write_datetime);
    int sis_AnyErrorHandling (char *err_messages);
//    typedef unsigned char           u_int8_t;
//    typedef unsigned short          u_int16_t;
    
    // DAC Offset
      unsigned int  gl_uint_DacOffset_Conf[MAX_NO_OF_MODULES] [NO_OF_ADC_CHANNELS]     ;

    // FIR Trigger Parameters
      unsigned int  gl_uint_FirTriggerP_Conf[MAX_NO_OF_MODULES] [NO_OF_ADC_CHANNELS]     ;
      unsigned int  gl_uint_FirTriggerG_Conf[MAX_NO_OF_MODULES] [NO_OF_ADC_CHANNELS]     ;
      int  gl_int_FirTriggerThresholdValue_Conf[MAX_NO_OF_MODULES] [NO_OF_ADC_CHANNELS]     ;
      unsigned int  gl_uint_FirTriggerThreshold_GtLtFlag_Conf[MAX_NO_OF_MODULES] [NO_OF_ADC_CHANNELS]     ;
      unsigned int  gl_uint_FirTriggerPulseLength_Conf[MAX_NO_OF_MODULES] [NO_OF_ADC_CHANNELS]     ;

    // ADC Signal Invert Flag
      unsigned int  gl_uint_ADC_SignalInvertConf[MAX_NO_OF_MODULES][NO_OF_ADC_CHANNELS]     ;

    // ADC Gain Factor
      unsigned int  gl_uint_ADC_Next_NeighborConf[MAX_NO_OF_MODULES][NO_OF_ADC_CHANNELS]     ;

    // Energy Factor
      float  gl_float_EnergyFactorConf[MAX_NO_OF_MODULES][ NO_OF_ADC_CHANNELS]    ;
      float  gl_float_EnergyFactorRun[MAX_NO_OF_MODULES][ NO_OF_ADC_CHANNELS]    ;

    // Decay Factor
      unsigned int  gl_uint_DecayTauConf[MAX_NO_OF_MODULES][NO_OF_ADC_CHANNELS]     ;


    // Common Sample and Energy Parameters
      char gl_char_FileName_sis3150_9300_TSLoaderFile[MAX_FILENAME_LENGTH];
      unsigned int  gl_uint_FirEnergyGap     ;
      unsigned int  gl_uint_FirEnergyLength     ;
      unsigned int  gl_uint_FirDecimationMode     ;
      unsigned int  gl_uint_FirMwShapingLength     ;
      unsigned int  gl_uint_EnergyGateLength     ;
      unsigned int  gl_uint_RawSampleStart_Index     ;
      unsigned int  gl_uint_RawSampleLength     ;
      unsigned int  gl_uint_EnergySampleMode     ;
      unsigned int  gl_uint_EnergySampleStart_Index1     ;
      unsigned int  gl_uint_EnergySampleStart_Index2     ;
      unsigned int  gl_uint_EnergySampleStart_Index3     ;
      unsigned int  gl_uint_EnergySampleLength     ;
      unsigned int  gl_uint_PreTriggerDelay     ;
      unsigned int  gl_uint_TriggerGateLength     ;
      
      int sub_vme_A32MBLT64_read (unsigned int vme_adr, unsigned long* dma_buffer, unsigned int request_nof_words, unsigned long* got_nof_words);



int sub_vme_A32D32_read (unsigned int vme_adr, unsigned int* vme_data);


int sub_vme_A32D32_write (unsigned int vme_adr, unsigned int vme_data);

    // Library functions (sis3150usb_vme_win.dll - windows) or (libsis3150.so - linux)
    /*
      sis3150Usb_Register_Single_Read_PROT                sis3150usb_Register_Single_Read;
      FindAll_SIS3150USB_Devices_PROT                     FindAll_sis3150usb_Devices;
      Sis3150usb_OpenDriver_And_Download_FX2_Setup_PROT   sis3150usb_OpenDriver_And_Download_FX2_Setup;
      sis3150Usb_Register_Single_Write_PROT               sis3150usb_Register_Single_Write;
      sis3150Usb_Vme_Dma_Read_PROT                        sis3150usb_Vme_Dma_Read;   // used for sub_vme_A32MBLT64_read
      sis3150Usb_Vme_Single_Read_PROT                     sis3150usb_Vme_Single_Read;
      sis3150Usb_Vme_Single_Write_PROT                    sis3150usb_Vme_Single_Write;
*/


    ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////Unchangeable//////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
      HANDLE  gl_USBDevice;                // used for keeping track of the device - important
      struct SIS3150USB_Device_Struct gl_sis3150_device_information_struct[MAX_USB_DEV_NUMBER];       // used throughout program

      char gl_char_FileNameData[MAX_FILENAME_LENGTH];
      char gl_char_FileNameLoadersis3150usb_TS[MAX_FILENAME_LENGTH];


    ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////Unchangeable - END////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // configuration globals
      unsigned int gl_uint_system_status ;
      unsigned int gl_uint_ModEnableConf[MAX_NO_OF_MODULES]   ;
      unsigned int gl_uint_ModAddrConf[MAX_NO_OF_MODULES]		;
      unsigned int gl_uint_SIS3302_FourChannelFlag   ;

    // Clock Module
      unsigned int gl_uint_SIS3820EnableConf   ;
      unsigned int gl_uint_SIS3820AddrConf	;

    // Common Sample and Energy Parameters
      unsigned int gl_uint_SIS3820ClockModeConf	;
      unsigned int gl_uint_SIS3820ClockCtrl4FlagConf	;
      unsigned int gl_uint_SIS3820ClockCtrl3FlagConf	;
      unsigned int gl_uint_SIS3820ClockCtrl2FlagConf	;
      unsigned int gl_uint_SIS3820ClockTriggerMaskConf	;
      unsigned int  gl_uint_SIS3302ClockModeConf     ;

    // Common SIS3150
      unsigned int  gl_uint_SIS3302_Trigger_Mode_Source_ModeConf     ;
      unsigned int  gl_uint_SIS3302External_Lemo_InOutput_Conf     ;
      unsigned int  gl_uint_SIS3302_Reserved1_ModeConf     ;
      unsigned int  gl_uint_SIS3302_Reserved2_ModeConf     ;

    // Instance variables:

    /***********************************************************/
    /***********************************************************/
    /********************Instance Variables*********************/

    // Added by Jeremy to reconcile the separation of the acquisition function into separate parts
    unsigned int event_length_lwords;
    unsigned int max_events;
    unsigned int  adc_buffer_reg_offset[8] ;
    unsigned int  energy_max_index ;
    unsigned int  previous_bank_sample_address_reg_offset[8] ;
    unsigned int bank1_armed_flag;
    unsigned int buffer_switch_counter;
    unsigned int nof_raw_data_lwords;
    unsigned int raw_data_index;
    //////////////////////////////////////////////////////////////////////////

    //run
      unsigned int  gl_uint_RunStopStatusFlag     ;
      unsigned int  gl_uint_RunReadoutMode     ;


      unsigned int  gl_uint_RunCheckWrDataToFileFlag     ;
      unsigned int  gl_uint_RunCheckStopEventsFlag     ;       // flag for whether we will stop after a certain # of events
      unsigned int  gl_uint_RunEventCounter        ;
      unsigned int  gl_uint_RunEventOldCounter     ;
      unsigned int  gl_unit_RunOneSecondCounter    ;


    // run globals
      unsigned int gl_uint_ModConfIndexRun[MAX_NO_OF_MODULES]  ;
      unsigned int gl_uint_ModAddrRun[MAX_NO_OF_MODULES]  ;
      unsigned int gl_uint_NoOfModulesRun ;
      unsigned int gl_uint_SIS3302_BroadcastAddrConf ;

    // Display Diagnostic
      unsigned int  gl_uint_CountOfPileupsTriggerCountBasedArray[16]  ;

    // data event file
      FILE* gl_FILE_DataEvenFilePointer;
      unsigned int  gl_uint_DataEvent_OpenFlag	;
      unsigned int  gl_uint_DataEvent_FileCounter ;
      unsigned int  gl_uint_DataEvent_RunFile_NOF_Energy ;
      unsigned int  gl_uint_DataEvent_LWordCounter ;

    // MCA Mode upgrading
    // offline


    // offline

    // Macro for IS_64_BIT is found in GRIDefines.h
    #if IS_64_BIT
      unsigned long gl_dma_rd_buffer[0x800000] ;
    #else
      unsigned long gl_dma_rd_buffer[0x1000000] ;
    #endif

      unsigned int  gl_uint_end_sample_address[NO_OF_ADC_CHANNELS]  ;

    /*******************Instance Variables END******************/
    /***********************************************************/
    /***********************************************************/


#endif // sis3150usb_H
