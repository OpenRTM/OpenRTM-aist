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
            delete info.second;
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
        m_data[id] = info;
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
        if (m_data.count(id) == 0)
        {
            return false;
        }
        m_data.erase(id);
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
        if (m_data.count(id) == 0)
        {
            return nullptr;
        }
        return m_data[id];
    }
}



