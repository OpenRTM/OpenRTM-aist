#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <rtm/ByteDataStreamBase.h>
#include <CameraCommonInterfaceSkel.h>
template class __declspec(dllexport) coil::GlobalFactory < ::RTC::ByteDataStream<Img::TimedCameraImage> >;
template class __declspec(dllexport) coil::GlobalFactory < ::RTC::ByteDataStream<Img::TimedMultiCameraImage> >;
#endif
