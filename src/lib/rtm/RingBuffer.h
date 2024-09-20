// -*- C++ -*-
/*!
 * @file RingBuffer.h
 * @brief Defautl Buffer class
 * @date $Date: 2007-12-31 03:08:06 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2009
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

#ifndef RTC_RINGBUFFER_H
#define RTC_RINGBUFFER_H

#include <mutex>
#include <condition_variable>
#include <coil/stringutil.h>

#include <rtm/BufferBase.h>
#include <rtm/BufferStatus.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#define RINGBUFFER_DEFAULT_LENGTH 8
/*!
 * @if jp
 * @namespace RTC
 *
 * @brief RTコンポーネント
 *
 * @else
 *
 * @namespace RTC
 *
 * @brief RT-Component
 *
 * @endif
 */
namespace RTC
{
  /*!
   * @if jp
   * @class RingBuffer
   * @brief リングバッファ実装クラス
   *
   * 指定した長さのリング状バッファを持つバッファ実装クラス。
   * バッファ全体にデータが格納された場合、以降のデータは古いデータから
   * 順次上書きされる。
   * 従って、バッファ内には直近のバッファ長分のデータのみ保持される。
   *
   * 注)現在の実装では、一番最後に格納したデータのみバッファから読み出し可能
   *
   * @param DataType バッファに格納するデータ型
   *
   * @since 0.4.0
   *
   * @else
   * @class RingBuffer
   * @brief Ring buffer implementation class
   *
   * This is the buffer implementation class with ring shaped buffer of
   * specified length.
   * If data is stored in the entire buffer, data from now on will be
   * overwritten from old data one by one.
   * Therefore, only the length of latest data is stored in the buffer.
   *
   * Note: In the current implementation, only last stored data can be read
   *       from the buffer.
   *
   * @param DataType Data type to store in the buffer
   *
   * @since 0.4.0
   *
   * @endif
   */
  template <class DataType>
  class RingBuffer
    : public BufferBase<DataType>
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     * 指定されたバッファ長でバッファを初期化する。
     *
     * @param length バッファ長
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor.
     * Initialize the buffer by specified buffer length.
     * However, if the specified length is less than two, the buffer should
     * be initialized by two in length.
     *
     * @param length Buffer length
     *
     * @endif
     */
    explicit RingBuffer(long int length = RINGBUFFER_DEFAULT_LENGTH)
      : m_length(length), m_buffer(m_length)
    {
      this->reset();
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
    ~RingBuffer() override;

    /*!
     * @if jp
     * @brief バッファの設定
     *
     * coil::Properties で与えられるプロパティにより、
     * バッファの設定を初期化する。
     * 使用できるオプションと意味は以下の通り
     *
     * - buffer.length:
     *     バッファの長さ。自然数以外の数値が指定されても無視される。す
     *     でにバッファが使用状態でも、長さが再設定されたのち、すべての
     *     ポインタが初期化される。
     *
     * - buffer.write.full_policy:
     *     上書きするかどうかのポリシー。
     *     overwrite (上書き), do_nothing (何もしない), block (ブロックする)
     *     block を指定した場合、次の timeout 値を指定すれば、指定時間後
     *     書き込み不可能であればタイムアウトする。
     *     デフォルトは  overwrite (上書き)。
     *
     * - buffer.write.timeout:
     *     タイムアウト時間を [sec] で指定する。デフォルトは 1.0 [sec]。
     *     1 sec -> 1.0, 1 ms -> 0.001, タイムアウトしない -> 0.0
     *
     * - buffer.read.empty_policy:
     *     バッファが空のときの読み出しポリシー。
     *     readback (最後の要素), do_nothing (何もしない), block (ブロックする)
     *     block を指定した場合、次の timeout 値を指定すれば、指定時間後
     *     読み出し不可能であればタイムアウトする。
     *     デフォルトは readback (最後の要素)。
     *
     * - buffer.read.timeout:
     *     タイムアウト時間 [sec] で指定する。デフォルトは 1.0 [sec]。
     *     1sec -> 1.0, 1ms -> 0.001, タイムアウトしない -> 0.0
     *
     * @else
     *
     * @endif
     */
    void init(const coil::Properties& prop) override
    {
      initLength(prop);
      initWritePolicy(prop);
      initReadPolicy(prop);
    }

    /*!
     * @if jp
     *
     * @brief バッファ長を取得する
     *
     * バッファ長を取得する。
     *
     * @return バッファ長
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Get the buffer length.
     *
     * @return Buffer length
     *
     *
     * @endif
     */
    size_t length() const override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return m_length;
    }

    /*!
     * @if jp
     *
     * @brief バッファの長さをセットする
     *
     * バッファ長を設定する。設定不可な場合はNOT_SUPPORTEDが返る。
     * この実装では OK しか返さない。
     *
     * @return OK: 正常終了
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Pure virtual function to get the buffer length.
     *
     * @return buffer length
     *
     * @endif
     */
    BufferStatus length(size_t n) override
    {
      m_buffer.resize(n);
      m_length = n;
      this->reset();
      return BufferStatus::OK;
    }

    /*!
     * @if jp
     *
     * @brief バッファの状態をリセットする
     *
     * バッファの読み出しポインタと書き込みポインタの位置をリセットする。
     * この実装では OK しか返さない。
     *
     * @return OK: 正常終了
     *         NOT_SUPPORTED: リセット不可能
     *         BUFFER_ERROR: 異常終了
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Pure virtual function to get the buffer length.
     *
     * @return buffer length
     *
     * @endif
     */
    BufferStatus reset() override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      m_fillcount = 0;
      m_wcount = 0;
      m_wpos = 0;
      m_rpos = 0;
      return BufferStatus::OK;
    }



    //----------------------------------------------------------------------
    /*!
     * @if jp
     *
     * @brief バッファの現在の書込み要素のポインタ
     *
     * バッファの現在の書込み要素のポインタまたは、n個先のポインタを返す
     *
     * @param  n 書込みポインタ + n の位置のポインタ
     * @return 書込み位置のポインタ
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Pure virtual function to get the buffer length.
     *
     * @return buffer length
     *
     * @endif
     */
    DataType* wptr(long int n = 0) override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return &m_buffer[(m_wpos + n + m_length) % m_length];
    }

    /*!
     * @if jp
     *
     * @brief 書込みポインタを進める
     *
     * 現在の書き込み位置のポインタを n 個進める。
     * 書き込み可能な要素数以上の数値を指定した場合、PRECONDITION_NOT_MET
     * を返す。
     *
     * @param  n 書込みポインタ + n の位置のポインタ
     * @param  unlock_enable trueの場合にバッファエンプティのブロックを解除する
     * @return OK:            正常終了
     *         PRECONDITION_NOT_MET: n > writable()
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Pure virtual function to get the buffer length.
     *
     * @return buffer length
     *
     * @endif
     */
    BufferStatus advanceWptr(long int n = 1, bool unlock_enable = true) override
    {
      bool empty_ = false;
      bool lock_ = (unlock_enable && n > 0);
      if(lock_)
        {
          m_empty.mutex.lock();
          empty_ = empty();
        }
      // n > 0 :
      //     n satisfies n <= writable elements
      //                 n <= m_length - m_fillcout
      // n < 0 : -n = n'
      //     n satisfies n'<= readable elements
      //                 n'<= m_fillcount
      //                 n >= - m_fillcount
      {
          std::lock_guard<std::mutex> guard(m_posmutex);
          if ((n > 0 && n > static_cast<long int>(m_length) - static_cast<long int>(m_fillcount)) ||
              (n < 0 && n < -static_cast<long int>(m_fillcount)))
            {
              if (lock_)
              {
                  m_empty.mutex.unlock();
              }
              return BufferStatus::PRECONDITION_NOT_MET;
            }

          m_wpos = (m_wpos + n + m_length) % m_length;
          m_fillcount += n;
          m_wcount += n;
      }

      if(lock_)
        {
          if(empty_)
            {
              m_empty.cond.notify_one();
            }
          m_empty.mutex.unlock();
        }

      return BufferStatus::OK;
    }
    /*!
     * @if jp
     *
     * @brief バッファにデータを書き込む
     *
     * バッファにデータを書き込む。書き込みポインタの位置は変更されない。
     * この実装では常に OK を返す。
     *
     * @param value 書き込み対象データ
     *
     * @return OK: 正常終了
     *         BUFFER_ERROR: 異常終了
     *
     * @else
     *
     * @brief Write data into the buffer
     *
     * Pure virtual function to write data into the buffer.
     * Always OK will be returned in this implementation.
     *
     * @param value Target data to write.
     *
     * @return OK:    Successful
     *         BUFFER_ERROR: Failed
     *
     * @endif
     */
    BufferStatus put(const DataType& value) override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      m_buffer[m_wpos] = value;
      return BufferStatus::OK;
    }

    /*!
     * @if jp
     *
     * @brief バッファに書き込む
     *
     * 引数で与えられたデータをバッファに書き込む。
     *
     * 第2引数(timeout)が指定されていない場合、バッファフル
     * 時の書込みモード (overwrite, do_nothing, block) は init() で設定
     * されたモードに従う。
     *
     * 第2引数(timeout)に引数が指定された場合、init() で設定されたモード
     * に関わらず、block モードとなり、バッファがフル状態であれば指定時
     * 間まち、タイムアウトする。タイムアウト待ち中に、読み出しスレッド
     * 側でバッファから読み出せば、ブロッキングは解除されデータが書き込
     * まれる。
     *
     * 書き込み時にバッファが空(empty)状態で、別のスレッドがblockモード
     * で読み出し待ちをしている場合、signalを発行して読み出し側のブロッ
     * キングが解除される。
     *
     * @param value 書き込み対象データ
     * @param timeout タイムアウト時間 nsec (default -1: 無効)
     * @return OK            正常終了
     *         FULL          バッファがフル状態
     *         TIMEOUT              書込みがタイムアウトした
     *         PRECONDITION_NOT_MET 設定異常
     *
     * @else
     *
     * @brief Write data into the buffer
     *
     * Write data which is given argument into the buffer.
     *
     * @param value Target data for writing
     *
     * @return Writing result (Always true: writing success is returned)
     *
     * @endif
     */
    BufferStatus write(const DataType& value,
                       std::chrono::nanoseconds timeout
                       = std::chrono::nanoseconds(-1)) override
    {
      {
      std::unique_lock<std::mutex> guard(m_full.mutex);

      if (full())
        {

          bool timedwrite(m_timedwrite);
          bool overwrite(m_overwrite);

          if (timeout >= std::chrono::seconds::zero())  // block mode
            {
              timedwrite = true;
              overwrite  = false;
            }

          if (overwrite && !timedwrite)  // "overwrite" mode
            {
              advanceRptr(1,false);
            }
          else if (!overwrite && !timedwrite)  // "do_nothing" mode
            {
              return BufferStatus::FULL;
            }
          else if (!overwrite && timedwrite)  // "block" mode
            {
              if (timeout < std::chrono::seconds::zero())
                {
                  timeout = m_wtimeout;
                }
              if (std::cv_status::timeout == m_empty.cond.wait_for(guard, timeout))
                {
                  return BufferStatus::TIMEOUT;
                }
            }
          else                                    // unknown condition
            {
              return BufferStatus::PRECONDITION_NOT_MET;
            }
        }
      }

      put(value);

      advanceWptr(1);


      return BufferStatus::OK;
    }

    /*!
     * @if jp
     *
     * @brief バッファに書込み可能な要素数
     *
     * バッファに書込み可能な要素数を返す。
     *
     * @return 書き込み可能な要素数
     *
     * @else
     *
     * @brief Write data into the buffer
     *
     * Pure virtual function to write data into the buffer.
     *
     * @param value Target data to write.
     *
     * @return Result of having written in data (true:Successful, false:Failed)
     *
     * @endif
     */
    size_t writable() const override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return m_length - m_fillcount;
    }

    /*!
     * @if jp
     *
     * @brief バッファfullチェック
     *
     * バッファfullチェック用純粋仮想関数
     *
     * @return fullチェック結果(true:バッファfull，false:バッファ空きあり)
     *
     * @else
     *
     * @brief Check on whether the buffer is full.
     *
     * Pure virtual function to check on whether the buffer is full.
     *
     * @return True if the buffer is full, else false.
     *
     * @endif
     */
    bool full() const override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return m_length == m_fillcount;
    }

    //----------------------------------------------------------------------
    /*!
     * @if jp
     *
     * @brief バッファの現在の読み出し要素のポインタ
     *
     * バッファの現在の読み出し要素のポインタまたは、n個先のポインタを返す
     *
     * @param  n 読み出しポインタ + n の位置のポインタ
     * @return 読み出し位置のポインタ
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Pure virtual function to get the buffer length.
     *
     * @return buffer length
     *
     * @endif
     */
    DataType* rptr(long int n = 0) override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return &(m_buffer[(m_rpos + n + m_length) % m_length]);
    }

    /*!
     * @if jp
     *
     * @brief 読み出しポインタを進める
     *
     * 現在の読み出し位置のポインタを n 個進める。
     *
     * @param  n 読み出しポインタ + n の位置のポインタ
     * @param  unlock_enable trueの場合にバッファフルのブロックを解除する
     * @return OK: 正常終了
     *         BUFFER_ERROR: 異常終了
     *
     * @else
     *
     * @brief Get the buffer length
     *
     * Pure virtual function to get the buffer length.
     *
     * @return buffer length
     *
     * @endif
     */
    BufferStatus advanceRptr(long int n = 1, bool unlock_enable = true) override
    {
      bool full_ = false;
      bool lock_ = (unlock_enable && n > 0);
      if(lock_)
        {
          m_full.mutex.lock();
          full_ = full();
        }
      // n > 0 :
      //     n satisfies n <= readable elements
      //                 n <= m_fillcout
      // n < 0 : -n = n'
      //     n satisfies n'<= m_length - m_fillcount
      //                 n >= m_fillcount - m_length
      {
          std::lock_guard<std::mutex> guard(m_posmutex);
          if ((n > 0 && n > static_cast<long int>(m_fillcount)) ||
              (n < 0 && n < static_cast<long int>(m_fillcount) - static_cast<long int>(m_length)))
            {
              if (lock_)
              {
                  m_full.mutex.unlock();
              }
              return BufferStatus::PRECONDITION_NOT_MET;
            }

          m_rpos = (m_rpos + n + m_length) % m_length;
          m_fillcount -= n;
      }

      if(lock_)
        {
          if(full_)
            {
              m_full.cond.notify_one();
            }
          m_full.mutex.unlock();
        }

      return BufferStatus::OK;
    }

    /*!
     * @if jp
     *
     * @brief バッファからデータを読み出す
     *
     * バッファからデータを読みだす。読み出しポインタの位置は変更されない。
     *
     * @param value 読み出しデータ
     *
     * @return OK: 正常終了
     *         BUFFER_ERROR: 異常終了
     *
     * @else
     *
     * @brief Write data into the buffer
     *
     * Pure virtual function to write data into the buffer.
     *
     * @param value Target data to write.
     *
     * @return Result of having written in data (true:Successful, false:Failed)
     *
     * @endif
     */
    BufferStatus get(DataType& value) override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      value = m_buffer[m_rpos];
      return BufferStatus::OK;
    }


    /*!
     * @if jp
     *
     * @brief バッファからデータを読み出す
     *
     * バッファからデータを読みだす。読み出しポインタの位置は変更されない。
     *
     * @return 読み出しデータ
     *
     * @else
     *
     * @brief Reading data from the buffer
     *
     * @return Read data
     *
     * @endif
     */
    DataType& get() override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return m_buffer[m_rpos];
    }


    /*!
     * @if jp
     *
     * @brief バッファから読み出す
     *
     * バッファに格納されたデータを読み出す。
     *
     * 第2引数(timeout)が指定されていない場合、バッファ空状態での読み出
     * しモード (readback, do_nothing, block) は init() で設定された
     * モードに従う。
     *
     * 第2引数(timeout)に引数が指定された場合、init() で設定されたモード
     * に関わらず、block モードとなり、バッファが空状態であれば指定時間
     * 待ち、タイムアウトする。タイムアウト待ち中に、書込みスレッド側で
     * バッファへ書込みがあれば、ブロッキングは解除されデータが読みださ
     * れる。
     *
     * 読み出し時にバッファが空(empty)状態で、別のスレッドがblockモード
     * で書込み待ちをしている場合、signalを発行して書込み側のブロッキン
     * グが解除される。
     *
     * @param value 読み出し対象データ
     * @param timeout タイムアウト時間 (default -1: 無効)
     * @return OK            正常終了
     *         EMPTY         バッファが空状態
     *         TIMEOUT              書込みがタイムアウトした
     *         PRECONDITION_NOT_MET 設定異常
     *
     * @else
     *
     * @brief Readout data from the buffer
     *
     * Readout data stored into the buffer.
     *
     * @param value Readout data
     *
     * @return Readout result (Always true: readout success is returned)
     *
     * @endif
     */
    BufferStatus read(DataType& value,
                      std::chrono::nanoseconds timeout
                      = std::chrono::nanoseconds(-1)) override
    {
      {
      std::unique_lock<std::mutex> guard(m_empty.mutex);

      if (empty())
        {
          bool timedread(m_timedread);
          bool readback(m_readback);

          if (timeout >= std::chrono::seconds::zero()) // block mode
            {
              timedread = true;
              readback  = false;
              timeout = m_rtimeout;
            }

          if (readback && !timedread)       // "readback" mode
            {
              if (!(m_wcount > 0))
                {
                  return BufferStatus::EMPTY;
                }
              advanceRptr(-1,false);
            }
          else if (!readback && !timedread)  // "do_nothing" mode
            {
              return BufferStatus::EMPTY;
            }
          else if (!readback && timedread)  // "block" mode
            {
              if (timeout < std::chrono::seconds::zero())
                {
                  timeout = m_rtimeout;
                }
              if (std::cv_status::timeout == m_empty.cond.wait_for(guard, timeout))
                {
                  return BufferStatus::TIMEOUT;
                }
            }
          else                                    // unknown condition
            {
              return BufferStatus::PRECONDITION_NOT_MET;
            }
        }
      }

      get(value);

      advanceRptr(1);

      return BufferStatus::OK;
    }

    /*!
     * @if jp
     *
     * @brief バッファから読み出し可能な要素数
     *
     * バッファから読み出し可能な要素数を返す。
     *
     * @return 読み出し可能な要素数
     *
     * @return OK: 正常終了
     *         BUFFER_ERROR: 異常終了
     *
     * @else
     *
     * @brief Write data into the buffer
     *
     * Pure virtual function to write data into the buffer.
     *
     * @param value Target data to write.
     *
     * @return Result of having written in data (true:Successful, false:Failed)
     *
     * @endif
     */
    size_t readable() const override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return m_fillcount;
    }

    /*!
     * @if jp
     *
     * @brief バッファemptyチェック
     *
     * バッファemptyチェック用純粋仮想関数
     *
     * @return emptyチェック結果(true:バッファempty，false:バッファデータあり)
     *
     * @else
     *
     * @brief Check on whether the buffer is empty.
     *
     * Pure virtual function to check on whether the buffer is empty.
     *
     * @return True if the buffer is empty, else false.
     *
     * @endif
     */
    bool empty() const override
    {
      std::lock_guard<std::mutex> guard(m_posmutex);
      return m_fillcount == 0;
    }

  private:
    void initLength(const coil::Properties& prop)
    {
      if (!prop["length"].empty())
        {
          size_t n;
          if (coil::stringTo(n, prop["length"].c_str()))
            {
              if (n > 0)
                {
                  this->length(n);
                }
            }
        }
    }

    void initWritePolicy(const coil::Properties& prop)
    {
      std::string policy(coil::normalize(prop["write.full_policy"]));
      if (policy == "overwrite")
        {
          m_overwrite = true;
          m_timedwrite = false;
        }
      else if (policy == "do_nothing")
        {
          m_overwrite = false;
          m_timedwrite = false;
        }
      else if (policy == "block")
        {
          m_overwrite = false;
          m_timedwrite = true;

          std::chrono::nanoseconds tm;
          if (coil::stringTo(tm, prop["write.timeout"].c_str())
              && !(tm < std::chrono::seconds::zero()))
            {
              m_wtimeout = tm;
            }
        }
    }

    void initReadPolicy(const coil::Properties& prop)
    {
      std::string policy(prop["read.empty_policy"]);
      if (policy == "readback")
        {
          m_readback = true;
          m_timedread = false;
        }
      else if (policy == "do_nothing")
        {
          m_readback = false;
          m_timedread = false;
        }
      else if (policy == "block")
        {
          m_readback = false;
          m_timedread = true;
          std::chrono::nanoseconds tm;
          if (coil::stringTo(tm, prop["read.timeout"].c_str()))
            {
              m_rtimeout = tm;
            }
        }
    }

  private:
    /*!
     * @if jp
     * @brief 上書きフラグ
     * @else
     * @brief Overwrite flag
     * @endif
     */
    bool m_overwrite{true};

    /*!
     * @if jp
     * @brief 読み戻しフラグ
     * @else
     * @brief Readback flag
     * @endif
     */
    bool m_readback{true};

    /*!
     * @if jp
     * @brief タイムアウト付き書き込みフラグ
     * @else
     * @brief Timedwrite flag
     * @endif
     */
    bool m_timedwrite{false};
    /*!
     * @if jp
     * @brief タイムアウト付き読み出しフラグ
     * @else
     * @brief Timedread flag
     * @endif
     */
    bool m_timedread{false};

    /*!
     * @if jp
     * @brief 書き込み時タイムアウト
     * @else
     * @brief Timeout time for writing
     * @endif
     */
    std::chrono::nanoseconds m_wtimeout{std::chrono::seconds(1)};

    /*!
     * @if jp
     * @brief 読み出し時タイムアウト
     * @else
     * @brief Timeout time of reading
     * @endif
     */
    std::chrono::nanoseconds m_rtimeout{std::chrono::seconds(1)};

    /*!
     * @if jp
     * @brief バッファ長
     * @else
     * @brief Buffer length
     * @endif
     */
    size_t m_length;

    /*!
     * @if jp
     * @brief 書き込みポインタ
     * @else
     * @brief pointer to write
     * @endif
     */
    size_t m_wpos{0};

    /*!
     * @if jp
     * @brief 読み出しポインタ
     * @else
     * @brief poitner to read
     * @endif
     */
    size_t m_rpos{0};

    /*!
     * @if jp
     * @brief Fillカウント
     * @else
     * @brief Fill count
     * @endif
     */
    size_t m_fillcount{0};

    /*!
     * @if jp
     * @brief 書き込みカウント
     * @else
     * @brief Counter for writing
     * @endif
     */
    size_t m_wcount{0};

    /*!
     * @if jp
     * @brief バッファ配列
     * @else
     * @brief baffer array
     * @endif
     */
    std::vector<DataType> m_buffer;

    /*!
     * @if jp
     * @brief 条件変数構造体
     * @else
     * @brief struct for condition variable
     * @endif
     */
    struct condition
    {
      condition() {}
      std::condition_variable cond;
      std::mutex mutex;
    };

    /*!
     * @if jp
     * @brief 位置変数ミューテックス
     * @else
     * @brief mutex for position variable
     * @endif
     */
    mutable std::mutex m_posmutex;

    /*!
     * @if jp
     * @brief 空条件変数
     * @else
     * @brief empty condition variable
     * @endif
     */
    condition m_empty;

    /*!
     * @if jp
     * @brief 満杯条件変数
     * @else
     * @brief full condition variable
     * @endif
     */
    condition m_full;
  };

  template <class T> RingBuffer<T>::~RingBuffer() = default; // no-inline because of its size.
} // namespace RTC

#endif  // RTC_RINGBUFFER_H
