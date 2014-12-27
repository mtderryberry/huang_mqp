//----------------------------------------------------------------
//      _____
//     /     \
//    /____   \____
//   / \===\   \==/
//  /___\===\___\/  AVNET
//       \======/
//        \====/
//---------------------------------------------------------------
//
// This design is the property of Avnet.  Publication of this
// design is not authorized without written consent from Avnet.
//
// Please direct any questions to:  technical.support@avnet.com
//
// Disclaimer:
//    Avnet, Inc. makes no warranty for the use of this code or design.
//    This code is provided  "As Is". Avnet, Inc assumes no responsibility for
//    any errors, which may appear in this code, nor does it make a commitment
//    to update the information contained herein. Avnet, Inc specifically
//    disclaims any implied warranties of fitness for a particular purpose.
//                     Copyright(c) 2012 Avnet, Inc.
//                             All rights reserved.
//
//----------------------------------------------------------------
//
// Create Date:         Oct 24, 2012
// Design Name:         FMC-IMAGEON HDMI Video Frame Buffer
// Module Name:         fmc_imageon_hdmi_framebuffer.c
// Project Name:        FMC-IMAGEON HDMI Video Frame Buffer Program
// Target Devices:      Spartan-6, Virtex-6, Kintex-6
// Hardware Boards:     FMC-IMAGEON
// 
//
// Tool versions:       ISE 14.3
//
// Description:         FMC-IMAGEON HDMI Demo Program
//                      This application will configure the FMC-IMAGEON module
//                      for the simple case of an HDMI passthrough:
//                      - HDMI Input
//                         - ADV7611 configured for 16 bit YCbCr 4:2:2 mode
//                           with embedded syncs
//                      - HDMI Output
//                         - ADV7511 configured for 16 bit YCbCr 4:2:2 mode
//                           with embedded syncs
//                      If present, it will also reset the Video Timing Controller
//                      that is used for timing in the AXI4-Stream bridges
//
// Dependencies:
//
// Revision:            Oct 24, 2012: 1.00 Initial version
//                      Nov 01, 2012: 1.02 Update EDID content
//                      Jan 10, 2013: 1.03 Add SSC on video output clock
//                                         for CE compliance
//
//----------------------------------------------------------------

#include <stdio.h>

#include "fmc_imageon_hdmi_framebuffer.h"

#define offset 0x01000000
//unsigned int findCenter(unsigned int current_pixel);
#define TEST2 1
//#define HISTTEST 1
#define hist0_v 50
#define hist1_v 130
#define hist2_v 5
#define hist3_v 5



//void reset_everything();
void check_vertical_center_point(unsigned int found_center,  unsigned int extra, Xuint8 *filter,
								 unsigned int i, unsigned char cbcr, unsigned char luma);

Xuint8 fmc_imageon_hdmii_edid_content[256] =
{
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
		0x06, 0xD4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x16, 0x01, 0x03, 0x81, 0x46, 0x27, 0x78,
		0x0A, 0x32, 0x30, 0xA1, 0x54, 0x52, 0x9E, 0x26,
		0x0A, 0x49, 0x4B, 0xA3, 0x08, 0x00, 0x81, 0xC0,
		0x81, 0x00, 0x81, 0x0F, 0x81, 0x40, 0x81, 0x80,
		0x95, 0x00, 0xB3, 0x00, 0x01, 0x01, 0x02, 0x3A,
		0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
		0x45, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E,
		0xA9, 0x1A, 0x00, 0xA0, 0x50, 0x00, 0x16, 0x30,
		0x30, 0x20, 0x37, 0x00, 0xC4, 0x8E, 0x21, 0x00,
		0x00, 0x1A, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x46,
		0x4D, 0x43, 0x2D, 0x49, 0x4D, 0x41, 0x47, 0x45,
		0x4F, 0x4E, 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD,
		0x00, 0x38, 0x4B, 0x20, 0x44, 0x11, 0x00, 0x0A,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x54,
		0x02, 0x03, 0x1F, 0x71, 0x4B, 0x90, 0x03, 0x04,
		0x05, 0x12, 0x13, 0x14, 0x1F, 0x20, 0x07, 0x16,
		0x26, 0x15, 0x07, 0x50, 0x09, 0x07, 0x01, 0x67,
		0x03, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x1E, 0x01,
		0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E,
		0x28, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00,
		0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16,
		0x20, 0x58, 0x2C, 0x25, 0x00, 0xC4, 0x8E, 0x21,
		0x00, 0x00, 0x9E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20,
		0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xC4,
		0x8E, 0x21, 0x00, 0x00, 0x18, 0x01, 0x1D, 0x80,
		0x3E, 0x73, 0x38, 0x2D, 0x40, 0x7E, 0x2C, 0x45,
		0x80, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x1A,
		0x36, 0x80, 0xA0, 0x70, 0x38, 0x1F, 0x40, 0x30,
		0x20, 0x25, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00,
		0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

int fmc_imageon_hdmi_framebuffer_init( fmc_imageon_hdmi_framebuffer_t *pDemo )
{
   int ret;
   Xuint32 timeout = 100;
   Xuint32 iterations = 0;

   xil_printf("\n\r");
   xil_printf("------------------------------------------------------\n\r");
   xil_printf("--       FMC-IMAGEON HDMI Video Frame Buffer        --\n\r");
   xil_printf("------------------------------------------------------\n\r");
   xil_printf("\n\r");

   xil_printf( "FMC-IMAGEON Initialization ...\n\r" );

   ret = fmc_iic_xps_init(&(pDemo->fmc_imageon_iic),"FMC-IMAGEON I2C Controller", pDemo->uBaseAddr_IIC_FmcImageon );
   if ( !ret )
   {
      xil_printf( "ERROR : Failed to open FMC-IIC driver\n\r" );
      exit(0);
   }

   fmc_imageon_init(&(pDemo->fmc_imageon), "FMC-IMAGEON", &(pDemo->fmc_imageon_iic));
   pDemo->fmc_imageon.bVerbose = pDemo->bVerbose;

   // Configure Video Clock Synthesizer
   fmc_imageon_vclk_init( &(pDemo->fmc_imageon) );
   fmc_imageon_vclk_config( &(pDemo->fmc_imageon), FMC_IMAGEON_VCLK_FREQ_148_500_000);

   // Initialize HDMI Input (including EDID content)
   xil_printf( "HDMI Input Initialization ...\n\r" );
   ret = fmc_imageon_hdmii_init( &(pDemo->fmc_imageon),
                                 1, // hdmiiEnable = 1
                                 1, // editInit = 1
                                 fmc_imageon_hdmii_edid_content
                                 );
   if ( !ret )
   {
      xil_printf( "ERROR : Failed to init HDMI Input Interface\n\r" );
      exit(0);
   }

   // Configure Video Clock Synthesizer
   xil_printf( "Video Clock Synthesizer Configuration ...\n\r" );
   fmc_imageon_vclk_config( &(pDemo->fmc_imageon), FMC_IMAGEON_VCLK_FREQ_148_500_000);
   sleep(1);

#if 0
   xil_printf( "Enabling spread-spectrum clocking (SSC)\n\r" );
   xil_printf( "\ttype=down-spread, amount=-0.75%%\n\r" );
   {
	   Xuint8 num_bytes;
	   int i;
	   Xuint8 iic_cdce913_ssc_on[3][2]=
	   {
	      0x10, 0x6D, // SSC = 011 (0.75%)
	      0x11, 0xB6, //
	      0x12, 0xDB  //
	   };
       fmc_imageon_iic_mux( &(pDemo->fmc_imageon), FMC_IMAGEON_I2C_SELECT_VID_CLK );
       for ( i = 0; i < 3; i++ )
       {
           num_bytes = pDemo->fmc_imageon.pIIC->fpIicWrite( pDemo->fmc_imageon.pIIC, FMC_IMAGEON_VID_CLK_ADDR,
       		(0x80 | iic_cdce913_ssc_on[i][0]), &(iic_cdce913_ssc_on[i][1]), 1);
       }
   }
#endif

   // Set HDMI output to 1080P60 resolution
   pDemo->hdmio_resolution = VIDEO_RESOLUTION_1080P;
   pDemo->hdmio_width  = 1920;
   pDemo->hdmio_height = 1080;
/*
 *
   { "720P",   720,    5,    5,   20,    1, 1280,  110,   40,  220,    1 }, // VIDEO_RESOLUTION_720P
   { "1080P", 1080,    4,    5,   36,    1, 1920,   88,   44,  148,    1 }, // VIDEO_RESOLUTION_1080P
 * */



   //pDemo->hdmio_timing.IsHDMI        = 1; // HDMI Mode
   pDemo->hdmio_timing.IsHDMI        = 0; // DVI Mode
   pDemo->hdmio_timing.IsEncrypted   = 0;
   pDemo->hdmio_timing.IsInterlaced  = 0;
   pDemo->hdmio_timing.ColorDepth    = 8;

   pDemo->hdmio_timing.HActiveVideo  = 1920;
   pDemo->hdmio_timing.HFrontPorch   =   88;
   pDemo->hdmio_timing.HSyncWidth    =   44;
   pDemo->hdmio_timing.HSyncPolarity =    1;
   pDemo->hdmio_timing.HBackPorch    =  148;/*
   pDemo->hdmio_timing.HFrontPorch   =   110;
   pDemo->hdmio_timing.HSyncWidth    =   40;
   pDemo->hdmio_timing.HSyncPolarity =    1;
   pDemo->hdmio_timing.HBackPorch    =  220;*/

   pDemo->hdmio_timing.VActiveVideo  = 1080;/*
   pDemo->hdmio_timing.VFrontPorch   =    5;
   pDemo->hdmio_timing.VSyncWidth    =    5;
   pDemo->hdmio_timing.VSyncPolarity =    1;
   pDemo->hdmio_timing.VBackPorch    =   20;*/
   pDemo->hdmio_timing.VFrontPorch   =    4;
   pDemo->hdmio_timing.VSyncWidth    =    5;
   pDemo->hdmio_timing.VSyncPolarity =    1;
   pDemo->hdmio_timing.VBackPorch    =   36;

   xil_printf( "HDMI Output Initialization ...\n\r" );
   ret = fmc_imageon_hdmio_init( &(pDemo->fmc_imageon),
 	                             1,                      // hdmioEnable = 1
 	                             &(pDemo->hdmio_timing), // pTiming
 	                             0                       // waitHPD = 0
 	                             );
   if ( !ret )
   {
      xil_printf( "ERROR : Failed to init HDMI Output Interface\n\r" );
      //exit(0);
   }

   // Clear frame stores
   Xuint32 i;
   Xuint32 storage_size = pDemo->uNumFrames_HdmiFrameBuffer * ((1920*1080)<<1);
   volatile Xuint8 *pStorageMem = (Xuint8 *)pDemo->uBaseAddr_MEM_HdmiFrameBuffer;
   for ( i = 0; i < storage_size; i += 2 )
   {
      // Black Pixel
      *pStorageMem++ = 0x80; // CbCr (chroma)
      *pStorageMem++ = 0x00; // Y (luma)
   }


   volatile Xuint8 *filter = (Xuint8 *)(pDemo->uBaseAddr_MEM_HdmiFrameBuffer+offset);
      for ( i = 0; i < storage_size; i += 2 )
      {
         // Black Pixel
         *filter++ = 0x80; // CbCr (chroma)
         *filter++ = 0x00; // Y (luma)
      }

   // Initialize Output Side of AXI VDMA
   xil_printf( "Video DMA (Output Side) Initialization ...\n\r" );
   vfb_common_init(
      pDemo->uDeviceId_VDMA_HdmiFrameBuffer, // uDeviceId
      &(pDemo->vdma_hdmi)                    // pAxiVdma
      );
   vfb_tx_init(
      &(pDemo->vdma_hdmi),                   // pAxiVdma
      &(pDemo->vdmacfg_hdmi_read),           // pReadCfg
      pDemo->hdmio_resolution,               // uVideoResolution
      pDemo->hdmio_resolution,               // uStorageResolution
      (pDemo->uBaseAddr_MEM_HdmiFrameBuffer+offset),  // uMemAddr
      pDemo->uNumFrames_HdmiFrameBuffer      // uNumFrames
      );

   // Configure VTC on output data path
   xil_printf( "Video Timing Controller (generator) Initialization ...\n\r" );
   vgen_init( &(pDemo->vtc_hdmio_generator), pDemo->uDeviceId_VTC_HdmioGenerator );
   vgen_config( &(pDemo->vtc_hdmio_generator), pDemo->hdmio_resolution, 1 );

  while (1)
  {
   if ( iterations > 0 )
   {
      xil_printf( "\n\rPress ENTER to re-start ...\n\r" );
      getchar();
   }
   iterations++;

   xil_printf( "Waiting for ADV7611 to locked on incoming video ...\n\r" );
   pDemo->hdmii_locked = 0;
   timeout = 100;
   while ( !(pDemo->hdmii_locked) && timeout-- )
   {
      usleep(100000); // wait 100msec ...
      pDemo->hdmii_locked = fmc_imageon_hdmii_get_lock( &(pDemo->fmc_imageon) );
   }
   if ( !(pDemo->hdmii_locked) )
   {
      xil_printf( "\tERROR : ADV7611 has NOT locked on incoming video, aborting !\n\r" );
      //return -1;
      continue;
   }
   xil_printf( "\tADV7611 Video Input LOCKED\n\r" );
   usleep(100000); // wait 100msec for timing to stabilize

   // Get Video Input information
   fmc_imageon_hdmii_get_timing( &(pDemo->fmc_imageon), &(pDemo->hdmii_timing) );
   pDemo->hdmii_width  = pDemo->hdmii_timing.HActiveVideo;
   pDemo->hdmii_height = pDemo->hdmii_timing.VActiveVideo;
   pDemo->hdmii_resolution = vres_detect( pDemo->hdmii_width, pDemo->hdmii_height );

   xil_printf( "ADV7611 Video Input Information\n\r" );
    xil_printf( "\tVideo Input      = %s", pDemo->hdmii_timing.IsHDMI ? "HDMI" : "DVI" );
   xil_printf( "%s", pDemo->hdmii_timing.IsEncrypted ? ", HDCP Encrypted" : "" );
   xil_printf( ", %s\n\r", pDemo->hdmii_timing.IsInterlaced ? "Interlaced" : "Progressive" );
   xil_printf( "\tColor Depth      = %d bits per channel\n\r", pDemo->hdmii_timing.ColorDepth );
   xil_printf( "\tHSYNC Timing     = hav=%04d, hfp=%02d, hsw=%02d(hsp=%d), hbp=%03d\n\r",
      pDemo->hdmii_timing.HActiveVideo,
      pDemo->hdmii_timing.HFrontPorch,
      pDemo->hdmii_timing.HSyncWidth, pDemo->hdmii_timing.HSyncPolarity,
      pDemo->hdmii_timing.HBackPorch
      );
   xil_printf( "\tVSYNC Timing     = vav=%04d, vfp=%02d, vsw=%02d(vsp=%d), vbp=%03d\n\r",
      pDemo->hdmii_timing.VActiveVideo,
      pDemo->hdmii_timing.VFrontPorch,
      pDemo->hdmii_timing.VSyncWidth, pDemo->hdmii_timing.VSyncPolarity,
      pDemo->hdmii_timing.VBackPorch
      );
   xil_printf( "\tVideo Dimensions = %d x %d\n\r", pDemo->hdmii_width, pDemo->hdmii_height );

   if ( (pDemo->hdmii_resolution) == -1 )
   {
      xil_printf( "\tERROR : Invalid resolution, aborting !\n\r" );
      //return -1;
      continue;
   }

   // Reset VTC on input data path
   vdet_init( &(pDemo->vtc_hdmii_detector), pDemo->uDeviceId_VTC_HdmiiDetector );
   vdet_reset( &(pDemo->vtc_hdmii_detector) );

   xil_printf( "Video DMA (Input Side) Initialization ...\n\r" );

   // Stop Input Side of AXI_VDMA (from previous iteration)
   vfb_rx_stop(
      &(pDemo->vdma_hdmi)                    // pAxiVdma
      );

   // Clear frame stores
   Xuint32 i;
   Xuint32 storage_size = pDemo->uNumFrames_HdmiFrameBuffer * ((1920*1080)<<1);
   volatile Xuint8 *pStorageMem = (Xuint8 *)pDemo->uBaseAddr_MEM_HdmiFrameBuffer;
   for ( i = 0; i < storage_size; i += 2 )
   {
      // Black Pixel
      *(pStorageMem+2*i+1) = 0x80; // CbCr (chroma)
      *(pStorageMem+2*i+2) = 0x00; // Y (luma)
   }

   volatile Xuint8 *filter = (Xuint8 *)(pDemo->uBaseAddr_MEM_HdmiFrameBuffer+offset);
      for ( i = 0; i < storage_size; i += 2 )
      {
         // Black Pixel
         *(filter+2*i+1) = 0x80; // CbCr (chroma)
         *(filter+2*i+2) = 0x00; // Y (luma)
      }



   // Initialize Input Side of AXI VDMA
   vfb_rx_init(
      &(pDemo->vdma_hdmi),                   // pAxiVdma
      &(pDemo->vdmacfg_hdmi_write),          // pWriteCfg
      pDemo->hdmii_resolution,               // uVideoResolution
      pDemo->hdmio_resolution,               // uStorageResolution
      pDemo->uBaseAddr_MEM_HdmiFrameBuffer,  // uMemAddr
      pDemo->uNumFrames_HdmiFrameBuffer      // uNumFrames
      );

   xil_printf( "HDMI Output Re-Initialization ...\n\r" );
   ret = fmc_imageon_hdmio_init( &(pDemo->fmc_imageon),
 	                             1,                      // hdmioEnable = 1
 	                             &(pDemo->hdmio_timing), // pTiming
 	                             0                       // waitHPD = 0
 	                             );
   if ( !ret )
   {
      xil_printf( "ERROR : Failed to init HDMI Output Interface\n\r" );
      //exit(0);
   }


#if 0 // Activate for debug
   sleep(1);
   // Status of AXI VDMA
   vfb_dump_registers( &(pDemo->vdma_hdmi) );
   if ( vfb_check_errors( &(pDemo->vdma_hdmi), 1/*clear errors, if any*/ ) )
   {
      vfb_dump_registers( &(pDemo->vdma_hdmi) );
   }
#endif

   //*(volatile int*) (0x7D800000) = 0;

   //-----------------------------------------------------------------------------------------------

   //put image processing algorithm below
   

   Xuint32 new_storage_size = storage_size/2;
   unsigned int display_size = storage_size;
   unsigned int leftside_x, rightside_x = 0;
   unsigned char entry_flag, reentry_flag, exit_flag = 0;

   unsigned long frame_count = 0;
   unsigned char cbcr = 0;
   unsigned char luma = 0;
   unsigned int pixel_count_total = 0;
   unsigned int x, y = 0;
   enum {white_to_white, white_to_grey, grey_to_grey, grey_to_white};
   unsigned char current_state, last_state = 0;

   unsigned int found_center, center_y_temp = 0;

   unsigned char stop_flag = 0;

   unsigned int found = 0;

#define TRUE 1
#define FALSE 0
   while(TRUE)
   {
	   stop_flag = 0;
	   pixel_count_total = 1;
	   //xil_printf("\nNumber Found: %d", found);
	   found = 0;
	   frame_count++;

	   memcpy(filter, pStorageMem, display_size/*new_storage_size*/);
	   //xil_printf("\nNEW FRAME: %d\n", frame_count);
	   //display_size = storage_size;
	   for(i = 0; i < new_storage_size; i += 2) {
		   cbcr = *(filter + i + 1);	// cbcr
		   luma = *(filter + i);	// y

		   //get coordinates of current pixel indexed from 1, 1
		   x = pixel_count_total % 1920;
		   y = (pixel_count_total / 1920) + 1;
		   pixel_count_total++;

		   switch (current_state) {
				   case (white_to_white):
				   	   	   if(luma == 255) {
				   	   		   current_state = white_to_white;
				   	   	   }
				   	   	   else {
				   	   		   //xil_printf("\nWhite to grey\n");
				   	   		   current_state = white_to_grey;
				   	   	   }
				   	   	   //reset if distance is too great
				   	   	   if(((x - leftside_x) > 100) && entry_flag) {
				   	   		   //reset_everything();
				   	   		   leftside_x = 0;
				   	   		   rightside_x = 0;
				   	   		   entry_flag = FALSE;
				   	   		   reentry_flag = FALSE;
				   	   		   exit_flag = FALSE;
				   	   	   }
				   	   	   last_state = white_to_white;
				   	   	   break;

				   case (white_to_grey) :
						   if(entry_flag) {
							   reentry_flag = TRUE;
							   entry_flag = FALSE;
						   }
				   	   	   if(reentry_flag) {
				   	   		   rightside_x = x;
				   	   		   found++;
				   	   		   found_center = (rightside_x-leftside_x)/2;
				   	   		   //xil_printf("\nCENTER FOUND: %d, %d\n Number Found: %d", ((rightside_x+leftside_x)/2), y, found);

				   	   		   if ((*(filter + i - 2*(found_center) + 1)) == 127) {	// check that vertical central point was found at this location previously
	   		  	  	  	  	  	  check_vertical_center_point(found_center, 0, filter, i, cbcr, luma);
				   	   		   }
				   	   		   else {
				   	   			   *(filter + i - 2*(found_center+1)+1) = 129;
				   	   		   }

				   	   		   if ((*(filter + i - 2*(found_center+1)+1)) == 127) {	// additional checks for odd right-left since c truncates
				   	   			   check_vertical_center_point(found_center+1, 0, filter, i, cbcr, luma);
				   	   		   }
				   	   		   else {
				   	   			   *(filter + i - 2*(found_center+1)+1) = 129;
				   	   		   }

				   	   			unsigned int center_y = 0;
				   	   			unsigned char up_counter = 0;
				   	   			unsigned int temp_color_up = 255;

				   	   			unsigned char down_counter = 0;
				   	   			unsigned char temp_color_down = 255;

				   	   			while(temp_color_up == 255 && up_counter < 100 && ((y - up_counter) > 0)) {	// need to add and not at the top of the image
				   	   				temp_color_up = *(filter + i - 2*(found_center)-(2*1920)*up_counter);
				   	   				up_counter++;
				   	   			}

				   	   			while(temp_color_down == 255 && down_counter < 100 && ((y + down_counter) < 541)) {	// need to add and not at the bottom of the image
				   	   				temp_color_down = *(filter + i - 2*(found_center)+(2*1920)*down_counter);
				   	   				down_counter++;
				   	   			}
				   	   			center_y_temp = up_counter+down_counter/2;
				   	   			if(up_counter > down_counter) {
				   	   				center_y = up_counter - center_y_temp;

				   	   				if (*(filter + i- 2*(found_center) - 2*(center_y*1920) + 1) == 129)
				   	   				{
				   	   					check_vertical_center_point(found_center, -1*(center_y*1920), filter, i, cbcr, luma);
				   	   				}

				   	   				else
				   	   					*(filter + i- 2*(found_center) - 2*(center_y*1920) + 1) = 127;
				   	   			}
				   	   			else {
				   	   				center_y = down_counter - center_y_temp;

				   	   				if (*(filter + i- 2*(found_center) - 2*(center_y*1920) + 1) == 129)
				   	   				{
				   	   					check_vertical_center_point(found_center, center_y*1920, filter, i, cbcr, luma);
				   	   				}

				   	   				else
				   	   					*(filter + i- 2*(found_center) - 2*(center_y*1920) + 1) = 127;
				   	   			}
				   	   		   //reset_everything();
				   	   			leftside_x = 0;
				   	   			rightside_x = 0;
				   	   			entry_flag = FALSE;
				   	   			reentry_flag = FALSE;
				   	   			exit_flag = FALSE;
				   	   	   }
						   else {
							   entry_flag = TRUE;
							   reentry_flag = FALSE;
						   }
				   	   	   if(luma == 255) {
				   	   		   //xil_printf("\nGrey to white\n");
				   	   		   current_state = grey_to_white;
				   	   	   }
				   	   	   else {
				   	   		   //xil_printf("\nGrey to grey\n");
				   	   		   current_state = grey_to_grey;
				   	   	   }
						   last_state = white_to_grey;
				   	   	   break;

				   case (grey_to_grey) :
						   if(luma != 255) {
							   current_state = grey_to_grey;
						   }
						   else {
				   	   		   //xil_printf("\nGrey to white\n");
							   current_state = grey_to_white;
						   }
						   last_state = grey_to_grey;
				   	   	   break;

				   case (grey_to_white) :
						   if(entry_flag && !reentry_flag) {
							   exit_flag = TRUE;
						   }
				   	   	   if(exit_flag) {
				   	   		   leftside_x = x;
				   	   	   }
				   	   	   //do everything else
			   	   		   //xil_printf("\nGrey to White\n");
				   	   	   current_state = white_to_white;
						   last_state = grey_to_white;
				   	   	   break;
				   default:
					   current_state = white_to_white;
					   break;
		   }
	   } // close for
	   if (stop_flag == 1){
	   for(i = 0; i < (1920*20); i += 2) {
		   *(filter + i + 1) = 255;	// cbcr
		   *(filter + i) = 90;	// y
	   }
	   }
   }

   //put image processing algorithm above
   //-----------------------------------------------------------------------------------------------
   xil_printf("\n\r");
   xil_printf( "Done\n\r" );
   xil_printf("\n\r");

   sleep(1);
  }

   return 0;
}

/*void reset_everything() {
	leftside_x = 0;
	rightside_x = 0;
	entry_flag = FALSE;
	reentry_flag = FALSE;
	exit_flag = FALSE;
}*/

void check_vertical_center_point(unsigned int found_center,  unsigned int extra, Xuint8 *filter, unsigned int i,
								 unsigned char cbcr, unsigned char luma) {
	*(filter + i - 2*(found_center) + 2*(extra)) = 0;

	signed int x_min, x_max, y_min, y_max = 0;
	unsigned long hist0;
	unsigned long hist1;
	unsigned long hist2;
	unsigned long hist3;
#ifdef TEST2
	// crude go out and draw a box 31 by 31 (picked semiarbitrarily for proof of concept)
	// for each pixel in side the range
	x_min = -15;
	x_max = 16;
	y_min = -15;
	y_max = 16;
	hist0 = 0;
	hist1 = 0;
	hist2 = 0;
	hist3 = 0;
	for(x_min = -15; x_min < x_max; x_min++) {
		for (y_min = 0; y_min < y_max; y_min++) {
			 //*(filter + i - 2*(found_center - x_min) - 2*(1920)*(y_min));	// pixel value at image(x,y)
			 cbcr = *(filter + i - 2*(found_center + x_min) - 2*(1920)*(y_min) + 1);	// cbcr
			 luma = *(filter + i - 2*(found_center + x_min) - 2*(1920)*(y_min));	// y
#ifndef HISTTEST
			 if (luma > 250) {	// sub test to check that we see the right region
				 *(filter + i - 2*(found_center + x_min) - 2*(1920)*(y_min) + 1) = 255;	// cbcr
				 *(filter + i - 2*(found_center + x_min) - 2*(1920)*(y_min)) = 0;	// y
			 }
#endif
#ifdef HISTTEST
			 if (luma < 100)
				 hist0++;
			 else if (luma < 150)
				 hist1++;
			 else if (luma < 200)
				 hist2++;
			 else if (luma < 250)
				 hist3++;
			 //xil_printf("\n\nHist0 = %d\nHist1 = %d\nHist2 = %d\nHist3 = %d\n",hist0,hist1,hist2,hist3);
			 // if statement to check histogram values to determine if its a red light
			 // do whatever when we know its a red light
			 if(hist0 < hist0_v && hist1 > hist1_v && hist2 < hist2_v && hist3 < hist3_v) {
				 //xil_printf("\nRED LIGHT\n");
				 stop_flag = 1;
				 //display_size = new_storage_size;
			 }
#endif
		}
	}
	//xil_printf("\n\nHist0 = %d\nHist1 = %d\nHist2 = %d\nHist3 = %d\n X: %d\n Y: %d\n",hist0,hist1,hist2,hist3,x,y);

#endif
	// center found call histogram function
}



