// -*- C++ -*-
/*!
 * @file FluentBit.h
 * @brief File logger stream class
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

#ifndef RTC_LOGGER_FLUENTBIT_H
#define RTC_LOGGER_FLUENTBIT_H

#include <string>
#include <fstream>
#include <iostream>
#include <fluent-bit.h>

#include <coil/stringutil.h>
#include <rtm/LogstreamBase.h>

#ifndef LINE_MAX
#define LINE_MAX  1024
#endif

#ifndef BUFFER_LEN
#define BUFFER_LEN LINE_MAX
#endif

namespace RTC
{
  /*!
   * @if jp
   * @class FluentbitStream
   *
   * このクラスは ログ出力を fluent-bit へ送信するための logstream_buf
   * クラスである。FluentBit クラスにてインスタンス化され、
   * SystemLogger の logstream に追加される。
   *
   * @else
   * @class FluentbitStream
   *
   * This class is a logstream_buf class to send log output to
   * fluent-bit. This is instantiate in the FluentBit class and it is
   * added to SystemLogger's logstreams.
   *
   * @endif
   */
  class FluentBitStream
    : public coil::LogStreamBuffer
  {
  public:
    FluentBitStream();

    virtual ~FluentBitStream();

    bool init(const coil::Properties& prop);

    bool createOutputStream(const coil::Properties& prop);

    bool createInputStream(const coil::Properties& prop);

    int setServiceOption(const coil::Properties& prop);

    virtual void write(int level, const std::string &name, const std::string &date, const std::string &mes);

  protected:
    std::streamsize pushLogger(int level, const std::string &name, const std::string &date, const char* mes);

  private:
    char m_buf[BUFFER_LEN];
    size_t m_pos;

    typedef int FlbHandler;
    FlbHandler m_fibInLib;

    std::vector<FlbHandler> m_flbIn;
    std::vector<FlbHandler> m_flbOut;

    // Static variables
    /*!
     * @brief Fluent-bit context
     */
    static flb_ctx_t* s_flbContext;
    /*!
     * @brief Fluent-bit instance count
     */
    static int s_instance;
  };

  /*!
   * @if jp
   * @class FluentBit
   *
   * このクラスは ログ出力を fluent-bit へ送信するためのログストリーム
   * 用プラグインクラスである。
   *
   * fluent-bit はログ収集・分配ミドルウェア fluentd のC言語実装である。
   * fluent-bit/fluentd は様々なプロトコルでログの受信、フィルタリング、
   * 送信を行うことができる。このクラスは、ログストリームのプラグインを
   * 構成する FluentBit クラスの std::stream_buff クラスのサブクラスで
   * あり、実際の FluentBit へのログの出力部分を担うクラスである。
   *
   * デフォルトでは、OpenRTMのログ出力を入力 (input) として取り、
   * rtc.conf に設定された出力 (output) に対してログを送出することがで
   * きる。input も fluent-bit で利用できるプラグインを rtc.conf から有
   * 効にすることができ、他の fluentd/fluent-bit からのログ出力を受信し
   * たり、CPUやメモリ使用量などをログ入力として取得することも可能であ
   * る。実質的に、コマンドラインプログラムの fluent-bit とほぼ同じこと
   * が実現可能になっている。
   *
   * オプションは、基本的には fluent-bit の key-value 型のプロパティを
   * rtc.conf で指定することですべてのプラグインを利用できるが、以下に、
   * 代表的なプラグインとそのオプションを示す。
   *   
   * * Available Output plugins
   * - reference: http://fluentbit.io/documentation/0.8/output/index.html
   *
   * ** forward: fluentd forwarding
   * ______________________________________________________________________
   * |  key   |                  Description                 |   Default  |
   * ----------------------------------------------------------------------
   * | host   | Target host where Fluent-Bit  or Fluentd are |  127.0.0.1 |
   * |        | listening for Forward messages.              |            |
   * ----------------------------------------------------------------------
   * | port   | TCP port of the target service.              |      24224 |
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Example:
   * logger.logstream.fluentd.output0.plugin: forward
   * logger.logstream.fluentd.output0.tag:    <tagname>
   * logger.logstream.fluentd.output0.host:   <fluentd_hostname>
   * logger.logstream.fluentd.output0.port:   <fluentd_port>
   *
   * ** es: Elasticsearch
   * ______________________________________________________________________
   * |  key   |                  Description                 |   Default  |
   * ----------------------------------------------------------------------
   * | host   | IP address or hostname of the target         |  127.0.0.1 |
   * |        | Elasticsearch instance.                      |            |
   * ----------------------------------------------------------------------
   * | port   | TCP port of the target Elasticsearch         |       9200 |
   * |        | instance.                                    |            |
   * ----------------------------------------------------------------------
   * | index  | Elastic index.                               | fluentbit  |
   * ----------------------------------------------------------------------
   * | type   | Elastic type.                                | test       |
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * Example:
   * logger.logstream.fluentd.output0.plugin: es
   * logger.logstream.fluentd.output0.tag:    <tagname>
   * logger.logstream.fluentd.output0.host:   <es_hostname>
   * logger.logstream.fluentd.output0.port:   <es_port>
   * logger.logstream.fluentd.output0.index:  <es_index>
   * logger.logstream.fluentd.output0.type:   <es_type>
   *
   * ** http: HTTP POST request in MessagePack format
   * ______________________________________________________________________
   * |   key  |            Description                       |   Default  |
   * ----------------------------------------------------------------------
   * |  Host  | IP address or hostname of the target HTTP    |  127.0.0.1 |
   * |        | Server.                                      |            |
   * ----------------------------------------------------------------------
   * |  Port  | TCP port of the target HTTP Server.          |         80 |
   * ----------------------------------------------------------------------
   * |  Proxy | Specify an HTTP Proxy. The expected format   |            |
   * |        | of this value is http://host:port.           |            |
   * |        | Note that https is not supported yet.        |            |
   * ----------------------------------------------------------------------
   * |  URI   | Specify an optional HTTP URI for the target  |          / |
   * |        | web server, e.g: /something                  |            |
   * ----------------------------------------------------------------------
   * | Format | Specify the data format to be used in the    |    msgpack |
   * |        | HTTP request body, by default it uses        |            |
   * |        | msgpack, optionally it can be set to json.   |            |
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * Example:
   * logger.logstream.fluentd.output0.plugin: http
   * logger.logstream.fluentd.output0.tag:    <tagname>
   * logger.logstream.fluentd.output0.host:   127.0.0.1
   * logger.logstream.fluentd.output0.port:   80
   * logger.logstream.fluentd.output0.proxy:
   * logger.logstream.fluentd.output0.uri:     /openrtm/
   * logger.logstream.fluentd.output0.format:  msgpack
   *
   * ** nats: NATS output plugin
   * ______________________________________________________________________
   * |  key   |                  Description                 |   Default  |
   * ----------------------------------------------------------------------
   * | host   | IP address or hostname of the NATS Server.   |  127.0.0.1 |
   * ----------------------------------------------------------------------
   * | port   | TCP port of the target NATS Server.          |       4222 |
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * Example:
   * logger.logstream.fluentd.output0.plugin: nats
   * logger.logstream.fluentd.output0.tag:    <tagname>
   * logger.logstream.fluentd.output0.host:   <nats_host>
   * logger.logstream.fluentd.output0.port:   <nats_port>
   *
   * * stdout: Standard Output plugin
   *
   * Example:
   * logger.logstream.fluentd.output0.plugin: stdin
   *
   * @else
   * @class FluentbitStream
   *
   * @endif
   */
  class FluentBit
    : public LogstreamBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    FluentBit();

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    virtual ~FluentBit(void);

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * Logstreamクラスの各種設定を行う。実装クラスでは、与えられた
     * Propertiesから必要な情報を取得して各種設定を行う。
     *
     * @param prop 設定情報
     *
     * @else
     *
     * @brief Initializing configuration
     *
     * This operation would be called to configure in initialization.
     * In the concrete class, configuration should be performed
     * getting appropriate information from the given Properties data.
     *
     * @param prop Configuration information
     *
     * @endif
     */
    virtual bool init(const coil::Properties& prop);

    /*!
     * @if jp
     * @brief basic_strembuf へのポインタを返す
     *
     * Loggerで使用する basic_streambuf へのポインタを返す。
     *
     * @return basic_streambuf (coil::LogStreambuf) へのポインタ
     *
     * @else
     *
     * @brief Returns a pointer to the basic_streambuf 
     *
     * This operation would returns a pointer to the basic_streambuf
     * or its subclass that is kept in this class.
     *
     * @return pointer to the basic_streambuf (coil::LogStreambuf)
     *
     * @endif
     */
    virtual coil::LogStreamBuffer* getStreamBuffer();

  protected:
    FluentBitStream m_fbstream;
  };
}; // namespace RTC

extern "C"
{
  void DLL_EXPORT FluentBitInit();
};

#endif // RTC_LOGGER_FLUENTBIT_H