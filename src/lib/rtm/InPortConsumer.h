// -*- C++ -*-
/*!
 * @file  InPortConsumer.h
 * @brief InPortConsumer class
 * @date  $Date: 2007-12-31 03:08:03 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
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

#ifndef RTC_INPORTCONSUMER_H
#define RTC_INPORTCONSUMER_H

#include <coil/Factory.h>
#include <rtm/DataPortStatus.h>
#include <rtm/ByteData.h>

namespace coil
{
  class Properties;
} // namespace coil

// Why RtORB does not allow forward declaration?
#if !defined ORB_IS_RTORB && !defined ORB_IS_ORBEXPRESS
namespace SDOPackage
{
  class NVList;
} // namespace SDOPackage
#endif  // ORB_IS_RTORB



namespace RTC
{
  /*!
   * @if jp
   *
   * @class InPortConsumer
   *
   * @brief InPortConsumer 抽象クラス
   *
   * 入力ポートコンシューマのための抽象インターフェースクラス
   * 各具象クラスは、以下の純粋仮想関数の実装を提供しなければならない。
   * - push(): データ送信
   * - clone(): ポートのコピー
   * - subscribeInterface(): データ送出通知への登録
   * - unsubscribeInterface(): データ送出通知の登録解除
   *
   * @since 0.4.0
   *
   * @else
   * @class InPortConsumer
   *
   * @brief InPortConsumer abstract class
   *
   * This is the abstract interface class for the input port Consumer.
   * Concrete classes must implement the following pure virtual functions.
   * - push(): Send data
   * - clone(): Copy ports
   * - subscribeInterface(): Subscribe the data send notification
   * - unsubscribeInterface(): Unsubscribe the data send notification
   *
   * @since 0.4.0
   *
   * @endif
   *
   */
  class InPortConsumer
  {
  public:

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
    virtual ~InPortConsumer() = default;

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * InPortConsumerの各種設定を行う
     *
     * @else
     * @brief Initializing configuration
     *
     * This operation would be called to configure this consumer
     * in initialization.
     *
     * @endif
     */
    virtual void init(coil::Properties& prop) = 0;

    /*!
     * @if jp
     * @brief 接続先へのデータ送信
     *
     * 接続先のポートへデータを送信するための純粋仮想関数。
     *
     * この関数は、以下のリターンコードを返す。
     *
     * - PORT_OK:         正常終了。
     * - PORT_ERROR:      データ送信の過程で何らかのエラーが発生した。
     * - SEND_FULL:       データを送信したが、相手側バッファがフルだった。
     * - SEND_TIMEOUT:    データを送信したが、相手側バッファがタイムアウトした。
     * - CONNECTION_LOST: 接続が切断された
     * - UNKNOWN_ERROR:   原因不明のエラー
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
     * - PORT_OK:         Normal return
     * - PORT_ERROR:      Error occurred in data transfer process
     * - SEND_FULL:       Buffer full although OutPort tried to send data
     * - SEND_TIMEOUT:    Timeout although OutPort tried to send data
     * - CONNECTION_LOST: Connection lost
     * - UNKNOWN_ERROR:   Unknown error
     *
     * @endif
     */
    virtual DataPortStatus put(ByteData& data) = 0;

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
    virtual void publishInterfaceProfile(SDOPackage::NVList& properties) = 0;

    /*!
     * @if jp
     * @brief データ送出通知受け取りへの登録
     *
     * 指定されたプロパティの内容に基づいて、データ送出通知の受け取りに登録する
     * ための純粋仮想関数。
     *
     * @param properties 登録時に参照するプロパティ
     *
     * @return 登録処理結果
     *
     * @else
     * @brief Subscribe the data send notification
     *
     * Pure virtual function to subscribe the data send notification
     * based on specified property information.
     *
     * @param properties Properties for reference when subscribing
     *
     * @return Subscription result
     *
     * @endif
     */
    virtual bool subscribeInterface(const SDOPackage::NVList& properties) = 0;

    /*!
     * @if jp
     * @brief データ送出通知受け取りからの登録解除
     *
     * データ送出通知の受け取りから登録解除するための純粋仮想関数。
     *
     * @param properties 登録解除時に参照するプロパティ
     *
     * @else
     * @brief Unsubscribe the data send notification
     *
     * Pure virtual function to unsubscribe the data send notification.
     *
     * @param properties Properties for reference when unsubscribing
     *
     * @endif
     */
    virtual void unsubscribeInterface(const SDOPackage::NVList& properties) = 0;

    /*!
     * @if jp
     * @brief インターフェースプロファイルを公開するたのファンクタ
     * @else
     * @brief Functor to publish interface profile
     * @endif
     */
    struct publishInterfaceProfileFunc
    {
      explicit publishInterfaceProfileFunc(SDOPackage::NVList& prop)
             : m_prop(prop) {}
      void operator()(InPortConsumer* consumer)
      {
        consumer->publishInterfaceProfile(m_prop);
      }
      SDOPackage::NVList& m_prop;
    };

    /*!
     * @if jp
     * @brief インターフェースプロファイルを公開するたのファンクタ
     * @else
     * @brief Functor to publish interface profile
     * @endif
     */
    struct subscribeInterfaceFunc
    {
      explicit subscribeInterfaceFunc(SDOPackage::NVList& prop)
             : m_prop(prop) {}
      bool operator()(InPortConsumer* consumer)
      {
        return consumer->subscribeInterface(m_prop);
      }
      SDOPackage::NVList& m_prop;
    };


  };

  using InPortConsumerFactory = ::coil::GlobalFactory<InPortConsumer>;
} // namespace RTC

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
EXTERN template class DLL_PLUGIN coil::GlobalFactory<RTC::InPortConsumer>;
#elif defined(__GNUC__)
EXTERN template class coil::GlobalFactory<RTC::InPortConsumer>;
#endif

#endif  // RTC_INPORTCONSUMER_H
