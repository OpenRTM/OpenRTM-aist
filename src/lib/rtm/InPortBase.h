// -*- C++ -*-
/*!
 * @file InPortBase.h
 * @brief RTC::Port implementation for InPort
 * @date $Date: 2008-01-13 15:06:40 $
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

#ifndef RTC_INPORTBASE_H
#define RTC_INPORTBASE_H

#include <rtm/PortBase.h>
#include <rtm/DataPortStatus.h>
#include <rtm/CdrBufferBase.h>
#include <rtm/ConnectorListener.h>
#include <rtm/OutPortBase.h>

/*!
 * @if jp
 * @namespace RTC
 *
 * @brief RTコンポーネント
 *
 * @else
 *
 * @namespace RTC
 *
 * @brief RT-Component
 *
 * @endif
 */
namespace RTC
{
  class InPortProvider;
  class OutPortConsumer;
  class InPortConnector;

  /*!
   * @if jp
   * @class InPortBase
   * @brief InPort 用 Port
   *
   * データ入力ポートの実装クラス。
   *
   * @since 0.4.0
   *
   * @else
   * @class InPortBase
   * @brief Port for InPort
   *
   * This is an implementation class for the data input port.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class InPortBase
    : public PortBase
  {
  public:

    using ConnectorList = std::vector<InPortConnector*>;

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param name ポート名称
     * @param data_type データタイプ
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @param name Port name
     * @param data_type Data type
     *
     * @endif
     */
    InPortBase(const char* name, const char* data_type);

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
    ~InPortBase() override;

    /*!
     * @if jp
     * @brief プロパティの初期化
     *
     * 指定されたプロパティで初期化する。
     *
     * @param prop 設定するプロパティ
     * @else
     * @brief Initializing properties
     *
     * This method initializes the port in the specified property.
     *
     * @param prop Property for setting ports
     * @endif
     */
    virtual void init(coil::Properties& prop);

    /*!
     * @if jp
     * @brief RTObject_impl::readAll()から呼ばれる仮想関数
     *
     * DataPort からデータを読み出す
     *
     * @return true:成功,false:失敗
     * @else
     * @brief It is a virtual method that is called from RTObject_impl::readAll().
     * This method reads out data from DataPort.
     *
     * @return true:Success,false:Failure
     * @endif
     */
    virtual bool read(std::string name="") = 0;

    /*!
     * @if jp
     * @brief プロパティを取得する
     *
     * ポートのプロパティを取得する。
     *
     * @return プロパティ
     * @else
     * @brief Get properties
     *
     * This method gets properties in the port.
     *
     * @return Properties
     * @endif
     */
    coil::Properties& properties();

    /*!
     * @if jp
     * @brief Connector を取得
     *
     * 現在所有しているコネクタを取得する。
     *
     * @return connector のリスト
     *
     * @else
     *
     * @brief Connector list
     *
     * This operation returns connector list
     *
     * @return connector list
     *
     * @endif
     */
    const std::vector<InPortConnector*>& connectors();

    /*!
     * @if jp
     * @brief ConnectorProfile を取得
     *
     * 現在所有しているコネクタのProfileを取得する。
     *
     * @return ConnectorProfile のリスト
     *
     * @else
     *
     * @brief ConnectorProfile list
     *
     * This operation returns ConnectorProfile list
     *
     * @return connector list
     *
     * @endif
     */
    ConnectorInfoList getConnectorProfiles();

    /*!
     * @if jp
     * @brief ConnectorId を取得
     *
     * 現在所有しているコネクタのIDを取得する。
     *
     * @return ConnectorId のリスト
     *
     * @else
     *
     * @brief ConnectorId list
     *
     * This operation returns ConnectorId list
     *
     * @return connector list
     *
     * @endif
     */
    coil::vstring getConnectorIds();

    /*!
     * @if jp
     * @brief Connectorの名前を取得
     *
     * 現在所有しているコネクタの名前を取得する。
     *
     * @return Connector名のリスト
     *
     * @else
     *
     * @brief Connector name list
     *
     * This operation returns Connector name list
     *
     * @return connector name list
     *
     * @endif
     */
    coil::vstring getConnectorNames();

    /*!
     * @if jp
     * @brief ConnectorProfileをIDで取得
     *
     * 現在所有しているコネクタをIDで取得する。
     *
     * @param id Connector ID
     * @return コネクタへのポインタ
     *
     * @else
     *
     * @brief Getting ConnectorProfile by ID
     *
     * This operation returns Connector specified by ID.
     *
     * @param id Connector ID
     * @return A pointer to connector
     *
     * @endif
     */
    InPortConnector* getConnectorById(const char* id);

     /*!
     * @if jp
     * @brief ConnectorProfileを名前で取得
     *
     * 現在所有しているコネクタを名前で取得する。
     *
     * @param name Connector name
     * @return コネクタへのポインタ
     *
     * @else
     *
     * @brief Getting Connector by name
     *
     * This operation returns Connector specified by name.
     *
     * @param id Connector ID
     * @return A pointer to connector
     *
     * @endif
     */
    InPortConnector* getConnectorByName(const char* name);

    /*!
     * @if jp
     * @brief ConnectorProfileをIDで取得
     *
     * 現在所有しているコネクタをIDで取得する。
     *
     * @param id Connector ID
     * @param prof ConnectorProfile
     * @return false 指定したIDがない
     *
     * @else
     *
     * @brief Getting ConnectorProfile by name
     *
     * This operation returns ConnectorProfile specified by name
     *
     * @param id Connector ID
     * @param prof ConnectorProfile
     * @return false　specified ID does not exist
     *
     * @endif
     */
    bool getConnectorProfileById(const char* id,
                                 ConnectorInfo& prof);

    /*!
     * @if jp
     * @brief ConnectorProfileを名前で取得
     *
     * 現在所有しているコネクタを名前で取得する。
     *
     * @param name Connector name
     * @param prof ConnectorProfile
     * @return false 指定した名前がない
     *
     * @else
     *
     * @brief Getting ConnectorProfile by name
     *
     * This operation returns ConnectorProfile specified by name
     *
     * @param id Connector ID
     * @param prof ConnectorProfile
     * @return false specified name does not exist
     *
     * @endif
     */
    bool getConnectorProfileByName(const char* name,
                                   ConnectorInfo& prof);


    /*!
     * @if jp
     *
     * @brief InPortを activates する
     *
     * Port に登録されている全てのインターフェースを activate する。
     *
     * @else
     *
     * @brief Activate all Port interfaces
     *
     * This operation activate all interfaces that is registered in the
     * ports.
     *
     * @endif
     */
    void activateInterfaces() override;

    /*!
     * @if jp
     *
     * @brief 全ての Port のインターフェースを deactivates する
     *
     * Port に登録されている全てのインターフェースを deactivate する。
     *
     * @else
     *
     * @brief Deactivate all Port interfaces
     *
     * This operation deactivate all interfaces that is registered in the
     * ports.
     *
     * @endif
     */
    void deactivateInterfaces() override;

    /*!
     * @if jp
     * @brief ConnectorDataListener リスナを追加する
     *
     * バッファ書き込みまたは読み出しイベントに関連する各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - ON_BUFFER_WRITE:          バッファ書き込み時
     * - ON_BUFFER_FULL:           バッファフル時
     * - ON_BUFFER_WRITE_TIMEOUT:  バッファ書き込みタイムアウト時
     * - ON_BUFFER_OVERWRITE:      バッファ上書き時
     * - ON_BUFFER_READ:           バッファ読み出し時
     * - ON_SEND:                  InProtへの送信時
     * - ON_RECEIVED:              InProtへの送信完了時
     * - ON_SEND_ERTIMEOUT:        OutPort側タイムアウト時
     * - ON_SEND_ERERROR:          OutPort側エラー時
     * - ON_RECEIVER_FULL:         InProt側バッファフル時
     * - ON_RECEIVER_TIMEOUT:      InProt側バッファタイムアウト時
     * - ON_RECEIVER_ERROR:        InProt側エラー時
     *
     * リスナは ConnectorDataListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * ConnectorDataListener::
     *         operator()(const ConnectorProfile&, const cdrStream&)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * OutPortに移り、OutPort解体時もしくは、
     * removeConnectorDataListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding BufferDataListener type listener
     *
     * This operation adds certain listeners related to buffer writing and
     * reading events.
     * The following listener types are available.
     *
     * - ON_BUFFER_WRITE:          At the time of buffer write
     * - ON_BUFFER_FULL:           At the time of buffer full
     * - ON_BUFFER_WRITE_TIMEOUT:  At the time of buffer write timeout
     * - ON_BUFFER_OVERWRITE:      At the time of buffer overwrite
     * - ON_BUFFER_READ:           At the time of buffer read
     * - ON_SEND:                  At the time of sending to InPort
     * - ON_RECEIVED:              At the time of finishing sending to InPort
     * - ON_SENDER_TIMEOUT:        At the time of timeout of OutPort
     * - ON_SENDER_ERROR:          At the time of error of OutPort
     * - ON_RECEIVER_FULL:         At the time of bufferfull of InPort
     * - ON_RECEIVER_TIMEOUT:      At the time of timeout of InPort
     * - ON_RECEIVER_ERROR:        At the time of error of InPort
     *
     * Listeners should have the following function operator().
     *
     * ConnectorDataListener::
     *         operator()(const ConnectorProfile&, const cdrStream&)
     *
     * The ownership of the given listener object is transferred to
     * this OutPort object in default.  The given listener object will
     * be destroied automatically in the OutPort's dtor or if the
     * listener is deleted by removeConnectorDataListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void addConnectorDataListener(ConnectorDataListenerType type,
                                  ConnectorDataListener* listener,
                                  bool autoclean = true);


    /*!
     * @if jp
     * @brief ConnectorDataListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing BufferDataListener type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void removeConnectorDataListener(ConnectorDataListenerType type,
                                     ConnectorDataListener* listener);


    /*!
     * @if jp
     * @brief ConnectorListener リスナを追加する
     *
     * バッファ書き込みまたは読み出しイベントに関連する各種リスナを設定する。
     *
     * 設定できるリスナのタイプは
     *
     * - ON_BUFFER_EMPTY:       バッファが空の場合
     * - ON_BUFFER_READTIMEOUT: バッファが空でタイムアウトした場合
     *
     * リスナは以下のシグニチャを持つ operator() を実装している必要がある。
     *
     * ConnectorListener::operator()(const ConnectorProfile&)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * OutPortに移り、OutPort解体時もしくは、
     * removeConnectorListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding ConnectorListener type listener
     *
     * This operation adds certain listeners related to buffer writing and
     * reading events.
     * The following listener types are available.
     *
     * - ON_BUFFER_EMPTY:       At the time of buffer empty
     * - ON_BUFFER_READTIMEOUT: At the time of buffer read timeout
     *
     * Listeners should have the following function operator().
     *
     * ConnectorListener::operator()(const ConnectorProfile&)
     *
     * The ownership of the given listener object is transferred to
     * this OutPort object in default.  The given listener object will
     * be destroied automatically in the OutPort's dtor or if the
     * listener is deleted by removeConnectorListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void addConnectorListener(ConnectorListenerType type,
                              ConnectorListener* listener,
                              bool autoclean = true);

    /*!
     * @if jp
     * @brief ConnectorDataListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing BufferDataListener type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void removeConnectorListener(ConnectorListenerType type,
                                 ConnectorListener* listener);

    /*!
     * @if jp
     * @brief endian 設定を返す
     *
     * endian 設定のbool値を返す。
     *
     * @return m_littleEndian がlittleの場合true、bigの場合false を返す。
     *
     * @else
     * @brief
     *
     * return it whether endian setting.
     *
     *@return Return true in the case of "little", false in "big" than it.
     *
     * @endif
     */
    bool isLittleEndian();

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Port の接続を行う
     *
     * 与えられた ConnectoionProfile の情報に基づき、Port間の接続を確立
     * する。この関数は主にアプリケーションプログラムやツールから呼び出
     * すことを前提としている。
     *
     * @param connector_profile ConnectorProfile
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Connect the Port
     *
     * This operation establishes connection according to the given
     * ConnectionProfile inforamtion. This function is premised on
     * calling from mainly application program or tools.
     *
     * @param connector_profile The ConnectorProfile.
     * @return ReturnCode_t The return code of ReturnCode_t type.
     *
     * @endif
     */
    ReturnCode_t
    connect(ConnectorProfile& connector_profile) override;

    /*!
     * @if jp
     * @brief リスナホルダを取得する
     *
     * InPortBaseが保持するリスナホルダを返す。
     *
     * @return ConnectorListeners
     *
     * @else
     * @brief Getting listeners holder
     *
     * This operation returns listeners holder.
     *
     * @return ConnectorListeners
     *
     * @endif
     */
    virtual ConnectorListenersBase* getListeners();
    ReturnCode_t notify_connect(ConnectorProfile& connector_profile) override;
  protected:
    /*!
     * @if jp
     * @brief Interface情報を公開する
     *
     * Interface情報を公開する。
     * 引数の ConnectorProfile に格納されている dataflow_type が push 型
     * の場合は、指定された interface_type の InPortProvider に関する情報
     * を ConnectorProfile::properties に書込み呼び出し側に戻す。
     *
     *  dataport.dataflow_type
     *
     * @param connector_profile コネクタプロファイル
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     * @brief Publish interface information
     *
     * Publish interface information.
     * Assign the Provider information that owned by this port
     * to ConnectorProfile#properties
     *
     * @param connector_profile The connector profile
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t
    publishInterfaces(ConnectorProfile& cprof) override;

    /*!
     * @if jp
     * @brief Interfaceに接続する
     *
     * Interfaceに接続する。
     * Portが所有するConsumerに適合するProviderに関する情報を
     * ConnectorProfile#properties から抽出し、
     * ConsumerにCORBAオブジェクト参照を設定する。
     *
     * @param connector_profile コネクタ・プロファイル
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     * @brief Subscribe to the interface
     *
     * Subscribe to interface.
     * Derive Provider information that matches Consumer owned by the Port
     * from ConnectorProfile#properties and
     * set the Consumer to the reference of the CORBA object.
     *
     * @param connector_profile The connector profile
     *
     * @return ReturnCode_t The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t
    subscribeInterfaces(const ConnectorProfile& cprof) override;

    /*!
     * @if jp
     * @brief Interfaceへの接続を解除する
     *
     * Interfaceへの接続を解除する。
     * 与えられたConnectorProfileに関連するConsumerに設定された全てのObjectを
     * 解放し接続を解除する。
     *
     * @param connector_profile コネクタ・プロファイル
     *
     * @else
     * @brief Disconnect the interface connection
     *
     * Disconnect the interface connection.
     * Release all objects set in Consumer associated with
     * given ConnectorProfile and unscribe the interface.
     *
     * @param connector_profile The connector profile
     *
     * @endif
     */
    void
    unsubscribeInterfaces(const ConnectorProfile& connector_profile) override;


    /*!
     * @if jp
     * @brief InPort provider の初期化
     * @else
     * @brief InPort provider initialization
     * @endif
     */
    void initProviders();

    /*!
     * @if jp
     * @brief OutPort consumer の初期化
     * @else
     * @brief OutPort consumer initialization
     * @endif
     */
    void initConsumers();

    /*!
     * @if jp
     * @brief シリアライザのエンディアンをチェックする
     *
     * 与えられたプロパティに設定されている、データのシリアライザのエン
     * ディアン指定をチェックする。正しいエンディアン指定がなされていれ
     * ば、true を返し、引数 littleEndian に、設定値がリトルエンディア
     * ンであれば true が、ビッグエンディアンであれば false が返される。
     *
     * @param prop チェックするプロパティ
     * @param littleEndian エンディアン情報（true:little,false:big）
     * @return true:"serializer"キーが存在しない または 存在していて内容がある。
     * ,false:"serializer"キーが存在しているが内容が空 または 存在しているが内容が"little","big" 以外。
     *
     * @else
     *
     * @brief Checking endian flag of serializer
     *
     * This operation checks endian flag of data serializer that is
     * specified properties. If valid specification is found, this
     * operation returns true and set argument littleEndian. True
     * means little endian, false means big endian.
     *
     * @param prop Properties
     * @param littleEndian Endian Information(true:little,false:big)
     * @return true:"Serializer" key doesn't exist. or  "Serializer" key exists and there is a content.
     *
     *false:There is no content though "Serializer" key exists. or ithe content is not "Little. " though "Serializer" key exists. or The content is not "little" or "big" though "Serializer" key exists.
     *
     * @endif
     */
    bool checkEndian(const coil::Properties& prop, bool& littleEndian);

    /*!
     * @if jp
     * @brief InPort provider の生成
     *
     * InPortProvider を生成し、情報を ConnectorProfile に公開する。
     * 生成に失敗した場合 0 を返す。
     *
     * @else
     * @brief InPort provider creation
     * @endif
     */
    InPortProvider*
    createProvider(ConnectorProfile& cprof, coil::Properties& prop);

    /*!
     * @if jp
     * @brief OutPort consumer の生成
     *
     * OutPortConsumer を生成する。
     * 生成に失敗した場合 0 を返す。
     *
     * @else
     * @brief InPort provider creation
     * @endif
     */
    OutPortConsumer*
    createConsumer(const ConnectorProfile& cprof, coil::Properties& prop);

    /*!
     * @if jp
     * @brief InPortPushConnector の生成
     *
     * Connector を生成し、生成が成功すれば m_connectors に保存する。
     * 生成に失敗した場合 0 を返す。
     *
     * @else
     * @brief InPortPushConnector creation
     * @endif
     */
    InPortConnector*
    createConnector(ConnectorProfile& cprof, coil::Properties& prop,
                    InPortProvider* provider);
    /*!
     * @if jp
     * @brief InPortPullConnector の生成
     *
     * Connector を生成し、生成が成功すれば m_connectors に保存する。
     * 生成に失敗した場合 0 を返す。
     *
     * @else
     * @brief InPortPullConnector creation
     * @endif
     */
    InPortConnector*
    createConnector(const ConnectorProfile& cprof, coil::Properties& prop,
                    OutPortConsumer* consumer);
    /*!
     * @if jp
     * @brief ローカルのピアOutPortを取得
     * @else
     * @brief Getting local peer OutPort if available
     * @endif
     */
    OutPortBase* getLocalOutPort(const ConnectorInfo& profile);

    /*!
     * @if jp
     *
     * @brief コネクタリスナの初期化 
     *
     * 
     *
     * @else
     *
     * @brief 
     *
     *
     * @endif
     */
    virtual void initConnectorListeners();
    /*!
     * @if jp
     * @brief バッファモード
     *
     * true:single buffer mode.
     * false:multi buffer mode.
     *
     * @else
     * @brief Buffer mode
     *
     * true:single buffer mode.
     * false:multi buffer mode.
     *
     * @endif
     */
    bool m_singlebuffer;
    /*!
     * @if jp
     * @brief バッファ
     * @else
     * @brief Buffer
     * @endif
     */
    CdrBufferBase* m_thebuffer;
    /*!
     * @if jp
     * @brief 利用可能provider
     * @else
     * @brief Available providers
     * @endif
     */
    coil::vstring m_providerTypes;
    /*!
     * @if jp
     * @brief 利用可能consumer
     * @else
     * @brief Available consumers
     * @endif
     */
    coil::vstring m_consumerTypes;
    /*!
     * @if jp
     * @brief 接続リスト
     * @else
     * @brief Connection list
     * @endif
     */
    ConnectorList m_connectors;
    /*!
     * @if jp
     * @brief 接続エンディアン
     * @else
     * @brief Connected Endian
     * @endif
     */
    bool m_littleEndian;

    /*!
     * @if jp
     * @brief ConnectorDataListener リスナ
     * @else
     * @brief ConnectorDataListener listener
     * @endif
     */
    ConnectorListenersBase* m_listeners;
  };
} // namespace RTC

#endif // RTC_INPORTBASE_H
