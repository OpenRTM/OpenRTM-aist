// -*- C++ -*-
/*!
* @file NodeNumberingPolicy.h
* @brief Object numbering policy class
* @date $Date: 2018-1-11 03:08:04 $
* @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
*
*  Copyright (C) 2018
*     Intelligent Systems Research Institute,
*     National Institute of
*         Advanced Industrial Science and Technology (AIST), Japan
*     All rights reserved.
*
*
*/

#ifndef RTC_NODENUMBERINGPOLICY_H
#define RTC_NODENUMBERINGPOLICY_H

#include <string>
#include <vector>
#include <rtm/RTC.h>
#include <rtm/NumberingPolicyBase.h>
#include <rtm/Manager.h>


namespace RTM
{
  /*!
   * @if jp
   *
   * @class NodeNumberingPolicy
   * @brief オブジェクト生成時ネーミング・ポリシー(命名規則)管理用クラス
   *
   * マスターマネージャ、スレーブマネージャからRTCを検索してナンバリングを行う
   *
   * @since 1.2.0
   *
   * @else
   *
   * @class NodeNumberingPolicy
   * @brief Class for naming policy management when creating objects
   *
   * 
   *
   * @since 1.2.0
   *
   * @endif
   */
  class NodeNumberingPolicy
    : public NumberingPolicyBase
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
    NodeNumberingPolicy();
    
    /*!
     * @if jp
     *
     * @brief デストラクタ
     * 
     * @else
     *
     * @brief Destractor
     *
     * @endif
     */
    ~NodeNumberingPolicy() override = default;
    
    /*!
     * @if jp
     *
     * @brief オブジェクト生成時の名称作成
     *
     * オブジェクト生成時の名称を生成する。
     * 生成済みインスタンスの数に応じた名称を生成する。
     * 
     * @param obj 名称生成対象オブジェクト
     *
     * @return 生成したオブジェクト名称
     *
     * @else
     *
     * @brief Create the name when creating object
     *
     * Create the name when creating object.
     * Create the name corresponding to the number of generated instances.
     * 
     * @param obj The target object for the name creation
     *
     * @return Names of the created object
     *
     * @endif
     */
    std::string onCreate(void* obj) override;
    
    /*!
     * @if jp
     *
     * @brief オブジェクト削除時の名称解放
     *
     * オブジェクト削除時に名称を解放する。
     * オブジェクト削除時に生成済みインスタンス数を減算する。
     * 
     * @param obj 名称解放対象オブジェクト
     *
     * @else
     *
     * @brief Delete the name when deleting object
     *
     * Delete the name when deleting object.
     * Substract the generated number of instances when deleting the object.
     * 
     * @param obj The target object for the name delete
     *
     * @endif
     */
    void onDelete(void* obj) override;
    
  protected:
    /*!
     * @if jp
     *
     * @brief オブジェクトの検索
     *
     * マスターマネージャ、およびスレーブマネージャに登録されたRTCを検索し、
     * 　　　　名前が一致するRTCが存在する場合はTrueを返す
     * このプロセスで起動したマネージャがマスターマネージャではなく、
     *  さらにマスターマネージャが1つも登録されていない場合はこのプロセスのマネージャから検索
     *
     * @param name 検索対象オブジェクトの名前
     *
     * @return 判定
     *
     * @else
     *
     * @brief
     *
     *
     * @param name
     *
     * @return
     *
     * @endif
     */
    virtual bool find(std::string name);
    
  private:
    std::vector<void*> m_objects;
    RTC::Manager *m_mgr;
  };
} // namespace RTM

extern "C"
{
    void NodeNumberingPolicyInit();
}

#endif // RTC_NODENUMBERINGPOLICY_H
