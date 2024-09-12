// -*- C++ -*-
/*!
 * @file PortBase.h
 * @brief RTC's Port base class
 * @date $Date: 2008-01-14 07:56:44 $
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

#ifndef RTC_PORTBASE_H
#define RTC_PORTBASE_H

#include <rtm/RTC.h>

#include <mutex>
#include <rtm/idl/RTCSkel.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/NVUtil.h>
#include <rtm/SystemLogger.h>
#include <rtm/PortConnectListener.h>

#include <iostream>
#include <rtm/DirectPortBase.h>

namespace RTC
{
  class ConnectionCallback;

  /*!
   * @if jp
   * @class PortBase
   * @brief Port の基底クラス
   *
   * RTC::Port の基底となるクラス。
   * RTC::Port はほぼ UML Port の概念を継承しており、ほぼ同等のものとみなす
   * ことができる。RT コンポーネントのコンセプトにおいては、
   * Port はコンポーネントに付属し、コンポーネントが他のコンポーネントと相互作用
   * を行う接点であり、通常幾つかのインターフェースと関連付けられる。
   * コンポーネントは Port を通して外部に対しインターフェースを提供または要求
   * することができ、Portはその接続を管理する役割を担う。
   * <p>
   * Port の具象クラスは、通常 RT コンポーネントインスタンス生成時に同時に
   * 生成され、提供・要求インターフェースを登録した後、RT コンポーネントに
   * 登録され、外部からアクセス可能な Port として機能することを想定している。
   * <p>
   * RTC::Port は CORBA インターフェースとして以下のオペレーションを提供する。
   *
   * - get_port_profile()
   * - get_connector_profiles()
   * - get_connector_profile()
   * - connect()
   * - notify_connect()
   * - disconnect()
   * - notify_disconnect()
   * - disconnect_all()
   *
   * このクラスでは、これらのオペレーションの実装を提供する。
   * <p>
   * これらのオペレーションのうち、get_port_profile(), get_connector_profiles(),
   * get_connector_profile(), connect(), disconnect(), disconnect_all() は、
   * サブクラスにおいて特に振る舞いを変更する必要がないため、オーバーライド
   * することは推奨されない。
   * <p>
   * notify_connect(), notify_disconnect() については、サブクラスが提供・要求
   * するインターフェースの種類に応じて、振る舞いを変更する必要が生ずる
   * 可能性があるが、これらを直接オーバーライドすることは推奨されず、
   * 後述の notify_connect(), notify_disconnect() の項においても述べられる通り
   * これらの関数に関連した protected 関数をオーバーライドすることにより
   * 振る舞いを変更することが推奨される。
   *
   * @since 0.4.0
   *
   * @else
   * @class PortBase
   * @brief Port base class
   *
   * This class is a base class of RTC::Port.
   * RTC::Port inherits a concept of RT-Component, and can be regarded as almost
   * the same as it. In the concept of RT-Component, Port is attached to the
   * component, can mediate interaction between other components and usually is
   * associated with some interfaces.
   * Component can provide or require interface for outside via Port, and the
   * Port plays a role to manage the connection.
   * <p>
   * Concrete class of Port assumes to be usually created at the same time that
   * RT-Component's instance is created, be registerd to RT-Component after
   * provided and required interfaces are registerd, and function as accessible
   * Port from outside.
   * <p>
   * RTC::Port provides the following operations as CORBA interface:
   *
   * - get_port_profile()
   * - get_connector_profiles()
   * - get_connector_profile()
   * - connect()
   * - notify_connect()
   * - disconnect()
   * - notify_disconnect()
   * - disconnect_all()
   *
   * This class provides implementations of these operations.
   * <p>
   * In these operations, as for get_port_profile(), get_connector_profiles(),
   * get_connector_profile(), connect(), disconnect() and disconnect_all(),
   * since their behaviors especially need not to be change in subclass,
   * overriding is not recommended.
   * <p>
   * As for notify_connect() and notify_disconnect(), you may have to modify
   * behavior according to the kind of interfaces that subclass provides and
   * requires, however it is not recommended these are overriden directly.
   * In the section of notify_connect() and notify_disconnect() as described
   * below, it is recommended that you modify behavior by overriding the
   * protected function related to these functions.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class PortBase
    : public virtual POA_RTC::PortService,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * PortBase のコンストラクタは Port 名 name を引数に取り初期化を行う
     * と同時に、自分自身を CORBA Object として活性化し、自身の PortProfile
     * の port_ref に自身のオブジェクトリファレンスを格納する。
     * 名前には、"." 以外の文字列を使用することができる。
     *
     * @param name Port の名前(デフォルト値:"")
     *
     * @else
     *
     * @brief Constructor
     *
     * The constructor of the ProtBase class is given the name of this Port
     * and initialized. At the same time, the PortBase activates itself
     * as CORBA object and stores its object reference to the PortProfile's
     * port_ref member.
     * Characters except "." can be used for the name of the port.
     *
     * @param name The name of Port (The default value:"")
     *
     * @endif
     */
    explicit PortBase(const char* name = "");

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * デストラクタでは、PortService CORBA オブジェクトの deactivate を
     * 行う。deactivateに際して例外を投げることはない。
     *
     * @else
     *
     * @brief Destructor
     *
     * In the destructor, PortService CORBA object is deactivated.
     * This function never throws exception.
     *
     * @endif
     */
    ~PortBase() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] PortProfileを取得する
     *
     * Portが保持するPortProfileを返す。この関数は CORBA オペレーション
     * であり、CORBA のメモリ管理規則に従い、呼び出し側は返される
     * PortProfile オブジェクトを解体する責任がある。PortProfile 構造体
     * は以下のメンバーを持つ。
     *
     * - name              [string 型] Port の名前。
     * - interfaces        [PortInterfaceProfileList 型] Port が保持する
     *                     PortInterfaceProfile のシーケンス
     * - port_ref          [Port Object 型] Port 自身のオブジェクトリファレンス
     * - connector_profile [ConnectorProfileList 型] Port が現在保持する
     *                     ConnectorProfile のシーケンス
     * - owner             [RTObject Object 型] この Port を所有する
     *                     RTObjectのリファレンス
     * - properties        [NVList 型] その他のプロパティ。
     *
     * @post この関数を呼び出すことにより内部状態が変更されることはない。
     *
     * @return PortProfile
     *
     * @else
     *
     * @brief [CORBA interface] Get the PortProfile of the Port
     *
     * This operation returns the PortProfile of the Port. Since this
     * function is CORBA operation, callers have responsibility to
     * destruction of the returned PortProfile object according to the
     * CORBA memory management rules.
     *
     * PortProfile struct has the following members:
     *
     * - name              [string type] The name of the Port.
     * - interfaces        [PortInterfaceProfileList type] The sequence of
     *                     PortInterfaceProfile owned by the Port
     * - port_ref          [Port Object type] The object reference of the Port.
     * - connector_profile [ConnectorProfileList type] The sequence of
     *                     ConnectorProfile owned by the Port.
     * - owner             [RTObject Object type] The object reference of
     *                     RTObject that is owner of the Port.
     * - properties        [NVList type] The other properties.
     *
     * @post This function never changes the state of the object.
     *
     * @return PortProfile of the Port
     *
     * @endif
     */
    PortProfile* get_port_profile() override;

    /*!
     * @if jp
     *
     * @brief PortProfile を取得する。
     *
     * この関数は、オブジェクト内部に保持されている PortProfile の
     * const 参照を返す const 関数である。
     *
     * @post この関数を呼び出すことにより内部状態が変更されることはない。
     *
     * @return PortProfile
     *
     * @else
     *
     * @brief Get the PortProfile of the Port
     *
     * This function is a const function that returns a const
     * reference of the PortProfile stored in this Port.
     *
     * @post This function never changes the state of the object.
     *
     * @return PortProfile
     *
     * @endif
     */
    const PortProfile& getPortProfile() const;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConnectorProfileListを取得する
     *
     * Portが保持する ConnectorProfile の sequence を返す。この関数は
     * CORBA オペレーションであり、CORBA のメモリ管理規則に従い、呼び出
     * し側は返される ConnectorProfileList オブジェクトを解体する責任が
     * ある。
     *
     * ConnectorProfile は Port 間の接続プロファイル情報を保持する構造体であり、
     * 接続時にPort間で情報交換を行い、関連するすべての Port で同一の値が
     * 保持される。
     * ConnectorProfile は以下のメンバーを保持している。
     *
     * - name         [string 型] このコネクタの名前。
     * - connector_id [string 型] ユニークなID
     * - ports        [Port sequnce] このコネクタに関連する Port のオブジェクト
     *                リファレンスのシーケンス。
     * - properties   [NVList 型] その他のプロパティ。
     *
     * @post この関数を呼び出すことにより内部状態が変更されることはない。
     *
     * @return この Port が保持する ConnectorProfile
     *
     * @else
     *
     * @brief [CORBA interface] Get the ConnectorProfileList of the Port
     *
     * This operation returns a list of the ConnectorProfiles of the
     * Port.  Since this function is CORBA operation, callers have
     * responsibility to destruction of the returned ConnectorProfileList
     * object according to the CORBA memory management rules.
     *
     * ConnectorProfile includes the connection information that
     * describes relation between (among) Ports, and Ports exchange
     * the ConnectionProfile on connection process and hold the same
     * information in every Port.  ConnectionProfile has the following
     * members:
     *
     * - name         [string type] The name of the connection.
     * - connector_id [string type] Unique identifier.
     * - ports        [Port sequnce] The sequence of Port's object reference
     *                that are related the connection.
     * - properties   [NVList type] The other properties.
     *
     * @post This function never changes the state of the object.
     *
     * @return ConnectorProfileList of the Port
     *
     * @endif
     */
    ConnectorProfileList* get_connector_profiles() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConnectorProfile を取得する
     *
     * connector_id で指定された ConnectorProfile を返す。この関数は
     * CORBA オペレーションであり、CORBA のメモリ管理規則に従い、呼び出
     * し側は返される ConnectorProfile オブジェクトを解体する責任がある。
     *
     * @pre 引数に与える connector_id は有効な文字列でなければならない。
     * 空文字を指定した場合、または指定した connector_id を持つ
     * ConnectorProfile が見つからない場合は、空の ConnectorProfile を
     * 返す。
     *
     * @post この関数を呼び出すことにより内部状態が変更されることはない。
     *
     * @param connector_id ConnectorProfile の ID
     * @return connector_id で指定された ConnectorProfile
     *
     * @else
     *
     * @brief [CORBA interface] Get the ConnectorProfile
     *
     * This operation returns the ConnectorProfiles specified
     * connector_id.  Since this function is CORBA operation, callers
     * have responsibility to destruction of the returned
     * ConnectorProfile object according to the CORBA memory
     * management rules.
     *
     * If ConnectorProfile with specified connector_id is not included,
     * empty ConnectorProfile is returned.
     *
     * @post This function never changes the state of the object.
     *
     * @param connector_id ID of the ConnectorProfile
     * @return the ConnectorProfile identified by the connector_id
     *
     * @endif
     */
    ConnectorProfile* get_connector_profile(const char* connector_id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Port の接続を行う
     *
     * 与えられた ConnectoionProfile の情報に基づき、Port間の接続を確立
     * する。この関数は主にアプリケーションプログラムやツールから呼び出
     * すことを前提としている。
     *
     * @pre アプリケーションプログラムは、コンポーネント間の複数の
     * Port を接続するために、適切な値をセットした ConnectorProfile を
     * connect() の引数として与えて呼び出さなければならない。
     *
     * @pre connect() に与える ConnectorProfile のメンバーのうち、
     * name, ports, properties メンバーに対してデータをセットしなければ
     * ならない。connector_id には通常空文字を設定するか、適当なUUIDを
     * 文字列で設定する必要がある。
     *
     * @pre ConnectorProfile::name は接続につける名前で CORBA::string
     * 型に格納できる任意の文字列である必要がある。
     *
     * @pre ConnectorProfile::connector_id はすべての接続に対して一意な
     * ID (通常はUUID) が格納される。UUIDの設定は connect() 関数内で行
     * われるので、呼び出し側は空文字を設定する。既存の接続と同じUUIDを
     * 設定し connect() を呼び出した場合には PRECONDITION_NOT_MET エラー
     * を返す。ただし、将来の拡張で既存の接続プロファイルを更新するため
     * に既存の UUID を設定して呼び出す使用法が用いられる可能性がある。
     *
     * @pre ConnectorProfile::ports は RTC::PortService のシーケンスで、
     * 接続を構成する通常2つ以上のポートのオブジェクト参照を代入する必
     * 要がある。例外として、ポートのオブジェクト参照を1つだけ格納して
     * connect()を呼び出すことで、ポートのインターフェース情報を取得し
     * たり、特殊なポート(CORBAのRTC::PortService以外の相手)に対して接
     * 続を行う場合もある。
     *
     * @pre ConnectorProfile::properties はポートに関連付けられたインター
     * フェースに対するプロパティを与えるために使用する。プロパティは、
     * string 型をキー、Any 型を値としてもつペアのシーケンスであり、値
     * には任意のCORBAデータ型を格納できるが、可能な限り string 型とし
     * て格納されることが推奨される。
     *
     * @pre 以上 connect() 呼び出し時に設定する ConnectorProfile のメン
     * バをまとめると以下のようになる。
     *
     * - ConnectorProfile::name: 任意の接続名
     * - ConnectorProfile::connector_id: 空文字
     * - ConnectorProfile::ports: 1つ以上のポート
     * - ConnectorProfile::properties: インターフェースに対するプロパティ
     *
     * @post connect() 関数は、ConnectorProfile::portsに格納されたポー
     * トシーケンスの先頭のポートに対して notify_connect() を呼ぶ。
     *
     * @post notify_connect() は ConnectorProfile::ports に格納されたポー
     * ト順に notify_connect() をカスケード呼び出しする。このカスケード
     * 呼び出しは、途中のnotify_connect() でエラーが出てもポートのオブ
     * ジェクト参照が有効である限り、必ずすべてのポートに対して行われる
     * ことが保証される。有効でないオブジェクト参照がシーケンス中に存在
     * する場合、そのポートをスキップして、次のポートに対して
     * notify_connect() を呼び出す。
     *
     * @post connect() 関数は、notify_connect()の戻り値がRTC_OKであれば、
     * RTC_OK を返す。この時点で接続は完了する。RTC_OK以外
     * の場合は、この接続IDに対してdisconnect()を呼び出し接続を解除し、
     * notify_connect() が返したエラーリターンコードをそのまま返す。
     *
     * @post connect() の引数として渡した ConnectorProfile には、
     * ConnectorProfile::connector_id および、途中のポートが
     * publishInterfaces() で公開したポートインターフェースの各種情報が
     * 格納されている。connect() および途中の notify_connect() が
     * ConnectorProfile::{name, ports} を変更することはない。
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
     * @pre To establish the connection among Ports of RT-Components,
     * application programs must call this operation giving
     * ConnectorProfile with valid values as an argument.
     *
     * @pre Out of ConnectorProfile member variables, "name", "ports"
     * and "properties" members shall be set valid
     * data. "connector_id" shall be set as empty string value or
     * valid string UUID value.
     *
     * @pre ConnectorProfile::name that is connection identifier shall
     * be any valid CORBA::string.
     *
     *
     * @pre ConnectorProfile::connector_id shall be set unique
     * identifier (usually UUID is used) for all connections. Since
     * UUID string value is usually set in the connect() function,
     * caller should just set empty string. If the connect() is called
     * with the same UUID as existing connection, this function
     * returns PRECONDITION_NOT_MET error. However, in order to update
     * the existing connection profile, the "connect()" operation with
     * existing connector ID might be used as valid method by future
     * extension
     *
     * @pre ConnectorProfile::ports, which is sequence of
     * RTC::PortService references, shall store usually two or more
     * ports' references. As exceptions, the "connect()" operation
     * might be called with only one reference in ConnectorProfile, in
     * case of just getting interfaces information from the port, or
     * connecting a special port (i.e. the peer port except
     * RTC::PortService on CORBA).
     *
     * @pre ConnectorProfile::properties might be used to give certain
     * properties to the service interfaces associated with the port.
     * The properties is a sequence variable with a pair of key string
     * and Any type value. Although the A variable can store any type
     * of values, it is not recommended except string.
     *
     * @pre The following is the summary of the ConnectorProfile
     * member to be set when this operation is called.
     *
     * - ConnectorProfile::name: The any name of connection
     * - ConnectorProfile::connector_id: Empty string
     * - ConnectorProfile::ports: One or more port references
     * - ConnectorProfile::properties: Properties for the interfaces
     *
     * @post connect() operation will call the first port in the
     * sequence of the ConnectorProfile.
     *
     * @post "noify_connect()"s perform cascaded call to the ports
     * stored in the ConnectorProfile::ports by order. Even if errors
     * are raised by intermediate notify_connect() operation, as long
     * as ports' object references are valid, it is guaranteed that
     * this cascaded call is completed in all the ports.  If invalid
     * or dead ports exist in the port's sequence, the ports are
     * skipped and notify_connect() is called for the next valid port.
     *
     * @post connect() function returns RTC_OK if all the
     * notify_connect() return RTC_OK. At this time the connection is
     * completed.  If notify_connect()s return except RTC_OK,
     * connect() calls disconnect() operation with the connector_id to
     * destruct the connection, and then it returns error code from
     * notify_connect().
     *
     * @post The ConnectorProfile argument of the connect() operation
     * returns ConnectorProfile::connector_id and various information
     * about service interfaces that is published by
     * publishInterfaces() in the halfway ports. The connect() and
     * halfway notify_connect() functions never change
     * ConnectorProfile::{name, ports}.
     *
     * @param connector_profile The ConnectorProfile.
     * @return ReturnCode_t The return code of ReturnCode_t type.
     *
     * @endif
     */
    ReturnCode_t connect(ConnectorProfile& connector_profile) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Port の接続通知を行う
     *
     * このオペレーションは、Port間の接続が行われる際に、Port間で内部的
     * に呼ばれるオペレーションであって、通常アプリケーションプログラム
     * や、Port以外のRTC関連オブジェクト等から呼び出されることは想定さ
     * れていない。
     *
     * notify_connect() 自体はテンプレートメソッドパターンとして、サブ
     * クラスで実装されることが前提の publishInterfaces(),
     * subscribeInterfaces() の2つの関数を内部で呼び出す。処理の手順は
     * 以下の通りである。
     *
     * - publishInterfaces(): インターフェース情報の公開
     * - connectNext(): 次の Port の notify_connect() の呼び出し
     * - subscribeInterfaces(): インターフェース情報の取得
     * - 接続情報の保存
     *
     * notify_connect() は ConnectorProfile::ports に格納されている
     * Port の順序に従って、カスケード状に呼び出しを行うことにより、イ
     * ンターフェース情報の公開と取得を関連すすべてのポートに対して行う。
     * このカスケード呼び出しは途中で中断されることはなく、必ず
     * ConnectorProfile::ports に格納されている全ポートに対して行われる。
     *
     * @pre notify_connect() は ConnectorProfile::ports 内に格納されて
     * いる Port 参照リストのうち、当該 Port 自身の参照の次に格納されて
     * いる Port に対して notify_connect() を呼び出す。したがって
     * ConnectorProfile::ports には当該 Port の参照が格納されている必要
     * がある。もし、自身の参照が格納されていない場合、その他の処理によ
     * りエラーが上書きされなければ、BAD_PARAMETER エラーが返される。
     *
     * @pre 呼び出し時に ConnectorProfile::connector_id には一意なIDと
     * して UUID が保持されている必要がある。通常 connector_id は
     * connect() 関数により与えられ、空文字の場合は動作は未定義である。
     *
     * @post ConnectorProfile::name, ConnectorProfile::connector_id,
     * ConnectorProfile::ports は notify_connect() の呼び出しにより
     * 書き換えられることはなく不変である。
     *
     * @post ConnectorProfile::properties は notify_connect() の内部で、
     * 当該 Port が持つサービスインターフェースに関する情報を他の Port
     * に伝えるために、プロパティ情報が書き込まれる。
     *
     * @post なお、ConnectorProfile::ports のリストの最初 Port の
     * notify_connet() が終了した時点では、すべての関連する Port の
     * notify_connect() の呼び出しが完了する。publishInterfaces(),
     * connectNext(), subscribeInterfaces() および接続情報の保存のいず
     * れかの段階でエラーが発生した場合でも、エラーコードは内部的に保持
     * されており、最初に生じたエラーのエラーコードが返される。
     *
     * @param connector_profile ConnectorProfile
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Notify the Ports connection
     *
     * This operation is usually called from other ports' connect() or
     * notify_connect() operations when connection between ports is
     * established.  This function is not premised on calling from
     * other functions or application programs.
     *
     * According to the template method pattern, the notify_connect()
     * calls "publishInterfaces()" and "subsctiveInterfaces()"
     * functions, which are premised on implementing in the
     * subclasses. The processing sequence is as follows.
     *
     * - publishInterfaces(): Publishing interface information
     * - connectNext(): Calling notify_connect() of the next port
     * - subscribeInterfaces(): Subscribing interface information
     * - Storing connection profile
     *
     * According to the order of port's references stored in the
     * ConnectorProfile::ports, publishing interface information to
     * all the ports and subscription interface information from all
     * the ports is performed by "notify_connect()"s.  This cascaded
     * call never aborts in the halfway operations, and calling
     * sequence shall be completed for all the ports.
     *
     * @pre notify_connect() calls notify_connect() for the port's
     * reference that is stored in next of this port's reference in
     * the sequence of the ConnectorProfile::ports. Therefore the
     * reference of this port shall be stored in the
     * ConnectorProfile::ports. If this port's reference is not stored
     * in the sequence, BAD_PARAMETER error will be returned, except
     * the return code is overwritten by other operations.
     *
     * @pre UUID shall be set to ConnectorProfile::connector_id as a
     * unique identifier when this operation is called.  Usually,
     * connector_id is given by a connect() function and, the behavior
     * is undefined in the case of a null character.
     *
     * @post ConnectorProfile::name, ConnectorProfile::connector_id,
     * ConnectorProfile::ports are invariant, and they are never
     * rewritten by notify_connect() operations.
     *
     * @post In order to transfer interface information to other
     * ports, interface property information is stored into the
     * ConnectorProfile::properties.
     *
     * @post At the end of notify_connect() operation for the first
     * port stored in the ConnectorProfile::ports sequence, the
     * related ports' notify_connect() invocations complete. Even if
     * errors are raised at the halfway of publishInterfaces(),
     * connectNext(), subscribeInterfaces() and storing process of
     * ConnectorProfile, error codes are saved and the first error is
     * returned.
     *
     * @param connector_profile The ConnectorProfile.
     * @return ReturnCode_t The return code of ReturnCode_t type.
     *
     * @endif
     */
    ReturnCode_t notify_connect(ConnectorProfile& connector_profile) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Port の接続を解除する
     *
     * このオペレーションは与えられた connector_id に対応する接続を解除
     * する。connector_id は通常、システム全体において一意な UUID の文
     * 字列であり、事前に connect()/notify_connect() の呼び出しにより確
     * 立された接続プロファイル ConnectorProfile::connector_id に対応す
     * る。
     *
     * @pre connector_id は Port が保持する ConnectorProfile の少なくと
     * も一つの ID に一致する文字列でなければならない。当該 Port が持つ
     * ConnectorProfile のリスト内に connector_id と同一の ID を持つ
     * ConnectorProfile が存在しなければこの関数は BAD_PARAMETER エラー
     * を返す。
     *
     * @pre connector_id と同じ ID を持つ ConnectorProfile::ports には
     * 有効な Port の参照が含まれていなければならない。
     *
     * @post disconnect() 関数は、ConnectorProfile::ports の Port の参
     * 照リストの先頭に対して、notify_disconnect() を呼び出す。参照が無
     * 効であるなど、notify_disconnect() の呼び出しに失敗した場合には、
     * 参照リストの先頭から順番に成功するまで notify_disconnect() の呼
     * び出しを試す。notify_disconnect() の呼び出しに一つでも成功すれば、
     * notify_disconnect() の返却値をそのまま返し、一つも成功しなかった
     * 場合には RTC_ERROR エラーを返す。
     *
     * @param connector_id ConnectorProfile の ID
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Disconnect the Port
     *
     * This operation destroys connection between this port and the
     * peer port according to given connector_id. Usually connector_id
     * should be a UUID string that is unique in the system.  And the
     * connection, which is established by connect()/notify_connect()
     * functions, is identified by the ConnectorProfile::connector_id.
     *
     * @pre connector_id shall be a character string which is same
     * with ID of at least one of the ConnectorProfiles stored in this
     * port. If ConnectorProfile that has same ID with the given
     * connector_id does not exist in the list of ConnectorProfile,
     * this operation returns BAD_PARAMTER error.
     *
     * @pre ConnectorProfile::ports that is same ID with given
     * connector_id shall store the valid ports' references.
     *
     * @post disconnect() function invokes the notify_disconnect() for
     * the port that is stored in the first of the
     * ConnectorProfile::ports. If notify_disconnect() call fails for
     * the first port, It tries on calling "notify_disconnect()" in
     * order for ports stored in ConnectorProfile::ports until the
     * operation call is succeeded. If notify_disconnect() succeeded
     * for at least one port, it returns return code from
     * notify_disconnect(). If none of notify_connect() call
     * succeeded, it returns RTC_ERROR error.
     *
     * @param connector_id The ID of the ConnectorProfile.
     * @return ReturnCode_t The return code of ReturnCode_t type.
     *
     * @endif
     */
    ReturnCode_t disconnect(const char* connector_id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Port の接続解除通知を行う
     *
     * このオペレーションは、Port間の接続解除が行われる際に、Port間で内
     * 部的に呼ばれるオペレーションであり、通常アプリケーションプログラ
     * ムや、 Port 以外の RTC 関連オブジェクト等から呼び出されることは
     * 想定されていない。
     *
     * notify_disconnect() 自体はテンプレートメソッドパターンとして、サ
     * ブクラスで実装されることが前提の unsubscribeInterfaces() 関数を
     * 内部で呼び出す。処理の手順は以下の通りである。
     *
     * - ConnectorProfile の検索
     * - 次の Port の notify_disconnect() 呼び出し
     * - unsubscribeInterfaces()
     * - ConnectorProfile の削除
     *
     * notify_disconnect() は ConnectorProfile::ports に格納されている
     * Port の順序に従って、カスケード状に呼び出しを行うことにより、接
     * 続の解除をすべての Port に通知する。
     *
     * @pre Port は与えられた connector_id に対応する ConnectorProfile
     * を保持していなければならない。
     *
     * @post connector_id に対応する ConnectorProfile が見つからない場
     * 合はBAD_PARAMETER エラーを返す。
     *
     * @post カスケード呼び出しを行う際には ConnectorProfile::ports に
     * 保持されている Port の参照リストのうち、自身の参照の次の参照に対
     * して notify_disconnect() を呼び出すが、その呼び出しで例外が発生
     * した場合には、呼び出しをスキップしリストの次の参照に対して
     * notify_disconnect() を呼び出す。一つも呼び出しに成功しない場合、
     * RTC_ERROR エラーコードを返す。
     *
     * @post なお、ConnectorProfile::ports のリストの最初 Port の
     * notify_disconnet() が終了した時点では、すべての関連する Port の
     * notify_disconnect() の呼び出しが完了する。
     *
     * @param connector_id ConnectorProfile の ID
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Notify the Ports disconnection
     *
     * This operation is invoked between Ports internally when the
     * connection is destroied. Generally it is not premised on
     * calling from application programs or RTC objects except Port
     * object.
     *
     * According to the template method pattern, the
     * notify_disconnect() calls unsubsctiveInterfaces() function,
     * which are premised on implementing in the subclasses. The
     * processing sequence is as follows.
     *
     * - Searching ConnectorProfile
     * - Calling notify_disconnect() for the next port
     * - Unsubscribing interfaces
     * - Deleting ConnectorProfile
     *
     * notify_disconnect() notifies disconnection to all the ports by
     * cascaded call to the stored ports in the
     * ConnectorProfile::ports in order.
     *
     * @pre The port shall store the ConnectorProfile having same id
     * with connector_id.
     *
     * @post If ConnectorProfile of same ID with connector_id does not
     * exist, it returns BAD_PARAMETER error.
     *
     * @post For the cascaded call, this operation calls
     * noify_disconnect() for the port that is stored in the next of
     * this port in the ConnectorProfile::ports.  If the operation
     * call raises exception for some failure, it tries to call
     * notify_disconnect() and skips until the operation succeeded.
     * If none of operation call succeeded, it returns RTC_ERROR.
     *
     * @post At the end of notify_disconnect() operation for the first
     * port stored in the ConnectorProfile::ports sequence, the
     * related ports' notify_disconnect() invocations complete.
     *
     * @param connector_id The ID of the ConnectorProfile.
     * @return ReturnCode_t The return code of ReturnCode_t type.
     *
     * @endif
     */
    ReturnCode_t notify_disconnect(const char* connector_id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Port の全接続を解除する
     *
     * このオペレーションはこの Port に関連した全ての接続を解除する。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Disconnect the All Ports
     *
     * This operation destroys all connections associated with this Port.
     *
     * @return ReturnCode_t The return code of ReturnCode_t type.
     *
     * @endif
     */
    ReturnCode_t disconnect_all() override;

    //============================================================
    // Local operations
    //============================================================
    /*!
     * @if jp
     *
     * @brief Port の全てのインターフェースを activates する
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
    virtual void activateInterfaces() = 0;

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
    virtual void deactivateInterfaces() = 0;

    /*!
     * @if jp
     * @brief Port の名前を設定する
     *
     * Port の名前を設定する。この名前は Port が保持する PortProfile.name
     * に反映される。
     *
     * @param name Port の名前
     *
     * @else
     * @brief Set the name of this Port
     *
     * This operation sets the name of this Port. The given Port's name is
     * applied to Port's PortProfile.name.
     *
     * @param name The name of this Port.
     *
     * @endif
     */
    void setName(const char* name);

     /*!
     * @if jp
     * @brief Port の名前を取得する
     *
     * Port の名前を取得する。
     *
     * @return Port の名前
     *
     * @else
     * @brief Get the name of this Port
     *
     * This operation returns the name of this Port.
     *
     * @return The name of this Port.
     *
     * @endif
     */
    const char* getName() const;

    /*!
     * @if jp
     * @brief PortProfileを取得する
     *
     * Portが保持する PortProfile の const 参照を返す。
     *
     * @return この Port の PortProfile
     *
     * @else
     * @brief Get the PortProfile of the Port
     *
     * This operation returns const reference of the PortProfile.
     *
     * @return PortProfile of the Port
     *
     * @endif
     */
    const PortProfile& getProfile() const;

    /*!
     * @if jp
     *
     * @brief Port のオブジェクト参照を設定する
     *
     * このオペレーションは Port の PortProfile にこの Port 自身の
     * オブジェクト参照を設定する。
     *
     * @param port_ref この Port のオブジェクト参照
     *
     * @else
     *
     * @brief Set the object reference of this Port
     *
     * This operation sets the object reference itself
     * to the Port's PortProfile.
     *
     * @param port_ref The object reference of this Port.
     *
     * @endif
     */
    void setPortRef(PortService_ptr port_ref);

    /*!
     * @if jp
     *
     * @brief Port のオブジェクト参照を取得する
     *
     * このオペレーションは Port の PortProfile が保持している
     * この Port 自身のオブジェクト参照を取得する。
     *
     * @return この Port のオブジェクト参照
     *
     * @else
     *
     * @brief Get the object reference of this Port
     *
     * This operation returns the object reference
     * that is stored in the Port's PortProfile.
     *
     * @return The object reference of this Port.
     *
     * @endif
     */
    PortService_ptr getPortRef() const;

    /*!
     * @if jp
     *
     * @brief Port の owner の RTObject を指定する
     *
     * このオペレーションは Port の PortProfile.owner を設定する。
     *
     * @param owner この Port を所有する RTObject の参照
     *
     * @else
     *
     * @brief Set the owner RTObject of the Port
     *
     * This operation sets the owner RTObject of this Port.
     *
     * @param owner The owner RTObject's reference of this Port
     *
     * @endif
     */
    void setOwner(RTObject_ptr owner);

    //============================================================
    // callbacks
    //============================================================
    /*!
     * @if jp
     *
     * @brief インターフェースを公開する際に呼ばれるコールバックをセットする
     *
     * このオペレーションは、このポートが接続時に、ポート自身が持つサー
     * ビスインターフェース情報を公開するタイミングで呼ばれるコールバッ
     * クファンクタをセットする。
     *
     * コールバックファンクタの所有権は、呼び出し側にあり、オブジェクト
     * が必要なくなった時に解体するのは呼び出し側の責任である。
     *
     * このコールバックファンクタは、PortBaseクラスの仮想関数である
     * publishInterfaces() が呼ばれたあとに、同じ引数 ConnectorProfile と
     * ともに呼び出される。このコールバックを利用して、
     * publishInterfaces() が公開した ConnectorProfile を変更することが可
     * 能であるが、接続関係の不整合を招かないよう、ConnectorProfile の
     * 変更には注意を要する。
     *
     * @param on_publish ConnectionCallback のサブクラスオブジェクトのポインタ
     *
     * @else
     *
     * @brief Setting callback called on publish interfaces
     *
     * This operation sets a functor that is called after publishing
     * interfaces process when connecting between ports.
     *
     * Since the ownership of the callback functor object is owned by
     * the caller, it has the responsibility of object destruction.
     *
     * The callback functor is called after calling
     * publishInterfaces() that is virtual member function of the
     * PortBase class with an argument of ConnectorProfile type that
     * is same as the argument of publishInterfaces() function.
     * Although by using this functor, you can modify the ConnectorProfile
     * published by publishInterfaces() function, the modification
     * should be done carefully for fear of causing connection
     * inconsistency.
     *
     * @param on_publish a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setOnPublishInterfaces(ConnectionCallback* on_publish);

    /*!
     * @if jp
     *
     * @brief インターフェースを取得する際に呼ばれるコールバックをセットする
     *
     * このオペレーションは、このポートが接続時に、相手のポートが持つサー
     * ビスインターフェース情報を取得するタイミングで呼ばれるコールバッ
     * クファンクタをセットする。
     *
     * コールバックファンクタの所有権は、呼び出し側にあり、オブジェクト
     * が必要なくなった時に解体するのは呼び出し側の責任である。
     *
     * このコールバックファンクタは、PortBaseクラスの仮想関数である
     * subscribeInterfaces() が呼ばれる前に、同じ引数 ConnectorProfile と
     * ともに呼び出される。このコールバックを利用して、
     * subscribeInterfaces() に与える ConnectorProfile を変更することが可
     * 能であるが、接続関係の不整合を招かないよう、ConnectorProfile の
     * 変更には注意を要する。
     *
     * @param on_subscribe ConnectionCallback のサブクラスオブジェクトのポインタ
     *
     * @else
     *
     * @brief Setting callback called on publish interfaces
     *
     * This operation sets a functor that is called before subscribing
     * interfaces process when connecting between ports.
     *
     * Since the ownership of the callback functor object is owned by
     * the caller, it has the responsibility of object destruction.
     *
     * The callback functor is called before calling
     * subscribeInterfaces() that is virtual member function of the
     * PortBase class with an argument of ConnectorProfile type that
     * is same as the argument of subscribeInterfaces() function.
     * Although by using this functor, you can modify ConnectorProfile
     * argument for subscribeInterfaces() function, the modification
     * should be done carefully for fear of causing connection
     * inconsistency.
     *
     * @param on_subscribe a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setOnSubscribeInterfaces(ConnectionCallback* on_subscribe);

    /*!
     * @if jp
     *
     * @brief 接続完了時に呼ばれるコールバックをセットする
     *
     * このオペレーションは、このポートが接続完了時に呼ばれる、コールバッ
     * クファンクタをセットする。
     *
     * コールバックファンクタの所有権は、呼び出し側にあり、オブジェクト
     * が必要なくなった時に解体するのは呼び出し側の責任である。
     *
     * このコールバックファンクタは、ポートの接続実行関数である
     * notify_connect() の終了直前に、接続処理が正常終了する際に限って
     * 呼び出されるコールバックである。接続処理の過程でエラーが発生した
     * 場合には呼び出されない。
     *
     * このコールバックファンクタは notify_connect() が out パラメータ
     * として返すのと同じ引数 ConnectorProfile とともに呼び出されるので、
     * この接続において公開されたすべてのインターフェース情報を得ること
     * ができる。このコールバックを利用して、notify_connect() が返す
     * ConnectorProfile を変更することが可能であるが、接続関係の不整合
     * を招かないよう、ConnectorProfile の変更には注意を要する。
     *
     * @param on_subscribe ConnectionCallback のサブクラスオブジェクトのポインタ
     *
     * @else
     *
     * @brief Setting callback called on connection established
     *
     * This operation sets a functor that is called when connection
     * between ports established.
     *
     * Since the ownership of the callback functor object is owned by
     * the caller, it has the responsibility of object destruction.
     *
     * The callback functor is called only when notify_connect()
     * function successfully returns. In case of error, the functor
     * will not be called.
     *
     * Since this functor is called with ConnectorProfile argument
     * that is same as out-parameter of notify_connect() function, you
     * can get all the information of published interfaces of related
     * ports in the connection.  Although by using this functor, you
     * can modify ConnectorProfile argument for out-paramter of
     * notify_connect(), the modification should be done carefully for
     * fear of causing connection inconsistency.
     *
     * @param on_subscribe a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setOnConnected(ConnectionCallback* on_connected);

    /*!
     * @if jp
     *
     * @brief インターフェースを解放する際に呼ばれるコールバックをセットする
     *
     * このオペレーションは、このポートが接続時に、相手のポートが持つサー
     * ビスインターフェース情報を解放するタイミングで呼ばれるコールバッ
     * クファンクタをセットする。
     *
     * コールバックファンクタの所有権は、呼び出し側にあり、オブジェクト
     * が必要なくなった時に解体するのは呼び出し側の責任である。
     *
     * このコールバックファンクタは、PortBaseクラスの仮想関数である
     * unsubscribeInterfaces() が呼ばれる前に、同じ引数 ConnectorProfile と
     * ともに呼び出される。このコールバックを利用して、
     * unsubscribeInterfaces() に与える ConnectorProfile を変更することが可
     * 能であるが、接続関係の不整合を招かないよう、ConnectorProfile の
     * 変更には注意を要する。
     *
     * @param on_unsubscribe ConnectionCallback のサブクラスオブジェク
     * トのポインタ
     *
     * @else
     *
     * @brief Setting callback called on unsubscribe interfaces
     *
     * This operation sets a functor that is called before unsubscribing
     * interfaces process when disconnecting between ports.
     *
     * Since the ownership of the callback functor object is owned by
     * the caller, it has the responsibility of object destruction.
     *
     * The callback functor is called before calling
     * unsubscribeInterfaces() that is virtual member function of the
     * PortBase class with an argument of ConnectorProfile type that
     * is same as the argument of unsubscribeInterfaces() function.
     * Although by using this functor, you can modify ConnectorProfile
     * argument for unsubscribeInterfaces() function, the modification
     * should be done carefully for fear of causing connection
     * inconsistency.
     *
     * @param on_unsubscribe a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setOnUnsubscribeInterfaces(ConnectionCallback* on_unsubscribe);

    /*!
     * @if jp
     *
     * @brief 接続解除に呼ばれるコールバックをセットする
     *
     * このオペレーションは、このポートの接続解除時に呼ばれる、コールバッ
     * クファンクタをセットする。
     *
     * コールバックファンクタの所有権は、呼び出し側にあり、オブジェクト
     * が必要なくなった時に解体するのは呼び出し側の責任である。
     *
     * このコールバックファンクタは、ポートの接続解除実行関数である
     * notify_disconnect() の終了直前に、呼び出されるコールバックである。
     *
     * このコールバックファンクタは接続に対応する ConnectorProfile とと
     * もに呼び出される。この ConnectorProfile はこのファンクタ呼出し後
     * に破棄されるので、変更がほかに影響を与えることはない。
     *
     * @param on_disconnected ConnectionCallback のサブクラスオブジェク
     * トのポインタ
     *
     * @else
     *
     * @brief Setting callback called on disconnected
     *
     * This operation sets a functor that is called when connection
     * between ports is destructed.
     *
     * Since the ownership of the callback functor object is owned by
     * the caller, it has the responsibility of object destruction.
     *
     * The callback functor is called just before notify_disconnect()
     * that is disconnection execution function returns.
     *
     * This functor is called with argument of corresponding
     * ConnectorProfile.  Since this ConnectorProfile will be
     * destructed after calling this functor, modifications never
     * affect others.
     *
     * @param on_disconnected a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setOnDisconnected(ConnectionCallback* on_disconnected);

    /*!
     * @if jp
     * @brief ポートの接続がロストした場合に呼び出されるコールバックをセットする
     *
     * このオペレーションは、このポートの接続がロストした場合に呼ばれる、
     * コールバックファンクタをセットする。
     *
     * InPortは、相手側OutPortとの
     * 接続をロストした場合、接続を強制的に切断するので、
     * 引き続き OnDisconnect コールバックが呼び出される。
     *
     * @param on_connection_lost ConnectionCallback のサブクラスオブジェク
     * トのポインタ
     *
     * @else
     * @brief Setting callback called on connection lost
     *
     * This operation sets a functor that is called when connection
     * of this port does lost.
     *
     * @param on_connection_lost a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setOnConnectionLost(ConnectionCallback* on_connection_lost);

    /*!
     * @if jp
     * @brief PortConnectListeners のホルダをセットする
     *
     * ポートの接続に関するリスナ群を保持するホルダクラスへのポインタを
     * セットする。この関数は通常親のRTObjectから呼ばれ、RTObjectが持つ
     * ホルダクラスへのポインタがセットされる。
     *
     * @param portconnListeners PortConnectListeners オブジェクトのポインタ
     *
     * @else
     * @brief Setting PortConnectListener holder
     *
     * This operation sets a functor that is called when connection
     * of this port does lost.
     *
     * @param on_connection_lost a pointer to ConnectionCallback's subclasses
     *
     * @endif
     */
    void setPortConnectListenerHolder(PortConnectListeners* portconnListeners);

    /*!
     * @if jp
     * @brief direct通信用ポートオブジェクト取得
     *
     * @return ポートのポインタ
     *
     * @else
     * @brief 
     *
     * @return
     *
     * @endif
     */
    virtual DirectPortBase* getDirectPort();
    //============================================================
    // protected operations
    //============================================================
  protected:
    /*! @if jp
     *
     * @brief Interface 情報を公開する
     *
     * このオペレーションは、notify_connect() 処理シーケンスの始めにコール
     * される純粋仮想関数である。
     * notify_connect() では、
     *
     * - publishInterfaces()
     * - connectNext()
     * - subscribeInterfaces()
     * - updateConnectorProfile()
     *
     * の順に protected 関数がコールされ接続処理が行われる。
     * <br>
     * 具象 Port ではこのオペレーションをオーバーライドし、引数として
     * 与えられた ConnectorProfile に従い処理を行い、パラメータが不適切
     * であれば、RteurnCode_t 型のエラーコードを返す。
     * 通常 publishInterafaces() 内においては、この Port に属する
     * インターフェースに関する情報を ConnectorProfile に対して適切に設定し
     * 他の Port に通知しなければならない。
     * <br>
     * また、この関数がコールされる段階では、他の Port の Interface に関する
     * 情報はすべて含まれていないので、他の Port の Interface を取得する処理
     * は subscribeInterfaces() 内で行われるべきである。
     * <br>
     * このオペレーションは、新規の connector_id に対しては接続の生成、
     * 既存の connector_id に対しては更新が適切に行われる必要がある。
     *
     * @param connector_profile 接続に関するプロファイル情報
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Publish interface information
     *
     * This operation is pure virutal function that would be called at the
     * beginning of the notify_connect() process sequence.
     * In the notify_connect(), the following methods would be called in order.
     *
     * - publishInterfaces()
     * - connectNext()
     * - subscribeInterfaces()
     * - updateConnectorProfile()
     *
     * In the concrete Port, this method should be overridden. This method
     * processes the given ConnectorProfile argument and if the given parameter
     * is invalid, it would return error code of ReturnCode_t.
     * Usually, publishInterfaces() method should set interfaces information
     * owned by this Port, and publish it to the other Ports.
     * <br>
     * When this method is called, other Ports' interfaces information may not
     * be completed. Therefore, the process to obtain other Port's interfaces
     * information should be done in the subscribeInterfaces() method.
     * <br>
     * This operation should create the new connection for the new
     * connector_id, and should update the connection for the existing
     * connection_id.
     *
     * @param connector_profile The connection profile information
     *
     * @return The return code of ReturnCode_t type.
     *
     * @endif
     */
    virtual ReturnCode_t
    publishInterfaces(ConnectorProfile& connector_profile) = 0;

    /*!
     * @if jp
     *
     * @brief 次の Port に対して notify_connect() をコールする
     *
     * ConnectorProfile の port_ref 内に格納されている Port のオブジェクト
     * リファレンスのシーケンスの中から、自身の Port の次の Port に対して
     * notify_connect() をコールする。
     *
     * @param connector_profile 接続に関するプロファイル情報
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Call notify_connect() of the next Port
     *
     * This operation calls the notify_connect() of the next Port's
     * that stored in ConnectorProfile's port_ref sequence.
     *
     * @param connector_profile The connection profile information
     *
     * @return The return code of ReturnCode_t type.
     *
     * @endif
     */
    virtual ReturnCode_t connectNext(ConnectorProfile& connector_profile);

    /*!
     * @if jp
     *
     * @brief 次の Port に対して notify_disconnect() をコールする
     *
     * ConnectorProfile の port_ref 内に格納されている Port のオブジェクト
     * リファレンスのシーケンスの中から、自身の Port の次の Port に対して
     * notify_disconnect() をコールする。
     *
     * @param connector_profile 接続に関するプロファイル情報
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Call notify_disconnect() of the next Port
     *
     * This operation calls the notify_disconnect() of the next Port's
     * that stored in ConnectorProfile's port_ref sequence.
     *
     * @param connector_profile The connection profile information
     *
     * @return The return code of ReturnCode_t type.
     *
     * @endif
     */
    virtual ReturnCode_t disconnectNext(ConnectorProfile& cprof);

    /*! @if jp
     *
     * @brief Interface 情報を公開する
     *
     * このオペレーションは、notify_connect() 処理シーケンスの中間にコール
     * される純粋仮想関数である。
     * notify_connect() では、
     *
     * - publishInterfaces()
     * - connectNext()
     * - subscribeInterfaces()
     * - updateConnectorProfile()
     *
     * の順に protected 関数がコールされ接続処理が行われる。
     * <br>
     * 具象 Port ではこのオペレーションをオーバーライドし、引数として
     * 与えられた ConnectorProfile に従い処理を行い、パラメータが不適切
     * であれば、RteurnCode_t 型のエラーコードを返す。
     * 引数 ConnectorProfile には他の Port の Interface に関する情報が
     * 全て含まれている。
     * 通常 subscribeInterafaces() 内においては、この Port が使用する
     * Interface に関する情報を取得し、要求側のインターフェースに対して
     * 情報を設定しなければならない。
     * <br>
     * このオペレーションは、新規の connector_id に対しては接続の生成、
     * 既存の connector_id に対しては更新が適切に行われる必要がある。
     *
     * @param connector_profile 接続に関するプロファイル情報
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Publish interface information
     *
     * This operation is pure virutal function that would be called at the
     * mid-flow of the notify_connect() process sequence.
     * In the notify_connect(), the following methods would be called in order.
     *
     * - publishInterfaces()
     * - connectNext()
     * - subscribeInterfaces()
     * - updateConnectorProfile()
     *
     * In the concrete Port, this method should be overridden. This method
     * processes the given ConnectorProfile argument and if the given parameter
     * is invalid, it would return error code of ReturnCode_t.
     * The given argument ConnectorProfile includes all the interfaces
     * information in it.
     * Usually, subscribeInterafaces() method obtains information of interfaces
     * from ConnectorProfile, and should set it to the interfaces that require
     * them.
     * <br>
     * This operation should create the new connection for the new
     * connector_id, and should update the connection for the existing
     * connection_id.
     *
     * @param connector_profile The connection profile information
     *
     * @return The return code of ReturnCode_t type.
     *
     * @endif
     */
    virtual ReturnCode_t
    subscribeInterfaces(const ConnectorProfile& connector_profile) = 0;

    /*! @if jp
     *
     * @brief Interface の接続を解除する
     *
     * このオペレーションは、notify_disconnect() 処理シーケンスの終わりにコール
     * される純粋仮想関数である。
     * notify_disconnect() では、
     * - disconnectNext()
     * - unsubscribeInterfaces()
     * - eraseConnectorProfile()
     * の順に protected 関数がコールされ接続解除処理が行われる。
     * <br>
     * 具象 Port ではこのオペレーションをオーバーライドし、引数として
     * 与えられた ConnectorProfile に従い接続解除処理を行う。
     *
     * @param connector_profile 接続に関するプロファイル情報
     *
     * @else
     *
     * @brief Disconnect interface connection
     *
     * This operation is pure virutal function that would be called at the
     * end of the notify_disconnect() process sequence.
     * In the notify_disconnect(), the following methods would be called
     * in order to disconnect.
     * - disconnectNext()
     * - unsubscribeInterfaces()
     * - eraseConnectorProfile()
     *  <br>
     * In the concrete Port, this method should be overridden. This method
     * processes the given ConnectorProfile argument and disconnect interface
     * connection.
     *
     * @param connector_profile The connection profile information
     *
     * @endif
     */
    virtual void
    unsubscribeInterfaces(const ConnectorProfile& connector_profile) = 0;

    /*! @if jp
     *
     * @brief 接続の最大数を設定する。
     *
     * @param limit_value 最大数
     *
     * @else
     *
     * @brief Set the maximum number of connections
     *
     *
     * @param limit_value The maximum number of connections
     *
     * @endif
     */
    virtual void setConnectionLimit(int limit_value);

    /*!
     * @if jp
     * @brief Interface情報を公開する
     *
     * Interface情報を公開する。
     *
     *  dataport.dataflow_type
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     * @brief Publish interface information
     *
     * Publish interface information.
     *
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    virtual ReturnCode_t _publishInterfaces();
    //============================================================
    // protected utility functions
    //============================================================
    /*!
     * @if jp
     *
     * @brief ConnectorProfile の connector_id フィールドが空かどうか判定
     *
     * 指定された ConnectorProfile の connector_id が空であるかどうかの判定を
     * 行う。
     *
     * @param connector_profile 判定対象コネクタプロファイル
     *
     * @return 引数で与えられた ConnectorProfile の connector_id が空であれば、
     *         true、そうでなければ false を返す。
     *
     * @else
     *
     * @brief Check whether connector_id of ConnectorProfile is empty
     *
     * Check whether connector_id of specified ConnectorProfile is empty.
     *
     * @param connector_profile Target ConnectorProfile for the check
     *
     * @return If the given ConnectorProfile's connector_id is empty string,
     *         it returns true.
     *
     * @endif
     */
    static bool isEmptyId(const ConnectorProfile& connector_profile) ;

    /*!
     * @if jp
     *
     * @brief UUIDを生成する
     *
     * このオペレーションは UUID を生成する。
     *
     * @return uuid
     *
     * @else
     *
     * @brief Generate the UUID
     *
     * This operation generates UUID.
     *
     * @return uuid
     *
     * @endif
     */
    static std::string getUUID() ;

    /*!
     * @if jp
     *
     * @brief UUIDを生成し ConnectorProfile にセットする
     *
     * このオペレーションは UUID を生成し、ConnectorProfile にセットする。
     *
     * @param connector_profile connector_id をセットする ConnectorProfile
     *
     * @else
     *
     * @brief Generate and set the UUID to the ConnectorProfile
     *
     * This operation generates and set UUID to the ConnectorProfile.
     *
     * @param connector_profile ConnectorProfile to be set connector_id
     *
     * @endif
     */
    static void setUUID(ConnectorProfile& connector_profile) ;

    /*!
     * @if jp
     *
     * @brief id が既存の ConnectorProfile のものかどうか判定する
     *
     * このオペレーションは与えられた ID が既存の ConnectorProfile のリスト中に
     * 存在するかどうか判定する。
     *
     * @param id 判定する connector_id
     *
     * @return id の存在判定結果
     *
     * @else
     *
     * @brief Check whether the given id exists in stored ConnectorProfiles
     *
     * This operation returns boolean whether the given id exists in
     * the Port's ConnectorProfiles.
     *
     * @param id connector_id to be find in Port's ConnectorProfiles
     *
     * @return id exestance resutl
     *
     * @endif
     */
    bool isExistingConnId(const char* id);

    /*!
     * @if jp
     *
     * @brief id を持つ ConnectorProfile を探す
     *
     * このオペレーションは与えられた ID を持つ ConnectorProfile を Port が
     * もつ ConnectorProfile のリスト中から探す。
     * もし、同一の id を持つ ConnectorProfile がなければ、空の ConnectorProfile
     * が返される。
     *
     * @param id 検索する connector_id
     *
     * @return connector_id を持つ ConnectorProfile
     *
     * @else
     *
     * @brief Find ConnectorProfile with id
     *
     * This operation returns ConnectorProfile with the given id from Port's
     * ConnectorProfiles' list.
     * If the ConnectorProfile with connector id that is identical with the
     * given id does not exist, empty ConnectorProfile is returned.
     *
     * @param id the connector_id to be searched in Port's ConnectorProfiles
     *
     * @return CoonectorProfile with connector_id
     *
     * @endif
     */
    ConnectorProfile findConnProfile(const char* id);

    /*!
     * @if jp
     *
     * @brief id を持つ ConnectorProfile を探す
     *
     * このオペレーションは与えられた ID を持つ ConnectorProfile を Port が
     * もつ ConnectorProfile のリスト中から探しインデックスを返す。
     * もし、同一の id を持つ ConnectorProfile がなければ、-1 を返す。
     *
     * @param id 検索する connector_id
     *
     * @return Port の ConnectorProfile リストのインデックス
     *
     * @else
     *
     * @brief Find ConnectorProfile with id
     *
     * This operation returns ConnectorProfile with the given id from Port's
     * ConnectorProfiles' list.
     * If the ConnectorProfile with connector id that is identical with the
     * given id does not exist, -1 is returned.
     *
     * @param id the connector_id to be searched
     *
     * @return The index of ConnectorProfile of the Port
     *
     * @endif
     */
    CORBA::Long findConnProfileIndex(const char* id);

    /*!
     * @if jp
     *
     * @brief ConnectorProfile の追加もしくは更新
     *
     * このオペレーションは与えられた ConnectorProfile を Port に追加もしくは
     * 更新保存する。
     * 与えられた ConnectorProfile の connector_id と同じ ID を持つ
     * ConnectorProfile がリストになければ、リストに追加し、
     * 同じ ID が存在すれば ConnectorProfile を上書き保存する。
     *
     * @param connector_profile 追加もしくは更新する ConnectorProfile
     *
     * @else
     *
     * @brief Append or update the ConnectorProfile list
     *
     * This operation appends or updates ConnectorProfile of the Port
     * by the given ConnectorProfile.
     * If the connector_id of the given ConnectorProfile does not exist
     * in the Port's ConnectorProfile list, the given ConnectorProfile would be
     * append to the list. If the same id exists, the list would be updated.
     *
     * @param connector_profile the ConnectorProfile to be appended or updated
     *
     * @endif
     */
    void updateConnectorProfile(const ConnectorProfile& connector_profile);

    /*!
     * @if jp
     *
     * @brief ConnectorProfile を削除する
     *
     * このオペレーションは Port の PortProfile が保持している
     * ConnectorProfileList のうち与えられた id を持つ ConnectorProfile
     * を削除する。
     *
     * @param id 削除する ConnectorProfile の id
     *
     * @return 正常に削除できた場合は true、
     *         指定した ConnectorProfile が見つからない場合は false を返す
     *
     * @else
     *
     * @brief Delete the ConnectorProfile
     *
     * This operation deletes a ConnectorProfile specified by id from
     * ConnectorProfileList owned by PortProfile of this Port.
     *
     * @param id The id of the ConnectorProfile to be deleted.
     *
     * @return true would be returned if it deleted correctly.
     *         false woluld be returned if specified ConnectorProfile
     *         cannot be found.
     *
     * @endif
     */
    bool eraseConnectorProfile(const char* id);

    /*!
     * @if jp
     *
     * @brief PortInterfaceProfile に インターフェースを登録する
     *
     * このオペレーションは Port が持つ PortProfile の、PortInterfaceProfile
     * にインターフェースの情報を追加する。
     * この情報は、get_port_profile() によって得られる PortProfile のうち
     * PortInterfaceProfile の値を変更するのみであり、実際にインターフェースを
     * 提供したり要求したりする場合には、サブクラスで、 publishInterface() ,
     *  subscribeInterface() 等の関数を適切にオーバーライドしインターフェースの
     * 提供、要求処理を行わなければならない。
     *
     * インターフェース(のインスタンス)名は Port 内で一意でなければならない。
     * 同名のインターフェースがすでに登録されている場合、この関数は false を
     * 返す。
     *
     * @param name インターフェースのインスタンスの名前
     * @param type_name インターフェースの型の名前
     * @param pol インターフェースの属性 (RTC::PROVIDED もしくは RTC:REQUIRED)
     *
     * @return インターフェース登録処理結果。
     *         同名のインターフェースが既に登録されていれば false を返す。
     *
     * @else
     *
     * @brief Append an interface to the PortInterfaceProfile
     *
     * This operation appends interface information to the PortInterfaceProfile
     * that is owned by the Port.
     * The given interfaces information only updates PortInterfaceProfile of
     * PortProfile that is obtained through get_port_profile().
     * In order to provide and require interfaces, proper functions (for
     * example publishInterface(), subscribeInterface() and so on) should be
     * overridden in subclasses, and these functions provide concrete interface
     * connection and disconnection functionality.
     *
     * The interface (instance) name have to be unique in the Port.
     * If the given interface name is identical with stored interface name,
     * this function returns false.
     *
     * @param name The instance name of the interface.
     * @param type_name The type name of the interface.
     * @param pol The interface's polarity (RTC::PROVIDED or RTC:REQUIRED)
     *
     * @return false would be returned if the same name is already registered.
     *
     * @endif
     */
    bool appendInterface(const char* instance_name, const char* type_name,
                         PortInterfacePolarity pol);

    /*!
     * @if jp
     *
     * @brief PortInterfaceProfile からインターフェース登録を削除する
     *
     * このオペレーションは Port が持つ PortProfile の、PortInterfaceProfile
     * からインターフェースの情報を削除する。
     *
     * @param name インターフェースのインスタンスの名前
     * @param pol インターフェースの属性 (RTC::PROVIDED もしくは RTC:REQUIRED)
     *
     * @return インターフェース削除処理結果。
     *         インターフェースが登録されていなければ false を返す。
     *
     * @else
     *
     * @brief Delete the interface registration from the PortInterfaceProfile
     *
     * This operation deletes interface information from the
     * PortInterfaceProfile that is owned by the Port.
     *
     * @param name The instance name of the interface.
     * @param pol The interface's polarity (RTC::PROVIDED or RTC:REQUIRED)
     *
     * @return Delete processing result of interface.
     *         false would be returned if the given name is not registered.
     *
     * @endif
     */
    bool deleteInterface(const char* name, PortInterfacePolarity pol);

    /*!
     * @if jp
     *
     * @brief PortProfile の properties に NameValue 値を追加する
     *
     * PortProfile の properties に NameValue 値を追加する。
     * 追加するデータの型をValueTypeで指定する。
     *
     * @param key properties の name
     * @param value properties の value
     *
     * @else
     *
     * @brief Add NameValue data to PortProfile's properties
     *
     * Add NameValue data to PortProfile's properties.
     * Type of additional data is specified by ValueType.
     *
     * @param key The name of properties
     * @param value The value of properties
     *
     * @endif
     */
    template <class ValueType>
    void addProperty(const char* key, ValueType value)
    {
      CORBA_SeqUtil::push_back(m_profile.properties,
                               NVUtil::newNV(key, value));
    }

    /*!
     * @if jp
     *
     * @brief PortProfile の properties に NameValue 値を要素に追加する
     *
     * PortProfile の properties に NameValue 値を要素に追加する。この
     * 関数により設定された properties は get_prot_profile() により外部
     * から参照される。
     *
     * @param key properties の name
     * @param value properties の value
     *
     * @else
     *
     * @brief Append NameValue data to PortProfile's properties
     *
     * Append NameValue data to PortProfile's properties.  The
     * properties which are set by this function would be referred
     * through get_port_profile() from outsides.
     *
     * @param key The name of properties
     * @param value The value of properties
     *
     * @endif
     */
    void appendProperty(const char* key, const char* value)
    {
      NVUtil::appendStringValue(m_profile.properties, key, value);
    }
    /*!
     * @if jp
     * @brief プロパティ
     * @else
     * @brief Properties
     * @endif
     */
    coil::Properties m_properties;
  protected:
    /*!
     * @if jp
     *
     * @brief 存在しないポートをdisconnectする。
     *
     * 死んだPortを検出し、もし死んでいるポートがあった場合には、接続を
     * 解除する。
     *
     * @else
     *
     * @brief Disconnect ports that doesn't exist.
     *
     * This function detects dead-port, and if dead ports are found in
     * the connection list, disconnects them.
     *
     * @endif
     */
    void updateConnectors();

    /*!
     * @if jp
     *
     * @brief ポートの存在を確認する。
     *
     * @param ports 確認するポート
     * @return true:存在する,false:存在しない
     *
     * @else
     *
     * @brief Existence of ports
     *
     * @param ports Checked ports
     * @return true:existent,false:non existent
     *
     * @endif
     */
#ifndef ORB_IS_RTORB
    bool checkPorts(::RTC::PortServiceList& ports);
#else  // ORB_IS_RTORB
    bool checkPorts(RTC_PortServiceList& ports);
#endif  // ORB_IS_RTORB


    inline void onNotifyConnect(const char* portname,
                                RTC::ConnectorProfile& profile)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectListenerType::ON_NOTIFY_CONNECT, portname, profile);
        }
    }

    inline void onNotifyDisconnect(const char* portname,
                                   RTC::ConnectorProfile& profile)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectListenerType::ON_NOTIFY_DISCONNECT, portname, profile);
        }
    }
    inline void onUnsubscribeInterfaces(const char* portname,
                                        RTC::ConnectorProfile& profile)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectListenerType::ON_UNSUBSCRIBE_INTERFACES, portname, profile);
        }
    }

    inline void onPublishInterfaces(const char* portname,
                                    RTC::ConnectorProfile& profile,
                                    ReturnCode_t ret)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectRetListenerType::ON_PUBLISH_INTERFACES, portname,
                                                       profile, ret);
        }
    }

    inline void onConnectNextport(const char* portname,
                                  RTC::ConnectorProfile& profile,
                                  ReturnCode_t ret)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectRetListenerType::ON_CONNECT_NEXTPORT, portname,
                                                     profile, ret);
        }
    }

    inline void onSubscribeInterfaces(const char* portname,
                                      RTC::ConnectorProfile& profile,
                                      ReturnCode_t ret)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectRetListenerType::ON_SUBSCRIBE_INTERFACES, portname,
                                                         profile, ret);
        }
    }

    inline void onConnected(const char* portname,
                            RTC::ConnectorProfile& profile,
                            ReturnCode_t ret)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectRetListenerType::ON_CONNECTED, portname, profile, ret);
        }
    }

    inline void onDisconnectNextport(const char* portname,
                                 RTC::ConnectorProfile& profile,
                                 ReturnCode_t ret)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectRetListenerType::ON_DISCONNECT_NEXT, portname, profile, ret);
        }
    }

    inline void onDisconnected(const char* portname,
                               RTC::ConnectorProfile& profile,
                               ReturnCode_t ret)
    {
      if (m_portconnListeners != nullptr)
        {
          m_portconnListeners->notify(
            PortConnectRetListenerType::ON_DISCONNECTED, portname, profile, ret);
        }
    }

  protected:
  /*!
   * @if jp
   *
   * @brief 指定のシリアライザが使用可能かを判定する
   *
   *
   * @param con_prop コネクタプロファイルのプロパティ
   * @return true：利用可能、false：利用不可
   *
   * @else
   *
   * @brief Whether the specified serializer can be used
   *
   * @param con_prop Properties of Connectorprofile
   * @return ture: avialable, false: un-available
   *
   * @endif
   */
    bool isExistingMarshalingType(coil::Properties& con_prop);
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    mutable Logger rtclog;
    /*!
     * @if jp
     * @brief Port の PortProfile
     * @else
     * @brief PortProfile of the Port
     * @endif
     */
    PortProfile m_profile;

    /*!
     * @if jp
     * @brief Port の オブジェクト参照
     * @else
     * @brief Object Reference of the Port
     * @endif
     */
    RTC::PortService_var m_objref;
    /*!
     * @if jp
     * @brief PortProfile の mutex
     * @else
     * @brief Mutex of PortProfile
     * @endif
     */
    mutable std::mutex m_profile_mutex;
    mutable std::mutex m_connectorsMutex;

    /*!
     * @if jp
     * @brief インスタンス名
     * @else
     * @brief Instance name
     * @endif
     */
    std::string m_ownerInstanceName = "unknown";

    /*!
     * @if jp
     * @brief Port の接続の最大数
     * @else
     * @brief The maximum number of connections
     * @endif
     */
    int m_connectionLimit{-1};

    /*!
     * @if jp
     * @brief Callback functor オブジェクト
     *
     * インターフェースを公開する際に呼ばれるコールバックオブジェクト
     *
     * @else
     * @brief Callback functor objects
     *
     * This is callback objedct that is called when the interface is opened
     * to the public.
     *
     * @endif
     */
    ConnectionCallback* m_onPublishInterfaces{nullptr};
    /*!
     * @if jp
     * @brief Callback functor オブジェクト
     *
     * インターフェースを取得する際に呼ばれるコールバックオブジェクト
     *
     * @else
     * @brief Callback functor objects
     *
     * This is callback objedct that is called when the interface is got.
     *
     * @endif
     */
    ConnectionCallback* m_onSubscribeInterfaces{nullptr};
    /*!
     * @if jp
     * @brief Callback functor オブジェクト
     *
     * 接続完了時に呼ばれるコールバックオブジェクト
     *
     * @else
     * @brief Callback functor objects
     *
     * This is a callback object that is called
     * when the connection is completed.
     *
     * @endif
     */
    ConnectionCallback* m_onConnected{nullptr};
    /*!
     * @if jp
     * @brief Callback functor オブジェクト
     *
     * インターフェースを解放する際に呼ばれるコールバックオブジェクト
     *
     * @else
     * @brief Callback functor objects
     *
     * This is a callback object that is called when the interface is released.
     *
     * @endif
     */
    ConnectionCallback* m_onUnsubscribeInterfaces{nullptr};
    /*!
     * @if jp
     * @brief Callback functor オブジェクト
     *
     * 接続解除に呼ばれるコールバックオブジェクト
     *
     * @else
     * @brief Callback functor objects
     *
     * This is a callback object that is called in connected release.
     *
     * @endif
     */
    ConnectionCallback* m_onDisconnected{nullptr};

    /*!
     * @if jp
     * @brief Callback functor オブジェクト
     *
     * ポートの接続がロストした場合に呼び出されるコールバックオブジェクト
     *
     * @else
     * @brief Callback functor objects
     *
     * This is a callback object called when the connection of the port does
     * lost.
     *
     * @endif
     */
    ConnectionCallback* m_onConnectionLost{nullptr};

    /*!
     * @if jp
     * @brief PortConnectListenerホルダ
     *
     * PortConnectListenrを保持するホルダ
     *
     * @else
     * @brief PortConnectListener holder
     *
     * Holders of PortConnectListeners
     *
     * @endif
     */
    PortConnectListeners* m_portconnListeners{nullptr};

    DirectPortBase *m_directport{nullptr};

    //============================================================
    // Functor
    //============================================================
    /*!
     * @if jp
     * @brief id を持つ ConnectorProfile を探す Functor
     * @else
     * @brief Functor to find a ConnectorProfile named id
     * @endif
     */
    struct find_conn_id
    {
      explicit find_conn_id(const char* id) : m_id(id) {}
      bool operator()(const ConnectorProfile& cprof)
      {
        return m_id == std::string(cprof.connector_id);
      }
      std::string m_id;
    };  // struct find_conn_id

    /*!
     * @if jp
     * @brief コンストラクタ引数 port_ref と同じオブジェクト参照を探す Functor
     * @else
     * @brief Functor to find the object reference that is identical port_ref
     * @endif
     */
    struct find_port_ref
    {
      explicit find_port_ref(PortService_ptr port_ref) : m_port(port_ref) {}
#ifdef ORB_IS_ORBEXPRESS
      bool operator()(PortService_var port_ref)
      {
        return m_port->_is_equivalent(port_ref.in());
      }
#else
      bool operator()(PortService_ptr port_ref)
      {
        return m_port->_is_equivalent(port_ref);
      }
#endif
      PortService_ptr m_port;
    };  // struct find_port_ref

    /*!
     * @if jp
     * @brief name と polarity から interface を探す Functor
     * @else
     * @brief Functor to find interface from name and polarity
     * @endif
     */
    struct find_interface
    {
      find_interface(const char* name, PortInterfacePolarity pol)
        : m_name(name), m_pol(pol)
      {}

      bool operator()(const PortInterfaceProfile& prof)
      {
        CORBA::String_var name(CORBA::string_dup(prof.instance_name));
        return   ((m_name == static_cast<const char *>(name)) 
              && (m_pol == prof.polarity));
      }
      std::string m_name;
      PortInterfacePolarity m_pol;
    };  // struct find_interface
  };  // class PortBase
} // namespace RTC

#endif  // RTC_PORTBASE_H
