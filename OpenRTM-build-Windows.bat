@echo off
@rem 1. 本スクリプトは、GitHub Actionsとローカル環境それぞれでのビルドで使用する
@rem 2. GitHub Actionsの場合、openrtm-windows.ymlの中から呼び出される
@rem 3. ローカル環境での実行の場合は本スクリプトを直接実行し、msm生成時に利用する
@rem ============================================================================= 
if "%GITHUB_ACTIONS%"=="true" goto :ON_GITHUB_ACTIONS

:ON_LOCAL
@rem msm生成に向けたローカル環境でのビルドと判断
set GITHUB_ACTION=OFF
if not defined CMAKE_GENERATOR set CMAKE_GENERATOR="Visual Studio 16 2019" 

@rem Pythonバージョンを指定し、パスを通す
set PYTHON_SHORT_VERSION=312
set "path=C:\Python%PYTHON_SHORT_VERSION%;%PATH%"

@rem Boost
set BOOST_PATH=C:\Boost
set Boost_DIR=C:\Boost\lib\cmake\Boost-1.87.0
  
@rem OpenRTM-aistのインストール先指定
set INSTALL_PREFIX=C:\localRTM
if exist %INSTALL_PREFIX% rmdir /s/q %INSTALL_PREFIX%

@rem FluentBitのインストール先
set FLB_ROOT=C:\localFLB
goto :AFTER_CHECK

:ON_GITHUB_ACTIONS
@rem GitHub Actionsでの実行と判断
set GITHUB_ACTION=ON
if not defined CMAKE_GENERATOR set CMAKE_GENERATOR="Visual Studio 17 2022"
goto :AFTER_CHECK

:AFTER_CHECK
echo [INFO] Running in GitHub Actions: %GITHUB_ACTION%
echo [INFO] Using CMAKE_GENERATOR: %CMAKE_GENERATOR%

@rem ======================================================== 

@rem 下記はOpenRTM2.1.0版で変更不要
@rem OpenSSLはOpenRTM向けにビルド済みバイナリをダウンロードするため
@rem 下記バージョンの指定が必須
set VC_VERSION=vc16
set OMNI_VERSION=4.3.4
set SSL_VC_VERSION=vc14
set SSL_VERSION=3.0.1

@rem パス中の "\" を "/" に変換する
set current_dir=%~dp0
set RTM_ROOT=%current_dir:\=/%
set BOOST_PATH=%BOOST_PATH:\=/%
if %GITHUB_ACTION%=="ON" goto :ON_OMNIORB_DOWNLOAD
set INSTALL_PREFIX=%INSTALL_PREFIX:\=/%
set FLB_ROOT=%FLB_ROOT:\=/%

:ON_OMNIORB_DOWNLOAD
@rem omniORB download
set base_omni_url="https://openrtm.org/pub/omniORB/win32/omniORB-%OMNI_VERSION%/"
set OMNIORB_DIR=omniORB-%OMNI_VERSION%-x64-%VC_VERSION%-py%PYTHON_SHORT_VERSION%
set OMNIORB_ZIP=%OMNIORB_DIR%.zip
set OMNIORB_URL=%base_omni_url%/%OMNIORB_ZIP%
if not exist %OMNIORB_ZIP% (
  powershell wget -O %OMNIORB_ZIP% %OMNIORB_URL%
  if exist %OMNIORB_DIR% rmdir /s/q %OMNIORB_DIR%
  powershell Expand-Archive .\%OMNIORB_ZIP% -DestinationPath .\
)
set OMNIORB_ROOT=%RTM_ROOT%/%OMNIORB_DIR%

@rem OpenSSL download
set base_ssl_url="https://openrtm.org/pub/OpenSSL/%SSL_VERSION%"
set OPENSSL_ZIP=openssl-%SSL_VERSION%-win64-%SSL_VC_VERSION%.zip
set OPENSSL_URL=%base_ssl_url%/%OPENSSL_ZIP%
if not exist %OPENSSL_ZIP% (
  powershell wget -O %OPENSSL_ZIP% %OPENSSL_URL%
  if exist OpenSSL rmdir /s/q OpenSSL
  powershell Expand-Archive .\%OPENSSL_ZIP% -DestinationPath .\
)
set SSL_ROOT=%RTM_ROOT%OpenSSL/build
echo [INFO] SSL_ROOT: %SSL_ROOT%
echo [INFO] GITHUB_ACTION: %GITHUB_ACTION%

@rem set cmake parameter
if "%GITHUB_ACTION%"=="OFF" goto :ON_LOCAL_CMAKE
echo [INFO] Running cmake in GitHub Actions: %GITHUB_ACTION%
set CMAKE_OPT=-DRTM_VC_VER=%VC_VERSION% ^
 -DORB_ROOT=%OMNIORB_ROOT% ^
 -DCORBA=omniORB ^
 -DSSL_ENABLE=ON ^
 -DOPENSSL_ROOT=%SSL_ROOT% ^
 -DBOOST_ROOT=%BOOST_PATH% ^
 -DBoost_NO_BOOST_CMAKE=ON ^
 -DBoost_USE_STATIC_LIBS=OFF ^
 -DHTTP_ENABLE=ON ^
 -G %CMAKE_GENERATOR% ^
 -A x64 ..
goto :CMAKE_BUILD

:ON_LOCAL_CMAKE
echo [INFO] Running cmake in Local: %GITHUB_ACTION%
set CMAKE_OPT=-DRTM_VC_VER=%VC_VERSION% ^
 -DORB_ROOT=%OMNIORB_ROOT% ^
 -DCORBA=omniORB ^
 -DSSL_ENABLE=ON ^
 -DOPENSSL_ROOT=%SSL_ROOT% ^
 -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% ^
 -DWINDOWS_MSM_BUILD=ON ^
 -DBOOST_ROOT=%BOOST_PATH% ^
 -DBoost_USE_STATIC_LIBS=OFF ^
 -DHTTP_ENABLE=ON ^
 -DFLUENTBIT_ENABLE=ON ^
 -DFLUENTBIT_ROOT=%FLB_ROOT% ^
 -G %CMAKE_GENERATOR% ^
 -A x64 ..

:CMAKE_BUILD
call :CMAKE_Debug
call :CMAKE_Release
exit /b

:CMAKE_Release
if exist build-release rmdir /s/q build-release
mkdir build-release
cd build-release
cmake %CMAKE_OPT% 
cmake --build . --verbose --config Release
if "%GITHUB_ACTION%"=="OFF" cmake --install .
cd ..
exit /b

:CMAKE_Debug
if exist build-debug rmdir /s/q build-debug
mkdir build-debug
cd build-debug
echo [INFO] Cmake options: %CMAKE_OPT%
cmake %CMAKE_OPT%
cmake --build . --verbose --config Debug
if "%GITHUB_ACTION%"=="OFF" cmake --install . --config Debug
cd ..
exit /b

