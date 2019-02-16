// -*- C++ -*-
/*!
 * @file SystemLogger.h
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
 * $Id$
 *
 */

#ifndef RTC_SYSTEMLOGGER_H
#define RTC_SYSTEMLOGGER_H

#include <rtm/config_rtc.h>

#include <coil/Time.h>
#include <coil/ClockManager.h>
#include <coil/Logger.h>
#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <coil/stringutil.h>
#include <coil/Properties.h>

#include <string>

namespace RTC
{
  typedef ::coil::LogStreamBuffer LogStreamBuf;
  typedef ::coil::LogStream LogStream;

  /*!
   * @if jp
   * @class Logger
   * @brief Logger クラス
   *
   * - ログ出力をシリアライズしかつ分配するバッファクラス
   * - ログをフォーマットするフォーマットクラス
   * で構成されるロガークラス
   *
   * - バッファクラス
   *  - マルチスレッド書き込みに対してシリアライズしてバッファリングする
   *  - 複数の出力先にログを出力できる
   *  - 出力先の例としては、ファイル、標準出力、リモートのログサーバ等
   *  - バッファに対してaddStreamで出力先を追加できる
   * - フォーマットクラス
   *  - ログレベルを指定して出力できる
   *  - 書式は、[時間] [ログレベル] [サフィックス] [メッセージ]
   *  - [時間] [ログレベル] [サフィックス]は自動付加
   *  - [サフィックス] を指定できる関数を用意
   *  - ログレベルは以下のとおり
   *   - RTL_SILENT
   *   - RTL_FATAL
   *   - RTL_ERROR
   *   - RTL_WARN
   *   - RTL_INFO
   *   - RTL_DEBUG
   *   - RTL_TRACE
   *   - RTL_VERBOSE
   *   - RTL_PARANOID
   *  - このフォーマットオブジェクトに対するロック・アンロック機能
   *
   * @else
   * @class Logger
   * @brief Logger class
   *
   * This class is composed of the buffer class and the format class.
   * - The buffer class
   *  - The buffer class serializes to the MultiThreading writing and
   *    does buffering.
   *  - The buffer class can output the log at two or more output destination.
   *    As the example of the output destination, filing, the standard,
   *    the log server etc.
   *  - The output destination can be added to the buffer with addStream.
   * - The format class
   *  - The format class can output it by specifying the log level.
   *  - The format is [Time] [Loglevel] [Ssuffix] [Message].
   *  - [Time] [Loglevel] [Suffix] are added by the automatic operation.
   *  - [Suffix] can be specified.
   *  - Loglevel
   *   - RTL_SILENT
   *   - RTL_FATAL
   *   - RTL_ERROR
   *   - RTL_WARN
   *   - RTL_INFO
   *   - RTL_DEBUG
   *   - RTL_TRACE
   *   - RTL_VERBOSE
   *   - RTL_PARANOID
   *  - The format class can be locked/unlocked to this format object.
   *
   * @endif
   */
  class Logger
    : public coil::LogStream
  {
  public:
    enum
      {               // No: Write out messages include the following.
        RTL_SILENT,   // 0: ()
        RTL_FATAL,    // 1: (FATAL)
        RTL_ERROR,    // 2: (FATAL, ERROR)
        RTL_WARN,     // 3: (FATAL, ERROR, WARN)
        RTL_INFO,     // 4: (FATAL, ERROR, WARN, INFO)
        RTL_DEBUG,    // 5: (FATAL, ERROR, WARN, INFO, DEBUG)
        RTL_TRACE,    // 6: (FATAL, ERROR, WARN, INFO, DEBUG, TRACE)
        RTL_VERBOSE,  // 7: (FATAL, ERROR, WARN, INFO, DEBUG, TRACE, VERBOSE)
        RTL_PARANOID  // 8: (FATAL, ERROR, WARN, INFO,
                      //     DEBUG, TRACE, VERBOSE, PARA)
      };

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param name ヘッダの日時の後に付加する文字列
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param name suffix of date/time string of header.
     *
     * @endif
     */
    explicit Logger(const char* name = "");
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param streambuf LogStream オブジェクト
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param streambuf LogStream object
     *
     * @endif
     */
    explicit Logger(LogStreamBuf* streambuf);
    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * @else
     *
     * @brief Virtual destructor
     *
     * @endif
     */
    virtual ~Logger(void);

    /*!
     * @if jp
     *
     * @brief ログレベルを文字列で設定する
     *
     * @param level ログレベル
     *
     * @else
     *
     * @brief Set log level by string
     *
     * @param level log level
     *
     * @endif
     */
    bool setLevel(const char* level);

    /*!
     * @if jp
     *
     * @brief ヘッダに付加する日時フォーマットを指定する。
     *
     * フォーマット指定文字列は以下のとおり。
     * <pre>
     * @%a abbreviated weekday name
     * @%A full weekday name
     * @%b abbreviated month name
     * @%B full month name
     * @%c the standard date and time string
     * @%d day of the month, as a number (1-31)
     * @%H hour, 24 hour format (0-23)
     * @%I hour, 12 hour format (1-12)
     * @%j day of the year, as a number (1-366)
     * @%m month as a number (1-12).
     *    Note: some versions of Microsoft Visual C++ may use values that range
     *    from 0-11.
     * @%M minute as a number (0-59)
     * @%p locale's equivalent of AM or PM
     * @%Q millisecond as a number (0-999) from ver 1.1
     * @%q microsecond as a number (0-999) from ver 1.1
     * @%S second as a number (0-59)
     * @%U week of the year, sunday as the first day
     * @%w weekday as a decimal (0-6, sunday=0)
     * @%W week of the year, monday as the first day
     * @%x standard date string
     * @%X standard time string
     * @%y year in decimal, without the century (0-99)
     * @%Y year in decimal, with the century
     * @%Z time zone name
     * %% a percent sign
     * </pre>
     *
     * @param fmt 日時フォーマット
     *
     * @else
     *
     * @brief Set date/time format for adding the header
     *
     * The format specification string is as follows:
     * <pre>
     * @%a abbreviated weekday name
     * @%A full weekday name
     * @%b abbreviated month name
     * @%B full month name
     * @%c the standard date and time string
     * @%d day of the month, as a number (1-31)
     * @%H hour, 24 hour format (0-23)
     * @%I hour, 12 hour format (1-12)
     * @%j day of the year, as a number (1-366)
     * @%m month as a number (1-12).
     *    Note: some versions of Microsoft Visual C++ may use values that range
     *    from 0-11.
     * @%M minute as a number (0-59)
     * @%p locale's equivalent of AM or PM
     * @%Q millisecond as a number (0-999) from ver 1.1
     * @%q microsecond as a number (0-999) from ver 1.1
     * @%S second as a number (0-59)
     * @%U week of the year, sunday as the first day
     * @%w weekday as a decimal (0-6, sunday=0)
     * @%W week of the year, monday as the first day
     * @%x standard date string
     * @%X standard time string
     * @%y year in decimal, without the century (0-99)
     * @%Y year in decimal, with the century
     * @%Z time zone name
     * %% a percent sign
     * </pre>
     *
     * @param fmt Datetime format
     *
     * @endif
     */
    void setDateFormat(const char* format);

    /*!
     * @if jp
     *
     * @brief ログ記録時に使用するクロックを指定する
     *
     * ログ記録時に時刻を取得するためのクロックの種類を指定することができる。
     * - system: システムクロック。デフォルト
     * - logical: 論理時間クロック。
     * - adjusted: 調整済みクロック。
     *
     * 論理時間クロックについては
     * <pre>
     * coil::ClockManager::instance().getClock("logical").settime()
     * </pre>
     * で時刻を設定する必要がある。
     *
     * @param clocktype 上述のクロックタイプ
     *
     * @else
     *
     * @brief Specifying clock type to be used for logging
     *
     * This function sets a clock type getting time when it is used
     * for logging. Available clock types are,
     * - system: System clock. Default option.
     * - logical: Logical time clock.
     * - adjusted: Adjusted clock.
     *
     * To use logical time clock, call and set time by the following
     * function in somewhere.
     * <pre>
     * coil::ClockManager::instance().getClock("logical").settime()
     * </pre>
     *
     * @param clocktype A clock type above mentioned
     *
     * @endif
     */
    void setClockType(std::string clocktype);

    /*!
     * @if jp
     *
     * @brief ヘッダの日時の後に付加する文字列を設定する。
     *
     * ヘッダの日時の後に付加する接頭語文字列を設定する。
     *
     * @param suffix 接頭語文字列
     *
     * @else
     *
     * @brief Set suffix of date/time string of header.
     *
     * Set the suffix string added after the datatime of the header.
     *
     * @param suffix Suffix string
     *
     * @endif
     */
    void setName(const char* name);


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
    virtual void write(int level, const std::string &mes);

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
    void write(int level, const coil::Properties &prop);

    /*!
     * @if jp
     *
     * @brief ログレベルを数値から文字列に変換
     *
     *
     * @param level ログレベル
     * 
     * @return 文字列
     *
     * @else
     *
     * @brief 
     *
     * 
     *
     * @param level log level
     * 
     * @return string
     *
     * @endif
     */
    static std::string getLevelString(int level)
    {
        return m_levelString[level];
    }

  protected:


    /*!
     * @if jp
     * @brief フォーマットされた現在日時文字列を取得する。
     * 指定された書式で記述した現在日時を取得する。
     *
     * @return 書式指定現在日時
     *
     * @else
     * @brief Get the current formatted date/time string
     * Get the current datetime described by specified format.
     *
     * @return Format specification current datetime
     *
     * @endif
     */
    std::string getDate(void);

    /*!
     * @if jp
     * @brief ログレベル設定
     * 与えられた文字列に対応したログレベルを設定する。
     *
     * @param lv ログレベル文字列
     * @return 設定したログレベル
     *
     * @else
     * @brief Set the log level
     * Set the log level corresponding to the given string.
     *
     * @param lv Log level string
     * @return The set log level
     *
     * @endif
     */
    int strToLevel(const char* level);



  private:
    std::string m_name;
    std::string m_dateFormat;
    coil::IClock* m_clock;
    static const char* m_levelString[];
    int m_msEnable;
    int m_usEnable;
  };


#ifndef NO_LOGGING
/*!
 * @if jp
 *
 * @brief 汎用ログ出力マクロ
 *
 * ログレベルおよび出力フォーマット文字列を引数としてとる。
 *
 * @else
 *
 * @brief General-purpose log output macro
 *
 * Lock log level and output format string as arguments.
 *
 * @endif
 */
#define RTC_LOG(LV, fmt)                                    \
  if (rtclog.isValid(LV))                                   \
    {                                                       \
      std::string str = ::coil::sprintf fmt;                \
      rtclog.lock();                                        \
      rtclog.write(LV, str);                                \
      rtclog.unlock();                                      \
    }

#define RTC_LOG_STR(LV, str)                                \
  if (rtclog.isValid(LV))                                   \
    {                                                       \
      rtclog.lock();                                        \
      rtclog.write(LV, str);                                \
      rtclog.unlock();                                      \
    }

   /*!
   * @if jp
   *
   * @brief エラーログ出力マクロ。
   *
   * エラーレベルのログ出力マクロ。<BR>ログレベルが
   * FATAL, ERROR, WARN, INFO, DEBUG, TRACE, VERBOSE, PARANOID
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Error log output macro
   *
   * This is a log output macro of the error level.
   * If log levels are FATAL, ERROR, WARN, INFO, NORMAL, DEBUG, TRACE,
   * VERBOSE or PARANOID, message will be output to log.
   *
   * @endif
   */
#define RTC_FATAL(fmt) RTC_LOG(::RTC::Logger::RTL_FATAL, fmt)
#define RTC_FATAL_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_FATAL, str)

  /*!
   * @if jp
   *
   * @brief エラーログ出力マクロ。
   *
   * エラーレベルのログ出力マクロ。<BR>ログレベルが
   * ERROR, WARN, INFO, DEBUG, TRACE, VERBOSE, PARANOID
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Error log output macro
   *
   * This is a log output macro of the error level.
   * If log levels are ERROR, WARN, INFO, DEBUG, TRACE,
   * VERBOSE and PARANOID, message will be output to log.
   *
   * @endif
   */
#define RTC_ERROR(fmt) RTC_LOG(::RTC::Logger::RTL_ERROR, fmt)
#define RTC_ERROR_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_ERROR, str)

  /*!
   * @if jp
   *
   * @brief ワーニングログ出力マクロ。
   *
   * ワーニングレベルのログ出力マクロ。<BR>ログレベルが
   * ( WARN, INFO, DEBUG, TRACE, VERBOSE, PARANOID )
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Warning log output macro
   *
   * If log levels are
   * ( WARN, INFO, DEBUG, TRACE, VERBOSE, PARANOID ),
   * message will be output to log.
   *
   * @endif
   */
#define RTC_WARN(fmt) RTC_LOG(::RTC::Logger::RTL_WARN, fmt)
#define RTC_WARN_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_WARN, str)

  /*!
   * @if jp
   *
   * @brief インフォログ出力マクロ。
   *
   * インフォレベルのログ出力マクロ。<BR>ログレベルが
   * ( INFO, DEBUG, TRACE, VERBOSE, PARANOID )
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Information level log output macro
   *
   *  If log levels are
   * ( INFO, DEBUG, TRACE, VERBOSE, PARANOID ),
   * message will be output to log.
   *
   * @endif
   */
#define RTC_INFO(fmt) RTC_LOG(::RTC::Logger::RTL_INFO, fmt)
#define RTC_INFO_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_INFO, str)

  /*!
   * @if jp
   *
   * @brief デバッグログ出力マクロ。
   *
   * デバッグレベルのログ出力マクロ。<BR>ログレベルが
   * ( DEBUG, TRACE, VERBOSE, PARANOID )
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Debug level log output macro.
   *
   * If log levels are
   * ( DEBUG, TRACE, VERBOSE, PARANOID ),
   * message will be output to log.
   *
   * @endif
   */
#define RTC_DEBUG(fmt) RTC_LOG(::RTC::Logger::RTL_DEBUG, fmt)
#define RTC_DEBUG_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_DEBUG, str)

  /*!
   * @if jp
   *
   * @brief トレースログ出力マクロ。
   *
   * トレースレベルのログ出力マクロ。<BR>ログレベルが
   * ( TRACE, VERBOSE, PARANOID )
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Trace level log output macro.
   *
   * If log levels are
   * ( TRACE, VERBOSE, PARANOID ),
   * message will be output to log.
   *
   * @endif
   */
#define RTC_TRACE(fmt) RTC_LOG(::RTC::Logger::RTL_TRACE, fmt)
#define RTC_TRACE_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_TRACE, str)


  /*!
   * @if jp
   *
   * @brief ベルボーズログ出力マクロ。
   *
   * ベルボーズレベルのログ出力マクロ。<BR>ログレベルが
   * ( VERBOSE, PARANOID )
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Verbose level log output macro.
   *
   * If log levels are
   * ( VERBOSE, PARANOID ),
   * message will be output to log.
   *
   * @endif
   */
#define RTC_VERBOSE(fmt) RTC_LOG(::RTC::Logger::RTL_VERBOSE, fmt)
#define RTC_VERBOSE_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_VERBOSE, str)

  /*!
   * @if jp
   *
   * @brief パラノイドログ出力マクロ。
   *
   * パラノイドレベルのログ出力マクロ。<BR>ログレベルが
   * ( PARANOID )
   * の場合にログ出力される。
   *
   * @else
   *
   * @brief Paranoid level log output macro.
   *
   * If log levels are
   * ( PARANOID ),
   * message will be output to log.
   *
   * @endif
   */
#define RTC_PARANOID(fmt) RTC_LOG(::RTC::Logger::RTL_PARANOID, fmt)
#define RTC_PARANOID_STR(str) RTC_LOG_STR(::RTC::Logger::RTL_PARANOID, str)

#else
#define RTC_FATAL(fmt)
#define RTC_FATAL_STR(str)
#define RTC_ERROR(fmt)
#define RTC_ERROR_STR(str)
#define RTC_WARN(fmt)
#define RTC_WARN_STR(str)
#define RTC_INFO(fmt)
#define RTC_INFO_STR(str)
#define RTC_DEBUG(fmt)
#define RTC_DEBUG_STR(str)
#define RTC_TRACE(fmt)
#define RTC_TRACE_STR(str)
#define RTC_VERBOSE(fmt)
#define RTC_VERBOSE_STR(str)
#define RTC_PARANOID(fmt)
#define RTC_PARANOID_STR(str)
#define RTC_FATAL(fmt)
#define RTC_FATAL_STR(str)
#endif

};  // namespace RTC

#endif  // RTC_SYSTEMLOGGER_H
