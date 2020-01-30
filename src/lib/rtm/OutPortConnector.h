// -*- C++ -*-
/*!
 * @file OutPortConnector.h
 * @brief OutPortConnector class
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

#ifndef RTC_OUTPORTCONNECTOR_H
#define RTC_OUTPORTCONNECTOR_H

#include <rtm/SystemLogger.h>
#include <rtm/ConnectorBase.h>
#include <rtm/ConnectorListener.h>
#include <rtm/DirectInPortBase.h>
#include <rtm/PortBase.h>
#include <rtm/CORBA_CdrMemoryStream.h>
#include <rtm/ByteData.h>



namespace RTC
{
  class InPortBase;
  /*!
   * @if jp
   * @class OutPortConnector
   * @brief OutPortConnector 基底クラス
   *
   * OutPort の Push/Pull 各種 Connector を派生させるための
   * 基底クラス。
   *
   * @since 1.0.0
   *
   * @else
   * @class OutPortConnector
   * @brief OutPortConnector base class
   *
   * The base class to derive subclasses for OutPort's Push/Pull Connectors
   *
   * @since 1.0.0
   *
   * @endif
   */
  class OutPortConnector
    : public ConnectorBase
  {
  public:

    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    OutPortConnector(ConnectorInfo& info, ConnectorListenersBase* listeners);

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~OutPortConnector() override;
   /*!
     * @if jp
     * @brief Profile 取得
     *
     * Connector Profile を取得する
     *
     * @else
     * @brief Getting Profile
     *
     * This operation returns Connector Profile
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
     * @else
     * @brief Getting Connector ID
     *
     * This operation returns Connector ID
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
     * @else
     * @brief Getting Connector name
     *
     * This operation returns Connector name
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
     * @else
     * @brief Disconnect connection
     *
     * This operation disconnect this connection
     *
     * @endif
     */
    DataPortStatus disconnect() override = 0;

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
    CdrBufferBase* getBuffer() override = 0;

    /*!
     * @if jp
     * @brief write 関数
     *
     * OutPort からデータを Buffer へ write する関数
     *
     * @else
     * @brief Destructor
     *
     * The write function to write data from OutPort to Buffer
     *
     * @endif
     */
    virtual DataPortStatus write(ByteDataStreamBase* data) = 0;

    /*!
     * @if jp
     * @brief endianタイプ設定
     *
     * endianタイプを設定する
     *
     * @else
     * @brief Setting an endian type
     *
     * This operation set this connector's endian type
     *
     * @endif
     */
    virtual void setEndian(bool endian_type);

    /*!
     * @if jp
     * @brief endian 設定を返す
     *
     * endian 設定のbool値を返す。
     *
     * @return m_littleEndian がlittleの場合true、bigの場合false を返す。
     *
     * @else
     * @brief
     *
     * return it whether endian setting.
     *
     *@return Return true in the case of "little", false in "big" than it.
     *
     * @endif
     */
    virtual bool isLittleEndian();

    /*!
     * @if jp
     * @brief データ型の変換テンプレート
     *
     * Timed* から CdrMemoryStream に変換する。
     *
     * @else
     * @brief The conversion template of the data type
     *
     * This is convert it from Timed* into CdrStream.
     *
     * @endif
     */
    template <class DataType>
    DataPortStatus write(DataType& data)
    {

      if (m_directInPort != nullptr)
        {
          DirectInPortBase<DataType>* inport = dynamic_cast<DirectInPortBase<DataType>*>(m_directInPort->getDirectPort());
          if(inport)
            {
              if (inport->isNew())
                {
                  // ON_BUFFER_OVERWRITE(In,Out), ON_RECEIVER_FULL(In,Out) callback
                  m_listeners->notifyOut(ON_BUFFER_OVERWRITE, m_profile, data);
                  m_inPortListeners->notifyIn(ON_BUFFER_OVERWRITE, m_profile, data);
                  m_listeners->notifyOut(ON_RECEIVER_FULL, m_profile, data);
                  m_inPortListeners->notifyIn(ON_RECEIVER_FULL, m_profile, data);
                  RTC_PARANOID(("ON_BUFFER_OVERWRITE(InPort,OutPort), "
                                "ON_RECEIVER_FULL(InPort,OutPort) "
                                "callback called in direct mode."));
                }
              // ON_BUFFER_WRITE(In,Out) callback
              m_listeners->notifyOut(ON_BUFFER_WRITE, m_profile, data);
              m_inPortListeners->notifyIn(ON_BUFFER_WRITE, m_profile, data);
              RTC_PARANOID(("ON_BUFFER_WRITE(InPort,OutPort), "
                                "callback called in direct mode."));
              inport->write(data);  // write to InPort variable!!
              // ON_RECEIVED(In,Out) callback
              m_listeners->notifyOut(ON_RECEIVED, m_profile, data);
              m_inPortListeners->notifyIn(ON_RECEIVED, m_profile, data);
              RTC_PARANOID(("ON_RECEIVED(InPort,OutPort), "
                            "callback called in direct mode."));
              
              return DataPortStatus::PORT_OK;
            }
        }
      // normal case
      if(m_cdr == nullptr)
      {
          m_cdr = createSerializer<DataType>(m_marshaling_type);
      }
      ::RTC::ByteDataStream<DataType> *cdr = dynamic_cast<::RTC::ByteDataStream<DataType>*>(m_cdr);
      if (!cdr)
      {
          RTC_ERROR(("Can not find Marshalizer: %s", m_marshaling_type.c_str()));
          return DataPortStatus::PORT_ERROR;
      }
      cdr->isLittleEndian(isLittleEndian());
      cdr->serialize(data);
      RTC_TRACE(("connector endian: %s", isLittleEndian() ? "little":"big"));
      
      DataPortStatus ret = write((ByteDataStreamBase*)cdr);

      return ret;
    }

    virtual BufferStatus read(ByteData &data);

    bool setInPort(InPortBase* directInPort);
    /*!
     * @if jp
     * @brief ダイレクト接続モードに設定
     *
     *
     * @else
     * @brief
     *
     *
     * @endif
     */
    virtual void setPullDirectMode();
    /*!
     * @if jp
     * @brief ダイレクト接続モードかの判定
     *
     * @return True：ダイレクト接続モード,false：それ以外
     *
     * @else
     * @brief
     *
     *
     * @endif
     */
    virtual bool pullDirectMode();
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
    virtual void unsubscribeInterface(const coil::Properties& prop);
  protected:
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger streamf
     * @endif
     */
    Logger rtclog;
    /*!
     * @if jp
     * @brief Port の PortProfile
     * @else
     * @brief PortProfile of the Port
     * @endif
     */
    ConnectorInfo m_profile;
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
     * @brief 同一プロセス上のピアInPortのポインタ
     * @else
     * @brief InProt pointer to the peer in the same process
     * @endif
     */
    PortBase* m_directInPort;

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
     * @brief InPort 側の ConnectorListenrs への参照
     * @else
     * @brief A pointer to a InPort's ConnectorListener
     * @endif
     */
    ConnectorListenersBase* m_inPortListeners;

    /*!
     * @if jp
     * @brief ダイレクト接続のフラグ
     * Trueでダイレクト接続モード
     * @else
     * @brief 
     * @endif
     */
    bool m_directMode;

    /*!
     * @if jp
     * @brief シリアライザの名前
     * @else
     * @brief
     * @endif
     */
    std::string m_marshaling_type;
    ByteDataStreamBase* m_cdr;

  };
} // namespace RTC

#endif  // RTC_CONNECTORBASE_H
