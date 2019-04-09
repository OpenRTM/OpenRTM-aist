// -*- C++ -*-
/*!
 * @file Timevalue.h
 * @brief Timevalue class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_TIMEVALUE_H
#define COIL_TIMEVALUE_H

namespace coil
{

#define TIMEVALUE_ONE_SECOND_IN_USECS 1000000  // 1 [sec] = 1000000 [usec]

  /*!
   * @if jp
   *
   * @class TimeValue
   * @brief TimeValue クラス
   *
   * @else
   *
   * @class TimeValue
   * @brief TimeValue class
   *
   * @endif
   */
  class TimeValue
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     * 指定された秒，マイクロ秒で初期化する。
     *
     * @param sec 秒(デフォルト値:ゼロ)
     * @param usec マイクロ秒(デフォルト値:ゼロ)
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     * Initialize with the specified second and micro second.
     *
     * @param sec Second(The default value:0)
     * @param usec Micro second(The default value:0)
     *
     * @endif
     */
    TimeValue(long sec = 0, long usec = 0);

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     * 指定された秒，マイクロ秒で初期化する。
     *
     * @param timeval (秒 * 1000000 + マイクロ秒)
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     * Initialize with the specified second and micro second.
     *
     * @param timeval (Second * 1000000 + Micro second)
     *
     * @endif
     */
    explicit TimeValue(double timeval);

    /*!
     * @if jp
     *
     * @brief 秒単位の値を取得する
     *
     * 秒単位の値を取得する
     *
     * @return 値
     *
     * @else
     *
     * @brief Get value of second time scale
     *
     * Get value of second time scale.
     *
     * @return value
     *
     * @endif
     */
    inline long int sec() const {return m_sec;}

    /*!
     * @if jp
     *
     * @brief マイクロ秒単位の値を取得する
     *
     * マイクロ秒単位の値を取得する
     *
     * @return 値
     *
     * @else
     *
     * @brief Get value of micro second time scale
     *
     * Get value of micro second time scale.
     *
     * @return value
     *
     * @endif
     */
    inline long int usec() const {return m_usec;}

    /*!
     * @if jp
     *
     * @brief 時間減算
     *
     * 設定された時間から引数で与えられた時間を減算する。
     *
     * @param tm 減算時間
     *
     * @return 減算結果
     *
     * @else
     *
     * @brief Time subtraction
     *
     * Subtract the time given by the argument from the set time.
     *
     * @param tm Subtracted time
     *
     * @return Subtraction result
     *

     * @endif
     */
    TimeValue operator-(TimeValue& tm);

    /*!
     * @if jp
     *
     * @brief 時間加算
     *
     * 設定された時間に引数で与えられた時間を加算する。
     *
     * @param tm 加算時間
     *
     * @return 加算結果
     *
     * @else
     *
     * @brief Time addition
     *
     * Add the time given by the argument to the set time.
     *
     * @param tm Added time
     *
     * @return Addition result
     *
     * @endif
     */
    TimeValue operator+(TimeValue& tm);

    /*!
     * @if jp
     *
     * @brief double型→時間型変換
     *
     * 引数で与えられたdouble型を時間型に変換する。
     *
     * @param time 変換元値
     *
     * @return 変換結果
     *
     * @else
     *
     * @brief Convert double type into time type
     *
     * Convert double type given by the argument into time type.
     *
     * @param time the original value
     *
     * @return Conversion result
     *
     * @endif
     */
    TimeValue operator=(double time);

    /*!
     * @if jp
     *
     * @brief 時間型→double型変換
     *
     * 保持している内容をdouble型に変換する。
     *
     * @return double型変換結果
     *
     * @else
     *
     * @brief Convert time type into double type
     *
     * Convert held information into double type.
     *
     * @return Result for conversion of double type
     *
     * @endif
     */
    operator double() const;

    /*!
     * @if jp
     *
     * @brief 符号判定
     *
     * 保持している内容の符号を判定する。
     *
     * @return 正ならば1を、負ならば-1を、0ならば0
     *
     * @else
     *
     * @brief Sign judgment
     *
     * Judge sign of the held contents
     *
     * @return 1 if the return value is Plus sign, -1 if Minus, and 0 if 0.
     *
     * @endif
     */
    int sign() const;

    //  private:

    /*!
     * @if jp
     *
     * @brief 正規化
     *
     * 値の表現を正準形式に正規化する。
     *
     * @else
     *
     * @brief Normalize
     *
     * Normalize the value expression into a canonical form.
     *
     * @endif
     */
    void normalize();

  private:
    long int m_sec;
    long int m_usec;
  };
} // namespace coil

#endif  // COIL_TIMEVALUE_H
