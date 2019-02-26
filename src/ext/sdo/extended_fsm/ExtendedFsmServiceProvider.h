// -*- C++ -*-
/*!
 * @file ExtendedFsmServiceProvider.h
 * @brief Component observer SDO service consumer implementation
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */


#ifndef RTC_EXTENDEDFSMSERVICEPROVIDER_H
#define RTC_EXTENDEDFSMSERVICEPROVIDER_H

#include <coil/Mutex.h>
#include <coil/Factory.h>
#include <coil/stringutil.h>
#include <rtm/SdoServiceProviderBase.h>
#include <rtm/ComponentActionListener.h>
#include <rtm/idl/SDOPackageStub.h>
#include <rtm/idl/ExtendedFsmServiceSkel.h>

namespace RTC
{

  /*!
   * @if jp
   * @else
   * @endif
   */
  class ExtendedFsmServiceProvider
    : public virtual POA_RTC::ExtendedFsmService,
      public SdoServiceProviderBase
  {
  public:
    /*!
     * @if jp
     * @brief ctor of ExtendedFsmServiceProvider
     * @else
     * @brief ctor of ExtendedFsmServiceProvider
     * @endif
     */
    ExtendedFsmServiceProvider();

    /*!
     * @if jp
     * @brief dtor
     * @else
     * @brief dtor
     * @endif
     */
    virtual ~ExtendedFsmServiceProvider();

    /*!
     * @if jp
     * @brief 初期化
     * @else
     * @brief Initialization
     * @endif
     */
    virtual bool init(RTObject_impl& rtobj,
                      const SDOPackage::ServiceProfile& profile);

    /*!
     * @if jp
     * @brief 再初期化
     * @else
     * @brief Re-initialization
     * @endif
     */
    virtual bool reinit(const SDOPackage::ServiceProfile& profile);

    /*!
     * @if jp
     * @brief ServiceProfile を取得する
     * @else
     * @brief getting ServiceProfile
     * @endif
     */
    virtual const SDOPackage::ServiceProfile& getProfile() const;
    
    /*!
     * @if jp
     * @brief 終了処理
     * @else
     * @brief Finalization
     * @endif
     */
    virtual void finalize();

    //============================================================
    // CORBA operations
    //
    // string get_current_state();
    // ReturnCode_t set_fsm_structure(in FsmStructure fsm_structure);
    // ReturnCode_t get_fsm_structure(out FsmStructure fsm_structure);
    //============================================================

    /*!
     * @if jp
     * @brief FSMの現在の状態を取得
     *
     * このオペレーションはFSMコンポーネントのFSMの現在の状態を返す。
     * (FSM4RTC spec. p.20)
     *
     * @return 現在の状態を表す文字列
     *
     * @else
     * @brief Get Current FSM State
     *
     * This operation returns the current state of an FSM in the
     * target FSM component. (FSM4RTC spec. p.20)
     *
     * @return A string which represent the current status
     *
     * @endif
     */
    virtual char* get_current_state();
    /*!
     * @if jp
     * @brief FSMの構造を設定する
     *
     * このオペレーションは対象のコンポーネントに対して、FSMの構造を保
     * 持する FsmStruccture を設定する。対象コンポーネントは
     * fsm_structure に与えられた値を基に状態遷移ルール等のFSM構造を再
     * 設定する。このオペレーションが未実装の場合は、UNSUPPORTED を返す。
     *
     * @param fsm_structure FSMの構造を表すFsmStructure構造体。
     * @return RTC_OK 正常終了
     *         RTC_ERROR その他のエラー
     *         BAD_PARAMETER 不正なパラメータ
     *         UNSUPPORTED 未サポート
     *
     * @else
     * @brief Set FSM Structure
     *
     * This operation sets an FsmStructure to the target
     * component. Then the target component reconfigures its FSM
     * structure such as transition rules according to the values of
     * the given fsm_structure. RTCs may return UNSUPPORTED if this
     * operation is not implemented.
     *
     * @param fsm_structure FsmStructure structure which represents
     *        FSM structure
     * @return RTC_OK normal return
     *         RTC_ERROR other error
     *         BAD_PARAMETER invalid parameter
     *         UNSUPPORTED unsupported or not implemented
     *
     * @endif
     */
    virtual ReturnCode_t
    set_fsm_structure(const ::RTC::FsmStructure& fsm_structure);
    /*!
     * @if jp
     * @brief FSMの構造を取得する
     *
     * このオペレーションは対象のコンポーネントに対して、現在保持してい
     * るFSMの構造を取得する。ExtendedFsmService 構造体はフィールド
     * name (FSMの名称), structure (FSMの構造) 、EventProfile などを返
     * す。structure のフォーマットは、フィールド properties 内に格納さ
     * れたキー "fsm_structure.format" に指定される。このオペレーション
     * が未実装の場合は、UNSUPPORTED を返す。
     *
     * ref: SCXML https://www.w3.org/TR/scxml/
     *
     * @param fsm_structure FSMの構造を表すFsmStructure構造体。
     * @return RTC_OK 正常終了
     *         RTC_ERROR その他のエラー
     *         BAD_PARAMETER 不正なパラメータ
     *         UNSUPPORTED 未サポート
     *
     * @else
     * @brief Set FSM Structure
     *
     * This operation returns the structure of an FSM in the target
     * FSM component. ExtendedFsmService returns the name, structure
     * with format specified by fsm_structure.format and
     * EventProfiles. RTCs may return UNSUPPORTED if this operation is
     * not implemented.
     *
     * @param fsm_structure FsmStructure structure which represents
     *        FSM structure
     * @return RTC_OK normal return
     *         RTC_ERROR other error
     *         BAD_PARAMETER invalid parameter
     *         UNSUPPORTED unsupported or not implemented
     *
     * @endif
     */
    virtual ReturnCode_t
    get_fsm_structure(::RTC::FsmStructure_out fsm_structure);

    

  protected:
    /*!
     * @if jp
     * @brief RTObjectへのリスナ接続処理
     * @else
     * @brief Connectiong listeners to RTObject
     * @endif
     */
    void setListeners(coil::Properties& prop);

    /*!
     * @if jp
     * @brief FSM状態遷移
     * @else
     * @brief FSM status change
     * @endif
     */
    void changeStatus(std::string state);

    /*!
     * @if jp
     * @brief ハートビートを解除する
     * @else
     * @brief Unsetting heartbeat
     * @endif
     */
    void changeStructure(const std::string& fsm_structure);

    //============================================================
    // FSM status related functions
    /*!
     * @if jp
     * @brief FSM状態変化リスナの設定処理
     * @else
     * @brief Setting FSM status listeners
     * @endif
     */
    void setFSMStatusListeners();

    /*!
     * @if jp
     * @brief FSM状態変化リスナの解除処理
     * @else
     * @brief Unsetting FSM status listeners
     * @endif
     */
    void unsetFSMStatusListeners();

    //============================================================
    // FsmProfile related functions
    /*!
     * @if jp
     * @brief FsmProfile状態変化リスナの設定
     * @else
     * @brief Setting FsmProfile listener
     * @endif
     */
    void setFSMProfileListeners();

    /*!
     * @if jp
     * @brief FsmProfile状態変化リスナの解除
     * @else
     * @brief Unsetting FsmProfile listener
     * @endif
     */
    void unsetFSMProfileListeners();

    //============================================================
    // FsmStructure related functions
    /*!
     * @if jp
     * @brief FsmStructure状態変化リスナの設定
     * @else
     * @brief Setting FsmStructure listener
     * @endif
     */
    void setFSMStructureListeners();

    /*!
     * @if jp
     * @brief FsmStructure状態変化リスナの解除
     * @else
     * @brief Unsetting FsmStructure listener
     * @endif
     */
    void unsetFSMStructureListeners();

  private:
    RTC::RTObject_impl* m_rtobj;
    SDOPackage::ServiceProfile m_profile;
    coil::Properties m_properties;
    FsmStructure m_fsmStructure;
    std::string m_fsmState;

  };

}; // namespace RTC

extern "C"
{
  DLL_EXPORT void ExtendedFsmServiceProviderInit();
};

#endif // RTC_EXTENDEDFSMSERVICEPROVIDER_H


