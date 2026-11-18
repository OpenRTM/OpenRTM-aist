# SNMPManager (C++ 版)

`SNMPManager.py` を C++ (OpenRTM-aist 2.1 C++) で書き直したマスターマネージャです。Linux と Windows で動きます。
rtc.conf の `snmp.*` と構成ファイル (`snmp.migration_config` の JSON)、配置プロファイル (`placement.xml`)、
SQLite の保存形式は Python 版と同じなので、`docker-test` の構成ファイルや `snmpmgr_store.py` の CLI がそのまま使えます。

## 機能 (Python 版と同じ)

| 機能 | 実装 |
|---|---|
| ロボットの発見 (sysName / トラップ) と定期的な SNMP GET (CPU・メモリ・Wi-Fi・IF) | `src/SNMPClient.cpp` (Net-SNMP) か `src/SNMPBuiltinClient.cpp` + `src/SNMPBerCodec.cpp` (組み込みの SNMPv2c), `src/SNMPManager.cpp` |
| 起動時の構成 (manual / auto / plugin) とマイグレーション、条件 (AND/OR, 継続時間, クールダウン) | `src/SNMPManager.cpp`, `src/RTCOperations.cpp` |
| 自動配置 (cgroup の制限と使用量、`get_loadable_modules()` の確認、接続を同じコンテナに寄せる全探索) | `src/RTCPlacement.cpp` |
| ロードできない RTC がある場合の起動失敗 (`SYSTEM STARTUP FAILED`, `startup_status.json`) | `SNMPManager::auto_plan` |
| 死活監視 (sysUpTime のプローブ、ハートビートのトラップ)、レイテンシ・ジッタ・パケットロス | `SNMPManager::liveness_loop` ほか |
| 監視情報の保存 (SQLite: `metrics`, `events`) | `src/MonitoringStore.cpp` |
| プラグイン (条件・処理・配置・目的関数・イベント) | `src/PluginManager.cpp`, `include/snmpmgr/Plugin.hpp`, `plugins/*.cpp` |
| 配置のロジックの選択 (`logic: optimal / first_fit / objective`、目的関数の重み `objective.weights`、目的関数のプラグイン `objective_plugin`) | `src/RTCPlacement.cpp`, `SNMPManager::objective_fn` |
| ロボット間の制約 (`link_constraints`: レイテンシ・ジッタ・ロス・RSSI) | `link_ok`, `SNMPManager::robot_capacity` |
| RTC の一覧 (`snmp.inventory.interval`。名前・型・言語・状態・slave) | `rtcops::list_components`, `SNMPManager::refresh_inventory` |
| 待機モジュールの優先 (`standby`) | `SNMPManager::find_standby`、`apply_plan`、`auto_plan` |
| メモリの予約 (`reservation`) | `SNMPManager::reserve_mb` / `reserved_on` / `check_reservation` |
| 実行ロボットの自動選出 (`executor: "auto"`。電波強度の下限 `snmp.executor.min_rssi_dbm` を含む) と、止まっていたロボットの復帰 (`Startup Query`) | `src/SNMPManagerConfigure.cpp` |
| マイグレーションの `mode: auto` (`from` / `exclude`) | `SNMPManager::migration`、`auto_plan` |
| 効率化 (ポートの参照のキャッシュ、移動元の RTC の終了の並行実行) | `SNMPManager::lookup_port`、`apply_plan` |
| 自動復旧 (切り替えの条件、段階的な復旧) | `src/SNMPManagerRecovery.cpp` |
| コンポーネントオブザーバー (オブザーバ拡張マイグレーション。RTC の ERROR を通知で受け取り、待たずに復旧する) | `src/ComponentObserverServant.cpp`, `idl/ComponentObserver_*.idl`, `SNMPManager::attach_observers` |
| バッテリー残量 (`snmp.battery.*`、条件の指標 `battery_pct`) | `SNMPManager::set_robot_data` |
| 追加の監視の値 (`snmp.metrics`。条件の metric、自動復旧の `<名前>_max/_min`、`metric_constraints`、`executor_constraints`、SQLite `custom_metrics`) | `src/CustomMetrics.cpp`, `SNMPManager::set_robot_data`, `metric_excluded` |
| マネージャによる実測値の計測と保存 (runs.json・placement.xml・SQLite `rtc_measurements`)、計測値を自動配置の見込み値に使う | `src/MeasurementAggregator.cpp`, `SNMPManager::collect_measurements`, `ProfileDB` |
| C++ の RTC の計測 (C++ の slave `rtcd2` に読み込む計測モジュール `snmpmgr_measure.so` / `.dll`) | `src/SlaveMeasureModule.cpp` |
| slave マネージャの終了の検知 (feature/processcallback 相当)。異常終了でイベント `slave_crashed` とトラップ `Slave Exited` | `src/SlaveProcessObserver.cpp`, `SNMPManager::on_slave_exit` |
| Windows (Visual Studio 2022 / OpenRTM-aist 2.1.0 vc16) | `src/OSPlatform.cpp`, `build-win.ps1`, 下の「Windows でビルドする」 |

ほかに、RTCFailover（設定ファイル方式の RTC フェイルオーバー。`RTCFailover_configurable/RTCFailover.py`）の C++ 版 `rtcd_failover` も、同じプロジェクトでビルドします（`src/RTCFailover.{hpp,cpp}`、`src/RTCFailoverMain.cpp`。`RtcOps` と `Observer` を共有する）。使い方は `RTCFailover_configurable/README_RTCFailover.md` にあります。

## ビルド

WSL2 の Ubuntu (docker が使えること) で実行します。ビルドは Docker イメージ `rtm-snmp-cpp` の中で行い、
単体テストも流します。

```bash
bash snmpmanager-cpp/build.sh
```

結果は `snmpmanager-cpp/build/` に出ます。

- `snmpmanager`: マスターマネージャ本体。`snmpmanager -f rtc.conf` で起動する
- `snmpmgr_measure.so`: C++ の slave (`rtcd2`) に読み込む計測モジュール
- `plugins/lib*.so`: サンプルプラグイン
- `snmpmgr_tests`: 単体テスト (99 件)。配置・プロファイル・保存・プラグイン・計測・SNMP の符号化・slave の終了の検知を確認する

SNMP は既定で Net-SNMP を使います。`SNMPMGR_SNMP=builtin bash snmpmanager-cpp/build.sh` とすると、Windows と同じ組み込みの実装でビルドします (Net-SNMP をリンクしない)。

イメージ `rtm-snmp-cpp` は `docker/Dockerfile` から作ります。`rtm-bench` に次を追加したものです。

- `openrtm2-dev`
- omniORB の開発パッケージ
- `libsnmp-dev`
- `nlohmann-json3-dev`
- `libsqlite3-dev`
- `libtinyxml2-dev`
- `libglpk-dev` (ILP の配置プラグイン `ilp_placement` 用)

実行中のコンテナが `build/` をマウントしていても、`build.sh` は一時ファイルにコピーしてから rename するので置き換えられます。
新しいバイナリを使うには、コンテナを起動し直してください。

### OpenRTM-aist の ext としてビルドする

`../openrtm-ext/OpenRTM-aist/` の生成済みのファイルを OpenRTM-aist のソースにコピーして `src/ext/snmpmanager/` に配置し、OpenRTM-aist を `-DSNMPMANAGER_ENABLE=ON` でビルドすると、OpenRTM-aist と一緒にビルド・インストールされます (`bin/snmpmanager`、`lib/openrtm-2.1/snmpmanager/`)。この CMakeLists.txt は、OpenRTM-aist の中で使われたことを見分けて、OpenRTM-aist の中でビルドした `RTC2` を使います。詳しくは [`../openrtm-ext/README.md`](../openrtm-ext/README.md)。

## docker-test で動かす

`SNMPMGR_IMPL=cpp` を付けると、`rtmtest.sh` の動作が次のように変わります。

- compose ファイルとして `compose.yaml` と `compose.cpp.yaml` を併せて使う
- イメージ `rtm-snmp-cpp` で起動する。ビルドはしない (`--no-build`)
- `entrypoint.sh` は `SNMPManager.py` ではなく `/opt/snmpmgr-cpp/snmpmanager` を起動する

```bash
cd docker-test
SNMPMGR_IMPL=cpp ./rtmtest.sh up                                # 手動配置
SNMPMGR_IMPL=cpp ./rtmtest.sh migrate                           # robot1 の RTC を移す
SNMPMGR_IMPL=cpp ./rtmtest.sh up-auto                           # 自動配置
SNMPMGR_IMPL=cpp ./rtmtest.sh up-fail                           # ロードできない RTC → 起動失敗
SNMPMGR_IMPL=cpp ./rtmtest.sh up-with migration-plugin.json     # プラグインのマイグレーション
SNMPMGR_IMPL=cpp ./rtmtest.sh up-with startup-auto-firstfit.json
./rtmtest.sh db ; ./rtmtest.sh db events                        # 保存した監視情報 (Python 版の CLI で読める)
```

C++ のマスターが Python の RTC を扱うために、`entrypoint.sh` は rtc.conf に次の設定を足します。

```
manager.supported_languages: C++, Python
manager.modules.Python.profile_cmd: rtcprof2_python   # get_loadable_modules() に Python の RTC を出す
snmp.create.language: Python                          # 言語のない create を Python の RTC として生成する
snmp.plugin.path: /opt/snmpmgr-cpp/plugins
```

## Python 版との違い

- **create の言語。** C++ のマネージャは、言語の書かれていない create を C++ として扱います。そのため、構成ファイルの `"create": "SeqOut?instance_name=SeqOut0"` は `RTC:::SeqOut:Python:?instance_name=SeqOut0` に書き換えてから `create_component()` に渡します。
  - 言語は `snmp.create.language` で決めます。
  - 自動配置では、`get_loadable_modules()` が返したそのモジュールの言語を使います。
- **プラグイン。** プラグインは Python のモジュールではなく、共有ライブラリ (`.so`) です。
  - 構成ファイルの `"module": "alive_count"` から、`snmp.plugin.path` の下の `libalive_count.so` を探します。`alive_count.so` という名前でも見つかります。
  - 構成ファイルの書き方は Python 版と同じです。
- **`get_loadable_modules()` の負荷。** C++ のマスターは、Python の RTC のプロファイルを取るために `rtcprof2_python` を別プロセスで起動します。
  - 1回の呼び出しで、そのコンテナの CPU を一時的に 60〜100% 使います。
  - 自動配置では `get_loadable_modules()` の結果をリトライ間で使い回します。
  - 確認が終わった後の CPU 使用率だけで空きを見積もります。それより前のサンプルと、snmpd の extend のキャッシュに残った古い値は捨てます。
- **マイグレーション・自動復旧の時間。** `mode: auto` のマイグレーションと自動復旧の `other` は、移動先のロボットの `get_loadable_modules()` を使います (結果はロボットごとに使い回す)。C++ のマスターではこれに数秒〜30 秒かかります。
  - そこで、起動時の構成が終わったら、すべてのロボットが各マスターの `get_loadable_modules()` を裏で先に取っておきます (`prefetched loadable modules of ...`)。実行ロボットでないロボットも取るので、実行ロボットが止まって引き継いだときも待ちません。
  - 先に取っている間は、相手のコンテナの CPU が一時的に高くなります (CPU の制限が 45% の robot1 で 97%)。そのため、その間は自動復旧の判断を止め、終わったら CPU のサンプルを捨てて取り直します。これをしないと、`recovery.json` の `cpu_max_pct: 90` で、起動直後に RTC が移ってしまいました。
  - 対象は、構成ファイルに `recovery` があるか、`migration` が `mode: auto` のときです。
  - `observer.json` では、別のロボットへの切り替えが 9.6 秒から 0.8 秒になりました。
  - **残る遅れ (Python 版と同じ)。** docker-test の `failover robot1` は、マイグレーションの開始から完了まで約 40 秒かかります (Python 版も 46 秒)。これは `get_loadable_modules()` ではなく、OpenRTM の動作によるものです。
    - 生き残ったロボットの RTC は、止まった robot1 の RTC とつながっていたコネクタを持っています。そのポートのプロファイル (`get_component_profile()`、`get_connector_profiles()`) を取るたびに、OpenRTM が相手のポートの生存を確かめます (相手 1 つにつき接続のタイムアウト 3 秒)。
    - SeqIn0 は 10 本のコネクタを持つので、RTC の一覧を取るときに約 30 秒かかります。
    - 短くするには、`corba.args` の `-ORBclientConnectTimeOutPeriod` (docker-test は 3000ms) を小さくします。
- **RTC の一覧を取れなかったとき。** 一時的に取れなかった (CORBA のタイムアウトなど) ときは、直前の一覧を残し、ログに 1 回だけ出します。実行ロボットの選出で RTC の数が 0 に見えて、2 台が同時にマイグレーションするのを防ぐためです (一覧を一度も取れていないロボットは、選出のログで `robot3:?` と出ます)。
- **RTC のプロファイルの properties。** `NVUtil::copyToProperties` (coil::Properties の木を作る) は使わず、NVList を「名前 -> 文字列」として直接読みます。SimpleService のサンプルの RTC のプロファイルでは、`coil::Properties::createNode` が終わらない再帰になってスタックがあふれ、マスターが落ちました (docker-test の `startup-auto-firstfit.json` で、RTC の一覧を取ったときに確認。gdb で 6 万段の再帰)。
- **Windows。** ビルドして動かせます (下の「Windows でビルドする」)。Linux との違いは、SNMP が組み込みの実装であることと、slave の終了の検知の方法 (pid の観測) です。
- **常駐時の軽さ。** 何も動かしていないコンテナで比べると、C++ 版のほうがかなり軽い結果でした。

  | | C++ 版 | Python 版 |
  |---|---|---|
  | コンテナの CPU | 約 0.8% | 3〜5% |
  | コンテナのメモリ | 約 15MB | 約 72MB |

  - このため、`up-auto` では robot1 の空きが増えます。その結果、SeqOut0 / SeqIn0 / MyServiceProvider0 / MyServiceConsumer0 の4つが robot1 に入ります。Python 版では SliderComp0 / TkMotorComp0 の2つでした。
  - どちらも「主ロボットに置く RTC の数を最大にし、ロボットをまたぐ接続はなし」という同じ規則の結果です。

## マネージャによる実測値の計測と保存

Python 版（`docker-test/README.md` の「マネージャによる実測値の計測と保存」）と、同じ設定（`snmp.measure.*`）と同じ保存の形式で動きます。

- **計測**：slave の計測モジュールは、Python 版と同じ `snmpmgr_measure.py` です。docker-test の slave は Python（`rtcd_tk.py`）だからです。
  - C++ のマスターは `.py` を読めないので、rtc.conf の `manager.modules.preload` には書きません。代わりに、slave の起動コマンドで渡します（`manager.modules.Python.manager_cmd: python3 /opt/rtm/rtcd_tk.py -o manager.modules.preload:/opt/snmpmgr/snmpmgr_measure.py`）。
- **保存**：`SNMPManager::collect_measurements` が、`snmp.measure.save_interval` 秒ごとに `snmp.measure.runtime_dir` の計測を集めます。保存先は次の3つで、Python 版と同じファイル・同じテーブルです。
  - `snmp.measure.dir/<ロボット>/runs.json`
  - `<言語>/<型名>/placement.xml`（`plc:origin="runtime"`）
  - SQLite の `rtc_measurements`
  - まとめ方は `src/MeasurementAggregator.cpp` です（Python 版の `update_runs` / `aggregate` / `render_placement` と同じ）。
- **見込み値**：`ProfileDB` が、計測した placement.xml があればその値を使います。規則は Python 版の `placement.ProfileDB` と同じです（CPU・メモリは計測値の最大、ポートは送受信があったものだけ置き換える、`requires` は静的な値）。自動配置のログに `measured estimates: ...` が出ます。
- あわせて、`ProfileDB` がカンマ区切りの `snmp.placement.profile_dir` を読めるようにしました。docker-test の設定は `placement,placement-extra` の2つですが、これまでの C++ 版は1つしか読めませんでした。

- **C++ の RTC の計測**：計測モジュールの C++ 版 `snmpmgr_measure.so` (Windows は `snmpmgr_measure.dll`) を、C++ の slave (`rtcd2`) に preload します。Python 版 `snmpmgr_measure.py` と同じ値 (CPU・USS・ポートの送受信) を、同じ形式の `<マネージャ名>.json` に書きます。マスター (Python 版・C++ 版のどちらでも) が集めて、`C__/<型名>/placement.xml` と SQLite に保存します。
  ```
  manager.modules.C++.manager_cmd: rtcd2 -o manager.modules.preload:/opt/snmpmgr-cpp/snmpmgr_measure.so
  ```
  - 実行コンテキストのスレッドは、`PRE_ON_EXECUTE` のリスナーで、そのスレッドの ID を記録して求めます。
  - ポートは、POA (`reference_to_servant`) でサーバントを取り、`InPortBase` は `ON_RECEIVED`、`OutPortBase` は `ON_SEND` のリスナーを付けます。
  - USS は、Linux が `/proc/self/smaps_rollup` の Private、Windows が `PrivateUsage` です。
  - docker-test (`SNMPMGR_IMPL=cpp`) は、C++ の slave の起動コマンドにこの preload を入れます。

## slave マネージャの終了の検知

OpenRTM-aist-Python の feature/processcallback（`Process` と `ProcessCallback`）に当たる機能です。

- OpenRTM-aist C++ 2.1 のマスターは、slave を `coil::launch_shell()` で起動します。`launch_shell` は、fork する前に `signal(SIGCHLD, SIG_IGN)` を設定します（libRTC2 を逆アセンブルして確かめた）。そのため、slave の終了コードはカーネルが捨ててしまい、マスターは受け取れません。
- そこで snmpmanager（実行ファイル）が `coil::launch_shell` を定義して置き換えます（`src/SlaveProcessObserver.cpp`）。libRTC2 はこの関数を PLT 経由で呼ぶので、実行ファイルの定義が使われます（CMake の `ENABLE_EXPORTS` でシンボルを公開している）。
  - `snmp.process_observer.enabled: YES` のとき、slave を自分の子として起動します。`sh -c "exec <コマンド>"` なので、pid は slave 自身です。SIGCHLD は既定に戻します。
  - 終わるのを待って、終了コードを渡します（シグナルなら負の値）。
  - 無効のときは、`dlsym(RTLD_NEXT)` で元の `launch_shell` を呼びます。
- SNMPManager は、自分の slave の RTC の一覧（`snmp.inventory.interval` 秒ごと）から、終わった slave にあった RTC を求めます。
  - **終了コード 0**：イベント `slave_exited`。
  - **0 以外**：イベント `slave_crashed`、`snmpmgr.txt` の `SLAVE_CRASHED`、ほかのロボットへのトラップ `Slave Exited`（`<ロボット>|<RTC,...>`）。受け取ったロボットはログに出します。
- 自動復旧 (`recovery`) が有効なら、異常終了した slave の RTC が「消えた」ときに、`duration_seconds` を待たずに復旧を始めます (Python 版と同じ)。
- **Windows**：`launch_shell` は OpenRTM の DLL の中で呼ばれるので、置き換えられません。代わりに、RTC の一覧で見つけた slave の pid (slave の設定の `manager.pid`) のプロセスを開いて (`OpenProcess`)、終わるのを待ちます (`process::watch`)。
  - 終了コードは `GetExitCodeProcess` の値です。例外で落ちたなら `0xC0000005` などを int にした負の値、`taskkill /F` なら 1、`Stop-Process` なら -1 です。
  - 観測を始めるのは RTC の一覧を取ったとき (`snmp.inventory.interval`) です。RTC のない slave は観測しません。

## Windows でビルドする

Visual Studio 2022 (x64) と OpenRTM-aist 2.1.0 (C++、vc16) で、PowerShell からビルドします。単体テストも流します。

```powershell
cd snmpmanager-cpp
powershell -ExecutionPolicy Bypass -File build-win.ps1
```

結果は `build-win\bin\` に出ます (`snmpmanager.exe`、`snmpmgr_measure.dll`、`snmpmgr_tests.exe`、`plugins\lib*.dll`)。

依存するものと、その取り方は次のとおりです。ビルドのために新しくダウンロードするものはありません。

| 依存 | Windows での取り方 |
|---|---|
| OpenRTM-aist / omniORB | `C:\Program Files\OpenRTM-aist\2.1.0\cmake\OpenRTMConfig.cmake` (`-DOpenRTM_DIR=` で変えられる) |
| SNMP | 組み込みの実装 (`SNMPMGR_SNMP=builtin`)。SNMPv2c の GET・GETNEXT・トラップの送信と受信 (v1 のトラップも読む) |
| SQLite | Windows SDK の `winsqlite3` (OS の `winsqlite3.dll`) |
| tinyxml2 | 既定は Chocolatey の `C:\ProgramData\chocolatey\lib\tinyxml2` (`-DTINYXML2_DIR=` で変えられる)。`tinyxml2.dll` を `bin\` にコピーする |
| nlohmann_json | `third_party/nlohmann` (Debian の nlohmann-json3-dev 3.11.3 のヘッダ。MIT ライセンス、`COPYRIGHT`) |
| GLPK (ILP の配置プラグインだけ。なくてもほかはビルドできる) | winglpk 4.65 (SourceForge の `winglpk-4.65.zip`) を `%LOCALAPPDATA%\glpk` に展開したもの (既定。`-DGLPK_DIR=` で変えられる)。`glpk_4_65.dll` を `bin\plugins\` にコピーする |

Linux 版との違いと、Windows のための対処は次のとおりです。

- **日本語を含むパス**：実行ファイルのマニフェスト (`win/utf8.manifest`) で、プロセスの ANSI コードページを UTF-8 にしています (Windows 10 1903 以降)。SNMPManager は `std::string` をすべて UTF-8 として扱うので、`std::filesystem` と `ifstream` で日本語を含むパスを開けます。コンソールの出力も UTF-8 にします。
- **omniidl**：Windows の omniidl は、プリプロセッサのパスを引用符なしで呼ぶので、空白を含む `C:\Program Files` で失敗します。また、日本語を含む絶対パスの IDL を読めません。CMake で 8.3 形式の短いパスにし、IDL は生成先からの相対パスで渡します。
- **DLL のデータの参照**：omniidl が生成したスタブは、OpenRTM の DLL にある `SDOPackage` のデータ (リポジトリ ID など) を参照します。`USE_stub_in_nt_dll` を付けずにコンパイルすると dllimport にならず、プロキシの `is_a()` で落ちます。そこで、生成したスタブを包むファイル (`build-win/idl/*_stub.cpp`) で、先に `USE_stub_in_nt_dll` を付けて `SDOPackage.hh` を読みます。
- **プラグイン**：`.dll` を `LoadLibrary` で読みます (`lib<名前>.dll` / `<名前>.dll`)。`SNMPMGR_PLUGIN_EXPORT` は `__declspec(dllexport)` になります。プラグイン API は仮想関数とヘッダだけなので、DLL からも同じように使えます。
- **自分の IP アドレス**：`GetAdaptersAddresses` で取ります。
- **プラグインが依存する DLL**：プラグインは `LoadLibraryEx(LOAD_WITH_ALTERED_SEARCH_PATH)` で読むので、プラグインが依存する DLL (`glpk_4_65.dll` など) は、実行ファイルの隣ではなくプラグインの隣 (`bin\plugins\`) に置きます。
- **GLPK 4.65**：MIP の前処理 (presolve) ですべての列が消えると、`glp_add_cols(0)` でプロセスを止めてしまいます。そのため `ilp_placement` は前処理を使わず、LP の緩和問題を単体法で解いてから分枝限定法を行います (Linux の GLPK 5.0 でも同じ方法)。

動かすときは、OpenRTM と omniORB の DLL を PATH に入れます。

```powershell
$rtm = "C:\Program Files\OpenRTM-aist\2.1.0"
$env:PATH = "$rtm\bin\vc16;$rtm\omniORB\4.3.4_vc16\bin\x86_win32;$env:PATH"
.\build-win\bin\snmpmanager.exe -f rtc.conf
```

C++ の RTC の slave に計測モジュールとオブザーバーを入れる例です。`manager_cmd` は空白で区切られるので、DLL は空白のないディレクトリ (`manager.modules.load_path`) に置きます。

```
manager.modules.load_path: C:/rtc/comps
manager.modules.C++.manager_cmd: rtcd2 -o manager.modules.preload:snmpmgr_measure.dll,ComponentObserverConsumer.dll -o sdo_service.consumer_types:ALL
snmp.create.language: C++
```

**Windows で確認した結果** (Windows 11、SNMP のエージェントなし。自分のロボットだけ)

| 確認 | 結果 |
|---|---|
| 単体テスト (`build-win.ps1`) | passed 126, failed 0 (GLPK なしなら ILP のプラグインのテストを除く。slave の終了の検知は、`CreateProcess` した子を `watch()` で観測する版。ILP の配置プラグインは GLPK 4.65 で組み込みの optimal と評価が一致) |
| 起動時の構成 | C++ の SeqOut0 / SeqIn0 を C++ の slave (`rtcd2`) に生成し、`Long` をつないで ACTIVE にした (1.1 秒) |
| コンポーネントオブザーバー | 2 つの RTC に関連付けた。Windows の `ComponentObserverConsumer.dll` は `IDL:omg.org/RTC/ComponentObserver:1.0` だけを受け付けたので、2 つ目のインターフェースで関連付けた |
| 計測モジュール (`snmpmgr_measure.dll`) | RTC ごとの CPU・メモリ・`Long` の送受信 (12B × 4.6Hz) を計測し、マスターが `C__/SeqIn/placement.xml` などと SQLite に保存した |
| slave の異常終了 (`Stop-Process -Force`) | 12ms で `slave_crashed ... code=-1 rtcs=SeqOut0,SeqIn0` |
| プラグイン (`libevent_log.dll`) | イベントを JSONL に書いた |

まだ確かめていないこと：
- Windows で、SNMP のエージェントがあるほかのロボット (Linux の snmpd など) と組み合わせて動かすこと。組み込みの SNMP の実装は、Linux の docker-test で Net-SNMP の代わりに使い、発見・値の取得・トラップ (`Startup Completed`、`RTC Error`) が同じように動くことを確かめました。
- Windows で RTC の状態の通知 (ERROR) を受けること (関連付けまでは確認)。

## プラグインの書き方

仕様・ビルドのしかた・テストのしかたは [docs/06_reference.md の「C++ のプラグイン」](../docs/06_reference.md#23-c-のプラグイン) にあります。

```cpp
#include <snmpmgr/Plugin.hpp>

SNMPMGR_PLUGIN_EXPORT int snmpmgr_plugin_api_version() { return SNMPMGR_PLUGIN_API_VERSION; }

SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char* config_json)
{
  ctx->register_condition("alive_count", [](snmpmgr::PluginContext& c, const snmpmgr::json& args, double now) {
    return static_cast<int>(c.alive_robots().size()) >= args.at("value").get<int>();
  });
}
```

`PluginContext` (`include/snmpmgr/Plugin.hpp`) には4つの登録口があります。

| 登録口 | 用途 |
|---|---|
| `register_condition` | マイグレーションの条件 |
| `register_action` | マイグレーションの処理 |
| `register_placement` | 起動時の配置 |
| `subscribe` | イベント |

ほかに、次の機能を使えます。

- `robots()` と `alive_robots()`: SNMP の値、cgroup、死活の状態、追加の監視の値 (`metrics`) を返す
- `manager_object(robot)`: `RTM::Manager_ptr` を返す
- `apply_plan(plan, label)`: 生成・接続・アクティベートを行う
- `event()`: イベントを出す
- `conf()`: rtc.conf の値を返す

API のバージョンは `SNMPMGR_PLUGIN_API_VERSION` (=3) です。違うバージョンの `.so` は読み込みません。版 3 で `RobotInfo` に `metrics` (追加の監視の値) を足したので、版 2 までのプラグインはビルドし直します。

サンプル (`plugins/`) は Python 版の `plugins/*.py` と同じ動作をします。ソースのファイル名は先頭が大文字 (`plugins/AliveCountPlugin.cpp` など) ですが、プラグインの名前 (構成ファイルの `"plugin"`、ライブラリの `libalive_count.so` / `libalive_count.dll`) は下の表のとおりです。

| プラグイン | 種類 | 動作 |
|---|---|---|
| `least_loaded_migration` | 処理 | 移す RTC を、その時点で負荷が一番低いロボットへ順に置く |
| `alive_count` | 条件 | 生きているロボットの数 |
| `event_log` | イベント | イベントを JSON Lines でファイルに書く |
| `first_fit_placement` | 配置 | 主ロボットから順に、入るだけ詰める |
| `ilp_placement` | 配置 | 整数線形計画 (ILP) のソルバー GLPK で配置を決める (Python 版 `plugins/ilp_placement.py` と同じモデル・同じ `placement_args`。`solver` は `glpk` だけ)。GLPK があるときだけビルドする |

## 確認した結果 (docker-test, WSL2)

| シナリオ | 結果 |
|---|---|
| `up` (手動配置) | 4 つの RTC を robot1/2/3 に生成し、11 本の接続を張って ACTIVE |
| `migrate` | Wi-Fi -85 dBm で発動。SeqOut0 を robot3 に、SliderComp0 を robot2 に移し、接続を張り直した |
| `up-auto` | robot1 に SeqIO と SimpleService (CPU 34.0/34.2%)、robot2 に Slider/Motor。ロボットをまたぐ接続はなし |
| `up-fail` | ConsoleIn がどのマスターにもない → `SYSTEM STARTUP FAILED`。`startup_status.json` に理由を書いた |
| 死活監視 | robot3 を `docker pause` → 6.2 秒で down、再開で up。どちらも `events` に保存した |
| パケットロス | robot2 の UDP を 30% 捨てた → 20 回の窓で loss 40%。レイテンシは 0.5ms 前後 |
| `migration-plugin.json` | `alive_count` と Wi-Fi の AND で発動した。`least_loaded_migration` が SeqOut0 を robot2 に、SliderComp0 を robot3 に移した。`event_log` が JSONL に記録した |
| `startup-auto-firstfit.json` | `first_fit` が robot1 に 3 つ、robot2 に 3 つを置いた。SliderComp0-TkMotorComp0 はロボットをまたぐ (Python 版と同じ) |
| 実測値の計測と保存 (`up`) | 4 つの型の placement.xml を保存した (SeqOut 2.0%/2.3MB、SeqIn 6.0%/2.9MB、SliderComp 10.0%/1.3MB、TkMotorComp 9.0%/4.7MB)。SQLite の `rtc_measurements` にも残った |
| 計測値を使った自動配置 (`up-auto`) | `measured estimates: ...` の 4 つは計測値で見積もった。robot1 に SeqIO と Slider/Motor の 4 つ、robot2 に SimpleService |
| slave の異常終了 (`crash robot2 SeqIn0`) | kill から 0.15 秒で robot2 が `slave_crashed ... code=-9 rtcs=SeqIn0`。robot1・robot3 にトラップ `Slave Exited` が届いた。ゾンビは残らない |
| slave の正常な終了 (`migrate`) | RTC がなくなった robot1 の slave が終わり、`slave_exited ... code=0`。ゾンビは残らない |
| `up` (ポートの参照のキャッシュ) | 11 本の接続で、ポートの参照の取得は CORBA 呼び出し 4 回 |
| `spof-standby.json` の起動 | 3 台とも robot1 を実行ロボットに選んだ (`executor_elected`)。待機モジュール SeqOut_sb を robot3 に INACTIVE で起動した |
| `failover robot1` | robot2・robot3 とも robot3 を選び直した。`mode: auto`・`logic: objective` で、SeqOut0 は robot3 の待機モジュール SeqOut_sb を使い、SliderComp0 は robot3 に生成した。止まった robot1 の RTC の削除は飛ばした |
| robot1 の復帰 (`docker start robot1`) | `Startup Query` に robot3 が「完了」と答え、`startup_joined`。RTC は作らなかった |
| `recovery.json`: `fault FaultyComp0 reset` | ERROR → `reset` で直った (段階 4.1 秒) |
| `recovery.json`: `fault FaultyComp0 restart` | `reset` では直らない → `restart` で直った (段階 9.4 秒) |
| `recovery.json`: `crash robot2 FaultyComp0` | robot2 の `slave_crashed` から 0.03 秒で robot1 が復旧を始め、SeqIn0 と FaultyComp0 を `restart` で作り直した (8.7 秒) |
| `recovery.json`: `fault node` | `reset`・`restart` では直らない → `other`: robot3 の待機モジュール FaultyComp_sb に切り替えた (18.9 秒。うち約 10 秒は最初の `get_loadable_modules()`) |
| `recovery.json`: robot3 で `battery 10` | すぐ `other`: TkMotorComp0 と FaultyComp0 を robot1 に移した (4.3 秒) |
| `observer.json` (`OBSERVER=YES`): robot2 で `fault FaultyComp0 restart` | 0.27 秒でコンポーネントオブザーバーの通知 (`rtc_error_notified`) とトラップ `RTC Error`。`duration_seconds` (30 秒) を待たず、robot3 の待機 RTC FaultyComp_sb に切り替えた (故障から 0.78 秒で完了) |
| `observer.json`: 続けて robot3 の FaultyComp_sb を ERROR に | 待機 RTC がないので、robot1 に FaultyComp0 を新規に起動した (故障から 1.6 秒で完了) |
| `observer.json`: C++ の RTC (`rtcd2` の slave に ConsoleOut、`ComponentObserverConsumer.so` を preload) | 関連付けて、状態の通知 (`RTC_STATUS ACTIVE:0`) を受けた |
| C++ の RTC の計測 (robot1 で `rtcd2` に SeqOut・SeqIn と `snmpmgr_measure.so`) | SeqOut0 は CPU 1.0%・メモリ 2.0MB・送信 10 ポート (例: Double 16B × 4.99Hz)、SeqIn0 は同じ受信を計測した。マスターが `C__/SeqOut/placement.xml`・`C__/SeqIn/placement.xml` と SQLite に保存した |
| 組み込みの SNMP の実装 (`SNMPMGR_SNMP=builtin`) で `up`・`migrate`・`spof-standby.json`・`observer.json` | Net-SNMP 版と同じ: 3 台を発見、Wi-Fi -85 dBm でマイグレーション、実行ロボットの選出と `Startup Completed` のトラップ、`RTC Error` のトラップで 0.98 秒で復旧 |
| `startup-auto-ilp.json` (`ilp_placement`、GLPK 5.0) | 変数 22・制約 36 のモデルを解き (`ILP (glpk, lexicographic): optimal`)、robot1 に SeqIO と MyService の 4 つ、robot3 に Slider・Motor を置いた。ロボットをまたぐ接続はなし |
| `metrics-migration.json` / `metrics-recovery.json` (追加の監視の値 `temperature`) | Python 版と同じ: robot1 75℃・robot3 80℃ で、実行ロボットは robot2、robot3 を配置から外して robot1 の RTC を robot2 に移した。robot2 80℃ で、robot2 の RTC を robot1 と robot3 の待機モジュールに移した (2.4 秒) |
| 単体テスト | passed 128, failed 0 (Windows は 126。追加の監視の値の設定・制約・保存を含む。配置の結果は Python 版の `placement.py` と一致。first_fit・objective のロジック・ロボット間の制約・待機モジュールと既存の RTC・目的関数のプラグイン・ILP のプラグイン (組み込みの optimal と 5 通りで評価が一致)・実測値・計測の按分・SNMP の符号化・`launch_shell` の置き換えを含む) |

## ファイル

```
snmpmanager-cpp/
  CMakeLists.txt, build.sh (Linux), build-win.ps1 (Windows), docker/Dockerfile
  include/snmpmgr/Plugin.hpp              プラグイン API
  src/SNMPManagerMain.cpp                 マネージャの起動
  src/SNMPManager.{hpp,cpp}               本体 (発見・監視・構成・マイグレーション・自動配置)
  src/SNMPClient.{hpp,cpp}                SNMP GET/GETNEXT/トラップ送受信 (Net-SNMP)
  src/SNMPBuiltinClient.cpp               同じ (組み込みの実装。UDP で送受信)
  src/SNMPBerCodec.{hpp,cpp}              SNMPv2c のメッセージの符号化・復号 (BER)
  src/OSPlatform.{hpp,cpp}                OS に依存する部分 (IP アドレス・共有ライブラリ・実行ファイルの場所)
  src/SlaveMeasureModule.cpp              C++ の slave に読み込む計測モジュール (snmpmgr_measure.so / .dll)
  src/RTCOperations.{hpp,cpp}             マネージャ・RTC の CORBA 操作
  src/RTCPlacement.{hpp,cpp}              プロファイル (placement.xml) と配置ソルバ
  src/MonitoringStore.{hpp,cpp}           SQLite への保存
  src/PluginManager.{hpp,cpp}             プラグインの読み込みと呼び出し
  src/MeasurementAggregator.{hpp,cpp}     実測値のまとめ方と placement.xml (Python 版 snmpmgr_measure.py と同じ)
  src/SlaveProcessObserver.{hpp,cpp}      slave の起動と終了の検知 (Linux: coil::launch_shell の置き換え、Windows: pid の観測)
  src/SNMPManagerConfigure.cpp            実行ロボットの選出・起動時の構成・RTC の一覧・待機モジュール・予約・自動配置・計画の適用・マイグレーション
  src/SNMPManagerRecovery.cpp             自動復旧
  src/ComponentObserverServant.{hpp,cpp}  コンポーネントオブザーバーのサーバーオブジェクト
  src/RTCFailover.{hpp,cpp}               RTCFailover の C++ 版 (設定ファイル方式の RTC フェイルオーバー)
  src/RTCFailoverMain.cpp                 rtcd_failover の起動
  idl/ComponentObserver_*.idl             サーバーオブジェクトの IDL (OpenRTM 版と omg.org/RTC 版。ビルドで omniidl が生成)
  src/SNMPManagerUtil.hpp
  plugins/*.cpp                           サンプルプラグイン
  tests/TestMain.cpp                      単体テスト
  win/WifiRssi.cpp                        電波強度 (RSSI) を出力するツール wifi_rssi.exe
  win/utf8.manifest                       Windows の実行ファイルのマニフェスト (ANSI コードページを UTF-8 に)
  third_party/nlohmann                    nlohmann_json のヘッダ (Windows 用)
```
