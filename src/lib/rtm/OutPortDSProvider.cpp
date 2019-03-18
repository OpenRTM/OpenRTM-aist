// -*- C++ -*-
/*!
 * @file  OutPortDSProvider.cpp
 * @brief OutPortDSProvider class
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

#include <rtm/OutPortDSProvider.h>

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OutPortDSProvider::OutPortDSProvider()
    : m_buffer(nullptr), m_connector(nullptr)
  {
    // PortProfile setting
    setInterfaceType("data_service");

    // ConnectorProfile setting
#ifdef ORB_IS_OMNIORB
    ::RTC::Manager::instance().theShortCutPOA()->activate_object(this);
#endif
    m_objref = this->_this();

    // set outPort's reference
    CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
    CORBA::String_var ior = orb->object_to_string(m_objref.in());
#ifndef ORB_IS_RTORB
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.data_service.outport_ior", ior));
#else  // ORB_IS_RTORB
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.data_service.outport_ior", ior.in()));
#endif  // ORB_IS_RTORB
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.data_service.outport_ref", m_objref));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortDSProvider::~OutPortDSProvider()
  {
    try
      {
        PortableServer::ObjectId_var oid;
#ifdef ORB_IS_OMNIORB
        oid = ::RTC::Manager::instance().theShortCutPOA()->servant_to_id(this);
        ::RTC::Manager::instance().theShortCutPOA()->deactivate_object(oid);
#else
        oid = _default_POA()->servant_to_id(this);
        _default_POA()->deactivate_object(oid);
#endif
      }
    catch (PortableServer::POA::ServantNotActive &e)
      {
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("%s", e._name()));
#endif

      }
    catch (PortableServer::POA::WrongPolicy &e)
      {
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("%s", e._name()));
#endif
      }
    catch (...)
      {
        // never throws exception
        RTC_ERROR(("Unknown exception caught."));
      }
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void OutPortDSProvider::init(coil::Properties& /*prop*/)
  {
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void OutPortDSProvider::setBuffer(CdrBufferBase* buffer)
  {
    m_buffer = buffer;
  }

  /*!
   * @if jp
   * @brief リスナを設定する。
   * @else
   * @brief Set the listener.
   * @endif
   */
  void OutPortDSProvider::setListener(ConnectorInfo& info,
                                            ConnectorListeners* listeners)
  {
    m_profile = info;
    m_listeners = listeners;
  }

  /*!
   * @if jp
   * @brief コネクタをセットする
   * @else
   * @brief Setting outside connector's pointer
   * @endif
   */
  void OutPortDSProvider::setConnector(OutPortConnector* connector)
  {
    m_connector = connector;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] バッファからデータを取得する
   * @else
   * @brief [CORBA interface] Get data from the buffer
   * @endif
   */
  ::RTC::PortStatus
      OutPortDSProvider::pull(::RTC::OctetSeq_out data)
    throw (CORBA::SystemException)
  {
    RTC_PARANOID(("OutPortDSProvider::get()"));
    // at least the output "data" area should be allocated
    data = new ::RTC::OctetSeq();

    if (m_connector == nullptr)
      {
        onSenderError();
        return ::RTC::UNKNOWN_ERROR;
      }

    ByteData cdr;
    CdrBufferBase::ReturnCode ret(m_connector->read(cdr));

    if (ret == CdrBufferBase::BUFFER_OK)
      {
        CORBA::ULong len(static_cast<CORBA::ULong>(cdr.getDataLength()));
        RTC_PARANOID(("converted CDR data size: %d", len));

        if (len == static_cast<CORBA::ULong>(0)) {
          RTC_ERROR(("buffer is empty."));
          return ::RTC::BUFFER_EMPTY;
        }
#ifndef ORB_IS_RTORB
        data->length(len);
        cdr.readData(static_cast<unsigned char*>(data->get_buffer()), len);
#else
        data->length(len);
        cdr.readData(reinterpret_cast<char *>(&((*data)[0]),
                                        static_cast<int>(len)));
#endif  // ORB_IS_RTORB
      }

    return convertReturn(ret, cdr);
  }

  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  ::RTC::PortStatus
  OutPortDSProvider::convertReturn(BufferStatus::Enum status,
                                         ByteData& data)
  {
    switch (status)
      {
      case BufferStatus::BUFFER_OK:
        onBufferRead(data);
        onSend(data);
        return ::RTC::PORT_OK;
        break;

      case BufferStatus::BUFFER_ERROR:
        onSenderError();
        return ::RTC::PORT_ERROR;
        break;

      case BufferStatus::BUFFER_FULL:
        // never come here
        return ::RTC::BUFFER_FULL;
        break;

      case BufferStatus::BUFFER_EMPTY:
        onBufferEmpty();
        onSenderEmpty();
        return ::RTC::BUFFER_EMPTY;
        break;

      case BufferStatus::PRECONDITION_NOT_MET:
        onSenderError();
        return ::RTC::PORT_ERROR;
        break;

      case BufferStatus::TIMEOUT:
        onBufferReadTimeout();
        onSenderTimeout();
        return ::RTC::BUFFER_TIMEOUT;
        break;

      default:
        return ::RTC::UNKNOWN_ERROR;
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
  void OutPortDSProviderInit(void)
  {
    RTC::OutPortProviderFactory&
      factory(RTC::OutPortProviderFactory::instance());
    factory.addFactory("data_service",
                       ::coil::Creator< ::RTC::OutPortProvider,
                                        ::RTC::OutPortDSProvider>,
                       ::coil::Destructor< ::RTC::OutPortProvider,
                                           ::RTC::OutPortDSProvider>);
  }
};
