// -*- C++ -*-
/*!
 * @file Affinity.h
 * @brief  Processor affinity operation class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_AFFINITY_H
#define COIL_AFFINITY_H

#include <string>
#include <vector>
#include <coil/Singleton.h>
#include <coil/TimeValue.h>
#include <coil/Mutex.h>
#include <coil/Guard.h>

namespace coil
{
  typedef std::vector<unsigned int> CpuMask;
  /*!
   * @if jp
   * @brief プロセスのCPU affinityを取得する
   * @prop cpu_mask 現在設定されている affinity が CPU IDの
   *                std::vector<unsigned int> で返される。
   * @return True: 成功、False: 失敗
   * @else
   * @brief Getting current process CPU affinity
   * @prop cpu_mask Current CPU affinity mask is returned as CPU ID in
   *                std::vector<unsigned int>.
   * @return True: success, False: fail
   * @endif
   */
  bool getProcCpuAffinity(CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief プロセスのCPU affinityを設定する
   * @prop cpu_mask 設定する CPU affinity を CPU ID の
   *                std::vector<unsigned int> リストで与える
   * @return True: 成功、False: 失敗
   * @else
   * @brief Setting process CPU affinity
   * @prop cpu_mask CPU affinity mask to be set is given with CPU ID in
   *                std::vector<unsigned int> list.
   * @return True: success, False: fail
   * @endif
   */
  bool setProcCpuAffinity(const CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief プロセスのCPU affinityを文字列で設定する
   * @prop cpu_mask 設定する CPU affinity を CPU ID のカンマ区切り文字
   *                列のリストで与える
   * @return True: 成功、False: 失敗
   * @else
   * @brief Setting process CPU affinity with string
   * @prop cpu_mask CPU affinity mask to be set is given with comma
   *                separated CPU ID string.
   * @return True: success, False: fail
   * @endif
   */
  bool setProcCpuAffinity(const std::string& cpu_mask);

  /*!
   * @if jp
   * @brief スレッドのCPU affinityを取得する
   * @prop cpu_mask 現在設定されている affinity が CPU IDの
   *                std::vector<unsigned int> で返される。
   * @return True: 成功、False: 失敗
   * @else
   * @brief Getting current process CPU affinity
   * @prop cpu_mask Current CPU affinity mask is returned as CPU ID in
   *                std::vector<unsigned int>.
   * @return True: success, False: fail
   * @endif
   */
  bool getThreadCpuAffinity(CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief スレッドのCPU affinityを設定する
   * @prop cpu_mask 設定する CPU affinity を CPU ID の
   *                std::vector<unsigned int> リストで与える
   * @return True: 成功、False: 失敗
   * @else
   * @brief Setting process CPU affinity
   * @prop cpu_mask CPU affinity mask to be set is given with CPU ID in
   *                std::vector<unsigned int> list.
   * @return True: success, False: fail
   * @endif
   */
  bool setThreadCpuAffinity(const CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief スレッドのCPU affinityを文字列で設定する
   * @prop cpu_mask 設定する CPU affinity を CPU ID のカンマ区切り文字
   *                列のリストで与える
   * @return True: 成功、False: 失敗
   * @else
   * @brief Setting process CPU affinity with string
   * @prop cpu_mask CPU affinity mask to be set is given with comma
   *                separated CPU ID string.
   * @return True: success, False: fail
   * @endif
   */
  bool setThreadCpuAffinity(const std::string& mask);

}; // namespace coil
#endif // COIL_AFFINITY_H
