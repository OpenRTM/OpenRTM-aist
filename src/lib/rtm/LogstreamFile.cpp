// -*- C++ -*-
/*!
 * @file LogstreamBase.h
 * @brief Logger stream buffer base class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */
#include <algorithm>

#include <rtm/LogstreamBase.h>
#include <rtm/LogstreamFile.h>
#include <coil/stringutil.h>
#include <rtm/Manager.h>

namespace RTC
{
  coil::vstring LogstreamFile::s_files;

  /*!
   * @if jp
   *
   * @brief ログメッセージのヘッダーの設定
   *
   * ログにログレベル、名前、時間、エスケープシケーンスを設定する。
   *
   * @param level レベル
   * @param name 名前
   * @param date 時間
   * @param es_enable エスケープシケーンスの有無
   *
   *
   * @else
   *
   * @brief
   *
   * @param level log level
   * @param name log name
   * @param date time
   * @param es_enable enable escape sequence
   *
   *
   * @endif
   */
  void FileStreamBase::header(int level, std::string name, std::string date, bool es_enable)
  {
      const char* color[] =
      {
        "\x1b[0m",          // SLILENT  (none)
        "\x1b[0m\x1b[31m",  // FATAL    (red)
        "\x1b[0m\x1b[35m",  // ERROR    (magenta)
        "\x1b[0m\x1b[33m",  // WARN     (yellow)
        "\x1b[0m\x1b[34m",  // INFO     (blue)
        "\x1b[0m\x1b[32m",  // DEBUG    (green)
        "\x1b[0m\x1b[36m",  // TRACE    (cyan)
        "\x1b[0m\x1b[39m",  // VERBOSE  (default)
        "\x1b[0m\x1b[37m"   // PARANOID (white)
      };
      if (es_enable)
      {
          *m_stream << color[level];
      }
      *m_stream << date << " " << Logger::getLevelString(level) << ": " << name << ": ";
      if (es_enable)
      {
          *m_stream << "\x1b[0m";
      }
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
  void FileStreamBase::flush()
  {
      *m_stream << std::flush;
  }

    /*!
  * @if jp
  *
  * @brief エスケープシーケンスの有効にする
  *
  *
  * @else
  *
  * @brief
  *
  * 
  *
  *
  * @endif
  */
  void FileStreamBase::enableEscapeSequence()
  {
	  m_esEnable = true;
  }

  /*!
  * @if jp
  *
  * @brief エスケープシーケンスの無効にする
  *
  *
  * @else
  *
  * @brief
  *
  * 
  *
  *
  * @endif
  */
  void FileStreamBase::disableEscapeSequence()
  {
	  m_esEnable = false;
  }

  /*!
   * @if jp
   *
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   *
   * @else
   *
   * @brief Constructor
   *
   * Constructor
   *
   *
   * @endif
   */
  StdoutStream::StdoutStream()
  {
      coil::Properties& prop(Manager::instance().getConfig());
      coil::toBool(prop["logger.escape_sequence_enable"], "YES", "NO", false) ?
          enableEscapeSequence() : disableEscapeSequence();

      m_stream = new std::basic_ostream<char>(std::cout.rdbuf());
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
  StdoutStream::~StdoutStream()
  {
      delete m_stream;
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
  void StdoutStream::write(int level, const std::string &name, const std::string &date, const std::string &mes)
  {
      header(level, name, date, m_esEnable);
      *m_stream << mes << std::endl;
  }


  /*!
   * @if jp
   *
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   *
   * @else
   *
   * @brief Constructor
   *
   * Constructor
   *
   *
   * @endif
   */
  StderrStream::StderrStream()
  {
      coil::Properties& prop(Manager::instance().getConfig());
      coil::toBool(prop["logger.escape_sequence_enable"], "YES", "NO", false) ?
          enableEscapeSequence() : disableEscapeSequence();
      m_stream = new std::basic_ostream<char>(std::cerr.rdbuf());
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
  StderrStream::~StderrStream()
  {
      delete m_stream;
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
  void StderrStream::write(int level, const std::string &name, const std::string &date, const std::string &mes)
  {
      header(level, name, date, m_esEnable);
      *m_stream << mes << std::endl;
  }

  /*!
   * @if jp
   *
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   * @param filename ファイル名
   *
   *
   * @else
   *
   * @brief Constructor
   *
   * Constructor
   *
   * @param filename
   *
   *
   * @endif
   */
  FileStream::FileStream(std::string filename)
  {
      coil::Properties& prop(Manager::instance().getConfig());
      coil::toBool(prop["logger.escape_sequence_enable"], "YES", "NO", false) ?
          enableEscapeSequence() : disableEscapeSequence();

      m_fileout.open(filename, std::ios::out | std::ios::app);
      m_stream = new std::basic_ostream<char>(&m_fileout);
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
  FileStream::~FileStream()
  {
      delete m_stream;
  }

  /*!
   * @if jp
   *
   * @brief ファイルを開くことに成功したかの確認
   *
   * @return 成功(True)
   *
   * @else
   *
   * @brief
   *
   *
   *
   * @endif
   */
  bool FileStream::is_open()
  {
      return m_fileout.is_open();
  }

  /*!
   * @if jp
   *
   * @brief ファイルを閉じる
   *
   *
   * @else
   *
   * @brief close file
   *
   *
   *
   * @endif
   */
  void FileStream::close()
  {
      if (m_fileout.is_open())
      {
          m_fileout.close();
      }
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
  void FileStream::write(int level, const std::string &name, const std::string &date, const std::string &mes)
  {
      header(level, name, date, m_esEnable);
      *m_stream << mes << std::endl;
  }
  
  LogstreamFile::LogstreamFile()
    : m_stdout(NULL), m_fileout(NULL)
  {
  }

  LogstreamFile::~LogstreamFile()
  {
    s_files.erase(std::remove(s_files.begin(), s_files.end(), m_fileName),
                  s_files.end());
    if (m_fileout != NULL)
      {
        m_fileout->close();
      }
  }

  bool LogstreamFile::init(const coil::Properties& prop)
  {
    coil::vstring files = coil::split(prop["file_name"], ",");

    for (size_t i(0); i < files.size(); ++i)
      {
        std::cout << "#### file: " << files[i] << std::endl;
        if (std::count(s_files.begin(), s_files.end(), files[i]) > 0) { continue; }
        m_fileName = files[i];
        s_files.push_back(files[i]);

        std::string fname(files[i]);
        coil::normalize(fname);
        if (fname == "stdout")
          {
            std::cout << "##### STDOUT!! #####" << std::endl;
            m_stdout = new StdoutStream();
            return true;
          }
        else if (fname == "stderr")
          {
            std::cout << "##### STDOUT!! #####" << std::endl;
            m_stdout = new StderrStream();
            return true;
          }
        else
          {
            std::cout << "##### file #####" << std::endl;
            m_fileout = new FileStream(files[i]);
            
            if (m_fileout->is_open()) { return true; }
          }
      }
    return false;
  }

  coil::LogStreamBuffer* LogstreamFile::getStreamBuffer()
  {
    if (m_stdout != NULL)
      {
        return m_stdout;
      }
    else if (m_fileout != NULL)
      {
        return m_fileout;
      }
    return new StdoutStream();
  }

};

extern "C"
{
  void LogstreamFileInit()
  {
    ::RTC::LogstreamFactory::
      instance().addFactory("file",
                            ::coil::Creator< ::RTC::LogstreamBase,
                                             ::RTC::LogstreamFile>,
                            ::coil::Destructor< ::RTC::LogstreamBase,
                                                ::RTC::LogstreamFile>);
  }
};

