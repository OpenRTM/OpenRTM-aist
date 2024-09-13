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

#include <mutex>
#include <rtm/RTC.h>
#include <rtm/ConnectorBase.h>
#include <rtm/ByteData.h>
#include <rtm/CORBA_CdrMemoryStream.h>

#include <string>
#include <vector>
#include <utility>
#include <array>


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
  using ReturnCode = ::RTC::ConnectorListenerStatus::Enum;  \
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
  enum class ConnectorDataListenerType : uint8_t
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
   *         addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE,
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
   *         addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE,
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
      if (type < ConnectorDataListenerType::CONNECTOR_DATA_LISTENER_NUM)
        {
          static const char* const typeString[] =
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
          return typeString[static_cast<uint8_t>(type)];
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
                            ByteData& data, const std::string& marshalingtype) = 0;
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
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ConnectorDataListenerT() = default;
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~ConnectorDataListenerT() override
    {
        SerializerFactory::instance().deleteObject(m_cdr);
    }

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
     * @param marshalingtype シリアライザの種類
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
     * @param marshalingtype 
     *
     * @endif
     */
    ReturnCode operator()(ConnectorInfo& info,
                                  ByteData& cdrdata, const std::string& marshalingtype) override
    {
      DataType data;

      if(m_cdr == nullptr || m_marshalingtype != marshalingtype)
      {
        m_cdr = createSerializer<DataType>(marshalingtype);
        m_marshalingtype = marshalingtype;
      }
      ::RTC::ByteDataStream<DataType> *cdr = dynamic_cast<::RTC::ByteDataStream<DataType>*>(m_cdr);
      

      if (!cdr)
      {
          return NO_CHANGE;
      }

      
      // endian type check
      std::string endian_type{coil::normalize(
        info.properties.getProperty("serializer.cdr.endian", "little"))};
      std::vector<std::string> endian(coil::split(endian_type, ","));

      if (endian[0] == "little")
      {
          cdr->isLittleEndian(true);
      }
      else if (endian[0] == "big")
      {
          cdr->isLittleEndian(false);
      }


      cdr->writeData(cdrdata.getBuffer(), cdrdata.getDataLength());

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
          cdrdata.setDataLength(cdr->getDataLength());
          cdr->readData(cdrdata.getBuffer(), cdrdata.getDataLength());
      }

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
  private:
      ByteDataStreamBase* m_cdr{nullptr};
      std::string m_marshalingtype;
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
  enum class ConnectorListenerType : uint8_t
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
   *         addConnectorListener(ConnectorListenerType::ON_BUFFER_EMPTY,
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
   *         addConnectorListener(ConnectorListenerType::ON_BUFFER_EMPTY,
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
      if (type < ConnectorListenerType::CONNECTOR_LISTENER_NUM)
        {
          static const char* const typeStr[] =
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
          return typeStr[static_cast<uint8_t>(type)];
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
    using Entry = std::pair<ConnectorDataListener*, bool>;
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
    virtual ReturnCode notify(ConnectorInfo& info,
                ByteData& cdrdata, const std::string& marshalingtype);


    virtual ReturnCode notifyIn(ConnectorInfo& info, ByteData& data);

    virtual ReturnCode notifyOut(ConnectorInfo& info, ByteData& data);


    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(データ型指定版、InPort側)
     * 登録されているリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * InPort側ではnotifyIn関数を使用する必要がある。
     *
     *
     * @param info ConnectorInfo
     * @param typeddata データ（データ型指定あり）
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     *
     * @param info ConnectorInfo
     * @param typeddata Data 
     * @endif
     */
    template <class DataType>
    ReturnCode notifyIn(ConnectorInfo& info, DataType& typeddata)
    {
        std::string type = info.properties.getProperty("marshaling_type", "cdr");
        std::string marshaling_type{coil::eraseBothEndsBlank(
          info.properties.getProperty("inport.marshaling_type", type))};
        return notify(info, typeddata, marshaling_type);
    }

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(データ型指定版、OutPort側)
     * 登録されているリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * InPort側ではnotifyOut関数を使用する必要がある。
     *
     *
     * @param info ConnectorInfo
     * @param typeddata データ（データ型指定あり）
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     *
     * @param info ConnectorInfo
     * @param typeddata Data
     * @endif
     */
    template <class DataType>
    ReturnCode notifyOut(ConnectorInfo& info, DataType& typeddata)
    {
        std::string type = info.properties.getProperty("marshaling_type", "cdr");
        std::string marshaling_type{coil::eraseBothEndsBlank(
          info.properties.getProperty("outport.marshaling_type", type))};
        return notify(info, typeddata, marshaling_type);
    }
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
     * @param marshalingtype シリアライザの種類
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * This calls the Callback method of the registered listener.
     * This operation calls only ConnectorDataListenerT type callback.
     *
     * @param info ConnectorInfo
     * @param typeddata Data
     * @param marshalingtype 
     * @endif
     */
    template <class DataType>
    ReturnCode notify(ConnectorInfo& info, DataType& typeddata, const std::string& marshalingtype)
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      ReturnCode ret(NO_CHANGE);

      if(m_listeners.empty())
      {
        return ret;
      }

      std::string endian_type{coil::normalize(
        info.properties.getProperty("serializer.cdr.endian", "little"))};
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
              if (m_cdr == nullptr || m_marshalingtype != marshalingtype)
              {
                  m_cdr = createSerializer<DataType>(marshalingtype);
                  m_marshalingtype = marshalingtype;
              }
              ::RTC::ByteDataStream<DataType> *cdr = dynamic_cast<::RTC::ByteDataStream<DataType>*>(m_cdr);

              if (!cdr)
              {
                  return NO_CHANGE;
              }

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
              ret = ret | listener.first->operator()(info, tmp, marshalingtype);

            }
        }
      return ret;
    }

  protected:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
    ByteDataStreamBase* m_cdr{ nullptr };
    std::string m_marshalingtype;
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
    using Entry = std::pair<ConnectorListener*, bool>;
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
    std::mutex m_mutex;
  };

  class ConnectorListenersBase
  {
  public:
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ConnectorListenersBase();
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(InPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * InPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data バイト列のデータ
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    virtual ::RTC::ConnectorListenerStatus::Enum notifyIn(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(OutPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * OutPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data バイト列のデータ
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    virtual ::RTC::ConnectorListenerStatus::Enum notifyOut(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * データポートの Connector において発生する各種イベントに対するコー
     * ルバックメソッド
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @return リターンコード
     *
     * @else
     *
     * @brief Virtual Callback method
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @return リターンコード
     *
     * @return
     * @endif
     */
    virtual ::RTC::ConnectorListenerStatus::Enum notify(ConnectorListenerType type, ConnectorInfo& info) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のConnectorDataListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    virtual bool addListener(ConnectorDataListenerType type, ConnectorDataListener* listener, bool autoclean=true) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のConnectorListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    virtual bool addListener(ConnectorListenerType type, ConnectorListener* listener, bool autoclean=true) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のConnectorDataListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    virtual bool removeListener(ConnectorDataListenerType type, ConnectorDataListener* listener) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のConnectorListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    virtual bool removeListener(ConnectorListenerType type, ConnectorListener* listener) = 0;
    /*!
     * @if jp
     *
     * @brief 指定の種類のConnectorDataListenerHolderを取得する
     *
     *
     * @param type リスナの種類
     * @return ConnectorDataListenerHolder
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    virtual ConnectorDataListenerHolder* getDataListenerHolder(ConnectorDataListenerType type) = 0;
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(データ型指定、InPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * InPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data データ(型指定)
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    template<class DataType> ::RTC::ConnectorListenerStatus::Enum notifyIn(ConnectorDataListenerType type, ConnectorInfo& info, DataType& data)
    {
        ConnectorDataListenerHolder* holder = getDataListenerHolder(type);
        if (holder != nullptr)
        {
            return holder->notifyIn(info, data);
        }
        return ConnectorListenerStatus::NO_CHANGE;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(データ型指定、OutPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * OutPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data データ(型指定)
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    template<class DataType> ::RTC::ConnectorListenerStatus::Enum notifyOut(ConnectorDataListenerType type, ConnectorInfo& info, DataType& data)
    {
        ConnectorDataListenerHolder* holder = getDataListenerHolder(type);
        if (holder != nullptr)
        {
            return holder->notifyOut(info, data);
        }
        return ConnectorListenerStatus::NO_CHANGE;
    }
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
       : public ConnectorListenersBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ConnectorListeners();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~ConnectorListeners() override;
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(InPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * InPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data バイト列のデータ
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    ::RTC::ConnectorListenerStatus::Enum notifyIn(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data) override;
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(OutPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * OutPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data バイト列のデータ
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    ::RTC::ConnectorListenerStatus::Enum notifyOut(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data) override;
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * データポートの Connector において発生する各種イベントに対するコー
     * ルバックメソッド
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @return リターンコード
     *
     * @else
     *
     * @brief Virtual Callback method
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @return リターンコード
     *
     * @return
     * @endif
     */
    ::RTC::ConnectorListenerStatus::Enum notify(ConnectorListenerType type, ConnectorInfo& info) override;
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のConnectorDataListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(ConnectorDataListenerType type, ConnectorDataListener* listener, bool autoclean=true) override;
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のConnectorListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(ConnectorListenerType type, ConnectorListener* listener, bool autoclean=true) override;
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のConnectorDataListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(ConnectorDataListenerType type, ConnectorDataListener* listener) override;
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のConnectorListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(ConnectorListenerType type, ConnectorListener* listener) override;
    /*!
     * @if jp
     *
     * @brief 指定の種類のConnectorDataListenerHolderを取得する
     *
     *
     * @param type リスナの種類
     * @return ConnectorDataListenerHolder
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    ConnectorDataListenerHolder* getDataListenerHolder(ConnectorDataListenerType type) override;
  
  private:
    /*!
     * @if jp
     * @brief ConnectorDataListenerTypeリスナ配列
     * ConnectorDataListenerTypeリスナを格納
     * @else
     * @brief ConnectorDataListenerType listener array
     * The ConnectorDataListenerType listener is stored.
     * @endif
     */
    std::array<ConnectorDataListenerHolder,
               static_cast<uint8_t>
               (ConnectorDataListenerType::CONNECTOR_DATA_LISTENER_NUM)>
               connectorData_;
    /*!
     * @if jp
     * @brief ConnectorListenerTypeリスナ配列
     * ConnectorListenerTypeリスナを格納
     * @else
     * @brief ConnectorListenerType listener array
     * The ConnectorListenerType listener is stored.
     * @endif
     */
    std::array<ConnectorListenerHolder,
               static_cast<uint8_t>
               (ConnectorListenerType::CONNECTOR_LISTENER_NUM)>
               connector_;
  };


  /*!
   * @if jp
   * @class ConnectorDataListenerHolderT
   * @brief データ型指定のConnectorListener ホルダクラス
   *
   * 複数の ConnectorListener を保持し管理するクラス。
   *
   * @else
   * @class ConnectorDataListenerHolderT
   * @brief ConnectorListener holder class
   *
   * This class manages one ore more instances of ConnectorListener class.
   *
   * @endif
   */
  template <class DataType>
  class ConnectorDataListenerHolderT
      : public ConnectorDataListenerHolder
  {
  public:
      /*!
       * @if jp
       * @brief コンストラクタ
       * @else
       * @brief Constructor
       * @endif
       */
      ConnectorDataListenerHolderT() = default;
      /*!
       * @if jp
       * @brief デストラクタ
       * @else
       * @brief Destructor
       * @endif
       */
      ~ConnectorDataListenerHolderT() override
      {

      }



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
          ByteData& cdrdata, const std::string& marshalingtype) override
      {
          std::lock_guard<std::mutex> guard(m_mutex);
          ConnectorListenerHolder::ReturnCode ret(NO_CHANGE);
          bool endian = true;

          if(m_listeners.empty())
          {
            return ret;
          }

          DataType data;

          if (m_cdr == nullptr || m_marshalingtype != marshalingtype)
          {
              m_cdr = createSerializer<DataType>(marshalingtype);
              m_marshalingtype = marshalingtype;
          }
          ::RTC::ByteDataStream<DataType> *cdr = dynamic_cast<::RTC::ByteDataStream<DataType>*>(m_cdr);


          if (!cdr)
          {
              return NO_CHANGE;
          }


          // endian type check
          std::string endian_type{ coil::normalize(
            info.properties.getProperty("serializer.cdr.endian", "little")) };
          std::vector<std::string> endian_str(coil::split(endian_type, ","));

          if (endian_str[0] == "little")
          {
              endian = true;
          }
          else if (endian_str[0] == "big")
          {
              endian = false;
          }

          cdr->isLittleEndian(endian);
          cdr->writeData(cdrdata.getBuffer(), cdrdata.getDataLength());
          cdr->deserialize(data);


          for (auto & listener : m_listeners)
          {
              ConnectorDataListenerT<DataType>* datalistener(nullptr);
              datalistener =
                  dynamic_cast<ConnectorDataListenerT<DataType>*>(listener.first);
              if (datalistener != nullptr)
              {
                  ConnectorListenerHolder::ReturnCode linstener_ret(datalistener->operator()(info, data));
                  if (linstener_ret == DATA_CHANGED || linstener_ret == BOTH_CHANGED)
                  {
                      cdr->isLittleEndian(endian);
                      cdr->serialize(data);
                      cdrdata.setDataLength(cdr->getDataLength());
                      cdr->readData(cdrdata.getBuffer(), cdrdata.getDataLength());
                  }
                  ret = ret | linstener_ret;
              }
              else
              {
                  ConnectorListenerHolder::ReturnCode linstener_ret(listener.first->operator()(info, cdrdata, marshalingtype));
                  if (linstener_ret == DATA_CHANGED || linstener_ret == BOTH_CHANGED)
                  {
                      cdr->isLittleEndian(endian);
                      cdr->writeData(cdrdata.getBuffer(), cdrdata.getDataLength());
                      cdr->deserialize(data);
                  }
                  ret = ret | linstener_ret;
              }
          }

          return ret;
      }


      /*!
       * @if jp
       *
       * @brief リスナーへ通知する(OutPort側)
       * リスナのコールバックメソッドを呼び出す。
       * InPortとOutPortでシリアライザの種類が違う場合があるため、
       * OutPort側ではnotifyOut関数を使用する必要がある。
       *
       * @param info ConnectorInfo
       * @param data バイト列のデータ
       * @return リターンコード
       * @else
       *
       * @brief Notify listeners. (Typed data version)
       *
       * @param info ConnectorInfo
       * @param data Data
       * @return
       * @endif
       */
      ReturnCode notifyIn(ConnectorInfo& info, ByteData& data) override
      {
          std::string type = info.properties.getProperty("marshaling_type", "cdr");
          std::string marshaling_type{ coil::eraseBothEndsBlank(
            info.properties.getProperty("inport.marshaling_type", type)) };
          return notify(info, data, marshaling_type);
      }

      /*!
       * @if jp
       *
       * @brief リスナーへ通知する(OutPort側)
       * リスナのコールバックメソッドを呼び出す。
       * InPortとOutPortでシリアライザの種類が違う場合があるため、
       * OutPort側ではnotifyOut関数を使用する必要がある。
       *
       * @param info ConnectorInfo
       * @param data バイト列のデータ
       * @return リターンコード
       * @else
       *
       * @brief Notify listeners. (Typed data version)
       *
       * @param info ConnectorInfo
       * @param data Data
       * @return
       * @endif
       */
      ReturnCode notifyOut(ConnectorInfo& info, ByteData& data) override
      {
          std::string type = info.properties.getProperty("marshaling_type", "cdr");
          std::string marshaling_type{ coil::eraseBothEndsBlank(
            info.properties.getProperty("outport.marshaling_type", type)) };
          return notify(info, data, marshaling_type);
      }

  };

  /*!
   * @if jp
   * @class ConnectorListenersT
   * @brief ConnectorListenersT クラス
   *
   *
   * @else
   * @class ConnectorListenersT
   * @brief ConnectorListenersT class
   *
   *
   * @endif
   */
  template <class DataType>
  class ConnectorListenersT
       : public ConnectorListenersBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ConnectorListenersT()
    {
    }
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~ConnectorListenersT() override {}

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(InPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * InPort側ではnotifyIn関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data バイト列のデータ
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    ::RTC::ConnectorListenerStatus::Enum notifyIn(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data) override
    {
      if (static_cast<uint8_t>(type) < connectorData_.size())
      {
          return connectorData_[static_cast<uint8_t>(type)].notifyIn(info, data);
      }
      return ConnectorListenerStatus::NO_CHANGE;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する(OutPort側)
     * 指定の種類のリスナのコールバックメソッドを呼び出す。
     * InPortとOutPortでシリアライザの種類が違う場合があるため、
     * OutPort側ではnotifyOut関数を使用する必要がある。
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @param data バイト列のデータ
     * @return リターンコード
     * @else
     *
     * @brief Notify listeners. (Typed data version)
     *
     * @param type
     * @param info ConnectorInfo
     * @param data Data
     * @return
     * @endif
     */
    ::RTC::ConnectorListenerStatus::Enum notifyOut(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data) override
    {
        if (static_cast<uint8_t>(type) < connectorData_.size())
        {
            return connectorData_[static_cast<uint8_t>(type)].notifyOut(info, data);
        }
        return ConnectorListenerStatus::NO_CHANGE;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * データポートの Connector において発生する各種イベントに対するコー
     * ルバックメソッド
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @return リターンコード
     *
     * @else
     *
     * @brief Virtual Callback method
     *
     * @param type リスナの種類
     * @param info ConnectorInfo
     * @return リターンコード
     *
     * @return
     * @endif
     */
    ::RTC::ConnectorListenerStatus::Enum notify(ConnectorListenerType type, ConnectorInfo& info) override
    {
        if (static_cast<uint8_t>(type) < connector_.size())
        {
            return connector_[static_cast<uint8_t>(type)].notify(info);
        }
        return ConnectorListenerStatus::NO_CHANGE;
    }
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のConnectorDataListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(ConnectorDataListenerType type, ConnectorDataListener* listener, bool autoclean=true) override
    {
        if (static_cast<uint8_t>(type) < connectorData_.size())
        {
            connectorData_[static_cast<uint8_t>(type)].addListener(listener, autoclean);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のConnectorListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(ConnectorListenerType type, ConnectorListener* listener, bool autoclean=true) override
    {
        if (static_cast<uint8_t>(type) < connector_.size())
        {
            connector_[static_cast<uint8_t>(type)].addListener(listener, autoclean);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のConnectorDataListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(ConnectorDataListenerType type, ConnectorDataListener* listener) override
    {
        if (static_cast<uint8_t>(type) < connectorData_.size())
        {
            connectorData_[static_cast<uint8_t>(type)].removeListener(listener);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のConnectorListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(ConnectorListenerType type, ConnectorListener* listener) override
    {
        if (static_cast<uint8_t>(type) < connector_.size())
        {
            connector_[static_cast<uint8_t>(type)].removeListener(listener);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief 指定の種類のConnectorDataListenerHolderを取得する
     *
     *
     * @param type リスナの種類
     * @return ConnectorDataListenerHolder
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    ConnectorDataListenerHolder* getDataListenerHolder(ConnectorDataListenerType type) override
    {
        if (static_cast<uint8_t>(type) < connectorData_.size())
        {
            return &connectorData_[static_cast<uint8_t>(type)];
        }
        return nullptr;
    }

  private:
    /*!
     * @if jp
     * @brief ConnectorDataListenerTypeリスナ配列
     * ConnectorDataListenerTypeリスナを格納
     * @else
     * @brief ConnectorDataListenerType listener array
     * The ConnectorDataListenerType listener is stored.
     * @endif
     */
    std::array<ConnectorDataListenerHolder,
               static_cast<uint8_t>
               (ConnectorDataListenerType::CONNECTOR_DATA_LISTENER_NUM)>
               connectorData_;
    /*!
     * @if jp
     * @brief ConnectorListenerTypeリスナ配列
     * ConnectorListenerTypeリスナを格納
     * @else
     * @brief ConnectorListenerType listener array
     * The ConnectorListenerType listener is stored.
     * @endif
     */
    std::array<ConnectorListenerHolder,
               static_cast<uint8_t>
               (ConnectorListenerType::CONNECTOR_LISTENER_NUM)>
               connector_;

  };
} // namespace RTC

#endif // RTC_CONNECTORLISTENER_H
