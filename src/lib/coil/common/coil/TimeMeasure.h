// -*- C++ -*-
/*!
 * @file TimeMeasure.h
 * @brief Periodic time measurement class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_TIMEMEASURE_H
#define COIL_TIMEMEASURE_H

#include <coil/TimeValue.h>
#include <vector>

namespace coil
{

  /*!
   * @if jp
   *
   * @class TimeMeasure
   * @brief TimeMeasure クラス
   *
   * このクラスは、コード実行時間の統計を取る為に使用します。
   * get_stat を使用してコード実行の最大・最小・平均・標準偏差時間を計測できます。
   *
   * @else
   *
   * @class TimeMeasure
   * @brief TimeMeasure class
   *
   * This class is used for getting statistics of code execution time.
   * Using get_stat you can get maximum, minimum, mean and standard
   * deviation time for code execution.
   *
   * @endif
   */
  class TimeMeasure
  {
  public:
    /*!
     * @if jp
     *
     * @brief 時間統計用構造体
     *
     * @else
     *
     * @brief Structure for time statistics
     *
     * @endif
     */
    struct Statistics
    {
      double max_interval;
      double min_interval;
      double mean_interval;
      double std_deviation;
    };

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * 時間統計のプロファイリング
     *
     * @else
     *
     * @brief Constructor
     *
     * Time Statistics object for profiling.
     *
     * @endif
     */
    explicit TimeMeasure(unsigned long buflen = 100);

    /*!
     * @if jp
     *
     * @brief 時間統計の計測を開始する
     *
     * 時間統計の計測を開始する
     *
     * @else
     *
     * @brief Begin time measurement for time statistics
     *
     * Begin time measurement for time statistics.
     *
     * @endif
     */
    void tick();

    /*!
     * @if jp
     *
     * @brief 時間統計の計測を終了する
     *
     * 時間統計の計測を終了する
     *
     * @else
     *
     * @brief Finish time measurement for time statistics
     *
     * End of time measurement for time statistics.
     *
     * @endif
     */
    void tack();

    /*!
     * @if jp
     *
     * @brief 経過時間を取得する
     *
     * 経過時間を取得する
     *
     * @return TimeValue オブジェクト
     *
     * @else
     *
     * @brief Get a interval time
     *
     * Get a interval time.
     *
     * @return TimeValue object
     *
     * @endif
     */
    coil::TimeValue& interval();

    /*!
     * @if jp
     *
     * @brief 統計関連データの初期化
     *
     * 統計関連データの初期化
     *
     * @else
     *
     * @brief Initialize for statistics related data
     *
     * Initialize for statistics related data.
     *
     * @endif
     */
    void reset();

    /*!
     * @if jp
     *
     * @brief 時間統計バッファサイズを取得する
     *
     * 時間統計バッファサイズを取得する
     *
     * @return 計測件数
     *
     * @else
     *
     * @brief Get number of time measurement buffer
     *
     * Get number of time measurement buffer.
     *
     * @return Measurement count
     *
     * @endif
     */
    unsigned long int count() const;

    /*!
     * @if jp
     *
     * @brief 統計データの総計を取得する
     *
     * 統計データの総計を取得する
     *
     * @param max_interval 最大値 [ns]
     * @param min_interval 最小値 [ns]
     * @param mean_interval 平均値 [ns]
     * @param stddev 標準偏差値
     *
     * @return true: データあり, false: データなし
     *
     * @else
     *
     * @brief Get total statistics
     *
     * Get total statistics.
     *
     * @param max_interval Max value [ns]
     * @param min_interval Min value [ns]
     * @param mean_interval Mean value [ns]
     * @param stddev Standard deviation value
     *
     * @return true: Data found, false: Data not found
     *
     * @endif
     */
    bool getStatistics(double &max_interval,
                       double &min_interval,
                       double &mean_interval,
                       double &stddev);

    /*!
     * @if jp
     *
     * @brief 統計結果を取得する
     *
     * 統計結果を取得する
     *
     * @return 統計結果
     *
     * @else
     *
     * @brief Get statistics result
     *
     * Get statistics result.
     *
     * @return Statistics result
     *
     * @endif
     */
    Statistics getStatistics();

  private:
    std::vector<coil::TimeValue> m_record;
    coil::TimeValue m_begin;
    coil::TimeValue m_interval;

    unsigned long int m_count;
    const unsigned long int m_countMax;

    bool m_recurred;
  };
} // namespace coil
#endif  // COIL_TIMEMEASURE_H
