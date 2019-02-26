// -*- C++ -*-
/*!
 * @file LogstreamFile.h
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

#ifndef RTC_LOGSTREAMFILE_H
#define RTC_LOGSTREAMFILE_H

#include <string>
#include <fstream>
#include <iostream>
#include <coil/stringutil.h>
#include <rtm/LogstreamBase.h>

namespace RTC
{
  /*!
   * @if jp
   *
   * @class FileStreamBase
   * @brief ファイル出力のストリーミングバッファ基底クラス
   *
   * @else
   *
   * @class FileStreamBase
   * @brief 
   *
   * @endif
   */
  class FileStreamBase : public coil::LogStreamBuffer
  {
  public:
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
      FileStreamBase(){};
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
      virtual ~FileStreamBase() {};
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
      void header(int level, const std::string &name, const std::string &date, bool es_enable = false);

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
      virtual void flush();
      /*!
       * @if jp
       *
       * @brief エスケープシーケンスを有効にする
       *
       *
       *
       * @else
       *
       * @brief
       *
       *
       *
       *
       *
       * @endif
       */
      void enableEscapeSequence();
      /*!
       * @if jp
       *
       * @brief エスケープシーケンスを無効にする
       *
       *
       *
       * @else
       *
       * @brief
       *
       *
       *
       *
       *
       * @endif
       */
      void disableEscapeSequence();

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
      
  protected:
      std::basic_ostream<char> *m_stream;
      bool m_esEnable;
  };

  /*!
   * @if jp
   *
   * @class StdoutStream
   * @brief 標準出力のストリーミングラッパークラス
   *
   * @else
   *
   * @class StdoutStream
   * @brief 
   *
   * @endif
   */
  class StdoutStream : public FileStreamBase
  {
  public:
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
      StdoutStream();
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
      virtual ~StdoutStream();

      
      
  };

  /*!
   * @if jp
   *
   * @class StderrStream
   * @brief 標準エラー出力のストリーミングラッパークラス
   *
   * @else
   *
   * @class StderrStream
   * @brief 
   *
   * @endif
   */
  class StderrStream : public FileStreamBase
  {
  public:
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
      StderrStream();
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
      virtual ~StderrStream();
      
      

  };

  /*!
   * @if jp
   *
   * @class FileStream
   * @brief ファイル出力のストリーミングラッパークラス
   *
   * @else
   *
   * @class FileStream
   * @brief
   *
   * @endif
   */
  class FileStream : public FileStreamBase
  {
  public:
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
      FileStream(const std::string& filename);
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
      virtual ~FileStream();
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
      bool is_open();
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
      void close();
      
      
  private:
      std::filebuf m_fileout;
  };
  /*!
   * @if jp
   *
   * @class PublisherBase
   *
   * @brief Publisher 基底クラス
   * 
   * データ送出タイミングを管理して送出を駆動するPublisher* の基底クラス。
   * 各種 Publisher はこのクラスを継承して詳細を実装する。
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class PublisherBase
   *
   * @brief Base class of Publisher.
   *
   * This is a base class of Publisher*. This class manages data send timing.
   * Variation of Publisher* which implements details of Publisher inherits
   * this PublisherBase class.
   *
   * @endif
   */
  class LogstreamFile
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
    LogstreamFile();

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
    virtual ~LogstreamFile(void);

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
    static coil::vstring s_files;
    std::string m_fileName;
    FileStreamBase* m_stdout;
    FileStream* m_fileout;
  };
}; // namespace RTC

extern "C"
{
  void DLL_EXPORT LogstreamFileInit();
};

#endif // RTC_LOGSTREAMFILE_H
