// -*- C++ -*-
/*!
 * @file  ROSSerializer.h
 * @brief ROS Serializer 
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

#ifndef RTC_ROSSERIALIZER_H
#define RTC_ROSSERIALIZER_H

#include <rtm/ByteDataStreamBase.h>
#include <coil/Factory.h>
#include <rtm/Manager.h>
#include <ros/serialization.h>



namespace RTC
{
  /*!
   * @if jp
   *
   * @class ROSSerializerBase
   *
   * @brief ROSシリアライザ基底クラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSSerializerBase
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <class DataType>
  class ROSSerializerBase : public ByteDataStream<DataType>
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
    ROSSerializerBase(){};
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
    virtual ~ROSSerializerBase(){};

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
      boost::shared_array<uint8_t> buffer_(new uint8_t[length]);

      memcpy(buffer_.get(), buffer, length);

      m_message = ros::SerializedMessage(buffer_, length);
      m_message.message_start = buffer_.get()+4;

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
      memcpy(buffer, m_message.buf.get(), length);

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
      return m_message.num_bytes;
    };
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
      (void)data;
      return false;
    };

  protected:
    ros::SerializedMessage m_message;
  };


  /*!
   * @if jp
   *
   * @class ROSSimpleData
   *
   * @brief ROSのFloat32、Int64等をRTMのTimedFloat、TimedLong等の単純なデータ型に変換するシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSSimpleData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <class DataType, class MessageType>
  class ROSSimpleData : public ROSSerializerBase<DataType>
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
    ROSSimpleData()
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
    virtual ~ROSSimpleData()
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
      
      ROSSerializerBase<DataType>::m_message = ros::serialization::serializeMessage<MessageType>(msg);

      return true;
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
        
      ros::serialization::deserializeMessage(ROSSerializerBase<DataType>::m_message, msg);
      data.data = msg.data;

      return true;
    };
  };

  /*!
   * @if jp
   *
   * @class ROSSequenceData
   *
   * @brief ROSのFloat32MultiArray、Int64MultiArray等をRTMのTimedFloatSeq、TimedLongSeq等の配列データ型に変換するシリアライザ
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSSequenceData
   *
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <class DataType, class MessageType>
  class ROSSequenceData : public ROSSerializerBase<DataType>
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
    ROSSequenceData()
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
    virtual ~ROSSequenceData()
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
      
      
      ROSSerializerBase<DataType>::m_message = ros::serialization::serializeMessage<MessageType>(msg);

      return true;
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
      
      ros::serialization::deserializeMessage(ROSSerializerBase<DataType>::m_message, msg);

      data.data.length(msg.data.size());
      int count = 0;
      for(auto & d : msg.data)
      {
        data.data[count] = d;
        count++;
      }

      return true;
    };
  };
}


extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * ROSシリアライザ のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * 
   *
   * @endif
   */
  DLL_EXPORT void ROSSerializerInit(RTC::Manager* manager);
};




#endif // RTC_ROSSERIALIZER_H

