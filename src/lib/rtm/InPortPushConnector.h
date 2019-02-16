// -*- C++ -*-
/*!
 * @file InPortPushConnector.h
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

#ifndef RTC_INPORTPUSHCONNECTOR_H
#define RTC_INPORTPUSHCONNECTOR_H

#include <rtm/InPortConnector.h>
#include <rtm/InPortConsumer.h>
#include <rtm/PublisherBase.h>

namespace RTC
{
  class InPortProvider;

  /*!
   * @if jp
   * @class InPortPushConnector
   * @brief InPortPushConnector クラス
   *
   * InPort の push 型データフローのための Connector クラス。このオブ
   * ジェクトは、接続時に dataflow_type に push が指定された場合、
   * InPort によって生成・所有され、OutPortPushConnector と対になって、
   * データポートの push 型のデータフローを実現する。一つの接続に対して、
   * 一つのデータストリームを提供する唯一の Connector が対応する。
   * Connector は 接続時に生成される UUID 形式の ID により区別される。
   *
   * InPortPushConnector は以下の三つのオブジェクトを所有し管理する。
   *
   * - InPortProvider
   * - Buffer
   *
   * OutPort に書き込まれたデータは、OutPortConnector によって
   * InPortProvider::put() にデータが渡される。書き込まれたデータは
   * Connector 内で Buffer にデータが書き込まれる。
   *
   * @since 1.0.0
   *
   * @else
   * @class InPortPushConnector
   * @brief InPortPushConnector class
   *
   * Connector class of InPort for push type dataflow.  When "push"
   * is specified as dataflow_type at the time of establishing
   * connection, this object is generated and owned by the InPort.
   * This connector and OutPortPushConnector make a pair and realize
   * push type dataflow of data ports.  One connector corresponds to
   * one connection which provides a data stream.  Connector is
   * distinguished by ID of the UUID that is generated at establishing
   * connection.
   *
   * InPortPushConnector owns and manages the following objects.
   *
   * - OutPortConsumer
   * - Buffer
   *
   * Data written into the OutPort are passed to the
   * InPortProvider::put() by OutPortConnector.  The data is written
   * into the buffer in the connector.
   *
   * @since 1.0.0
   *
   * @endif
   */
  class InPortPushConnector
    : public InPortConnector
  {
    typedef coil::Guard<coil::Mutex> Guard;
  public:
    DATAPORTSTATUS_ENUM

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * InPortPushConnector のコンストラクタはオブジェクト生成時に下記を
     * 引数にとる。ConnectorInfo は接続情報を含み、この情報に従いバッファ
     * 等を生成する。InPort インターフェースのプロバイダオブジェクトへ
     * のポインタを取り、所有権を持つので、InPortPushConnector は
     * InPortProvider の解体責任を持つ。各種イベントに対するコールバッ
     * ク機構を提供する ConnectorListeners を持ち、適切なタイミングでコー
     * ルバックを呼び出す。データバッファがもし InPortBase から提供され
     * る場合はそのポインタを取る。
     *
     * @param info ConnectorInfo
     * @param provider InPortProvider
     * @param listeners ConnectorListeners 型のリスナオブジェクトリスト
     * @param buffer CdrBufferBase 型のバッファ
     *
     * @elsek
     * @brief Constructor
     *
     * InPortPushConnector's constructor is given the following
     * arguments.  According to ConnectorInfo which includes
     * connection information, a buffer is created.
     * It is also given a pointer to the provider object for the
     * InPort interface.  The owner-ship of the pointer is owned by
     * this InPortPushConnector, it has responsibility to destruct
     * the InPortProvider.  InPortPushConnector also has
     * ConnectorListeners to provide event callback mechanisms, and
     * they would be called at the proper timing.  If data buffer is
     * given by InPortBase, the pointer to the buffer is also given
     * as arguments.
     *
     * @param info ConnectorInfo
     * @param provider InPortProvider
     * @param listeners ConnectorListeners type lsitener object list
     * @param buffer CdrBufferBase type buffer
     *
     * @endif
     */
    InPortPushConnector(ConnectorInfo info,
                        InPortProvider* provider,
                        ConnectorListeners& listeners,
                        CdrBufferBase* buffer = 0);

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
    virtual ~InPortPushConnector();

    /*!
     * @if jp
     * @brief データの読み出し
     *
     * バッファからデータを読み出す。正常に読み出せた場合、戻り値は
     * PORT_OK となり、data に読み出されたデータが格納される。それ以外
     * の場合には、エラー値として BUFFER_EMPTY, TIMEOUT,
     * PRECONDITION_NOT_MET, PORT_ERROR が返される。
     *
     * @return PORT_OK              正常終了
     *         BUFFER_EMPTY         バッファは空である
     *         TIMEOUT              タイムアウトした
     *         PRECONDITION_NOT_MET 事前条件を満たさない
     *         PORT_ERROR           その他のエラー
     *
     * @else
     *
     * @brief Reading data
     *
     * This function reads data from the buffer. If data is read
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
    virtual ReturnCode read(cdrMemoryStream& data);

    /*!
     * @if jp
     * @brief 接続解除
     *
     * consumer, publisher, buffer が解体・削除される。
     *
     * @return PORT_OK
     *
     * @else
     *
     * @brief disconnect
     *
     * This operation destruct and delete the consumer, the publisher
     * and the buffer.
     *
     * @return PORT_OK
     *
     * @endif
     */
    virtual ReturnCode disconnect();

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
    virtual void activate() {}  // do nothing

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
    virtual void deactivate() {}  // do nothing

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
    virtual CdrBufferBase* createBuffer(ConnectorInfo& info);

    virtual BufferStatus::Enum write(cdrMemoryStream &cdr);

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
    
    inline void onBufferRead(cdrMemoryStream& data)
    {
      m_listeners.
        connectorData_[ON_BUFFER_READ].notify(m_profile, data);

    }
    void onBufferEmpty(cdrMemoryStream& data)
    {
      m_listeners.
        connector_[ON_BUFFER_EMPTY].notify(m_profile);

    }
    void onBufferReadTimeout(cdrMemoryStream& data)
    {
      m_listeners.
        connector_[ON_BUFFER_READ_TIMEOUT].notify(m_profile);
    }

  private:
    /*!
     * @if jp
     * @brief InPortConsumer へのポインタ
     * @else
     * @brief the pointer to the InPortConsumer
     * @endif
     */
    InPortProvider* m_provider;

    /*!
     * @if jp
     * @brief ConnectorListenrs への参照
     * @else
     * @brief A reference to a ConnectorListener
     * @endif
     */
    ConnectorListeners& m_listeners;

    bool m_deleteBuffer;

    bool m_sync_readwrite;

    struct WorkerThreadCtrl
    {
        WorkerThreadCtrl() : cond_(mutex_), completed_(false) {}
        coil::Mutex mutex_;
        coil::Condition<coil::Mutex> cond_;
        bool completed_;
    };
    WorkerThreadCtrl m_writecompleted_worker;
    WorkerThreadCtrl m_readcompleted_worker;
    WorkerThreadCtrl m_readready_worker;

  };
};  // namespace RTC

#endif  // RTC_PUSH_CONNECTOR_H
