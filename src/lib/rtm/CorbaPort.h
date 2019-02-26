// -*- C++ -*-
/*!
 * @file  CorbaPort.h
 * @brief CorbaPort class
 * @date  $Date: 2007-12-31 03:08:02 $
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

#ifndef RTC_CORBAPORT_H
#define RTC_CORBAPORT_H

#include <rtm/PortBase.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/NVUtil.h>

#include <list>
#include <string>
#include <vector>

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
  /*!
   * @if jp
   * @class CorbaPort
   * @brief RT コンポーネント CORBA provider/consumer 用 Port
   *
   * CorbaPort は RT コンポーネントにおいて、ユーザ定義の CORBA オブジェ
   * クトサービスおよびコンシューマを提供する Port 実装である。
   *
   * RT コンポーネントは、Port を介してユーザが定義した CORBA サービス
   * を提供することができ、これを RT Service (Provider) と呼ぶ。また、
   * 他の RT コンポーネントのサービスを利用するための CORBA オブジェク
   * トのプレースホルダを提供することができ、これを RT Service
   * Consumer と呼ぶ。
   * CorbaPort は任意の数の Provider および Consumer を管理することがで
   * き、Port 同士を接続する際に対応する Provider と Consumer を適切に
   * 関連付けることができる。
   * CorbaPort は通常以下のように利用される。
   *
   * <pre>
   * RTC::CorbaPort m_port0; // Port の宣言
   *
   * MyService_impl m_mysvc0; // この Port が提供する Serivce Provider
   * RTC::CorbaConsumer<YourService> m_cons0; // この Port の Consumer
   *
   * // Service Provider を Port に登録
   * m_port0.registerProvider("MyService0", "Generic", m_mysvc0);
   * // Service Consumer を Port に登録
   * m_port0.registerConsumer("YourService0", "Generic", m_cons0 );
   *
   * // connect が行われた後
   *
   * m_cons0->your_service_function(); // YourService の関数をコール
   *
   * // connect された 別のコンポーネントにおいて
   * m_cons1->my_service_function(); // MyService の関数をコール
   * </pre>
   *
   * このように、提供したい Service Provider を registerProvider() で登
   * 録することにより、他のコンポーネントから利用可能にし、他方、利用し
   * たい Service Consumer を registerConsumer() で登録することにより他
   * のコンポーネントの Service をコンポーネント内で利用可能にすること
   * ができる。
   *
   * PortInterfaceProfile は Port に所属するプロバイダもしくはコンシュー
   * マインターフェースについての情報を記述するためのプロファイルである。
   * 接続を行うツール等は、これらの情報に基づき ConnectorProfile を適切
   * に生成し、接続を構成する Port のうち任意の一つに対して引数に
   * ConnectorProfile を与えて Port::connect() を呼び出す必要がある。
   *
   * なお、PortInterfaceProfile のインスタンス名 "*" は特殊なインスタン
   * スを表す。
   *
   * PROVIDEDすなわちプロバイダのインスタンス名が "*" の場合は、接続開
   * 始時点ではインスタンスが存在せず、コンシューマの要求に応じて動的に
   * インスタンスを生成するタイプのプロバイダであることを表す。したがっ
   * て、接続開始時点ではインスタンス名は存在しないが、接続シーケンス中
   * のインターフェースを公開するプロセスにおいて、プロバイダは生成した
   * インスタンスに対応した記述子を ConnectorProfile に適正に設定するも
   * のとする。(未実装)
   *
   * REQUIREDすなわちコンシューマのインスタンス名が "*" の場合は、一つ
   * のコンシューマが複数のプロバイダと接続可能なタイプのコンシューマで
   * あることを示す。(未実装)
   *
   * 以下は、Port間のインターフェースを接続するために ConnectorProfile に
   * マッピングを記述するためのルールを示す。
   *
   * Portに付属するインターフェースの指定子のフォーマットを以下のように
   * 定める。インターフェースに関するプロパティが以下の場合
   *
   * - RTCインスタンス名:              rtc_iname
   * - ポート名:                       port_name
   * - インターフェース極性:           if_polarity
   * - インターフェース型名:           if_tname
   * - インターフェースインスタンス名: if_iname
   *
   * インターフェースの指定子を以下の文字列名称で指定するものとする。
   *
   * <rtc_iname>.port.<port_name>.<if_polarity>.<if_tname>.<if_iname>
   *
   * PROVIDED(提供)型すなわちプロバイダのインタフェースのプロパティが以
   * 下の場合、
   *
   * - rtc_iname   = MyComp0
   * - port_name   = myservice
   * - if_polarity = provided
   * - if_tname    = echo_interface
   * - if_iname    = echo_interface2
   *
   * インターフェース指定子は
   *
   * MyComp0.port.myservice.provided.echo_interface.echo_interface2
   *
   * のように記述される。また、同様にREQUIRED(要求)型すなわちコンシュー
   * マのインターフェースのプロパティが以下の場合、
   *
   * - rtc_iname   = YourComp0
   * - port_name   = yourservice
   * - if_polarity = required
   * - if_tname    = hoge_interface
   * - if_iname    = hoge_interface1
   *
   * インターフェース指定子は、
   *
   * YourComp0.port.myservice.required.hoge_interface.hoge_inteface1
   *
   * のように記述することができる。
   *
   * なお、ここで動的生成インターフェースのインスタンスのための特殊なタ
   * イプのインスタンス名記述子
   *
   * - <type_name>*: 動的生成型インスタンス名記述子
   * - <type_name>+: インクリメンタル生成型インスタンス名記述子
   *
   * を定義する。動的生成インターフェースとは、接続時にインスタンスが生
   * 成されるタイプのインターフェースである。(未実装)
   *
   * コンシューマが要求するプロバイダインターフェース記述子に動的生成型
   * インスタンス名記述子 "<type_name>*" が指定された場合、プロバイダは
   * インスタンスを1つ新規に生成する。"<type_name>*" の記述子によりプロバ
   * イダを要求する n 個のコンシューマが存在する場合、これらからの要求
   * (オペレーションコール)を1 つのプロバイダにより処理する関係を構築す
   * る(下図)。
   *
   * <pre>
   * consumer0 ]---<
   * consumer1 ]---<  O----[ provider0
   * consumer2 ]---<
   * </pre>
   *
   * これに対し、コンシューマが要求するプロバイダインターフェース記述子
   * にインクリメンタル生成型インスタンス名記述子 "<type_name>+" が指定
   * された場合、記述子 "<type_name>+" の数だけプロバイダのインスタン
   * スが動的に生成される。すなわち、"<type_name>+" の記述子によりプロバ
   * イダを要求する n 個のコンシューマが存在する場合、n 個のプロバイダ
   * がそれぞれの要求を処理する以下のような関係が構築される。
   *
   * <pre>
   * consumer0 ]---<  O----[ provider0
   * consumer1 ]---<  O----[ provider1
   * consumer2 ]---<  O----[ provider2
   * </pre>
   *
   *
   * 接続に際して、ツール等から ConnectorProfile::properties に適切なイ
   * ンターフェースマッピング指定を記述することで、相互のプロバイダ/コ
   * ンシューマインターフェースを自由に接続することができる。ただし、接
   * 続に関わる RTC の中に、異なるインスタンスでありながら、同一のインス
   * タンス名が存在する場合、インターフェース記述子の一意性が保証できな
   * いので、この方法による接続性は保証されない。
   *
   * ここでインターフェース記述子を簡単のために <if_desc0>,
   * <if_desc1>, ...  とする。また、ConnectorProfile::properties の
   * NVListの key と value を key: value のように記述するものとする。
   *
   * いま、2つのコンポーネントのサービスポートを接続する場合を考える。
   * それぞれのコンポーネントのサービスポートが以下の場合、
   *
   * - rtc_iname: MyComp0        <br>
   *   port_name: mycomp_service <br>
   *   interfaces:
   *   - if_polarity: provided   <br>
   *     if_iname: echo0         <br>
   *     if_tname: Echo
   *   - if_polarity: required   <br>
   *     if_iname: add0          <br>
   *     if_tname: add
   *
   * - rtc_iname: YourComp0        <br>
   *   port_name: yourcomp_service <br>
   *   interfaces:
   *   - if_polarity: required     <br>
   *     if_iname: echo9           <br>
   *     if_tname: Echo
   *   - if_polarity: provided     <br>
   *     if_iname: add9            <br>
   *     if_tname: add
   *
   * <pre>
   *      MyComp0                                 YourComp0
   *     _______ mycomp_service   yourcomp_service ______
   *            |                                 |
   *          |~~~|---O echo0         echo9 >---|~~~|
   *          |   |---< add0          add9  O---|   |
   *           ~T~                               ~T~
   *            |                                 |
   * </pre>
   *
   * MyComp0 の echo0 (プロバイダ) と YourComp0 の echo9 (コンシューマ)、
   * MyComp0 の add0 (コンシューマ) と YourComp0 の echo9 (プロバイダ)
   * をそれぞれ対にして接続させるものと仮定する。この場合、
   * ConnectorProfile は以下のように設定する。
   *
   * <pre>
   * ConnectorProfile:
   *   name: 任意のコネクタ名
   *   connector_id: 空文字
   *   ports[]: mycomp_service の参照, yourcomp_service の参照
   *   properties:
   *     <add0>: <add9>
   *     <echo9>: <echo0>
   * </pre>
   *
   * ただし、それぞれ
   *
   * <pre>
   * <add0> は MyComp0.port.mycomp_service.required.add.add0
   * <add9> は YourComp0.port.yourcomp_service.provided.add.add9
   * <echo0> は MyComp0.port.mycomp_service.provided.echo.echo0
   * <echo9> は YourComp0.port.yourcomp_service.required.echo.echo9
   * </pre>
   *
   * である。接続プロセスにおいて、各ポートのプロバイダおよびコンシュー
   * マは、それぞれ以下の作業を、CorbaPort::publishInterfaces(),
   * CorbaPort::PortsubscribeInterfaces() 仮想関数において行う。
   *
   * プロバイダは、publishInterfaces() 関数において、自分のインターフェー
   * ス記述子をキーとし、値にIORの文字列表記したものを
   * ConnectorProfile::properties に設定する。前提として、このインター
   * フェース記述子は今行おうとしているコネクタにおいては一意であるため、
   * 同じキーは1つしか存在してはいけない。
   *
   * [この部分の記述は未実装の機能] なお、動的生成インターフェースにつ
   * いては、以下の手続きに従い処理することとなる。publishInterface()
   * 関数において、動的生成インスタンス名記述子 "<type_name>*" または、
   * インクリメンタル生成型インスタンス名記述子 "<type_name>+" が存在す
   * るかどうかを走査する。動的生成インスタンス名記述子 "<type_name>*"
   * が存在する場合、プロバイダのインスタンスを1つ生成し、そのインター
   * フェース指定子を key に、IOR文字列を value に設定するとともに、動
   * 的生成インスタンス名記述子 "<type_name>*" を value に含むすべての
   * value 上のインターフェース指定子を、ここで生成したインターフェース
   * 指定子に置き換える。
   *
   * インクリメンタル生成型インスタンス名記述子"<type_name>+" が存在す
   * る場合、インスタンス名記述子の数だけプロバイダのインスタンスを生成
   * し、それぞれのインターフェース指定子をkey に、IOR文字列を value に
   * 設定するとともに、インクリメンタル生成型インスタンス名記述子
   * "<type_name>+" を value 含むすべての value 上のインターフェース指
   * 定子に対して順に、ここで生成したそれぞれのインターフェース指定子に
   * 置き換える。
   *
   * プロバイダは subscribeInterfaces() では特に操作は行わない。
   *
   * コンシューマは、 publishInterfaces() においては特に操作を行わない。
   *
   * 一方、 subscribeInterfaces() では、自分の記述子を key とする
   * key-value ペア が存在するかどうか調べ、もし存在すれば、その value
   * に設定されたプロバイダのインターフェース指定子で指定される参照を、
   * さらに ConnectorProfile::properties から探し、それをコンシューマの
   * 接続先として設定する。なお、意図的にコンシューマにプロバイダの参照
   * を設定しない場合は、予約文字列 "nil" または "null" を設定するもの
   * とする。
   *
   * コンシューマは、もし自分の記述子が存在しない場合、またはプロバイダ
   * の参照が Connector::properties に存在しない場合、コンシューマは、
   * 自分のインスタンス名および型名と同一のプロバイダを探し、その参照を
   * 自分自身に設定する。これは、OpenRTM-aist-0.4 との互換性を保持する
   * ためのルールであり、1.0以降では推奨されない。
   *
   * プロバイダ対コンシューマの対応は一対一である必要はなく、プロバイダ
   * 1 に対して、コンシューマ n、またはコンシューマ 1 に対してプロバイ
   * ダ n のケースも許される。プロバイダ 1 に対して、コンシューマ n の
   * ケースでは、あるプロバイダの指定子が、複数のコンシューマに対して、
   * 上記の方法で指定されることにより、実現される。一方、コンシューマ
   * 1 に対してプロバイダ n のケースでは、コンシューマ指定子の key に対
   * して、複数のプロバイダの指定子がカンマ区切りで列挙される形式となる
   * ものとする。
   *
   * なお、インターフェースの対応関係の厳密さを指定するオプションとして、
   * 以下のオプションを指定することができる。
   *
   * port.connection.strictness: strict, best_effort
   *
   * strict: すべてのコンシューマに指定した参照が存在し、かつナローイン
   *         グにも成功しコンシューマに適切にセットできた場合にのみ Port
   *         間の接続を確立する。
   *
   * best_effort: ナローイング等に失敗した場合でも、エラーを返すことな
   *         く Port 間の接続を確立する。
   *
   * @since 0.4.0
   *
   * @else
   * @class CorbaPort
   * @brief RT Conponent CORBA service/consumer Port
   *
   * CorbaPort is an implementation of the Port of RT-Component's that provides
   * user-defined CORBA Object Service and Consumer.
   * <p>
   * RT-Component can provide user-defined CORBA serivces, which is called
   * RT-Serivce (Provider), through the Ports.
   * RT-Component can also provide place-holder, which is called RT-Serivce
   * Consumer, to use other RT-Component's service.
   * <p>
   * The CorbaPort can manage any number of Providers and Consumers, can
   * associate Consumers with correspondent Providers when establishing
   * connection among Ports.
   * <p>
   * Usually, CorbaPort is used like the following.
   *
   * <pre>
   * RTC::CorbaPort m_port0; // declaration of Port
   *
   * MyService_impl m_mysvc0; // Serivce Provider that is provided by the Port
   * RTC::CorbaConsumer<YourService> m_cons0; // Consumer of the Port
   *
   * // register Service Provider to the Port
   * m_port0.registerProvider("MyService0", "Generic", m_mysvc0);
   * // register Service Consumer to the Port
   * m_port0.registerConsumer("YourService0", "Generic", m_cons0 );
   *
   * // after connect established
   *
   * m_cons0->your_service_function(); // call a YourService's function
   *
   * // in another component that is connected with the Port
   * m_cons1->my_service_function(); // call a MyService's function
   * </pre>
   *
   * Registering Service Provider by registerProvider(), it can be
   * used from other RT-Components.  Registering Service Consumer by
   * registerConsumer(), other RT-Component's services can be used
   * through the consumer object.
   *
   * PortInterfaceProfile is a one of the profile information to store
   * Provider interface and Consumer interface information. Tools or
   * other RTCs should call one of the Port::connect() with an
   * appropriate ConnectorProfile.
   *
   * In addition, the instance name "*" declares a special type of instance.
   *
   * When the name of the PROVIDED type interface that is the provider
   * interface is "*", Provider interface's instance does not exist at
   * the beginning of connection sequence.  The instances will be
   * created dynamically according to the consumer interface
   * requirement at the connection sequence.  Although the instance
   * name does not exist at the beginning of connection sequence, the
   * created providers shall publish its references to the
   * ConnectorProfile with interface descriptor adequately in the
   * interface publisher phase of the connection sequence.
   *
   * If REQUIRED interface name that is Consumer interface name is
   * "*", it shows that one Consumer interface is able to connect with
   * multiple Provider interfaces. (This feature is not implemented.)
   *
   * The following describes the rules that specify interface
   * connection between ports.
   *
   * The descriptor format of interfaces associated with Ports is
   * declared as follows. Now some of interface properties are assumed
   * as the followings.
   *
   * - RTC instance name:              rtc_iname
   * - Port name:                      port_name
   * - Interface polarity:             if_polarity
   * - Interface type name:            if_tname
   * - INterface instance name:        if_iname
   *
   * The interface descriptors shall be declared as follows.
   *
   * <rtc_iname>.port.<port_name>.<if_polarity>.<if_tname>.<if_iname>
   *
   * When PROVIDED that is Provider interface properties are the followings,
   *
   * - rtc_iname   = MyComp0
   * - port_name   = myservice
   * - if_polarity = provided
   * - if_tname    = echo_interface
   * - if_iname    = echo_interface2
   * the interface descriptor is here.
   *
   * MyComp0.port.myservice.provided.echo_interface.echo_interface2
   *
   * And, when REQUIRED that is Consumer interfaces properties are the
   * followings,
   *
   * - rtc_iname   = YourComp0
   * - port_name   = yourservice
   * - if_polarity = required
   * - if_tname    = hoge_interface
   * - if_iname    = hoge_interface1
   *
   * interface descriptor is as follows.
   *
   * YourComp0.port.myservice.required.hoge_interface.hoge_inteface1
   *
   * Specific instance name descriptors that are dynamically generated
   * at the connection time are defined here.
   *
   * - <type_name>*: "Dynamically generated" instance descriptor.
   * - <type_name>+: "Incrementally generated" instance descriptor.
   *
   * When the "Dynamically generated" instance descriptor:
   * "<type_name>*" is specified as interface descriptor that is
   * required by consumers, the provider will generate a instance. If
   * n consumers who demand a provider by the "<type_name>" descriptor
   * exist, the following relation which processes the call from these
   * consumers by one provider will be established.
   *
   * <pre>
   * consumer0 ]---<
   * consumer1 ]---<  O----[ provider0
   * consumer2 ]---<
   * </pre>
   *
   * On the other hand, when incremental generated type instance name
   * descriptor "<type_name>+" is specified as the provider interface
   * descriptor whom consumers demand, provider's instances are
   * dynamically generated for the number of the descriptors
   * "<type_name>+". When n consumers who demand a provider by the
   * descriptor "<type_name>+" exist the following relations in which
   * n providers process each call from the consumers will be
   * established.
   *
   * <pre>
   * consumer0 ]---<  O----[ provider0
   * consumer1 ]---<  O----[ provider1
   * consumer2 ]---<  O----[ provider2
   * </pre>
   *
   *
   * Describing the appropriate interface mapping specification in the
   * ConnectorProfile::properties, selective connections between
   * providers/consumers interface can be established at the time of
   * connection. However, when different RTC instances of the same
   * instance name exist in a connection, since an interface
   * descriptor uniqueness cannot be guaranteed, this connection
   * mapping rules cannot be used.
   *
   * Here, assume that an interface descriptor is given as <if_desc0>,
   * <if_desc1>, .... And assume that the key and the value of NVList
   * in ConnectorProfile::properties are given as "key: value".
   *
   * Now the case where the service ports of two components are
   * connected is considered. When the service port of each component
   * is the following,
   *
   * - rtc_iname: MyComp0          <br>
   *   port_name: mycomp_service   <br>
   *   interfaces:
   *   - if_polarity: provided     <br>
   *     if_iname: echo0           <br>
   *     if_tname: Echo
   *   - if_polarity: required     <br>
   *     if_iname: add0            <br>
   *     if_tname: add
   *
   * - rtc_iname: YourComp0        <br>
   *   port_name: yourcomp_service <br>
   *   interfaces:
   *   - if_polarity: required     <br>
   *     if_iname: echo9           <br>
   *     if_tname: Echo
   *   - if_polarity: provided     <br>
   *     if_iname: add9            <br>
   *     if_tname: add
   *
   *
   * <pre>
   *      MyComp0                                 YourComp0
   *     _______ mycomp_service   yourcomp_service ______
   *            |                                 |
   *          |~~~|---O echo0         echo9 >---|~~~|
   *          |   |---< add0          add9  O---|   |
   *           ~T~                               ~T~
   *            |                                 |
   * </pre>
   *
   *
   *
   * Assume that connection between echo0 (provider) of MyComp0
   * component and echo9 (consumer) of YourComp0 component, and add0
   * (consumer) of MyComp0 and add0 (provider) of YourComp0 is
   * established.  In this case, ConnectorProfile is set up as
   * follows.
   *
   * <pre>
   * ConnectorProfile:
   *   name: any connector name
   *   connector_id: empty string
   *   ports[]: mycomp_service's reference, yourcomp_service's reference
   *   properties:
   *     <add0>: <add9>
   *     <echo9>: <echo0>
   * </pre>
   *
   * Please note that <add0>, <add9>, <echo0> and <echo9> are the following.
   *
   * <pre>
   * <add0> is MyComp0.port.mycomp_service.required.add.add0
   * <add9> is YourComp0.port.yourcomp_service.provided.add.add9
   * <echo0> is MyComp0.port.mycomp_service.provided.echo.echo0
   * <echo9> is YourComp0.port.yourcomp_service.required.echo.echo9
   * </pre>
   *
   * In the connection process, the provider and the consumer of each
   * port carries out the following process respectively in the
   * virtual functions such as CorbaPort::publishInterfaces() and
   * CorbaPort::subscribeInerfaces().
   *
   * A provider sets its IOR string as a value and its interface
   * descriptor as a key in the ConnectorProfile::properties in a
   * publishInterfaces() function. Since this interface descriptor's
   * uniqueness is guaranteed in the current connector, the key of
   * NameValue in the ConnectorProfile::properties is unique.
   *
   *
   * [This functionalities are not implemented] The dynamically
   * generated provider is processed according to the following
   * procedure. The publishInterface() function scans dynamic instance
   * descriptors such as "<type_name>*" and "<type_name>+" in the
   * ConnectorProfile::properties. When the dynamic generation
   * instance descriptor "<tupe_name>*" exists, one instance of
   * provider is generated, and its descriptor and its IOR string are
   * set to ConnectorProfile::properties as the key and the value
   * respectively. Simultaneously, in the
   * ConnectorProfile::properties, all the instance descriptor with
   * the dynamic generation instance name "<type_name>*" will be
   * replaced with newly generated instance descriptor.
   *
   * When the incremental dynamic generation instance descriptor
   * exists, providers are generated for the number of the
   * descriptors, and its descriptor and its IOR string are set to
   * ConnectorProfile::properties as the key and the value
   * respectively. Simultaneously, in the
   * ConnectorProfile::properties, all the instance descriptor with
   * the dynamic generation instance name "<type_name>+" will be
   * replaced with newly generated instance descriptor.
   *
   * The providers do not perform particular operation in
   * subscribeInterfaces() function.
   *
   *
   * The consumers do not perform particular operation in
   * publisherInterfaces() function.
   *
   * On the other hand, a consumer searches a key-value pair with the
   * key of consumer interface descriptor, and if the pair exists, it
   * obtains provider's descriptor from the value. The consumer
   * searches again a key-value pair with the key of provider
   * interface descriptor, and it obtains provider's reference and the
   * reference is set as the consumer's service object. In addition,
   * reserved string "nil" or "null" are used not to set specific
   * provider.
   *
   * If consumer's interface descriptors does not exists in the
   * ConnectorProfile::properties, the consumer searches a provider
   * with same type name and instance name, and its reference is set
   * to the consumer. This rule is for only backward compatibility,
   * and it is not recommended from version 1.0.
   *
   * The correspondence of a provider versus a consumer does not need
   * to be one to one, and the case of one provider to n-consumers and
   * the case of m-providers to one consumer are allowed. The one
   * provider to n-consumers case can be realized by the above
   * mentioned methods. The one consumer to m-provider case can be
   * specified to set the consumer descriptor and comma-separated
   * provider descriptors into the key and the value respectively.
   *
   * The following option is available to specify the strictness of
   * interfaces connection.
   *
   * port.connection.strictness: strict, best_effort
   *
   * strict: The connection is established, if only all the specified
   *         consumers are set appropriate references and narrowed
   *         successfully.
   *
   * best_effort: The connection is established without any errors,
   *         even if appropriate reference does not exist or reference
   *         narrowing fails.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class CorbaPort
    : public PortBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタでは、引数に与えられた名前を PortBase に対してセッ
     * トするとともに、PortProfile::properties に対して
     *
     * - port.port_type: "CorbaPort"
     *
     * をプロパティとして追加する。
     *
     * @param name Port の名前
     *
     * @else
     *
     * @brief Constructor
     *
     * In the ctor, a given name is set into PortBase, and the
     * following property is added to the PortProfile::properties,
     *
     * - port.port_type: "CorbaPort"
     *
     * @param name The name of Port
     *
     * @endif
     */
    explicit CorbaPort(const char* name);

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * @else
     *
     * @brief Virtual destructor
     *
     * @endif
     */
    virtual ~CorbaPort(void);

    /*!
     * @if jp
     * @brief プロパティの初期化
     *
     * OutPortのプロパティを初期化する。このポートへの接続数を指定する
     * プロパティ "connection_limit" が含まれ、適切な数値が設定されてい
     * る場合、最大接続数としてその数値が設定される。プロパティが設定さ
     * れていない場合、もしくは適切な値が設定されていない場合には、最大
     * 接続数は無制限となる。
     *
     * @param prop CorbaPort のプロパティ
     *
     * @else
     *
     * @brief Initializing properties
     *
     * This operation initializes outport's properties. If a property
     * "connection_limit" is set and appropriate value is set to this
     * property value, the number of maximum connection is set as this
     * value. If the property does not exist or invalid value is set
     * to this property, the maximum number of connection will be set
     * unlimited.
     *
     * @param prop properties of the CorbaPort
     *
     * @endif
     */
    void init(coil::Properties& prop);

    /*!
     * @if jp
     *
     * @brief Provider を登録する
     *
     * この Port において提供したいサーバントをこの Port に対して登録す
     * る。サーバントは、引数で与えられる instance_name, type_name を、
     * サーバント自身のインスタンス名およびタイプ名として、サーバントに
     * 関連付けられる。この関数により、サーバントは CorbaPort 内部に保
     * 持されるとともに、PortInterfaceProfile にRTC::PROVIDED インター
     * フェースとして登録される。
     *
     * @param instance_name サーバントのインスタンス名
     * @param type_name サーバントのタイプ名
     * @param provider CORBA サーバント
     *
     * @return 既に同名の instance_name が登録されていれば false を返す。
     *
     * @else
     *
     * @brief Register the provider
     *
     * This operation registers a servant, which is provided in this
     * Port, to the Port. The servant is associated with
     * "instance_name" and "type_name" as the instance name of the
     * servant and as the type name of the servant. A given servant
     * will be stored in the CorbaPort, and this is registered as
     * RTC::PROVIDED interface into the PortInterfaceProfile.
     *
     * @param instance_name Instance name of servant
     * @param type_name Type name of the servant
     * @param provider CORBA servant
     *
     * @return Return false if the same name of instance_name is already
     *         registered.
     *
     * @endif
     */
    bool registerProvider(const char* instance_name, const char* type_name,
                          PortableServer::RefCountServantBase& provider);

    /*!
     * @if jp
     *
     * @brief Consumer を登録する
     *
     * この Port が要求するサービスのプレースホルダであるコンシューマ
     * (Consumer) を登録する。Consumer が関連付けられるサービスのインス
     * タンス名およびタイプ名として、引数に instance_name, type_name お
     * よび Consumer 自身を与えることにより、内部でこれらが関連付けられ
     * る。Port 間の接続 (connect) 時 には、subscribeInterfaces() で述
     * べられているルールに基づき、Provider Interface の参照が自動的に
     * Consumer にセットされる。
     *
     * @param instance_name Consumer が要求するサービスのインスタンス名
     * @param type_name Consumer が要求するサービスのタイプ名
     * @param consumer CORBA サービスコンシューマ
     *
     * @return 既に同名の instance_name が登録されていれば false を返す。
     *
     * @else
     *
     * @brief Register the consumer
     *
     * This operation registers a consumer, which is a service
     * placeholder this port requires. These are associated internally
     * with specified instance_name, type_name and Consumer itself to
     * the argument as service's instance name and its type name
     * associated with Consumer.  The service Provider interface'
     * references will be set automatically to the Consumer Interface
     * object when connections are established, according to the rules
     * that are described at the subscribeInterface() function's
     * documentation.
     *
     * @param instance_name Instance name of the service Consumer requires
     * @param type_name Type name of the service Consumer requires
     * @param consumer CORBA service consumer
     *
     * @return False would be returned if the same instance_name was registered
     *
     * @endif
     */
    bool registerConsumer(const char* instance_name, const char* type_name,
                          CorbaConsumerBase& consumer);

  protected:
    /*!
     * @if jp
     *
     * @brief Provider Interface 情報を公開する
     *
     * この Port が所有する Provider インターフェースに関する情報を
     * ConnectorProfile::properties に代入し他の Port に対して公開する。
     * 今、RTCのインスタンス名等の情報が以下の通りであるとして、
     *
     * - RTCインスタンス名:              rtc_iname
     * - ポート名:                       port_name
     * - インターフェース極性:           if_polarity
     * - インターフェース型名:           if_tname
     * - インターフェースインスタンス名: if_iname
     *
     * NameValue 型の ConnectorProfile::properties の name と value として
     * 以下のものが格納される。
     *
     * - name
     *   <rtc_iname>.port.<port_name>.provided.<if_tname>.<if_iname>
     * - value
     *   Provider インターフェースの IOR 文字列
     *
     * なお、旧バージョンとの互換性のため以下の表記の NameValue も同時
     * に格納されるが、将来のバージョンでは削除される可能性がある。
     *
     * - name
     *   port.<if_tname>.<if_iname>
     * - value
     *   Provider インターフェースの IOR 文字列
     *
     * これらの値は ConnectorProfile::properties に格納され、他のポートに対して
     * 伝達される。他の Port でこのインターフェースを使用する Consumer が
     * 存在すれば、ConnectorProfile からこのキーからオブジェクトリファレンスを
     * 取得し何らかの形で使用される。
     *
     * @param connector_profile コネクタプロファイル
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Publish information about interfaces
     *
     * This operation publishes Provider interfaces information, which
     * is owned by this port, to the other Ports via
     * ConnectorProfile::properties.
     * Now it is assumed RTC instance name and other information is as follows,
     *
     * - RTC instance name:              rtc_iname
     * - Port name:                      port_name
     * - Interface polarity:             if_polarity
     * - Interface type name:            if_tname
     * - Interface instance name:        if_iname
     *
     * the following values are stored as the "name" and the "value"
     * of the NameValue typee element in ConnectorProfile::properties.
     *
     * - name
     *   <rtc_iname>.port.<port_name>.provided.<if_tname>.<if_iname>
     * - value
     *   IOR string value of interface reference
     *
     * In addition, although the following NameValue values are also
     * stored for the backward compatibility, this will be deleted in
     * the future version.
     *
     * - name
     *   port.<if_tname>.<if_iname>
     * - value
     *   IOR string value of interface reference
     *
     * These values are stored in the ConnectorProfile::properties and
     * are propagated to the other Ports. If the Consumer interface
     * exists that requires this Provider interface, it will retrieve
     * reference from the ConnectorProfile and utilize it.
     *
     * @param connector_profile Connector profile
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    virtual ReturnCode_t
    publishInterfaces(ConnectorProfile& connector_profile);

    /*!
     * @if jp
     *
     * @brief Provider Interface 情報を取得する
     *
     * この Portが所有する Consumer Interface に適合する Provider
     * Interface に関する情報をConnectorProfile::properties から抽出し
     * Consumer Interface にオブジェクト参照をセットする。
     *
     * 今、RTC のインスタンス名や Consumer Interface 等の情報が以下のと
     * おりであると仮定すると、
     *
     * - RTCインスタンス名:              rtc_iname
     * - ポート名:                       port_name
     * - インターフェース極性:           if_polarity
     * - インターフェース型名:           if_tname
     * - インターフェースインスタンス名: if_iname
     *
     * この Consumer Interface を表すインターフェース指定子は以下のよう
     * に表される。
     *
     * <rtc_iname>.port.<port_name>.required.<if_tname>.<if_iname>
     *
     * この関数は、まず引数 ConnectorProfile::properties に上記インター
     * フェース指定子をキーとして格納されている Provider Interface 指定
     * 子を探し出す。さらに、その Provider Interface 指定子をキーとして
     * 格納されている Provider Interface の参照を表す IOR 文字列を取得
     * し、Consumer Interface にセットする。
     *
     * 今、仮に、Provider を prov(n), その参照をIOR(n) さらに Consumer
     * をcons(n) のように記述し、これらすべてのインターフェースの型が同
     * 一であり、ConnectorProfile に以下の値が設定されているとする。
     *
     * <pre>
     * ConnectorProfile::properties =
     * {
     *   prov0: IOR0,
     *   prov1: IOR1,
     *   prov2: IOR2,
     *   cons0: prov2,
     *   cons1: prov1,
     *   cons2: prov0
     * }
     * </pre>
     *
     * このとき、cons(0..2) にはそれぞれ、参照が以下のようにセットされる。
     *
     * <pre>
     *   cons0 = IOR2
     *   cons1 = IOR1
     *   cons2 = IOR0
     * </pre>
     *
     * なお、旧バージョンとの互換性のため、
     * ConnectorProfile::properties に Consumer Interface をキーとした
     * 値がセットされていない場合でも、次のルールが適用される。
     *
     * 今、仮に Consumer Interface が
     *
     * <pre>
     *  PortInterfaceProfile
     *  {
     *    instance_name = "PA10_0";
     *    type_name     = "Manipulator";
     *    polarity      = REQUIRED;
     *  }
     * </pre>
     *
     * として登録されていれば、他の Port の
     *
     * <pre>
     *  PortInterfaceProfile
     *  {
     *    instance_name = "PA10_0";
     *    type_name     = "Manipulator";
     *    polarity      = PROVIDED;
     *  }
     * </pre>
     *
     * として登録されている Serivce Provider のオブジェクト参照を探し、
     * Consumer にセットする。実際には、ConnectorProfile::properties に
     *
     * <pre>
     * NameValue = { "port.Manipulator.PA10_0": <Object reference> }
     * </pre>
     *
     * として登録されている NameValue を探し、そのオブジェクト参照を
     * Consumer にセットする。
     *
     * @param connector_profile コネクタプロファイル
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Subscribe to interface
     *
     * Retrieve information associated with Provider matches Consumer
     * owned by this port and set the object reference to Consumer.
     *
     * Now, Consumer is registered as the following:
     * <pre>
     *  PortInterfaceProfile
     *  {
     *    instance_name = "PA10_0";
     *    type_name     = "Manipulator";
     *    polarity      = REQUIRED;
     *  }
     * </pre>
     * Find the object reference of Serivce Provider that is registered as
     * the following of other ports:
     * <pre>
     *  PortInterfaceProfile
     *  {
     *    instance_name = "PA10_0";
     *    type_name     = "Manipulator";
     *    polarity      = PROVIDED;
     *  }
     * </pre>
     * and set to Consumer.
     * In fact, find NameValue that is registered as the following to
     * ConnectorProfile::properties:
     * <pre>
     * NameValue = { "port.Manipulator.PA10_0": <Object reference> }
     * </pre>
     * and set the object reference to Consumer.
     *
     * @param connector_profile Connector profile
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    virtual ReturnCode_t
    subscribeInterfaces(const ConnectorProfile& connector_profile);

    /*!
     * @if jp
     *
     * @brief Interface への接続を解除する
     *
     * 与えられた ConnectorProfile に関連する Consumer にセットされた
     * すべての Object を解放し接続を解除する。
     *
     * @param connector_profile コネクタプロファイル
     *
     * @else
     *
     * @brief Unsubscribe interfaces
     *
     * Release all Objects that was set in Consumer associated with the given
     * ConnectorProfile.
     *
     * @param connector_profile Connector profile
     *
     * @endif
     */
    virtual void
    unsubscribeInterfaces(const ConnectorProfile& connector_profile);

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
    virtual void activateInterfaces();

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
    virtual void deactivateInterfaces();

  protected:
    /*!
     * @if jp
     * @brief プロパティ
     * @else
     * @brief Properties
     * @endif
     */
    coil::Properties m_properties;

  private:
    class CorbaConsumerHolder;
    /*!
     * @if jp
     * @brief Consumer に合致する Provider を NVList の中から見つける
     *
     * NVList 中から CorbaConsumerHolder に保持されている Consumer に合
     * 致するキーを持つ Provider を見つけ、IOR を抽出しナローイングして
     * Consumer にセットする。対応するキーが存在しない、IOR が見つから
     * ない、ナローイングに失敗した場合、false を返す。
     *
     * @param nv Provider が含まれている ConnectorProfile::properties の NVList
     * @param cons Provider と対応する Consumer のホルダ
     *
     * @retrun bool Consumer に対応する Provider が見つからない場合 false
     *
     * @else
     * @brief Find out a provider corresponding to the consumer from NVList
     *
     * This function finds out a Provider with the key that is matched
     * with Cosumer's name in the CorbaConsumerHolder, extracts IOR
     * and performs narrowing into the Consumer and set it to the
     * Consumer. False is returned when there is no corresponding key
     * and IOR and the narrowing failed.
     *
     * @param nv NVlist of ConnectorProfile::properties that includes Provider
     * @param cons a Consumer holder to be matched with a Provider
     *
     * @return bool false is returned if there is no provider for the consumer
     *
     * @endif
     */
    virtual bool findProvider(const NVList& nv, CorbaConsumerHolder& cons,
                              std::string& iorstr);

    /*!
     * @if jp
     * @brief Consumer に合致する Provider を NVList の中から見つける
     *
     * この関数は、古いバージョンの互換性のための関数である。
     *
     * NVList 中から CorbaConsumerHolder に保持されている Consumer に合
     * 致するキーを持つ Provider を見つける。対応するキーが存在しない、
     * IOR が見つからない場合、false を返す。
     *
     * @param nv Provider が含まれている ConnectorProfile::properties の NVList
     * @param cons Provider と対応する Consumer のホルダ
     * @param iorstr 見つかったIOR文字列を格納する変数
     *
     * @retrun bool Consumer に対応する Provider が見つからない場合 false
     *
     * @else
     * @brief Find out a provider corresponding to the consumer from NVList
     *
     * This function is for the old version's compatibility.
     *
     * This function finds out a Provider with the key that is matched
     * with Cosumer's name in the CorbaConsumerHolder and extracts
     * IOR.  False is returned when there is no corresponding key and
     * IOR.
     *
     * @param nv NVlist of ConnectorProfile::properties that includes Provider
     * @param cons a Consumer holder to be matched with a Provider
     * @param iorstr variable which is set IOR string
     *
     * @return bool false is returned if there is no provider for the consumer
     *
     * @endif
     */
    virtual bool findProviderOld(const NVList&nv, CorbaConsumerHolder& cons,
                                 std::string& iorstr);

    /*!
     * @if jp
     * @brief Consumer に IOR をセットする
     *
     * IOR をナローイングしてConsumer にセットする。ナローイングに失敗
     * した場合、false を返す。ただし、IOR文字列が、nullまたはnilの場合、
     * オブジェクトに何もセットせずに true を返す。
     *
     * @param ior セットする IOR 文字列
     * @param cons Consumer のホルダ
     *
     * @retrun bool Consumer へのナローイングに失敗した場合 false
     *
     * @else
     * @brief Setting IOR to Consumer
     *
     * This function performs narrowing into the Consumer and set it to the
     * Consumer. False is returned when the narrowing failed. But, if IOR
     * string is "null" or "nil", this function returns true.
     *
     * @param ior IOR string
     * @param cons Consumer holder
     *
     * @retrun bool false if narrowing failed.
     *
     * @endif
     */
    bool setObject(const std::string& ior, CorbaConsumerHolder& cons);

    /*!
     * @if jp
     * @brief Consumer のオブジェクトをリリースする
     *
     * Consumer にセットされた参照をリリースする。ConsumerのIORが与えら
     * れたIOR文字列と異なる場合、falseを返す。
     *
     * @param ior セットする IOR 文字列
     * @param cons Consumer のホルダ
     *
     * @retrun ConsumerのIORが与えられたIOR文字列と異なる場合、falseを返す。
     *
     * @else
     * @brief Releasing Consumer Object
     *
     * This function releases object reference of Consumer. If the
     * given IOR string is different from Consumer's IOR string, it
     * returns false.
     *
     * @param ior IOR string
     * @param cons Consumer holder
     *
     * @retrun bool False if IOR and Consumer's IOR are different
     *
     * @endif
     */
    bool releaseObject(const std::string& ior, CorbaConsumerHolder& cons);

  private:
    /*!
     * @if jp
     * @class CorbaProviderHolder
     * @brief Provider の情報を格納する構造体
     *
     * CORBA Provider のホルダクラス
     *
     * @else
     * @class CorbaProviderHolder
     * @brief The structure to be stored Provider information.
     *
     * CORBA Provider holder class
     *
     * @endif
     */
    class CorbaProviderHolder
    {
    public:
      CorbaProviderHolder(const char* type_name,
                          const char* instance_name,
                          PortableServer::RefCountServantBase* servant)
        : m_typeName(type_name),
          m_instanceName(instance_name),
          m_servant(servant)
          
      {
#ifndef ORB_IS_RTORB
#ifdef ORB_IS_OMNIORB
        m_poa = ::RTC::Manager::instance().getShortCutPOA();
        m_poa->activate_object(m_servant);
#else
        m_poa = ::RTC::Manager::instance().getPOA();
#endif
        m_oid = m_poa->servant_to_id(m_servant);
        try
          {
            m_poa->activate_object_with_id(m_oid, m_servant);
          }
        catch(...)
          {
          }
        CORBA::Object_var obj;
        obj = m_poa->id_to_reference(m_oid);
        CORBA::ORB_var orb = Manager::instance().getORB();
        CORBA::String_var ior_var = orb->object_to_string(obj);
        m_ior = ior_var;
        deactivate();
#else  // ORB_IS_RTORB
        // why RtORB does not activate object by __this()
        // and does not deactivate at the end of ctor?
        PortableServer::POA_var poa = ::RTC::Manager::instance().getPOA();
        m_oid = poa->servant_to_id(m_servant);
        CORBA::Object_var obj;
        obj = CORBA::Object_var(m_servant->__this());
        CORBA::ORB_var orb = Manager::instance().getORB();
        CORBA::String_var ior_var = orb->object_to_string(obj);
        m_ior = ior_var;
#endif  // ORB_IS_RTORB
      }
      virtual ~CorbaProviderHolder()
      {
        deactivate();
      }
      std::string instanceName() { return m_instanceName; }
      std::string typeName() { return m_typeName; }
      std::string ior() { return m_ior; }
      std::string descriptor() { return m_typeName + "." + m_instanceName; }

      void activate()
      {
        try
          {
            m_poa->activate_object_with_id(m_oid, m_servant);
          }
        catch(const ::PortableServer::POA::ServantAlreadyActive &)
          {
          }
        catch(...)
          {
          }
      }
      void deactivate()
      {
        try
          {
            m_poa->deactivate_object(m_oid);
          }
        catch(...)
          {
          }
      }
    private:
      std::string m_typeName;
      std::string m_instanceName;
      PortableServer::RefCountServantBase* m_servant;
      PortableServer::ObjectId_var m_oid;
      std::string m_ior;
      PortableServer::POA_var m_poa;
    };

    /*!
     * @if jp
     * @brief Provider の情報を格納する vector
     * @else
     * @brief vector to stored Providers' information
     * @endif
     */
    typedef std::vector<CorbaProviderHolder> CorbaProviderList;
    CorbaProviderList m_providers;

    /*!
     * @if jp
     * @brief Consumer の情報を格納する構造体
     * @else
     * @brief The structure to be stored Consumer information.
     * @endif
     */
    class CorbaConsumerHolder
    {
    public:
      CorbaConsumerHolder(const char* type_name,
                          const char* instance_name,
                          CorbaConsumerBase* consumer)
        : m_typeName(type_name),
          m_instanceName(instance_name),
          m_consumer(consumer),
          m_ior("")
      {
      }
      std::string instanceName() { return m_instanceName; }
      std::string typeName() { return m_typeName; }
      std::string descriptor() { return m_typeName + "." + m_instanceName; }

      bool setObject(const char* ior)
      {
        m_ior = ior;
        CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
        CORBA::Object_var obj = orb->string_to_object(ior);
        if (CORBA::is_nil(obj))
          {
            return false;
          }

        return m_consumer->setObject(obj.in());
      }
      void releaseObject()
      {
        m_consumer->releaseObject();
      }
      const std::string& getIor()
      {
        return m_ior;
      }
    private:
      std::string m_typeName;
      std::string m_instanceName;
      CorbaConsumerBase* m_consumer;
      std::string m_ior;
    };
    typedef std::vector<CorbaConsumerHolder> CorbaConsumerList;
    CorbaConsumerList m_consumers;

    // functors
    /*!
     * @if jp
     * @brief Consumer のオブジェクトを解放するための Functor
     * @else
     * @brief Functor to release Consumer's object
     * @endif
     */
    struct unsubscribe
    {
      explicit unsubscribe(CorbaConsumerList& consumers)
        : m_consumers(consumers)
      {
      }

      void operator()(const SDOPackage::NameValue& nv)
      {
        for (CorbaConsumerList::iterator it(m_consumers.begin());
             it != m_consumers.end(); ++it)
          {
            if (it->descriptor() == (const char*)nv.name)
              {
                it->releaseObject();
              }
          }
      }
      CorbaConsumerList& m_consumers;
    };
  };
};  // namespace RTC
#endif  // RTC_CORBAPORT_H
