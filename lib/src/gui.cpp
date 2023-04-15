#include "gui.h"

#ifdef WIN32
    #define cimg_display 2 //Windows-GDI
#else
    #define cimg_display 1 //X11
#endif

#include <cstdio>
#include <jpeglib.h>
#include <jerror.h>

#define cimg_plugin <plugins/jpeg_buffer.h>
#include <CImg.h>

using namespace cimg_library;

gui::gui() : m_display( std::make_unique<CImgDisplay>(640,480,"eospp") )
{}

gui::~gui()
{}

void gui::display( const CImg<std::uint8_t>& image, int scale)
{
    m_display->resize(scale*image.width(),scale*image.height());
    m_display->display(image);
    m_display->wait(1);
}

void gui::display( const std::uint8_t* img, int w, int h, int scale )
{
    // http://www.cimg.eu/reference/storage.html
    // https://www.codefull.org/2014/11/cimg-does-not-store-pixels-in-the-interleaved-format/
    CImg<std::uint8_t> image(img,3,static_cast<std::uint32_t>(w),static_cast<std::uint32_t>(h),1,false);
    image.permute_axes("yzcx");
    m_display->resize(scale*w,scale*h);
    m_display->display(image);
    m_display->wait(1);
}

void gui::display_jpeg( const std::uint8_t* jpeg_buffer, size_t size, int scale )
{
    CImg<std::uint8_t> image;
    image.load_jpeg_buffer(jpeg_buffer, size);
    m_display->resize(scale*image.width(),scale*image.height());
    m_display->display(image);
    m_display->wait(1);
}