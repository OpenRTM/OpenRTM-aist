// -*- C++ -*-
/*!
 * @file  OpenSpliceMessageInfo.cpp
 * @brief Fast-RTPS Massage Type Info class
 * @date  $Date: 2019-02-05 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#include "OpenSpliceMessageInfo.h"
#include "OpenSpliceManagerFunc.h"
#include <coil/stringutil.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include <rtm/Typename.h>
#include <coil/OS.h>
#include <algorithm>

namespace RTC
{
    OpenSpliceMessageInfoBase::~OpenSpliceMessageInfoBase(void) = default;

    template <class DATATYPE, class IDLPATH>
    class OpenSpliceMessageInfo : public OpenSpliceMessageInfoBase
    {
        std::string topic_name(std::string topic) override
        {
            return topic;
        }

        std::string data_type() override
        {
            std::string data_type = ::CORBA_Util::toRepositoryIdOfStruct<DATATYPE>();
            coil::vstring strlist = coil::split(data_type, ":");
            if (strlist.size() != 3)
            {
                return "";
            }
            return coil::replaceString(std::move(strlist[1]), "/", "::");
        }

        std::string idl_path() override
        {
            std::string env;
            std::string path;
            if (coil::getenv("RTM_IDL_PATH", env))
            {
              path = coil::replaceEnv(IDLPATH::idlpath);
            }
            else
            {
              path = coil::replaceString(IDLPATH::idlpath, "${RTM_IDL_PATH}", RTM_IDL_PATH);
            }
            path = coil::replaceString(std::move(path), "//", "/");
            return coil::replaceString(std::move(path), "\\\\", "\\");
        }
    };

    template <class DATATYPE, class IDLPATH>
    void appendOpenSpliceMessageInfo(coil::vstring& datatypes)
    {
        std::string data_type = ::CORBA_Util::toRepositoryIdOfStruct<DATATYPE>();
        GlobalOpenSpliceMessageInfoList::instance().addInfo(data_type,
            new ::RTC::OpenSpliceMessageInfo<DATATYPE, IDLPATH>);
        
        if (datatypes.empty())
        {
            return;
        }
        OpenSpliceMessageInfoBase *info = GlobalOpenSpliceMessageInfoList::instance().getInfo(data_type);
        if (info != nullptr)
        {
            auto itr = std::find(datatypes.begin(), datatypes.end(), info->data_type());
            size_t index = std::distance(datatypes.begin(), itr);

            if (index != datatypes.size() || datatypes[0] == "ALL")
            {
                if (!RTC_OpenSplice::registeredType(info->data_type()))
                {
                    RTC_OpenSplice::registerType(info->data_type(), info->idl_path());
                }
            }
        }
        
    }

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    OpenSpliceMessageInfoList::OpenSpliceMessageInfoList() = default;
    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    OpenSpliceMessageInfoList::~OpenSpliceMessageInfoList()
    {
        for (auto info : m_data)
        {
            info.second.deleteObject();
        }
    }
    
    /*!
     * @if jp
     *
     * @brief OpenSpliceMessageInfoを追加
     * 
     * @param id 名前
     * @param info OpenSpliceMessageInfo
     *
     * @else
     *
     * @brief Destructor
     *
     * @param id 
     * @param info 
     *
     * @endif
     */
    void OpenSpliceMessageInfoList::addInfo(const std::string &id, OpenSpliceMessageInfoBase* info)
    {
        auto data = m_data.find(id);
        if (data != m_data.end())
        {
            data->second.deleteObject();
        }
        m_data[id] = OpenSpliceMessageInfoEntry(info, [](OpenSpliceMessageInfoBase*& obj) { delete obj; });
    }

    /*!
     * @if jp
     *
     * @brief OpenSpliceMessageInfoを削除
     *
     * @param id 名前
     * @return 削除に成功した場合はtrue
     *
     * @else
     *
     * @brief
     *
     * @param id 名前
     * @return
     *
     * @endif
     */
    bool OpenSpliceMessageInfoList::removeInfo(const std::string& id)
    {
        auto data = m_data.find(id);
        if (data == m_data.end())
        {
            return false;
        }

        data->second.deleteObject();
        m_data.erase(data);
        return true;
    }
    /*!
     * @if jp
     *
     * @brief 指定名のOpenSpliceMessageInfoを取得
     *
     * @param id 名前
     * @return OpenSpliceMessageInfo
     *
     * @else
     *
     * @brief
     *
     * @param id
     * @return
     *
     * @endif
     */
    OpenSpliceMessageInfoBase* OpenSpliceMessageInfoList::getInfo(const std::string& id)
    {
        if (m_data.find(id) == m_data.end())
        {
            return nullptr;
        }
        return m_data[id].object_;
    }
    
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @param object OpenSpliceMessageInfo
     * @param destructor 終了関数
     *
     * @else
     *
     * @brief Constructor
     *
     * @param object 
     * @param destructor 
     *
     * @endif
     */
    OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::OpenSpliceMessageInfoEntry(OpenSpliceMessageInfoBase* object, void(*destructor)(OpenSpliceMessageInfoBase*&))
        : object_(object), destructor_(destructor)
    {
    }
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * @endif
     */
    OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::OpenSpliceMessageInfoEntry() = default;

    /*!
     * @if jp
     *
     * @brief コピーコンストラクタ
     *
     * @param obj コピー元のオブジェクト
     *
     * @else
     *
     * @brief Copy Constructor
     *
     * @param obj
     *
     * @endif
     */
    OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::OpenSpliceMessageInfoEntry(const OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry& obj)
    {
        object_ = obj.object_;
        destructor_ = obj.destructor_;
    }

    /*!
     * @if jp
     *
     * @brief 代入演算子
     *
     * @param obj コピー元のオブジェクト
     *
     * @else
     *
     * @brief
     *
     * @param obj
     *
     * @endif
     */
    OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry& OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::operator = (const OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry& obj)
    {
        object_ = obj.object_;
        destructor_ = obj.destructor_;
        return *this;
    }

    /*!
     * @if jp
     *
     * @brief 比較演算子
     *
     * @param obj 比較対象のオブジェクト
     *
     * @else
     *
     * @brief
     *
     * @param obj
     *
     * @endif
     */
    bool OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::operator==(const OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry& obj)
    {
        return (object_ == obj.object_);
    }

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::~OpenSpliceMessageInfoEntry() = default;

    /*!
     * @if jp
     *
     * @brief 終了関数呼び出し
     *
     * @else
     *
     * @brief
     *
     * @endif
     */
    void OpenSpliceMessageInfoList::OpenSpliceMessageInfoEntry::deleteObject()
    {
        if (object_)
        {
            destructor_(object_);
            object_ = nullptr;
        }
    }



}

class BasicDataTypeFile
{
public:
    static const char* idlpath;
};

class ExtendedDataTypesFile
{
public:
    static const char* idlpath;
};

class InterfaceDataTypesFile
{
public:
    static const char* idlpath;
};

const char* BasicDataTypeFile::idlpath = "${RTM_IDL_PATH}/BasicDataType.idl";
const char* ExtendedDataTypesFile::idlpath = "${RTM_IDL_PATH}/ExtendedDataTypes.idl";
const char* InterfaceDataTypesFile::idlpath = "${RTM_IDL_PATH}/InterfaceDataTypes.idl";

void OpenSpliceMessageInfoInit(const coil::Properties& prop)
{
    std::string datalist = prop.getProperty("datatypes", "ALL");
    coil::vstring datatypes;
    for (auto& datatype : coil::split(datalist, ","))
    {
        datatypes.emplace_back(coil::eraseBothEndsBlank(std::move(datatype)));
    }

    RTC::appendOpenSpliceMessageInfo<RTC::TimedState, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedShort, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedLong, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedUShort, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedULong, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedFloat, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedDouble, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedChar, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedBoolean, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedOctet, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedString, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedShortSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedLongSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedUShortSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedULongSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedFloatSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedDoubleSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedCharSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedBooleanSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedOctetSeq, BasicDataTypeFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedStringSeq, BasicDataTypeFile>(datatypes);

    RTC::appendOpenSpliceMessageInfo<RTC::TimedRGBColour, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPoint2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedVector2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPose2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedVelocity2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedAcceleration2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPoseVel2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedSize2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedGeometry2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedCovariance2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPointCovariance2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedCarlike, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedSpeedHeading2D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPoint3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedVector3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedOrientation3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPose3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedVelocity3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedAngularVelocity3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedAcceleration3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedAngularAcceleration3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedPoseVel3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedSize3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedGeometry3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedCovariance3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedSpeedHeading3D, ExtendedDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo<RTC::TimedOAP, ExtendedDataTypesFile>(datatypes);

    RTC::appendOpenSpliceMessageInfo < RTC::ActArrayActuatorPos, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::ActArrayActuatorSpeed, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::ActArrayActuatorCurrent, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::ActArrayState, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::CameraImage, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::Fiducials, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::GPSData, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::GripperState, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::INSData, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::LimbState, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::Hypotheses2D, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::Hypotheses3D, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::Features, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::MultiCameraImages, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::Path2D, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::Path3D, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::PointCloud, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::PanTiltAngles, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::PanTiltState, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::RangeData, InterfaceDataTypesFile>(datatypes);
    RTC::appendOpenSpliceMessageInfo < RTC::IntensityData, InterfaceDataTypesFile>(datatypes);
}
