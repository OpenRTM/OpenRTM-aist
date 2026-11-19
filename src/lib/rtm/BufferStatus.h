// -*- C++ -*-
/*!
 *
 * @file BufferStatus.h
 * @brief Buffer status enum definition
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 */

#ifndef RTC_BUFFERSTATUS_H
#define RTC_BUFFERSTATUS_H

#include <array>
#include <cstdint>

namespace RTC
{
    /*!
     * @if jp
     * @brief BufferStatus リターンコード
     *
     * バッファ関連のクラスで共通のリターンコード
     *
     * - OK:                   正常終了
     * - BUFFER_ERROR:         バッファエラー
     * - FULL:                 バッファフル
     * - EMPTY:                バッファエンプティ
     * - NOT_SUPPORTED:        未サポート機能
     * - TIMEOUT:              タイムアウト
     * - PRECONDITION_NOT_MET: 事前条件を満たしていない
     *
     * @else
     * @brief DataPortStatus return codes
     *
     * Common return codes for buffer classes.
     *
     * - OK:                   Normal return
     * - BUFFER_ERROR:         Buffer error
     * - FULL:                 Buffer full
     * - EMPTY:                Buffer empty
     * - NOT_SUPPORTED:        Not supported function
     * - TIMEOUT:              Timeout
     * - PRECONDITION_NOT_MET: Precodition not met
     *
     * @endif
     */
    enum class BufferStatus : uint8_t
      {
        OK = 0,
        BUFFER_ERROR, // MSVC has the macro named "ERROR".
        FULL,
        EMPTY,
        NOT_SUPPORTED,
        TIMEOUT,
        PRECONDITION_NOT_MET,
      };

    /*!
     * @if jp
     *
     * @brief BufferStatus リターンコードを文字列に変換
     *
     * BufferStatus リターンコードを文字列に変換する
     *
     * @param status 変換対象 BufferStatus リターンコード
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert BufferStatus into the string.
     *
     * Convert BufferStatus into the string.
     *
     * @param status The target BufferStatus for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    inline char const* toString(BufferStatus status)
    {
      static const std::array<const char*, 7> buffer_status = {{
        "BUFFER_OK",
        "BUFFER_ERROR",
        "BUFFER_FULL",
        "BUFFER_EMPTY",
        "NOT_SUPPORTED",
        "TIMEOUT",
        "PRECONDITION_NOT_MET"
      }};
      const auto index = static_cast<std::size_t>(status);
      return index < buffer_status.size() ? buffer_status[index] : "";
    }

} // namespace RTC

#endif  // RTC_BUFFERSTATUS_H
