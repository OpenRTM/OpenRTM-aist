// -*- C++ -*-
/*!
 * @file DirectInPortBase.h
 * @brief DirectInPortBase class
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

#ifndef RTC_DIRECTINPORTBASE_H
#define RTC_DIRECTINPORTBASE_H



#include <rtm/DirectPortBase.h>



namespace RTC
{
  /*!
   * @if jp
   * @class DirectInPortBase
   * @brief direct接続用InPort基底クラス
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
  class DirectInPortBase : public DirectPortBase
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
      ~DirectInPortBase() override = default;



  
     /*!
      * @if jp
      *
      * @brief 最新データが存在するか確認する
      * 
      * InPortに未読の最新データが到着しているかをbool値で返す。
      * InPortが未接続の場合、および接続コネクタのバッファがEmpty
      * の場合にはfalseを返す。
      *
      * @return true 未読の最新データが存在する
      *         false 未接続またはバッファにデータが存在しない。
      * 
      * @else
      *
      * @brief Check whether the data is newest
      * 
      * Check whether the data stored at a current buffer position is newest.
      *
      * @return Newest data check result
      *         ( true:Newest data. Data has not been readout yet.
      *          false:Past data．Data has already been readout.)
      * 
      * @endif
      */
     virtual bool isNew() = 0;

     /*!
      * @if jp
      *
      * @brief バッファが空かどうか確認する
      * 
      * InPortのバッファが空かどうかを bool 値で返す。
      * 空の場合は true, 未読データがある場合は false を返す。
      *
      * @return true  バッファは空
      *         false バッファに未読データがある
      * 
      * @else
      *
      * @brief Check whether the data is newest
      * 
      * Check whether the data stored at a current buffer position is newest.
      *
      * @return Newest data check result
      *         ( true:Newest data. Data has not been readout yet.
      *          false:Past data．Data has already been readout.)
      * 
      * @endif
      */
     virtual bool isEmpty() = 0;

     /*!
      * @if jp
      * @brief データの書き込み
      *
      * @param data データ
      *
      * @else
      * @brief
      *
      * @param data
      *
      * @endif
      */
     virtual void write(DataType& data) = 0;


  protected:
    

  };
} // namespace RTC

#endif // RTC_DIRECTINPORTBASE_H
