// -*- C++ -*-
/*!
 * @file  InPortDSConsumer.h
 * @brief InPortDSConsumer class
 * @date  $Date: 2018-09-20 07:49:59 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_INPORTDSCONSUMER_H
#define RTC_INPORTDSCONSUMER_H



#include <rtm/idl/DataPortSkel.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/InPortConsumer.h>
#include <rtm/Manager.h>

namespace RTC
{
  /*!
   * @if jp
   * @class InPortDSConsumer
   * @brief InPortDSConsumer クラス
   *
   * InPortConsumer
   *
   * データ転送に CORBA の RTC::DataPushService インターフェースを利用し
   * た、push 型データフロー型を実現する InPort コンシューマクラス。
   *
   * @since 2.0.0
   *
   * @else
   * @class InPortDSConsumer
   * @brief InPortDSConsumer class
   *
   * The InPort consumer class which uses the RTC::DataPushService
   * interface in CORBA for data transfer and realizes a push-type
   * dataflow.
   *
   * @since 2.0.0
   *
   * @endif
   */
  class InPortDSConsumer
    : public InPortConsumer,
      public CorbaConsumer< ::RTC::DataPushService >
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param buffer 当該コンシューマに割り当てるバッファオブジェクト
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @param buffer The buffer object that is attached to this Consumer
     *
     * @endif
     */
    InPortDSConsumer();

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
    ~InPortDSConsumer() override;

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * InPortConsumerの各種設定を行う。実装クラスでは、与えられた
     * Propertiesから必要な情報を取得して各種設定を行う。この init() 関
     * 数は、InPortProvider生成直後および、接続時にそれぞれ呼ばれる可
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
     * @brief 接続先へのデータ送信
     *
     * 接続先のポートへデータを送信するための純粋仮想関数。
     *
     * この関数は、以下のリターンコードを返す。
     *
     * - PORT_OK:       正常終了。
     * - PORT_ERROR:    データ送信の過程で何らかのエラーが発生した。
     * - SEND_FULL:     データを送信したが、相手側バッファがフルだった。
     * - SEND_TIMEOUT:  データを送信したが、相手側バッファがタイムアウトした。
     * - UNKNOWN_ERROR: 原因不明のエラー
     *
     * @param data 送信するデータ
     * @return リターンコード
     *
     * @else
     * @brief Send data to the destination port
     *
     * Pure virtual function to send data to the destination port.
     *
     * This function might the following return codes
     *
     * - PORT_OK:       Normal return
     * - PORT_ERROR:    Error occurred in data transfer process
     * - SEND_FULL:     Buffer full although OutPort tried to send data
     * - SEND_TIMEOUT:  Timeout although OutPort tried to send data
     * - UNKNOWN_ERROR: Unknown error
     *
     * @endif
     */
    DataPortStatus put(ByteData& data) override;

    /*!
     * @if jp
     * @brief InterfaceProfile情報を公開する
     *
     * InterfaceProfile情報を公開する。
     * 引数で指定するプロパティ情報内の NameValue オブジェクトの
     * dataport.interface_type 値を調べ、当該ポートに設定されている
     * インターフェースタイプと一致する場合のみ情報を取得する。
     *
     * @param properties InterfaceProfile情報を受け取るプロパティ
     *
     * @else
     * @brief Publish InterfaceProfile information
     *
     * Publish interfaceProfile information.
     * Check the dataport.interface_type value of the NameValue object
     * specified by an argument in property information and get information
     * only when the interface type of the specified port is matched.
     *
     * @param properties Properties to get InterfaceProfile information
     *
     * @endif
     */
    void publishInterfaceProfile(SDOPackage::NVList& properties) override;

    /*!
     * @if jp
     * @brief データ送信通知への登録
     *
     * 指定されたプロパティに基づいて、データ送出通知の受け取りに登録する。
     *
     * @param properties 登録情報
     *
     * @return 登録処理結果(登録成功:true、登録失敗:false)
     *
     * @else
     * @brief Subscribe to the data sending notification
     *
     * Subscribe to the data sending notification based on specified
     * property information.
     *
     * @param properties Information for subscription
     *
     * @return Subscription result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool subscribeInterface(const SDOPackage::NVList& properties) override;

    /*!
     * @if jp
     * @brief データ送信通知からの登録解除
     *
     * データ送出通知の受け取りから登録を解除する。
     *
     * @param properties 登録解除情報
     *
     * @else
     * @brief Unsubscribe the data send notification
     *
     * Unsubscribe the data send notification.
     *
     * @param properties Information for unsubscription
     *
     * @endif
     */
    void unsubscribeInterface(const SDOPackage::NVList& properties) override;

  private:
    /*!
     * @if jp
     * @brief IOR文字列からオブジェクト参照を取得する
     *
     * @return true: 正常取得, false: 取得失敗
     *
     * @else
     * @brief Getting object reference fromn IOR string
     *
     * @return true: succeeded, false: failed
     *
     * @endif
     */
    bool subscribeFromIor(const SDOPackage::NVList& properties);

    /*!
     * @if jp
     * @brief Anyから直接オブジェクト参照を取得する
     *
     * @return true: 正常取得, false: 取得失敗
     *
     * @else
     * @brief Getting object reference fromn Any directry
     *
     * @return true: succeeded, false: failed
     *
     * @endif
     */
    bool subscribeFromRef(const SDOPackage::NVList& properties);

    /*!
     * @if jp
     * @brief 接続解除(IOR版)
     *
     * @return true: 正常取得, false: 取得失敗
     *
     * @else
     * @brief ubsubscribing (IOR version)
     *
     * @return true: succeeded, false: failed
     *
     * @endif
     */
    bool unsubscribeFromIor(const SDOPackage::NVList& properties);

    /*!
     * @if jp
     * @brief 接続解除(Object reference版)
     *
     * @return true: 正常取得, false: 取得失敗
     *
     * @else
     * @brief ubsubscribing (Object reference version)
     *
     * @return true: succeeded, false: failed
     *
     * @endif
     */
    bool unsubscribeFromRef(const SDOPackage::NVList& properties);

  private:
    /*!
     * @if jp
     * @brief リターンコード変換
     * @else
     * @brief Return codes conversion
     * @endif
     */
    static DataPortStatus convertReturnCode(RTC::PortStatus ret);

    mutable Logger rtclog;
    coil::Properties m_properties;
    ::RTC::OctetSeq m_data;
  };
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * InPortDSConsumer のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers InPortDSConsumer's factory.
   *
   * @endif
   */
  void InPortDSConsumerInit(void);
}

#endif  // RTC_INPORTDSCONSUMER_H

