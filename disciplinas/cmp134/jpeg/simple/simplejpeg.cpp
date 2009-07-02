#include <stdio.h>
#include <jpeglib.h>

using namespace std;

int main() {
//   vector<char> buffer;
//   XInfo_t      xinfo  = getXInfo(":0");
//   XImage* screen_shot = takeScreenshot(xinfo, buffer);
   FILE* outfile = fopen("/tmp/test.jpeg", "wb");

   if (!outfile)
      throw FormattedException("%s:%d Failed to open output file", __FILE__, __LINE__);
   
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr       jerr;

   cinfo.err = jpeg_std_error(&jerr);
   jpeg_create_compress(&cinfo);
   jpeg_stdio_dest(&cinfo, outfile);

   cinfo.image_width      = xinfo.width;      
   cinfo.image_height     = xinfo.height;
   cinfo.input_components = 3;
   cinfo.in_color_space   = JCS_RGB; 
   jpeg_set_defaults(&cinfo);
   /*set the quality [0..100]  */
   jpeg_set_quality (&cinfo, 75, true);
   jpeg_start_compress(&cinfo, true);
   JSAMPROW row_pointer;          /* pointer to a single row */

   while (cinfo.next_scanline < cinfo.image_height) {
      row_pointer = (JSAMPROW) &buffer[cinfo.next_scanline*(screen_shot->depth>>3)*screen_shot->width];
      jpeg_write_scanlines(&cinfo, &row_pointer, 1);
   }
   
   jpeg_finish_compress(&cinfo);
   XCloseDisplay(xinfo.display);
}
