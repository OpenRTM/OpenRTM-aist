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
  void FileStreamBase::header(int level, const std::string &name, const std::string &date, bool es_enable)
  {
      if (es_enable)
      {
          *m_stream << Logger::getLevelColor(level);
      }
      *m_stream << date << Logger::getLevelOutputString(level) << name << ": ";
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
  void FileStreamBase::write(int level, const std::string &name, const std::string &date, const std::string &mes)
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
  StdoutStream::StdoutStream()
  {
      

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
  FileStream::FileStream(const std::string& filename)
  {
      

      m_fileout.open(filename.c_str(), std::ios::out | std::ios::app);
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

  
  
  LogstreamFile::LogstreamFile()
  {
  }

  LogstreamFile::~LogstreamFile()
  {
    s_files.erase(std::remove(s_files.begin(), s_files.end(), m_fileName),
                  s_files.end());
    if (m_fileout != nullptr)
      {
        m_fileout->close();
      }
  }

  bool LogstreamFile::init(const coil::Properties& prop)
  {

    bool escape_sequence = coil::toBool(prop["escape_sequence_enable"], "YES", "NO", false);

    coil::vstring files = coil::split(prop["file_name"], ",");

    for (auto & file : files)
      {
        if (std::count(s_files.begin(), s_files.end(), file) > 0) { continue; }
        m_fileName = file;
        s_files.emplace_back(file);

        std::string fname{coil::normalize(file)};
        if (fname == "stdout")
          {
            std::cout << "#### file: " << file << std::endl;
            std::cout << "##### STDOUT!! #####" << std::endl;
            m_stdout = new StdoutStream();
            if (escape_sequence)
            {
                m_stdout->enableEscapeSequence();
            }
            else
            {
                m_stdout->disableEscapeSequence();
            }
            return true;
          }
        else if (fname == "stderr")
          {
            std::cout << "#### file: " << file << std::endl;
            std::cout << "##### STDOUT!! #####" << std::endl;
            m_stdout = new StderrStream();
            if (escape_sequence)
            {
                m_stdout->enableEscapeSequence();
            }
            else
            {
                m_stdout->disableEscapeSequence();
            }
            return true;
          }
        else
          {
            m_fileout = new FileStream(file);
            if (escape_sequence)
            {
                m_fileout->enableEscapeSequence();
            }
            else
            {
                m_fileout->disableEscapeSequence();
            }
            
            if (m_fileout->is_open()) { return true; }
          }
      }
    return false;
  }

  coil::LogStreamBuffer* LogstreamFile::getStreamBuffer()
  {
    if (m_stdout != nullptr)
      {
        return m_stdout;
      }
    else if (m_fileout != nullptr)
      {
        return m_fileout;
      }
    return new StdoutStream();
  }

} // namespace RTC

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
}

