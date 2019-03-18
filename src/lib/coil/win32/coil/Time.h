// -*- C++ -*-
/*!
 * @file Time_win32.h
 * @brief Time functions
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_TIME_H
#define COIL_TIME_H


#include <windows.h>
#ifdef WITH_ACE
#include <WinSock2.h>
#else
#include <winsock.h>
#endif
//#include <winsock2.h>
//#pragma comment(lib, "WS2_32.LIB")
#include <time.h>
#include <coil/config_coil.h>
#include <coil/TimeValue.h>

namespace coil
{
#define EPOCHFILETIME (116444736000000000i64)
  struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };

  /*!
   * @if jp
   * @brief 指定された秒間は処理を休止する
   *
   * 指定された秒間は処理を休止する。
   *
   * @param seconds 秒数
   *
   * @return 0: 成功
   *
   * @else
   * @brief Stop a processing at specified second time
   *
   * Stop a processing at specified second time.
   *
   * @param seconds Second time
   *
   * @return 0: successful
   *
   * @endif
   */
  inline unsigned int sleep(unsigned int seconds)
  {

    ::Sleep(seconds *1000);
    return 0;
  }

//static short m_time_DLLinit_count = 0;

  /*!
   * @if jp
   * @brief 指定された秒間は処理を休止する
   *
   * 指定された秒間は処理を休止する。
   *
   * @param interval TimeValueオブジェクト
   *
   * @return 0: 成功, != 0: 失敗
   *
   * @else
   * @brief Stop a processing at specified second time
   *
   * Stop a processing at specified second time.
   *
   * @param interval TimeValue object
   *
   * @return 0: successful, != 0: failed
   *
   * @endif
   */
  inline int sleep(TimeValue interval)
  {
    struct timeval tv;
    WSADATA wsa;
    SOCKET ssoc;
    fd_set mask;
    WORD ver;
    int iret;

    // The WSAStartup function initiates use of the Winsock DLL by a process.
    ver = MAKEWORD(2, 2);
    iret = ::WSAStartup(ver, &wsa);
    if ( iret != 0 )
      {
        return iret;
      }

    // The socket function creates a socket that is bound to
    // a specific transport service provider.
    // It is assumed AF_INET because there is no AF_UNIX for Windows.
    ssoc = ::socket(AF_INET,
                    SOCK_STREAM,
                    0);
    if (ssoc == INVALID_SOCKET)
      {
        iret = ::WSAGetLastError();
        ::WSACleanup();
        return iret;
      }


    // Initialize fd_set.
    FD_ZERO(&mask);
    // Register the reading socket.
    FD_SET(ssoc, &mask);

    tv.tv_sec = interval.sec();
    tv.tv_usec = interval.usec();
    iret = ::select(static_cast<int>(ssoc+1), &mask, NULL, NULL, &tv);
    if ( iret == SOCKET_ERROR )
      {
        iret = ::WSAGetLastError();
        // The closesocket function closes an existing socket.
        ::closesocket(ssoc);
        // The WSACleanup function terminates
        // use of the Winsock 2 DLL (Ws2_32.dll).
        ::WSACleanup();
        return iret;
      }

    // The closesocket function closes an existing socket.
    ::closesocket(ssoc);

    // The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
    ::WSACleanup();
    return iret;
  }

  /*!
   * @if jp
   * @brief 指定されたマイクロ秒間は処理を休止する
   *
   * 指定されたマイクロ秒間は処理を休止する。
   *
   * @param usec マイクロ秒数
   *
   * @return 0: 成功, != 0: 失敗
   *
   * @else
   * @brief Stop a processing at specified micro second time
   *
   * Stop a processing at specified micro second time.
   *
   * @param usec Micro second time
   *
   * @return 0: successful, != 0: failed
   *
   * @endif
   */
  inline int usleep(unsigned int usec)
  {
    struct timeval tv;
    int iret;
    WORD ver;
    WSADATA wsa;
    fd_set mask;
    SOCKET ssoc;

    // The WSAStartup function initiates use of the Winsock DLL by a process.
    ver = MAKEWORD(2, 2);
    iret = ::WSAStartup(ver, &wsa);
    if ( iret != 0 )
      {
        return iret;
      }

    // The socket function creates a socket that is
    // bound to a specific transport service provider.
    ssoc = ::socket(AF_INET, SOCK_STREAM, 0);
    if (ssoc == INVALID_SOCKET)
      {
        iret = ::WSAGetLastError();
        ::WSACleanup();
        return iret;
      }
    FD_ZERO(&mask);
    FD_SET(ssoc, &mask);

    tv.tv_sec = static_cast<long>(usec) / 1000000;
    tv.tv_usec = static_cast<long>(usec) % 1000000;
    iret = ::select(static_cast<int>(ssoc+1), &mask, NULL, NULL, &tv);
    if ( iret == SOCKET_ERROR )
      {
        iret = ::WSAGetLastError();
        // The closesocket function closes an existing socket.
        ::closesocket(ssoc);
        // The WSACleanup function terminates
        // use of the Winsock 2 DLL (Ws2_32.dll).
        ::WSACleanup();
        return iret;
      }
    // The closesocket function closes an existing socket.
    ::closesocket(ssoc);
    // The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
    ::WSACleanup();
    return iret;

  }

  /*!
   * @if jp
   * @brief 時刻とタイムゾーンを取得する
   *
   * 時刻とタイムゾーンを取得する。
   *
   * @param tv 時刻構造体
   * @param tz タイムゾーン構造体
   *
   * @return 0: 成功
   *
   * @else
   * @brief Get the time and timezone
   *
   * Get the time and timezone
   *
   * @param tv Structure of time
   * @param tz Structure of timezone
   *
   * @return 0: successful
   *
   * @endif
   */
  inline int gettimeofday(struct timeval *tv, struct timezone *tz)
  {
      FILETIME        ftime;
      LARGE_INTEGER   lint;
      __int64         val64;
      static int      tzflag;
      if (tv != NULL)
      {
	      ::GetSystemTimeAsFileTime(&ftime);
	      lint.LowPart = ftime.dwLowDateTime;
	      lint.HighPart = static_cast<LONG>(ftime.dwHighDateTime);
	      val64 = lint.QuadPart;
	      val64 = val64 - EPOCHFILETIME;
	      val64 = val64 / 10;
	      tv->tv_sec = (long)(val64 / 1000000);
	      tv->tv_usec = (long)(val64 % 1000000);
      }
      if (tz)
      {
	      if (!tzflag)
	      {
		      ::_tzset();
		      ++tzflag;
	      }
	      long tzone = 0;
	      ::_get_timezone(&tzone);
	      tz->tz_minuteswest = tzone / 60;
	      int dlight = 0;
	      ::_get_daylight(&dlight);
	      tz->tz_dsttime = dlight;
      }
      return 0;
  }

  /*!
   * @if jp
   * @brief 時刻を取得する
   *
   * 時刻を取得する。
   *
   * @return TimeValueオブジェクト
   *
   * @else
   * @brief Get the time
   *
   * Get the time
   *
   * @return TimeValue object
   *
   * @endif
   */
  inline TimeValue gettimeofday()
  {
    timeval tv;
    coil::gettimeofday(&tv, 0);
    return TimeValue(tv.tv_sec, tv.tv_usec);
  }

  /*!
   * @if jp
   * @brief 時刻とタイムゾーンを設定する
   *
   * 時刻とタイムゾーンを設定する。
   *
   * @param tv 時刻構造体
   * @param tz タイムゾーン構造体
   *
   * @return 0: 成功
   *
   * @else
   * @brief Set the time and timezone
   *
   * Set the time and timezone
   *
   * @param tv Structure of time
   * @param tz Structure of timezone
   *
   * @return 0: successful
   *
   * @endif
   */
  inline int settimeofday(const struct timeval *tv , const struct timezone *tz)
  {

    SYSTEMTIME systime;
    FILETIME ftime;
    LARGE_INTEGER lint;
    __int64 val64;

    // tv,tz -> ftime
    if (tv != NULL)
      {
        int bias(0);
        if (tz != NULL)
          {
            bias = tz->tz_minuteswest;
          }

        val64 = (tv->tv_sec + bias * 60) * 1000000;
        val64 = val64 + tv->tv_usec;
        lint.QuadPart = val64;
        ftime.dwHighDateTime = static_cast<DWORD>(lint.LowPart);
        ftime.dwHighDateTime = static_cast<DWORD>(lint.HighPart);
        ::FileTimeToSystemTime(&ftime, &systime);
        ::SetSystemTime(&systime);
      }

    return 0;
  }


  /*!
  * @if jp
  * @brief 高分解能パフォーマンスカウンタから時間を取得する
  * 高分解能パフォーマンスカウンタが利用できない場合はgettimeofday関数から取得する
  *
  * @param tv 時間
  *
  * @return 0: 成功
  *
  * @else
  * @brief
  *
  *
  * @param tv
  *
  * @return 0: successful
  *
  * @endif
  */
  inline int clock(TimeValue &tv)
  {
	  LARGE_INTEGER frequency;
	  BOOL ret = QueryPerformanceFrequency(&frequency);

	  if (ret == 0)
	  {
		  return 1;
	  }
	  else
	  {
		  LARGE_INTEGER current;
		  QueryPerformanceCounter(&current);
		  LONGLONG current_time = (current.QuadPart * 1000000) / frequency.QuadPart;
		  tv = TimeValue((long)(current_time / 1000000), (long)(current_time % 1000000));
		  return 0;
	  }
  }

  /*!
   * @if jp
   * @brief 高分解能パフォーマンスカウンタから時間を取得する
   *
   *
   * @return TimeValueオブジェクト
   *
   * @else
   * @brief 
   *
   *
   *
   * @return TimeValue object
   *
   * @endif
   */
  inline TimeValue clock()
  {
    TimeValue ret;
    if(coil::clock(ret))
    {
      timeval tv;
      coil::gettimeofday(&tv, 0);
      return TimeValue(tv.tv_sec, tv.tv_usec);
    }
    else
    {
      return ret;
    }
  }



} // namespace coil

#endif  // COIL_TIME_H
