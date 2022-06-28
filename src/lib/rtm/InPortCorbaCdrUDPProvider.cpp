// -*- C++ -*-
/*!
 * @file  InPortCorbaCdrUDPProvider.cpp
 * @brief InPortCorbaCdrUDPProvider class
 * @date  $Date: 2018-01-15 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#include <rtm/InPortCorbaCdrUDPProvider.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  InPortCorbaCdrUDPProvider::InPortCorbaCdrUDPProvider(void)
   : m_buffer(nullptr) 
  {
    // PortProfile setting
    setInterfaceType("corba_cdr_udp");
    
    // ConnectorProfile setting

#ifdef ORB_IS_OMNIORB
    PortableServer::ObjectId_var oid = ::RTC::Manager::instance().theShortCutPOA()->activate_object(this);
#endif
    m_objref = this->_this();
    
    // set InPort's reference
    CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
    CORBA::String_var ior = orb->object_to_string(m_objref.in());
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.corba_cdr.inport_ior", ior.in()));
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.corba_cdr.inport_ref", m_objref));
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortCorbaCdrUDPProvider::~InPortCorbaCdrUDPProvider(void)
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

  void InPortCorbaCdrUDPProvider::init(coil::Properties& /*prop*/)
  {
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void InPortCorbaCdrUDPProvider::
  setBuffer(BufferBase<ByteData>* buffer)
  {
    m_buffer = buffer;
  }

  /*!
   * @if jp
   * @brief リスナを設定する
   * @else
   * @brief Set the listener
   * @endif
   */
  void InPortCorbaCdrUDPProvider::setListener(ConnectorInfo& info,
                                           ConnectorListenersBase* listeners)
  {
    m_profile = info;
    m_listeners = listeners;
  }

  /*!
   * @if jp
   * @brief Connectorを設定する。
   * @else
   * @brief set Connector
   * @endif
   */
  void InPortCorbaCdrUDPProvider::setConnector(InPortConnector* connector)
  {
    m_connector = connector;
  }

  /*!
   * @if jp
   * @brief バッファにデータを書き込む
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  void
  InPortCorbaCdrUDPProvider::put(const ::OpenRTM::CdrData& data)
  {
    RTC_PARANOID(("InPortCorbaCdrUDPProvider::put()"));

    if (m_buffer == nullptr)
      {

        m_cdr.writeData((unsigned char*)data.get_buffer(), data.length());

        onReceiverError(m_cdr);
        return;
      }

    RTC_PARANOID(("received data size: %d", data.length()));
    // set endian type
    bool endian_type = m_connector->isLittleEndian();
    RTC_TRACE(("connector endian: %s", endian_type ? "little":"big"));

    m_cdr.isLittleEndian(endian_type);
    m_cdr.writeData((unsigned char*)&(data[0]), data.length());
    RTC_PARANOID(("converted CDR data size: %d", m_cdr.getDataLength()));


    onReceived(m_cdr);
    BufferStatus ret = m_buffer->write(m_cdr);

    convertReturn(ret, m_cdr);
  }

  /*!
  * @if jp
  * @brief リターンコード変換
  * @else
  * @brief Return codes conversion
  * @endif
  */
  void
  InPortCorbaCdrUDPProvider::convertReturn(BufferStatus status,
                                           ByteData& data)
  {
    switch (status)
    {
      case BufferStatus::OK:
        onBufferWrite(data);
         break;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        break;

      case BufferStatus::FULL:
        onBufferFull(data);
        onReceiverFull(data);
        break;

      case BufferStatus::EMPTY:
        // never come here
        break;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        break;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        break;
    case BufferStatus::NOT_SUPPORTED: /* FALLTHROUGH */
    default:
      return;

    }

    onReceiverError(data);
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
  void InPortCorbaCdrUDPProviderInit(void)
  {
    RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
    factory.addFactory("corba_cdr_udp",
                       ::coil::Creator< ::RTC::InPortProvider,
                                        ::RTC::InPortCorbaCdrUDPProvider>,
                       ::coil::Destructor< ::RTC::InPortProvider,
                                           ::RTC::InPortCorbaCdrUDPProvider>);
  }
}
