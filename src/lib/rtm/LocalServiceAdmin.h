// -*- C++ -*-
/*!
 * @file LocalServiceAdmin.h
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

#ifndef RTC_LOCALSERVICEADMIN_H
#define RTC_LOCALSERVICEADMIN_H

#include <coil/Mutex.h>
#include <coil/Factory.h>
#include <coil/Singleton.h>

#include <rtm/idl/SDOPackageStub.h>
#include <rtm/SystemLogger.h>
#include <rtm/LocalServiceBase.h>

#include <string>
#include <vector>

namespace RTM
{
  // forward decl
  class LocalServiceBase;
  typedef LocalServiceBase* (*ECNewFunc)();
  typedef void (*ECDeleteFunc)(LocalServiceBase* ls);
  typedef coil::Guard<coil::Mutex> Guard;

  /*!
   * @if jp
   *
   * @class LocalService administration class
   * @brief LocalService 管理クラス
   *
   * TODO: Documentation
   *
   * @since 1.1.0
   *
   *
   * @else
   *
   * @class SDO service administration class
   * @brief SDO service administration class
   *
   * TODO: Documentation
   *
   * @since 1.1.0
   *
   * @endif
   */
  class LocalServiceAdmin
    : public coil::Singleton<LocalServiceAdmin>
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    LocalServiceAdmin();

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
    virtual ~LocalServiceAdmin();

    /*!
     * @if jp
     *
     * @brief LocaServiceAdminの初期化
     *
     * TODO: Documentation
     *
     * @else
     *
     * @brief Initialization of LocalServiceAdmin
     *
     * TODO: Documentation
     *
     * @endif
     */
    void init(coil::Properties& props);

    /*!
     * @if jp
     *
     * @brief LocalserviceAdmin の終了処理
     *
     * TODO: Documentation
     *
     * @else
     *
     * @brief Finalization ofLocalServiceAdmin
     *
     * TODO: Documentation
     *
     * @endif
     */
    void finalize();

    /*!
     * @if jp
     *
     * @brief LocalServiceProfileListの取得
     *
     * TODO: Documentation
     *
     * @else
     *
     * @brief Getting LocalServiceProfileList
     *
     * TODO: Documentation
     *
     * @endif
     */
    ::RTM::LocalServiceProfileList getServiceProfiles();

    /*!
     * @if jp
     *
     * @brief LocalServiceProfile を取得する
     *
     * id で指定されたIDを持つLocalService の
     * LocalServiceProfile を取得する。id が NULL ポインタの場合、指定された
     * id に該当するServiceProfile が存在しない場合、falseを返す。
     *
     * @param id LocalService の IFR ID
     * @return 指定された id を持つ LocalServiceProfile
     *
     * @else
     *
     * @brief Get LocalServiceProfile of an LocalService
     *
     * This operation returns LocalServiceProfile of a LocalService
     * which has the specified id. If the specified id is
     * NULL pointer or the specified id does not exist in the
     * ServiceProfile list, false will be returned.
     *
     * @param id ID of an LocalService
     * @return LocalServiceProfile which has the specified id
     *
     * @endif
     */
    bool getServiceProfile(const std::string& name,
                           ::RTM::LocalServiceProfile& prof);

    /*!
     * @if jp
     *
     * @brief LocalService の Service を取得する
     *
     * id で指定されたIDを持つLocalService のポインタを取得する。id が
     * NULL ポインタの場合、指定された id に該当するServiceProfile が存
     * 在しない場合、NULLを返す。
     *
     * @param id LocalService の ID
     * @return 指定された id を持つ LocalService のポインタ
     *
     * @else
     *
     * @brief Get a pointer of a LocalService
     *
     * This operation returnes a pointer to the LocalService
     * which has the specified id. If the specified id is
     * NULL pointer or the specified id does not exist in the
     * ServiceProfile list, NULL pointer will be returned.
     *
     * @param id ID of a LocalService
     * @return a pointer which has the specified id
     *
     * @endif
     */
    ::RTM::LocalServiceBase* getService(const char* id);

    /*!
     * @if jp
     * @brief SDO service provider をセットする
     *
     * TODO: Documentation
     *
     * @else
     * @brief Set a SDO service provider
     *
     * TODO: Documentation
     *
     * @endif
     */
    bool addLocalService(::RTM::LocalServiceBase* service);

    /*!
     * @if jp
     * @brief LocalService を削除する
     *
     * TODO: Documentation
     *
     * @else
     * @brief Remove a LocalService
     *
     * TODO: Documentation
     *
     * @endif
     */
    bool removeLocalService(const std::string& name);

  private:
    /*!
     * @if jp
     * @brief 指定されたIDが有効かどうかチェックする
     * @else
     * @brief Check if specified ID is enabled
     * @endif
     */
    bool isEnabled(const std::string& id, const coil::vstring& enabled);

    /*!
     * @if jp
     * @brief 指定されたIDがすでに存在するかどうかチェックする
     * @else
     * @brief Check if specified ID is existing
     * @endif
     */
    bool notExisting(const std::string& id);

    /*!
     * @if jp
     * @brief Lock 付き SDO ServiceProfileList
     * @else
     * @brief SDO ServiceProfileList with mutex lock
     * @endif
     */
    std::vector<LocalServiceBase*> m_services;
    coil::Mutex m_services_mutex;

    /*!
     * @if jp
     * @brief logger
     * @else
     * @brief logger
     * @endif
     */
    ::RTC::Logger rtclog;
  };
};  // namespace RTM

#endif  // RTC_LOCALSERVICEADMIN_H
