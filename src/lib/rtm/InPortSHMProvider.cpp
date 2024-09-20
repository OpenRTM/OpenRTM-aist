// -*- C++ -*-
/*!
 * @file  InPortCorbaCdrProvider.cpp
 * @brief InPortCorbaCdrProvider class
 * @date  $Date: 2008-01-14 07:49:59 $
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
 * $Id: InPortCorbaCdrProvider.cpp 1244 2009-03-13 07:25:42Z n-ando $
 *
 */

#include <rtm/InPortSHMProvider.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  InPortSHMProvider::InPortSHMProvider()
  {
    // PortProfile setting
    setInterfaceType("shared_memory");
    
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
  InPortSHMProvider::~InPortSHMProvider() = default;

  void InPortSHMProvider::init(coil::Properties& /*prop*/)
  {
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void InPortSHMProvider::
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
  void InPortSHMProvider::setListener(ConnectorInfo& info,
                                           ConnectorListenersBase* listeners)
  {
    m_profile = info;
    m_listeners = listeners;
  }


  void InPortSHMProvider::setConnector(InPortConnector* connector)
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
  ::OpenRTM::PortStatus
  InPortSHMProvider::put()
  {
    RTC_PARANOID(("InPortSHMProvider::put()"));
    if (m_connector == nullptr)
    {
      return ::OpenRTM::PORT_ERROR;
    }

    bool endian_type = m_connector->isLittleEndian();

    try
    {
      setEndian(endian_type);
      read(m_cdr);

      RTC_PARANOID(("received data size: %d", m_cdr.getDataLength()));


    }
    catch (...)
    {

    }

    onReceived(m_cdr);

    BufferStatus ret = m_connector->write(m_cdr);

    return convertReturn(ret, m_cdr);
  }

  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  ::OpenRTM::PortStatus
  InPortSHMProvider::convertReturn(BufferStatus status,
                                        ByteData& data)
  {
    switch(status)
      {
      case BufferStatus::OK:
        onBufferWrite(data);
        return ::OpenRTM::PORT_OK;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        return ::OpenRTM::PORT_ERROR;

      case BufferStatus::FULL:
        onBufferFull(data);
        onReceiverFull(data);
        return ::OpenRTM::BUFFER_FULL;

      case BufferStatus::EMPTY:
        // never come here
        return ::OpenRTM::BUFFER_EMPTY;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        return ::OpenRTM::PORT_ERROR;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        return ::OpenRTM::BUFFER_TIMEOUT;

      case BufferStatus::NOT_SUPPORTED: /* FALLTHROUGH */
      default:
        return ::OpenRTM::UNKNOWN_ERROR;
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
  void InPortSHMProviderInit(void)
  {
    RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
    factory.addFactory("shared_memory",
                       ::coil::Creator< ::RTC::InPortProvider,
                                        ::RTC::InPortSHMProvider>,
                       ::coil::Destructor< ::RTC::InPortProvider,
                                           ::RTC::InPortSHMProvider>);
  }
}
