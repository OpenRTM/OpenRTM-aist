// -*- C++ -*-
/*!
 * @file OutPortPushConnector.h
 * @brief Push type connector class
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

#ifndef RTC_OUTPORTPUSHCONNECTOR_H
#define RTC_OUTPORTPUSHCONNECTOR_H

#include <rtm/OutPortConnector.h>
#include <rtm/InPortConsumer.h>
#include <rtm/PublisherBase.h>

namespace RTC
{
  class ConnectorListeners;

  /*!
   * @if jp
   * @class OutPortPushConnector
   * @brief OutPortPushConnector クラス
   *
   * OutPort の push 型データフローのための Connector クラス。このオブ
   * ジェクトは、接続時に dataflow_type に push が指定された場合、
   * OutPort によって生成・所有され、InPortPushConnector と対になって、
   * データポートの push 型のデータフローを実現する。一つの接続に対して、
   * 一つのデータストリームを提供する唯一の Connector が対応する。
   * Connector は 接続時に生成される UUID 形式の ID により区別される。
   *
   * OutPortPushConnector は以下の三つのオブジェクトを所有し管理する。
   *
   * - InPortConsumer
   * - Buffer
   * - Publisher
   *
   * OutPort に書き込まれたデータは OutPortPushConnector::write() に渡
   * され、Connector は Publisher にデータを書き込む。Publisher はその
   * 特性に従ってデータを Buffer から取得し InPortConsumer に対して
   * push することで InPort にデータが転送される。
   *
   * @since 1.0.0
   *
   * @else
   * @class OutPortPushConnector
   * @brief OutPortPushConnector class
   *
   * Connector class of OutPort for push type dataflow.  When "push"
   * is specified as dataflow_type at the time of establishing
   * connection, this object is generated and owned by the OutPort.
   * This connector and InPortPushConnector make a pair and realize
   * push type dataflow of data ports.  One connector corresponds to
   * one connection which provides a data stream.  Connector is
   * distinguished by ID of the UUID that is generated at establishing
   * connection.
   *
   * OutPortPushConnector owns and manages the following objects.
   *
   * - InPortConsumer
   * - Buffer
   * - Publisher
   *
   * @since 1.0.0
   *
   * Data written into the OutPort is passed to
   * OutPortPushConnector::write(), and the connector writes into the
   * publisher.  The publisher gets data from the buffer based on the
   * policy and it is transferred to InPort by pushing it into the
   * InPortConsumer.
   *
   * @endif
   */
  class OutPortPushConnector
    : public OutPortConnector
  {
  public:

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * OutPortPushConnector のコンストラクタはオブジェクト生成時に下記
     * を引数にとる。ConnectorInfo は接続情報を含み、この情報に従いパブ
     * リッシャやバッファ等を生成する。InPort インターフェースに対する
     * コンシューマオブジェクトへのポインタを取り、所有権を持つので、
     * OutPortPushConnector は InPortConsumer の解体責任を持つ。各種イ
     * ベントに対するコールバック機構を提供する ConnectorListeners を持
     * ち、適切なタイミングでコールバックを呼び出す。データバッファがも
     * し OutPortBase から提供される場合はそのポインタを取る。
     *
     * コンストラクタが例外を送出する場合も、渡されたオブジェクトと
     * 自身が生成したバッファ等を解放してから送出する。
     *
     * @param info ConnectorInfo
     * @param consumer InPortConsumer
     * @param listeners ConnectorListeners 型のリスナオブジェクトリスト
     * @param buffer CdrBufferBase 型のバッファ
     *
     * @else
     * @brief Constructor
     *
     * OutPortPushConnector's constructor is given the following
     * arguments.  According to ConnectorInfo which includes
     * connection information, a publisher and a buffer are created.
     * It is also given a pointer to the consumer object for the
     * InPort interface.  The owner-ship of the pointer is owned by
     * this OutPortPushConnector, it has responsibility to destruct
     * the InPortConsumer.  OutPortPushConnector also has
     * ConnectorListeners to provide event callback mechanisms, and
     * they would be called at the proper timing.  If data buffer is
     * given by OutPortBase, the pointer to the buffer is also given
     * as arguments.
     *
     * Even when the constructor throws, it releases the given object
     * and the buffer and other resources it created before throwing.
     *
     * @param info ConnectorInfo
     * @param consumer InPortConsumer
     * @param listeners ConnectorListeners type lsitener object list
     * @param buffer CdrBufferBase type buffer
     *
     * @endif
     */
    OutPortPushConnector(ConnectorInfo info,
                         InPortConsumer* consumer,
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
    ~OutPortPushConnector() override;

    /*!
     * @if jp
     * @brief データの書き込み
     *
     * Publisherに対してデータを書き込み、これにより対応するInPortへデー
     * タが転送される。正常終了した場合 PORT_OK が返される。それ以外の
     * 場合、エラー値として、CONNECTION_LOST, BUFFER_FULL,
     * BUFFER_ERROR, PORT_ERROR, BUFFER_TIMEOUT, PRECONDITION_NO_MET が
     * 返される。
     *
     * @return PORT_OK              正常終了
     *         CONNECTION_LOST      接続がロストした
     *         BUFFER_FULL          バッファが一杯である
     *         BUFFER_ERROR         バッファエラー
     *         BUFFER_TIMEOUT       バッファへの書き込みがタイムアウトした
     *         PRECONDITION_NOT_MET 事前条件を満たさない
     *         PORT_ERROR           その他のエラー
     *
     * @else
     *
     * @brief Writing data
     *
     * This operation writes data into publisher and then the data
     * will be transferred to correspondent InPort. If data is written
     * properly, this function will return PORT_OK return code. Except
     * normal return, CONNECTION_LOST, BUFFER_FULL, BUFFER_ERROR,
     * PORT_ERROR, BUFFER_TIMEOUT and PRECONDITION_NO_MET will be
     * returned as error codes.
     *
     * @return PORT_OK              Normal return
     *         CONNECTION_LOST      Connectin lost
     *         BUFFER_FULL          Buffer full
     *         BUFFER_ERROR         Buffer error
     *         BUFFER_TIMEOUT       Timeout
     *         PRECONDITION_NOT_MET Precondition not met
     *         PORT_ERROR           Other error
     *
     * @endif
     */
    DataPortStatus write(RTC::ByteDataStreamBase* data) override;

    /*!
     * @if jp
     * @brief 接続解除
     *
     * consumer, publisher, buffer が解体・削除される。
     *
     * @else
     *
     * @brief disconnect
     *
     * This operation destruct and delete the consumer, the publisher
     * and the buffer.
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
    void activate() override;

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
    void deactivate() override;

    /*!
     * @if jp
     * @brief Buffer を取得する
     *
     * Connector が保持している Buffer を返す
     *
     * @else
     * @brief Getting Buffer
     *
     * This operation returns this connector's buffer
     *
     * @endif
     */
    CdrBufferBase* getBuffer() override;

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
     * @brief Publisherの生成
     *
     * 与えられた接続情報に基づきパブリッシャを生成する。
     *
     * @param info 接続情報
     * @return パブリッシャへのポインタ
     *
     * @else
     * @brief create buffer
     *
     * This function creates a publisher based on given information.
     *
     * @param info Connector information
     * @return The poitner to the publisher
     *
     * @endif
     */
    virtual PublisherBase* createPublisher(ConnectorInfo& info);

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
    virtual CdrBufferBase* createBuffer(ConnectorInfo& info);

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
     * @brief InPortConsumer へのポインタ
     * @else
     * @brief A pointer to an InPortConsumer
     * @endif
     */
    InPortConsumer* m_consumer;

    /*!
     * @if jp
     * @brief Publisher へのポインタ
     * @else
     * @brief A pointer to a publisher
     * @endif
     */
    PublisherBase* m_publisher;

    /*!
     * @if jp
     * @brief ConnectorListenrs への参照
     * @else
     * @brief A reference to a ConnectorListener
     * @endif
     */
    ConnectorListenersBase* m_listeners;

    /*!
     * @if jp
     * @brief Buffer へのポインタ
     * @else
     * @brief A pointer to a buffer
     * @endif
     */
    CdrBufferBase* m_buffer;

  };
} // namespace RTC

#endif  // RTC_PUSH_CONNECTOR_H
