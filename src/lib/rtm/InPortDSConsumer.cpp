// -*- C++ -*-
/*!
 * @file  InPortDSConsumer.h
 * @brief InPortDSConsumer class
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

#include <rtm/NVUtil.h>
#include <rtm/InPortDSConsumer.h>

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
  InPortDSConsumer::InPortDSConsumer()
    : rtclog("InPortDSConsumer")
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortDSConsumer::~InPortDSConsumer()
  {
    RTC_PARANOID(("~InPortDSConsumer()"));
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void InPortDSConsumer::init(coil::Properties& prop)
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
  InPortConsumer::ReturnCode InPortDSConsumer::
	  put(ByteData& data)
  {
    RTC_PARANOID(("put()"));

#ifndef ORB_IS_RTORB
    ::RTC::OctetSeq tmp;
    CORBA::ULong len = static_cast<CORBA::ULong>(data.getDataLength());
    tmp.length(len);
    data.readData(static_cast<unsigned char*>(tmp.get_buffer()), len);
#else // ORB_IS_RTORB
    OpenRTM_CdrData *cdrdata_tmp = new OpenRTM_CdrData();
    cdrdata_tmp->_buffer =
        (CORBA_octet *)RtORB_alloc(data.bufSize(), "InPortDSComsumer::push");
    memcpy(cdrdata_tmp->_buffer, data.bufPtr(), data.bufSize());
    cdrdata_tmp->_length = cdrdata_tmp->_maximum = data.bufSize();
    ::RTC::OctetSeq tmp(cdrdata_tmp);
#endif  // ORB_IS_RTORB
    try
      {
        // return code conversion
        // (IDL)OpenRTM::DataPort::ReturnCode_t -> DataPortStatus
        return convertReturnCode(_ptr()->push(tmp));
      }
    catch (...)
      {
        return CONNECTION_LOST;
      }
    return UNKNOWN_ERROR;
  }

  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void InPortDSConsumer::
  publishInterfaceProfile(SDOPackage::NVList&  /*properties*/)
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
  bool InPortDSConsumer::
  subscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));

    // getting InPort's ref from IOR string
    if (subscribeFromIor(properties)) { return true; }

    // getting InPort's ref from Object reference
    if (subscribeFromRef(properties)) { return true; }

    return false;
  }

  /*!
   * @if jp
   * @brief データ送信通知からの登録解除
   * @else
   * @brief Unsubscribe the data send notification
   * @endif
   */
  void InPortDSConsumer::
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
  bool InPortDSConsumer::
  subscribeFromIor(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeFromIor()"));

    CORBA::Long index;
    index = NVUtil::find_index(properties,
                               "dataport.data_service.inport_ior");
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

  /*!
   * @if jp
   * @brief Anyから直接オブジェクト参照を取得する
   * @else
   * @brief Getting object reference fromn Any directry
   * @endif
   */
  bool InPortDSConsumer::
  subscribeFromRef(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeFromRef()"));
    CORBA::Long index;
    index = NVUtil::find_index(properties,
                               "dataport.data_service.inport_ref");
    if (index < 0)
      {
        RTC_ERROR(("inport_ref not found"));
        return false;
      }

    CORBA::Object_var obj = CORBA::Object::_nil();
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
  bool InPortDSConsumer::
  unsubscribeFromIor(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeFromIor()"));
    CORBA::Long index;
    index = NVUtil::find_index(properties,
                               "dataport.data_service.inport_ior");
    if (index < 0)
      {
        RTC_ERROR(("inport_ior not found"));
        return false;
      }

    const char* ior = nullptr;
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
  bool InPortDSConsumer::
  unsubscribeFromRef(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeFromRef()"));
    CORBA::Long index;
    index = NVUtil::find_index(properties,
                               "dataport.data_service.inport_ref");
    if (index < 0) { return false; }

    CORBA::Object_var obj = CORBA::Object::_nil();
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

  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  InPortConsumer::ReturnCode
  InPortDSConsumer::convertReturnCode(RTC::PortStatus ret)
  {
    switch (ret)
      {
      case RTC::PORT_OK:
        return InPortConsumer::PORT_OK;
        break;
      case RTC::PORT_ERROR:
        return InPortConsumer::PORT_ERROR;
        break;
      case RTC::BUFFER_FULL:
        return InPortConsumer::SEND_FULL;
        break;
      case RTC::BUFFER_TIMEOUT:
        return InPortConsumer::SEND_TIMEOUT;
        break;
      case RTC::UNKNOWN_ERROR:
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
  void InPortDSConsumerInit(void)
  {
    RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
    factory.addFactory("data_service",
                       ::coil::Creator< ::RTC::InPortConsumer,
                                        ::RTC::InPortDSConsumer>,
                       ::coil::Destructor< ::RTC::InPortConsumer,
                                           ::RTC::InPortDSConsumer>);
  }
};
