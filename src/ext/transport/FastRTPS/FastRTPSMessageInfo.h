// -*- C++ -*-
/*!
 * @file  FastRTPSMessageInfo.h
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
   * @class FastRTPSMessageInfoBase
   *
   * @brief FastRTPSのメッセージ型に関する情報を格納する基底クラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class FastRTPSMessageInfoBase
   *
   * @brief 
   *
   *
   * @endif
   */
  
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) FastRTPSMessageInfoBase
#else
    class __declspec(dllimport) FastRTPSMessageInfoBase
#endif
#else
    class FastRTPSMessageInfoBase
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
    virtual ~FastRTPSMessageInfoBase(void);
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
   * @class FastRTPSMessageInfoList
   *
   * @brief FastRTPSのメッセージ型に関する情報のリストを名前とセットで保持するクラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class FastRTPSMessageInfoList
   *
   * @brief 
   *
   *
   * @endif
   */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) FastRTPSMessageInfoList
#else
    class __declspec(dllimport) FastRTPSMessageInfoList
#endif
#else
    class FastRTPSMessageInfoList
#endif
  {
    class FastRTPSMessageInfoEntry;
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
    FastRTPSMessageInfoList();
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
    ~FastRTPSMessageInfoList();
    /*!
     * @if jp
     *
     * @brief FastRTPSMessageInfoを追加
     * 
     * @param id 名前
     * @param info FastRTPSMessageInfo
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
    void addInfo(const std::string &id, FastRTPSMessageInfoBase* info);
    /*!
     * @if jp
     *
     * @brief FastRTPSMessageInfoを削除
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
     * @brief 指定名のFastRTPSMessageInfoを取得
     *
     * @param id 名前
     * @return FastRTPSMessageInfo
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
    FastRTPSMessageInfoBase* getInfo(const std::string& id);
  public:

    std::map<std::string, FastRTPSMessageInfoEntry> m_data;

  private:
    /*!
     * @if jp
     *
     * @class FastRTPSMessageInfoEntry
     *
     * @brief FastRTPSMessageInfoと終了関数を登録するクラス
     * dllから登録したFastRTPSMessageInfoはdllでdeleteする必要があるため、
     * 必ず終了関数を登録する。
     *
     * @since 2.0.0
     *
     * @else
     *
     * @class FastRTPSMessageInfoEntry
     *
     * @brief 
     *
     *
     * @endif
     */
    class FastRTPSMessageInfoEntry
    {
    public:
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         * @param object FastRTPSMessageInfo
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
        FastRTPSMessageInfoEntry(FastRTPSMessageInfoBase* object, void(*destructor)(FastRTPSMessageInfoBase*&));
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
        FastRTPSMessageInfoEntry();
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
        FastRTPSMessageInfoEntry(const FastRTPSMessageInfoEntry& obj);
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
        FastRTPSMessageInfoEntry& operator = (const FastRTPSMessageInfoEntry& obj);
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
        bool operator==(const FastRTPSMessageInfoEntry& obj);
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
        ~FastRTPSMessageInfoEntry();
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
        FastRTPSMessageInfoBase* object_;
        void(*destructor_)(FastRTPSMessageInfoBase*&);

    };
  };

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
  class __declspec(dllexport) GlobalFastRTPSMessageInfoList
#else
  class __declspec(dllimport) GlobalFastRTPSMessageInfoList
#endif
#else
  class GlobalFastRTPSMessageInfoList
#endif
      : public FastRTPSMessageInfoList,
      public coil::Singleton<GlobalFastRTPSMessageInfoList >
  {
  public:
  private:
      friend class coil::Singleton<GlobalFastRTPSMessageInfoList>;
  };

}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( pop )
#endif


#endif // RTC_FASTRTPSMESSAGEINFO_H

