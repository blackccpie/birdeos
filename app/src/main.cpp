#include "darknetpp.h"
#include "eospp.h"

#include <iostream>

int main(int argc, char **argv) try
{
    darknetpp darkp;
    //bool bird = darkp.detect("data/bird.jpg");
    //std::cout << (bird ? "BIRD!!!" : "NOT A BIRD!!!") << std::endl;
    //return EXIT_SUCCESS;

    auto image_callback = [&](const std::uint8_t* p, int w, int h)
    {
        bool bird = darkp.detect(p, w, h);
        std::cout << (bird ? "BIRD!!!" : "NOT A BIRD!!!") << std::endl;

        static int bird_count = 0;
        if(bird)
        {
            CImg<std::uint8_t> bird_image(p,static_cast<std::uint32_t>(w),static_cast<std::uint32_t>(h),1,3,false);
            bird_image.save( ("birds/bird" + std::to_string(++bird_count) + ".jpg").c_str() );
        }

    };

    eospp eos;
    eos.init();
    eos.detect_camera();
    eos.register_image_callback( image_callback );
    eos.start_live();
    eos.resume_live();

    return EXIT_SUCCESS;
}
catch(const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
