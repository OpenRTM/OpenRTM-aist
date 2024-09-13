// -*- C++ -*-
/*!
 * @file DirectPortBase.h
 * @brief DirectPortBase class
 * @date $Date: 2018-1-23 03:08:05 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_DIRECTPORTBASE_H
#define RTC_DIRECTPORTBASE_H







namespace RTC
{
  /*!
   * @if jp
   * @class DirectPortBase
   * @brief direct接続用Port基底クラス
   *
   *
   * @since 1.2.0
   *
   * @else
   * @class DirectPortBase
   * @brief
   *
   *
   *
   * @since 1.2.0
   *
   * @endif
   */
  class DirectPortBase
  {
  public:
     /*!
      * @if jp
      * @brief デストラクタ
      *
      *
      * @else
      * @brief Destructor
      *
      *
      * @endif
      */
     virtual ~DirectPortBase() = default;



  };
} // namespace RTC

#endif // RTC_DIRECTINPORTBASE_H
