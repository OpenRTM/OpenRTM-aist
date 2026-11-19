// -*- C++ -*-
/*!
 * @file OSPlatform.hpp
 * @brief OS-dependent parts (Linux and Windows). The other sources do not include OS headers directly
 *
 * @~japanese
 * OS に依存する部分 (Linux と Windows)。ほかのソースは OS のヘッダを直接読まない
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

#ifndef SNMPMGR_PLATFORM_HPP
#define SNMPMGR_PLATFORM_HPP

#include <cstdint>
#include <optional>
#include <set>
#include <string>

namespace snmpmgr
{
  namespace platform
  {
    /*!
     * @~japanese
     * @brief 自分の IPv4 アドレス (127.* を除く) を返す
     *
     * @return IP アドレスの集合
     * @~english
     * @brief Returns the local IPv4 addresses (excluding 127.*)
     *
     * @return set of IP addresses
     * @~
     */
    std::set<std::string> local_ipv4_addresses();
    /*!
     * @~japanese
     * @brief 実行ファイルのあるディレクトリを返す
     *
     * @return ディレクトリ (分からなければ ".")
     * @~english
     * @brief Returns the directory of the executable
     *
     * @return directory ("." if unknown)
     * @~
     */
    std::string executable_dir();
    /*!
     * @~japanese
     * @brief "a.b.c.d" を 32 ビットの値 (ホストのバイト順) にする
     *
     * @param ip IP アドレス
     * @return 値 (正しくなければ nullopt)
     * @~english
     * @brief Converts "a.b.c.d" into a 32-bit value (host byte order)
     *
     * @param ip IP address
     * @return value (nullopt if invalid)
     * @~
     */
    std::optional<uint32_t> parse_ipv4(const std::string& ip);
    /*!
     * @~japanese
     * @brief 32 ビットの値 (ホストのバイト順) を "a.b.c.d" にする
     *
     * @param v 値
     * @return IP アドレス
     * @~english
     * @brief Converts a 32-bit value (host byte order) into "a.b.c.d"
     *
     * @param v value
     * @return IP address
     * @~
     */
    std::string format_ipv4(uint32_t v);

    /*!
     * @~japanese
     * @brief 共有ライブラリ (Linux: .so、Windows: .dll) を開く
     *
     * @param path パス
     * @param error 失敗したときの理由
     * @return ハンドル (失敗なら nullptr)
     * @~english
     * @brief Opens a shared library (Linux: .so, Windows: .dll)
     *
     * @param path path
     * @param error reason on failure
     * @return handle (nullptr on failure)
     * @~
     */
    void* dl_open(const std::string& path, std::string& error);
    /*!
     * @~japanese
     * @brief 共有ライブラリのシンボルを探す
     *
     * @param handle ハンドル
     * @param name シンボルの名前
     * @return アドレス (なければ nullptr)
     * @~english
     * @brief Looks up a symbol of a shared library
     *
     * @param handle handle
     * @param name symbol name
     * @return address (nullptr if not found)
     * @~
     */
    void* dl_symbol(void* handle, const char* name);
    /*!
     * @~japanese
     * @brief 共有ライブラリを閉じる
     *
     * @param handle ハンドル
     * @~english
     * @brief Closes a shared library
     *
     * @param handle handle
     * @~
     */
    void dl_close(void* handle);
    /*!
     * @~japanese
     * @brief プラグインのファイルの拡張子を返す (ファイル名の候補は lib\<名前\>.so / \<名前\>.so、Windows は \<名前\>.dll / lib\<名前\>.dll)
     *
     * @return 拡張子
     * @~english
     * @brief Returns the extension of plugin files (candidate names are lib\<name\>.so / \<name\>.so, or \<name\>.dll / lib\<name\>.dll on Windows)
     *
     * @return extension
     * @~
     */
    const char* plugin_suffix();
  } // namespace platform
} // namespace snmpmgr

#endif
