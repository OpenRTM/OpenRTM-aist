// -*- C++ -*-
/*!
 * @file ConnectorListener.h
 * @brief connector listener class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009-2017
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_CONNECTORLISTENER_H
#define RTC_CONNECTORLISTENER_H

#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <rtm/RTC.h>
#include <rtm/ConnectorBase.h>
#include <rtm/ByteData.h>
#include <rtm/CORBA_CdrMemoryStream.h>

#include <string>
#include <vector>
#include <utility>



namespace RTC
{
  class ConnectorInfo;
  /*!
   * @if jp
   * @class ConnectorListenerStatus mixin class
   * @brief ConnectorListenerStatus mixin クラス
   *
   * このクラスは、enum定義されたリターンコードを、ConnectorListener関
   * 連のサブクラスで共通利用するための mixin クラスである。このリター
   * ンコードを使用するクラスでは、ConnectorListenerStatus クラスを
   * public 継承し、下にdefine してある CONNLISTENER_STATUS_ENUM をクラ
   * ス内に記述することで利用可能となる。これにより、enum を
   * ReturnCode 型として typedef し、以後 ReturnCode を利用できるように
   * するとともに、名前空間に enum 定義された各識別子を当該クラス名前空
   * 間内に導入する。
   *
   * @else
   * @class DataPortStatus mixin class
   * @brief DataPortStatus mixin class
   *
   * This is a mixin class to provide enumed return codes that are
   * commonly utilised in connector listener related sub-classes. To
   * use this class, sub-class should inherit this class as a public
   * super class, and declare CONNLISTENERSTATUS_ENUM defined
   * below. Consequently, ReturnCode type that is typedefed by this
   * macro can be used in the sub-class, and enumed identifiers are
   * imported to the class's namespace.
   *
   * @endif
   */
  class ConnectorListenerStatus
  {
  public:
    enum Enum
      {
        NO_CHANGE    = 0,
        INFO_CHANGED = 1 << 0,
        DATA_CHANGED = 1 << 1,
        BOTH_CHANGED = INFO_CHANGED | DATA_CHANGED
      };
  };
  ConnectorListenerStatus::Enum operator|(ConnectorListenerStatus::Enum L,
                                          ConnectorListenerStatus::Enum R);
  ConnectorListenerStatus::Enum operator&(ConnectorListenerStatus::Enum L,
                                          ConnectorListenerStatus::Enum R);

  /*!
   * @if jp
   *
   * @brief ::RTC::ConnDataPortStatus 導入
   *
   * ::RTC::DataPortStatus で宣言されている Enum のすべてのメンバをネーム
   * スペースに導入するためのマクロ。DataPortStatus を利用するクラスにお
   * いて、クラス宣言の先頭において DATAPORTSTATUS_ENUM を記載するだけで、
   * DataPortStatus で宣言されている enum メンバが名前解決演算子なしにア
   * クセス可能になる。
   *
   * @else
   *
   * @brief Importing ::RTC::DataPortStatus macro
   *
   * This macro imports all the member of enum declared in
   * ::RTC::DataPortStatus into the current namespace.  Inserting at the
   * head of class declaration, classes which utilize DataPortStatus can
   * access Enum members of DataPortStatus without using namespace
   * resolve operator.
   *
   * @endif
   */
#define USE_CONNLISTENER_STATUS                             \
  typedef ::RTC::ConnectorListenerStatus::Enum ReturnCode;  \
  using ::RTC::ConnectorListenerStatus::NO_CHANGE;          \
  using ::RTC::ConnectorListenerStatus::INFO_CHANGED;       \
  using ::RTC::ConnectorListenerStatus::DATA_CHANGED;       \
  using ::RTC::ConnectorListenerStatus::BOTH_CHANGED

  /*!
   * @if jp
   * @brief ConnectorDataListener のタイプ
   *
   * - ON_BUFFER_WRITE:          バッファ書き込み時
   * - ON_BUFFER_FULL:           バッファフル時
   * - ON_BUFFER_WRITE_TIMEOUT:  バッファ書き込みタイムアウト時
   * - ON_BUFFER_OVERWRITE:      バッファ上書き時
   * - ON_BUFFER_READ:           バッファ読み出し時
   * - ON_SEND:                  InProtへの送信時
   * - ON_RECEIVED:              InProtへの送信完了時
   * - ON_RECEIVER_FULL:         InProt側バッファフル時
   * - ON_RECEIVER_TIMEOUT:      InProt側バッファタイムアウト時
   * - ON_RECEIVER_ERROR:        InProt側エラー時
   *
   * @else
   * @brief The types of ConnectorDataListener
   * - ON_BUFFER_READ:           At the time of buffer read
   * - ON_BUFFER_WRITE:          At the time of buffer write
   * - ON_BUFFER_FULL:           At the time of buffer full
   * - ON_BUFFER_WRITE_TIMEOUT:  At the time of buffer write timeout
   * - ON_BUFFER_OVERWRITE:      At the time of buffer overwrite
   * - ON_BUFFER_READ:           At the time of buffer read
   * - ON_SEND:                  At the time of sending to InPort
   * - ON_RECEIVED:              At the time of finishing sending to InPort
   * - ON_RECEIVER_FULL:         At the time of bufferfull of InPort
   * - ON_RECEIVER_TIMEOUT:      At the time of timeout of InPort
   * - ON_RECEIVER_ERROR:        At the time of error of InPort
   *
   * @endif
   */
  enum ConnectorDataListenerType
    {
      ON_BUFFER_WRITE = 0,
      ON_BUFFER_FULL,
      ON_BUFFER_WRITE_TIMEOUT,
      ON_BUFFER_OVERWRITE,
      ON_BUFFER_READ,
      ON_SEND,
      ON_RECEIVED,
      ON_RECEIVER_FULL,
      ON_RECEIVER_TIMEOUT,
      ON_RECEIVER_ERROR,
      CONNECTOR_DATA_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class ConnectorDataListener クラス
   * @brief ConnectorDataListener クラス
   *
   * データポートの Connector において発生する各種イベントに対するコー
   * ルバックを実現するリスナクラスの基底クラス。
   *
   * コアロジックがOutPortに対してデータ書き込み、InPort側でデータが取
   * 得されるまでの間で発生する各種イベントをフックするコールバックを設
   * 定することができる。なお、リスナークラスは2種類存在し、バッファフ
   * ルや送信時のコールバックで、その時点で有効なデータをファンクタの引
   * 数として受け取る ConnectorDataListener であり、もう一方はデータエ
   * ンプティやバッファ読み込み時のタイムアウトなどデータが取得できない
   * 場合などにコールされるファンクタの引数に何もとらならい
   * ConnecotorListener がある。
   *
   * ConnectorDataListener クラスによってデータが関連する動作をフックし
   * たい場合、以下の例のように、このクラスを継承し、コネクタの情報とデー
   * タを引数に取る以下のようなコールバックオブジェクトを定義し、データ
   * ポートの適切なコールバック設定関数からコールバックオブジェクトをセッ
   * トする必要がある。なお、Data Listener には、データとしてマーシャリ
   * ング後のデータを引数に取る ConnectorDataListener と、データを特定
   * の型にアンマーシャルした後の値を引数に取る、
   * ConnectorDataListenerT がある。以下の例は、ConnectorDataListenerT
   * の定義例である。
   *
   * <pre>
   * class MyDataListener
   *   : public ConnectorDataListenerT<RTC::TimedLong>
   * {
   * public:
   *   MyDataListener(const char* name) : m_name(name) {}
   *   virtual ~MyDataListener() {}
   *   virtual ReturnCode operator()(ConnectorInfo& info, TimedLong& data)
   *   {
   *     std::cout << "Data Listener: " << m_name       << std::endl;
   *     std::cout << "Data:          " << data.data    << std::endl;
   *     std::cout << "Profile::name: " << info.name    << std::endl;
   *     std::cout << "Profile::id:   " << info.id      << std::endl;
   *     std::cout << "Profile::properties: "           << std::endl;
   *     std::cout << info.properties;
   *   };
   *   std::string m_name;
   * };
   * </pre>
   *
   * このようにして定義されたリスナクラスは、以下のようにデータポートに
   * 対して、以下のようにセットされる。
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     m_outOut.
   *         addConnectorDataListener(ON_BUFFER_WRITE,
   *                                  new MyDataListener("ON_BUFFER_WRITE"));
   *    :
   * </pre>
   *
   * 第1引数の "ON_BUFFER_WRITE" は、コールバックをフックするポイントで
   * あり、以下に列挙する値を取ることが可能である。データポートには、接
   * 続時にデータの送受信方法について、インターフェース型、データフロー
   * 型、サブスクリプション型等を設定することができるが、これらの設定に
   * よりフックされるポイントは異なる。以下に、インターフェースがCORBA
   * CDR型の場合のコールバック一覧を示す。
   *
   * OutPort:
   * -  Push型: Subscription Typeによりさらにイベントの種類が分かれる。
   *   - Flush: Flush型にはバッファがないため ON_BUFFER 系のイベントは発生しない
   *     - ON_SEND
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   *   - New型
   *     - ON_BUFFER_WRITE
   *     - ON_BUFFER_FULL
   *     - ON_BUFFER_WRITE_TIMEOUT
   *     - ON_BUFFER_OVERWRITE
   *     - ON_BUFFER_READ
   *     - ON_SEND
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   *   - Periodic型
   *     - ON_BUFFER_WRITE
   *     - ON_BUFFER_FULL
   *     - ON_BUFFER_WRITE_TIMEOUT
   *     - ON_BUFFER_READ
   *     - ON_SEND
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   *   .
   * - Pull型
   *   - ON_BUFFER_WRITE
   *   - ON_BUFFER_FULL
   *   - ON_BUFFER_WRITE_TIMEOUT
   *   - ON_BUFFER_OVERWRITE
   *   - ON_BUFFER_READ
   *   - ON_SEND
   *   .
   * InPort:
   * - Push型:
   *     - ON_BUFFER_WRITE
   *     - ON_BUFFER_FULL
   *     - ON_BUFFER_WRITE_TIMEOUT
   *     - ON_BUFFER_WRITE_OVERWRITE
   *     - ON_BUFFER_READ
   *     - ON_BUFFER_READ_TIMEOUT
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   * - Pull型
   *     - ON_BUFFER_READ
   *
   * @else
   * @class ConnectorDataListener class
   * @brief ConnectorDataListener class
   *
   * This class is abstract base class for listener classes that
   * realize callbacks for various events in the data port's
   * connectors.
   *
   * Callbacks can be hooked to the various kind of events which occur
   * throgh OutPort side data write action to InPort side data-read
   * action.  Two types listener classes exist. One is
   * ConnectorDataListener which receives valid data-port's data value
   * at that time such as buffer-full event, data-send event, and so
   * on. Other is ConnectorListener which does not receive any data
   * such as buffer-empty event, buffer-read-timeout event and so on.
   *
   * If you want to hook actions which related data-port's data by
   * ConnectorDataListener, a class which inherits this class should
   * be defined, and the functor should receive a connector
   * information and a data value as arguments. And then, the defined
   * class must be set to data-port object through its member
   * function, as follows.  Two types of ConnectorDataListeners are
   * available. One is "ConnectorDataListener" which receives
   * marshalled data as data value, the other is
   * "ConnectorDataListenerT" which receives unmarshalled data as data
   * value. The following example is the use of ConnectorDataListenerT.
   *
   * <pre>
   * class MyDataListener
   *   : public ConnectorDataListenerT<RTC::TimedLong>
   * {
   * public:
   *   MyDataListener(const char* name) : m_name(name) {}
   *   virtual ~MyDataListener() {}
   *   virtual ReturnCode operator()(ConnectorInfo& info, TimedLong& data)
   *   {
   *     std::cout << "Data Listener: " << m_name       << std::endl;
   *     std::cout << "Data:          " << data.data    << std::endl;
   *     std::cout << "Profile::name: " << info.name    << std::endl;
   *     std::cout << "Profile::id:   " << info.id      << std::endl;
   *     std::cout << "Profile::properties: "           << std::endl;
   *     std::cout << info.properties;
   *   };
   *   std::string m_name;
   * };
   * </pre>
   *
   * The listener class defained as above can be attached to a
   * data-port as follows.
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     m_outOut.
   *         addConnectorDataListener(ON_BUFFER_WRITE,
   *                                  new MyDataListener("ON_BUFFER_WRITE"));
   *    :
   * </pre>
   *
   * The first argument "ON_BUFFER_WRITE" specifies hook point of
   * callback, and the following values are available. Data-port can
   * be specified some properties such as interface-type,
   * dataflow-type, subscription type and so on. Available hook points
   * vary by the those settings. The following hook points are
   * available when interface type is CORBA CDR type.
   *
   * OutPort:
   * -  Push type: Available hook event varies by subscription type.
   *   - Flush: No ON_BUFFER* events since flush-type has no buffer.
   *     - ON_SEND
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   *   - New type:
   *     - ON_BUFFER_WRITE
   *     - ON_BUFFER_FULL
   *     - ON_BUFFER_WRITE_TIMEOUT
   *     - ON_BUFFER_OVERWRITE
   *     - ON_BUFFER_READ
   *     - ON_SEND
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   *   - Periodic type:
   *     - ON_BUFFER_WRITE
   *     - ON_BUFFER_FULL
   *     - ON_BUFFER_WRITE_TIMEOUT
   *     - ON_BUFFER_READ
   *     - ON_SEND
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   *   .
   * - Pull type:
   *   - ON_BUFFER_WRITE
   *   - ON_BUFFER_FULL
   *   - ON_BUFFER_WRITE_TIMEOUT
   *   - ON_BUFFER_OVERWRITE
   *   - ON_BUFFER_READ
   *   - ON_SEND
   *   .
   * InPort:
   * - Push type:
   *     - ON_BUFFER_WRITE
   *     - ON_BUFFER_FULL
   *     - ON_BUFFER_WRITE_TIMEOUT
   *     - ON_BUFFER_WRITE_OVERWRITE
   *     - ON_BUFFER_READ
   *     - ON_BUFFER_READ_TIMEOUT
   *     - ON_RECEIVED
   *     - ON_RECEIVER_FULL
   *     - ON_RECEIVER_TIMEOUT
   *     - ON_RECEIVER_ERROR
   *     .
   * - Pull type
   *     - ON_BUFFER_READ
   *
   * @endif
   */
  class ConnectorDataListener
    : public ConnectorListenerStatus
  {
  public:
    USE_CONNLISTENER_STATUS;
    /*!
     * @if jp
     *
     * @brief ConnectorDataListenerType を文字列に変換
     *
     * ConnectorDataListenerType を文字列に変換する
     *
     * @param type 変換対象 ConnectorDataListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert ConnectorDataListenerType into the string.
     *
     * Convert ConnectorDataListenerType into the string.
     *
     * @param type The target ConnectorDataListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(ConnectorDataListenerType type)
    {
      if (type < CONNECTOR_DATA_LISTENER_NUM)
        {
          static const char* typeString[] =
          {
            "ON_BUFFER_WRITE",
            "ON_BUFFER_FULL",
            "ON_BUFFER_WRITE_TIMEOUT",
            "ON_BUFFER_OVERWRITE",
            "ON_BUFFER_READ",
            "ON_SEND",
            "ON_RECEIVED",
            "ON_RECEIVER_FULL",
            "ON_RECEIVER_TIMEOUT",
            "ON_RECEIVER_ERROR",
            "CONNECTOR_DATA_LISTENER_NUM"
          };
          return typeString[type];
        }
      return "";
    }

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ConnectorDataListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバックメソッド
     *
     * データポートの Connector において発生する各種イベントに対するコー
     * ルバックメソッド
     *
     * @else
     *
     * @brief Virtual Callback method
     *
     * This is a the Callback method to various events generated in Connector.
     *
     * @endif
     */
    virtual ReturnCode operator()(ConnectorInfo& info,
                            ByteData& data) = 0;
  };

  /*!
   * @if jp
   * @class ConnectorDataListenerT クラス
   * @brief ConnectorDataListenerT クラス
   *
   * データポートの Connector において発生する各種イベントに対するコー
   * ルバックを実現するリスナクラスの基底クラス。
   *
   * このクラスは、operator()() の第2引数に cdrMemoryStream 型ではなく、
   * 実際にデータポートで使用される変数型をテンプレート引数として
   * 渡すことができる。
   *
   * @else
   * @class ConnectorDataListenerT class
   * @brief ConnectorDataListenerT class
   *
   * This class is abstract base class for listener classes that
   * provides callbacks for various events in the data port's
   * connectors.
   *
   * This class template can have practical data types that are used
   * as typed variable for DataPort as an argument of template instead
   * of cdrMemoryStream.
   *
   * @endif
   */ 
  template <class DataType>
  class ConnectorDataListenerT
    : public ConnectorDataListener
  {
  public:
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~ConnectorDataListenerT() override {}

    /*!
     * @if jp
     *
     * @brief コールバックメソッド
     *
     * データをデータポートで使用される変数型に変換して ConnectorDataListenerT
     * のコールバックメソッドを呼び出す。
     *
     * @param info ConnectorInfo
     * @param cdrdata cdrMemoryStream型のデータ
     *
     * @else
     *
     * @brief Callback method
     *
     * This method invokes the callback method of ConnectorDataListenerT.
     * Data is converted into the variable type used in DataPort.
     *
     * @param info ConnectorInfo
     * @param cdrdata Data of cdrMemoryStream type
     *
     * @endif
     */
    ReturnCode operator()(ConnectorInfo& info,
                                  ByteData& cdrdata) override
    {
      DataType data;
      std::string marshaling_type = info.properties.getProperty("marshaling_type", "corba");
      coil::eraseBothEndsBlank(marshaling_type);
      
      ByteDataStream<DataType> *cdr = coil::GlobalFactory < ::RTC::ByteDataStream<DataType> >::instance().createObject(marshaling_type);
      
      
      if (!cdr)
      {
          
          return NO_CHANGE;
      }
      
      
      cdr->writeData(cdrdata.getBuffer(), cdrdata.getDataLength());
      // endian type check
      std::string endian_type;
      endian_type = info.properties.getProperty("serializer.cdr.endian",
                                                "little");
      coil::normalize(endian_type);
      std::vector<std::string> endian(coil::split(endian_type, ","));

      if (endian[0] == "little")
      {
          cdr->isLittleEndian(true);
      }
      else if (endian[0] == "big")
      {
          cdr->isLittleEndian(false);
      }

      cdr->deserialize(data);

      ReturnCode ret = this->operator()(info, data);
      if (ret == DATA_CHANGED || ret == BOTH_CHANGED)
      {
          if (endian[0] == "little")
          {
              cdr->isLittleEndian(true);
          }
          else if (endian[0] == "big")
          {
              cdr->isLittleEndian(false);
          }

          cdr->serialize(data);
      }

      cdrdata.setDataLength(cdr->getDataLength());
      cdr->readData(cdrdata.getBuffer(), cdrdata.getDataLength());

      coil::GlobalFactory < ::RTC::ByteDataStream<DataType> >::instance().deleteObject(cdr);
  
      return ret;
    }

    /*!
     * @if jp
     *
     * @brief 仮想コールバックメソッド
     *
     * データポートの Connector において発生する各種イベントに対するコー
     * ルバックメソッド
     *
     * @else
     *
     * @brief Virtual Callback method
     *
     * This method invokes the callback method of ConnectorDataListenerT.
     * Data is converted into the variable type used in DataPort.
     *
     * @endif
     */
    virtual ReturnCode operator()(ConnectorInfo& info,
                                 DataType& data) = 0;
  };

  /*!
   * @if jp
   * @brief ConnectorListener のタイプ
   *
   * - ON_BUFFER_EMPTY:       バッファが空の場合
   * - ON_BUFFER_READTIMEOUT: バッファが空でタイムアウトした場合
   * - ON_SENDER_EMPTY:       OutPort側バッファが空
   * - ON_SENDER_TIMEOUT:     OutPort側タイムアウト時
   * - ON_SENDER_ERROR:       OutPort側エラー時
   * - ON_CONNECT:            接続確立時
   * - ON_DISCONNECT:         接続切断時
   *
   * @else
   * @brief The types of ConnectorListener
   *
   * - ON_BUFFER_EMPTY:       At the time of buffer empty
   * - ON_BUFFER_READTIMEOUT: At the time of buffer read timeout
   * - ON_BUFFER_EMPTY:       At the time of empty of OutPort
   * - ON_SENDER_TIMEOUT:     At the time of timeout of OutPort
   * - ON_SENDER_ERROR:       At the time of error of OutPort
   * - ON_CONNECT:            At the time of connection
   * - ON_DISCONNECT:         At the time of disconnection
   *
   * @endif
   */
  enum ConnectorListenerType
    {
      ON_BUFFER_EMPTY = 0,
      ON_BUFFER_READ_TIMEOUT,
      ON_SENDER_EMPTY,
      ON_SENDER_TIMEOUT,
      ON_SENDER_ERROR,
      ON_CONNECT,
      ON_DISCONNECT,
      CONNECTOR_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class ConnectorListener クラス
   * @brief ConnectorListener クラス
   *
   * データポートの Connector において発生する各種イベントに対するコー
   * ルバックを実現するリスナクラスの基底クラス。
   *
   * コアロジックがOutPortに対してデータ書き込み、InPort側でデータが取
   * 得されるまでの間で発生する各種イベントをフックするコールバックを設
   * 定することができる。なお、リスナークラスは2種類存在し、バッファフ
   * ルや送信時のコールバックで、その時点で有効なデータをファンクタの引
   * 数として受け取る ConnectorDataListener であり、もう一方はデータエ
   * ンプティやバッファ読み込み時のタイムアウトなどデータが取得できない
   * 場合などにコールされるファンクタの引数に何もとらならい
   * ConnecotorListener がある。
   *
   * ConnectorListener クラスによって関連する動作をフックしたい場合、以
   * 下の例のように、このクラスを継承し、コネクタの情報を引数に取る以下
   * のようなコールバックオブジェクトを定義し、データポートの適切なコー
   * ルバック設定関数からコールバックオブジェクトをセットする必要がある。
   *
   * <pre>
   * class MyListener
   *   : public ConnectorListener
   * {
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *   virtual ReturnCode operator()(ConnectorInfo& info)
   *   {
   *     std::cout << "Data Listener: " << m_name       << std::endl;
   *     std::cout << "Profile::name: " << info.name    << std::endl;
   *     std::cout << "Profile::id:   " << info.id      << std::endl;
   *     std::cout << "Profile::properties: "           << std::endl;
   *     std::cout << info.properties;
   *   };
   *   std::string m_name;
   * };
   * </pre>
   *
   * このようにして定義されたリスナクラスは、以下のようにデータポートに
   * 対して、以下のようにセットされる。
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     m_outOut.
   *         addConnectorListener(ON_BUFFER_EMPTY,
   *                              new MyListener("ON_BUFFER_EMPTY"));
   *    :
   * </pre>
   *
   * 第1引数の "ON_BUFFER_EMPTY" は、コールバックをフックするポイントで
   * あり、以下に列挙する値を取ることが可能である。データポートには、接
   * 続時にデータの送受信方法について、インターフェース型、データフロー
   * 型、サブスクリプション型等を設定することができるが、これらの設定に
   * よりフックされるポイントは異なる。以下に、インターフェースがCORBA
   * CDR型の場合のコールバック一覧を示す。
   *
   * OutPort:
   * -  Push型: Subscription Typeによりさらにイベントの種類が分かれる。
   *   - Flush: Flush型にはバッファがないため ON_BUFFER 系のイベントは発生しない
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   *   - New型
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   *   - Periodic型
   *     - ON_BUFFER_EMPTY
   *     - ON_BUFFER_READ_TIMEOUT
   *     - ON_SENDER_EMPTY
   *     - ON_SENDER_ERROR
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   *   .
   * - Pull型
   *   - ON_BUFFER_EMPTY
   *   - ON_BUFFER_READ_TIMEOUT
   *   - ON_SENDER_EMPTY
   *   - ON_SENDER_TIMEOUT
   *   - ON_SENDER_ERROR
   *   - ON_CONNECT
   *   - ON_DISCONNECT
   *   .
   * InPort:
   * - Push型:
   *     - ON_BUFFER_EMPTY
   *     - ON_BUFFER_READ_TIMEOUT
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   * - Pull型
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *
   * @else
   * @class ConnectorListener class
   * @brief ConnectorListener class
   *
   * This class is abstract base class for listener classes that
   * realize callbacks for various events in the data port's
   * connectors.
   *
   * Callbacks can be hooked to the various kind of events which occur
   * throgh OutPort side data write action to InPort side data-read
   * action. Two types listener classes exist. One is
   * ConnectorDataListener which receives valid data-port's data value
   * at that time such as buffer-full event, data-send event, and so
   * on. Other is ConnectorListener which does not receive any data
   * such as buffer-empty event, buffer-read-timeout event and so on.
   *
   * If you want to hook related actions by
   * ConnectorListener, a class which inherits this class should
   * be defined, and the functor should receive a connector
   * information as an argument. And then, the defined
   * class must be set to data-port object through its member
   * function, as follows.
   *
   * <pre>
   * class MyListener
   *   : public ConnectorListener
   * {
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *   virtual ReturnCode operator()(ConnectorInfo& info)
   *   {
   *     std::cout << "Data Listener: " << m_name       << std::endl;
   *     std::cout << "Profile::name: " << info.name    << std::endl;
   *     std::cout << "Profile::id:   " << info.id      << std::endl;
   *     std::cout << "Profile::properties: "           << std::endl;
   *     std::cout << info.properties;
   *   };
   *   std::string m_name;
   * };
   * </pre>
   *
   * The listener class defained as above can be attached to a
   * data-port as follows.
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     m_outOut.
   *         addConnectorListener(ON_BUFFER_EMPTY,
   *                              new MyDataListener("ON_BUFFER_EMPTY"));
   *    :
   * </pre>
   *
   * The first argument "ON_BUFFER_EMPTY" specifies hook point of
   * callback, and the following values are available. Data-port can
   * be specified some properties such as interface-type,
   * dataflow-type, subscription type and so on. Available hook points
   * vary by the those settings. The following hook points are
   * available when interface type is CORBA CDR type.
   *
   * OutPort:
   * -  Push type: Available hook event varies by subscription type.
   *   - Flush: No ON_BUFFER* events since flush-type has no buffer.
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   *   - New type:
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   *   - Periodic type:
   *     - ON_BUFFER_EMPTY
   *     - ON_BUFFER_READ_TIMEOUT
   *     - ON_SENDER_EMPTY
   *     - ON_SENDER_ERROR
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   *   .
   * - Pull type:
   *   - ON_BUFFER_EMPTY
   *   - ON_BUFFER_READ_TIMEOUT
   *   - ON_SENDER_EMPTY
   *   - ON_SENDER_TIMEOUT
   *   - ON_SENDER_ERROR
   *   - ON_CONNECT
   *   - ON_DISCONNECT
   *   .
   * InPort:
   * - Push type:
   *     - ON_BUFFER_EMPTY
   *     - ON_BUFFER_READ_TIMEOUT
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *     .
   * - Pull type:
   *     - ON_CONNECT
   *     - ON_DISCONNECT
   *
   * @endif
   */
  class ConnectorListener
    : public ConnectorListenerStatus
  {
  public:
    USE_CONNLISTENER_STATUS;
    /*!
     * @if jp
     *
     * @brief ConnectorListenerType を文字列に変換
     *
     * ConnectorListenerType を文字列に変換する
     *
     * @param type 変換対象 ConnectorListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert ConnectorListenerType into the string.
     *
     * Convert ConnectorListenerType into the string.
     *
     * @param type The target ConnectorListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(ConnectorListenerType type)
    {
      if (type < CONNECTOR_LISTENER_NUM)
        {
          static const char* typeStr[] =
          {
            "ON_BUFFER_EMPTY",
            "ON_BUFFER_READ_TIMEOUT",
            "ON_SENDER_EMPTY",
            "ON_SENDER_TIMEOUT",
            "ON_SENDER_ERROR",
            "ON_CONNECT",
            "ON_DISCONNECT",
            "CONNECTOR_LISTENER_NUM"
          };
          return typeStr[type];
        }
      return "";
    }

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ConnectorListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバックメソッド
     *
     * データポートの Connector において発生する各種イベントに対するコー
     * ルバックメソッド
     *
     * @else
     *
     * @brief Virtual Callback method
     *
     * This method invokes the callback method of ConnectorDataListenerT.
     * Data is converted into the variable type used in DataPort.
     *
     * @endif
     */
    virtual ReturnCode operator()(ConnectorInfo& info) = 0;
  };


  /*!
   * @if jp
   * @class ConnectorDataListenerHolder
   * @brief ConnectorDataListener ホルダクラス
   *
   * 複数の ConnectorDataListener を保持し管理するクラス。
   *
   * @else
   * @class ConnectorDataListenerHolder
   * @brief ConnectorDataListener holder class
   *
   * This class manages one ore more instances of ConnectorDataListener class.
   *
   * @endif
   */
  class ConnectorDataListenerHolder
    : public ConnectorListenerStatus
  {
    typedef std::pair<ConnectorDataListener*, bool> Entry;
    typedef coil::Guard<coil::Mutex> Guard;
  public:
    USE_CONNLISTENER_STATUS;
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ConnectorDataListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ConnectorDataListenerHolder();

    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * リスナーを追加する。
     *
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @else
     *
     * @brief Add the listener.
     *
     * This method adds the listener.
     *
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @endif
     */
    void addListener(ConnectorDataListener* listener, bool autoclean);

    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * リスナを削除する。
     *
     * @param listener 削除するリスナ
     * @else
     *
     * @brief Remove the listener.
     *
     * This method removes the listener.
     *
     * @param listener Removed listener
     * @endif
     */
    void removeListener(ConnectorDataListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーの数を得る
     *
     * 現在登録されているリスナー数を得る。
     *
     * @return listener数
     * @else
     *
     * @brief Getting number of listeners.
     *
     * This method returns current number of listenrs.
     *
     * @return number of listeners
     * @endif
     */
    size_t size();

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param info ConnectorInfo
     * @param cdrdata データ
     * @else
     *
     * @brief Notify listeners.
     *
     * This calls the Callback method of the registered listener.
     *
     * @param info ConnectorInfo
     * @param cdrdata Data
     * @endif
     */
    ReturnCode notify(ConnectorInfo& info,
                ByteData& cdrdata);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(データ型指定版)
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     * ConnectorDataListenerT 型のコールバックのみコールされる。
     *
     * @param info ConnectorInfo
     * @param typeddata データ（データ型指定あり）
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * This calls the Callback method of the registered listener.
     * This operation calls only ConnectorDataListenerT type callback.
     *
     * @param info ConnectorInfo
     * @param typeddata Data
     * @endif
     */
    template <class DataType>
    ReturnCode notify(ConnectorInfo& info, DataType& typeddata)
    {
      Guard guard(m_mutex);
      ReturnCode ret(NO_CHANGE);

      std::string endian_type;
      endian_type = info.properties.getProperty("serializer.cdr.endian",
          "little");
      coil::normalize(endian_type);
      std::vector<std::string> endian(coil::split(endian_type, ","));

      

      for (auto & listener : m_listeners)
        {
          ConnectorDataListenerT<DataType>* datalistener(nullptr);
          datalistener =
          dynamic_cast<ConnectorDataListenerT<DataType>*>(listener.first);
          if (datalistener != nullptr)
            {
              ret = ret | datalistener->operator()(info, typeddata);
            }
          else
            {
              std::string marshaling_type = info.properties.getProperty("marshaling_type", "corba");
              coil::normalize(marshaling_type);

              ByteDataStream<DataType> *cdr = coil::GlobalFactory < ::RTC::ByteDataStream<DataType> >::instance().createObject(marshaling_type);

              
              if (endian[0] == "little")
              {
                  cdr->isLittleEndian(true);
              }
              else if (endian[0] == "big")
              {
                  cdr->isLittleEndian(false);
              }
              cdr->serialize(typeddata);
              ByteData tmp = *cdr;
              ret = ret | listener.first->operator()(info, tmp);
              coil::GlobalFactory < ::RTC::ByteDataStream<DataType> >::instance().deleteObject(cdr);
            }
        }
      return ret;
    }

  private:
    std::vector<Entry> m_listeners;
    coil::Mutex m_mutex;
  };


  /*!
   * @if jp
   * @class ConnectorListenerHolder
   * @brief ConnectorListener ホルダクラス
   *
   * 複数の ConnectorListener を保持し管理するクラス。
   *
   * @else
   * @class ConnectorListenerHolder
   * @brief ConnectorListener holder class
   *
   * This class manages one ore more instances of ConnectorListener class.
   *
   * @endif
   */
  class ConnectorListenerHolder
      : public ConnectorListenerStatus
  {
    typedef std::pair<ConnectorListener*, bool> Entry;
    typedef coil::Guard<coil::Mutex> Guard;
  public:
    USE_CONNLISTENER_STATUS;
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ConnectorListenerHolder();

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ConnectorListenerHolder();

    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * リスナーを追加する。
     *
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @else
     *
     * @brief Add the listener.
     *
     * This method adds the listener.
     *
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @endif
     */
    void addListener(ConnectorListener* listener, bool autoclean);

    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * リスナを削除する。
     *
     * @param listener 削除するリスナ
     * @else
     *
     * @brief Remove the listener.
     *
     * This method removes the listener.
     *
     * @param listener Removed listener
     * @endif
     */
    void removeListener(ConnectorListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーの数を得る
     *
     * 現在登録されているリスナー数を得る。
     *
     * @return listener数
     * @else
     *
     * @brief Getting number of listeners.
     *
     * This method returns current number of listenrs.
     *
     * @return number of listeners
     * @endif
     */
    size_t size();

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param info ConnectorInfo
     * @else
     *
     * @brief Notify listeners.
     *
     * This calls the Callback method of the registered listener.
     *
     * @param info ConnectonotifyrInfo
     * @endif
     */
    ReturnCode notify(ConnectorInfo& info);

  private:
    std::vector<Entry> m_listeners;
    coil::Mutex m_mutex;
  };

  /*!
   * @if jp
   * @class ConnectorListeners
   * @brief ConnectorListeners クラス
   *
   *
   * @else
   * @class ConnectorListeners
   * @brief ConnectorListeners class
   *
   *
   * @endif
   */
  class ConnectorListeners
  {
  public:
    /*!
     * @if jp
     * @brief ConnectorDataListenerTypeリスナ配列
     * ConnectorDataListenerTypeリスナを格納
     * @else
     * @brief ConnectorDataListenerType listener array
     * The ConnectorDataListenerType listener is stored.
     * @endif
     */
    ConnectorDataListenerHolder connectorData_[CONNECTOR_DATA_LISTENER_NUM];
    /*!
     * @if jp
     * @brief ConnectorListenerTypeリスナ配列
     * ConnectorListenerTypeリスナを格納
     * @else
     * @brief ConnectorListenerType listener array
     * The ConnectorListenerType listener is stored.
     * @endif
     */
    ConnectorListenerHolder connector_[CONNECTOR_LISTENER_NUM];
  };
} // namespace RTC

#endif // RTC_CONNECTORLISTENER_H
