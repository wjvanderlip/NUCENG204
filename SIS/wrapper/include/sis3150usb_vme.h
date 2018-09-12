/***************************************************************************/
/*  Filename: sis3150usb_vme .h                                         */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 10.05.2004                                       */
/*  last modification:    20.12.2005                                       */
/*  20.12.2005 :  add Sis3150usb_CloseDriver                               */
/*  14.04.2006 :  Linux version (R.Fox)                                    */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*  SIS  Struck Innovative Systeme GmbH                                    */
/*                                                                         */
/*  Harksheider Str. 102A                                                  */
/*  22399 Hamburg                                                          */
/*                                                                         */
/*  Tel. +49 (0)40 60 87 305 0                                             */
/*  Fax  +49 (0)40 60 87 305 20                                            */
/*                                                                         */
/*  http://www.struck.de                                                   */
/*                                                                         */
/*  © 2005                                                                 */
/*                                                                         */
/***************************************************************************/
#ifndef __SIS3150USB_VME_H
#define __SIS3150USB_VME_H	/* multiple include gaurd. */

#ifdef unix


#ifndef __USB_H__
#include <usb.h>		/* LIBUSB header. */
#endif

/*  Windows MSVC does not bother to provide stdint as required by the C89
    standard hence the #ifdefery below with hopes that it's correct on 
    64 bit windows.
*/

#ifdef __WINDOWS__
typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef unsigned long  unsigned long;
typedef long           LONG;
#else
#ifndef __CRT_STDINT_H
#include <stdint.h>
#ifndef __CRT_STDINT_H
#define __CRT_STDINT_H
#endif
#endif
typedef uint8_t  UCHAR;
typedef uint16_t USHORT;
typedef uint32_t unsigned long;
typedef int32_t  LONG;
#endif



/*   Typedefs to allow common prototypes with WIN */
/*    NOTE: Should define the 'sized' based ones in terms of e.g. uint32_t 
            for unsigned long that way this >might< port to 64 bits  
*/
typedef usb_dev_handle* HANDLE;
//typedef char          *PCHAR;
//typedef unsigned long unsigned long;
typedef unsigned int   UINT;
typedef int            BOOLEAN;	/* c has no bool type. */

#endif

#define USB_DEVICE_NAME_SIZE 128

struct SIS3150USB_Device_Struct {
    char	cDName[USB_DEVICE_NAME_SIZE];
    unsigned short	idVendor;
    unsigned short	idProduct;
    unsigned short	idSerNo;
    unsigned short	idFirmwareVersion;
#ifdef unix
    struct usb_device*  pDeviceStruct;
#endif
} ;

#ifdef __cplusplus
extern "C" {
#endif

// DLL support

int FindAll_SIS3150USB_Devices(struct SIS3150USB_Device_Struct* sis3150usb_Device, 
				      unsigned int*                    nof_usbdevices, 
				      unsigned int                     max_usb_device_Number) ;

int  Sis3150usb_OpenDriver (char* usbDeviceName, HANDLE *usbDeviceHandle );
int  Sis3150usb_OpenDriver_And_Download_FX2_Setup (char* usbDeviceName, HANDLE *usbDeviceHandle ) ;
int  Sis3150usb_CloseDriver(HANDLE usbDevice);




int  sis3150Usb_Register_Single_Read(HANDLE usbDevice, unsigned long addr, unsigned long* data) ;
int  sis3150Usb_Register_Dma_Read(HANDLE usbDevice, unsigned long addr, unsigned long* dmabufs,unsigned long req_nof_data, unsigned long* got_nof_data) ;

int  sis3150Usb_Register_Single_Write(HANDLE usbDevice, unsigned long addr, unsigned long data) ;
int  sis3150Usb_Register_Dma_Write(HANDLE usbDevice, unsigned long addr,unsigned long* dmabufs, unsigned long req_nof_data, unsigned long* put_nof_data) ;


int  sis3150Usb_TsBus_Single_Read(HANDLE usbDevice, unsigned long addr, unsigned long* data) ;
int  sis3150Usb_TsBus_Dma_Read(HANDLE usbDevice, unsigned long addr, unsigned long* dmabufs,unsigned long req_nof_data, unsigned long* got_nof_data) ;

int  sis3150Usb_TsBus_Single_Write(HANDLE usbDevice, unsigned long addr, unsigned long data) ;
int  sis3150Usb_TsBus_Dma_Write(HANDLE usbDevice, unsigned long addr,unsigned long* dmabufs, 
				      unsigned long req_nof_data, unsigned long* put_nof_data) ;


int  sis3150Usb_Vme_Single_Read(HANDLE usbDevice, unsigned long addr, unsigned long am, unsigned long size, unsigned long* data) ;
int  sis3150Usb_Vme_Dma_Read(HANDLE usbDevice, unsigned long addr, unsigned long am, 
				   unsigned long size, unsigned long fifo_mode,
				   unsigned long* dmabufs, unsigned long req_nof_data, 
				   unsigned long* got_nof_data) ;


int  sis3150Usb_Vme_Single_Write(HANDLE usbDevice, unsigned long addr, unsigned long am, unsigned long size, unsigned long data);

int  sis3150Usb_Vme_Dma_Write(HANDLE usbDevice, unsigned long addr, unsigned long am, 
				    unsigned long size, unsigned long fifo_mode,
				    unsigned long* dmabufs, unsigned long req_nof_data, 
				    unsigned long* put_nof_data);


int  sis3150Usb_VmeSysreset(HANDLE usbDevice) ;

#ifdef __cplusplus
}
#endif

#endif /* Multiple include gaurd. */
