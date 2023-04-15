#ifndef GUI_H
#define GUI_H

#include <cstdint>
#include <cstddef>
#include <memory>

namespace cimg_library{ class CImgDisplay; template <typename T> class CImg; }

class gui
{
public:
    gui();
    ~gui();
    void display( const cimg_library::CImg<std::uint8_t>& image, int scale);
    void display( const std::uint8_t* img, int w, int h, int scale );
    void display_jpeg( const std::uint8_t* jpeg_buffer, size_t size, int scale );
private:
    std::unique_ptr<cimg_library::CImgDisplay> m_display;
};

#endif
