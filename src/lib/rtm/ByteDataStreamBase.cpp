// -*- C++ -*-
/*!
 * @file ByteDataStreamBase.cpp
 * @brief Data Stream Buffer Base class
 * @date $Date: 2019-1-26 03:08:06 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2006-2019
 *     Noriaki Ando
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */


#include <rtm/ByteDataStreamBase.h>

namespace RTC
{
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     *
     *
     * @else
     *
     * @brief Constructor
     *
     *
     * @endif
     */
    ByteDataStreamBase::ByteDataStreamBase()
    {

    }

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * 仮想デストラクタ。
     *
     * @else
     *
     * @brief Virtual destractor
     *
     * Virtual destractor
     *
     * @endif
     */
    ByteDataStreamBase::~ByteDataStreamBase()
    {
    }

    /*!
     * @if jp
     * @brief 初期化関数(未使用)
     *
     * @param prop プロパティ(コネクタプロファイルから取得)
     *
     * @else
     * @brief
     *
     * @param prop
     *
     * @endif
     */
    void ByteDataStreamBase::init(const coil::Properties& /*prop*/)
    {

    }


    /*!
     * @if jp
     * @brief エンディアンの設定
     *
     * @param little_endian リトルエンディアン(True)、ビッグエンディアン(False)
     *
     * @else
     * @brief
     *
     * @param little_endian
     *
     * @endif
     */
    void ByteDataStreamBase::isLittleEndian(bool /*little_endian*/)
    {

    }


} // namespace RTC
