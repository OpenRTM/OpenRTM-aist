// -*- C++ -*-
/*!
 * @file  InPortCorbaCdrUDPConsumer.cpp
 * @brief InPortCorbaCdrUDPConsumer class
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

#include <rtm/NVUtil.h>
#include <rtm/InPortCorbaCdrUDPConsumer.h>
#include <rtm/CORBA_IORUtil.h>

#ifdef ORB_IS_TAO
#include <tao/Endpoint.h>
#include <tao/Profile.h>
#include <tao/Stub.h>
#endif


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
  InPortCorbaCdrUDPConsumer::InPortCorbaCdrUDPConsumer(void)
    : rtclog("InPortCorbaCdrUDPConsumer")
  {
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortCorbaCdrUDPConsumer::~InPortCorbaCdrUDPConsumer(void)
  {
    RTC_PARANOID(("~InPortCorbaCdrUDPConsumer()"));
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void InPortCorbaCdrUDPConsumer::init(coil::Properties& prop)
  {
    m_properties = prop;
  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus InPortCorbaCdrUDPConsumer::put(ByteData& data)
  {
    RTC_PARANOID(("put()"));

#ifndef ORB_IS_RTORB
    CORBA::ULong len = (CORBA::ULong)data.getDataLength();
    m_data.length(len);
    data.readData((unsigned char*)m_data.get_buffer(), len);
#else // ORB_IS_RTORB
    OpenRTM_CdrData *cdrdata_tmp = new OpenRTM_CdrData();
    cdrdata_tmp->_buffer =
        (CORBA_octet *)RtORB_alloc(data.bufSize(), "InPortCorbaCdrComsumer::put");
    memcpy(cdrdata_tmp->_buffer, data.bufPtr(), data.bufSize());
    cdrdata_tmp->_length = cdrdata_tmp->_maximum = data.bufSize();
    ::OpenRTM::CdrData tmp(cdrdata_tmp);
#endif  // ORB_IS_RTORB
    try
      {
        // return code conversion
        // (IDL)OpenRTM::DataPort::ReturnCode_t -> DataPortStatus
        _ptr()->put(m_data);
        return DataPortStatus::PORT_OK;
      }
    catch (...)
      {
        return DataPortStatus::CONNECTION_LOST;
      }
  }
  
  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void InPortCorbaCdrUDPConsumer::
  publishInterfaceProfile(SDOPackage::NVList& /*properties*/)
  {
    return;
  }

  /*!
   * @if jp
   * @brief データ送信通知への登録
   * @else
   * @brief Subscribe to the data sending notification
   * @endif
   */
  bool InPortCorbaCdrUDPConsumer::
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
  
  /*!
   * @if jp
   * @brief データ送信通知からの登録解除
   * @else
   * @brief Unsubscribe the data send notification
   * @endif
   */
  void InPortCorbaCdrUDPConsumer::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    
    if (unsubscribeFromIor(properties)) { return; }
    unsubscribeFromRef(properties);
  }
  
  //----------------------------------------------------------------------
  // private functions

  /*!
   * @if jp
   * @brief IOR文字列からオブジェクト参照を取得する
   * @else
   * @brief Getting object reference fromn IOR string
   * @endif
   */
  bool InPortCorbaCdrUDPConsumer::
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

#ifdef ORB_IS_TAO
    TAO_Stub *stub = obj->_stubobj();


    TAO_MProfile profiles = stub->base_profiles();
    
    while (profiles.profile_count() > 1)
      {
        if (profiles.get_profile(0)->tag() != TAO_TAG_DIOP_PROFILE)
          {
            profiles.remove_profile(profiles.get_profile(0));
          }
        else
          {
            break;
          }
    }
    
    stub->base_profiles(profiles);
#endif
    
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
  
  /*!
   * @if jp
   * @brief Anyから直接オブジェクト参照を取得する
   * @else
   * @brief Getting object reference fromn Any directry
   * @endif
   */
  bool InPortCorbaCdrUDPConsumer::
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
  
  /*!
   * @if jp
   * @brief 接続解除(IOR版)
   * @else
   * @brief ubsubscribing (IOR version)
   * @endif
   */
  bool InPortCorbaCdrUDPConsumer::
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
  
  /*!
   * @if jp
   * @brief 接続解除(Object reference版)
   * @else
   * @brief ubsubscribing (Object reference version)
   * @endif
   */
  bool InPortCorbaCdrUDPConsumer::
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
  void InPortCorbaCdrUDPConsumerInit(void)
  {
    RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
    factory.addFactory("corba_cdr_udp",
                       ::coil::Creator< ::RTC::InPortConsumer,
                                        ::RTC::InPortCorbaCdrUDPConsumer>,
                       ::coil::Destructor< ::RTC::InPortConsumer,
                                           ::RTC::InPortCorbaCdrUDPConsumer>);
  }
}
