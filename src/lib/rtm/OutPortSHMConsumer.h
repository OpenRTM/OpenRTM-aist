// -*- C++ -*-
/*!
 * @file  OutPortSHMConsumer.h
 * @brief OutPortSHMConsumer class
 * @date  $Date: 2017-10-30 07:49:59 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017 Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_OUTPORTSHMCONSUMER_H
#define RTC_OUTPORTSHMCONSUMER_H

#include <rtm/SharedMemoryPort.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/OutPortConsumer.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @class OutPortSHMConsumer
   * @brief OutPortSHMConsumer クラス
   *
   * OutPortConsumer 
   *
   * 通信手段に 共有メモリ を利用した出力ポートプロバイダーの実装クラス。
   *
   * @since 0.4.0
   *
   * @else
   * @class OutPortSHMConsumer
   * @brief OutPortSHMConsumer class
   *
   * 
   *
   * @since 0.4.0
   *
   * @endif
   */
  class OutPortSHMConsumer
    : public OutPortConsumer,
      public CorbaConsumer< ::OpenRTM::PortSharedMemory >
  {
  public:

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    OutPortSHMConsumer();

    /*!
     * @if jp
     * @brief デストラクタ
     *
     * デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~OutPortSHMConsumer() override; 

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * 
     * 
     * @param prop 設定情報
     *
     * @else
     *
     * @brief Initializing configuration
     *
     * 
     *
     * @param prop Configuration information
     *
     * @endif
     */
    void init(coil::Properties& prop) override;

    /*!
     * @if jp
     * @brief バッファをセットする
     *
     * 
     *
     * @param buffer OutPortProviderがデータを取り出すバッファへのポインタ
     *
     * @else
     * @brief Setting outside buffer's pointer
     *
     * 
     * 
     * @param buffer A pointer to a data buffer to be used by OutPortProvider
     *
     * @endif
     */
    void setBuffer(CdrBufferBase* buffer) override;

    /*!
     * @if jp
     * @brief リスナを設定する。
     *
     * 
     *
     * @param info 接続情報
     * @param listeners リスナオブジェクト
     *
     * @else
     * @brief Set the listener. 
     *
     * 
     *
     * @param info Connector information
     * @param listeners Listener objects
     *
     * @endif
     */
    void setListener(ConnectorInfo& info,
                             ConnectorListenersBase* listeners) override;

    /*!
     * @if jp
     * @brief データを読み出す
     *
     * 設定されたデータを読み出す。
     *
     * @param data 読み出したデータを受け取るオブジェクト
     *
     * @return データ読み出し処理結果(読み出し成功:true、読み出し失敗:false)
     *
     * @else
     * @brief Read data
     *
     * Read set data
     *
     * @param data Object to receive the read data
     *
     * @return Read result (Successful:true, Failed:false)
     *
     * @endif
     */
    DataPortStatus get(ByteData& data) override;

    /*!
     * @if jp
     * @brief データ受信通知への登録
     *
     * 指定されたプロパティに基づいて、データ受信通知の受け取りに登録する。
     *
     * @param properties 登録情報
     *
     * @return 登録処理結果(登録成功:true、登録失敗:false)
     *
     * @else
     * @brief Subscribe the data receive notification
     *
     * Subscribe the data receive notification based on specified property
     * information
     *
     * @param properties Subscription information
     *
     * @return Subscription result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool subscribeInterface(const SDOPackage::NVList& properties) override;
    
    /*!
     * @if jp
     * @brief データ受信通知からの登録解除
     *
     * データ受信通知の受け取りから登録を解除する。
     *
     * @param properties 登録解除情報
     *
     * @else
     * @brief Unsubscribe the data receive notification
     *
     * Unsubscribe the data receive notification.
     *
     * @param properties Unsubscription information
     *
     * @endif
     */
    void unsubscribeInterface(const SDOPackage::NVList& properties) override;
    bool setObject(CORBA::Object_ptr obj) override;
    
  private:
    /*!
     * @if jp
     * @brief リターンコード変換 (DataPortStatus -> BufferStatus)
     * @else
     * @brief Return codes conversion
     * @endif
     */
    DataPortStatus convertReturn(::OpenRTM::PortStatus status,
                                              ByteData& data);

    /*!
     * @if jp
     * @brief ON_BUFFER_WRITE のリスナへ通知する。 
     * @param data cdrMemoryStream
     * @else
     * @brief Notify an ON_BUFFER_WRITE event to listeners
     * @param data cdrMemoryStream
     * @endif
     */
    inline void onBufferWrite(ByteData& data)
    {
      m_listeners->notifyIn(ConnectorDataListenerType::ON_BUFFER_WRITE, m_profile, data);
    }

    /*!
     * @if jp
     * @brief ON_BUFFER_FULL のリスナへ通知する。 
     * @param data cdrMemoryStream
     * @else
     * @brief Notify an ON_BUFFER_FULL event to listeners
     * @param data cdrMemoryStream
     * @endif
     */
    inline void onBufferFull(ByteData& data)
    {
      m_listeners->notifyIn(ConnectorDataListenerType::ON_BUFFER_FULL, m_profile, data);
    }

    /*!
     * @if jp
     * @brief ON_RECEIVED のリスナへ通知する。 
     * @param data cdrMemoryStream
     * @else
     * @brief Notify an ON_RECEIVED event to listeners
     * @param data cdrMemoryStream
     * @endif
     */
    inline void onReceived(ByteData& data)
    {
      m_listeners->notifyIn(ConnectorDataListenerType::ON_RECEIVED, m_profile, data);
    }

    /*!
     * @if jp
     * @brief ON_RECEIVER_FULL のリスナへ通知する。 
     * @param data cdrMemoryStream
     * @else
     * @brief Notify an ON_RECEIVER_FULL event to listeners
     * @param data cdrMemoryStream
     * @endif
     */
    inline void onReceiverFull(ByteData& data)
    {
      m_listeners->notifyIn(ConnectorDataListenerType::ON_RECEIVER_FULL, m_profile, data);
    }

    /*!
     * @if jp
     * @brief ON_SENDER_EMPTYのリスナへ通知する。 
     * @else
     * @brief Notify an ON_SENDER_EMPTY event to listeners
     * @endif
     */
    inline void onSenderEmpty()
    {
      m_listeners->notify(ConnectorListenerType::ON_SENDER_EMPTY, m_profile);
    }

    /*!
     * @if jp
     * @brief ON_SENDER_TIMEOUT のリスナへ通知する。 
     * @else
     * @brief Notify an ON_SENDER_TIMEOUT event to listeners
     * @endif
     */
    inline void onSenderTimeout()
    {
      m_listeners->notify(ConnectorListenerType::ON_SENDER_TIMEOUT, m_profile);
    }

    /*!
     * @if jp
     * @brief ON_SENDER_ERRORのリスナへ通知する。 
     * @else
     * @Brief Notify an ON_SENDER_ERROR event to listeners
     * @endif
     */
    inline void onSenderError()
    {
      m_listeners->notify(ConnectorListenerType::ON_SENDER_ERROR, m_profile);
    }
protected:
    mutable Logger rtclog;
    coil::Properties m_properties;
    std::mutex m_mutex;
    std::string m_shm_address;
    SharedMemoryPort m_shmem;
    int m_memory_size;
    bool m_endian;

    CdrBufferBase* m_buffer;
    ConnectorListenersBase* m_listeners{nullptr};
    ConnectorInfo m_profile;
  };
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * OutPortSHMConsumer のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers OutPortSHMConsumer's factory.
   *
   * @endif
   */
  void OutPortSHMConsumerInit(void);
}

#endif // RTC_OUTPORTSHMCONSUMER_H
