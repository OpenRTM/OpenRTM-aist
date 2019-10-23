// -*- C++ -*-
/*!
 * @file  InPortProvider.h
 * @brief InPortProvider class
 * @date  $Date: 2007-12-31 03:08:03 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
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

#ifndef RTC_INPORTPROVIDER_H
#define RTC_INPORTPROVIDER_H

#include <coil/Factory.h>

#include <rtm/BufferBase.h>
#include <rtm/NVUtil.h>
#include <rtm/SystemLogger.h>
#include <rtm/DataPortStatus.h>
#include <rtm/InPortConnector.h>

#include <string>

namespace RTC
{
  class ConnectorListeners;
  class ConnectorInfo;

  /*!
   * @if jp
   *
   * @class OutPortProvider
   * @brief OutPortProvider
   *
   * InPort の PROVIDED インターフェースを実装するための抽象基底クラス。
   * InPort に対して新しいインターフェースを実装する場合には、このクラ
   * スを継承し、以下の関数を実装する必要がある。
   *
   * - init()
   * - setBuffer()
   * - setListener()
   * - setConnector()
   *
   * さらに、コンストラクタ内で以下の関数を呼び、設定情報を初期化する必
   * 要がある。
   *
   * - setInterfaceType()
   * - setDataFlowType()
   * - setSubscriptionType()
   *
   * そのほか、InPortProvider のプロパティとして外部に公開する必要のあ
   * る値は、protected 変数 (SDOPackage::NVList) m_properties に対して
   * セットすること。セットされた値は、インターフェースのプロファイルと
   * して、また、接続時に他のインターフェースにこのインターフェースに関
   * する情報を与える際に利用される。以下の仮想関数は、ポートのインター
   * フェースプロファイル取得時および接続処理時にポートから呼び出される。
   * 予めセットされたこのインターフェースのプロファイル情報はこれらの関
   * 数呼び出しによりポートに伝えられる。
   *
   * - publishInterfaceProfile()
   * - publishInterface()
   *
   * InPort は InPortProvider のファクトリ管理クラスに対して利用可能
   * な InPortProvider を問合せ、提供可能なインターフェースタイプを外
   * 部に宣言する。従って、InPort　に対して PROVIDED インターフェース
   * を提供する InPortProvider のサブクラスは、InPortProviderFactory
   * にファクトリ関数を登録する必要がある。
   *
   * RTC::InPortProviderFactory::instance().addFactory() を、
   *
   * - 第1引数: プロバイダの名前, "corba_cdr" など
   * - 第2引数: ファクトリ関数 coil::Creator<B, T>
   *
   * を与えて呼び出す必要がある。以下は、ファクトリへの登録と、それを初
   * 期化関数とした例である。
   *
   * <pre>
   * extern "C"
   * {
   *   void InPortCorbaCdrProviderInit(void)
   *   {
   *     RTC::InPortProviderFactory&
   *                         factory(RTC::InPortProviderFactory::instance());
   *     factory.addFactory("corba_cdr",
   *                        ::coil::Creator<::RTC::InPortProvider,
   *                                        ::RTC::InPortCorbaCdrProvider>);
   *   }
   * };
   * </pre>
   *
   * この例のように、ファクトリへの登録を初期化関数として、extern "C"
   * によりシンボルを参照可能にしておく。こうすることで、
   * InPortProvider を共有オブジェクト化 (DLL化) して動的ロード可能に
   * し、プロバイダの型を動的に追加することが可能となる。
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class InPortProvider
   * @brief InPortProvider
   *
   * The virtual class for InPort's PROVIDED interface
   * implementation.  New interface for InPort have to inherit this
   * class, and have to implement the following functions.
   *
   * - init()
   * - setBuffer()
   * - setListener()
   * - setConnector()
   *
   * Moreover, calling the following functions in the constructor, and
   * properties have to be set.
   *
   * - setPortType()
   * - setDataType()
   * - setInterfaceType()
   * - setDataFlowType()
   * - setSubscriptionType()
   *
   * InPortProvider's properties that have to be provided to others
   * should be set to protected variable (SDOPackage::NVList)
   * m_properties. Values that are set to the property are published
   * as interface profile information, and it is also published to
   * required interface when connection is established.  The following
   * virtual functions are called when port's profiles are acquired
   * from others or connections are established. The following virtual
   * functions are called when port's profiles are acquired from
   * others or connections are established. Interface profile
   * information that is reviously set is given to Port calling by
   * these functions.
   *
   * - publishInterfaceProfile()
   * - publishInterface()
   *
   * InPort inquires available InPortProviders to the factory class
   * of InPortProvider, and publishes available interfaces to
   * others. Therefore, sub-classes of InPortProvider that provides
   * PROVIDED interface to InPort should register its factory to
   * InPortProviderFactory.
   *
   * RTC::InPortProviderFactory::instance().addFactory() would be
   * called with the following arguments.
   *
   * 1st arg: The name of provider. ex. "corba_cdr"
   * 2nd arg: Factory function. coil::Creator<B, T>
   *
   * The following example shows how to register factory function.
   * And it is also declared as a initialization function.
   *
   * <pre>
   * extern "C"
   * {
   *   void InPortCorbaCdrProviderInit(void)
   *   {
   *     RTC::InPortProviderFactory&
   *                         factory(RTC::InPortProviderFactory::instance());
   *     factory.addFactory("corba_cdr",
   *                        ::coil::Creator<::RTC::InPortProvider,
   *                                        ::RTC::InPortCorbaCdrProvider>);
   *   }
   * };
   * </pre>
   *
   * It is recommended that the registration process is declared as a
   * initialization function with "extern C" to be accessed from the
   * outside of module.  If the InPortProviders are compiled as a
   * shared object or DLL for dynamic loading, new InPortProvider
   * types can be added dynamically.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class InPortProvider
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
    InPortProvider();

    /*!
     * @if jp
     * @brief デストラクタ
     *
     * 仮想デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Virtual destructor
     *
     * @endif
     */
    virtual ~InPortProvider();

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * OutPortProvider の各種設定を行う。実装クラスでは、与えられた
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
     * OutPortProviderがデータを取り出すバッファをセットする。
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
    virtual void setBuffer(BufferBase<ByteData>* buffer) = 0;

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
     * @brief Connectorを設定する。
     *
     * OutPort は接続確立時に OutPortConnector オブジェクトを生成し、生
     * 成したオブジェクトのポインタと共にこの関数を呼び出す。所有権は
     * OutPort が保持するので OutPortProvider は OutPortConnector を削
     * 除してはいけない。
     *
     * @param connector OutPortConnector
     *
     * @else
     * @brief set Connector
     *
     * OutPort creates OutPortConnector object when it establishes
     * connection between OutPort and InPort, and it calls this
     * function with a pointer to the connector object. Since the
     * OutPort has the ownership of this connector, OutPortProvider
     * should not delete it.
     *
     * @param connector OutPortConnector
     *
     * @endif
     */
    virtual void setConnector(InPortConnector* connector) = 0;

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
    virtual void publishInterfaceProfile(SDOPackage::NVList& prop);

    /*!
     * @if jp
     * @brief Interface情報を公開する
     *
     * Interface情報を公開する。引数で指定するプロパティ情報内の
     * NameValue オブジェクトのdataport.interface_type 値を調べ、当該ポー
     * トに設定されていなければNameValue に情報を追加する。すでに同一イ
     * ンターフェースが登録済みの場合は何も行わない。
     *
     * @param properties Interface情報を受け取るプロパティ
     * @return true: 正常終了
     *
     * @else
     * @brief Publish interface information
     *
     * Publish interface information.  Check the
     * dataport.interface_type value of the NameValue object specified
     * by an argument in the property information, and add the
     * information to the NameValue if this port is not specified.
     * This does not do anything if the same interface is already
     * subscribed.
     *
     * @param properties Properties to receive interface information
     * @return true: normal return
     *
     * @endif
     */
    virtual bool publishInterface(SDOPackage::NVList& prop);

  protected:
    /*!
     * @if jp
     * @brief インタフェースタイプを設定する
     *
     * インタフェースタイプを設定する。
     *
     * @param interface_type 設定対象インタフェースタイプ
     *
     * @else
     * @brief Set the interface type
     *
     * Set the interface type.
     *
     * @param interface_type The target interface type for set
     *
     * @endif
     */
    void setInterfaceType(const char* interface_type);

    /*!
     * @if jp
     * @brief データフロータイプを設定する
     *
     * データフロータイプを設定する。
     *
     * @param dataflow_type 設定対象データフロータイプ
     *
     * @else
     * @brief Set the dataflow type
     *
     * Set the dataflow type.
     *
     * @param dataflow_type The target data flow type for set
     *
     * @endif
     */
    void setDataFlowType(const char* dataflow_type);

    /*!
     * @if jp
     * @brief サブスクリプションタイプを設定する
     *
     * サブスクリプションタイプを設定する。
     *
     * @param subs_type 設定対象サブスクリプションタイプ
     *
     * @else
     * @brief Set the subscription type
     *
     * Set the subscription type.
     *
     * @param subs_type The target subscription type for set
     *
     * @endif
     */
    void setSubscriptionType(const char* subs_type);


  protected:
    /*!
     * @if jp
     * @brief ポートプロファイルを保持するプロパティ
     * @else
     * @brief Properties to hold port profile
     * @endif
     */
    SDOPackage::NVList m_properties;

    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    mutable Logger rtclog;

  private:
    std::string m_interfaceType;
    std::string m_dataflowType;
    std::string m_subscriptionType;

  public:
    // functors
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
      void operator()(InPortProvider* provider)
      {
        provider->publishInterfaceProfile(m_prop);
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
    struct publishInterfaceFunc
    {
      explicit publishInterfaceFunc(SDOPackage::NVList& prop)
        : m_prop(prop), provider_(nullptr) {}
      void operator()(InPortProvider* provider)
      {
        if (provider->publishInterface(m_prop))
          {
            provider_ = provider;
          }
      }
      SDOPackage::NVList& m_prop;
      InPortProvider* provider_;
    };


  };

  /*!
   * @if jp
   * @brief InPortProviderFactory型宣言
   * @else
   * @brief InPortProviderFactory type definition
   * @endif
   */
  using InPortProviderFactory = ::coil::GlobalFactory<InPortProvider>;
} // namespace RTC

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
EXTERN template class DLL_PLUGIN coil::GlobalFactory<RTC::InPortProvider>;
#elif defined(__GNUC__)
EXTERN template class coil::GlobalFactory<RTC::InPortProvider>;
#endif

#endif  // RTC_INPORTPROVIDER_H
