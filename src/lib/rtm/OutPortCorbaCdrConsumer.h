// -*- C++ -*-
/*!
 * @file  OutPortCorbaCdrConsumer.h
 * @brief OutPortCorbaCdrConsumer class
 * @date  $Date: 2008-01-13 10:28:27 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009-2010
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_OUTPORTCORBACDRCONSUMER_H
#define RTC_OUTPORTCORBACDRCONSUMER_H

#include <rtm/idl/DataPort_OpenRTMSkel.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/OutPortConsumer.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @class OutPortCorbaCdrConsumer
   * @brief OutPortCorbaCdrConsumer クラス
   *
   * OutPortConsumer
   *
   * データ転送に CORBA の OpenRTM::OutPortCdr インターフェースを利用し
   * た、pull 型データフロー型を実現する OutPort コンシューマクラス。
   *
   * @since 0.4.0
   *
   * @else
   * @class OutPortCorbaCdrConsumer
   * @brief OutPortCorbaCdrConsumer class
   *
   * The OutPort consumer class which uses the OpenRTM::OutPortCdr
   * interface in CORBA for data transfer and realizes a pull-type
   * dataflow.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class OutPortCorbaCdrConsumer
    : public OutPortConsumer,
      public CorbaConsumer< ::OpenRTM::OutPortCdr >
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
    OutPortCorbaCdrConsumer();

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
    ~OutPortCorbaCdrConsumer() override;

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * OutPortConsumerの各種設定を行う。実装クラスでは、与えられた
     * Propertiesから必要な情報を取得して各種設定を行う。この init() 関
     * 数は、OutPortProvider生成直後および、接続時にそれぞれ呼ばれる可
     * 能性がある。したがって、この関数は複数回呼ばれることを想定して記
     * 述されるべきである。
     *
     * @param prop 設定情報
     *
     * @else
     *
     * @brief Initializing configuration
     *
     * This operation would be called to configure in initialization.
     * In the concrete class, configuration should be performed
     * getting appropriate information from the given Properties data.
     * This function might be called right after instantiation and
     * connection sequence respectivly.  Therefore, this function
     * should be implemented assuming multiple call.
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
     * OutPortConsumerがデータを取り出すバッファをセットする。
     * すでにセットされたバッファがある場合、以前のバッファへの
     * ポインタに対して上書きされる。
     * OutPortProviderはバッファの所有権を仮定していないので、
     * バッファの削除はユーザの責任で行わなければならない。
     *
     * @param buffer OutPortProviderがデータを取り出すバッファへのポインタ
     *
     * @else
     * @brief Setting outside buffer's pointer
     *
     * A pointer to a buffer from which OutPortProvider retrieve data.
     * If already buffer is set, previous buffer's pointer will be
     * overwritten by the given pointer to a buffer.  Since
     * OutPortProvider does not assume ownership of the buffer
     * pointer, destructor of the buffer should be done by user.
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
     * InPort はデータ送信処理における各種イベントに対して特定のリスナ
     * オブジェクトをコールするコールバック機構を提供する。詳細は
     * ConnectorListener.h の ConnectorDataListener, ConnectorListener
     * 等を参照のこと。OutPortCorbaCdrProvider では、以下のコールバック
     * が提供される。
     *
     * - ON_BUFFER_WRITE
     * - ON_BUFFER_FULL
     * - ON_RECEIVED
     * - ON_RECEIVER_FULL
     * - ON_SENDER_EMPTY
     * - ON_SENDER_TIMEOUT
     * - ON_SENDER_ERROR
     *
     * @param info 接続情報
     * @param listeners リスナオブジェクト
     *
     * @else
     * @brief Set the listener.
     *
     * OutPort provides callback functionality that calls specific
     * listener objects according to the events in the data publishing
     * process. For details, see documentation of
     * ConnectorDataListener class and ConnectorListener class in
     * ConnectorListener.h. In this OutPortCorbaCdrProvider provides
     * the following callbacks.
     *
     * - ON_BUFFER_WRITE
     * - ON_BUFFER_FULL
     * - ON_RECEIVED
     * - ON_RECEIVER_FULL
     * - ON_SENDER_EMPTY
     * - ON_SENDER_TIMEOUT
     * - ON_SENDER_ERROR
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
  protected:
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    mutable Logger rtclog;
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
    CdrBufferBase* m_buffer;
    ConnectorListenersBase* m_listeners;
    ConnectorInfo m_profile;
  };
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * OutPortCorbaCdrConsumer のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers OutPortCorbaCdrConsumer's factory.
   *
   * @endif
   */
  void OutPortCorbaCdrConsumerInit(void);
}

#endif  // RTC_OUTPORTCORBACDRCONSUMER_H
