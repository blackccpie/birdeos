#ifndef DARKNETPP_H
#define DARKNETPP_H

#include <darknet.h>

#include <cstdint>

#define cimg_use_jpeg
#include <CImg.h>

#include <string>

using namespace cimg_library;

class darknetpp
{
public:
    bool detect( const std::uint8_t* img, int w, int h )
    {
        image im = make_image(w,h,3);

        // http://www.cimg.eu/reference/storage.html
        // https://www.codefull.org/2014/11/cimg-does-not-store-pixels-in-the-interleaved-format/
        CImg<std::uint8_t> img_interleaved(img,static_cast<std::uint32_t>(w),static_cast<std::uint32_t>(h),1,3,false);
        img_interleaved.permute_axes("cxyz");
        copy_image_from_bytes( im, (char*)img_interleaved.data() );
        
        //copy_image_from_bytes( im, (char*)img );
        
        int expected = test_detector_image("cfg/coco.data", "cfg/yolov3-tiny.cfg", "data/yolov3-tiny.weights", im, 0.05f, 0.5, 1, 1, 14); // 14 = bird
        free_image(im);

        return expected == 1;
    }

    bool detect( const std::string& filename )
    {
        CImg<std::uint8_t> img(filename.c_str());
        image im = make_image(img.width(),img.height(),3);
        img.permute_axes("cxyz");
        copy_image_from_bytes( im, (char*)img.data() );
        
        int expected = test_detector_image("cfg/coco.data", "cfg/yolov3-tiny.cfg", "data/yolov3-tiny.weights", im, 0.05f, 0.5, 1, 1, 14); // 14 = bird
        free_image(im);

        return expected == 1;
    }
};

#endif