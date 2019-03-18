// -*- C++ -*-
/*!
 * @file InPortConnector.h
 * @brief InPortConnector base class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009-2010
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

#ifndef RTC_INPORTCONNECTOR_H
#define RTC_INPORTCONNECTOR_H

#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>
#include <rtm/DirectOutPortBase.h>
#include <rtm/PortBase.h>
#include <rtm/ByteData.h>


namespace RTC
{
  class OutPortBase;
  /*!
   * @if jp
   * @class InPortConnector
   * @brief InPortConnector 基底クラス
   *
   * InPort の Push/Pull 各種 Connector を派生させるための基底クラス。
   *
   * @since 1.0.0
   *
   * @else
   * @class InPortConnector
   * @brief InPortConnector base class
   *
   * The base class to derive subclasses for InPort's Push/Pull Connectors
   *
   * @since 1.0.0
   *
   * @endif
   */
  class InPortConnector
    : public ConnectorBase
  {
  public:
    DATAPORTSTATUS_ENUM
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * @param info 接続情報を含む ConnectorInfo オブジェクト
     * @param buffer このコネクタのバッファへのポインタ
     *
     * @else
     * @brief Constructor
     *
     * @param info ConnectorInfo object which includes connection information
     * @param buffer A pointer to the buffer of the connector
     *
     * @endif
     */
    InPortConnector(ConnectorInfo& info,
                    ConnectorListeners& listeners,
                    CdrBufferBase* buffer);

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~InPortConnector() override;

   /*!
     * @if jp
     * @brief ConnectorInfo 取得
     *
     * Connector ConnectorInfo を取得する
     *
     * @return このコネクタが保持する ConnectorInfo オブジェクト
     *
     * @else
     * @brief Getting ConnectorInfo
     *
     * This operation returns ConnectorInfo
     *
     * @return ConnectorInfo object which is owned by this connector
     *
     * @endif
     */
    const ConnectorInfo& profile() override;

    /*!
     * @if jp
     * @brief Connector ID 取得
     *
     * Connector ID を取得する
     *
     * @return コネクタ ID 文字列へのポインタ
     *
     * @else
     * @brief Getting Connector ID
     *
     * This operation returns Connector ID
     *
     * @return A pointer to the connector id string
     *
     * @endif
     */
    const char* id() override;

    /*!
     * @if jp
     * @brief Connector 名取得
     *
     * Connector 名を取得する
     *
     * @return コネクタ名文字列へのポインタ
     *
     * @else
     * @brief Getting Connector name
     *
     * This operation returns Connector name
     *
     * @return A pointer to the connector's name string
     *
     * @endif
     */
    const char* name() override;

    /*!
     * @if jp
     * @brief 接続解除関数
     *
     * Connector が保持している接続を解除する
     *
     * @return ReturnCode
     *
     * @else
     * @brief Disconnect connection
     *
     * This operation disconnect this connection
     *
     * @return ReturnCode
     *
     * @endif
     */
    ReturnCode disconnect() override = 0;

    /*!
     * @if jp
     * @brief Buffer を取得する
     *
     * Connector が保持している Buffer を返す
     *
     * @return このコネクタが保持するバッファへのポインタ
     *
     * @else
     * @brief Getting Buffer
     *
     * This operation returns this connector's buffer
     *
     * @return A pointer to the buffer owned by this connector
     *
     * @endif
     */
    CdrBufferBase* getBuffer() override;

    /*!
     * @if jp
     * @brief read 関数
     *
     * Buffer からデータを InPort へ read する関数
     *
     * @param data このコネクタから読み出されるデータを格納する変数への参照
     * @return ReturnCode
     *
     * @else
     * @brief Destructor
     *
     * The read function to read data from buffer to InPort
     *
     * @param data A reference to a variable to which data from this
     *             connector is stored.
     * @return ReturnCode
     *
     * @endif
     */
    virtual ReturnCode read(ByteDataStreamBase* data) = 0;


    /*!
     * @if jp
     * @brief データ型の変換テンプレート
     *
     *
     * @param data データを格納する変数
     *
     * @return ReturnCode
     *
     * @else
     * @brief 
     *
     *
     * @param data
     * @return 
     *
     * @endif
     */
    template<class DataType>
    ReturnCode read(DataType& data)
    {
        ::RTC::ByteDataStream<DataType> *cdr = coil::GlobalFactory < ::RTC::ByteDataStream<DataType> >::instance().createObject(m_marshaling_type);

        
        if (!cdr)
        {
            RTC_ERROR(("Can not find Marshalizer: %s", m_marshaling_type.c_str()));
            return PORT_ERROR;
        }
        ReturnCode ret = read((ByteDataStreamBase*)cdr);
        if (ret == PORT_OK)
        {
            cdr->isLittleEndian(isLittleEndian());
            cdr->deserialize(data);
        }
        coil::GlobalFactory < ::RTC::ByteDataStream<DataType> >::instance().deleteObject(cdr);
        return ret;
    }

    /*!
     * @if jp
     * @brief endianタイプ設定
     *
     * endianタイプを設定する
     *
     * @param endian_type true: little, false: big
     *
     * @else
     * @brief Setting an endian type
     *
     * This operation set this connector's endian type
     *
     * @param endian_type true: little, false: big
     *
     * @endif
     */
    virtual void setEndian(bool endian_type);

    /*!
     * @if jp
     * @brief endian 設定を返す
     *
     * このコネクタに設定されたエンディアンが little endian かどうか。
     *
     * @return true: little endian, false: big endian
     *
     * @else
     * @brief Whether this connector's endian is little
     *
     * This operation returns whether the connector's endian is little or not.
     *
     * @return true: little endian, false: big endian
     *
     * @endif
     */
    virtual bool isLittleEndian();

    virtual BufferStatus::Enum write(ByteData &cdr);


    /*!
    * @if jp
    * @brief データをダイレクトに書き込むためのOutPortのサーバントを設定する
    *
    * @param directOutPort OutPortのサーバント
    *
    * @return True: 設定に成功 False: 既に設定済みのため失敗
    *
    * @else
    * @brief
    *
    * @param directOutPort
    *
    * @return true: little endian, false: big endian
    *
    * @endif
    */
    bool setOutPort(OutPortBase* directOutPort);

    /*!
    * @if jp
    * @brief ダイレクト接続時に変数渡しでデータを取得する
    *
    * @param data 格納先のデータ
    *
    * @return True: 取得成功 False: 取得失敗
    *
    * @else
    * @brief
    *
    * @param data
    *
    * @return 
    *
    * @endif
    */
    template <typename DataType>
    bool getDirectData(DataType &data)
    {
        if (m_directOutPort == nullptr)
        {
            return false;
        }
        DirectOutPortBase<DataType>* outport;
        outport = dynamic_cast<DirectOutPortBase<DataType>*>(m_directOutPort->getDirectPort());

        if(outport)
        {
            if (outport->isEmpty())
            {
                m_listeners.
                    connector_[ON_BUFFER_EMPTY].notify(m_profile);
                m_outPortListeners->
                    connector_[ON_SENDER_EMPTY].notify(m_profile);
                RTC_PARANOID(("ON_BUFFER_EMPTY(InPort,OutPort), "
                    "ON_SENDER_EMPTY(InPort,OutPort) "
                    "callback called in direct mode."));
            }
            outport->read(data);
            m_outPortListeners->connectorData_[ON_BUFFER_READ].notify(m_profile, data);
            RTC_TRACE(("ON_BUFFER_READ(OutPort), "));
            RTC_TRACE(("callback called in direct mode."));
            m_outPortListeners->connectorData_[ON_SEND].notify(m_profile, data);
            RTC_TRACE(("ON_SEND(OutPort), "));
            RTC_TRACE(("callback called in direct mode."));
            m_listeners.connectorData_[ON_RECEIVED].notify(m_profile, data);
            RTC_TRACE(("ON_RECEIVED(InPort), "));
            RTC_TRACE(("callback called in direct mode."));
            m_listeners.connectorData_[ON_SEND].notify(m_profile, data);
            RTC_TRACE(("ON_BUFFER_WRITE(InPort), "));
            RTC_TRACE(("callback called in direct mode."));


            return true;
        }
        return false;
    }

  protected:
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    Logger rtclog;
    /*!
     * @if jp
     * @brief ConnectorInfo
     * @else
     * @brief ConnectorInfo
     * @endif
     */
    ConnectorInfo m_profile;
    /*!
     * @if jp
     * @brief ConnectorListenrs への参照
     * @else
     * @brief A reference to a ConnectorListener
     * @endif
     */
    ConnectorListeners& m_listeners;
    /*!
     * @if jp
     * @brief Connector が保持している Buffer
     * @else
     * @brief Connector's buffer
     * @endif
     */
    CdrBufferBase* m_buffer;
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
     * @brief OutPort 側の ConnectorListenrs への参照
     * @else
     * @brief A pointer to a OutPort's ConnectorListener
     * @endif
     */
    ConnectorListeners* m_outPortListeners;
    /*!
     * @if jp
     * @brief 同一プロセス上のピアOutPortのポインタ
     * @else
     * @brief OutProt pointer to the peer in the same process
     * @endif
     */
    PortBase* m_directOutPort;

    /*!
     * @if jp
     * @brief シリアライザの名前
     * @else
     * @brief
     * @endif
     */
    std::string m_marshaling_type;

  };
} // namespace RTC

#endif  // RTC_INPORTCONNECTOR_H
