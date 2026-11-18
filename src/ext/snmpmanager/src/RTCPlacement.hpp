// -*- C++ -*-
/*!
 * @file RTCPlacement.hpp
 * @brief Automatic placement of RTCs (same approach and priorities as the Python version RTCPlacement.py)
 *
 * @~japanese
 * RTC の自動配置 (Python 版 RTCPlacement.py と同じ考え方・同じ優先順位)
 *
 * logic:
 * - optimal (既定): 全探索 (大きい問題は貪欲法) で、次の優先順位 (辞書式) の最良解を求める
 *   1. 制約 (置けるロボット・CPU/メモリの空き・通信状態の悪いロボットをまたぐ接続をしない) を守る
 *   2. ロボットをまたぐ接続の通信量を最小にする
 *   3. primary に置く RTC の数を最大にする
 *   4. primary の CPU をできるだけ使う
 *   5. primary 以外の CPU 使用率の最大値を最小にする
 * - first_fit: RTC を並び順に、ロボットのリストの先頭から制約を満たす最初のロボットに置く
 * - objective: RTC を並び順に、制約を満たすロボットのうち目的関数が最大のものに置く
 * @~english
 * logic:
 * - optimal (default): finds the best solution by exhaustive search (greedy for large problems) with the following lexicographic priorities
 *   1. keep the constraints (allowed robots, free CPU/memory, no connections across robots with bad links)
 *   2. minimize the traffic of connections across robots
 *   3. maximize the number of RTCs on the primary
 *   4. use the CPU of the primary as much as possible
 *   5. minimize the maximum CPU usage of the robots other than the primary
 * - first_fit: places RTCs in order on the first robot in the robot list satisfying the constraints
 * - objective: places RTCs in order on the robot maximizing the objective function among those satisfying the constraints
 * @~
 *
 * @date $Date: 2026-11-13 16:24:28 $
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

#ifndef SNMPMGR_PLACEMENT_HPP
#define SNMPMGR_PLACEMENT_HPP

#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <snmpmgr/Plugin.hpp>

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief ポートの通信量の見込み (向き、1 回のバイト数、頻度)
   * @~english
   * @brief Estimated traffic of a port (direction, bytes per message, rate)
   * @~
   */
  struct PortProfile
  {
    std::string direction;
    double bytes{0};
    double hz{1};
  };

  /*!
   * @~japanese
   * @brief RTC のプロファイル (placement.xml から読んだ CPU・メモリ・ポートの通信量の見込み値)
   * @~english
   * @brief Profile of an RTC (estimated CPU, memory and port traffic read from placement.xml)
   * @~
   */
  struct RtcProfile
  {
    std::string rtc, language, environment, scaled_from, source;
    double cpu{0}, memory_mb{0}, slave_overhead_mb{0};
    std::set<std::string> required_tags;
    std::map<std::string, PortProfile> ports;
    bool measured{false};     // マネージャが実行中に計測した値 (plc:origin="runtime") を使った
    int samples{0};           // その計測の標本数
  };

  /*!
   * @~japanese
   * @brief placement.xml / environments.xml の読み込み
   *
   * root はカンマ区切りで複数指定できる。マネージャが実行中に計測して保存した placement.xml (plc:origin="runtime") があれば、prefer_measured のとき、その値で CPU・メモリ・ポートの通信量を置き換える (複数のロボットの計測があれば大きい方)。標本数が min_samples に満たない計測は使わない。
   * @~english
   * @brief Loads placement.xml / environments.xml
   *
   * root may contain several directories separated by commas. If there are placement.xml files measured and saved by the managers at runtime (plc:origin="runtime") and prefer_measured is set, their values replace the CPU, memory and port traffic (the larger one if measured on several robots). Measurements with fewer samples than min_samples are not used.
   * @~
   */
  class ProfileDB
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ。root の下の placement.xml を読む
     *
     * @param root placement.xml を探すディレクトリ (カンマ区切り)
     * @param environment 使う計測の環境の名前
     * @param prefer_measured 実行中の計測値を優先するか
     * @param min_samples 使う計測の最小の標本数
     * @~english
     * @brief Constructor. Reads placement.xml files under root
     *
     * @param root directories to search for placement.xml (comma separated)
     * @param environment name of the measurement environment to use
     * @param prefer_measured whether to prefer runtime measurements
     * @param min_samples minimum number of samples of a measurement to use
     * @~
     */
    ProfileDB(const std::string& root, std::string environment, bool prefer_measured = true, int min_samples = 1);
    /*!
     * @~japanese
     * @brief RTC のプロファイルを返す
     *
     * @param type_name 型名
     * @param language 言語
     * @return プロファイル (なければ nullopt)
     * @~english
     * @brief Returns the profile of an RTC
     *
     * @param type_name type name
     * @param language language
     * @return profile (nullopt if none)
     * @~
     */
    std::optional<RtcProfile> get(const std::string& type_name, const std::string& language) const;
    /*!
     * @~japanese
     * @brief 読んだプロファイルの数を返す
     *
     * @return 数
     * @~english
     * @brief Returns the number of loaded profiles
     *
     * @return number
     * @~
     */
    size_t size() const { return profiles_.size(); }
  private:
    /*!
     * @~japanese
     * @brief 1 つの placement.xml の内容 (実行中の計測か、環境ごとのプロファイル)
     * @~english
     * @brief Contents of one placement.xml (whether runtime-measured, profiles per environment)
     * @~
     */
    struct Entry { bool runtime; std::vector<RtcProfile> per_env; };
    /*!
     * @~japanese
     * @brief ディレクトリの placement.xml を読む
     *
     * @param root ディレクトリ
     * @~english
     * @brief Loads placement.xml files in a directory
     *
     * @param root directory
     * @~
     */
    void load(const std::string& root);
    /*!
     * @~japanese
     * @brief 候補の中から、使うプロファイルを選ぶ
     *
     * @param entries 候補
     * @return プロファイル (なければ nullopt)
     * @~english
     * @brief Selects the profile to use among candidates
     *
     * @param entries candidates
     * @return profile (nullopt if none)
     * @~
     */
    std::optional<RtcProfile> best(const std::vector<const Entry*>& entries) const;
    std::string env_;
    bool prefer_measured_;
    int min_samples_;
    std::map<std::string, double> perf_;
    std::multimap<std::pair<std::string, std::string>, Entry> profiles_;  // (lang, rtc) -> ファイルごと
  };

  /*!
   * @~japanese
   * @brief 静的な見込み値を実行中の計測値 (1 つ以上) で置き換える (RTCPlacement.py の merge_measured)
   *
   * @param static_prof 静的な見込み値 (なければ nullopt)
   * @param runtime 実行中の計測値
   * @return 置き換えたプロファイル
   * @~english
   * @brief Replaces the static estimates with runtime measurements (one or more) (merge_measured of RTCPlacement.py)
   *
   * @param static_prof static estimates (nullopt if none)
   * @param runtime runtime measurements
   * @return merged profile
   * @~
   */
  RtcProfile merge_measured(const std::optional<RtcProfile>& static_prof, const std::vector<RtcProfile>& runtime);

  /*!
   * @~japanese
   * @brief 配置の結果 (割り当て、ロボットをまたぐ接続、ロボットごとの使用量、評価値)
   * @~english
   * @brief Result of the placement (assignment, connections across robots, usage of each robot, score)
   * @~
   */
  struct PlacementResult
  {
    std::map<std::string, std::string> assign;
    std::vector<PlacementEdge> cross;
    std::map<std::string, std::pair<double, double>> usage;   // robot -> (cpu, mem)
    std::tuple<double, int, double, double> score;
  };

  /*!
   * @~japanese
   * @brief connections (JSON) から接続の重みを作る (同じ 2 つの RTC の接続はまとめる)
   *
   * @param connections 構成ファイルの connections
   * @param items 配置する RTC
   * @param profiles RTC 名 -> プロファイル
   * @return PlacementEdge のリスト
   * @~english
   * @brief Builds connection weights from connections (JSON) (connections between the same two RTCs are merged)
   *
   * @param connections connections in the configuration file
   * @param items RTCs to place
   * @param profiles RTC name -> profile
   * @return list of PlacementEdge
   * @~
   */
  std::vector<PlacementEdge> build_edges(const json& connections, const std::vector<PlacementItem>& items,
                                         const std::map<std::string, RtcProfile>& profiles);
  /*!
   * @~japanese
   * @brief 割り当てを評価する
   *
   * @param assign RTC 名 -> ロボット名
   * @param items 配置する RTC
   * @param robots ロボット
   * @param edges 接続
   * @param primary 主ロボットの名前
   * @return PlacementResult (制約を満たさなければ nullopt)
   * @~english
   * @brief Evaluates an assignment
   *
   * @param assign RTC name -> robot name
   * @param items RTCs to place
   * @param robots robots
   * @param edges connections
   * @param primary name of the primary robot
   * @return PlacementResult (nullopt if the constraints are not satisfied)
   * @~
   */
  std::optional<PlacementResult> evaluate(const std::map<std::string, std::string>& assign,
                                          const std::vector<PlacementItem>& items,
                                          const std::vector<PlacementRobot>& robots,
                                          const std::vector<PlacementEdge>& edges, const std::string& primary);
  /*!
   * @~japanese
   * @brief 使える配置のロジックを返す
   *
   * @return ロジックの名前
   * @~english
   * @brief Returns the available placement logics
   *
   * @return names of logics
   * @~
   */
  const std::vector<std::string>& logics();
  /*!
   * @~japanese
   * @brief 目的関数 fn(item, robot, state) (objective のロジック)
   * @~english
   * @brief Objective function fn(item, robot, state) (objective logic)
   * @~
   */
  using Objective = std::function<double(const PlacementItem&, const PlacementRobot&, const ObjectiveState&)>;
  /*!
   * @~japanese
   * @brief 既定の重み (cpu / memory / traffic / primary / link) を返す
   *
   * @return 重み
   * @~english
   * @brief Returns the default weights (cpu / memory / traffic / primary / link)
   *
   * @return weights
   * @~
   */
  const std::map<std::string, double>& default_weights();
  /*!
   * @~japanese
   * @brief objective のロジックの既定の目的関数 (RTCPlacement.py の objective_value。大きいほどよい)
   *
   * @param item 置く RTC
   * @param robot 置くロボット
   * @param state 配置の途中の状態
   * @param weights 項ごとの重み (空なら既定値)
   * @return 目的関数の値
   * @~english
   * @brief Default objective function of the objective logic (objective_value of RTCPlacement.py; larger is better)
   *
   * @param item RTC to place
   * @param robot robot to place on
   * @param state state of the placement
   * @param weights weight of each term (defaults if empty)
   * @return value of the objective function
   * @~
   */
  double objective_value(const PlacementItem& item, const PlacementRobot& robot, const ObjectiveState& state,
                         const std::map<std::string, double>& weights = {});
  /*!
   * @~japanese
   * @brief 配置する。配置できなければ PlacementError
   *
   * @param items 配置する RTC
   * @param robots ロボット
   * @param edges 接続
   * @param primary 主ロボットの名前
   * @param logic optimal / first_fit / objective
   * @param objective 目的関数 (空なら objective_value)
   * @return PlacementResult
   * @~english
   * @brief Places RTCs. PlacementError if they cannot be placed
   *
   * @param items RTCs to place
   * @param robots robots
   * @param edges connections
   * @param primary name of the primary robot
   * @param logic optimal / first_fit / objective
   * @param objective objective function (objective_value if empty)
   * @return PlacementResult
   * @~
   */
  PlacementResult solve(const std::vector<PlacementItem>& items, const std::vector<PlacementRobot>& robots,
                        const std::vector<PlacementEdge>& edges, const std::string& primary,
                        const std::string& logic = "optimal", const Objective& objective = nullptr);
  /*!
   * @~japanese
   * @brief ロボット間の制約を満たすか: ルール (max_latency_ms 等) と通信状態
   *
   * @param link 通信状態
   * @param rules ルール
   * @return 満たせば true
   * @~english
   * @brief Whether the inter-robot constraints are satisfied: rules (max_latency_ms, etc.) and link quality
   *
   * @param link link quality
   * @param rules rules
   * @return true if satisfied
   * @~
   */
  bool link_ok(const std::map<std::string, double>& link, const json& rules);
  /*!
   * @~japanese
   * @brief 配置できないときの説明 (必要量の合計、ロボットごとの空き、どこにも入らない RTC) を作る
   *
   * @param items 配置する RTC
   * @param robots ロボット
   * @return 説明の行
   * @~english
   * @brief Makes the explanation for a failed placement (total demand, free resources per robot, RTCs that fit nowhere)
   *
   * @param items RTCs to place
   * @param robots robots
   * @return lines of explanation
   * @~
   */
  std::vector<std::string> capacity_report(const std::vector<PlacementItem>& items,
                                           const std::vector<PlacementRobot>& robots);
  /*!
   * @~japanese
   * @brief 配置の結果のログ用の説明文を作る
   *
   * @param p 配置の結果
   * @param items 配置した RTC
   * @param robots ロボット
   * @param primary 主ロボットの名前
   * @param profiles RTC 名 -> プロファイル
   * @return 説明の行
   * @~english
   * @brief Makes a description of the placement result for logs
   *
   * @param p placement result
   * @param items placed RTCs
   * @param robots robots
   * @param primary name of the primary robot
   * @param profiles RTC name -> profile
   * @return lines of description
   * @~
   */
  std::vector<std::string> describe(const PlacementResult& p, const std::vector<PlacementItem>& items,
                                    const std::vector<PlacementRobot>& robots, const std::string& primary,
                                    const std::map<std::string, RtcProfile>& profiles = {});
  /*!
   * @~japanese
   * @brief create 文字列のモジュール名を返す
   *
   * @param create create 文字列
   * @return モジュール名
   * @~english
   * @brief Returns the module name of a create string
   *
   * @param create create string
   * @return module name
   * @~
   */
  std::string module_name_of(const std::string& create);
} // namespace snmpmgr

#endif
