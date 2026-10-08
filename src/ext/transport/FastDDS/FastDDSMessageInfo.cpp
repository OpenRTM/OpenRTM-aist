// -*- C++ -*-
/*!
 * @file  FastDDSMessageInfo.cpp
 * @brief Fast-RTPs Massage Type Info class
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

#include "FastDDSMessageInfo.h"

namespace RTC
{
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
    FastDDSMessageInfoBase::~FastDDSMessageInfoBase(void) = default;


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
    FastDDSMessageInfoList::FastDDSMessageInfoList() = default;
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
    FastDDSMessageInfoList::~FastDDSMessageInfoList()
    {
        for (auto info : m_data)
        {
            info.second.deleteObject();
        }
    }

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
    void FastDDSMessageInfoList::addInfo(const std::string &id, FastDDSMessageInfoBase* info)
    {
        auto data = m_data.find(id);
        if (data != m_data.end())
        {
            data->second.deleteObject();
        }
        m_data[id] = FastDDSMessageInfoEntry(info, [](FastDDSMessageInfoBase*& obj) { delete obj; });
    }

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
    bool FastDDSMessageInfoList::removeInfo(const std::string& id)
    {
        auto data = m_data.find(id);
        if (data == m_data.end())
        {
            return false;
        }

        data->second.deleteObject();
        m_data.erase(data);
        return true;
        
    }
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
    FastDDSMessageInfoBase* FastDDSMessageInfoList::getInfo(const std::string& id)
    {
        if (m_data.find(id) == m_data.end())
        {
            return nullptr;
        }
        return m_data[id].object_;
    }

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
    FastDDSMessageInfoList::FastDDSMessageInfoEntry::FastDDSMessageInfoEntry(FastDDSMessageInfoBase* object, void(*destructor)(FastDDSMessageInfoBase*&))
        : object_(object), destructor_(destructor)
    {
    }
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
    FastDDSMessageInfoList::FastDDSMessageInfoEntry::FastDDSMessageInfoEntry() = default;

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
    FastDDSMessageInfoList::FastDDSMessageInfoEntry::FastDDSMessageInfoEntry(const FastDDSMessageInfoList::FastDDSMessageInfoEntry& obj)
    {
        object_ = obj.object_;
        destructor_ = obj.destructor_;
    }

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
    FastDDSMessageInfoList::FastDDSMessageInfoEntry& FastDDSMessageInfoList::FastDDSMessageInfoEntry::operator = (const FastDDSMessageInfoList::FastDDSMessageInfoEntry& obj)
    {
        object_ = obj.object_;
        destructor_ = obj.destructor_;
        return *this;
    }

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
    bool FastDDSMessageInfoList::FastDDSMessageInfoEntry::operator==(const FastDDSMessageInfoList::FastDDSMessageInfoEntry& obj)
    {
        return (object_ == obj.object_);
    }

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
    FastDDSMessageInfoList::FastDDSMessageInfoEntry::~FastDDSMessageInfoEntry() = default;

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
    void FastDDSMessageInfoList::FastDDSMessageInfoEntry::deleteObject()
    {
        if (object_)
        {
            destructor_(object_);
            object_ = nullptr;
        }
    }
}
