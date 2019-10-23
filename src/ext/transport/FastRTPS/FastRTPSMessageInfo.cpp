// -*- C++ -*-
/*!
 * @file  FastRTPSMessageInfo.cpp
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

#include "FastRTPSMessageInfo.h"

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
    FastRTPSMessageInfoBase::~FastRTPSMessageInfoBase(void) = default;

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
    FastRTPSMessageInfoList::FastRTPSMessageInfoList() = default;
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
    FastRTPSMessageInfoList::~FastRTPSMessageInfoList()
    {
        for (auto info : m_data)
        {
            delete info.second;
        }
    }
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
    void FastRTPSMessageInfoList::addInfo(const std::string &id, FastRTPSMessageInfoBase* info)
    {
        m_data[id] = info;
    }
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
    bool FastRTPSMessageInfoList::removeInfo(const std::string& id)
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
    FastRTPSMessageInfoBase* FastRTPSMessageInfoList::getInfo(const std::string& id)
    {
        if (m_data.count(id) == 0)
        {
            return nullptr;
        }
        return m_data[id];
    }
}