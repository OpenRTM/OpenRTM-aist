// -*- C++ -*-
/*!
 * @file  OutPortConsumer.h
 * @brief OutPortConsumer class
 * @date  $Date: 2007-12-31 03:08:05 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2009
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

#ifndef RTC_OUTPORTCONSUMER_H
#define RTC_OUTPORTCONSUMER_H

#include <coil/Factory.h>
#include <rtm/DataPortStatus.h>
#include <rtm/CdrBufferBase.h>
#include <rtm/SystemLogger.h>

// Why RtORB does not allow the following foward declaration?
#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS)
namespace SDOPackage
{
  class NVList;
} // namespace SDOPackage
#endif  // ORB_IS_RTORB

namespace coil
{
  class Properties;
} // namespace coil

namespace RTC
{
  class ConnectorListenersBase;
  class ConnectorInfo;

  /*!
   * @if jp
   *
   * @class OutPortConsumer
   *
   * @brief OutPortConsumer 抽象クラス
   *
   * OutPort の REQUIRED インターフェースを実装するための抽象基底クラス。
   * このサブクラスのオブジェクトは InPort に属し、pull 型のデータスト
   * リームを実現する。InPort に対して新しいインターフェースを実装する
   * 場合には、このクラスを継承し、以下の関数を実装する必要がある。
   *
   * - init()
   * - setBuffer()
   * - setListener()
   * - get()
   *
   * さらに、以下の仮想関数に、ConnectorProfile から必要とする情報を取
   * 得するなど、接続を確立あるいは接続の切断を実行するために必要な処理
   * を実装しなければならない。
   *
   * - subscribeInterface()
   * - unsubscribeInterface()
   *
   * InPort は OutPortConsumer のファクトリ管理クラスに対して利用可能
   * な OutPortConsumer を問合せ、提供可能なインターフェースタイプを外
   * 部に宣言する。従って、InPort　に対して REQUIRED インターフェース
   * を提供する OutPortConsumer のサブクラスは、OutPortConsumerFactory
   * にファクトリ関数を登録する必要がある。
   *
   * RTC::OutPortConsumerFactory::instance().addFactory() を、
   *
   * - 第1引数: プロバイダの名前, "corba_cdr" など
   * - 第2引数: ファクトリ関数 coil::Creator<B, T>
   * - 第3引数: 削除関数 coil::Destructor<B, T>
   *
   * を与えて呼び出す必要がある。以下は、ファクトリへの登録と、それを初
   * 期化関数とした例である。
   *
   * <pre>
   * extern "C"
   * {
   *   void OutPortCorbaCdrConsumerInit(void)
   *   {
   *     RTC::OutPortConsumerFactory&
   *                         factory(RTC::OutPortConsumerFactory::instance());
   *     factory.addFactory("corba_cdr",
   *                        ::coil::Creator<::RTC::OutPortConsumer,
   *                                        ::RTC::OutPortCorbaCdrConsumer>,
   *                        ::coil::Destructor<::RTC::OutPortConsumer,
   *                                           ::RTC::OutPortCorbaCdrConsumer>);
   *   }
   * };
   * </pre>
   *
   * この例のように、ファクトリへの登録を初期化関数として、extern "C"
   * によりシンボルを参照可能にしておく。こうすることで、
   * OutPortConsumer を共有オブジェクト化 (DLL化) して動的ロード可能に
   * し、プロバイダの型を動的に追加することが可能となる。
   *
   * @since 0.4.0
   *
   * @else
   * @class OutPortConsumer
   *
   * @brief OutPortConsumer abstract class
   *
   * The virtual class for OutPort's PROVIDED interface
   * implementation.  New interface for OutPort have to inherit this
   * class, and have to implement the following functions.
   *
   * - init()
   * - setBuffer()
   * - setListener()
   * - get()
   *
   * Furthermore, connecting or disconnecting processes, such as
   * obtaining some information from ConnectorProfile or releasing
   * some resources, should be implemented in the following virtual
   * functions.
   *
   * - subscribeInterface()
   * - unsubscribeInterface()
   *
   * InPort inquires available OutPortConsumers to the factory class
   * of OutPortConsumer, and publishes available interfaces to
   * others. Therefore, sub-classes of OutPortConsumer that provides
   * PROVIDED interface to OutPort should register its factory to
   * OutPortConsumerFactory.
   *
   * RTC::OutPortConsumerFactory::instance().addFactory() would be
   * called with the following arguments.
   *
   * 1st arg: The name of provider. ex. "corba_cdr"
   * 2nd arg: Factory function. coil::Creator<B, T>
   * 3rd arg: Destruction function. coil::Destructor<B, T>
   *
   * The following example shows how to register factory function.
   * And it is also declared as a initialization function.
   *
   * <pre>
   * extern "C"
   * {
   *   void OutPortCorbaCdrConsumerInit(void)
   *   {
   *     RTC::OutPortConsumerFactory&
   *                         factory(RTC::OutPortConsumerFactory::instance());
   *     factory.addFactory("corba_cdr",
   *                        ::coil::Creator<::RTC::OutPortConsumer,
   *                                        ::RTC::OutPortCorbaCdrConsumer>,
   *                        ::coil::Destructor<::RTC::OutPortConsumer,
   *                                           ::RTC::OutPortCorbaCdrConsumer>);
   *   }
   * };
   * </pre>
   *
   * It is recommended that the registration process is declared as a
   * initialization function with "extern C" to be accessed from the
   * outside of module.  If the OutPortConsumers are compiled as a
   * shared object or DLL for dynamic loading, new OutPortConsumer
   * types can be added dynamically.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class OutPortConsumer
  {
  public:

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * 仮想デストラクタ。
     *
     * @else
     * @brief Destructor
     *
     * Virtual destructor
     *
     * @endif
     */
    virtual ~OutPortConsumer() = default;

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
    virtual void init(coil::Properties& prop) = 0;

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
    virtual void setBuffer(CdrBufferBase* buffer) = 0;

    /*!
     * @if jp
     * @brief リスナを設定する。
     *
     * OutPort はデータ送信処理における各種イベントに対して特定のリスナ
     * オブジェクトをコールするコールバック機構を提供する。詳細は
     * ConnectorListener.h の ConnectorDataListener, ConnectorListener
     * 等を参照のこと。OutPortProvider のサブクラスでは、与えられたリス
     * ナを適切なタイミングで呼び出すべきである。ただし、すべてのリスナ
     * を呼び出す必要はない。
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
     * ConnectorListener.h. In the sub-classes of OutPortProvider, the
     * given listeners should be called in the proper timing. However,
     * it is not necessary to call all the listeners.
     *
     * @param info Connector information
     * @param listeners Listener objects
     *
     * @endif
     */
    virtual void setListener(ConnectorInfo& info,
                             ConnectorListenersBase* listeners) = 0;

    /*!
     * @if jp
     *
     * @brief データを受信する
     *
     * データ受信を実行するための純粋仮想関数。
     * 具象クラスでは、それぞれの方法でリモートのOutPortからデータを
     * 受信するロジックを実装する。
     * 受信に関する状態に応じて以下の戻り値を返す。
     *
     * @param data 受信データ
     * @return PORT_OK         正常終了
     *         BUFFER_TIMEOUT  タイムアウトした
     *         RECV_EMPTY      取得先のバッファが空である。
     *         CONNECTION_LOST 接続が切断された
     *         PORT_ERROR      エラー
     *         UNKNOWN_ERROR   本来ありえないエラー
     *
     * @else
     *
     * @brief Receive data
     *
     * Pure virtual function to receive data.
     *
     * @endif
     */
    virtual DataPortStatus get(ByteData& data) = 0;

    /*!
     * @if jp
     *
     * @brief データ受信通知への登録
     *
     * 指定されたプロパティ情報に基づいて、データ受信通知に登録するための
     * 純粋仮想関数。
     *
     * @param properties 登録用プロパティ
     *
     * @return 登録処理結果(登録成功:true、登録失敗:false)
     *
     * @else
     *
     * @brief Subscribe the data receive notification
     *
     * Pure virtual function to subscribe the data receive notification
     * based on specified property information.
     *
     * @param properties Properties for subscription
     *
     * @return Subscription result (Successful:true, Failed:false)
     *
     * @endif
     */
    virtual bool subscribeInterface(const SDOPackage::NVList& properties) = 0;

    /*!
     * @if jp
     *
     * @brief データ受信通知からの登録解除
     *
     * データ受信通知からの登録を解除するための純粋仮想関数。
     *
     * @param properties 登録解除用プロパティ
     *
     * @return 登録解除処理結果(登録解除成功:true、登録解除失敗:false)
     *
     * @else
     *
     * @brief Unsubscribe the data receive notification
     *
     * Pure virtual function to unsubscribe the data receive notification.
     *
     * @param properties Properties for unsubscription
     *
     * @return Unsubscription result (Successful:true, Failed:false)
     *
     * @endif
     */
    virtual void unsubscribeInterface(const SDOPackage::NVList& properties) = 0;

  protected:

    /*!
     * @if jp
     * @brief Interface接続用Functor
     * @else
     * @brief Functor to subscribe the interface
     * @endif
     */
    struct subscribe
    {
      explicit subscribe(const SDOPackage::NVList& prop) : m_prop(prop) {}
      void operator()(OutPortConsumer* consumer)
      {
        consumer->subscribeInterface(m_prop);
      }
      const SDOPackage::NVList& m_prop;
    };

    /*!
     * @if jp
     * @brief Interface接続解除用Functor
     * @else
     * @brief Functor to unsubscribe the interface
     * @endif
     */
    struct unsubscribe
    {
      explicit unsubscribe(const SDOPackage::NVList& prop) : m_prop(prop) {}
      void operator()(OutPortConsumer* consumer)
      {
        consumer->unsubscribeInterface(m_prop);
      }
      const SDOPackage::NVList& m_prop;
    };
  };

  /*!
   * @if jp
   * @brief OutPortConsumerFactory型宣言
   * @else
   * @brief OutPortConsumerFactory type definition
   * @endif
   */
  using OutPortConsumerFactory = ::coil::GlobalFactory<OutPortConsumer>;
} // namespace RTC


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
EXTERN template class DLL_PLUGIN coil::GlobalFactory<RTC::OutPortConsumer>;
#elif defined(__GNUC__)
EXTERN template class coil::GlobalFactory<RTC::OutPortConsumer>;
#endif

#endif  // RTC_OUTPORTCONSUMER_H

