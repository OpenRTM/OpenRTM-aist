// -*- C++ -*-
/*!
 * @file SlaveProcessObserver.hpp
 * @brief Detection of slave manager exits (corresponds to Process / ProcessCallback of feature/processcallback in OpenRTM-aist-Python)
 *
 * @~japanese
 * slave マネージャの終了の検知 (OpenRTM-aist-Python の feature/processcallback の Process / ProcessCallback に当たる)
 *
 * OpenRTM-aist C++ 2.1 のマスターマネージャは、slave を coil::launch_shell() で起動する。launch_shell は signal(SIGCHLD, SIG_IGN) を設定してから fork するので、slave の終了コードはカーネルが捨ててしまう。そこで snmpmanager (実行ファイル) が coil::launch_shell を定義して置き換える (libRTC2 は PLT 経由で呼ぶので、実行ファイルの定義が使われる。CMake の ENABLE_EXPORTS でシンボルを公開している)。有効にすると、slave を自分の子として起動し (sh -c "exec \<コマンド\>" なので pid は slave 自身)、終わるのを待って、コールバックに終了コード (シグナルなら負の値) を渡す。無効なら元の launch_shell を呼ぶ。
 *
 * Windows では launch_shell は OpenRTM の DLL の中で呼ばれるので置き換えられない。代わりに、RTC の一覧で見つけた slave の pid (slave の設定の manager.pid) を watch() に渡し、そのプロセスの終了を待つ (OpenProcess)。終了コードは GetExitCodeProcess の値 (例外で落ちたなら 0xC0000005 などを int にした負の値)。
 * @~english
 * The master manager of OpenRTM-aist C++ 2.1 starts slaves with coil::launch_shell(). launch_shell forks after setting signal(SIGCHLD, SIG_IGN), so the kernel discards the exit codes of slaves. Therefore snmpmanager (the executable) defines coil::launch_shell to replace it (libRTC2 calls it via the PLT, so the definition in the executable is used; the symbol is exported with ENABLE_EXPORTS in CMake). When enabled, slaves are started as children of this process (sh -c "exec \<command\>", so the pid is the slave itself), and when one exits, its exit code (negative for signals) is passed to the callback. When disabled, the original launch_shell is called.
 *
 * On Windows, launch_shell is called inside the OpenRTM DLL and cannot be replaced. Instead, the pid of a slave found in the RTC list (manager.pid in the slave's configuration) is passed to watch(), and the exit of that process is awaited (OpenProcess). The exit code is the value of GetExitCodeProcess (a negative int such as 0xC0000005 if it crashed by an exception).
 * @~
 *
 * @date $Date: 2026-11-13 16:24:29 $
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

#ifndef SNMPMGR_PROCESSOBSERVER_HPP
#define SNMPMGR_PROCESSOBSERVER_HPP

#include <functional>
#include <string>

namespace snmpmgr
{
  namespace process
  {
    /*!
     * @~japanese
     * @brief 終わった slave の情報 (pid、マネージャ名、起動のコマンド)
     * @~english
     * @brief Information of an exited slave (pid, manager name, launch command)
     * @~
     */
    struct ExitInfo
    {
      int pid{-1};
      std::string manager_name;   // "-o manager.instance_name:manager_%p" の %p を pid に置き換えたもの
      std::string command;
    };
    /*!
     * @~japanese
     * @brief 終了コードを取れなかったことを表す値 (ほかに回収された)
     * @~english
     * @brief Value meaning that the exit code could not be obtained (reaped by someone else)
     * @~
     */
    constexpr int kUnknownExit = -1000;   // 終了コードを取れなかった (ほかに回収された)

    /*!
     * @~japanese
     * @brief slave が終わったときに呼ばれる関数 (終了コード, 情報)
     * @~english
     * @brief Function called when a slave exits (exit code, information)
     * @~
     */
    using Callback = std::function<void(int code, const ExitInfo& info)>;
    /*!
     * @~japanese
     * @brief コールバックを設定し、以後の slave の起動を観測する
     *
     * @param cb コールバック (nullptr なら元の launch_shell に戻す)
     * @~english
     * @brief Sets the callback and observes subsequent slave launches
     *
     * @param cb callback (nullptr restores the original launch_shell)
     * @~
     */
    void set_callback(Callback cb);
    /*!
     * @~japanese
     * @brief slave の pid を観測する (Windows。同じ pid は 1 回だけ。Linux では何もしない)
     *
     * @param pid プロセス ID
     * @param manager_name slave マネージャの名前
     * @~english
     * @brief Observes the pid of a slave (Windows; each pid only once; does nothing on Linux)
     *
     * @param pid process ID
     * @param manager_name name of the slave manager
     * @~
     */
    void watch(int pid, const std::string& manager_name);
    /*!
     * @~japanese
     * @brief launch_shell を置き換えて観測するか (Linux)。false なら watch() で観測する (Windows)
     *
     * @return 置き換えるなら true
     * @~english
     * @brief Whether it observes by replacing launch_shell (Linux). If false, it observes with watch() (Windows)
     *
     * @return true if replacing
     * @~
     */
    bool replaces_launch();
    /*!
     * @~japanese
     * @brief 起動のコマンドから slave マネージャ名を取り出す (テスト用に公開)
     *
     * @param command 起動のコマンド
     * @param pid プロセス ID (%p を置き換える)
     * @return マネージャ名
     * @~english
     * @brief Extracts the slave manager name from the launch command (public for tests)
     *
     * @param command launch command
     * @param pid process ID (replaces %p)
     * @return manager name
     * @~
     */
    std::string manager_name_of(const std::string& command, int pid);
  } // namespace process
} // namespace snmpmgr

#endif
