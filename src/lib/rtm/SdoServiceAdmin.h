// -*- C++ -*-
/*!
 * @file SdoServiceAdmin.h
 * @brief SDO service administration class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: SdoConfiguration.cpp 1971 2010-06-03 08:46:40Z n-ando $
 *
 */

#ifndef RTC_SDOSERVICEADMIN_H
#define RTC_SDOSERVICEADMIN_H

#include <coil/Factory.h>

#include <rtm/idl/SDOPackageStub.h>
#include <rtm/SystemLogger.h>

#include <string>
#include <atomic>
#include <mutex>
#include <vector>

namespace RTC
{
  class RTObject_impl;
  class SdoServiceProviderBase;
  class SdoServiceConsumerBase;

  /*!
   * @if jp
   *
   * @class SDO service administration class
   * @brief SDO service 管理クラス
   *
   * このクラスは、SDO Service を管理するためのクラスである。SDO
   * Service は OMG SDO Specification において定義されている、SDOが特定
   * の機能のために提供また要求するサービスの一つである。詳細は仕様にお
   * いて定義されていないが、本クラスでは以下のように振る舞うサービスで
   * あるものとし、これらを管理するためのクラスが本クラスである。
   *
   * SDO Service においては、SDO/RTCに所有され、ある種のサービスを提供
   * するものを SDO Service Provider、他のSDO/RTCやアプリケーションが提
   * 供するサービスオブジェクトの参照を受け取り、それらの機能を利用する
   * ものを、SDO Service Consumer と呼ぶ。
   *
   * SDO Service Provider は他のアプリケーションから呼ばれ、SDO/RTC内部
   * の機能にアクセスするために用いられる。他のSDO/RTCまたはアプリケー
   * ションは、
   *
   * - SDO::get_service_profiles ()
   * - SDO::get_service_profile (in UniqueIdentifier id)
   * - SDO::get_sdo_service (in UniqueIdentifier id)
   *
   * のいずれかのオペレーションにより、ServiceProfile または SDO
   * Service の参照を取得し、機能を利用するためのオペレーションを呼び出
   * す。他のSDO/RTCまたはアプリケーション上での参照の破棄は任意のタイ
   * ミングで行われ、サービス提供側では、どこからどれだけ参照されている
   * かは知ることはできない。一方で、SDO/RTC側も、任意のタイミングでサー
   * ビスの提供を停止することもできるため、サービスの利用側では、常にい
   * サービスが利用できるとは限らないものとしてサービスオペレーションを
   * 呼び出す必要がある。
   *
   * 一方、SDO Service Consumer は当該SDO/RTC以外のSDO/RTCまたはアプリ
   * ケーションがサービスの実体を持ち、当該SDO/RTCにオブジェクト参照を
   * 含むプロファイルを与えることで、SDO/RTC側からサービスオペレーショ
   * ンが呼ばれ外部のSDO/RTCまたはアプリケーションが提供する機能を利用
   * できる。また、オブザーバ的なオブジェクトを与えることで、SDO/RTC側
   * からのコールバックを実現するためにも利用することができる。コンシュー
   * マは、プロバイダとは異なり、SDO Configurationインターフェースから
   * 追加、削除が行われる。関連するオペレーションは以下のとおりである。
   *
   * - Configuration::add_service_profile (in ServiceProfile sProfile)
   * - Configuration::remove_service_profile (in UniqueIdentifier id)
   *
   * 外部のSDO/RTCまたはアプリケーションは、自身が持つSDO Servcie
   * Provider の参照をIDおよびinterface type、プロパティとともに
   * ServcieProfile にセットしたうえで、add_service_profile() の引数と
   * して与えることで、当該SDO/RTCにサービスを与える。この際、IDはUUID
   * など一意なIDでなければならない。また、削除する際にはIDにより対象と
   * するServiceProfileを探索するため、サービス提供側では削除時までIDを
   * 保持しておかなければならない。
   *
   *
   * @since 1.1.0
   *
   * @else
   *
   * @class SDO service administration class
   * @brief SDO service administration class
   *
   * This class is the administration class for SDO Services. The SDO
   * Service, which is defined in the OMG SDO Specification, is a kind
   * of service for certain functionalities which is provided and/or
   * consumed by SDO. The specification does not define details.
   * However, in this implementation, the following behaviors of SDO
   * services are assumed and this class manages these SDO services.
   *
   * In this context, the SDO Services that are owned by SDO/RTC is
   * called SDO Service Provider, and the SDO Services that receive
   * references to provided services by other SDOs/RTCs or
   * applications is called SDO Serivce Consumer.
   *
   * SDO Service Provider is called from other applications, and it is
   * used to access to internal functionality of an SDO/RTC.  Other
   * SDO/RTC or applications would get a ServiceProfiles or a
   * reference to the SDO serivce through the following operations,
   * and then they call operations of the service.
   *
   * - SDO::get_service_profiles ()
   * - SDO::get_service_profile (in UniqueIdentifier id)
   * - SDO::get_sdo_service (in UniqueIdentifier id)
   *
   * Since references of services in other SDOs/RTCs or applications
   * could be released anytime, service providers cannot know where
   * and how many consumers refer them. On the other hand, since
   * SDO/RTC which provides services can stop and delete them anytime,
   * consumers have to call operations by assuming that the reference
   * cannot be accessed always.
   *
   * SDO Service Consumer, which is a reference to the service entity
   * in the other SDOs/RTCs or other applications, is given with
   * ServiceProfile, and SDO/RTC would call operation to access some
   * functionality to the service object. And giving certain observer
   * object, it will work as callback from SDO/RTC. SDO service
   * consumer, which is defferent from SDO service provider, is added
   * and deleted through SDO Configuration interface shown as follows.
   *
   * - Configuration::add_service_profile (in ServiceProfile sProfile)
   * - Configuration::remove_service_profile (in UniqueIdentifier id)
   *
   * To set a SDO service to the target SDO/RTC, other SDOs/RTCs and
   * applications have to give their service references with
   * ServiceProfile including ID, interface type and properties to the
   * add_service_profile() operation.  The ID has to be unique such as
   * UUID.  Since ID is used when the service is removed from the
   * target SDO/RTC, SDOs/RTCs and applications of service provider
   * side have to keep the ID until removing the service.
   *
   * @since 1.1.0
   *
   * @endif
   */
  class SdoServiceAdmin
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param
     *
     * @endif
     */
    explicit SdoServiceAdmin(::RTC::RTObject_impl& rtobj);

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * 仮想デストラクタ。
     *
     * @else
     *
     * @brief Virtual destractor
     *
     * Virtual destractor.
     *
     * @endif
     */
    virtual ~SdoServiceAdmin();

    /*!
     * @if jp
     * @brief リスナーの破棄前にサービスを一度だけ終了する
     * @else
     * @brief Finalize services once before listener holders are destroyed
     * @endif
     */
    void finalize();

    /*!
     * @if jp
     * @brief 初期化処理
     *
     * @else
     * @brief Initialization
     *
     * @endif
     */
    virtual void init(const coil::Properties& prop);

    /*!
     * @if jp
     *
     * @brief SDO Service Provider の ServiceProfileList を取得する
     *
     * @else
     *
     * @brief Get ServiceProfileList of SDO Service Provider
     *
     * @endif
     */
    SDOPackage::ServiceProfileList* getServiceProviderProfiles();

    /*!
     * @if jp
     *
     * @brief SDO Service Provider の ServiceProfile を取得する
     *
     * id で指定されたIFR IDを持つSDO Service Provider の
     * ServiceProfile を取得する。id が NULL ポインタの場合、指定された
     * id に該当するServiceProfile が存在しない場合、InvalidParameter
     * 例外が送出される。
     *
     * @param id SDO Service provider の IFR ID
     * @return 指定された id を持つ ServiceProfile
     *
     * @else
     *
     * @brief Get ServiceProfile of an SDO Service Provider
     *
     * This operation returnes ServiceProfile of an SDO Service
     * Provider which has the specified id. If the specified id is
     * NULL pointer or the specified id does not exist in the
     * ServiceProfile list, InvalidParameter exception will be thrown.
     *
     * @param id IFR ID of an SDO Service provider
     * @return ServiceProfile which has the specified id
     *
     * @endif
     */
    SDOPackage::ServiceProfile* getServiceProviderProfile(const char* id);

    /*!
     * @if jp
     *
     * @brief SDO Service Provider の Service を取得する
     *
     * id で指定されたIFR IDを持つSDO Service のオブジェクトリファレン
     * ス を取得する。id が NULL ポインタの場合、指定された id に該当す
     * るServiceProfile が存在しない場合、InvalidParameter 例外が送出さ
     * れる。
     *
     * @param id SDO Service provider の IFR ID
     * @return 指定された id を持つ SDO Service のオブジェクトリファレンス
     *
     * @else
     *
     * @brief Get ServiceProfile of an SDO Service
     *
     * This operation returnes an object reference of an SDO Service
     * Provider which has the specified id. If the specified id is
     * NULL pointer or the specified id does not exist in the
     * ServiceProfile list, InvalidParameter exception will be thrown.
     *
     * @param id IFR ID of an SDO Service provider
     * @return an SDO Service reference which has the specified id
     *
     * @endif
     */
    SDOPackage::SDOService_ptr getServiceProvider(const char* id);

    /*!
     * @if jp
     * @brief SDO service provider をセットする
     * @else
     * @brief Set a SDO service provider
     * @endif
     */
    bool addSdoServiceProvider(const SDOPackage::ServiceProfile& prof,
                               SdoServiceProviderBase* provider);

    /*!
     * @if jp
     * @brief SDO service provider を削除する
     * @else
     * @brief Remove a SDO service provider
     * @endif
     */
    bool removeSdoServiceProvider(const char* id);

    /*!
     * @if jp
     *
     * @brief Service Consumer を追加する
     *
     * @else
     *
     * @brief Add Service Consumer
     *
     * @endif
     */
    bool addSdoServiceConsumer(const SDOPackage::ServiceProfile& sProfile);

    /*!
     * @if jp
     *
     * @brief Service Consumer を削除する
     *
     * @else
     *
     * @brief Remove Service Consumer
     *
     * @endif
     */
    bool removeSdoServiceConsumer(const char* id);

protected:
    /*!
     * @if jp
     * @brief Provider 初期化処理
     *
     * @else
     * @brief Provider Initialization
     *
     * @endif
     */
    void initProvider(const coil::Properties& prop);
    /*!
     * @if jp
     * @brief Consumer 初期化処理
     *
     * @else
     * @brief Consumer Initialization
     *
     * @endif
     */
    void initConsumer(const coil::Properties& prop);


    /*!
     * @if jp
     *
     * @brief 許可されたサービス型かどうか調べる
     *
     * @else
     *
     * @brief If it is enabled service type
     *
     * @endif
     */
    bool isEnabledConsumerType(const SDOPackage::ServiceProfile& sProfile);

    /*!
     * @if jp
     *
     * @brief 存在するサービス型かどうか調べる
     *
     * @else
     *
     * @brief If it is existing service type
     *
     * @endif
     */
    bool isExistingConsumerType(const SDOPackage::ServiceProfile& sProfile);

    static std::string getUUID() ;

    static std::string ifrToKey(std::string& ifr);


  private:
    std::atomic<bool> m_finalized{false};
    std::mutex m_finalizeMutex;
    RTC::RTObject_impl& m_rtobj;
    coil::vstring m_consumerTypes;
    bool m_allConsumerEnabled;

    /*!
     * @if jp
     * @brief Lock 付き SDO ServiceProfileList
     * @else
     * @brief SDO ServiceProfileList with mutex lock
     * @endif
     */
    std::vector<SdoServiceProviderBase*> m_providers;
    std::mutex m_provider_mutex;

    /*!
     * @if jp
     * @brief Lock 付き SDO ServiceProfileList
     * @else
     * @brief SDO ServiceProfileList with mutex lock
     * @endif
     */
    std::vector<SdoServiceConsumerBase*> m_consumers;
    std::mutex m_consumer_mutex;

    /*!
     * @if jp
     * @brief logger
     * @else
     * @brief logger
     * @endif
     */
    ::RTC::Logger rtclog;
  };


} // namespace RTC

#endif  // RTC_SDOSERVICEADMIN_H
