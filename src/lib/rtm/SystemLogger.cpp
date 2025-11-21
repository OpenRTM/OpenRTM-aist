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

#define MAXSIZE 256

namespace RTC
{
  const char* const Logger::m_levelString[] =
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

  const char* const Logger::m_levelOutputString[] =
    {
      " SILENT: ",
      " FATAL: ",
      " ERROR: ",
      " WARNING: ",
      " INFO: ",
      " DEBUG: ",
      " TRACE: ",
      " VERBOSE: ",
      " PARANOID: "
    };

  const char* const Logger::m_levelColor[] =
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

  Logger::Logger(const char* name)
    : ::coil::LogStream(&(Manager::instance().getLogStreamBuf()),
                        RTL_SILENT, RTL_PARANOID, RTL_SILENT),
      m_name(name)
  {
    setLevel(Manager::instance().getLogLevel().c_str());
    coil::Properties& prop(Manager::instance().getConfig());
    if (prop.findNode("logger.date_format") != nullptr)
      {
        setDateFormat(prop["logger.date_format"].c_str());
      }
    if (prop.findNode("logger.clock_type") != nullptr)
      {
        setClockType(prop["logger.clock_type"]);
      }
  }

  Logger::Logger(LogStreamBuf* streambuf)
    : ::coil::LogStream(streambuf,
                        RTL_SILENT, RTL_PARANOID,  RTL_SILENT)
  {
    setDateFormat(m_dateFormat.c_str());
  }

  Logger::~Logger() = default;

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
    std::string fmt(format);
    m_msEnable = std::string::npos != fmt.find("%Q");
    m_usEnable = std::string::npos != fmt.find("%q");
    if (m_msEnable){ fmt = coil::replaceString(std::move(fmt), "%Q", "#m#"); }
    if (m_usEnable){ fmt = coil::replaceString(std::move(fmt), "%q", "#u#"); }
    m_dateFormat = std::move(fmt);
  }

  void Logger::setClockType(const std::string& clocktype)
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
  std::string Logger::getDate()
  {
    char buf[MAXSIZE];
    auto tm = m_clock->gettime();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(tm);

    time_t timer = sec.count();
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    struct tm date;
    errno_t error = gmtime_s(&date, &timer);
    if (error == EOVERFLOW)
    {
        return std::string();
    }
    strftime(buf, sizeof(buf), m_dateFormat.c_str(), &date);
#else
    struct tm* date;
    date = gmtime(&timer);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    // The format parameter is not literal, as it allows the user to change the format.
    // Therefore, we have controlled -Wformat-nonliteral with pragma.
    strftime(buf, sizeof(buf), m_dateFormat.c_str(), date);
#pragma GCC diagnostic pop
#endif

    std::string fmt(buf);
    if (m_msEnable)
      {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tm - sec);
        std::stringstream msec("");
        msec << std::setfill('0') << std::setw(3) << ms.count();
        fmt = coil::replaceString(std::move(fmt), "#m#", msec.str());
      }
    if (m_usEnable)
      {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(tm - sec);
        std::stringstream usec("");
        usec << std::setfill('0') << std::setw(6) << us.count();
        fmt = coil::replaceString(std::move(fmt), "#u#", usec.str());
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
      if (ostream_type != nullptr)
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
      if (ostream_type != nullptr)
      {
          const std::vector<std::string>& vec(prop);
          for (const auto & str : vec)
          {
              ostream_type->write(level, m_name, getDate(), str);
          }
      }
  }

} // namespace RTC
