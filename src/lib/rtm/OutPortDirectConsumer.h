// -*- C++ -*-
/*!
 * @file  InPortDirectConsumer.cpp
 * @brief InPortDirectConsumer class
 * @date  $Date: 2008-01-14 07:49:59 $
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
 * $Id: InPortDirectConsumer.cpp 1244 2009-03-13 07:25:42Z n-ando $
 *
 */

#ifndef RTC_OUTPORTDIRECTCONSUMER_H
#define RTC_OUTPORTDIRECTCONSUMER_H


#include <rtm/Manager.h>
#include <rtm/OutPortConsumer.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @class OutPortDirectConsumer
   * @brief OutPortDirectConsumer クラス
   *
   * OutPortConsumer 
   *
   * データ転送に CORBA の OpenRTM::OutPortCdr インターフェースを利用し
   * た、pull 型データフロー型を実現する OutPort コンシューマクラス。
   *
   * @since 1.2.0
   *
   * @else
   * @class OutPortDirectConsumer
   * @brief OutPortDirectConsumer class
   *
   * The OutPort consumer class which uses the OpenRTM::OutPortCdr
   * interface in CORBA for data transfer and realizes a pull-type
   * dataflow.
   *
   * @since 1.2.0
   *
   * @endif
   */
  class OutPortDirectConsumer
    : public OutPortConsumer
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
    OutPortDirectConsumer();

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
    ~OutPortDirectConsumer() override;

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
    
private:
    mutable Logger rtclog;
    coil::Properties m_properties;
  };
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * OutPortDirectConsumer のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers OutPortDirectConsumer's factory.
   *
   * @endif
   */
  void OutPortDirectConsumerInit(void);
}

#endif // RTC_OUTPORTDIRECTCONSUMER_H
