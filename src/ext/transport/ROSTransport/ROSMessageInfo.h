// -*- C++ -*-
/*!
 * @file  ROSMessageInfo.h
 * @brief ROS Massage Type Info class
 * @date  $Date: 2019-02-05 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_ROSMESSAGEINFO_H
#define RTC_ROSMESSAGEINFO_H


#include <coil/Properties.h>
#include <coil/Factory.h>
#include <ros/builtin_message_traits.h>



namespace RTC
{
  /*!
   * @if jp
   *
   * @class ROSMessageInfo
   *
   * @brief ROSのメッセージ型に関する情報を格納する基底クラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSMessageInfo
   *
   * @brief 
   *
   *
   * @endif
   */
  class ROSMessageInfoBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    virtual ~ROSMessageInfoBase(void) {}
    virtual std::string type();
    virtual std::string md5sum();
    virtual std::string message_definition();
  protected:
    std::string m_type;
    std::string m_md5sum;
    std::string m_message_definition;

  };

  template <class MessageType>
  class ROSMessageInfo : public ROSMessageInfoBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    ROSMessageInfo()
    {
      m_type = ros::message_traits::DataType<MessageType>::value();
      m_md5sum = ros::message_traits::MD5Sum<MessageType>::value();
      m_message_definition = ros::message_traits::Definition<MessageType>::value();
    }

  };

  /*!
   * @if jp
   *
   * @class ROSMessageInfoList
   *
   * @brief ROSのメッセージ型に関する情報のリストを名前とセットで保持するクラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class ROSMessageInfoList
   *
   * @brief 
   *
   *
   * @endif
   */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) ROSMessageInfoList
#else
    class __declspec(dllimport) ROSMessageInfoList
#endif
#else
    class ROSMessageInfoList
#endif
  {
    class ROSMessageInfoEntry;
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    ROSMessageInfoList();
    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    ~ROSMessageInfoList();
    /*!
     * @if jp
     *
     * @brief ROSMessageInfoを追加
     * 
     * @param id 名前
     * @param info ROSMessageInfo
     *
     * @else
     *
     * @brief Destructor
     *
     * @param id 
     * @param info 
     *
     * @endif
     */
    void addInfo(const std::string &id, ROSMessageInfoBase* info);
    /*!
     * @if jp
     *
     * @brief ROSMessageInfoを削除
     *
     * @param id 名前
     * @return 削除に成功した場合はtrue
     *
     * @else
     *
     * @brief 
     *
     * @param id 名前
     * @return
     *
     * @endif
     */
    bool removeInfo(const std::string& id);
    /*!
     * @if jp
     *
     * @brief 指定名のROSMessageInfoを取得
     *
     * @param id 名前
     * @return ROSMessageInfo
     *
     * @else
     *
     * @brief 
     *
     * @param id 
     * @return
     *
     * @endif
     */
    ROSMessageInfoBase* getInfo(const std::string& id);
  public:

    std::map<std::string, ROSMessageInfoEntry> m_data;

  private:
    /*!
     * @if jp
     *
     * @class ROSMessageInfoEntry
     *
     * @brief ROSMessageInfoと終了関数を登録するクラス
     * dllから登録したROSMessageInfoはdllでdeleteする必要があるため、
     * 必ず終了関数を登録する。
     *
     * @since 2.0.0
     *
     * @else
     *
     * @class ROSMessageInfoEntry
     *
     * @brief 
     *
     *
     * @endif
     */
    class ROSMessageInfoEntry
    {
    public:
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         * @param object ROSMessageInfo
         * @param destructor 終了関数
         *
         * @else
         *
         * @brief Constructor
         *
         * @param object 
         * @param destructor 
         *
         * @endif
         */
        ROSMessageInfoEntry(ROSMessageInfoBase* object, void(*destructor)(ROSMessageInfoBase*&));
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         * @else
         *
         * @brief Constructor
         *
         * @endif
         */
        ROSMessageInfoEntry();
        /*!
         * @if jp
         *
         * @brief コピーコンストラクタ
         *
         * @param obj コピー元のオブジェクト
         *
         * @else
         *
         * @brief Copy Constructor
         *
         * @param obj
         *
         * @endif
         */
        ROSMessageInfoEntry(const ROSMessageInfoEntry& obj);
        /*!
         * @if jp
         *
         * @brief 代入演算子
         *
         * @param obj コピー元のオブジェクト
         *
         * @else
         *
         * @brief 
         *
         * @param obj
         *
         * @endif
         */
        ROSMessageInfoEntry& operator = (const ROSMessageInfoEntry& obj);
        /*!
         * @if jp
         *
         * @brief 比較演算子
         *
         * @param obj 比較対象のオブジェクト
         *
         * @else
         *
         * @brief
         *
         * @param obj
         *
         * @endif
         */
        bool operator==(const ROSMessageInfoEntry& obj);
        /*!
         * @if jp
         *
         * @brief デストラクタ
         *
         * @else
         *
         * @brief Destructor
         *
         * @endif
         */
        ~ROSMessageInfoEntry();
        /*!
         * @if jp
         *
         * @brief 終了関数呼び出し
         *
         * @else
         *
         * @brief 
         *
         * @endif
         */
        void deleteObject();
        ROSMessageInfoBase* object_;
        void(*destructor_)(ROSMessageInfoBase*&);

    };
  };

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
  class __declspec(dllexport) GlobalROSMessageInfoList
#else
  class __declspec(dllimport) GlobalROSMessageInfoList
#endif
#else
  class GlobalROSMessageInfoList
#endif
      : public ROSMessageInfoList,
      public coil::Singleton<GlobalROSMessageInfoList >
  {
  public:
  private:
      friend class coil::Singleton<GlobalROSMessageInfoList>;
  };

}


#endif // RTC_ROSMESSAGEINFO_H

