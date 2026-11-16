// -*- C++ -*-
/*!
 * @file  FastDDSMessageInfo.h
 * @brief Fast-RTPS Massage Type Info class
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

#ifndef RTC_FASTRTPSMESSAGEINFO_H
#define RTC_FASTRTPSMESSAGEINFO_H


#include <coil/Properties.h>
#include <coil/Factory.h>


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace RTC
{
  /*!
   * @if jp
   *
   * @class FastDDSMessageInfoBase
   *
   * @brief FastDDSのメッセージ型に関する情報を格納する基底クラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class FastDDSMessageInfoBase
   *
   * @brief 
   *
   *
   * @endif
   */
  
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) FastDDSMessageInfoBase
#else
    class __declspec(dllimport) FastDDSMessageInfoBase
#endif
#else
    class FastDDSMessageInfoBase
#endif
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
    virtual ~FastDDSMessageInfoBase(void);
    /*!
     * @if jp
     *
     * @brief トピック名を装飾する
     *
     * @param topic 装飾前のトピック名
     * @return 装飾後のトピック名
     *
     * @else
     *
     * @brief 
     *
     * @param topic 
     * @return 
     *
     * @endif
     */
    virtual std::string topic_name(std::string topic) = 0;
    /*!
     * @if jp
     *
     * @brief データの型名取得
     *
     * @return 型名
     *
     * @else
     *
     * @brief
     *
     * @return
     *
     * @endif
     */
    virtual std::string data_type() = 0;
  };

  /*!
   * @if jp
   *
   * @class FastDDSMessageInfoList
   *
   * @brief FastDDSのメッセージ型に関する情報のリストを名前とセットで保持するクラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class FastDDSMessageInfoList
   *
   * @brief 
   *
   *
   * @endif
   */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) FastDDSMessageInfoList
#else
    class __declspec(dllimport) FastDDSMessageInfoList
#endif
#else
    class FastDDSMessageInfoList
#endif
  {
    class FastDDSMessageInfoEntry;
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
    FastDDSMessageInfoList();
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
    ~FastDDSMessageInfoList();
    /*!
     * @if jp
     *
     * @brief FastDDSMessageInfoを追加
     * 
     * @param id 名前
     * @param info FastDDSMessageInfo
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
    void addInfo(const std::string &id, FastDDSMessageInfoBase* info);
    /*!
     * @if jp
     *
     * @brief FastDDSMessageInfoを削除
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
     * @brief 指定名のFastDDSMessageInfoを取得
     *
     * @param id 名前
     * @return FastDDSMessageInfo
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
    FastDDSMessageInfoBase* getInfo(const std::string& id);
  public:

    std::map<std::string, FastDDSMessageInfoEntry> m_data;

  private:
    /*!
     * @if jp
     *
     * @class FastDDSMessageInfoEntry
     *
     * @brief FastDDSMessageInfoと終了関数を登録するクラス
     * dllから登録したFastDDSMessageInfoはdllでdeleteする必要があるため、
     * 必ず終了関数を登録する。
     *
     * @since 2.0.0
     *
     * @else
     *
     * @class FastDDSMessageInfoEntry
     *
     * @brief 
     *
     *
     * @endif
     */
    class FastDDSMessageInfoEntry
    {
    public:
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         * @param object FastDDSMessageInfo
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
        FastDDSMessageInfoEntry(FastDDSMessageInfoBase* object, void(*destructor)(FastDDSMessageInfoBase*&));
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
        FastDDSMessageInfoEntry();
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
        FastDDSMessageInfoEntry(const FastDDSMessageInfoEntry& obj);
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
        FastDDSMessageInfoEntry& operator = (const FastDDSMessageInfoEntry& obj);
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
        bool operator==(const FastDDSMessageInfoEntry& obj);
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
        ~FastDDSMessageInfoEntry();
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
        FastDDSMessageInfoBase* object_;
        void(*destructor_)(FastDDSMessageInfoBase*&);

    };
  };

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
  class __declspec(dllexport) GlobalFastDDSMessageInfoList
#else
  class __declspec(dllimport) GlobalFastDDSMessageInfoList
#endif
#else
  class GlobalFastDDSMessageInfoList
#endif
      : public FastDDSMessageInfoList,
      public coil::Singleton<GlobalFastDDSMessageInfoList >
  {
  public:
  private:
      friend class coil::Singleton<GlobalFastDDSMessageInfoList>;
  };

}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( pop )
#endif


#endif // RTC_FASTRTPSMESSAGEINFO_H

