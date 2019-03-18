// -*- C++ -*-
/*!
 * @file  OutPortSHMProvider.cpp
 * @brief OutPortSHMProvider class
 * @date  $Date: 2017-10-30 07:49:59 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017 Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 */

#include <rtm/OutPortSHMProvider.h>
#include <coil/UUID.h>
#include <memory>

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
  OutPortSHMProvider::OutPortSHMProvider()
   : m_buffer(nullptr),
     m_connector(nullptr),
     m_memory_size(0)
  {
    // PortProfile setting
    setInterfaceType("shared_memory");
    
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
                NVUtil::newNV("dataport.corba_cdr.outport_ior", ior));
#else // ORB_IS_RTORB
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.corba_cdr.outport_ior", ior.in()));
#endif // ORB_IS_RTORB
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.corba_cdr.outport_ref", m_objref));

    coil::UUID_Generator uugen;
    uugen.init();
    std::unique_ptr<coil::UUID> uuid(uugen.generateUUID(2, 0x01));
    m_shm_address = uuid->to_string();
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortSHMProvider::~OutPortSHMProvider()
  {

  }
  
  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void OutPortSHMProvider::init(coil::Properties& prop)
  {
	std::string ds = prop["shem_default_size"];
	m_memory_size = string_to_MemorySize(ds);

	if (prop.hasKey("serializer") == nullptr)
	{
		m_endian = true;
		return;
	}

	
	std::string endian_type(prop.getProperty("serializer.cdr.endian", ""));
	coil::normalize(endian_type);
	std::vector<std::string> endian(coil::split(endian_type, ","));
	if (endian.empty()) { return; }
	if (endian[0] == "little")
	{
		m_endian = true;
	}
	else if (endian[0] == "big")
	{
		m_endian = false;
		return;
	}
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void OutPortSHMProvider::setBuffer(CdrBufferBase* buffer)
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
  void OutPortSHMProvider::setListener(ConnectorInfo& info,
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
  void OutPortSHMProvider::setConnector(OutPortConnector* connector)
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
  ::OpenRTM::PortStatus
  OutPortSHMProvider::get()
    throw (CORBA::SystemException)
  {
    RTC_PARANOID(("OutPortSHMProvider::get()"));
    // at least the output "data" area should be allocated


    if (m_buffer == nullptr)
      {
        onSenderError();
        return ::OpenRTM::UNKNOWN_ERROR;
      }

    ByteData cdr;
    CdrBufferBase::ReturnCode ret(m_connector->read(cdr));
    if (ret == CdrBufferBase::BUFFER_OK)
      {
        CORBA::ULong len(static_cast<CORBA::ULong>(cdr.getDataLength()));
        RTC_PARANOID(("converted CDR data size: %d", len));
	if (len == static_cast<CORBA::ULong>(0)) {
	  RTC_ERROR(("buffer is empty."));
	  return ::OpenRTM::BUFFER_EMPTY;
	}
	bool endian_type = m_connector->isLittleEndian();
	setEndian(endian_type);
	create_memory(m_memory_size, m_shm_address.c_str());
	write(cdr);
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
  ::OpenRTM::PortStatus
  OutPortSHMProvider::convertReturn(BufferStatus::Enum status,
                                        ByteData& data)
  {
    switch(status)
      {
      case BufferStatus::BUFFER_OK:
        onBufferRead(data);
        onSend(data);
        return ::OpenRTM::PORT_OK;
        break;

      case BufferStatus::BUFFER_ERROR:
        onSenderError();
        return ::OpenRTM::PORT_ERROR;
        break;

      case BufferStatus::BUFFER_FULL:
        // never come here
        return ::OpenRTM::BUFFER_FULL;
        break;

      case BufferStatus::BUFFER_EMPTY:
        onBufferEmpty();
        onSenderEmpty();
        return ::OpenRTM::BUFFER_EMPTY;
        break;

      case BufferStatus::PRECONDITION_NOT_MET:
        onSenderError();
        return ::OpenRTM::PORT_ERROR;
        break;

      case BufferStatus::TIMEOUT:
        onBufferReadTimeout();
        onSenderTimeout();
        return ::OpenRTM::BUFFER_TIMEOUT;
        break;

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
  void OutPortSHMProviderInit(void)
  {
    RTC::OutPortProviderFactory&
      factory(RTC::OutPortProviderFactory::instance());
    factory.addFactory("shared_memory",
                       ::coil::Creator< ::RTC::OutPortProvider,
                                        ::RTC::OutPortSHMProvider>,
                       ::coil::Destructor< ::RTC::OutPortProvider,
                                           ::RTC::OutPortSHMProvider>);
  }
};
