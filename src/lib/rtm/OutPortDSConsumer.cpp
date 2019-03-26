// -*- C++ -*-
/*!
 * @file  OutPortDSConsumer.h
 * @brief OutPortDSConsumer class
 * @date  $Date: 2018-09-20 07:49:59 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#include <rtm/Manager.h>
#include <rtm/OutPortDSConsumer.h>
#include <rtm/NVUtil.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OutPortDSConsumer::OutPortDSConsumer()
  {
    rtclog.setName("OutPortDSConsumer");
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortDSConsumer::~OutPortDSConsumer()
  {
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void OutPortDSConsumer::init(coil::Properties&  /*prop*/)
  {
    RTC_TRACE(("OutPortDSConsumer::init()"));
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void OutPortDSConsumer::setBuffer(CdrBufferBase* buffer)
  {
    RTC_TRACE(("OutPortDSConsumer::setBuffer()"));
    m_buffer = buffer;
  }

  /*!
   * @if jp
   * @brief リスナを設定する。
   * @else
   * @brief Set the listener.
   * @endif
   */
  void OutPortDSConsumer::setListener(ConnectorInfo& info,
                                            ConnectorListeners* listeners)
  {
    RTC_TRACE(("OutPortDSConsumer::setListener()"));
    m_listeners = listeners;
    m_profile = info;
  }

  /*!
   * @if jp
   * @brief データを読み出す
   * @else
   * @brief Read data
   * @endif
   */
  OutPortConsumer::ReturnCode
  OutPortDSConsumer::get(ByteData& data)
  {
    RTC_TRACE(("OutPortDSConsumer::get()"));
    ::RTC::OctetSeq_var cdr_data;

    try
      {
        ::RTC::PortStatus ret(_ptr()->pull(cdr_data.out()));

        if (ret == ::RTC::PORT_OK)
          {
            RTC_DEBUG(("get() successful"));
#ifdef ORB_IS_ORBEXPRESS
            data.writeData((unsigned char*)cdr_data.get_buffer(), (CORBA::ULong)cdr_data.length());
#elif defined(ORB_IS_TAO)
            data.writeData((unsigned char*)cdr_data->get_buffer(), (CORBA::ULong)cdr_data->length());
#else
            data.writeData(static_cast<unsigned char*>(&(cdr_data[0])), static_cast<CORBA::ULong>(cdr_data->length()));
#endif
            RTC_PARANOID(("CDR data length: %d", cdr_data->length()));

            onReceived(data);
            onBufferWrite(data);

            if (m_buffer->full())
              {
                RTC_INFO(("InPort buffer is full."));
                onBufferFull(data);
                onReceiverFull(data);
              }
            m_buffer->put(data);
            m_buffer->advanceWptr();
            m_buffer->advanceRptr();

            return PORT_OK;
          }
        return convertReturn(ret, data);
      }
    catch (...)
      {
        RTC_WARN(("Exception caought from OutPort::get()."));
        return CONNECTION_LOST;
      }
  }

  /*!
   * @if jp
   * @brief データ受信通知への登録
   * @else
   * @brief Subscribe the data receive notification
   * @endif
   */
  bool OutPortDSConsumer::
  subscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("OutPortDSConsumer::subscribeInterface()"));
    CORBA::Long index;
    index = NVUtil::find_index(properties,
                               "dataport.data_service.outport_ior");
    if (index < 0)
      {
        RTC_DEBUG(("dataport.data_service.outport_ior not found."));
        return false;
      }

    if (NVUtil::isString(properties,
                         "dataport.data_service.outport_ior"))
      {
        RTC_DEBUG(("dataport.data_service.outport_ior found."));
        const char* ior(nullptr);
        properties[index].value >>= ior;

        CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
        CORBA::Object_var var = orb->string_to_object(ior);
        bool ret(setObject(var.in()));
        if (ret)
          {
            RTC_DEBUG(("CorbaConsumer was set successfully."));
          }
        else
          {
            RTC_ERROR(("Invalid object reference."))
          }
        return ret;
      }

    return false;
  }

  /*!
   * @if jp
   * @brief データ受信通知からの登録解除
   * @else
   * @brief Unsubscribe the data receive notification
   * @endif
   */
  void OutPortDSConsumer::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("OutPortDSConsumer::unsubscribeInterface()"));
    CORBA::Long index;
    index = NVUtil::find_index(properties,
                               "dataport.data_service.outport_ior");
    if (index < 0)
      {
        RTC_DEBUG(("dataport.data_service.outport_ior not found."));
        return;
      }

    const char* ior;
    if (properties[index].value >>= ior)
      {
        RTC_DEBUG(("dataport.data_service.outport_ior found."));
        CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
        CORBA::Object_var var = orb->string_to_object(ior);
        if (_ptr()->_is_equivalent(var))
          {
            releaseObject();
            RTC_DEBUG(("CorbaConsumer's reference was released."));
            return;
          }
        RTC_ERROR(("hmm. Inconsistent object reference."));
      }
  }

  /*!
   * @if jp
   * @brief リターンコード変換 (DataPortStatus -> BufferStatus)
   * @else
   * @brief Return codes conversion
   * @endif
   */
  OutPortConsumer::ReturnCode
  OutPortDSConsumer::convertReturn(::RTC::PortStatus status,
                                         ByteData&  /*data*/)
  {
    switch (status)
      {
      case ::RTC::PORT_OK:
        // never comes here
        return PORT_OK;
        break;

      case ::RTC::PORT_ERROR:
        onSenderError();
        return PORT_ERROR;
        break;

      case ::RTC::BUFFER_FULL:
        // never comes here
        return BUFFER_FULL;
        break;

      case ::RTC::BUFFER_EMPTY:
        onSenderEmpty();
        return BUFFER_EMPTY;
        break;

      case ::RTC::BUFFER_TIMEOUT:
        onSenderTimeout();
        return BUFFER_TIMEOUT;
        break;

      case ::RTC::UNKNOWN_ERROR:
        onSenderError();
        return UNKNOWN_ERROR;
        break;

      default:
        onSenderError();
        return UNKNOWN_ERROR;
      }
  }


} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void OutPortDSConsumerInit(void)
  {
    RTC::OutPortConsumerFactory&
      factory(RTC::OutPortConsumerFactory::instance());
    factory.addFactory("data_service",
                       ::coil::Creator< ::RTC::OutPortConsumer,
                                        ::RTC::OutPortDSConsumer>,
                       ::coil::Destructor< ::RTC::OutPortConsumer,
                                           ::RTC::OutPortDSConsumer>);
  }
}
