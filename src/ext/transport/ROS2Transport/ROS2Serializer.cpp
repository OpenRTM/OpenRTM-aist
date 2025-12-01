// -*- C++ -*-
/*!
 * @file  ROS2Serializer.cpp
 * @brief ROS2Serializer class
 * @date  $Date: 2019-01-31 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#endif

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

#include <cstdint>


#include "ROS2Serializer.h"


namespace RTC
{
  
  /*!
   * @if jp
   *
   * @brief 単純なデータ型のシリアライザを登録するテンプレート関数
   * 
   * @param name シリアライザの名前
   *
   * @else
   *
   * @brief 
   * 
   * @param name 
   *
   * @endif
   */
  template <class DataType, class MessageType, typename originalType, typename convertedType>
  void ROS2SimpleDataInitBaseFunc(const char* name)
  {
    addRos2Serializer<DataType, MessageType, ROS2SimpleData<DataType, MessageType, originalType, convertedType>>(name);
  }

  /*!
   * @if jp
   *
   * @brief 単純なデータ型のシリアライザを登録するテンプレート関数
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  template <class DataType, typename originalType>
  void ROS2SimpleDataInit()
  {
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::Float32, originalType, float>("ros2:std_msgs/Float32");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::Float64, originalType, double>("ros2:std_msgs/Float64");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::Int8, originalType, int8_t>("ros2:std_msgs/Int8");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::Int16, originalType, int16_t>("ros2:std_msgs/Int16");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::Int32, originalType, int32_t>("ros2:std_msgs/Int32");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::Int64, originalType, int64_t>("ros2:std_msgs/Int64");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::UInt8, originalType, uint8_t>("ros2:std_msgs/UInt8");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::UInt16, originalType, uint16_t>("ros2:std_msgs/UInt16");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::UInt32, originalType, uint32_t>("ros2:std_msgs/UInt32");
    ROS2SimpleDataInitBaseFunc<DataType, std_msgs::msg::UInt64, originalType, uint64_t>("ros2:std_msgs/UInt64");
    
  }

  /*!
   * @if jp
   *
   * @brief 配列データ型のシリアライザを登録するテンプレート関数
   * 
   * @param name シリアライザの名前
   *
   * @else
   *
   * @brief 
   * 
   * @param name 
   *
   * @endif
   */
  template <class DataType, class MessageType, typename originalType, typename convertedType>
  void ROS2SequenceDataInitBaseFunc(const char* name)
  {
    addRos2Serializer<DataType, MessageType, ROS2SequenceData<DataType, MessageType, originalType, convertedType>>(name);
  }

  /*!
   * @if jp
   *
   * @brief 配列データ型のシリアライザを登録するテンプレート関数
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  template <class DataType, typename originalType>
  void ROS2SequenceDataInit()
  {
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::Float32MultiArray, originalType, float>("ros2:std_msgs/Float32MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::Float64MultiArray, originalType, double>("ros2:std_msgs/Float64MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::Int8MultiArray, originalType, int8_t>("ros2:std_msgs/Int8MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::Int16MultiArray, originalType, int16_t>("ros2:std_msgs/Int16MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::Int32MultiArray, originalType, int32_t>("ros2:std_msgs/Int32MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::Int64MultiArray, originalType, int64_t>("ros2:std_msgs/Int64MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::UInt8MultiArray, originalType, uint8_t>("ros2:std_msgs/UInt8MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::UInt16MultiArray, originalType, uint16_t>("ros2:std_msgs/UInt16MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::UInt32MultiArray, originalType, uint32_t>("ros2:std_msgs/UInt32MultiArray");
    ROS2SequenceDataInitBaseFunc<DataType, std_msgs::msg::UInt64MultiArray, originalType, uint64_t>("ros2:std_msgs/UInt64MultiArray");
  }

  /*!
   * @if jp
   *
   * @class ROS2StringData
   *
   * @brief RTC::TimedString<-->std_msgs::Stringのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2StringData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROS2StringData : public ROS2SerializerBase<RTC::TimedString>
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
    ROS2StringData() = default;
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
    ~ROS2StringData() override = default;

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
    bool serialize(const RTC::TimedString& data) override
    {
        
      std_msgs::msg::String msg;
      msg.data = data.data;
      
      return ROS2SerializerBase<RTC::TimedString>::stdmsg_serialize(msg);
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
    bool deserialize(RTC::TimedString& data) override
    { 

      std_msgs::msg::String msg;
        
      bool ret = ROS2SerializerBase<RTC::TimedString>::stdmsg_deserialize(msg);

      data.data = CORBA::string_dup(msg.data.c_str());

      return ret;
    }
  };

  /*!
   * @if jp
   *
   * @brief RTC::TimedString<-->std_msgs::Stringのシリアライザを登録する
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  void ROS2StringDataInit()
  {
    addRos2Serializer<TimedString, std_msgs::msg::String, ROS2StringData>("ros2:std_msgs/String");
  }

  /*!
   * @if jp
   *
   * @class ROS2Point3DData
   *
   * @brief RTC::TimedPoint3D<-->geometry_msgs::PointStampedのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2Point3DData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROS2Point3DData : public ROS2SerializerBase<RTC::TimedPoint3D>
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
    ROS2Point3DData() = default;

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
    ~ROS2Point3DData() override = default;

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
    bool serialize(const RTC::TimedPoint3D& data) override
    {
        
      geometry_msgs::msg::PointStamped msg;
      msg.header.stamp.sec = data.tm.sec;
      msg.header.stamp.nanosec = data.tm.nsec;
      msg.point.x = data.data.x;
      msg.point.y = data.data.y;
      msg.point.z = data.data.z;
      
      return ROS2SerializerBase<RTC::TimedPoint3D>::geometrymsg_serialize(msg);
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
    bool deserialize(RTC::TimedPoint3D& data) override
    { 

      geometry_msgs::msg::PointStamped msg;
      
      bool ret = ROS2SerializerBase<RTC::TimedPoint3D>::geometrymsg_deserialize(msg);

      data.tm.sec = msg.header.stamp.sec;
      data.tm.nsec = msg.header.stamp.nanosec;
      data.data.x = msg.point.x;
      data.data.y = msg.point.y;
      data.data.z = msg.point.z;

      return ret;
    }
  };

  /*!
   * @if jp
   *
   * @brief RTC::TimedPoint3D<-->geometry_msgs::PointStampedのシリアライザを登録する
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  void ROS2Pont3DDataInit()
  {
    addRos2Serializer<TimedPoint3D, geometry_msgs::msg::PointStamped, ROS2Point3DData>("ros2:geometry_msgs/PointStamped");
  }

  /*!
   * @if jp
   *
   * @class ROS2QuaternionData
   *
   * @brief RTC::TimedQuaternion<-->geometry_msgs::QuaternionStampedのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2QuaternionData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROS2QuaternionData : public ROS2SerializerBase<RTC::TimedQuaternion>
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
    ROS2QuaternionData() = default;
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
    ~ROS2QuaternionData() override = default;

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
    bool serialize(const RTC::TimedQuaternion& data) override
    {
        
      geometry_msgs::msg::QuaternionStamped  msg;
      msg.header.stamp.sec = data.tm.sec;
      msg.header.stamp.nanosec = data.tm.nsec;
      msg.quaternion.x = data.data.x;
      msg.quaternion.y = data.data.y;
      msg.quaternion.z = data.data.z;
      msg.quaternion.w = data.data.w;
      
      
      return ROS2SerializerBase<RTC::TimedQuaternion>::geometrymsg_serialize(msg);
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
    bool deserialize(RTC::TimedQuaternion& data) override
    { 

      geometry_msgs::msg::QuaternionStamped msg;
      
      bool ret = ROS2SerializerBase<RTC::TimedQuaternion>::geometrymsg_deserialize(msg);
      data.tm.sec = msg.header.stamp.sec;
      data.tm.nsec = msg.header.stamp.nanosec;
      data.data.x = msg.quaternion.x;
      data.data.y = msg.quaternion.y;
      data.data.z = msg.quaternion.z;
      data.data.w = msg.quaternion.w;

      return ret;
    }
  };

  /*!
   * @if jp
   *
   * @brief RTC::TimedQuaternion<-->geometry_msgs::QuaternionStampedのシリアライザを登録する
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  void ROS2QuaternionDataInit()
  {
    addRos2Serializer<TimedQuaternion, geometry_msgs::msg::QuaternionStamped, ROS2QuaternionData>("ros2:geometry_msgs/QuaternionStamped");
  }

  /*!
   * @if jp
   *
   * @class ROS2Vector3DData
   *
   * @brief RTC::TimedVector3D<-->geometry_msgs::Vector3Stampedのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2Vector3DData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROS2Vector3DData : public ROS2SerializerBase<RTC::TimedVector3D>
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
    ROS2Vector3DData() = default;

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
    ~ROS2Vector3DData() override = default;

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
    bool serialize(const RTC::TimedVector3D& data) override
    {
        
      geometry_msgs::msg::Vector3Stamped  msg;
      msg.header.stamp.sec = data.tm.sec;
      msg.header.stamp.nanosec = data.tm.nsec;
      msg.vector.x = data.data.x;
      msg.vector.y = data.data.y;
      msg.vector.z = data.data.z;

      
      return ROS2SerializerBase<RTC::TimedVector3D>::geometrymsg_serialize(msg);
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
    bool deserialize(RTC::TimedVector3D& data) override
    { 

      geometry_msgs::msg::Vector3Stamped msg;
      
      bool ret = ROS2SerializerBase<RTC::TimedVector3D>::geometrymsg_deserialize(msg);

      data.tm.sec = msg.header.stamp.sec;
      data.tm.nsec = msg.header.stamp.nanosec;
      data.data.x = msg.vector.x;
      data.data.y = msg.vector.y;
      data.data.z = msg.vector.z;

      return ret;
    }
  };

  /*!
   * @if jp
   *
   * @brief RTC::TimedVector3D<-->geometry_msgs::Vector3Stampedのシリアライザを登録する
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  void ROS2Vector3DDataInit()
  {
    addRos2Serializer<TimedVector3D, geometry_msgs::msg::Vector3Stamped , ROS2Vector3DData>("ros2:geometry_msgs/Vector3Stamped");
  }

  
  /*!
   * @if jp
   *
   * @class ROS2CameraImageData
   *
   * @brief RTC::CameraImage<-->sensor_msgs::Imageのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROS2CameraImageData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROS2CameraImageData : public ROS2SerializerBase<RTC::CameraImage>
  {
  private:
    sensor_msgs::msg::Image  m_msg;
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
    ROS2CameraImageData() = default;
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
    ~ROS2CameraImageData() override = default;

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
    bool serialize(const RTC::CameraImage& data) override
    {
      m_msg.header.stamp.sec = data.tm.sec;
      m_msg.header.stamp.nanosec = data.tm.nsec;
      m_msg.height = data.height;
      m_msg.width = data.width;
      if(std::string(data.format).empty())
      {
        m_msg.encoding = "rgb8";
      }
      else
      {
        m_msg.encoding = data.format;
      }
      m_msg.step = 1920;
      m_msg.data.resize(data.pixels.length());
      CORBA::ULong image_size = data.pixels.length() / 3;
      for(CORBA::ULong i=0;i < image_size;i++)
      {
        m_msg.data[i*3] = data.pixels[i*3+2];
        m_msg.data[i*3+1] = data.pixels[i*3+1];
        m_msg.data[i*3+2] = data.pixels[i*3];
      }
      /*
      if(m_msg.data.size() > 0)
      {
        memcpy(&m_msg.data[0], &data.pixels[0], data.pixels.length());
      }
      */

      
      return ROS2SerializerBase<RTC::CameraImage>::sensormsg_serialize(m_msg);
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
    bool deserialize(RTC::CameraImage& data) override
    {
      
      bool ret = ROS2SerializerBase<RTC::CameraImage>::sensormsg_deserialize(m_msg);
      
      
      data.tm.sec = m_msg.header.stamp.sec;
      data.tm.nsec = m_msg.header.stamp.nanosec;

      if (m_msg.height > USHRT_MAX || m_msg.width > USHRT_MAX)
      {
          return false;
      }

      data.height = static_cast<CORBA::UShort>(m_msg.height);
      data.width = static_cast<CORBA::UShort>(m_msg.width);

      
      data.format = CORBA::string_dup(m_msg.encoding.c_str());


      data.pixels.length((CORBA::ULong)m_msg.data.size());
      
      CORBA::ULong image_size = data.pixels.length() / 3;
      for(CORBA::ULong i=0;i < image_size;i++)
      {
        data.pixels[i*3+2] = m_msg.data[i*3];
        data.pixels[i*3+1] = m_msg.data[i*3+1];
        data.pixels[i*3] = m_msg.data[i*3+2];
      }
      /*
      if(data.pixels.length() > 0)
      {
        memcpy(&data.pixels[0], &m_msg.data[0], data.pixels.length());
      }
      */

      return ret;
    }
  };

  /*!
   * @if jp
   *
   * @brief RTC::CameraImage<-->sensor_msgs::Imageのシリアライザを登録する
   * 
   *
   * @else
   *
   * @brief 
   * 
   *
   * @endif
   */
  void ROS2CameraImageDataInit()
  {
    addSerializer<CameraImage, ROS2CameraImageData>("ros2:sensor_msgs/Image");

    GlobalFastRTPSMessageInfoList::
            instance().addInfo("ros2:sensor_msgs/Image",
            new ROS2MessageInfo<sensor_msgs::msg::Image >());
  }

}


extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void ROS2SerializerInit(RTC::Manager* /*manager*/)
  {
    
    RTC::ROS2SimpleDataInit<RTC::TimedState, CORBA::Short>();
    RTC::ROS2SimpleDataInit<RTC::TimedShort, CORBA::Short>();
    RTC::ROS2SimpleDataInit<RTC::TimedLong, CORBA::Long>();
    RTC::ROS2SimpleDataInit<RTC::TimedUShort, CORBA::UShort>();
    RTC::ROS2SimpleDataInit<RTC::TimedULong, CORBA::ULong>();
    RTC::ROS2SimpleDataInit<RTC::TimedFloat, CORBA::Float>();
    RTC::ROS2SimpleDataInit<RTC::TimedDouble, CORBA::Double>();

    RTC::ROS2SequenceDataInit<RTC::TimedShortSeq, CORBA::Short>();
    RTC::ROS2SequenceDataInit<RTC::TimedLongSeq, CORBA::Long>();
    RTC::ROS2SequenceDataInit<RTC::TimedUShortSeq, CORBA::UShort>();
    RTC::ROS2SequenceDataInit<RTC::TimedULongSeq, CORBA::ULong>();
    RTC::ROS2SequenceDataInit<RTC::TimedFloatSeq, CORBA::Float>();
    RTC::ROS2SequenceDataInit<RTC::TimedDoubleSeq, CORBA::Double>();

    RTC::ROS2StringDataInit();

    RTC::ROS2Pont3DDataInit();
    RTC::ROS2QuaternionDataInit();
    RTC::ROS2Vector3DDataInit();
    RTC::ROS2CameraImageDataInit();
  }
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif