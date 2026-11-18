// -*- C++ -*-
/*!
 * @file WifiRssi.cpp
 * @brief wifi_rssi.exe: outputs the signal strength (RSSI, dBm) of the connected Wi-Fi in one line (Windows)
 *
 * @~japanese
 * wifi_rssi.exe: 接続中の Wi-Fi の電波強度 (RSSI, dBm) を 1 行で出力する (Windows)
 *
 * Linux の実機の interface_info.sh と同じ役割で、Net-SNMP (snmpd) の extend から呼ぶ (extend wifiStrength C:/.../wifi_rssi.exe)。SNMPManager は rtc.conf の snmp.wifi_client_oid (extend wifiStrength の出力の OID) で読む。
 * @verbatim
 * 使い方
 *   wifi_rssi.exe              接続中の最初の無線 LAN インターフェースの RSSI (例: -52)
 *   wifi_rssi.exe <文字列>      説明 (例: "Intel(R) Wi-Fi 6 AX201") にこの文字列を含むインターフェース
 *   wifi_rssi.exe --list       インターフェースの一覧 (説明・状態・RSSI・信号の品質 %)
 * @endverbatim
 * 接続していない・無線 LAN がないときは、何も出力せずに終了コード 1 で終わる。
 * 値は Native Wifi API の WlanQueryInterface(wlan_intf_opcode_rssi) で取る。netsh の出力は表示の言語で項目名が変わり、古い Windows には Rssi の行がないので使わない。
 * @~english
 * Plays the same role as interface_info.sh on real Linux robots, and is called from extend of Net-SNMP (snmpd) (extend wifiStrength C:/.../wifi_rssi.exe). SNMPManager reads it with snmp.wifi_client_oid in rtc.conf (the OID of the output of extend wifiStrength).
 * @verbatim
 * Usage
 *   wifi_rssi.exe              RSSI of the first connected wireless LAN interface (e.g. -52)
 *   wifi_rssi.exe <string>     the interface whose description (e.g. "Intel(R) Wi-Fi 6 AX201") contains the string
 *   wifi_rssi.exe --list       list of interfaces (description, state, RSSI, signal quality %)
 * @endverbatim
 * When not connected or without wireless LAN, it outputs nothing and exits with code 1.
 * The value is obtained with WlanQueryInterface(wlan_intf_opcode_rssi) of the Native Wifi API. The output of netsh is not used because its item names depend on the display language and old Windows versions have no Rssi line.
 * @~
 *
 * @date $Date: 2026-11-13 16:24:33 $
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wlanapi.h>

#include <cstdio>
#include <cstring>
#include <string>

namespace
{
  /*!
   * @~japanese
   * @brief ワイド文字列を UTF-8 にする
   *
   * @param w ワイド文字列
   * @return UTF-8 の文字列
   * @~english
   * @brief Converts a wide string into UTF-8
   *
   * @param w wide string
   * @return UTF-8 string
   * @~
   */
  std::string narrow(const wchar_t* w)
  {
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    std::string s(n > 0 ? n - 1 : 0, '\0');
    if (n > 1) { WideCharToMultiByte(CP_UTF8, 0, w, -1, s.data(), n, nullptr, nullptr); }
    return s;
  }

  /*!
   * @~japanese
   * @brief インターフェースの状態の名前を返す
   *
   * @param s 状態
   * @return 名前
   * @~english
   * @brief Returns the name of an interface state
   *
   * @param s state
   * @return name
   * @~
   */
  const char* state_name(WLAN_INTERFACE_STATE s)
  {
    switch (s)
      {
      case wlan_interface_state_connected: return "connected";
      case wlan_interface_state_disconnected: return "disconnected";
      case wlan_interface_state_authenticating: return "authenticating";
      case wlan_interface_state_associating: return "associating";
      case wlan_interface_state_discovering: return "discovering";
      default: return "other";
      }
  }

  /*!
   * @~japanese
   * @brief インターフェースの RSSI を取る
   *
   * @param h WLAN のハンドル
   * @param guid インターフェースの GUID
   * @param rssi RSSI (dBm)
   * @return 取れれば true
   * @~english
   * @brief Gets the RSSI of an interface
   *
   * @param h WLAN handle
   * @param guid GUID of the interface
   * @param rssi RSSI (dBm)
   * @return true if obtained
   * @~
   */
  bool query_rssi(HANDLE h, const GUID& guid, long& rssi)
  {
    DWORD size = 0;
    PVOID data = nullptr;
    WLAN_OPCODE_VALUE_TYPE type;
    if (WlanQueryInterface(h, &guid, wlan_intf_opcode_rssi, nullptr, &size, &data, &type) != ERROR_SUCCESS || !data)
      {
        return false;
      }
    rssi = *static_cast<LONG*>(data);
    WlanFreeMemory(data);
    return true;
  }

  /*!
   * @~japanese
   * @brief インターフェースの信号の品質 (%) を取る
   *
   * @param h WLAN のハンドル
   * @param guid インターフェースの GUID
   * @return 品質 (%) (取れなければ -1)
   * @~english
   * @brief Gets the signal quality (%) of an interface
   *
   * @param h WLAN handle
   * @param guid GUID of the interface
   * @return quality (%) (-1 if not available)
   * @~
   */
  int query_quality(HANDLE h, const GUID& guid)
  {
    DWORD size = 0;
    PVOID data = nullptr;
    WLAN_OPCODE_VALUE_TYPE type;
    if (WlanQueryInterface(h, &guid, wlan_intf_opcode_current_connection, nullptr, &size, &data, &type) != ERROR_SUCCESS ||
        !data)
      {
        return -1;
      }
    int q = static_cast<int>(static_cast<WLAN_CONNECTION_ATTRIBUTES*>(data)->wlanAssociationAttributes.wlanSignalQuality);
    WlanFreeMemory(data);
    return q;
  }
} // namespace

/*!
 * @~japanese
 * @brief 入口
 *
 * @param argc 引数の数
 * @param argv 引数
 * @return 終了コード (RSSI を出せば 0、出せなければ 1)
 * @~english
 * @brief Entry point
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return exit code (0 if RSSI was output, 1 otherwise)
 * @~
 */
int main(int argc, char** argv)
{
  bool list = argc > 1 && std::strcmp(argv[1], "--list") == 0;
  std::string want = (argc > 1 && !list) ? argv[1] : "";
  DWORD version = 0;
  HANDLE h = nullptr;
  if (WlanOpenHandle(2, nullptr, &version, &h) != ERROR_SUCCESS) { return 1; }   // WLAN AutoConfig が動いていない
  PWLAN_INTERFACE_INFO_LIST ifs = nullptr;
  int rc = 1;
  if (WlanEnumInterfaces(h, nullptr, &ifs) == ERROR_SUCCESS && ifs)
    {
      for (DWORD i = 0; i < ifs->dwNumberOfItems; ++i)
        {
          const WLAN_INTERFACE_INFO& info = ifs->InterfaceInfo[i];
          std::string desc = narrow(info.strInterfaceDescription);
          long rssi = 0;
          bool connected = info.isState == wlan_interface_state_connected;
          bool have = connected && query_rssi(h, info.InterfaceGuid, rssi);
          if (list)
            {
              std::printf("%s\t%s\t%s\t%d%%\n", desc.c_str(), state_name(info.isState),
                          have ? std::to_string(rssi).c_str() : "-", connected ? query_quality(h, info.InterfaceGuid) : -1);
              rc = 0;
              continue;
            }
          if (!want.empty() && desc.find(want) == std::string::npos) { continue; }
          if (have)
            {
              std::printf("%ld\n", rssi);
              rc = 0;
              break;
            }
        }
      WlanFreeMemory(ifs);
    }
  WlanCloseHandle(h, nullptr);
  return rc;
}
