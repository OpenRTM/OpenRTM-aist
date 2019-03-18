// -*- C++ -*-
/*!
 * @file  ROS2Serializer.h
 * @brief ROS2 Serializer 
 * @date  $Date: 2019-02-04 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_ROS2SERIALIZER_H
#define RTC_ROS2SERIALIZER_H

#include <rtm/ByteDataStreamBase.h>
#include <coil/Factory.h>
#include <rtm/Manager.h>
#include <rmw_fastrtps_cpp/TypeSupport.hpp>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>
#include <std_msgs/msg/float32__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/float64__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int16__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int32__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int64__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int8__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/string__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int16__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int32__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int64__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int8__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/float32_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/float64_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int16_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int32_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int64_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/int8_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int16_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int32_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int64_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/u_int8_multi_array__rosidl_typesupport_fastrtps_cpp.hpp>
#include <std_msgs/msg/string__rosidl_typesupport_fastrtps_cpp.hpp>
#include <geometry_msgs/msg/point_stamped__rosidl_typesupport_fastrtps_cpp.hpp>
#include <geometry_msgs/msg/quaternion_stamped__rosidl_typesupport_fastrtps_cpp.hpp>
#include <geometry_msgs/msg/vector3_stamped__rosidl_typesupport_fastrtps_cpp.hpp>
#include <sensor_msgs/msg/image__rosidl_typesupport_fastrtps_cpp.hpp>


namespace RTC
{
  /*!
   * @if jp
   *
   * @class ROS2SerializerBase
   *
   * @brief ROS2シリアライザ基底クラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2SerializerBase
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <class DataType>
  class ROS2SerializerBase : public ByteDataStream<DataType>
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    ROS2SerializerBase(){
      m_message.reserve(65000);
    };

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    virtual ~ROS2SerializerBase(){};

    /*!
     * @if jp
     *
     * @brief 内部のバッファにデータを書き込む
     * 
     * @param buffer 書き込み元のバッファ
     * @param length データの長さ
     * 
     *
     * @else
     *
     * @brief 
     * 
     * @param buffer 
     * @param length 
     * 
     *
     * @endif
     */
    virtual void writeData(const unsigned char* buffer, unsigned long length)
    {
      m_message.reserve(length);
      m_message.length = length;
      m_message.max_size = length;
      m_message.data = new eprosima::fastrtps::rtps::octet[length];
      memcpy(m_message.data, buffer, length);
    }

    /*!
     * @if jp
     *
     * @brief 内部のバッファからデータを読み込む
     * 
     * @param buffer 書き込み咲のバッファ
     * @param length データの長さ
     * 
     *
     * @else
     *
     * @brief 
     * 
     * @param buffer 
     * @param length 
     * 
     *
     * @endif
     */
    virtual void readData(unsigned char* buffer, unsigned long length) const
    {
      memcpy(buffer, m_message.data, length);
    }
    /*!
     * @if jp
     *
     * @brief データの長さ取得
     * 
     * @return データ長さ
     * 
     *
     * @else
     *
     * @brief 
     * 
     * @return
     * 
     *
     * @endif
     */
    virtual unsigned long getDataLength() const
    {
      return m_message.length;
    }
    /*!
     * @if jp
     *
     * @brief データの符号化
     * 
     * @param data 符号化前のデータ
     * @return true：符号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param data 
     * @return 
     *
     * @endif
     */
    virtual bool serialize(const DataType& data)
    {
      (void)data;
      return false;
    }
    /*!
     * @if jp
     *
     * @brief データの復号化
     * 
     * @param data 復号化後のデータを格納する変数
     * @return true：復号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param data 
     * @return 
     *
     * @endif
     */
    virtual bool deserialize(DataType& data)
    {
      (void)data;
      return false;
    }
    /*!
     * @if jp
     *
     * @brief std_msgsに属するデータを符号化する
     * 
     * @param msg 符号化前のデータ
     * @return true：符号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param msg 
     * @return 
     *
     * @endif
     */
    template <class MessageType>
    bool stdmsg_serialize(const MessageType& msg)
    {
      
      eprosima::fastcdr::FastBuffer fastbuffer((char*) m_message.data, m_message.max_size);
      eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::Cdr::DDS_CDR);
      m_message.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
      ser.serialize_encapsulation();

      std_msgs::msg::typesupport_fastrtps_cpp::cdr_serialize(msg, ser);

      m_message.length = (uint32_t)ser.getSerializedDataLength();

      
      return true;
    }
    /*!
     * @if jp
     *
     * @brief std_msgsに属するデータを復号化する
     * 
     * @param msg 復号化したデータを格納する変数
     * @return true：復号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param msg 
     * @return 
     *
     * @endif
     */
    template <class MessageType>
    bool stdmsg_deserialize(MessageType& msg)
    { 


      eprosima::fastcdr::FastBuffer fastbuffer((char*)m_message.data, m_message.length);
      eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::Cdr::DDS_CDR); // Object that deserializes the data.
      
      deser.read_encapsulation();
      
      m_message.encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

      std_msgs::msg::typesupport_fastrtps_cpp::cdr_deserialize(deser, msg);


      return true;
    }

    /*!
     * @if jp
     *
     * @brief geometry_msgsに属するデータを符号化する
     * 
     * @param msg 符号化前のデータ
     * @return true：符号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param msg 
     * @return 
     *
     * @endif
     */
    template <class MessageType>
    bool geometrymsg_serialize(const MessageType& msg)
    {
      
      eprosima::fastcdr::FastBuffer fastbuffer((char*) m_message.data, m_message.max_size);
      eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::Cdr::DDS_CDR);
      m_message.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
      ser.serialize_encapsulation();

      geometry_msgs::msg::typesupport_fastrtps_cpp::cdr_serialize(msg, ser);

      m_message.length = (uint32_t)ser.getSerializedDataLength();

      
      return true;
    }
    
    /*!
     * @if jp
     *
     * @brief geometry_msgsに属するデータを復号化する
     * 
     * @param msg 復号化したデータを格納する変数
     * @return true：復号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param msg 
     * @return 
     *
     * @endif
     */
    template <class MessageType>
    bool geometrymsg_deserialize(MessageType& msg)
    { 


      eprosima::fastcdr::FastBuffer fastbuffer((char*)m_message.data, m_message.length);
      eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::Cdr::DDS_CDR); // Object that deserializes the data.
      
      deser.read_encapsulation();
      
      m_message.encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

      geometry_msgs::msg::typesupport_fastrtps_cpp::cdr_deserialize(deser, msg);


      return true;
    }

    /*!
     * @if jp
     *
     * @brief sensor_msgsに属するデータを符号化する
     * 
     * @param msg 符号化前のデータ
     * @return true：符号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param msg 
     * @return 
     *
     * @endif
     */
    template <class MessageType>
    bool sensormsg_serialize(const MessageType& msg)
    {
      
      eprosima::fastcdr::FastBuffer fastbuffer((char*) m_message.data, m_message.max_size);
      eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::Cdr::DDS_CDR);
      m_message.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
      ser.serialize_encapsulation();

      sensor_msgs::msg::typesupport_fastrtps_cpp::cdr_serialize(msg, ser);

      m_message.length = (uint32_t)ser.getSerializedDataLength();

      
      return true;
    }
    
    /*!
     * @if jp
     *
     * @brief sensor_msgsに属するデータを復号化する
     * 
     * @param msg 復号化したデータを格納する変数
     * @return true：復号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param msg 
     * @return 
     *
     * @endif
     */
    template <class MessageType>
    bool sensormsg_deserialize(MessageType& msg)
    { 


      eprosima::fastcdr::FastBuffer fastbuffer((char*)m_message.data, m_message.length);
      eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
                eprosima::fastcdr::Cdr::DDS_CDR); // Object that deserializes the data.
      
      deser.read_encapsulation();
      
      m_message.encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

      sensor_msgs::msg::typesupport_fastrtps_cpp::cdr_deserialize(deser, msg);


      return true;
    }

  protected:
    eprosima::fastrtps::rtps::SerializedPayload_t m_message;
  };

  /*!
   * @if jp
   *
   * @class ROS2SimpleData
   *
   * @brief ROS2のFloat32、Int64等をRTMのTimedFloat、TimedLong等の単純なデータ型に変換するシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2SimpleData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <class DataType, class MessageType>
  class ROS2SimpleData : public ROS2SerializerBase<DataType>
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    ROS2SimpleData()
    {

    }
    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    virtual ~ROS2SimpleData()
    {

    }

    /*!
     * @if jp
     *
     * @brief データの符号化
     * 
     * @param data 符号化前のデータ
     * @return true：符号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param data 
     * @return 
     *
     * @endif
     */
    virtual bool serialize(const DataType& data)
    {

      MessageType msg;
      msg.data = data.data;
      
      return ROS2SerializerBase<DataType>::stdmsg_serialize(msg);

    }
    /*!
     * @if jp
     *
     * @brief データの復号化
     * 
     * @param data 復号化後のデータを格納する変数
     * @return true：復号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param data 
     * @return 
     *
     * @endif
     */
    virtual bool deserialize(DataType& data)
    { 

      MessageType msg;

      bool ret = ROS2SerializerBase<DataType>::stdmsg_deserialize(msg);

      data.data = msg.data;

      return ret;
    }
  };

  /*!
   * @if jp
   *
   * @class ROS2SequenceData
   *
   * @brief ROS2のFloat32MultiArray、Int64MultiArray等をRTMのTimedFloatSeq、TimedLongSeq等の配列データ型に変換するシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2SequenceData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <class DataType, class MessageType>
  class ROS2SequenceData : public ROS2SerializerBase<DataType>
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    ROS2SequenceData()
    {

    }
    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    virtual ~ROS2SequenceData()
    {

    }

    /*!
     * @if jp
     *
     * @brief データの符号化
     * 
     * @param data 符号化前のデータ
     * @return true：符号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param data 
     * @return 
     *
     * @endif
     */
    virtual bool serialize(const DataType& data)
    {
      
      MessageType msg;
      
      for(size_t i=0;i < data.data.length();i++)
      {
        msg.data.push_back(data.data[i]);
      }
      
      
      return ROS2SerializerBase<DataType>::stdmsg_serialize(msg);
    };
    /*!
     * @if jp
     *
     * @brief データの復号化
     * 
     * @param data 復号化後のデータを格納する変数
     * @return true：復号化成功、false：失敗
     *
     * @else
     *
     * @brief 
     * 
     * @param data 
     * @return 
     *
     * @endif
     */
    virtual bool deserialize(DataType& data)
    { 

      MessageType msg;

      bool ret = ROS2SerializerBase<DataType>::stdmsg_deserialize(msg);

      data.data.length(msg.data.size());
      int count = 0;
      for(auto & d : msg.data)
      {
        data.data[count] = d;
        count++;
      }

      return ret;
    }
  };
  
}


extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * ROS2シリアライザ のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * 
   *
   * @endif
   */
  DLL_EXPORT void ROS2SerializerInit(RTC::Manager* manager);
}




#endif // RTC_ROSSERIALIZER_H

