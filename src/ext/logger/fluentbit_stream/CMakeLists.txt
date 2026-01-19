cmake_minimum_required (VERSION 3.16)

# mpack の C++ オーバーロードを強制無効化
add_compile_definitions(MPACK_NO_CPP=1)
# 念のため、古い名称や別名も定義しておく
add_compile_definitions(MPACK_CPLUSPLUS=0)

# /permissive- を無効化（または /permissive を有効化）して、標準への厳格な準拠を緩和する
if(MSVC)
	add_compile_options(/permissive)
endif()

set(FLUENTBIT_ROOT ${FLUENTBIT_ROOT} CACHE PATH "set FLUENTBIT_ROOT")
if(NOT FLUENTBIT_ROOT)
	message(STATUS "FLUENTBIT_ROOT is empty.")
	message(STATUS "fluent-bit is checked out from original repository.")

	include(BuildFluentBit.cmake)

	message(STATUS "FLUENTBIT_ROOT = " ${FLUENTBIT_ROOT})
	message(STATUS "FLB_BINARY_DIR = " ${FLB_BINARY_DIR})
	message(STATUS "FLB_LIBRARY_DIR = " ${FLB_LIBRARY_DIR})
	message(STATUS "FLB_INCLUDR_DIR = " ${FLB_INCLUDR_DIR})
	message(STATUS "FLB_SOURCE_DIR = " ${FLB_SOURCE_DIR})
	message(STATUS "libfluent-bit.so: " ${FLB_LIBRARY_DIR}/libfluent-bit.so)

else()
	set(FLB_BINARY_DIR ${FLUENTBIT_ROOT}/bin)
	set(FLB_INCLUDR_DIR ${FLUENTBIT_ROOT}/include)
	set(FLB_LIBRARY_DIR ${FLUENTBIT_ROOT}/lib/fluent-bit)
endif()

project (FluentBit
	VERSION ${RTM_VERSION}
	LANGUAGES C CXX)

link_directories(${ORB_LINK_DIR} ${FLB_LIBRARY_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
	add_definitions(-DFLB_SYSTEM_WINDOWS)
endif()
add_definitions(-Dtypeof=decltype)


set(target FluentBit)
set(srcs FluentBit.cpp FluentBit.h)

# ----------------------------------------------------------------------
# C言語専用のラッパーライブラリを独立させる (C++フラグの混入を防ぐ)
# ----------------------------------------------------------------------
add_library(flb_wrapper STATIC flb_api_wrapper.c)

# インクルードディレクトリの追加（Monkeyヘッダーの検索パスを通す）
target_include_directories(flb_wrapper PRIVATE
    /usr/local/include
    /usr/local/include/monkey
)

# 言語規格の設定 (CMake標準機能なので OS を問わず安全)
set_target_properties(flb_wrapper PROPERTIES
    C_STANDARD 11
    C_EXTENSIONS ON
)

if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	target_link_libraries(${target} ${libs} fluent-bit)

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				COMPONENT ext)
else()
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})

	add_library(${target} SHARED ${srcs})
	if(MSVC)
		# 発生している警告をすべて抑制リストに追加
		# /wd4127: 条件式が定数 (mpack.h)
		# /wd4819: 文字コード (ファイル保存形式)
		# /wd4100: 未使用の引数 (ラッパー関数)
		# /wd4245: 符号付き/符号なしの混在 (型の変換)
		# /wd4189: ローカル変数の初期化後、参照なし
		# /wd4200: サイズ 0 の配列 (非標準の拡張機能)
		# /wd4201: 無名の構造体または共用体 (非標準の拡張機能)
		# /wd4098: void関数が値を返す
		target_compile_options(${target} PRIVATE /wd4127 /wd4819 /wd4100 /wd4245
				 /wd4189 /wd4200 /wd4201 /wd4098)

		# 念のため、このターゲットだけは警告があってもビルドを止めない設定を明示
		set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/WX-")
	else()
		# Linux / GCC の場合：gnu11 を明示してマクロを正常展開 ＆ 警告抑制 
		target_compile_options(flb_wrapper PRIVATE -std=gnu11)
		# GNU 拡張機能を明示的に有効化
		target_compile_definitions(flb_wrapper PRIVATE _GNU_SOURCE)
	endif()
	if(FLB_PROJECT)
		add_dependencies(${target} ${FLB_PROJECT})
	endif()
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	openrtm_set_link_props_shared(${target})

	target_include_directories(${target}
			SYSTEM PRIVATE
			${FLB_INCLUDR_DIR}
	)
	if(NOT FLUENTBIT_ADD_INCLUDE)
		target_include_directories(${target}
			SYSTEM PRIVATE
			${FLB_INCLUDR_DIR}
		)
	else()
		target_include_directories(${target}
			SYSTEM PRIVATE
			${FLB_INCLUDR_DIR}
			${FLB_INCLUDR_DIR}/monkey
		)
	endif()
	target_link_libraries(${target} PRIVATE 
			${libs}
			${RTM_LINKER_OPTION}
			flb_wrapper
			fluent-bit)
	set_target_properties(${target} PROPERTIES PREFIX "")

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				COMPONENT ext)
endif()

if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)

if(BUILD_RTM_LINUX_PKGS)
	set(CMAKE_INSTALL_PREFIX "/usr")
endif()
set(FLUENTPLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_RTM_EXT_DIR}/logger/FluentBit${CMAKE_SHARED_LIBRARY_SUFFIX}")
if(MSVC)
	string(REPLACE "/" "\\\\" FLUENTPLUGIN_PATH "${FLUENTPLUGIN_PATH}")
else()
	
endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/rtc.fluentbit_stream.conf.in ${PROJECT_BINARY_DIR}/rtc.fluentbit_stream.conf @ONLY)
install(
	FILES ${PROJECT_BINARY_DIR}/rtc.fluentbit_stream.conf
	DESTINATION ${INSTALL_RTM_ETC_DIR}/logger
	COMPONENT ext)
if(MSVC)
	install(PROGRAMS ${FLB_BINARY_DIR}/fluent-bit.dll DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT fluentbitlib)
endif()
