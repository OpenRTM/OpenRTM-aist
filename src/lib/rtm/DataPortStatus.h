// -*- C++ -*-
/*!
 * @file DataPortStatus.h
 * @brief DataPortStatus class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009-2010
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_DATAPORTSTATUS_H
#define RTC_DATAPORTSTATUS_H

#include <array>
#include <vector>

namespace RTC
{
    /*!
     * @if jp
     * brief DataPortStatus リターンコード
     *
     * データポート関連のクラスで共通のリターンコード
     *
     * - PORT_OK:              正常終了
     * - PORT_ERROR:           異常終了
     * - BUFFER_ERROR:         バッファエラー
     * - BUFFER_FULL:          バッファフル
     * - BUFFER_EMPTY:         バッファエンプティ
     * - BUFFER_TIMEOUT:       バッファタイムアウト
     * - SEND_FULL:            データを送ったが相手側がバッファフル状態
     * - SEND_TIMEOUT:         データを送ったが相手側がタイムアウトした
     * - RECV_EMPTY:           データを受信しようとしたがデータが空状態
     * - RECV_TIMEOUT:         データを受信しようとしたがタイムうとした
     * - INVALID_ARGS:         不正な引数
     * - PRECONDITION_NOT_MET: 事前条件を満たしていない
     * - CONNECTION_LOST:      接続が切断された
     * - UNKNOWN_ERROR:        不明なエラー
     *
     * データポートのデータ経路上のエラー発生個所から呼び出し側へエラー
     * 情報を伝えるためにこのエラーコードを使用する。主に、伝送路上のエ
     * ラー、伝送先のエラーなどが考えられるが、各部分の界面で発生するエ
     * ラーを以下に列挙する。
     *
     * (1) Push型
     *  a) InPortConsumer と Publisher/Activity 間で発生するリターンコード
     *     PORT_OK, PORT_ERROR, SEND_FULL, SEND_TIMEOUT, CONNECTION_LOST,
     *     UNKNOWN_ERROR
     *
     *  b) Activity と OutPort の Buffer/Connector 間で発生するリターンコード
     *     PORT_OK, PORT_ERROR, BUFFER_ERROR, BUFFER_FULL, BUFFER_TIMEOUT,
     *     UNKNOWN_ERROR,
     *
     * (2) Pull型
     *  a) Activity と InPort の間で発生するリターンコード
     *     PORT_OK, PORT_ERROR, RECV_EMPTY, RECV_TIMEOUT, CONNETION_LOST,
     *     UNKNOWN_ERROR
     *
     * 各関数が返すリターンコードは関数ごとのリファレンスを参照のこと。
     *
     * @else
     * @brief DataPortStatus return codes
     *
     * Common return codes for data ports related classes.
     *
     * - PORT_OK:              Normal return
     * - PORT_ERROR:           Error return
     * - BUFFER_ERROR:         Buffer error
     * - BUFFER_FULL:          Buffer full
     * - BUFFER_EMPTY:         Buffer empty
     * - BUFFER_TIMEOUT:       Buffer timeout
     * - SEND_FULL:            Buffer full although OutPort tried to send data
     * - SEND_TIMEOUT:         Timeout although OutPort tried to send data
     * - RECV_EMPTY:           Buffer empty although InPort tried to receive
     *                         data
     * - RECV_TIMEOUT:         Timeout although InPort tried to receive data
     * - INVALID_ARGS:         Invalid arguments
     * - PRECONDITION_NOT_MET: Precondition not met
     * - CONNECTION_LOST:      Connection has been lost
     * - UNKNOWN_ERROR:        Unknown error
     *
     * This error codes might be used to propagate error status from
     * the error occurring point to the function caller in the data
     * stream path. It would occur in data-transfer path and data
     * receiver/sender. The errors that occur in the interface of each
     * portion of data port are shown below.
     *
     * (1) Push Type
     *  a) The return codes between InPortConsumer and Publisher/Activity
     *     PORT_OK, PORT_ERROR, SEND_FULL, SEND_TIMEOUT, CONNECTION_LOST,
     *     UNKNOWN_ERROR
     *  b) The return codes between Activity and Buffer/Connector of OutPort
     *     PORT_OK, PORT_ERROR, BUFFER_ERROR, BUFFER_FULL, BUFFER_TIMEOUT,
     *     UNKNOWN_ERROR,
     *
     * (2) Pull Type
     *  a) The return codes between Activity and InPort
     *     PORT_OK, PORT_ERROR, RECV_EMPTY, RECV_TIMEOUT, CONNETION_LOST,
     *     UNKNOWN_ERROR
     *
     * See function references for detailed return codes for each function.
     *
     * @endif
     */
    enum class DataPortStatus : uint8_t
      {
        PORT_OK = 0,
        PORT_ERROR,
        BUFFER_ERROR,
        BUFFER_FULL,
        BUFFER_EMPTY,
        BUFFER_TIMEOUT,
        SEND_FULL,
        SEND_TIMEOUT,
        RECV_EMPTY,
        RECV_TIMEOUT,
        INVALID_ARGS,
        PRECONDITION_NOT_MET,
        CONNECTION_LOST,
        UNKNOWN_ERROR
      };
 
    /*!
     * @if jp
     *
     * @brief DataPortStatus リターンコードを文字列に変換
     *
     * DataPortStatus リターンコードを文字列に変換する
     *
     * @param status 変換対象 DataPortStatus リターンコード
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert DataPortStatus into the string.
     *
     * Convert DataPortStatus into the string.
     *
     * @param status The target DataPortStatus for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    inline const char* toString(DataPortStatus status)
    {
      static const std::array<const char*, 14> data_port_status = {{
        "PORT_OK",
        "PORT_ERROR",
        "BUFFER_ERROR",
        "BUFFER_FULL",
        "BUFFER_EMPTY",
        "BUFFER_TIMEOUT",
        "SEND_FULL",
        "SEND_TIMEOUT",
        "RECV_EMPTY",
        "RECV_TIMEOUT",
        "INVALID_ARGS",
        "PRECONDITION_NOT_MET",
        "CONNECTION_LOST",
        "UNKNOWN_ERROR"
      }};
      const auto index = static_cast<std::size_t>(status);
      return index < data_port_status.size() ? data_port_status[index] : "";
    }

  using DataPortStatusList = std::vector<DataPortStatus>;

} // namespace RTC

#endif  // RTC_DATAPORTSTATUS_H
