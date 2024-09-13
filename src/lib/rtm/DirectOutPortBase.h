// -*- C++ -*-
/*!
 * @file DirectOutPortBase.h
 * @brief DirectOutPortBase class
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

#ifndef RTC_DIRECTOUTPORTBASE_H
#define RTC_DIRECTOUTPORTBASE_H


#include <rtm/DirectPortBase.h>




namespace RTC
{
  /*!
   * @if jp
   * @class DirectOutPortBase
   * @brief direct接続用OutPort基底クラス
   *
   *
   * @since 1.2.0
   *
   * @else
   * @class DirectOutPortBase
   * @brief 
   *
   * 
   *
   * @since 1.2.0
   *
   * @endif
   */
  template <class DataType>
  class DirectOutPortBase : public DirectPortBase
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
     ~DirectOutPortBase() override = default;
     /*!
      * @if jp
      * @brief データの取得
      * 
      * @param data データを格納する変数
      *
      * @else
      * @brief 
      *
      * @param data
      *
      * @endif
      */
     virtual void read(DataType& data) = 0;
     /*!
      * @if jp
      * @brief 新規データの存在確認
      *
      * @return true：新規データあり
      *
      * @else
      * @brief
      *
      * @return
      *
      * @endif
      */
     virtual bool isNew() = 0;
     /*!
      * @if jp
      * @brief 新規データが無いことを確認
      *
      * @return true：新規データなし
      *
      * @else
      * @brief
      *
      * @return
      *
      * @endif
      */
     virtual bool isEmpty() = 0;
    
  protected:
  };
} // namespace RTC

#endif // RTC_DIRECTOUTPORTBASE_H
