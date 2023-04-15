#include "eospp.h"
#include "gui.h"

#include <iostream>
#include <jpeglib.h>
#include <jerror.h>

#define cimg_plugin <plugins/jpeg_buffer.h>
#include <CImg.h>

using namespace cimg_library;

std::mutex eospp::m_mutex;
std::condition_variable eospp::m_output_device_condition;
bool eospp::m_output_device_set = false;

EdsError eospp::handle_object_event(EdsObjectEvent event, EdsBaseRef object, EdsVoid* context)
{
    std::cout << "object changed event!" << std::endl;

    // Object must be released
	if(object)
	{
		EdsRelease(object);
	}    

	return EDS_ERR_OK;
}

EdsError eospp::handle_property_event(EdsPropertyEvent event, EdsPropertyID property, EdsUInt32 inParam, EdsVoid* context)
{
	if (event == kEdsPropertyEvent_PropertyChanged)
	{
        //std::cout << "property event!" << std::endl;

		if (property == kEdsPropID_Evf_OutputDevice)
		{
            std::cout << "output device event!" << std::endl;

            {
                std::unique_lock<std::mutex> lock( m_mutex );
                m_output_device_set = true;
            }

			m_output_device_condition.notify_all();
		}
	}
	else if (event == kEdsPropertyEvent_PropertyDescChanged)
	{
        // TODO
	}
	return EDS_ERR_OK;
}

EdsError eospp::handle_state_event(EdsStateEvent event, EdsUInt32 parameter, EdsVoid* context)
{
	// TODO
	return EDS_ERR_OK;
}

eospp::~eospp()
{
    //resume_live();
}

bool eospp::init()
{
    // Initialize SDK
	EdsError err = EdsInitializeSDK();
	if (err == EDS_ERR_OK)
    {
        std::cout << "EDSK loaded successfully" << std::endl;
    }
    else
    {
        std::cerr << "cannot load EDSDK, error: " << err << std::endl;
        return false;
    }

    return true;
}

bool eospp::detect_camera()
{
    EdsCameraListRef list;
    EdsError err = EdsGetCameraList(&list);

    if(err != EDS_ERR_OK)
    {
        std::cout << "cannot list cameras, error: " << err << std::endl;
        return false;
    }

    EdsUInt32 count = 0;
    err = EdsGetChildCount( list, &count );

    std::cout << "detected #" << count << " cameras" << std::endl;

    // get first camera retrieved
	err = EdsGetChildAtIndex(list, 0, &m_camera);

    bool status = true;

    if(err != EDS_ERR_OK)
    {
        std::cout << "cannot retrieve first camera, error: " << err << std::endl;
        status = false;
    }

    EdsRelease(list);

    return status;
}

bool eospp::start_live()
{
    // set event handler
	EdsError err = EdsSetObjectEventHandler(m_camera, kEdsObjectEvent_All, eospp::handle_object_event, nullptr);

	// set event handler
	err = EdsSetPropertyEventHandler(m_camera, kEdsPropertyEvent_All, eospp::handle_property_event, nullptr);

	// set event handler
	err = EdsSetCameraStateEventHandler(m_camera, kEdsStateEvent_All, eospp::handle_state_event, nullptr);

    // open session with camera
	err = EdsOpenSession(m_camera);

    if(err != EDS_ERR_OK)
    {
        std::cout << "cannot open camera session, error: " << err << std::endl;
        return false;
    }

    // start live view
    EdsUInt32 device = kEdsEvfOutputDevice_PC;
	err = EdsSetPropertyData(m_camera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);

    if(err != EDS_ERR_OK)
    {
        std::cout << "cannot set live view's output device, error: " << err << std::endl;
        return false;
    }

    // wait for notification
    std::unique_lock<std::mutex> lock( m_mutex );
    if(!m_output_device_set)
    {
        m_output_device_condition.wait(lock);
    }

    // create memory stream
	err = EdsCreateMemoryStream(0, &m_stream);

    if(err != EDS_ERR_OK)
    {
        std::cout << "cannot create memory stream, error: " << err << std::endl;
        return false;
    }

	// create EvfImageRef
	err = EdsCreateEvfImageRef(m_stream, &m_image);

    if(err != EDS_ERR_OK)
    {
        std::cout << "cannot create image, error: " << err << std::endl;
        return false;
    }

    gui g;

    for(int i=0; i<1000; ++i)
    {
        // download live view image data.
        err = EdsDownloadEvfImage(m_camera, m_image);

        if(err != EDS_ERR_OK)
        {
            std::cout << "cannot download image, error: " << err << std::endl;
            //return false;
        }
        else
        {
            std::cout << "successfully downloaded image" << std::endl;

            EdsVoid* ptr;
            EdsUInt64 stream_len = 0;
            err = EdsGetPointer(m_stream, &ptr);
            err = EdsGetLength(m_stream, &stream_len);

            if(m_image_callback)
            {
                CImg<std::uint8_t> image;
                image.load_jpeg_buffer(reinterpret_cast<std::uint8_t*>(ptr), stream_len);
                m_image_callback( image.data(), image.width(), image.height() );

                g.display( image, 1 );
            }
        }
    }

    return true;
}

bool eospp::resume_live()
{
    EdsRelease(m_stream);
	EdsRelease(m_image);

    EdsUInt32 device;
	EdsError err = EdsGetPropertyData(m_camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	// PC live view ends if the PC is disconnected from the live view image output device.
	if (err == EDS_ERR_OK)
	{
		device &= ~kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(m_camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	}

    return true;
}