// -*- C++ -*-
/*!
 * @file FileNameservice.h
 * @brief FileNameservice 
 * @date 2011/08/20
 * @author n-ando
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 * 
 */

#ifndef RTM_LOCALSERVICE_FILENAMESERVICE_H
#define RTM_LOCALSERVICE_FILENAMESERVICE_H

#include <coil/stringutil.h>

#include <rtm/RTC.h>
#include <rtm/SystemLogger.h>
#include <rtm/LocalServiceBase.h>

namespace fs = boost::filesystem;

namespace RTM
{
  namespace LocalService
  {
    /*!
     * @if jp
     * @class FileNameservice クラス
     * @brief FileNameservice クラス
     * @else
     * @class FileNameservice class
     * @brief FileNameservice class
     * @endif
     */
    class FileNameservice
      : public RTM::LocalServiceBase
    {
    public:
      /*!
       * @if jp
       * @brief FileNameService ctor
       * @else
       * @brief FileNameService ctor
       * @endif
       */
      FileNameservice();
      
      /*!
       * @if jp
       * @brief FileNameService dtor
       * @else
       * @brief FileNameService dtor
       * @endif
       */
      ~FileNameservice() override;
      
      /*!
       * @if jp
       * @brief 初期化関数
       *
       * TODO: Documentation
       *
       * @param profile 外部から与えられた property
       * @return 
       *
       * @else
       * @brief Initialization function
       *
       * TODO: Documentation
       *
       * @endif
       */
      bool
      init(const ::coil::Properties& props) override;
      
      /*!
       * @if jp
       * @brief 再初期化関数
       *
       * TODO: Documentation
       *
       * @param profile 外部から与えられた property
       * @return 
       *
       * @else
       * @brief Reinitialization function
       *
       * TODO: Documentation
       *
       * @endif
       */
      bool
      reinit(const ::coil::Properties& props) override;
      
      /*!
       * @if jp
       * @brief LocalServiceProfile を取得する
       *
       * TODO: Documentation
       *
       * @return このオブジェクトが保持している LocalServiceProfile
       *
       * @else
       * @brief Getting LocalServiceProfile
       *
       * TODO: Documentation
       *
       * @return LocalServiceProfile of this service class
       *
       * @endif
       */
      const LocalServiceProfile& getProfile() const override;
      
      /*!
       * @if jp
       * @brief 終了関数
       *
       * TODO: Documentation
       *
       * @else
       * @brief Finalization function
       *
       * TODO: Documentation
       *
       * @endif
       */
      void finalize() override;
      
      /*!
       * @if jp
       * @brief 名前登録時に呼ばれるコールバック
       *
       * TODO: Documentation
       *
       * @else
       * @brief A call-back at name registration
       *
       * TODO: Documentation
       *
       * @endif
       */
      void
      onRegisterNameservice(coil::vstring& path, coil::vstring& ns_info);
      
      /*!
       * @if jp
       * @brief 名前登録解除に呼ばれるコールバック
       *
       * TODO: Documentation
       *
       * @else
       * @brief A call-back at name runegistration
       *
       * TODO: Documentation
       *
       * @endif
       */
      void
      onUnregisterNameservice(coil::vstring& path);
      
    protected:
      /*!
       * @if jp
       * @brief ディレクトリ作成
       * TODO: Documentation
       * @else
       * @brief Creating directories
       * TODO: Documentation
       * @endif
       */
      bool createDirectory(fs::path& directory);
      
      /*!
       * @if jp
       * @brief ファイル名取得
       * TODO: Documentation
       * @else
       * @brief Getting file name
       * TODO: Documentation
       * @endif
       */
      std::string getFname(std::string& path) const;
      
      /*!
       * @if jp
       * @brief 全ファイル削除
       * TODO: Documentation
       * @else
       * @brief Deleting all files
       * TODO: Documentation
       * @endif
       */
      void cleanupFiles();
      
    private:
      RTM::LocalServiceProfile m_profile;
      coil::vstring m_files;
      mutable RTC::Logger rtclog;
    };
    
    /*!
     * @if jp
     * @class NamingAction class
     * TODO: Documentation
     * @else
     * @class NamingActin class
     * TODO: Documentation
     * @endif
     */
    class NamingAction
      : public RTM::NamingActionListener
    {
    public:
      /*!
       * @if jp
       * @brief コンストラクタ
       * @else
       * @brief Constructor
       * @endif
       */
      NamingAction(FileNameservice& fns);

      /*!
       * @if jp
       * @brief デストラクタ
       * @else
       * @brief Destructor
       * @endif
       */
      ~NamingAction() override;

      /*!
       * @if jp
       * @brief preBind コールバック関数
       * TODO: Documentation
       * @else
       * @brief preBind callback function
       * TODO: Documentation
       * @endif
       */
      void preBind(RTC::RTObject_impl* rtobj,
                           coil::vstring& name) override;
      /*!
       * @if jp
       * @brief postBind コールバック関数
       * TODO: Documentation
       * @else
       * @brief postBind callback function
       * TODO: Documentation
       * @endif
       */
      void postBind(RTC::RTObject_impl* rtobj,
                            coil::vstring& name) override;
      
      /*!
       * @if jp
       * @brief preUnbind コールバック関数
       * TODO: Documentation
       * @else
       * @brief preUnbind callback function
       * TODO: Documentation
       * @endif
       */
      void preUnbind(RTC::RTObject_impl* rtobj,
                             coil::vstring& name) override;
      
      /*!
       * @if jp
       * @brief postUnbind コールバック関数
       * TODO: Documentation
       * @else
       * @brief postUnbind callback function
       * TODO: Documentation
       * @endif
       */
      void postUnbind(RTC::RTObject_impl* rtobj,
                              coil::vstring& name) override;
    private:
      RTM::LocalService::FileNameservice& m_fns;
    };
    
  } // namespace LocalService
} // namespace RTM

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * FileNameserviceをファクトリに登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers FileNameservice to the factory.
   *
   * @endif
   */
  DLL_EXPORT void FileNameserviceInit();
};

#endif /* FILENAMESERVICE_H_ */
