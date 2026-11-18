// -*- C++ -*-
/*!
 * @file ILPPlacementPlugin.cpp
 * @brief Example placement plugin: decides the placement with the integer linear programming (ILP) solver GLPK
 *
 * @~japanese
 * 配置 (placement) プラグインの例: 整数線形計画 (ILP) のソルバー GLPK で配置を決める
 *
 * Python 版 plugins/ILPPlacementPlugin.py と同じモデル・同じ placement_args (solver は glpk だけ)。
 * @code
 *   "plugins": [{"module": "ilp_placement"}],
 *   "startup": {"mode": "auto", "placement_plugin": "ilp",
 *               "placement_args": {"time_limit": 10, "objective": "lexicographic"}, ...}
 * @endcode
 * @verbatim
 * モデル (i: RTC、r: ロボット、l: 言語、e: 接続 a-b)
 *   変数  x[i,r] ∈ {0,1}  RTC i をロボット r に置く (r は i.candidates のロボットだけ)
 *         y[r,l] ∈ {0,1}  ロボット r で言語 l の slave マネージャを新しく起動する
 *         z[e]   ∈ [0,1]  接続 e がロボットをまたぐ (z[e] >= x[a,r] - x[b,r])
 *         t      >= 0     主ロボット以外の CPU の使用率の最大値
 *   制約  Σr x[i,r] = 1、Σi cpu_i x[i,r] <= cpu_free_r、Σi mem_i x[i,r] + Σl ovh_l y[r,l] <= mem_free_r、
 *         y[r,l] >= x[i,r]、x[a,r] = x[b,r] (r がロボット間の制約を満たさない)、t >= Σi cpu_i x[i,r] / cpu_free_r
 *   目的  lexicographic (既定): 組み込みの optimal と同じ優先順位で順に解く
 *           1. ロボットをまたぐ通信量 Σ w_e z[e] を最小  2. 主ロボットの RTC の数を最大
 *           3. 主ロボットの CPU を最大                    4. t を最小
 *         weighted: weights (cross / primary_count / primary_cpu / balance) で足し合わせて 1 回解く
 * @endverbatim
 * placement_args: time_limit (1 回の求解の秒数。既定 10)、objective、weights、tolerance (既定 1e-6)
 * @~english
 * Same model and placement_args as the Python version plugins/ILPPlacementPlugin.py (solver is glpk only).
 * @code
 *   "plugins": [{"module": "ilp_placement"}],
 *   "startup": {"mode": "auto", "placement_plugin": "ilp",
 *               "placement_args": {"time_limit": 10, "objective": "lexicographic"}, ...}
 * @endcode
 * @verbatim
 * Model (i: RTC, r: robot, l: language, e: connection a-b)
 *   Variables   x[i,r] in {0,1}  RTC i is placed on robot r (r is limited to the robots in i.candidates)
 *               y[r,l] in {0,1}  a new slave manager of language l is started on robot r
 *               z[e]   in [0,1]  connection e crosses robots (z[e] >= x[a,r] - x[b,r])
 *               t      >= 0      maximum CPU usage ratio of the non-primary robots
 *   Constraints sum_r x[i,r] = 1, sum_i cpu_i x[i,r] <= cpu_free_r, sum_i mem_i x[i,r] + sum_l ovh_l y[r,l] <= mem_free_r,
 *               y[r,l] >= x[i,r], x[a,r] = x[b,r] (r violates the inter-robot constraints), t >= sum_i cpu_i x[i,r] / cpu_free_r
 *   Objective   lexicographic (default): solves the same priorities as the built-in optimal in order
 *                 1. minimize the traffic across robots sum_e w_e z[e]  2. maximize the number of RTCs on the primary
 *                 3. maximize the CPU on the primary                    4. minimize t
 *               weighted: solved once with the sum weighted by weights (cross / primary_count / primary_cpu / balance)
 * @endverbatim
 * placement_args: time_limit (seconds of one solve; default 10), objective, weights, tolerance (default 1e-6)
 * @~
 *
 * @date $Date: 2026-11-13 16:24:26 $
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

#include <snmpmgr/Plugin.hpp>

#include <glpk.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <sstream>

using snmpmgr::json;

namespace
{
  constexpr double INF = std::numeric_limits<double>::infinity();

  /*!
   * @~japanese
   * @brief 線形計画のモデル (行は {変数の番号: 係数}, 下限, 上限)
   * @~english
   * @brief Linear programming model (each row is {variable index: coefficient}, lower bound, upper bound)
   * @~
   */
  struct Model
  {
    struct Var { std::string name; double lb, ub; bool integer; };
    struct Row { std::map<int, double> coefs; double lo, hi; };
    std::vector<Var> vars;
    std::vector<Row> rows;

    /*!
     * @~japanese
     * @brief 変数を足す
     *
     * @param name 変数の名前
     * @param lb 下限
     * @param ub 上限
     * @param integer 整数の変数なら true
     * @return 変数の番号
     * @~english
     * @brief Adds a variable
     *
     * @param name name of the variable
     * @param lb lower bound
     * @param ub upper bound
     * @param integer true for an integer variable
     * @return index of the variable
     * @~
     */
    int var(const std::string& name, double lb = 0, double ub = 1, bool integer = true)
    {
      vars.push_back({name, lb, ub, integer});
      return static_cast<int>(vars.size()) - 1;
    }

    /*!
     * @~japanese
     * @brief 制約の行 (lo <= Σ 係数 x 変数 <= hi) を足す。係数が 0 の項は除き、意味のない行は足さない
     *
     * @param coefs {変数の番号: 係数}
     * @param lo 下限
     * @param hi 上限
     * @~english
     * @brief Adds a constraint row (lo <= sum of coefficient x variable <= hi). Zero coefficients are dropped and trivial rows are not added
     *
     * @param coefs {variable index: coefficient}
     * @param lo lower bound
     * @param hi upper bound
     * @~
     */
    void add(std::map<int, double> coefs, double lo = -INF, double hi = INF)
    {
      for (auto it = coefs.begin(); it != coefs.end();) { it = it->second == 0 ? coefs.erase(it) : std::next(it); }
      if (!coefs.empty() || lo > 0 || hi < 0) { rows.push_back({std::move(coefs), lo, hi}); }
    }
  };

  /*!
   * @~japanese
   * @brief 下限と上限から GLPK の範囲の種類を返す
   *
   * @param lo 下限
   * @param hi 上限
   * @return GLP_FR / GLP_LO / GLP_UP / GLP_DB / GLP_FX
   * @~english
   * @brief Returns the GLPK bound type from the lower and upper bounds
   *
   * @param lo lower bound
   * @param hi upper bound
   * @return GLP_FR / GLP_LO / GLP_UP / GLP_DB / GLP_FX
   * @~
   */
  int bound_type(double lo, double hi)
  {
    if (lo == -INF && hi == INF) { return GLP_FR; }
    if (lo == -INF) { return GLP_UP; }
    if (hi == INF) { return GLP_LO; }
    return lo == hi ? GLP_FX : GLP_DB;
  }

  /*!
   * @~japanese
   * @brief GLPK でモデルを解く
   *
   * @param m モデル
   * @param obj 最小化する目的の式 {変数の番号: 係数}
   * @param time_limit 求解の秒数の上限
   * @param status 解の状態 (ログ用)
   * @return 変数の値 (解がなければ空)
   * @~english
   * @brief Solves the model with GLPK
   *
   * @param m model
   * @param obj objective to minimize {variable index: coefficient}
   * @param time_limit time limit of the solve in seconds
   * @param status status of the solution (for logs)
   * @return variable values (empty if there is no solution)
   * @~
   */
  std::vector<double> solve_glpk(const Model& m, const std::map<int, double>& obj, double time_limit, std::string& status)
  {
    glp_term_out(GLP_OFF);
    glp_prob* lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
    int n = static_cast<int>(m.vars.size());
    glp_add_cols(lp, n);
    for (int j = 0; j < n; ++j)
      {
        auto& v = m.vars[j];
        glp_set_col_bnds(lp, j + 1, bound_type(v.lb, v.ub), v.lb == -INF ? 0 : v.lb, v.ub == INF ? 0 : v.ub);
        glp_set_col_kind(lp, j + 1, !v.integer ? GLP_CV : (v.lb == 0 && v.ub == 1 ? GLP_BV : GLP_IV));
      }
    for (auto& [j, c] : obj) { glp_set_obj_coef(lp, j + 1, c); }
    std::vector<int> ia{0}, ja{0};
    std::vector<double> ar{0};
    if (!m.rows.empty())
      {
        glp_add_rows(lp, static_cast<int>(m.rows.size()));
        for (size_t i = 0; i < m.rows.size(); ++i)
          {
            auto& r = m.rows[i];
            glp_set_row_bnds(lp, static_cast<int>(i) + 1, bound_type(r.lo, r.hi), r.lo == -INF ? 0 : r.lo,
                             r.hi == INF ? 0 : r.hi);
            for (auto& [j, c] : r.coefs)
              {
                ia.push_back(static_cast<int>(i) + 1);
                ja.push_back(j + 1);
                ar.push_back(c);
              }
          }
        glp_load_matrix(lp, static_cast<int>(ar.size()) - 1, ia.data(), ja.data(), ar.data());
      }
    // MIP の前処理 (presolve) は使わない: GLPK 4.65 は、前処理ですべての列が消えると (lexicographic の後の段で
    // 前の段の値が配置を決めてしまうと起きる) glp_add_cols(0) でプロセスを止めてしまう。
    // 代わりに、LP の緩和問題を単体法で解いてから分枝限定法を行う
    std::vector<double> values;
    int tm_lim = static_cast<int>(std::min(time_limit, 2.0e6) * 1000.0);
    glp_smcp sm;
    glp_init_smcp(&sm);
    sm.msg_lev = GLP_MSG_OFF;
    sm.tm_lim = tm_lim;
    int sret = glp_simplex(lp, &sm);
    if (sret != 0 || glp_get_status(lp) != GLP_OPT)
      {
        int s = glp_get_status(lp);
        status = (s == GLP_NOFEAS || s == GLP_INFEAS || sret == GLP_ENOPFS) ? "infeasible"
                                                                            : "no solution (glp_simplex " + std::to_string(sret) + ")";
        glp_delete_prob(lp);
        return values;
      }
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_OFF;
    parm.msg_lev = GLP_MSG_OFF;
    parm.tm_lim = tm_lim;
    int ret = glp_intopt(lp, &parm);
    int st = glp_mip_status(lp);
    if ((ret == 0 || ret == GLP_ETMLIM) && (st == GLP_OPT || st == GLP_FEAS))
      {
        values.resize(n);
        for (int j = 0; j < n; ++j) { values[j] = glp_mip_col_val(lp, j + 1); }
        status = st == GLP_OPT ? "optimal" : "feasible (time limit)";
      }
    else
      {
        status = (ret == GLP_ENOPFS || st == GLP_NOFEAS) ? "infeasible"
                                                         : "no solution (glp_intopt " + std::to_string(ret) + ")";
      }
    glp_delete_prob(lp);
    return values;
  }

  /*!
   * @~japanese
   * @brief 解の値で式の値を計算する
   *
   * @param expr 式 {変数の番号: 係数}
   * @param v 変数の値
   * @return 式の値
   * @~english
   * @brief Evaluates an expression with the solution values
   *
   * @param expr expression {variable index: coefficient}
   * @param v variable values
   * @return value of the expression
   * @~
   */
  double value_of(const std::map<int, double>& expr, const std::vector<double>& v)
  {
    double s = 0;
    for (auto& [j, c] : expr) { s += c * v[j]; }
    return s;
  }

  /*!
   * @~japanese
   * @brief ロボットの空きの要約を作る (エラーのメッセージ用)
   *
   * @param robots ロボット
   * @return 要約
   * @~english
   * @brief Makes a summary of the free resources of robots (for error messages)
   *
   * @param robots robots
   * @return summary
   * @~
   */
  std::string capacity_summary(const std::vector<snmpmgr::PlacementRobot>& robots)
  {
    std::ostringstream os;
    for (auto& r : robots) { os << " " << r.name << ": CPU " << r.cpu_free << "%, " << r.mem_free_mb << "MB"; }
    return os.str();
  }

  /*!
   * @~japanese
   * @brief ILP で配置を決める
   *
   * @param ctx プラグインのコンテキスト
   * @param items 置く RTC
   * @param robots ロボット
   * @param edges 接続
   * @param primary 主ロボットの名前
   * @param args placement_args
   * @return RTC 名 -> ロボット名 (置けなければ snmpmgr::PlacementError)
   * @~english
   * @brief Decides the placement with ILP
   *
   * @param ctx plugin context
   * @param items RTCs to place
   * @param robots robots
   * @param edges connections
   * @param primary name of the primary robot
   * @param args placement_args
   * @return RTC name -> robot name (snmpmgr::PlacementError if impossible)
   * @~
   */
  std::map<std::string, std::string> solve_ilp(snmpmgr::PluginContext& ctx, const std::vector<snmpmgr::PlacementItem>& items,
                                               const std::vector<snmpmgr::PlacementRobot>& robots,
                                               const std::vector<snmpmgr::PlacementEdge>& edges, const std::string& primary,
                                               const json& args)
  {
    using snmpmgr::PlacementError;
    std::vector<std::string> missing;
    for (auto& it : items) { if (it.candidates.empty()) { missing.push_back(it.name + " (" + it.module + ") を置けるロボットがない"); } }
    if (!missing.empty()) { throw PlacementError("配置できない RTC があります", missing); }
    std::string solver = args.value("solver", std::string("auto"));
    if (solver != "auto" && solver != "glpk") { throw PlacementError("未知の ILP のソルバー " + solver + " (C++ 版は glpk だけ)"); }
    double time_limit = args.value("time_limit", 10.0);
    double tol = args.value("tolerance", 1e-6);
    std::string mode = args.value("objective", std::string("lexicographic"));
    if (mode != "lexicographic" && mode != "weighted") { throw PlacementError("未知の目的 " + mode + " (使えるのは lexicographic, weighted)"); }

    // ---- モデル
    Model m;
    std::map<std::string, const snmpmgr::PlacementRobot*> rmap;
    for (auto& r : robots) { rmap[r.name] = &r; }
    std::map<std::pair<std::string, std::string>, int> x;
    std::map<std::string, const snmpmgr::PlacementItem*> imap;
    for (auto& it : items)
      {
        imap[it.name] = &it;
        for (auto& rn : it.candidates) { if (rmap.count(rn)) { x[{it.name, rn}] = m.var("x[" + it.name + "," + rn + "]"); } }
      }
    for (auto& it : items)
      {
        std::map<int, double> c;
        for (auto& rn : it.candidates) { if (x.count({it.name, rn})) { c[x[{it.name, rn}]] = 1; } }
        m.add(c, 1, 1);
      }
    std::map<std::string, double> overhead;
    for (auto& it : items) { overhead[it.language] = std::max(overhead[it.language], it.slave_overhead_mb); }
    std::map<std::pair<std::string, std::string>, int> y;
    for (auto& r : robots)
      {
        std::map<int, double> cpu, mem;
        for (auto& it : items)
          {
            auto k = x.find({it.name, r.name});
            if (k == x.end()) { continue; }
            cpu[k->second] = it.cpu;
            mem[k->second] = it.memory_mb;
            bool running = std::find(r.languages.begin(), r.languages.end(), it.language) != r.languages.end();
            if (!running && overhead[it.language] > 0)
              {
                auto key = std::make_pair(r.name, it.language);
                if (!y.count(key))
                  {
                    y[key] = m.var("y[" + r.name + "," + it.language + "]");
                    mem[y[key]] = overhead[it.language];
                  }
                m.add({{y[key], 1}, {k->second, -1}}, 0);          // y >= x
              }
          }
        m.add(cpu, -INF, r.cpu_free + 1e-9);
        m.add(mem, -INF, r.mem_free_mb + 1e-9);
      }
    std::map<int, double> cross;
    for (auto& e : edges)
      {
        std::set<std::string> names;
        for (auto& [k, v] : x) { if (k.first == e.a || k.first == e.b) { names.insert(k.second); } }
        int z = m.var("z[" + e.a + "," + e.b + "]", 0, 1, false);
        cross[z] = e.weight;
        for (auto& rn : names)
          {
            auto xa = x.find({e.a, rn}), xb = x.find({e.b, rn});
            std::map<int, double> c1{{z, 1}}, c2{{z, 1}}, c3;
            if (xa != x.end()) { c1[xa->second] -= 1; c2[xa->second] += 1; c3[xa->second] += 1; }
            if (xb != x.end()) { c1[xb->second] += 1; c2[xb->second] -= 1; c3[xb->second] -= 1; }
            m.add(c1, 0);                                          // z >= xa - xb
            m.add(c2, 0);                                          // z >= xb - xa
            if (!rmap[rn]->link_ok) { m.add(c3, 0, 0); }           // またぐ接続の端にしない
          }
      }
    int t = m.var("t", 0, INF, false);
    for (auto& r : robots)
      {
        if (r.name == primary || r.cpu_free <= 0) { continue; }
        std::map<int, double> c{{t, 1}};
        for (auto& it : items)
          {
            auto k = x.find({it.name, r.name});
            if (k != x.end()) { c[k->second] = -it.cpu / r.cpu_free; }
          }
        m.add(c, 0);
      }
    std::map<std::string, std::map<int, double>> objs;
    objs["cross"] = cross;
    for (auto& [k, j] : x)
      {
        if (k.second != primary) { continue; }
        objs["primary_count"][j] = -1;
        objs["primary_cpu"][j] = -imap[k.first]->cpu;
      }
    objs["balance"] = {{t, 1}};

    // ---- 求解
    auto t0 = std::chrono::steady_clock::now();
    std::vector<double> values;
    std::string status;
    if (mode == "weighted")
      {
        std::map<std::string, double> w{{"cross", 1}, {"primary_count", 0}, {"primary_cpu", 0}, {"balance", 0}};
        if (args.contains("weights")) { for (auto& [k, v] : args["weights"].items()) { w[k] = v.get<double>(); } }
        std::map<int, double> obj;
        for (auto& [name, weight] : w) { for (auto& [j, c] : objs[name]) { obj[j] += weight * c; } }
        values = solve_glpk(m, obj, time_limit, status);
      }
    else
      {
        for (auto name : {"cross", "primary_count", "primary_cpu", "balance"})
          {
            values = solve_glpk(m, objs[name], time_limit, status);
            if (values.empty()) { break; }
            double best = value_of(objs[name], values);
            m.add(objs[name], -INF, best + tol * std::max(1.0, std::fabs(best)));   // 次の段では、この段の値を保つ
          }
      }
    if (values.empty())
      {
        throw PlacementError("ILP (glpk): リソースの制限内に全 RTC を配置できません (" + status + ")",
                             {"空き:" + capacity_summary(robots)});
      }
    std::map<std::string, std::string> assign;
    for (auto& [k, j] : x) { if (values[j] > 0.5) { assign[k.first] = k.second; } }
    if (assign.size() != items.size()) { throw PlacementError("ILP (glpk): ソルバーの解がすべての RTC を置いていません"); }
    double traffic = 0;
    for (auto& e : edges) { if (assign[e.a] != assign[e.b]) { traffic += e.weight; } }
    int on_primary = 0;
    for (auto& [n, r] : assign) { if (r == primary) { ++on_primary; } }
    double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os.precision(2);
    os << "ILP (glpk, " << mode << "): " << status << ", " << m.vars.size() << " variables, " << m.rows.size()
       << " constraints, " << secs << "s; cross traffic " << traffic << ", " << on_primary << " RTCs on " << primary;
    ctx.log(os.str());
    return assign;   // 空き容量・置けるロボットは SNMPManager が確かめる
  }
} // namespace

/*!
 * @~japanese
 * @brief プラグインの API のバージョンを返す
 *
 * @return SNMPMGR_PLUGIN_API_VERSION
 * @~english
 * @brief Returns the version of the plugin API
 *
 * @return SNMPMGR_PLUGIN_API_VERSION
 * @~
 */
SNMPMGR_PLUGIN_EXPORT int snmpmgr_plugin_api_version() { return SNMPMGR_PLUGIN_API_VERSION; }

/*!
 * @~japanese
 * @brief プラグインを登録する (配置のロジック ilp を登録する)
 *
 * @param ctx プラグインのコンテキスト
 * @~english
 * @brief Registers the plugin (registers the placement logic ilp)
 *
 * @param ctx plugin context
 * @~
 */
SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char*)
{
  ctx->register_placement("ilp", solve_ilp);
}
