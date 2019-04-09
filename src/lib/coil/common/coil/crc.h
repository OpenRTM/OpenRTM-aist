// -*- C++ -*-
/*!
 * @file crc.h
 * @brief CRC calculation functions
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_CRC_H
#define COIL_CRC_H

#include <sys/types.h>

namespace coil
{
  /*
   * @if jp
   *
   * @brief CRC-16 計算関数
   *
   * CRC種類: CRC-CCITT
   * CRC多項式:  x^16 + x^12 + x^5 + 1 (0x1021)
   * 初期値:  0xFFFF
   * 出力XOR: 0x0000
   * 入力ビット反転: なし
   * 出力ビット反転: なし
   * ビットシフト: 左
   *
   * @param str データストリーム
   * @param len データ長
   *
   * @return 計算結果
   *
   * @else
   *
   * @brief CRC-16 calculation function
   *
   * CRC type: CRC-CCITT
   * CRC generator polynomial:  x^16 + x^12 + x^5 + 1 (0x1021)
   * Initial value: 0xFFFF
   * Output XOR: 0x0000
   * Input bit inversion: None
   * Output bit inversion: None
   * Bit shift: left
   *
   * @param str Data stream
   * @param len Data length
   *
   * @return Result calculation
   *
   * @endif
   */
  unsigned short crc16(const char* str, size_t len);

  /*!
   * @if jp
   *
   * @brief CRC-32 計算関数
   *
   * CRC種類: RFC2083 Appendix 15
   * CRC多項式:  0xedb88320L
   * 初期値:  0xFFFFFFFF
   * 出力XOR: 0xFFFFFFFF
   * 入力ビット反転: なし
   * 出力ビット反転: なし
   * ビットシフト: 右
   *
   * @param str データストリーム
   * @param len データ長
   *
   * @return 計算結果
   *
   * @else
   *
   * @brief CRC-32 calculation function
   *
   * CRC type: RFC2083 Appendix 15
   *          http://www.faqs.org/rfcs/rfc2083.html
   *          http://www.efg2.com/Lab/Mathematics/CRC.htm
   * CRC polynomial:  0xedb88320L
   * Initial value: 0xFFFFFFFF
   * Output XOR: 0xFFFFFFFF
   * Input bit inversion: None
   * Output bit inversion: None
   * Bit shift: right
   *
   * @param str Data stream
   * @param len Data length
   *
   * @return Result calculation
   *
   * @endif
   */
  unsigned long crc32(const char* str, size_t len);
} // namespace coil

#endif  // COIL_CRC_H
