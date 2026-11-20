// -*- C++ -*-
/*!
 * @file InPortPullConnector.h
 * @brief InPortPull type connector class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
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

#ifndef RTC_OUTPORTPULLCONNECTOR_H
#define RTC_OUTPORTPULLCONNECTOR_H

#include <rtm/InPortConnector.h>
#include <rtm/InPortProvider.h>
#include <rtm/PublisherBase.h>
#include <rtm/DataPortStatus.h>

namespace RTC
{
  class OutPortConsumer;
  class ConnectorListeners;

  /*!
   * @if jp
   * @class InPortPullConnector
   * @brief InPortPullConnector クラス
   *
   * InPort の pull 型データフローのための Connector クラス。このオブ
   * ジェクトは、接続時に dataflow_type に pull が指定された場合、
   * InPort によって生成・所有され、OutPortPullConnector と対になって、
   * データポートの pull 型のデータフローを実現する。一つの接続に対して、
   * 一つのデータストリームを提供する唯一の Connector が対応する。
   * Connector は 接続時に生成される UUID 形式の ID により区別される。
   *
   * InPortPullConnector は以下の三つのオブジェクトを所有し管理する。
   *
   * - InPortConsumer
   * - Buffer
   *
   * OutPort に書き込まれたデータは OutPortPullConnector::write() に渡
   * され Buffer に書き込まれる。InPort::read(),
   * InPortPullConnector::read() は結果として、OutPortConsumer::get()
   * を呼び出し、OutPortPullConnector の持つバッファからデータを読み出
   * し、InPortPullConnector のもつバッファにデータを書き込む。
   *
   * @since 1.0.0
   *
   * @else
   * @class InPortPullConnector
   * @brief InPortPullConnector class
   *
   * Connector class of InPort for pull type dataflow. When "pull" is
   * specified as dataflow_type at the time of establishing
   * connection, this object is generated and owned by the InPort.
   * This connector and InPortPullConnector make a pair and realize
   * pull type dataflow of data ports. One connector corresponds to
   * one connection which provides a data stream. Connector is
   * distinguished by ID of the UUID that is generated at establishing
   * connection.
   *
   * InPortPullConnector owns and manages the following objects.
   *
   * - InPortConsumer
   * - Buffer
   *
   * Data written into the OutPort is passed to the
   * OutPortPullConnector::write(), and is written into the buffer.
   * Consequently, InPort::read() and InPortPullConnector::read() call
   * OutPortConsumer::get(), and it reads data from the buffer of
   * OutPortPullConnector.  Finally data would be written into the
   * InPortPullConnector's buffer.
   *
   * @since 1.0.0
   *
   * @endif
   */
  class InPortPullConnector
    : public InPortConnector
  {
  public:

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * InPortPullConnector のコンストラクタはオブジェクト生成時に下記
     * を引数にとる。ConnectorInfo は接続情報を含み、この情報に従いバッ
     * ファ等を生成する。OutPort インターフェースのプロバイダオブジェク
     * トへのポインタを取り、所有権を持つので、InPortPullConnector は
     * OutPortConsumer の解体責任を持つ。各種イベントに対するコールバッ
     * ク機構を提供する ConnectorListeners を持ち、適切なタイミングでコー
     * ルバックを呼び出す。データバッファがもし InPortBase から提供さ
     * れる場合はそのポインタを取る。
     *
     * コンストラクタが例外を送出する場合も、渡されたオブジェクトと
     * 自身が生成したバッファ等を解放してから送出する。
     *
     * @param info ConnectorInfo
     * @param consumer OutPortConsumer
     * @param listeners ConnectorListeners 型のリスナオブジェクトリスト
     * @param buffer CdrBufferBase 型のバッファ
     *
     * @else
     * @brief Constructor
     *
     * InPortPullConnector's constructor is given the following
     * arguments.  According to ConnectorInfo which includes
     * connection information, a buffer is created.  It is also given
     * a pointer to the consumer object for the OutPort interface.
     * The owner-ship of the pointer is owned by this
     * OutPortPullConnector, it has responsibility to destruct the
     * OutPortConsumer.  OutPortPullConnector also has
     * ConnectorListeners to provide event callback mechanisms, and
     * they would be called at the proper timing.  If data buffer is
     * given by OutPortBase, the pointer to the buffer is also given
     * as arguments.
     *
     * Even when the constructor throws, it releases the given object
     * and the buffer and other resources it created before throwing.
     *
     * @param info ConnectorInfo
     * @param consumer OutPortConsumer
     * @param listeners ConnectorListeners type lsitener object list
     * @param buffer CdrBufferBase type buffer
     *
     * @endif
     */
    InPortPullConnector(ConnectorInfo info,
                        OutPortConsumer* consumer,
                        ConnectorListenersBase* listeners,
                        CdrBufferBase* buffer = nullptr);

    /*!
     * @if jp
     * @brief デストラクタ
     *
     * disconnect() が呼ばれ、consumer, publisher, buffer が解体・削除される。
     *
     * @else
     *
     * @brief Destructor
     *
     * This operation calls disconnect(), which destructs and deletes
     * the consumer, the publisher and the buffer.
     *
     * @endif
     */
    ~InPortPullConnector() override;

    /*!
     * @if jp
     * @brief read 関数
     *
     * OutPortConsumer からデータを取得する。正常に読み出せた場合、戻り
     * 値は PORT_OK となり、data に読み出されたデータが格納される。それ
     * 以外の場合には、エラー値として BUFFER_EMPTY, TIMEOUT,
     * PRECONDITION_NOT_MET, PORT_ERROR が返される。
     *
     * @return PORT_OK              正常終了
     *         BUFFER_EMPTY         バッファは空である
     *         TIMEOUT              タイムアウトした
     *         PRECONDITION_NOT_MET 事前条件を満たさない
     *         PORT_ERROR           その他のエラー
     *
     * @else
     * @brief Destructor
     *
     * This function get data from OutPortConsumer.  If data is read
     * properly, this function will return PORT_OK return code. Except
     * normal return, BUFFER_EMPTY, TIMEOUT, PRECONDITION_NOT_MET and
     * PORT_ERROR will be returned as error codes.
     *
     * @return PORT_OK              Normal return
     *         BUFFER_EMPTY         Buffer empty
     *         TIMEOUT              Timeout
     *         PRECONDITION_NOT_MET Preconditin not met
     *         PORT_ERROR           Other error
     *
     * @endif
     */
    DataPortStatus read(ByteDataStreamBase* data) override;

    /*!
     * @if jp
     * @brief 接続解除関数
     *
     * Connector が保持している接続を解除する
     *
     * @else
     * @brief Disconnect connection
     *
     * This operation disconnect this connection
     *
     * @endif
     */
    DataPortStatus disconnect() override;

    /*!
     * @if jp
     * @brief アクティブ化
     *
     * このコネクタをアクティブ化する
     *
     * @else
     *
     * @brief Connector activation
     *
     * This operation activates this connector
     *
     * @endif
     */
    void activate() override {}  // do nothing

    /*!
     * @if jp
     * @brief 非アクティブ化
     *
     * このコネクタを非アクティブ化する
     *
     * @else
     *
     * @brief Connector deactivation
     *
     * This operation deactivates this connector
     *
     * @endif
     */
    void deactivate() override {}  // do nothing
    /*!
     * @if jp
     * @brief コンシューマのインターフェースの登録を取り消す
     *
     * @param prop コネクタプロファイルのプロパティ
     *
     * @else
     * @brief
     *
     * @param prop
     *
     * @endif
     */
    void unsubscribeInterface(const coil::Properties& prop) override;

  protected:
    /*!
     * @if jp
     * @brief Bufferの生成
     *
     * 与えられた接続情報に基づきバッファを生成する。
     *
     * @param info 接続情報
     * @return バッファへのポインタ
     *
     * @else
     * @brief create buffer
     *
     * This function creates a buffer based on given information.
     *
     * @param info Connector information
     * @return The poitner to the buffer
     *
     * @endif
     */
    static CdrBufferBase* createBuffer(ConnectorInfo& info);

    /*!
     * @if jp
     * @brief 接続確立時にコールバックを呼ぶ
     * @else
     * @brief Invoke callback when connection is established
     * @endif
     */
    void onConnect();

    /*!
     * @if jp
     * @brief 接続切断時にコールバックを呼ぶ
     * @else
     * @brief Invoke callback when connection is destroied
     * @endif
     */
    void onDisconnect();

  private:
    /*!
     * @if jp
     * @brief OutPortConsumer へのポインタ
     * @else
     * @brief the pointer to the OutPortConsumer
     * @endif
     */
    OutPortConsumer* m_consumer;

    /*!
     * @if jp
     * @brief ConnectorListenrs への参照
     * @else
     * @brief A reference to a ConnectorListener
     * @endif
     */
    ConnectorListenersBase* m_listeners;
    ByteData m_data;
  };
} // namespace RTC

#endif  // RTC_PULL_CONNECTOR_H
