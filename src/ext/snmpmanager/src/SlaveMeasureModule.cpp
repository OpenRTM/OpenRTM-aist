// -*- C++ -*-
/*!
 * @file SlaveMeasureModule.cpp
 * @brief snmpmgr_measure.so: measurement module loaded into C++ slave managers (rtcd2)
 *
 * @~japanese
 * snmpmgr_measure.so: C++ の slave マネージャ (rtcd2) に読み込む計測モジュール
 *
 * Python 版 SlaveMeasureModule.py と同じものを、同じ設定 (snmp.measure.*) で計測し、同じ形式の snmp.measure.runtime_dir/\<マネージャ名\>.json に書き出す。マスターの SNMPManager (Python 版・C++ 版) が集めて保存する。
 * @code
 *   manager.modules.preload: /opt/snmpmgr-cpp/snmpmgr_measure.so
 *   snmp.measure.enabled: YES
 * @endcode
 * (rtc.conf または rtcd2 の -o で指定する)
 *
 * 計測するもの (snmp.measure.interval 秒ごと)
 * - CPU: プロセス全体の使用率 (1 コア = 100) と、RTC ごとの実行コンテキストのスレッドの使用率。RTC の値 = 自分の実行コンテキストのスレッド + 残り (ORB のスレッドなど) をスレッドの使用率の比で按分。実行コンテキストのスレッドは、PRE_ON_EXECUTE のリスナーで、そのスレッドの ID を記録して求める
 * - メモリ: プロセスの USS (Linux: smaps_rollup の Private、Windows: PrivateUsage)。RTC の値 = (USS - 最初の RTC を置く直前の USS) を、生成したときの増分の比で按分 (増分を下回らない)
 * - データポート: InPort は受信 (ON_RECEIVED)、OutPort は送信 (ON_SEND) したデータのバイト数と回数 (コネクタごと)
 * @~english
 * Measures the same items as the Python version SlaveMeasureModule.py with the same settings (snmp.measure.*), and writes them to snmp.measure.runtime_dir/\<manager name\>.json in the same format. The master SNMPManager (Python or C++ version) collects and saves them.
 * @code
 *   manager.modules.preload: /opt/snmpmgr-cpp/snmpmgr_measure.so
 *   snmp.measure.enabled: YES
 * @endcode
 * (specified in rtc.conf or with -o of rtcd2)
 *
 * Measured items (every snmp.measure.interval seconds)
 * - CPU: usage of the whole process (1 core = 100) and usage of the execution context threads of each RTC. Value of an RTC = its own execution context threads + the rest (ORB threads, etc.) divided in proportion to the thread usages. The execution context threads are found by recording the thread ID in a PRE_ON_EXECUTE listener
 * - Memory: USS of the process (Linux: Private of smaps_rollup, Windows: PrivateUsage). Value of an RTC = (USS - USS just before the first RTC) divided in proportion to the increments at creation (not less than the increment)
 * - Data ports: bytes and counts of data received (ON_RECEIVED) by InPorts and sent (ON_SEND) by OutPorts (per connector)
 * @~
 *
 * @date $Date: 2026-11-13 16:24:27 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2026
 *     Nobuhiko Miyamoto
 *
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/Manager.h>
#include <rtm/RTObject.h>
#include <rtm/InPortBase.h>
#include <rtm/OutPortBase.h>
#include <rtm/ManagerActionListener.h>
#include <rtm/ComponentActionListener.h>
#include <rtm/ConnectorListener.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>

#include "MeasurementAggregator.hpp"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <direct.h>
#include <process.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#endif

using json = nlohmann::json;

namespace
{
  constexpr double MB = 1024.0 * 1024.0;
  constexpr size_t MAX_SAMPLES = 3600;

  /*!
   * @~japanese
   * @brief 単調な時計の時刻 (秒) を返す
   *
   * @return 秒
   * @~english
   * @brief Returns the time of the monotonic clock (seconds)
   *
   * @return seconds
   * @~
   */
  double monotonic()
  {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
  }

  /*!
   * @~japanese
   * @brief 実時間の時刻 (UNIX 時刻、秒) を返す
   *
   * @return 秒
   * @~english
   * @brief Returns the wall-clock time (UNIX time in seconds)
   *
   * @return seconds
   * @~
   */
  double wallclock()
  {
    return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

  /*!
   * @~japanese
   * @brief 設定の値が YES / TRUE / ON / 1 か
   *
   * @param v 値
   * @return そうなら true
   * @~english
   * @brief Whether a setting value is YES / TRUE / ON / 1
   *
   * @param v value
   * @return true if so
   * @~
   */
  bool yes(const std::string& v)
  {
    std::string s;
    for (char c : v) { if (!std::isspace(static_cast<unsigned char>(c))) { s += static_cast<char>(std::toupper(static_cast<unsigned char>(c))); } }
    return s == "YES";
  }

  /*!
   * @~japanese
   * @brief 自分のプロセス ID を返す
   *
   * @return プロセス ID
   * @~english
   * @brief Returns the ID of this process
   *
   * @return process ID
   * @~
   */
  long current_pid()
  {
#ifdef _WIN32
    return static_cast<long>(GetCurrentProcessId());
#else
    return static_cast<long>(getpid());
#endif
  }

  /*!
   * @~japanese
   * @brief 今のスレッドのネイティブの ID を返す
   *
   * @return スレッド ID
   * @~english
   * @brief Returns the native ID of the current thread
   *
   * @return thread ID
   * @~
   */
  long current_tid()
  {
#ifdef _WIN32
    return static_cast<long>(GetCurrentThreadId());
#else
    return static_cast<long>(syscall(SYS_gettid));
#endif
  }

  // ---------------------------------------------------------------------------
  // プロセスの情報 (自分のプロセスの CPU 時間・スレッドごとの CPU 時間・USS)
  // ---------------------------------------------------------------------------
#ifdef _WIN32
  /*!
   * @~japanese
   * @brief FILETIME を秒にする (Windows)
   *
   * @param ft FILETIME
   * @return 秒
   * @~english
   * @brief Converts FILETIME into seconds (Windows)
   *
   * @param ft FILETIME
   * @return seconds
   * @~
   */
  double filetime_seconds(const FILETIME& ft)
  {
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return static_cast<double>(u.QuadPart) / 1e7;
  }
#endif

  /*!
   * @~japanese
   * @brief プロセスの CPU 時間を返す
   *
   * @return CPU 秒
   * @~english
   * @brief Returns the CPU time of the process
   *
   * @return CPU seconds
   * @~
   */
  double process_cpu_seconds()
  {
#ifdef _WIN32
    FILETIME c, e, k, u;
    if (!GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u)) { return 0; }
    return filetime_seconds(k) + filetime_seconds(u);
#else
    timespec ts{};
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1e9;
#endif
  }

  /*!
   * @~japanese
   * @brief スレッドの CPU 時間を返す
   *
   * @param tid スレッド ID
   * @return CPU 秒 (取れなければ負)
   * @~english
   * @brief Returns the CPU time of a thread
   *
   * @param tid thread ID
   * @return CPU seconds (negative if not available)
   * @~
   */
  double thread_cpu_seconds(long tid)
  {
#ifdef _WIN32
    HANDLE h = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(tid));
    if (!h) { return -1; }
    FILETIME c, e, k, u;
    double v = GetThreadTimes(h, &c, &e, &k, &u) ? filetime_seconds(k) + filetime_seconds(u) : -1;
    CloseHandle(h);
    return v;
#else
    std::ifstream f("/proc/self/task/" + std::to_string(tid) + "/stat");
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    auto p = s.rfind(')');
    if (p == std::string::npos) { return -1; }
    std::istringstream in(s.substr(p + 1));
    std::string field;
    long long ut = 0, st = 0;
    // ')' の後: state(3) ppid(4) ... utime(14) stime(15)
    for (int i = 3; i <= 15 && (in >> field); ++i)
      {
        if (i == 14) { ut = std::stoll(field); }
        if (i == 15) { st = std::stoll(field); }
      }
    static const double clk = static_cast<double>(sysconf(_SC_CLK_TCK));
    return static_cast<double>(ut + st) / clk;
#endif
  }

  /*!
   * @~japanese
   * @brief プロセスの USS を返す (Linux: smaps_rollup の Private、Windows: PrivateUsage)
   *
   * @return USS (MB)
   * @~english
   * @brief Returns the USS of the process (Linux: Private of smaps_rollup, Windows: PrivateUsage)
   *
   * @return USS (MB)
   * @~
   */
  double uss_mb()
  {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
      {
        return static_cast<double>(pmc.PrivateUsage) / MB;
      }
    return 0;
#else
    std::ifstream f("/proc/self/smaps_rollup");
    std::string line;
    long long kb = 0;
    while (std::getline(f, line))
      {
        if (line.rfind("Private_Clean:", 0) == 0 || line.rfind("Private_Dirty:", 0) == 0)
          {
            std::istringstream in(line);
            std::string key;
            long long v = 0;
            in >> key >> v;
            kb += v;
          }
      }
    return static_cast<double>(kb) / 1024.0;
#endif
  }

  /*!
   * @~japanese
   * @brief ディレクトリを作る (途中のディレクトリも)
   *
   * @param path パス
   * @~english
   * @brief Creates a directory (including intermediate directories)
   *
   * @param path path
   * @~
   */
  void make_dirs(const std::string& path)
  {
    std::string cur;
    for (size_t i = 0; i <= path.size(); ++i)
      {
        if (i == path.size() || path[i] == '/' || path[i] == '\\')
          {
            if (!cur.empty())
              {
#ifdef _WIN32
                _mkdir(cur.c_str());
#else
                mkdir(cur.c_str(), 0755);
#endif
              }
          }
        if (i < path.size()) { cur += path[i]; }
      }
  }

  /*!
   * @~japanese
   * @brief 小数点以下 n 桁に丸める
   *
   * @param v 値
   * @param n 桁数
   * @return 丸めた値
   * @~english
   * @brief Rounds to n decimal places
   *
   * @param v value
   * @param n number of digits
   * @return rounded value
   * @~
   */
  double round_n(double v, int n)
  {
    double m = std::pow(10.0, n);
    return std::round(v * m) / m;
  }

  // ---------------------------------------------------------------------------
  // データポートの送受信
  // ---------------------------------------------------------------------------
  /*!
   * @~japanese
   * @brief 1 つのポートの送受信 (コネクタごとの回数と、バイト数) を数える
   * @~english
   * @brief Counts the data sent/received by one port (count per connector and bytes)
   * @~
   */
  class PortCounter
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param name ポートの名前
     * @param direction in / out
     * @~english
     * @brief Constructor
     *
     * @param name port name
     * @param direction in / out
     * @~
     */
    PortCounter(std::string name, std::string direction) : name_(std::move(name)), direction_(std::move(direction)) {}

    /*!
     * @~japanese
     * @brief 1 回の送受信を足す
     *
     * @param connector コネクタの ID
     * @param size バイト数
     * @~english
     * @brief Adds one transmission
     *
     * @param connector connector ID
     * @param size number of bytes
     * @~
     */
    void add(const std::string& connector, unsigned long size)
    {
      double now = monotonic();
      std::lock_guard<std::mutex> g(m_);
      ++msgs_;
      bytes_ += size;
      bytes_max_ = std::max<unsigned long long>(bytes_max_, size);
      ++per_connector_[connector];
      if (first_ < 0) { first_ = now; }
      last_ = now;
    }

    /*!
     * @~japanese
     * @brief 現在の値を返す
     *
     * @return {"direction", "msgs", "bytes", "bytes_per_msg", "bytes_max", "connectors", "hz"}
     * @~english
     * @brief Returns the current values
     *
     * @return {"direction", "msgs", "bytes", "bytes_per_msg", "bytes_max", "connectors", "hz"}
     * @~
     */
    json snapshot()
    {
      std::lock_guard<std::mutex> g(m_);
      double span = (first_ >= 0 && last_ >= 0) ? last_ - first_ : 0.0;
      long long busiest = 0;
      for (auto& [k, v] : per_connector_) { busiest = std::max(busiest, v); }
      // 周期は、いちばん多く送受信したコネクタの回数から求める (1 つのポートに複数のコネクタがあっても重ねない)
      double hz = (span > 0 && busiest > 1) ? static_cast<double>(busiest - 1) / span : 0.0;
      json out = {{"direction", direction_}, {"msgs", msgs_}, {"bytes", bytes_}, {"bytes_max", bytes_max_},
                  {"connectors", per_connector_.size()}, {"hz", round_n(hz, 3)}};
      out["bytes_per_msg"] = msgs_ ? json(round_n(static_cast<double>(bytes_) / static_cast<double>(msgs_), 2)) : json(nullptr);
      return out;
    }

  private:
    std::string name_, direction_;
    std::mutex m_;
    long long msgs_ = 0;
    unsigned long long bytes_ = 0, bytes_max_ = 0;
    std::map<std::string, long long> per_connector_;
    double first_ = -1, last_ = -1;
  };

  /*!
   * @~japanese
   * @brief ポートの送受信を数えるコネクタのデータリスナー
   * @~english
   * @brief Connector data listener that counts the data of a port
   * @~
   */
  class DataCounter : public RTC::ConnectorDataListener
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param c PortCounter
     * @~english
     * @brief Constructor
     *
     * @param c PortCounter
     * @~
     */
    explicit DataCounter(std::shared_ptr<PortCounter> c) : c_(std::move(c)) {}
    /*!
     * @~japanese
     * @brief データを送受信したときに呼ばれる
     *
     * @param info コネクタの情報
     * @param data データ
     * @return NO_CHANGE
     * @~english
     * @brief Called when data is sent or received
     *
     * @param info connector information
     * @param data data
     * @return NO_CHANGE
     * @~
     */
    ReturnCode operator()(RTC::ConnectorInfo& info, RTC::ByteData& data, const std::string&) override
    {
      try { c_->add(info.id.empty() ? info.name : info.id, data.getDataLength()); }
      catch (...) {}
      return NO_CHANGE;
    }
  private:
    std::shared_ptr<PortCounter> c_;
  };

  /*!
   * @~japanese
   * @brief 実行コンテキストのスレッドの ID を記録するリスナー (PRE_ON_EXECUTE)
   * @~english
   * @brief Listener that records the thread ID of the execution context (PRE_ON_EXECUTE)
   * @~
   */
  class EcThreadListener : public RTC::PreComponentActionListener
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param tid スレッド ID を書くところ
     * @~english
     * @brief Constructor
     *
     * @param tid where to store the thread ID
     * @~
     */
    explicit EcThreadListener(std::shared_ptr<std::atomic<long>> tid) : tid_(std::move(tid)) {}
    /*!
     * @~japanese
     * @brief onExecute の前に呼ばれ、スレッドの ID を記録する
     * @~english
     * @brief Called before onExecute; records the thread ID
     * @~
     */
    void operator()(RTC::UniqueId) override { tid_->store(current_tid(), std::memory_order_relaxed); }
  private:
    std::shared_ptr<std::atomic<long>> tid_;
  };

  // ---------------------------------------------------------------------------
  // 計測
  // ---------------------------------------------------------------------------
  /*!
   * @~japanese
   * @brief 計測している RTC の記録
   * @~english
   * @brief Record of a measured RTC
   * @~
   */
  struct Record
  {
    RTC::RTObject_impl* rtobj = nullptr;
    RTC::RTObject_var ref;
    RTC::ExecutionContext_var ec;
    std::string name, type, impl, language;
    double created = 0, increment = 0;
    std::map<std::string, std::shared_ptr<PortCounter>> ports;
    std::shared_ptr<std::atomic<long>> ec_tid = std::make_shared<std::atomic<long>>(0);
    std::map<long, double> ec_prev;
    std::deque<double> cpu, mem;
    double active_since = -1, active_seconds = 0, removed_at = 0;
    bool removed = false;
  };

  /*!
   * @~japanese
   * @brief マネージャの RTC の CPU・メモリ・データポートを計測し、ファイルに書き出す
   * @~english
   * @brief Measures the CPU, memory and data ports of the RTCs in the manager and writes them to a file
   * @~
   */
  class Measure
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ。計測のスレッドを起動する
     *
     * @param mgr マネージャ
     * @param conf マネージャの設定
     * @~english
     * @brief Constructor. Starts the measurement thread
     *
     * @param mgr manager
     * @param conf manager configuration
     * @~
     */
    Measure(RTC::Manager* mgr, coil::Properties& conf) : mgr_(mgr)
    {
      name_ = conf.getProperty("manager.instance_name", "");
      if (name_.empty()) { name_ = "manager_" + std::to_string(current_pid()); }
      dir_ = conf.getProperty("snmp.measure.runtime_dir", "/run/rtm/measure");
      interval_ = std::stod(conf.getProperty("snmp.measure.interval", "1.0"));
      warmup_ = std::stod(conf.getProperty("snmp.measure.warmup", "3"));
      write_interval_ = std::stod(conf.getProperty("snmp.measure.write_interval", "5"));
      language_ = conf.getProperty("manager.language", "C++");
      if (language_.empty()) { language_ = "C++"; }
      baseline_ = uss_mb();
      thread_ = std::thread([this] { loop(); });
    }

    /*!
     * @~japanese
     * @brief マネージャの名前を返す
     *
     * @return 名前
     * @~english
     * @brief Returns the manager name
     *
     * @return name
     * @~
     */
    const std::string& name() const { return name_; }
    /*!
     * @~japanese
     * @brief 計測の書き出し先のディレクトリを返す
     *
     * @return ディレクトリ
     * @~english
     * @brief Returns the output directory of measurements
     *
     * @return directory
     * @~
     */
    const std::string& dir() const { return dir_; }

    /*!
     * @~japanese
     * @brief RTC を生成する直前に呼ばれ、USS を記録する
     * @~english
     * @brief Called just before an RTC is created; records the USS
     * @~
     */
    void pre_create()
    {
      pending_uss_ = uss_mb();
      std::lock_guard<std::recursive_mutex> g(m_);
      // 最初の RTC を置く直前の USS を空マネージャの値にする
      if (comps_.empty()) { baseline_ = pending_uss_; }
    }

    /*!
     * @~japanese
     * @brief 生成した RTC を記録する
     *
     * @param rtobj RTC
     * @param increment 生成したときの USS の増分 (負なら pre_create との差)
     * @~english
     * @brief Records a created RTC
     *
     * @param rtobj RTC
     * @param increment USS increment at creation (difference from pre_create if negative)
     * @~
     */
    void post_create(RTC::RTObject_impl* rtobj, double increment = -1)
    {
      if (rtobj == nullptr) { return; }
      std::lock_guard<std::recursive_mutex> g(m_);
      if (comps_.count(rtobj)) { return; }
      try
        {
          auto rec = std::make_shared<Record>();
          rec->rtobj = rtobj;
          coil::Properties& props = rtobj->getProperties();
          rec->type = props.getProperty("type_name");
          rec->impl = props.getProperty("implementation_id", rec->type);
          rec->language = props.getProperty("language", language_);
          if (rec->language.empty()) { rec->language = language_; }
          rec->name = rtobj->getInstanceName();
          rec->created = wallclock();
          rec->increment = increment >= 0 ? increment : std::max(0.0, uss_mb() - (pending_uss_ > 0 ? pending_uss_ : baseline_));
          // getObjRef() が複製を返しても返さなくても安全なように、もう一度複製して持つ (前者なら 1 つ余るだけ)
          rec->ref = RTC::RTObject::_duplicate(rtobj->getObjRef());
          rtobj->addPreComponentActionListener(RTC::PreComponentActionListenerType::PRE_ON_EXECUTE,
                                               new EcThreadListener(rec->ec_tid), true);
          comps_[rtobj] = rec;
        }
      catch (std::exception& e) { std::cerr << "[snmpmgr_measure] post_create failed: " << e.what() << std::endl; }
      catch (CORBA::Exception& e) { std::cerr << "[snmpmgr_measure] post_create failed: CORBA " << e._name() << std::endl; }
    }

    /*!
     * @~japanese
     * @brief 計測を止めて、最後の結果を書き出す
     * @~english
     * @brief Stops the measurement and writes the final results
     * @~
     */
    void stop()
    {
      {
        std::lock_guard<std::mutex> g(wake_m_);
        running_ = false;
      }
      wake_.notify_all();
      if (thread_.joinable()) { thread_.join(); }
      try { write(); }
      catch (...) {}
    }

  private:
    /*!
     * @~japanese
     * @brief まだリスナーを付けていないポートに付ける
     *
     * @param rec RTC の記録
     * @~english
     * @brief Attaches listeners to ports that do not have them yet
     *
     * @param rec record of the RTC
     * @~
     */
    void attach_ports(Record& rec)
    {
      // まだリスナーを付けていないポートに付ける (ポートは onInitialize で作られるので、標本ごとに確かめる)
      RTC::PortServiceList_var ports = rec.rtobj->get_ports();
      PortableServer::POA_ptr poa = mgr_->getPOA();
      for (CORBA::ULong i = 0; i < ports->length(); ++i)
        {
          PortableServer::ServantBase* sv = nullptr;
          try { sv = poa->reference_to_servant(ports[i]); }
          catch (...) { continue; }
          if (sv == nullptr) { continue; }
          auto* base = dynamic_cast<RTC::PortBase*>(sv);
          if (base != nullptr)
            {
              std::string full = base->getName();
              std::string shortname = full.substr(full.rfind('.') == std::string::npos ? 0 : full.rfind('.') + 1);
              if (!rec.ports.count(shortname))
                {
                  if (auto* in = dynamic_cast<RTC::InPortBase*>(sv))
                    {
                      auto c = std::make_shared<PortCounter>(shortname, "in");
                      in->addConnectorDataListener(RTC::ConnectorDataListenerType::ON_RECEIVED, new DataCounter(c), true);
                      rec.ports[shortname] = c;
                    }
                  else if (auto* out = dynamic_cast<RTC::OutPortBase*>(sv))
                    {
                      auto c = std::make_shared<PortCounter>(shortname, "out");
                      out->addConnectorDataListener(RTC::ConnectorDataListenerType::ON_SEND, new DataCounter(c), true);
                      rec.ports[shortname] = c;
                    }
                }
            }
          sv->_remove_ref();     // reference_to_servant は参照数を増やす
        }
    }

    /*!
     * @~japanese
     * @brief RTC がアクティブか
     *
     * @param rec RTC の記録
     * @return アクティブなら true
     * @~english
     * @brief Whether the RTC is active
     *
     * @param rec record of the RTC
     * @return true if active
     * @~
     */
    bool is_active(Record& rec)
    {
      try
        {
          if (CORBA::is_nil(rec.ec))
            {
              RTC::ExecutionContext_ptr ec = rec.rtobj->getExecutionContext(0);
              if (CORBA::is_nil(ec)) { return false; }
              rec.ec = RTC::ExecutionContext::_duplicate(ec);
            }
          return rec.ec->get_component_state(rec.ref) == RTC::ACTIVE_STATE;
        }
      catch (...) { return false; }
    }

    /*!
     * @~japanese
     * @brief 計測のスレッドの本体
     * @~english
     * @brief Body of the measurement thread
     * @~
     */
    void loop()
    {
      double prev_t = monotonic(), prev_cpu = process_cpu_seconds(), last_write = 0;
      while (true)
        {
          {
            std::unique_lock<std::mutex> lk(wake_m_);
            wake_.wait_for(lk, std::chrono::duration<double>(interval_), [this] { return !running_; });
            if (!running_) { return; }
          }
          try
            {
              double now = monotonic(), cpu = process_cpu_seconds();
              double dt = std::max(1e-6, now - prev_t);
              double proc_cpu = 100.0 * (cpu - prev_cpu) / dt;
              prev_t = now;
              prev_cpu = cpu;
              sample(now, dt, proc_cpu);
              if (now - last_write >= write_interval_)
                {
                  write();
                  last_write = now;
                }
            }
          catch (std::exception& e) { std::cerr << "[snmpmgr_measure] sample failed: " << e.what() << std::endl; }
          catch (CORBA::Exception& e) { std::cerr << "[snmpmgr_measure] sample failed: CORBA " << e._name() << std::endl; }
        }
    }

    /*!
     * @~japanese
     * @brief 標本を 1 つ取る
     *
     * @param now 現在の時刻
     * @param dt 前の標本からの秒数
     * @param proc_cpu プロセス全体の CPU 使用率
     * @~english
     * @brief Takes one sample
     *
     * @param now current time
     * @param dt seconds since the previous sample
     * @param proc_cpu CPU usage of the whole process
     * @~
     */
    void sample(double now, double dt, double proc_cpu)
    {
      double uss = uss_mb();
      // マネージャの RTC。記録していない RTC (このモジュールを読み込む前に生成されたもの) も記録する
      // (生成したときの増分は分からないので 0 とする)
      std::vector<RTC::RTObject_impl*> alive = mgr_->getComponents();
      std::lock_guard<std::recursive_mutex> g(m_);
      for (auto* c : alive) { if (!comps_.count(c)) { post_create(c, 0.0); } }
      std::map<std::string, std::shared_ptr<Record>> live;
      for (auto& [ptr, rec] : comps_)
        {
          if (rec->removed) { continue; }
          if (std::find(alive.begin(), alive.end(), ptr) == alive.end())
            {
              rec->removed = true;
              rec->removed_at = wallclock();
              continue;
            }
          try { rec->name = rec->rtobj->getInstanceName(); }
          catch (...) {}
          try { attach_ports(*rec); }
          catch (...) {}
          live[std::to_string(reinterpret_cast<uintptr_t>(ptr))] = rec;
        }
      std::map<std::string, double> ec_cpu, inc;
      for (auto& [key, rec] : live)
        {
          double used = 0;
          long tid = rec->ec_tid->load(std::memory_order_relaxed);
          if (tid > 0)
            {
              double t = thread_cpu_seconds(tid);
              if (t >= 0)
                {
                  auto p = rec->ec_prev.find(tid);
                  if (p != rec->ec_prev.end()) { used = 100.0 * std::max(0.0, t - p->second) / dt; }
                  rec->ec_prev[tid] = t;
                }
            }
          ec_cpu[key] = used;
          inc[key] = rec->increment;
        }
      auto cpu_of = snmpmgr::measure::attribute_cpu(proc_cpu, ec_cpu);
      auto mem_of = snmpmgr::measure::attribute_memory(uss, baseline_, inc);
      for (auto& [key, rec] : live)
        {
          if (!is_active(*rec))
            {
              rec->active_since = -1;
              continue;
            }
          if (rec->active_since < 0)
            {
              rec->active_since = now;
              continue;
            }
          rec->active_seconds += dt;
          if (now - rec->active_since < warmup_) { continue; }
          rec->cpu.push_back(cpu_of[key]);
          rec->mem.push_back(std::max(rec->increment, mem_of[key]));
          if (rec->cpu.size() > MAX_SAMPLES) { rec->cpu.pop_front(); }
          if (rec->mem.size() > MAX_SAMPLES) { rec->mem.pop_front(); }
        }
      last_ = {{"proc_cpu", round_n(proc_cpu, 3)}, {"uss_mb", round_n(uss, 3)}};
    }

    /*!
     * @~japanese
     * @brief 計測の結果を返す
     *
     * @return 計測ファイルの内容
     * @~english
     * @brief Returns the measurement results
     *
     * @return contents of the measurement file
     * @~
     */
    json snapshot()
    {
      std::lock_guard<std::recursive_mutex> g(m_);
      std::vector<std::shared_ptr<Record>> recs;
      for (auto& [p, r] : comps_) { recs.push_back(r); }
      std::sort(recs.begin(), recs.end(), [](auto& a, auto& b) { return a->created < b->created; });
      json comps = json::object();
      for (auto& rec : recs)
        {
          std::string key = rec->name;
          if (comps.contains(key))
            {
              // 同じ名前で作り直した RTC: 前の分は別のキーで残す
              json prev = comps[key];
              comps.erase(key);
              comps[key + "#" + std::to_string(static_cast<long long>(prev["created"].get<double>()))] = prev;
            }
          json ports = json::object();
          for (auto& [n, c] : rec->ports) { ports[n] = c->snapshot(); }
          comps[key] = {{"instance", rec->name}, {"type", rec->type}, {"impl", rec->impl}, {"language", rec->language},
                        {"created", rec->created}, {"removed", rec->removed},
                        {"samples", rec->cpu.size()}, {"interval", interval_},
                        {"active_seconds", round_n(rec->active_seconds, 2)},
                        {"increment_mb", round_n(rec->increment, 3)},
                        {"cpu", snmpmgr::measure::summarize({rec->cpu.begin(), rec->cpu.end()})},
                        {"memory_mb", snmpmgr::measure::summarize({rec->mem.begin(), rec->mem.end()})},
                        {"ports", ports}};
        }
      return {{"manager", name_}, {"pid", current_pid()}, {"language", language_}, {"updated", wallclock()},
              {"baseline_uss_mb", round_n(baseline_, 3)}, {"process", last_}, {"components", comps}};
    }

    /*!
     * @~japanese
     * @brief 計測の結果をファイルに書き出す
     * @~english
     * @brief Writes the measurement results to the file
     * @~
     */
    void write()
    {
      json data = snapshot();
      if (data["components"].empty()) { return; }
      make_dirs(dir_);
      std::string path = dir_ + "/" + name_ + ".json", tmp = path + ".tmp";
      {
        std::ofstream f(tmp, std::ios::trunc);
        f << data.dump(1);
      }
#ifdef _WIN32
      MoveFileExA(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING);
#else
      std::rename(tmp.c_str(), path.c_str());
#endif
    }

    RTC::Manager* mgr_;
    std::string name_, dir_, language_;
    double interval_ = 1, warmup_ = 3, write_interval_ = 5;
    double baseline_ = 0, pending_uss_ = 0;
    std::recursive_mutex m_;
    std::map<RTC::RTObject_impl*, std::shared_ptr<Record>> comps_;
    json last_ = json::object();
    std::mutex wake_m_;
    std::condition_variable wake_;
    bool running_ = true;
    std::thread thread_;
  };

  Measure* g_measure = nullptr;

  /*!
   * @~japanese
   * @brief RTC の生成の前後を受けるリスナー
   * @~english
   * @brief Listener for before/after RTC creation
   * @~
   */
  class Lifecycle : public RTM::RtcLifecycleActionListener
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param m Measure
     * @~english
     * @brief Constructor
     *
     * @param m Measure
     * @~
     */
    explicit Lifecycle(Measure* m) : m_(m) {}
    void preCreate(std::string&) override { m_->pre_create(); }
    void postCreate(RTC::RTObject_impl* rtobj) override { m_->post_create(rtobj); }
    void preConfigure(coil::Properties&) override {}
    void postConfigure(coil::Properties&) override {}
    void preInitialize() override {}
    void postInitialize() override {}
  private:
    Measure* m_;
  };

  /*!
   * @~japanese
   * @brief マネージャの終了を受けて計測を止めるリスナー
   * @~english
   * @brief Listener that stops the measurement when the manager shuts down
   * @~
   */
  class Shutdown : public RTM::ManagerActionListener
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param m Measure
     * @~english
     * @brief Constructor
     *
     * @param m Measure
     * @~
     */
    explicit Shutdown(Measure* m) : m_(m) {}
    void preShutdown() override { m_->stop(); }
    void postShutdown() override {}
    void preReinit() override {}
    void postReinit() override {}
  private:
    Measure* m_;
  };
} // namespace

extern "C"
{
  /*!
   * @~japanese
   * @brief manager.modules.preload で読み込まれたときの入口 (ファイル名 snmpmgr_measure.so から "snmpmgr_measureInit")
   *
   * @param manager マネージャ
   * @~english
   * @brief Entry point when loaded by manager.modules.preload ("snmpmgr_measureInit" from the file name snmpmgr_measure.so)
   *
   * @param manager manager
   * @~
   */
  DLL_EXPORT void snmpmgr_measureInit(RTC::Manager* manager)
  {
    coil::Properties& conf = manager->getConfig();
    if (!yes(conf.getProperty("snmp.measure.enabled", "NO"))) { return; }
    if (yes(conf.getProperty("manager.is_master", "NO")) && !yes(conf.getProperty("snmp.measure.master", "NO"))) { return; }
    if (g_measure != nullptr) { return; }
    g_measure = new Measure(manager, conf);
    std::cerr << "[snmpmgr_measure] measuring RTCs of " << g_measure->name() << " (pid " << current_pid() << ") -> "
              << g_measure->dir() << std::endl;
    manager->addRtcLifecycleActionListener(new Lifecycle(g_measure), true);
    manager->addManagerActionListener(new Shutdown(g_measure), true);
  }

  /*!
   * @~japanese
   * @brief 入口の別名
   *
   * @param manager マネージャ
   * @~english
   * @brief Alias of the entry point
   *
   * @param manager manager
   * @~
   */
  DLL_EXPORT void SNMPMeasureInit(RTC::Manager* manager) { snmpmgr_measureInit(manager); }
}
