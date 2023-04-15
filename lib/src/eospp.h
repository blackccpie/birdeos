#ifndef EOSPP_H
#define EOSPP_H

#include <EDSDK.h>
#include <EDSDKTypes.h>
#include <EDSDKErrors.h>

#include <memory>
#include <mutex>

using t_image_callback = std::function<void(const std::uint8_t*, int, int)>;

class eospp
{
public:
    ~eospp();
    bool init();
    bool detect_camera();
    bool start_live();
    bool resume_live();

    void register_image_callback( t_image_callback image_callback)
    {
        m_image_callback = image_callback;
    }

private:
    static EdsError handle_object_event(EdsObjectEvent event, EdsBaseRef object, EdsVoid* context);
    static EdsError handle_property_event(EdsPropertyEvent event, EdsPropertyID property, EdsUInt32 inParam, EdsVoid* context);
    static EdsError handle_state_event(EdsStateEvent event, EdsUInt32 parameter, EdsVoid* context);

    static std::mutex m_mutex;
    static std::condition_variable m_output_device_condition;
    static bool m_output_device_set;

private:
    EdsCameraRef m_camera;
    EdsStreamRef m_stream;
    EdsEvfImageRef m_image;

    t_image_callback m_image_callback;
};

#endif