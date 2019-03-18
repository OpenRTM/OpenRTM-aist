// -*- C++ -*-
/*!
 * @file  InPortSHMConsumer.h
 * @brief InPortSHMConsumer class
 *
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


#include <rtm/NVUtil.h>
#include <rtm/InPortSHMConsumer.h>
#include <coil/UUID.h>
#include <coil/Guard.h>
#include <memory>


namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @param buffer The buffer object that is attached to this Consumer
   * @endif
   */
	InPortSHMConsumer::InPortSHMConsumer()
		: m_memory_size(0),
		m_endian(true),
		rtclog("InPortSHMConsumer")
  {
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
	InPortSHMConsumer::~InPortSHMConsumer()
  {
    RTC_PARANOID(("~InPortSHMConsumer()"));
	m_shmem.close_memory(true);

  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
	void InPortSHMConsumer::init(coil::Properties& prop)
  {
    m_properties = prop;
	std::string ds = m_properties["shem_default_size"];
	m_memory_size = m_shmem.string_to_MemorySize(ds);

	if (m_properties.hasKey("serializer") == nullptr)
	{
		m_endian = true;
		return;
	}

	
	std::string endian_type(m_properties.getProperty("serializer.cdr.endian", ""));
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


	bool InPortSHMConsumer::setObject(CORBA::Object_ptr obj)
  {
    RTC_PARANOID(("setObject()"));
	if (CorbaConsumer< ::OpenRTM::PortSharedMemory >::setObject(obj))
	{
		m_shmem.setInterface(_ptr());
		return true;
	}


    return false;
  }


	/*!
	* @if jp
	* @brief バッファへのデータ書込
	* @else
	* @brief Write data into the buffer
	* @endif
	*/
	InPortConsumer::ReturnCode InPortSHMConsumer::put(ByteData& data)
	{
		RTC_PARANOID(("put()"));

		
		try
		{

			Guard guard(m_mutex);
			m_shmem.setEndian(m_endian);

			m_shmem.create_memory(m_memory_size, m_shm_address.c_str());

			m_shmem.write(data);

			return convertReturnCode(_ptr()->put());
		}
		catch (...)
		{
			return CONNECTION_LOST;
		}
		return UNKNOWN_ERROR;
	}

  
	void InPortSHMConsumer::
		publishInterfaceProfile(SDOPackage::NVList&  /*properties*/)
	{
		return;
	}
	bool InPortSHMConsumer::
		subscribeInterface(const SDOPackage::NVList& properties)
	{
		RTC_TRACE(("subscribeInterface()"));
		RTC_DEBUG_STR((NVUtil::toString(properties)));

		// getting InPort's ref from IOR string
		if (subscribeFromIor(properties)) { return true; }

		// getting InPort's ref from Object reference
		if (subscribeFromRef(properties)) { return true; }

		return false;;
	}
	void InPortSHMConsumer::
		unsubscribeInterface(const SDOPackage::NVList& properties)
	{
		RTC_TRACE(("unsubscribeInterface()"));
		RTC_DEBUG_STR((NVUtil::toString(properties)));

		if (unsubscribeFromIor(properties)) { return; }
		unsubscribeFromRef(properties);
	}
	bool InPortSHMConsumer::
		subscribeFromIor(const SDOPackage::NVList& properties)
	{
		RTC_TRACE(("subscribeFromIor()"));

		CORBA::Long index;
		index = NVUtil::find_index(properties,
			"dataport.corba_cdr.inport_ior");
		if (index < 0)
		{
			RTC_ERROR(("inport_ior not found"));
			return false;
		}

		const char* ior(nullptr);
		if (!(properties[index].value >>= ior))
		{
			RTC_ERROR(("inport_ior has no string"));
			return false;
		}

		CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
		CORBA::Object_var obj = orb->string_to_object(ior);

		if (CORBA::is_nil(obj))
		{
			RTC_ERROR(("invalid IOR string has been passed"));
			return false;
		}

		if (!setObject(obj.in()))
		{
			RTC_WARN(("Setting object to consumer failed."));
			return false;
		}
		return true;
	}
	bool InPortSHMConsumer::
		subscribeFromRef(const SDOPackage::NVList& properties)
	{
		RTC_TRACE(("subscribeFromRef()"));
		CORBA::Long index;
		index = NVUtil::find_index(properties,
			"dataport.corba_cdr.inport_ref");
		if (index < 0)
		{
			RTC_ERROR(("inport_ref not found"));
			return false;
		}

		CORBA::Object_var obj;
#ifdef ORB_IS_ORBEXPRESS
		if (!(properties[index].value >>= obj))
#else
		if (!(properties[index].value >>= CORBA::Any::to_object(obj.out())))
#endif
		{
			RTC_ERROR(("prop[inport_ref] is not objref"));
			return true;
		}

		if (CORBA::is_nil(obj))
		{
			RTC_ERROR(("prop[inport_ref] is not objref"));
			return false;
		}

		if (!setObject(obj.in()))
		{
			RTC_ERROR(("Setting object to consumer failed."));
			return false;
		}
		return true;
	}
	bool InPortSHMConsumer::
		unsubscribeFromIor(const SDOPackage::NVList& properties)
	{
		RTC_TRACE(("unsubscribeFromIor()"));
		CORBA::Long index;
		index = NVUtil::find_index(properties,
			"dataport.corba_cdr.inport_ior");
		if (index < 0)
		{
			RTC_ERROR(("inport_ior not found"));
			return false;
		}

		const char* ior;
		if (!(properties[index].value >>= ior))
		{
			RTC_ERROR(("prop[inport_ior] is not string"));
			return false;
		}

		CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
		CORBA::Object_var var = orb->string_to_object(ior);
		if (!(_ptr()->_is_equivalent(var)))
		{
			RTC_ERROR(("connector property inconsistency"));
			return false;
		}

		releaseObject();
		return true;
	}
	bool InPortSHMConsumer::
		unsubscribeFromRef(const SDOPackage::NVList& properties)
	{
		RTC_TRACE(("unsubscribeFromRef()"));
		CORBA::Long index;
		index = NVUtil::find_index(properties,
			"dataport.corba_cdr.inport_ref");
		if (index < 0) { return false; }

		CORBA::Object_var obj;
#ifdef ORB_IS_ORBEXPRESS
		if (!(properties[index].value >>= obj))
#else
		if (!(properties[index].value >>= CORBA::Any::to_object(obj.out())))
#endif
		{
			return false;
		}

		if (!(_ptr()->_is_equivalent(obj.in()))) { return false; }

		releaseObject();
		return true;
	}
	InPortConsumer::ReturnCode
		InPortSHMConsumer::convertReturnCode(OpenRTM::PortStatus ret)
	{
		switch (ret)
		{
		case OpenRTM::PORT_OK:
			return InPortConsumer::PORT_OK;
			break;
		case OpenRTM::PORT_ERROR:
			return InPortConsumer::PORT_ERROR;
			break;
		case OpenRTM::BUFFER_FULL:
			return InPortConsumer::SEND_FULL;
			break;
		case OpenRTM::BUFFER_TIMEOUT:
			return InPortConsumer::SEND_TIMEOUT;
			break;
		case OpenRTM::UNKNOWN_ERROR:
			return InPortConsumer::UNKNOWN_ERROR;
			break;
		default:
			return InPortConsumer::UNKNOWN_ERROR;
			break;
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
	void InPortSHMConsumerInit(void)
  {
    RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
    factory.addFactory("shared_memory",
                       ::coil::Creator< ::RTC::InPortConsumer,
					   ::RTC::InPortSHMConsumer>,
                       ::coil::Destructor< ::RTC::InPortConsumer,
                                           ::RTC::InPortSHMConsumer>);
  }
};
