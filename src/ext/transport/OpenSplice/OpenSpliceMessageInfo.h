// -*- C++ -*-
/*!
 * @file  OpenSpliceMessageInfo.h
 * @brief OpenSplice Massage Type Info class
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

#ifndef RTC_OPENSPLICEMESSAGEINFO_H
#define RTC_OPENSPLICEMESSAGEINFO_H


#include <coil/Properties.h>
#include <coil/Singleton.h>


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif



namespace RTC
{
  /*!
   * @if jp
   *
   * @class OpenSpliceMessageInfoBase
   *
   * @brief OpenSpliceのメッセージ型に関する情報を格納する基底クラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class OpenSpliceMessageInfoBase
   *
   * @brief 
   *
   *
   * @endif
   */
  
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) OpenSpliceMessageInfoBase
#else
    class __declspec(dllimport) OpenSpliceMessageInfoBase
#endif
#else
    class OpenSpliceMessageInfoBase
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
    virtual ~OpenSpliceMessageInfoBase(void);
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
    /*!
     * @if jp
     *
     * @brief IDLファイルパスの取得
     *
     * @return IDLファイルのパス
     *
     * @else
     *
     * @brief
     *
     * @return
     *
     * @endif
     */
    virtual std::string idl_path() = 0;
  };

  
  /*!
   * @if jp
   *
   * @class OpenSpliceMessageInfoList
   *
   * @brief OpenSpliceのメッセージ型に関する情報のリストを名前とセットで保持するクラス
   *
   *
   * @since 2.0.0
   *
   * @else
   *
   * @class OpenSpliceMessageInfoList
   *
   * @brief 
   *
   *
   * @endif
   */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
    class __declspec(dllexport) OpenSpliceMessageInfoList
#else
    class __declspec(dllimport) OpenSpliceMessageInfoList
#endif
#else
    class OpenSpliceMessageInfoList
#endif
  {
    class OpenSpliceMessageInfoEntry;
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
    OpenSpliceMessageInfoList();
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
    ~OpenSpliceMessageInfoList();
    /*!
     * @if jp
     *
     * @brief OpenSpliceMessageInfoを追加
     * 
     * @param id 名前
     * @param info OpenSpliceMessageInfo
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
    void addInfo(const std::string &id, OpenSpliceMessageInfoBase* info);
    /*!
     * @if jp
     *
     * @brief OpenSpliceMessageInfoを削除
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
     * @brief 指定名のOpenSpliceMessageInfoを取得
     *
     * @param id 名前
     * @return OpenSpliceMessageInfo
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
    OpenSpliceMessageInfoBase* getInfo(const std::string& id);
  public:

    std::map<std::string, OpenSpliceMessageInfoEntry> m_data;

  private:
    /*!
     * @if jp
     *
     * @class OpenSpliceMessageInfoEntry
     *
     * @brief OpenSpliceMessageInfoと終了関数を登録するクラス
     * dllから登録したOpenSpliceMessageInfoはdllでdeleteする必要があるため、
     * 必ず終了関数を登録する。
     *
     * @since 2.0.0
     *
     * @else
     *
     * @class OpenSpliceMessageInfoEntry
     *
     * @brief 
     *
     *
     * @endif
     */
    class OpenSpliceMessageInfoEntry
    {
    public:
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         * @param object OpenSpliceMessageInfo
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
        OpenSpliceMessageInfoEntry(OpenSpliceMessageInfoBase* object, void(*destructor)(OpenSpliceMessageInfoBase*&));
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
        OpenSpliceMessageInfoEntry();
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
        OpenSpliceMessageInfoEntry(const OpenSpliceMessageInfoEntry& obj);
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
        OpenSpliceMessageInfoEntry& operator = (const OpenSpliceMessageInfoEntry& obj);
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
        bool operator==(const OpenSpliceMessageInfoEntry& obj);
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
        ~OpenSpliceMessageInfoEntry();
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
        OpenSpliceMessageInfoBase* object_;
        void(*destructor_)(OpenSpliceMessageInfoBase*&);

    };
  };

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef TRANSPORT_PLUGIN
  class __declspec(dllexport) GlobalOpenSpliceMessageInfoList
#else
  class __declspec(dllimport) GlobalOpenSpliceMessageInfoList
#endif
#else
  class GlobalOpenSpliceMessageInfoList
#endif
      : public OpenSpliceMessageInfoList,
      public coil::Singleton<GlobalOpenSpliceMessageInfoList >
  {
  public:
  private:
      friend class coil::Singleton<GlobalOpenSpliceMessageInfoList>;
  };

}

void OpenSpliceMessageInfoInit(const coil::Properties& prop);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( pop )
#endif

#endif // RTC_OPENSPLICEMESSAGEINFO_H

