// -*- C++ -*-
/*!
 * @file  ROSOutPort.h
 * @brief ROSOutPort class
 * @date  $Date: 2019-01-31 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_ROSOUTPORT_H
#define RTC_ROSOUTPORT_H



#include <map>
#include <rtm/InPortConsumer.h>
#include <rtm/Manager.h>
#include <ros/transport/transport_tcp.h>
#include <ros/connection.h>
#include <xmlrpcpp/XmlRpc.h>
#include "ROSMessageInfo.h"


namespace RTC
{
  /*!
   * @if jp
   * @class ROSOutPort
   * @brief ROSOutPort クラス
   *
   * InPortConsumer 
   *
   * データ転送に CORBA の OpenRTM::InPortCdr インターフェースを利用し
   * た、push 型データフロー型を実現する InPort コンシューマクラス。
   *
   * @since 2.0.0
   *
   * @else
   * @class ROSOutPort
   * @brief ROSOutPort class
   *
   * The InPort consumer class which uses the OpenRTM::InPortCdr
   * interface in CORBA for data transfer and realizes a push-type
   * dataflow.
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ROSOutPort
    : public InPortConsumer
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
    ROSOutPort(void);
    
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
    ~ROSOutPort(void) override;

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

    /*!
     * @if jp
     * @brief ROSTCPでパブリッシャーとサブスクライバーを接続する
     *
     *
     * @param transport TransportTCP
     *
     * @return True：接続成功、False：接続失敗
     * 接続済みの場合はFalse
     * 
     * @else
     * @brief 
     *
     *
     * @param transport 
     * 
     * @return
     *
     * @endif
     */
    bool connectTCP(const ros::TransportTCPPtr& transport);

    /*!
     * @if jp
     * @brief ヘッダ情報受信時のコールバック関数
     *
     *
     * @param conn ros::ConnectionPtr
     * @param header ヘッダ情報
     *
     * @return true：問題なし、false：ヘッダが不正
     * 
     * @else
     * @brief 
     *
     *
     * @param conn 
     * @param header 
     * 
     * @return
     *
     * @endif
     */
    bool onConnectionHeaderReceived(const ros::ConnectionPtr& conn, const ros::Header& header)
    {
      RTC_VERBOSE(("onConnectionHeaderReceived()"));

      std::string topic;
      if (!header.getValue("topic", topic))
      {
        RTC_ERROR(("Topic name does not exist in header"));
        return false;
      }

      if(m_topic != topic)
      {
        RTC_VERBOSE(("Topic names do not match. %s %s", m_topic.c_str(), topic.c_str()));
        return false;
      }

      std::string client_callerid;
      if (!header.getValue("callerid", client_callerid))
      {
        RTC_ERROR(("Callse id does not exist in header"));
        return false;
      }

      for (auto & con : m_tcp_connecters)
      {
        if(con.second.getConnection() == conn)
        {
          con.second.setNoneName(client_callerid);
        }
        
      }

      

      ROSMessageInfoBase* info = GlobalROSMessageInfoList::instance().getInfo(m_messageType);
      
      if(!info)
      {
        RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
        return false;
      }

      ros::M_string m;
      m["type"] = info->type();
      m["md5sum"] = info->md5sum();
      m["message_definition"] = info->message_definition();
      m["callerid"] = m_callerid;
      m["latching"] = "0";
      m["topic"] = topic;
      

      RTC_VERBOSE(("TCPTransPort created"));
      RTC_VERBOSE(("Message Type:%s", info->type().c_str()));
      RTC_VERBOSE(("Md5sum:%s", info->md5sum().c_str()));
      RTC_VERBOSE(("Message Definition:%s", info->message_definition().c_str()));
      RTC_VERBOSE(("Caller ID:%s", m_callerid.c_str()));
      RTC_VERBOSE(("Topic Name:%s", topic.c_str()));
      RTC_VERBOSE(("TCPTransPort created"));


      conn->writeHeader(m, boost::bind(&ROSOutPort::onHeaderWritten, this, _1));
      return true;
    }

    /*!
     * @if jp
     * @brief ヘッダー情報送信時のコールバック関数
     *
     *
     * @param conn ros::Connection
     *
     * 
     * @else
     * @brief 
     *
     *
     * @param conn 
     * 
     * @return
     *
     * @endif
     */
    void onHeaderWritten(const ros::ConnectionPtr& conn)
    {
      RTC_VERBOSE(("onHeaderWritten()"));
      (void)conn;
      std::lock_guard<std::mutex> guard(m_mutex);
      m_start = true;
    }



    /*!
     * @if jp
     * @brief メッセージ送信時のコールバック関数
     *
     *
     * @param conn ros::Connection
     *
     * 
     * @else
     * @brief 
     *
     *
     * @param conn 
     * 
     * @return
     *
     * @endif
     */
    void onMessageWritten(const ros::ConnectionPtr& conn)
    {
      (void)conn;
      RTC_VERBOSE(("onMessageWritten()"));
    };
    /*!
     * @if jp
     * @brief コネクタの情報を取得
     *
     *
     * @param info 情報を格納する変数
     * data[0]：コネクタID
     * data[1]：接続先のノード名
     * data[2]："o"
     * data[3]：TCPROS or UDPROS
     * data[4]：トピック名
     * data[5]：true
     * data[6]：接続情報
     * 
     * @else
     * @brief 
     *
     *
     * @param info
     * 
     *
     * @endif
     */
    void getInfo(XmlRpc::XmlRpcValue& info);
    /*!
     * @if jp
     * @brief メッセージ型の取得
     *
     *
     * @return メッセージ型
     * 
     * @else
     * @brief 
     *
     * 
     * @return
     *
     * @endif
     */
    const std::string& datatype() const;
    /*!
     * @if jp
     * @brief トピック名の取得
     *
     *
     * @return トピック名
     * 
     * @else
     * @brief 
     *
     * 
     * @return
     *
     * @endif
     */
    const std::string& getTopicName() const;
    /*!
     * @if jp
     * @brief ノード名の取得
     *
     *
     * @return ノード名
     * 
     * @else
     * @brief 
     *
     * 
     * @return
     *
     * @endif
     */
    const std::string& getName() const;

  private:

    mutable Logger rtclog;
    bool m_start;
    coil::Properties m_properties;
    int m_subnum;


    /*!
     * @if jp
     * @class SubscriberLink
     * @brief SubscriberLink クラス
     *
     * ros::Connection、接続先のノード名、コネクタのIDを格納するクラス
     *
     * @since 2.0.0
     *
     * @else
     * @class SubscriberLink
     * @brief SubscriberLink class
     *
     * 
     *
     * @since 2.0.0
     *
     * @endif
     */
    class SubscriberLink
    {
    public:
      /*!
       * @if jp
       * @brief コンストラクタ
       *
       * @else
       * @brief Constructor
       *
       * @endif
       */
      SubscriberLink();
      /*!
       * @if jp
       * @brief コンストラクタ
       *
       * @param conn ros::Connection
       * @param num コネクタのID
       *
       * @else
       * @brief Constructor
       *
       * @param conn 
       * @param num 
       *
       * @endif
       */
      SubscriberLink(ros::ConnectionPtr conn, int num);
      /*!
       * @if jp
       * @brief コピーコンストラクタ
       *
       * @param obj コピー元 
       *
       * @else
       * @brief Copy Constructor
       *
       * @param obj
       *
       * @endif
       */
      SubscriberLink(const SubscriberLink &obj);
      /*!
       * @if jp
       * @brief デストラクタ
       *
       *
       * @else
       * @brief Destructor
       *
       *
       * @endif
       */
      ~SubscriberLink();
      /*!
       * @if jp
       * @brief 接続先のノード名を設定
       *
       * @param name ノード名
       *
       * @else
       * @brief 
       *
       * @param name
       *
       * @endif
       */
      void setNoneName(std::string& name);
      /*!
       * @if jp
       * @brief 接続先のノード名を取得
       *
       * @return ノード名
       *
       * @else
       * @brief 
       *
       * @return
       *
       * @endif
       */
      const std::string getNodeName() const;
      /*!
       * @if jp
       * @brief ros::Connectionを設定
       *
       * @param conn ros::Connection
       *
       * @else
       * @brief 
       *
       * @param conn
       *
       * @endif
       */
      void setConnection(ros::ConnectionPtr conn);
      /*!
       * @if jp
       * @brief ros::Connectionを取得
       *
       * @return ros::Connection
       *
       * @else
       * @brief 
       *
       * @return
       *
       * @endif
       */
      ros::ConnectionPtr getConnection();
      /*!
       * @if jp
       * @brief コネクタのID取得
       *
       * @return コネクタのID
       *
       * @else
       * @brief 
       *
       * @return
       *
       * @endif
       */
      int getNum();
    private:
      std::string m_nodename;
      ros::ConnectionPtr m_conn;
      int m_num;
    };
    std::map<std::string, SubscriberLink> m_tcp_connecters;
    
    std::string m_topic;
    std::string m_callerid;
    std::string m_messageType;
    std::mutex m_mutex;
    std::string m_datatype;
    std::string m_roscorehost;
    unsigned int m_roscoreport;
  };
} // namespace RTC



#endif // RTC_ROSOUTPORT_H

