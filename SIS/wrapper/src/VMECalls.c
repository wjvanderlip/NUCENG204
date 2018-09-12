#include <SIS3150.h>
#include "sis3150usb_vme_calls.h"

/*
  Author: Cameron Bates cameron.r.bates@gmail.com
  Source code based on Struck Innovative Systeme vme routines from gamma_system code
  This is a companion file to SIS3150CCI2. It contains all of the 3150 vme calls that are used.

  */
/************************************************************************************************/
/************************************************************************************************/
// ERROR HANDLING AND SYSTEM MESSAGES

int sisVME_ErrorHandling (unsigned int prot_error, unsigned int vme_addr, const char *err_messages)
{
    //std::cout << "ERROR: prot_error=" << prot_error << ", vme_addr=0x" << hex << vme_addr << dec << endl;
    //std::cout << "\t\tMessage: " << err_messages << endl;

    return 0;
} // end function sisVME_ErrorHandling ()


int write_system_messages (char *system_messages, int write_datetime)
{
    //std::cout << system_messages << endl;
    return 0;
} // end function sisVME_ErrorHandling ()


/* --------------------------------------------------------------------------
   VME Error Handling
-------------------------------------------------------------------------- */


int sis_AnyErrorHandling (char *err_messages)
{
    //std::cout << err_messages << endl;
    return 0;
} 


/********************************************************************************************************/
/***********PC_TO_VME_ROUTINES***************************************************************************/


int sub_vme_A32MBLT64_read (unsigned int vme_adr, unsigned long* dma_buffer, unsigned int request_nof_words, unsigned long* got_nof_words)
{
    return sis3150Usb_Vme_Dma_Read(gl_USBDevice, vme_adr, 0x8, 8, 0, dma_buffer,
                                    request_nof_words & 0xfffffffe, got_nof_words);
} 


int sub_vme_A32D32_read (unsigned int vme_adr, unsigned int* vme_data)
{
    return vme_A32D32_read(gl_USBDevice, vme_adr, (unsigned int*)vme_data)  ;
} 


int sub_vme_A32D32_write (unsigned int vme_adr, unsigned int vme_data)
{
    return vme_A32D32_write(gl_USBDevice, vme_adr, vme_data)  ;
} 


/*************PC TO VME ROUTINES END*********************************************************************/
/********************************************************************************************************/
