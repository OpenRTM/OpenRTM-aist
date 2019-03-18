// -*- C++ -*-
/*!
 * @file  Throughput.h
 * @brief Clock Throughput Component
 * @date  $Date$
 *
 * @author n-ando n-ando@aist.go.jp
 *
 * $Id$
 */

#ifndef THROUGHPUT_H
#define THROUGHPUT_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="port_stub_h">
// </rtc-template>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

#include <vector>
#include <fstream>

using namespace RTC;


class Throughput;

/*!
 * @class Throughput
 * @brief Clock Throughput Component
 *
 * 時間計測用RTC
 *
 * データポートのスループットを計測するコンポーネント。interface_type,
 * subscription_type 等 ConnectorProfile パラメータやデータサイズ、サン
 * プル数などを変更して、その際の転送時間（最大、最小、平均、標準偏差）
 * およびスループットを測定してファイルに記録することができる。
 *
 * 基本的には、以下の(a)や(b)のような接続形態で使用する。
 * <pre>
 *  +-----------+
 *  |  ______   |    ______     ______
 *  +->|_____|>-+   >|_____|>-->|_____|>
 *       (a)                 (b)
 * </pre>
 * 同一コンポーネント内では(a)、同一プロセス内、同一ノード内のスループッ
 * トは (a)または(b)、異なるノード間のスループットを計測する際は (b)の
 * 接続形態で計測する。計測は以下の手順で行う。
 *
 * -# コンポーネントを起動する
 * -# コンフィグレーションパラメータを設定する
 * -# 必要なコネクタプロファイルを設定してポートを接続する
 * -# コンポーネントをアクティベートする
 *
 * 計測結果はデータを受け取ったコンポーネントがファイルに記録する。
 *
 * * コンフィギュレーションパラメータ
 *
 * - mode: 計測モード名。logincr, incr, const から選択可能。
 *  - logincr: logスケールでデータ数を増加させ計測。実際には、1, 2, 5,
 *             10, .. といった間隔でデータ数を増加させ、logスケールでプ
 *             ロットした際にほぼ等間隔となるように計測する。
 *  - incr: incrementパラメータで指定されたバイト数で、一定間隔でデータ
 *             数を増加させる。
 *  - const: データは増加させず一定サイズでスループットを計測する。
 *
 * - outputfile: 出力ファイル名。onActivated時、またはデータ受信時にファ
 *               イルがオープンされるので、それ以降にパラメータを設定し
 *               た場合は反映されない。
 * - increment: データ増分。mode が incr の場合のデータ増分を byte で指定する。
 *
 * - maxsize: 最大データ個数を指定する。送信するシーケンスデータのサイ
 *            ズを指定する。実際のデータサイズは、この個数に1データ当た
 *            りのバイト数をかけたものとなる。
 * - maxsend: 最大送信数。データ送信回数の最大値を指定する。モードが
 *            logincr, incr の場合、データサイズ毎に maxsend 回数データ
 *            を送信する。
 * - maxsample: 最大サンプリング数。データを受信し、統計情報を計算する
 *            際の最大サンプル数を指定する。データ送信側の送信数がサン
 *            プル数より少ない場合、受信したサンプル数で統計情報を計算
 *            する。データ送信側の送信数がサンプル数より多い場合、古い
 *            情報は破棄され、最新の maxsample 個の計測データから統計情
 *            報を計算する。
 *
 *
 */
class Throughput
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  Throughput(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~Throughput() override;

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onInitialize() override;

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id) override;

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id) override;

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id) override;

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);
  void writeData();
  void setDataSize(CORBA::ULong size);
  CORBA::ULong getDataSize();
  CORBA::ULong getInPortConnectorSize();
  void receiveData(const RTC::Time &tm, CORBA::ULong seq_length);
  void setConnectorProfile(const RTC::ConnectorInfo& info);

 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  /*!
   * データ型
   * - Name: datatype
   * - DefaultValue: double
   */
  std::string m_datatype;
  /*!
   * 出力ファイル名
   * - Name: outputfile outputfile
   * - DefaultValue: test.dat
   */
  std::string m_outputfile;
  std::string m_filesuffix;
  /*!
  * データ増分
  * - Name: increment increment
  * - DefaultValue: 100
  */
  int m_increment;
  /*!
  * 内部で待機する時間
  * - Name: sleep_time sleep_time
  * - DefaultValue: 0.01
  */
  double m_sleep_time;
  /*!
   * 通信データ量を一定値(const)にするか、徐々に増加する値(increase)に
   * するかの設定
   *
   * - Name: mode mode
   * - DefaultValue: const
   */
  std::string m_mode;
  /*!
   * 通信データ量を徐々に増やすモードの場合最大データサイズ
   * これを超えるとエラー状態に遷移
   * - Name: maxsize maxsize
   * - DefaultValue: 10000000
   */
  unsigned long m_maxsize;

  /*!
   * mode=constの時のデータ送信回数の最大値
   *
   * - Name: maxsend maxsend
   * - DefaultValue: 1000
   */
  unsigned long m_maxsend;

  /*!
   * 1データセットごとの最大サンプル数
   *
   * - Name: maxsize maxsample
   * - DefaultValue: 100
   */
  unsigned long m_maxsample;

  // </rtc-template>

  // OutPort, InPort declaration
  RTC::TimedOctetSeq m_inOctet;
  RTC::TimedOctetSeq m_outOctet;

  RTC::TimedShortSeq m_inShort;
  RTC::TimedShortSeq m_outShort;

  RTC::TimedLongSeq  m_inLong;
  RTC::TimedLongSeq  m_outLong;

  RTC::TimedFloatSeq m_inFloat;
  RTC::TimedFloatSeq m_outFloat;

  RTC::TimedDoubleSeq m_inDouble;
  RTC::TimedDoubleSeq m_outDouble;

  InPort<RTC::TimedOctetSeq> m_inOctetIn;
  OutPort<RTC::TimedOctetSeq> m_outOctetOut;

  InPort<RTC::TimedShortSeq> m_inShortIn;
  OutPort<RTC::TimedShortSeq> m_outShortOut;

  InPort<RTC::TimedLongSeq> m_inLongIn;
  OutPort<RTC::TimedLongSeq> m_outLongOut;

  InPort<RTC::TimedFloatSeq> m_inFloatIn;
  OutPort<RTC::TimedFloatSeq> m_outFloatOut;

  InPort<RTC::TimedDoubleSeq> m_inDoubleIn;
  OutPort<RTC::TimedDoubleSeq> m_outDoubleOut;


  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

  

 private:
  // file output stream
  std::ofstream m_fs;

  //============================================================
  // Time measurement statistics data
  // data size to be send
  unsigned long m_datasize;
  std::vector<coil::TimeValue> m_record;

  // received data store
  size_t m_sendcount;
  size_t m_logmulcnt;
  size_t m_varsize;
  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};

template <class DataType>
class DataListener
  : public ConnectorDataListenerT<DataType>
{
  USE_CONNLISTENER_STATUS;
public:
  DataListener(Throughput *comp) : m_comp(comp)  {}
  ~DataListener() override {}
  ReturnCode operator()(ConnectorInfo&  /*info*/,
                          DataType& data) override
  {
    m_comp->receiveData(data.tm, data.data.length());
	return RTC::ConnectorListenerStatus::NO_CHANGE;;
  }
  Throughput* m_comp;
};

class ConnListener
  : public ConnectorListener
{
  USE_CONNLISTENER_STATUS;
public:
  ConnListener(Throughput *comp) : m_comp(comp) {}
  ~ConnListener() override {}
  ReturnCode operator()(ConnectorInfo& info) override
  {
// Connector Listener: ON_CONNECT
// Profile::name:      ConsoleIn0.out_ConsoleOut0.in
// Profile::id:        2cf0daa4-76d8-4e74-b734-98a683e7fe5e
// Profile::properties:
// - data_type: IDL:RTC/TimedLong:1.0
// - interface_type: corba_cdr
// - dataflow_type: push
// - subscription_type: flush
// - publisher
//   - push_policy: all
// - serializer
//   - cdr
//     - endian: little,big
// - corba_cdr
//   - inport_ior: IOR:010000001a00000049444c3a4f70656e52544d2f496e506f72744364723a312e30000000010000000000000064000000010102000d00000031302e3231312e35352e3138000092bb0e000000fea5d4c259000007de000000000400000200000000000000080000000100000000545441010000001c00000001000000010001000100000001000105090101000100000009010100
// - consumer:
    
    std::cout << "------------------------------"   << std::endl;
    std::cout << "       Connected !!" <<std::endl;
    std::cout << "------------------------------"   << std::endl;
    std::cout << "Profile::name:      " << info.name    << std::endl;
    std::cout << "Profile::id:        " << info.id      << std::endl;
    std::cout << "Profile::properties: "            << std::endl;
    std::cout << info.properties;
    std::cout                                       << std::endl;
    std::cout << "------------------------------"   << std::endl;
    m_comp->setConnectorProfile(info);
	return RTC::ConnectorListenerStatus::NO_CHANGE;;
  }
  Throughput* m_comp;
};


extern "C"
{
  DLL_EXPORT void ThroughputInit(RTC::Manager* manager);
};

#endif // THROUGHPUT_H
