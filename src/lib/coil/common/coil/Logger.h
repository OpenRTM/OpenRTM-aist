// -*- C++ -*-
/*!
 * @file Logger.h
 * @brief log_streambuf and log_stream class
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
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <coil/Mutex.h>
#include <coil/Guard.h>

#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>



namespace coil
{
  /*!
   * @if jp
   *
   * @class LogStreamBuffer
   * @brief LogStreamBuffer基底クラス
   *
   * @else
   *
   * @class LogStreamBuffer
   * @brief LogStreamBuffer base class
   *
   * @endif
   */
  class LogStreamBuffer
  {
      typedef coil::Mutex Mutex;
      typedef coil::Guard<coil::Mutex> Guard;
  public:
      /*!
       * @if jp
       *
       * @brief コンストラクタ
       *
       * コンストラクタ
       *
       * @else
       *
       * @brief Constructor
       *
       * Constructor
       *
       * @endif
       */
      LogStreamBuffer();
      /*!
       * @if jp
       *
       * @brief デストラクタ
       *
       * デストラクタ。
       *
       * @else
       *
       * @brief Destructor
       *
       * Destructor
       *
       * @endif
       */
      virtual ~LogStreamBuffer();

      /*!
       * @if jp
       *
       * @brief ストリームを追加する
       *
       * LogStreamBuffer に実際の出力先であるストリームを追加する。
       * ここで追加されたストリームの解体責任はユーザにあり、
       * LogStreamBuffer 解体時には解体されることはない。
       * また追加されているストリームを LogStreamBuffer の解体前に
       * 解体してはならない。ストリームの解体は LogStreamBuffer の解体後に、
       * ユーザが解体しなければならない。
       *
       * @param stream std::LogStreamBuffer 型のストリームへのポインタ
       *
       * @else
       *
       * @brief add stream
       *
       * This operation adds a stream that is actual output stream.
       * User has responsibility to destruct the stream object, since
       * this object never destructs the stream objects.  The added
       * stream objects should not be destructed before the destruction
       * of this object.  The stream destruction should be done by user
       * after removing it from this object or destructing this object.
       *
       * @param stream a pointer to LogStreamBuffer type stream object
       *
       * @endif
       */
      void addStream(LogStreamBuffer *stream, bool cleanup = false);

      /*!
       * @if jp
       *
       * @brief ストリームを削除する
       *
       * LogStreamBuffer から出力先であるストリームを削除する。
       * ここで削除されたストリームの解体責任はユーザにある。
       *
       * @param stream LogStreamBuffer 型のストリームへのポインタ
       * @return 削除の成功(True)、失敗(False)
       *
       * @else
       *
       * @brief remove stream
       *
       * This operation remove a stream that is actual output stream.
       * User has responsibility to destruct the stream object.
       *
       * @param stream a pointer to std::basic_streambuf type stream object
       * @return Whether removing the stream succeeded.
       *
       * @endif
       */
      bool removeStream(LogStreamBuffer* stream);

      /*!
       * @if jp
       *
       * @brief ストリームバッファ取得
       *
       * ストリームバッファを返す。
       *
       * @return LogStreamBuffer リスト
       *
       * @else
       *
       * @brief Get stream buffer list
       *
       * Return a stream buffer list.
       *
       * @return LogStreamBuffer list
       *
       * @endif
       */
      std::vector<LogStreamBuffer*> getBuffers();

      /*!
       * @if jp
       *
       * @brief ログの出力
       *
       * 指定したメッセージのログを出力する
       *
       * @param level ログレベル
       * @param name 名前
       * @param date 時間
       * @param mes メッセージ
       *
       *
       * @else
       *
       * @brief log output
       *
       * 
       *
       * @param level log level
       * @param name log name
       * @param date time
       * @param mes message
       *
       * @endif
       */
      virtual void write(int level, const std::string &name, const std::string &date, const std::string &mes);

      /*!
       * @if jp
       *
       * @brief 標準出力のバッファのフラッシュ
       *
       * 
       *
       *
       * @else
       *
       * @brief 
       *
       *
       * @endif
       */
      virtual void flush();

      /*!
       * @if jp
       *
       * @brief ストリーム管理用構造体
       *
       * @else
       *
       * @brief Structure for stream management
       *
       * @endif
       */
      struct Stream
      {
          explicit Stream(LogStreamBuffer* stream, bool cleanup = false)
              : stream_(stream), cleanup_(cleanup)
          {
          }

          virtual ~Stream()
          {
          }

          Stream(const Stream& x)
              : stream_(x.stream_), cleanup_(false)
          {
          }

          Stream& operator=(const Stream& x)
          {
              Stream tmp(x);
              tmp.swap(*this);
              return *this;
          }

          void swap(Stream& x)
          {
              LogStreamBuffer* stream = x.stream_;
              bool cleanup = x.cleanup_;

              x.stream_ = this->stream_;
              x.cleanup_ = this->cleanup_;

              this->stream_ = stream;
              this->cleanup_ = cleanup;
          }
          mutable Mutex mutex_;
          LogStreamBuffer* stream_;
          bool cleanup_;
      };
  private:
      /*!
       * @if jp
       *
       * @brief コピーコンストラクタ
       *
       * コピーコンストラクタ
       *
       * @param x LogStreamBuffer オブジェクト
       *
       * @else
       *
       * @brief Copy Constructor
       *
       * Copy Constructor
       *
       * @param x LogStreamBuffer object
       *
       * @endif
       */
      LogStreamBuffer(const LogStreamBuffer& x);

      /*!
       * @if jp
       *
       * @brief 代入演算子
       *
       * LogStreamBufferオブジェクトをコピーする。
       *
       * @param x LogStreamBuffer オブジェクト
       *
       * @return 代入結果
       *
       * @else
       *
       * @brief Assignment operator
       *
       * Copy a LogStreamBuffer object.
       *
       * @param x LogStreamBuffer object.
       *
       * @return Assignment result.
       *
       * @endif
       */
      LogStreamBuffer& operator=(const LogStreamBuffer& x);
      std::vector<Stream> m_streams;
  };

  /*!
   * @if jp
   *
   * @class LogStream
   * @brief LogStream 基底クラス
   *
   * @else
   *
   * @class LogStream
   * @brief LogStream base class
   *
   * @endif
   */
  class LogStream
  {
  public:
      
      /*!
       * @if jp
       *
       * @brief コンストラクタ
       *
       * コンストラクタ
       *
       * @param sb LogStreamBuffer 型オブジェクト
       * @param levelmin ログレベルの最小値
       * @param levelmax ログレベルの最大値
       * @param デフォルトのログレベル
       *
       * @else
       *
       * @brief Constructor
       *
       * Constructor
       *
       * @param sb LogStreamBuffer type object
       * @param levelmin minimum value for log level
       * @param levelmax maximum value for log level
       * @param level default log level
       *
       * @endif
       */
      LogStream(LogStreamBuffer* sb, int levelmin, int levelmax, int level);

      /*!
       * @if jp
       *
       * @brief ログレベル設定
       *
       * ログレベルを設定する。
       *
       * @param level ログレベル
       *
       * @else
       *
       * @brief Set the log level
       *
       * Set the log level.
       *
       * @param level Log level
       *
       * @endif
       */
      bool setLevel(int level);

      /*!
       * @if jp
       *
       * @brief ログレベル取得
       *
       * ログレベルを取得する。
       *
       * @return ログレベル
       *
       * @else
       *
       * @brief Get the log level
       *
       * Get the log level.
       *
       * @return Log level
       *
       * @endif
       */
      int getLevel() const;

      /*!
       * @if jp
       *
       * @brief ログの出力
       *
       * 指定したログメッセージを出力する
       *
       * @param level ログレベル
       * @param mes ログメッセージ
       *
       *
       * @else
       *
       * @brief log output
       *
       * 
       * @param level log level
       * @param mes log message
       *
       * @endif
       */
      virtual void write(int level, const std::string &mes) = 0;


      /*!
       * @if jp
       *
       * @brief ロックモード設定
       *
       * ロックモードを有効にする。
       *
       * @else
       *
       * @brief Enable the lock mode
       *
       * Enable the lock mode.
       *
       * @endif
       */
      void enableLock();


      /*!
       * @if jp
       *
       * @brief ロックモード解除
       *
       * ロックモードを無効にする。
       *
       * @else
       *
       * @brief Disable the lock mode
       *
       * Disable the lock mode.
       *
       * @endif
       */
      void disableLock();

      /*!
       * @if jp
       *
       * @brief 標準出力のバッファのフラッシュ
       *
       * @else
       *
       * @brief cout buffer flush
       *
       *
       * @endif
       */
      void flush();

      /*!
       * @if jp
       *
       * @brief ログレベル有効チェック
       *
       * 指定されたログレベルが有効範囲かチェックし、有効・無効を返す。
       *
       * @param level ログレベル
       *
       * @return true: 有効, false: 無効
       *
       * @else
       *
       * @brief Log level effective check
       *
       * Check it whether an appointed log level is an effective range
       * and return effective or invalidity.
       *
       * @param level Log level
       *
       * @return true: Valid, false: Invalid
       *
       * @endif
       */
      inline bool isValid(int level) const
      {
          return m_minLevel <= level && level <= m_logLevel;
      }

      /*!
       * @if jp
       *
       * @brief ログロック取得
       * ロックモードが設定されている場合、ログのロックを取得する。
       *
       * @else
       *
       * @brief Acquire log lock
       * Acquire log lock when the lock mode is set.
       *
       * @endif
       */
      inline void lock()
      {
          if (m_lockEnable) m_mutex.lock();
      }

      /*!
       * @if jp
       *
       * @brief ログロック解放
       * ロックモードが設定されている場合に、ログのロックを解放する。
       *
       * @else
       *
       * @brief Release the log lock
       * Release the log lock when the lock mode is set.
       *
       * @endif
       */
      inline void unlock()
      {
          if (m_lockEnable) m_mutex.unlock();
      }
  protected:
      /*!
       * @if jp
       *
       * @brief デストラクタ
       *
       * デストラクタ。
       *
       * @else
       *
       * @brief Destructor
       *
       * Destructor
       *
       * @endif
       */
      virtual ~LogStream() {}
      LogStreamBuffer* ostream_type;

  private:

      /*!
       * @if jp
       *
       * @brief コピーコンストラクタ
       *
       * コピーコンストラクタ
       *
       * @param x LogStream オブジェクト
       *
       * @else
       *
       * @brief Copy Constructor
       *
       * Copy Constructor
       *
       * @param x LogStream object
       *
       * @endif
       */
      LogStream(const LogStream& x);


      /*!
       * @if jp
       *
       * @brief 代入演算子
       *
       * LogStreamオブジェクトをコピーする。
       *
       * @param x LogStreamオブジェクト
       *
       * @return 代入結果
       *
       * @else
       *
       * @brief Assignment operator
       *
       * Copy a LogStream object.
       *
       * @param x LogStream object.
       *
       * @return Assignment result.
       *
       * @endif
       */
      LogStream& operator=(const LogStream& x);

      
      int m_minLevel, m_maxLevel;
      int m_logLevel;

      
  public:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   ifdef COIL_LIBRARY_EXPORTS
      static __declspec(dllexport) bool m_lockEnable;
      static __declspec(dllexport) Mutex m_mutex;
#   else
      static __declspec(dllimport) bool m_lockEnable;
      static __declspec(dllimport) Mutex m_mutex;
#   endif
#else
      static bool m_lockEnable;
      static Mutex m_mutex;
#endif /* Windows */

  };


} // namespace coil
#endif  // LOGGER_H
