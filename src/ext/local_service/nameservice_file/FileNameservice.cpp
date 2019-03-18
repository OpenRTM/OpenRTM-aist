// -*- C++ -*-
/*!
 * @file FileNameservice.cpp
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

#include <fstream>
#include <boost/filesystem.hpp>
#include <coil/stringutil.h>
#include <rtm/Manager.h>
#include <rtm/RTObject.h>
#include <rtm/LocalServiceAdmin.h>
#include "FileNameservice.h"

/*!
 * @if jp
 * @brief FileNameService ctor
 * @else
 * @brief FileNameService ctor
 * @endif
 */
static const char* service_name =
  "org.openrtm.local_service.nameservice.file_nameservice";

/*!
 * @if jp
 * @brief FileNameService ctor
 * @else
 * @brief FileNameService ctor
 * @endif
 */
static const char* service_uuid = "7288D080-F618-480B-B6D9-A199686F3101";

/*!
 * @if jp
 * @brief FileNameService ctor
 * @else
 * @brief FileNameService ctor
 * @endif
 */
static const char* default_config[] =
  {
    "base_path",         "/tmp/.openrtm_ns/",
    "file_structure",    "tree",
    "context_delimiter", "|",
    ""
  };

namespace RTM
{
  namespace LocalService
  {
    /*!
     * @if jp
     * @brief FileNameService ctor
     * @else
     * @brief FileNameService ctor
     * @endif
     */
    FileNameservice::FileNameservice() :
      rtclog("file_nameservice")
    {
      m_profile.name = service_name;
      m_profile.uuid = service_uuid;
      coil::Properties prop(default_config);
      m_profile.properties = prop;
      m_profile.service = this;
      RTC_DEBUG(("FileNameservice was created("));
      RTC_DEBUG(("    name = %s", m_profile.name.c_str()));
      RTC_DEBUG(("    uuid = %s", m_profile.uuid.c_str()));
      RTC_DEBUG(("    properties ="));
      RTC_DEBUG_STR((m_profile.properties));
      RTC_DEBUG(("    service = 0x%lx)", this));
    }
    
    /*!
     * @if jp
     * @brief FileNameService dtor
     * @else
     * @brief FileNameService dtor
     * @endif
     */
    FileNameservice::~FileNameservice()
    {
      RTC_TRACE(("~FileNameservice()"))
        cleanupFiles();
    }
    
    /*!
     * @if jp
     * @brief 初期化関数
     * @else
     * @brief Initialization function
     * @endif
     */
    bool
    FileNameservice::init(const ::coil::Properties& props)
    {
      RTC_TRACE(("init()"));
      RTC_DEBUG_STR((props));
      m_profile.properties << props;
      
      RTC::Manager& manager = RTC::Manager::instance();
      manager.addNamingActionListener(new NamingAction(*this));
      return true;
    }
    
    /*!
     * @if jp
     * @brief 再初期化関数
     * @else
     * @brief Reinitialization function
     * @endif
     */
    bool
    FileNameservice::reinit(const ::coil::Properties& props)
    {
      RTC_TRACE(("reinit()"));
      RTC_DEBUG_STR((props));
      m_profile.properties << props;
      return true;
    }
    
    /*!
     * @if jp
     * @brief LocalServiceProfile を取得する
     * @else
     * @brief Getting LocalServiceProfile
     * @endif
     */
    const LocalServiceProfile&
    FileNameservice::getProfile() const
    {
      RTC_TRACE(("getProfile()"));
      return m_profile;
    }
    
    /*!
     * @if jp
     * @brief 終了関数
     * @else
     * @brief Finalization function
     * @endif
     */
    void
    FileNameservice::finalize()
    {
      cleanupFiles();
    }
    
    /*!
     * @if jp
     * @brief 名前登録時に呼ばれるコールバック
     * @else
     * @brief A call-back at name registration
     * @endif
     */
    void
    FileNameservice::onRegisterNameservice(coil::vstring& ns_path,
                                           coil::vstring& ns_info)
    {
      RTC_TRACE(("onRegisterNameservice(path = %s",
                 coil::flatten(ns_path).c_str()));
      RTC_TRACE((" nsinfo = %s", coil::flatten(ns_info).c_str()));
      
      for (auto & ns_name : ns_path)
        {
          fs::path filepath(getFname(ns_name).c_str());
          fs::path directory = filepath.parent_path();
          RTC_DEBUG(("file path: %s", filepath.string().c_str()));
          RTC_DEBUG(("directory: %s", directory.string().c_str()));
          if (!createDirectory(directory))
            {
              continue;
            }
          try
            {
              fs::path filename = filepath.filename();
              RTC_DEBUG(("file name: %s", filename.string().c_str()));
              std::ofstream ofs(filepath.string().c_str(),
                                std::ios::out | std::ios::trunc);
              for (const auto & ior : ns_info)
                {
                  ofs << ior << std::endl;
                }
              RTC_INFO(("RTC %s's IOR has been successfully registered: %s",
                        filename.string().c_str(), filepath.string().c_str()));
              m_files.push_back(filepath.string());
            }
          catch (...)
            {
              RTC_ERROR(("Creating file has been failed. %s",
                         filepath.string().c_str()));
            }
        }
    }
    
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
    void FileNameservice::onUnregisterNameservice(coil::vstring& ns_path)
    {
      RTC_TRACE(("onUnregisterNameservice(%s)",
                 coil::flatten(ns_path).c_str()));
      for (auto & ns_name : ns_path)
        {
          fs::path filepath(getFname(ns_name));
          if (!fs::exists(filepath))
            {
              RTC_ERROR(("No such file: %s", filepath.string().c_str()));
              continue;
            }
          std::vector<std::string>::iterator it;
          it = std::find(m_files.begin(), m_files.end(), filepath.string());
          if (it == m_files.end())
            {
              RTC_WARN(("This file (%s) might not be my file.",
                        filepath.string().c_str()));
              continue;
            }
          m_files.erase(it);
          boost::system::error_code error;
          const bool result = fs::remove(filepath); // , error);
          RTC_DEBUG(("Removing file: %s", filepath.string().c_str()));
          if (!result || (error != nullptr))
            {
              RTC_ERROR(("Removing a file has been failed. %s",
                         filepath.string().c_str()));
              continue;
            }
          RTC_PARANOID(("Removing a file done: %s", filepath.string().c_str()));
        }
      return;
    }
    
    /*!
     * @if jp
     * @brief ディレクトリ作成
     * TODO: Documentation
     * @else
     * @brief Creating directories
     * TODO: Documentation
     * @endif
     */
    bool FileNameservice::createDirectory(fs::path& directory)
    {
      RTC_TRACE(("onUnregisterNameservice(%s)", directory.string().c_str()));
      if (!fs::exists(directory))
        {
          RTC_DEBUG(("Directory %s not found", directory.string().c_str()));
          boost::system::error_code error;
          const bool result = fs::create_directories(directory); //, error);
          RTC_DEBUG(("Creating directory: %s", directory.string().c_str()));
          if (!result || (error != nullptr))
            {
              RTC_ERROR(("Creating directory has been failed. %s",
                         directory.string().c_str()));
              return false;
            }
          RTC_PARANOID(("Creating directory done: %s",
                        directory.string().c_str()));
        }
      else if (fs::exists(directory) && fs::is_directory(directory))
        {
          RTC_DEBUG(("Directory %s exists.", directory.string().c_str()));
        }
      else
        {
          RTC_ERROR(("File exists instead of base directory %s.",
                     directory.string().c_str()));
          return false;
        }
      return true;
    }
    
    /*!
     * @if jp
     * @brief ファイル名取得
     * TODO: Documentation
     * @else
     * @brief Getting file name
     * TODO: Documentation
     * @endif
     */
    std::string
    FileNameservice::getFname(std::string& ns_path) const
    {
      RTC_TRACE(("getFname(%s)", ns_path.c_str()));
      
      std::string pathstring(m_profile.properties["base_path"]);
      pathstring += "/";
      
      std::string fs(m_profile.properties["file_structure"]);
      coil::normalize(fs);
      if (fs == "flat")
        {
          RTC_DEBUG(("file_structure = flat"));
          std::string d(m_profile.properties["context_delimiter"]);
          coil::replaceString(ns_path, "/", d);
          pathstring += ns_path;
        }
      else if (fs == "tree")
        {
          RTC_DEBUG(("file_structure = tree"));
          pathstring += ns_path;
        }
      RTC_DEBUG(("path string = %s", pathstring.c_str()));
      
      return pathstring;
    }
    
    /*!
     * @if jp
     * @brief 全ファイル削除
     * TODO: Documentation
     * @else
     * @brief Deleting all files
     * TODO: Documentation
     * @endif
     */
    void
    FileNameservice::cleanupFiles()
    {
      RTC_TRACE(("cleanupFiles()"));
      for (auto & m_file : m_files)
        {
          fs::path p(m_file);
          fs::remove(p);
        }
      m_files.clear();
    }
    
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    NamingAction::NamingAction(FileNameservice& fns) :
      m_fns(fns)
    {
    }
    
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    NamingAction::~NamingAction()
    {
    }
    
    /*!
     * @if jp
     * @brief postBind コールバック関数
     * TODO: Documentation
     * @else
     * @brief postBind callback function
     * TODO: Documentation
     * @endif
     */
    void
    NamingAction::preBind(RTC::RTObject_impl* rtobj, coil::vstring& name_)
    {
      coil::vstring name = name_;
#ifndef ORB_IS_RTORB
      CORBA::Object_var objref =
        RTC::RTObject::_duplicate(rtobj->getObjRef());
#else
      RTC::RTObject_var objref =
        RTC::RTObject::_duplicate(rtobj->getObjRef());
#endif // ORB_IS_RTORB
      CORBA::String_var ior =
        RTC::Manager::instance().getORB()->object_to_string(objref);
      coil::vstring ns_info;
      ns_info.push_back(static_cast<const char*>(ior));
      m_fns.onRegisterNameservice(name, ns_info);
    }
    
    /*!
     * @if jp
     * @brief postBind コールバック関数
     * TODO: Documentation
     * @else
     * @brief postBind callback function
     * TODO: Documentation
     * @endif
     */
    void
    NamingAction::postBind(RTC::RTObject_impl* /*rtobj*/, coil::vstring& /*name*/)
    {
    }
    
    /*!
     * @if jp
     * @brief preUnbind コールバック関数
     * TODO: Documentation
     * @else
     * @brief preUnbind callback function
     * TODO: Documentation
     * @endif
     */
    void
    NamingAction::preUnbind(RTC::RTObject_impl* /*rtobj*/, coil::vstring& /*name*/)
    {
    }
    
    /*!
     * @if jp
     * @brief postUnbind コールバック関数
     * TODO: Documentation
     * @else
     * @brief postUnbind callback function
     * TODO: Documentation
     * @endif
     */
    void
    NamingAction::postUnbind(RTC::RTObject_impl*  /*rtobj*/, coil::vstring& name)
    {
      m_fns.onUnregisterNameservice(name);
    }
    
  } // namespace LocalService
} // namespace RTM

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void FileNameserviceInit()
  {
    RTM::LocalServiceFactory& factory
      = RTM::LocalServiceFactory::instance();
    factory.addFactory(service_name,
                       ::coil::Creator< ::RTM::LocalServiceBase,
                       ::RTM::LocalService::FileNameservice>,
                       ::coil::Destructor< ::RTM::LocalServiceBase,
                       ::RTM::LocalService::FileNameservice>);
  }
};
