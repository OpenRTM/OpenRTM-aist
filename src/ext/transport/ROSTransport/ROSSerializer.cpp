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
#include <algorithm>
#include <climits>
#include <cstring>
#include <limits>
#include <map>
#include <regex>
#include <string>
#include <utility>
#include "ROSSerializer.h"

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
    addRosSerializer<DataType, MessageType, ROSSimpleData<DataType, MessageType, originalType, convertedType>>(name);
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
    addRosSerializer<DataType, MessageType, ROSSequenceData<DataType, MessageType, originalType, convertedType>>(name);
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
    addRosSerializer<TimedString, std_msgs::String, ROSStringData>("ros:std_msgs/String");
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
    addRosSerializer<TimedPoint3D, geometry_msgs::PointStamped, ROSPoint3DData>("ros:geometry_msgs/PointStamped");
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
    addRosSerializer<TimedQuaternion, geometry_msgs::QuaternionStamped, ROSQuaternionData>("ros:geometry_msgs/QuaternionStamped");
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

    addRosSerializer<TimedVector3D, geometry_msgs::Vector3Stamped, ROSVector3DData>("ros:geometry_msgs/Vector3Stamped");
  }

  namespace
  {
    /*!
     * @if jp
     *
     * @brief 画像エンコーディングから画素要素のビット深度とチャネル数を取得する
     *
     * @param encoding sensor_msgs/Image のエンコーディング
     * @param depth 画素要素のビット深度
     * @param channels チャネル数
     * @return true：対応するエンコーディング、false：未対応
     *
     * @else
     *
     * @brief Get the bit depth of a pixel element and the number of channels
     *        of an image encoding
     *
     * @param encoding Encoding of sensor_msgs/Image
     * @param depth Bit depth of a pixel element
     * @param channels Number of channels
     * @return true: supported encoding, false: unsupported encoding
     *
     * @endif
     */
    bool imageEncodingLayout(const std::string& encoding,
                             unsigned int& depth, unsigned int& channels)
    {
      static const std::map<std::string, std::pair<unsigned int, unsigned int>> layouts{
        {"rgb8", {8, 3}}, {"bgr8", {8, 3}}, {"rgba8", {8, 4}}, {"bgra8", {8, 4}},
        {"rgb16", {16, 3}}, {"bgr16", {16, 3}}, {"rgba16", {16, 4}}, {"bgra16", {16, 4}},
        {"mono8", {8, 1}}, {"mono16", {16, 1}},
        {"yuv422", {8, 2}}, {"yuv422_yuy2", {8, 2}}};
      auto it = layouts.find(encoding);
      if (it != layouts.end())
        {
          depth = it->second.first;
          channels = it->second.second;
          return true;
        }
      static const std::regex bayer("bayer_(rggb|bggr|gbrg|grbg)(8|16)");
      static const std::regex opencv("(8U|8S|16U|16S|32S|32F|64F)C([1-9][0-9]{0,2})");
      std::smatch match;
      if (std::regex_match(encoding, match, bayer))
        {
          depth = match[2].str() == "16" ? 16U : 8U;
          channels = 1;
          return true;
        }
      if (std::regex_match(encoding, match, opencv))
        {
          depth = static_cast<unsigned int>(std::stoul(match[1].str()));
          channels = static_cast<unsigned int>(std::stoul(match[2].str()));
          return true;
        }
      return false;
    }

    /*!
     * @if jp
     *
     * @brief 実行環境がビッグエンディアンかを判定する
     *
     * @return true：ビッグエンディアン、false：リトルエンディアン
     *
     * @else
     *
     * @brief Check whether the host is big endian
     *
     * @return true: big endian, false: little endian
     *
     * @endif
     */
    bool isHostBigEndian()
    {
      const std::uint16_t probe(1);
      std::uint8_t first(0);
      std::memcpy(&first, &probe, 1);
      return first == 0;
    }

    /*!
     * @if jp
     *
     * @brief 画素要素ごとにバイト順を反転する
     *
     * @param pixels 画素データ
     * @param length 画素データのバイト数
     * @param sample 画素要素のバイト数
     *
     * @else
     *
     * @brief Reverse the byte order of each pixel element
     *
     * @param pixels Pixel data
     * @param length Number of bytes of the pixel data
     * @param sample Number of bytes of a pixel element
     *
     * @endif
     */
    void swapSampleBytes(CORBA::Octet* pixels, std::size_t length,
                         std::size_t sample)
    {
      for (std::size_t i = 0; i + sample <= length; i += sample)
        {
          std::reverse(pixels + i, pixels + i + sample);
        }
    }
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
      std::string encoding(static_cast<const char*>(data.format));
      if (encoding.empty())
        {
          encoding = "rgb8";
        }
      unsigned int depth(0);
      unsigned int channels(0);
      if (!imageEncodingLayout(encoding, depth, channels))
        {
          return false;
        }
      const unsigned int bpp(depth * channels);
      if (data.bpp != 0 && data.bpp != bpp)
        {
          return false;
        }
      const std::size_t step(static_cast<std::size_t>(data.width) * (bpp / 8));
      const std::size_t length(data.pixels.length());
      if (length != step * data.height
          || step > std::numeric_limits<std::uint32_t>::max())
        {
          return false;
        }

      m_msg.header.stamp.sec = data.tm.sec;
      m_msg.header.stamp.nsec = data.tm.nsec;
      m_msg.height = data.height;
      m_msg.width = data.width;
      m_msg.encoding = encoding;
      m_msg.is_bigendian = isHostBigEndian() ? 1 : 0;
      m_msg.step = static_cast<std::uint32_t>(step);
      m_msg.data.assign(data.pixels.get_buffer(),
                        data.pixels.get_buffer() + length);

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

      if (m_msg.height > USHRT_MAX || m_msg.width > USHRT_MAX)
      {
          return false;
      }

      unsigned int depth(0);
      unsigned int channels(0);
      if (!imageEncodingLayout(m_msg.encoding, depth, channels))
        {
          return false;
        }
      const unsigned int bpp(depth * channels);
      const std::size_t rowSize(static_cast<std::size_t>(m_msg.width) * (bpp / 8));
      const std::size_t step(m_msg.step);
      const std::size_t height(m_msg.height);
      if (step < rowSize || m_msg.data.size() != step * height
          || rowSize * height > std::numeric_limits<CORBA::ULong>::max())
        {
          return false;
        }

      data.tm.sec = m_msg.header.stamp.sec;
      data.tm.nsec = m_msg.header.stamp.nsec;
      data.height = static_cast<CORBA::UShort>(m_msg.height);
      data.width = static_cast<CORBA::UShort>(m_msg.width);
      data.bpp = static_cast<CORBA::UShort>(bpp);
      data.format = CORBA::string_dup(m_msg.encoding.c_str());

      const std::size_t length(rowSize * height);
      data.pixels.length(static_cast<CORBA::ULong>(length));
      CORBA::Octet* pixels(data.pixels.get_buffer());
      for (std::size_t row = 0; row < height && rowSize > 0; ++row)
        {
          std::memcpy(pixels + row * rowSize, m_msg.data.data() + row * step, rowSize);
        }
      if (depth > 8 && (m_msg.is_bigendian != 0) != isHostBigEndian())
        {
          swapSampleBytes(pixels, length, depth / 8);
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
    addRosSerializer<CameraImage, sensor_msgs::Image, ROSCameraImageData>("ros:sensor_msgs/Image");
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


