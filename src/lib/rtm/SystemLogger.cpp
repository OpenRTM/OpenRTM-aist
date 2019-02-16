// -*- C++ -*-
/*!
 * @file SystemLogger.cpp
 * @brief RT component logger class
 * @date $Date: 2007-07-20 16:10:32 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2003-2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: SystemLogger.cpp 845 2008-09-25 11:10:40Z n-ando $
 *
 */
#include <rtm/SystemLogger.h>
#include <rtm/Manager.h>

#include <sstream>
#include <iomanip>

#if defined(_MSC_VER)
#define snprintf _snprintf
#endif

#define MAXSIZE 256

namespace RTC
{
  const char* Logger::m_levelString[] =
    {
      "SILENT",
      "FATAL",
      "ERROR",
      "WARNING",
      "INFO",
      "DEBUG",
      "TRACE",
      "VERBOSE",
      "PARANOID"
    };

  Logger::Logger(const char* name)
    : ::coil::LogStream(&(Manager::instance().getLogStreamBuf()),
                        RTL_SILENT, RTL_PARANOID, RTL_SILENT),
      m_name(name),
      m_dateFormat("%b %d %H:%M:%S.%Q"),
      m_clock(&coil::ClockManager::instance().getClock("system")),
      m_msEnable(0), m_usEnable(0)
  {
    setLevel(Manager::instance().getLogLevel().c_str());
    coil::Properties& prop(Manager::instance().getConfig());
    if (prop.findNode("logger.date_format") != NULL)
      {
        setDateFormat(prop["logger.date_format"].c_str());
      }
    if (prop.findNode("logger.clock_type") != NULL)
      {
        setClockType(prop["logger.clock_type"]);
      }
	
  }

  Logger::Logger(LogStreamBuf* streambuf)
    : ::coil::LogStream(streambuf,
                        RTL_SILENT, RTL_PARANOID,  RTL_SILENT),
      m_name("unknown"),
      m_dateFormat("%b %d %H:%M:%S.%Q"),
      m_clock(&coil::ClockManager::instance().getClock("system")),
      m_msEnable(0), m_usEnable(0)
  {
    setDateFormat(m_dateFormat.c_str());
  }

  Logger::~Logger(void)
  {
  }

  /*!
   * @if jp
   * @brief ログレベルを文字列で設定する
   * @else
   * @brief Set log level by string
   * @endif
   */
  bool Logger::setLevel(const char* level)
  {
    return coil::LogStream::setLevel(strToLevel(level));
  }

  /*!
   * @if jp
   * @brief ヘッダに付加する日時フォーマットを指定する。
   * @else
   * @brief Set date/time format for adding the header
   * @endif
   */
  void Logger::setDateFormat(const char* format)
  {
    m_dateFormat = std::string(format);
    m_msEnable = coil::replaceString(m_dateFormat, "%Q", "#m#");
    m_usEnable = coil::replaceString(m_dateFormat, "%q", "#u#");
  }

  void Logger::setClockType(std::string clocktype)
  {
    m_clock = &coil::ClockManager::instance().getClock(clocktype);
  }
  /*!
   * @if jp
   * @brief ヘッダの日時の後に付加する文字列を設定する。
   * @else
   * @brief Set suffix of date/time string of header.
   * @endif
   */
  void Logger::setName(const char* name)
  {
    m_name = name;
  }



  /*!
   * @if jp
   * @brief フォーマットされた現在日時文字列を取得する。
   * @else
   * @brief Get the current formatted date/time string
   * @endif
   */
  std::string Logger::getDate(void)
  {
    char buf[MAXSIZE];
    coil::TimeValue tm(m_clock->gettime());

    time_t timer;
    struct tm* date;

    timer = tm.sec();
    date = gmtime(&timer);

    strftime(buf, sizeof(buf), m_dateFormat.c_str(), date);
    std::string fmt(buf);

    if (m_msEnable > 0)
      {
        std::stringstream msec("");
        msec << std::setfill('0') << std::setw(3);
        msec << static_cast<int>(tm.usec() / 1000);
        coil::replaceString(fmt, "#m#", msec.str());
      }
    if (m_usEnable > 0)
      {
        std::stringstream usec("");
        usec << std::setfill('0') << std::setw(3);
        usec << static_cast<int>(tm.usec() - ((tm.usec() / 1000) * 1000));
        coil::replaceString(fmt, "#u#", usec.str());
      }

    return fmt;
  }

  /*!
   * @if jp
   * @brief ログレベル設定
   * @else
   * @brief Set the log level
   * @endif
   */
  int Logger::strToLevel(const char* level)
  {
    std::string lv(level);
    if      (lv == "SILENT")
      return RTL_SILENT;
    else if (lv == "FATAL")
      return RTL_FATAL;
    else if (lv == "ERROR")
      return RTL_ERROR;
    else if (lv == "WARN")
      return RTL_WARN;
    else if (lv == "INFO")
      return RTL_INFO;
    else if (lv == "DEBUG")
      return RTL_DEBUG;
    else if (lv == "TRACE")
      return RTL_TRACE;
    else if (lv == "VERBOSE")
      return RTL_VERBOSE;
    else if (lv == "PARANOID")
      return RTL_PARANOID;
    else
      return RTL_SILENT;
  }



  /*!
   * @if jp
   *
   * @brief ログの出力
   *
   * 指定したメッセージのログを出力する
   *
   * @param level ログレベル
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
   * @param mes message
   *
   * @endif
   */
  void Logger::write(int level, const std::string &mes)
  {
      if (ostream_type)
      {
          std::string date = getDate();
          ostream_type->write(level, m_name, date, mes);
      }
  }
  /*!
   * @if jp
   *
   * @brief ログの出力
   *
   * 指定したプロパティを文字列に変換してログに出力する
   *
   * @param level ログレベル
   * @param prop プロパティ
   *
   *
   * @else
   *
   * @brief log output
   *
   * 
   *
   * @param level log level
   * @param prop properties
   *
   * @endif
   */
  void Logger::write(int level, const coil::Properties &prop)
  {
      if (ostream_type)
      {
          std::vector<std::string> vec(prop);
          for (std::vector<std::string>::iterator itr = vec.begin(); itr != vec.end(); ++itr)
          {
              ostream_type->write(level, m_name, getDate(), *itr);
          }
      }
  }

};  // namespace RTC
