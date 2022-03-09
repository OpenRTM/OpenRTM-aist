// -*- C++ -*-
/*!
 * @file  ROSSerializer.cpp
 * @brief ROSSerializer class
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
#include <WinSock2.h>
#pragma warning(push)
#pragma warning( disable : 4267 )
#endif
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int64.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/UInt32.h>
#include <std_msgs/UInt64.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Int8MultiArray.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Int64MultiArray.h>
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/UInt16MultiArray.h>
#include <std_msgs/UInt32MultiArray.h>
#include <std_msgs/UInt64MultiArray.h>
#include <std_msgs/String.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/QuaternionStamped.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <sensor_msgs/Image.h>
#include "ROSSerializer.h"
#include "ROSMessageInfo.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif


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
  void ROSSimpleDataInitBaseFunc(const char* name)
  {
    addSerializer<DataType, ROSSimpleData<DataType, MessageType, originalType, convertedType>>(name);

    GlobalROSMessageInfoList::
            instance().addInfo(name,
            new ROSMessageInfo<MessageType>());
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
  void ROSSimpleDataInit()
  {
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::Float32, originalType, float>("ros:std_msgs/Float32");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::Float64, originalType, double>("ros:std_msgs/Float64");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::Int8, originalType, int8_t>("ros:std_msgs/Int8");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::Int16, originalType, int16_t>("ros:std_msgs/Int16");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::Int32, originalType, int32_t>("ros:std_msgs/Int32");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::Int64, originalType, int64_t>("ros:std_msgs/Int64");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::UInt8, originalType, uint8_t>("ros:std_msgs/UInt8");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::UInt16, originalType, uint16_t>("ros:std_msgs/UInt16");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::UInt32, originalType, uint32_t>("ros:std_msgs/UInt32");
    ROSSimpleDataInitBaseFunc<DataType, std_msgs::UInt64, originalType, uint64_t>("ros:std_msgs/UInt64");
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
  void ROSSequenceDataInitBaseFunc(const char* name)
  {
    addSerializer<DataType, ROSSequenceData<DataType, MessageType, originalType, convertedType>>(name);

    GlobalROSMessageInfoList::
            instance().addInfo(name,
            new ROSMessageInfo<MessageType>());
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
  void ROSSequenceDataInit()
  {
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::Float32MultiArray, originalType, float>("ros:std_msgs/Float32MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::Float64MultiArray, originalType, double>("ros:std_msgs/Float64MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::Int8MultiArray, originalType, int8_t>("ros:std_msgs/Int8MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::Int16MultiArray, originalType, int16_t>("ros:std_msgs/Int16MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::Int32MultiArray, originalType, int32_t>("ros:std_msgs/Int32MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::Int64MultiArray, originalType, int64_t>("ros:std_msgs/Int64MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::UInt8MultiArray, originalType, uint8_t>("ros:std_msgs/UInt8MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::UInt16MultiArray, originalType, uint16_t>("ros:std_msgs/UInt16MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::UInt32MultiArray, originalType, uint32_t>("ros:std_msgs/UInt32MultiArray");
    ROSSequenceDataInitBaseFunc<DataType, std_msgs::UInt64MultiArray, originalType, uint64_t>("ros:std_msgs/UInt64MultiArray");
  }

  /*!
   * @if jp
   *
   * @class ROSStringData
   *
   * @brief RTC::TimedString<-->std_msgs::Stringのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSStringData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROSStringData : public ROSSerializerBase<RTC::TimedString>
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
    ROSStringData() = default;

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
    ~ROSStringData() override = default;

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
        
      std_msgs::String msg;
      msg.data = data.data;
      
      ROSSerializerBase<RTC::TimedString>::m_message = ros::serialization::serializeMessage<std_msgs::String>(msg);

      return true;
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

      std_msgs::String msg;
        
      ros::serialization::deserializeMessage(ROSSerializerBase<RTC::TimedString>::m_message, msg);
      data.data = CORBA::string_dup(msg.data.c_str());

      return true;
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
  void ROSStringDataInit()
  {
    addSerializer<TimedString, ROSStringData>("ros:std_msgs/String");

    GlobalROSMessageInfoList::
            instance().addInfo("ros:std_msgs/String",
            new ROSMessageInfo<std_msgs::String>());
  }

  /*!
   * @if jp
   *
   * @class ROSPoint3DData
   *
   * @brief RTC::TimedPoint3D<-->geometry_msgs::PointStampedのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSPoint3DData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROSPoint3DData : public ROSSerializerBase<RTC::TimedPoint3D>
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
    ROSPoint3DData() = default;
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
    ~ROSPoint3DData() override = default;

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
        
      geometry_msgs::PointStamped msg;
      msg.header.stamp.sec = data.tm.sec;
      msg.header.stamp.nsec = data.tm.nsec;
      msg.point.x = data.data.x;
      msg.point.y = data.data.y;
      msg.point.z = data.data.z;
      
      ROSSerializerBase<RTC::TimedPoint3D>::m_message = ros::serialization::serializeMessage<geometry_msgs::PointStamped>(msg);

      return true;
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

      geometry_msgs::PointStamped msg;
      
      ros::serialization::deserializeMessage(ROSSerializerBase<RTC::TimedPoint3D>::m_message, msg);
      
      data.tm.sec = msg.header.stamp.sec;
      data.tm.nsec = msg.header.stamp.nsec;
      data.data.x = msg.point.x;
      data.data.y = msg.point.y;
      data.data.z = msg.point.z;

      return true;
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
  void ROSPont3DDataInit()
  {
    addSerializer<TimedPoint3D, ROSPoint3DData>("ros:geometry_msgs/PointStamped");

    GlobalROSMessageInfoList::
            instance().addInfo("ros:geometry_msgs/PointStamped",
            new ROSMessageInfo<geometry_msgs::PointStamped>());
  }


  /*!
   * @if jp
   *
   * @class ROSQuaternionData
   *
   * @brief RTC::TimedQuaternion<-->geometry_msgs::QuaternionStampedのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSQuaternionData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROSQuaternionData : public ROSSerializerBase<RTC::TimedQuaternion>
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
    ROSQuaternionData() = default;
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
    ~ROSQuaternionData() override = default;

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
        
      geometry_msgs::QuaternionStamped  msg;
      msg.header.stamp.sec = data.tm.sec;
      msg.header.stamp.nsec = data.tm.nsec;
      msg.quaternion.x = data.data.x;
      msg.quaternion.y = data.data.y;
      msg.quaternion.z = data.data.z;
      msg.quaternion.w = data.data.w;
      
      ROSSerializerBase<RTC::TimedQuaternion>::m_message = ros::serialization::serializeMessage<geometry_msgs::QuaternionStamped>(msg);

      return true;
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

      geometry_msgs::QuaternionStamped msg;
      
      ros::serialization::deserializeMessage(ROSSerializerBase<RTC::TimedQuaternion>::m_message, msg);
      
      data.tm.sec = msg.header.stamp.sec;
      data.tm.nsec = msg.header.stamp.nsec;
      data.data.x = msg.quaternion.x;
      data.data.y = msg.quaternion.y;
      data.data.z = msg.quaternion.z;
      data.data.w = msg.quaternion.w;

      return true;
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
  void ROSQuaternionDataInit()
  {
    addSerializer<TimedQuaternion, ROSQuaternionData>("ros:geometry_msgs/QuaternionStamped");

    GlobalROSMessageInfoList::
            instance().addInfo("ros:geometry_msgs/QuaternionStamped",
            new ROSMessageInfo<geometry_msgs::QuaternionStamped >());
  }


  /*!
   * @if jp
   *
   * @class ROSVector3DData
   *
   * @brief RTC::TimedVector3D<-->geometry_msgs::Vector3Stampedのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSVector3DData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROSVector3DData : public ROSSerializerBase<RTC::TimedVector3D>
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
    ROSVector3DData() = default;
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
    ~ROSVector3DData() override = default;

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
        
      geometry_msgs::Vector3Stamped  msg;
      msg.header.stamp.sec = data.tm.sec;
      msg.header.stamp.nsec = data.tm.nsec;
      msg.vector.x = data.data.x;
      msg.vector.y = data.data.y;
      msg.vector.z = data.data.z;

      
      ROSSerializerBase<RTC::TimedVector3D>::m_message = ros::serialization::serializeMessage<geometry_msgs::Vector3Stamped>(msg);

      return true;
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

      geometry_msgs::Vector3Stamped msg;
      
      ros::serialization::deserializeMessage(ROSSerializerBase<RTC::TimedVector3D>::m_message, msg);
      
      data.tm.sec = msg.header.stamp.sec;
      data.tm.nsec = msg.header.stamp.nsec;
      data.data.x = msg.vector.x;
      data.data.y = msg.vector.y;
      data.data.z = msg.vector.z;

      return true;
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
  void ROSVector3DDataInit()
  {
    addSerializer<TimedVector3D, ROSVector3DData>("ros:geometry_msgs/Vector3Stamped");

    GlobalROSMessageInfoList::
            instance().addInfo("ros:geometry_msgs/Vector3Stamped",
            new ROSMessageInfo<geometry_msgs::Vector3Stamped >());
  }

  /*!
   * @if jp
   *
   * @class ROSCameraImageData
   *
   * @brief RTC::CameraImage<-->sensor_msgs::Imageのシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSCameraImageData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROSCameraImageData : public ROSSerializerBase<RTC::CameraImage>
  {
  private:
    sensor_msgs::Image  m_msg;
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
    ROSCameraImageData() = default;
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
    ~ROSCameraImageData() override
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
    bool serialize(const RTC::CameraImage& data) override
    {
        
      
      m_msg.header.stamp.sec = data.tm.sec;
      m_msg.header.stamp.nsec = data.tm.nsec;
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
      if(data.pixels.length() > 0)
      {
          memcpy(&m_msg.data[0], &data.pixels[0], data.pixels.length());
      }
      
      ROSSerializerBase<RTC::CameraImage>::m_message = ros::serialization::serializeMessage<sensor_msgs::Image>(m_msg);

      return true;
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

      ros::serialization::deserializeMessage(ROSSerializerBase<RTC::CameraImage>::m_message, m_msg);
      
      
      data.tm.sec = m_msg.header.stamp.sec;
      data.tm.nsec = m_msg.header.stamp.nsec;

      if (m_msg.height > USHRT_MAX || m_msg.width > USHRT_MAX)
      {
          return false;
      }

      data.height = static_cast<CORBA::UShort>(m_msg.height);
      data.width = static_cast<CORBA::UShort>(m_msg.width);
      data.format = CORBA::string_dup(m_msg.encoding.c_str());


      data.pixels.length(static_cast<CORBA::ULong>(m_msg.data.size()));
      
      if(m_msg.data.size() > 0)
      {
          memcpy(&data.pixels[0], &m_msg.data[0], data.pixels.length());
      }
      return true;
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
  void ROSCameraImageDataInit()
  {
    addSerializer<CameraImage, ROSCameraImageData>("ros:sensor_msgs/Image");

    RTC::GlobalROSMessageInfoList::
            instance().addInfo("ros:sensor_msgs/Image",
            new RTC::ROSMessageInfo<sensor_msgs::Image >());
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
  void ROSSerializerInit(RTC::Manager* /*manager*/)
  {
    RTC::ROSSimpleDataInit<RTC::TimedState, CORBA::Short>();
    RTC::ROSSimpleDataInit<RTC::TimedShort, CORBA::Short>();
    RTC::ROSSimpleDataInit<RTC::TimedLong, CORBA::Long>();
    RTC::ROSSimpleDataInit<RTC::TimedUShort, CORBA::UShort>();
    RTC::ROSSimpleDataInit<RTC::TimedULong, CORBA::ULong>();
    RTC::ROSSimpleDataInit<RTC::TimedFloat, CORBA::Float>();
    RTC::ROSSimpleDataInit<RTC::TimedDouble, CORBA::Double>();

    RTC::ROSSequenceDataInit<RTC::TimedShortSeq, CORBA::Short>();
    RTC::ROSSequenceDataInit<RTC::TimedLongSeq, CORBA::Long>();
    RTC::ROSSequenceDataInit<RTC::TimedUShortSeq, CORBA::UShort>();
    RTC::ROSSequenceDataInit<RTC::TimedULongSeq, CORBA::ULong>();
    RTC::ROSSequenceDataInit<RTC::TimedFloatSeq, CORBA::Float>();
    RTC::ROSSequenceDataInit<RTC::TimedDoubleSeq, CORBA::Double>();

    RTC::ROSStringDataInit();
    RTC::ROSPont3DDataInit();
    RTC::ROSQuaternionDataInit();
    RTC::ROSVector3DDataInit();
    RTC::ROSCameraImageDataInit();
  }
}


