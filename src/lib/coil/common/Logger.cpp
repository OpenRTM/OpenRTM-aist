// -*- C++ -*-
/*!
 * @file Logger.h
 * @brief log_streambuf and log_stream class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Noriaki Ando
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/Logger.h>



namespace coil
{
  bool LogStream::m_lockEnable = true;
  coil::Mutex LogStream::m_mutex("Mutex for Logger.");
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
    LogStreamBuffer::LogStreamBuffer()
    {
    
    }
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
    LogStreamBuffer::~LogStreamBuffer()
    {

    }

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
    void LogStreamBuffer::addStream(LogStreamBuffer *stream, bool cleanup)
    {
       m_streams.push_back(Stream(stream, cleanup));
    }

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
    bool LogStreamBuffer::removeStream(LogStreamBuffer* stream)
    {
       std::vector<Stream>::iterator it;
       for (it = m_streams.begin(); it != m_streams.end(); ++it)
       {
           if (it->stream_ == stream)
           {
               m_streams.erase(it);
               return true;
           }
       }
       return false;
    }


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
    std::vector<LogStreamBuffer*> LogStreamBuffer::getBuffers()
    {
        std::vector<LogStreamBuffer*> buffs;
        std::vector<Stream>::iterator it;
        for (it = m_streams.begin(); it != m_streams.end(); ++it)
        {
            buffs.push_back((*it).stream_);
        }
        return buffs;
    }

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
    void LogStreamBuffer::write(int level, const std::string &name, const std::string &date, const std::string &mes)
    {
        std::vector<Stream>::iterator it;
        for (it = m_streams.begin(); it != m_streams.end(); ++it)
        {
            Guard gaurd((*it).mutex_);
            (*it).stream_->write(level, name, date, mes);
        }

    }

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
    void LogStreamBuffer::flush()
    {
    }

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
    LogStreamBuffer::LogStreamBuffer(const LogStreamBuffer& x)
    {
         this->m_streams = x.m_streams;
    }



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
    LogStreamBuffer& LogStreamBuffer::operator=(const LogStreamBuffer& x)
    {
          this->m_streams = x.m_streams;
          return *this;
    }



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
    LogStream::LogStream(LogStreamBuffer* sb, int levelmin, int levelmax, int level)
          : ostream_type(sb), 
            m_minLevel(levelmin), m_maxLevel(levelmax), m_logLevel(level)
    {
        if (m_minLevel >= m_maxLevel) throw std::bad_alloc();
    }

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
    bool LogStream::setLevel(int level)
    {
        if (m_minLevel <= level && level <= m_maxLevel)
        {
            m_logLevel = level;
            return true;
        }
        return false;
    }

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
    int LogStream::getLevel() const
    {
        return m_logLevel;
    }

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
    void LogStream::enableLock()
    {
        m_lockEnable = true;
    }


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
    void LogStream::disableLock()
    {
        m_lockEnable = false;
    }

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
    void LogStream::flush()
    {
        if (ostream_type)
        {
            ostream_type->flush();
        }
    }

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
    LogStream::LogStream(const LogStream& x)
    {
        this->m_minLevel = x.m_minLevel;
        this->m_maxLevel = x.m_maxLevel;
        this->m_logLevel = x.m_logLevel;
        this->ostream_type = x.ostream_type;
        this->m_lockEnable = x.m_lockEnable;
    }

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
    LogStream& LogStream::operator=(const LogStream& x)
    {
        this->m_minLevel = x.m_minLevel;
        this->m_maxLevel = x.m_maxLevel;
        this->m_logLevel = x.m_logLevel;
        this->ostream_type = x.ostream_type;
        this->m_lockEnable = x.m_lockEnable;
        return *this;
    }

};  // namespace coil
