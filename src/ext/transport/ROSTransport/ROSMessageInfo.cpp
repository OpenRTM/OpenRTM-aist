// -*- C++ -*-
/*!
 * @file  ROSMessageInfo.cpp
 * @brief ROS Massage Type Info class
 * @date  $Date: 2019-02-05 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#include "ROSMessageInfo.h"

namespace RTC
{
    std::string ROSMessageInfoBase::type()
    {
        return m_type;
    }

    std::string ROSMessageInfoBase::md5sum()
    { 
        return m_md5sum;
    }

    std::string ROSMessageInfoBase::message_definition()
    {
        return m_message_definition;
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
    ROSMessageInfoList::ROSMessageInfoList() = default;
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
    ROSMessageInfoList::~ROSMessageInfoList()
    {
        for (auto info : m_data)
        {
            info.second.deleteObject();
        }
    }

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
    void ROSMessageInfoList::addInfo(const std::string &id, ROSMessageInfoBase* info)
    {
        auto data = m_data.find(id);
        if (data != m_data.end())
        {
            data->second.deleteObject();
        }
        m_data[id] = ROSMessageInfoEntry(info, [](ROSMessageInfoBase*& obj) { delete obj; });
    }

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
    bool ROSMessageInfoList::removeInfo(const std::string& id)
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
    ROSMessageInfoBase* ROSMessageInfoList::getInfo(const std::string& id)
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
    ROSMessageInfoList::ROSMessageInfoEntry::ROSMessageInfoEntry(ROSMessageInfoBase* object, void(*destructor)(ROSMessageInfoBase*&))
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
    ROSMessageInfoList::ROSMessageInfoEntry::ROSMessageInfoEntry() = default;

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
    ROSMessageInfoList::ROSMessageInfoEntry::ROSMessageInfoEntry(const ROSMessageInfoList::ROSMessageInfoEntry& obj)
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
    ROSMessageInfoList::ROSMessageInfoEntry& ROSMessageInfoList::ROSMessageInfoEntry::operator = (const ROSMessageInfoList::ROSMessageInfoEntry& obj)
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
    bool ROSMessageInfoList::ROSMessageInfoEntry::operator==(const ROSMessageInfoList::ROSMessageInfoEntry& obj)
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
    ROSMessageInfoList::ROSMessageInfoEntry::~ROSMessageInfoEntry() = default;

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
    void ROSMessageInfoList::ROSMessageInfoEntry::deleteObject()
    {
        if (object_)
        {
            destructor_(object_);
            object_ = nullptr;
        }
    }
}



