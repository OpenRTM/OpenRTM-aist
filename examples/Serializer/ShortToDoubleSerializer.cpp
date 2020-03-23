// -*- C++ -*-
/*!
 * @file  ShortToDoubleSerializer.cpp
 * @brief Convert double to short
 * $Date: 2019-06-25 07:33:08 $
 *
 * $Id$
 */

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/ByteDataStreamBase.h>
#include <rtm/CORBA_CdrMemoryStream.h>
#include <coil/Factory.h>
#include <rtm/Manager.h>

//以下はシリアライザの実装
class ShortToDoubleSerializer : public RTC::CORBA_CdrSerializer<RTC::TimedDouble>
{
public:
    ShortToDoubleSerializer() = default;

    bool deserialize(RTC::TimedDouble& data) override
    {
        RTC::TimedShort tmp_data;
        bool ret = m_cdr.deserializeCDR(tmp_data);
        data.tm.sec = tmp_data.tm.sec;
        data.tm.nsec = tmp_data.tm.nsec;
        data.data = static_cast<CORBA::Double>(tmp_data.data);
        return ret;
    }
private:
};

extern "C"
{
    //以下はモジュールロード時に呼び出される関数
    DLL_EXPORT void ShortToDoubleSerializerInit(RTC::Manager* /*manager*/)
    {
        //以下のファクトリはデータ型ごとに登録する必要がある
        RTC::addSerializer<RTC::TimedDouble, ShortToDoubleSerializer>("corba:RTC/TimedDouble:RTC/TimedShort");
        //addSerializer関数の第1引数で登録名を設定。以下で独自シリアライザを利用するときはこの名前を使用する。
    }
}
