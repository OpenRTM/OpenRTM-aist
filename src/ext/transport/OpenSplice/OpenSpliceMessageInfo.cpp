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
            std::string path = coil::replaceEnv(IDLPATH::idlpath);
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
            delete info.second;
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
        m_data[id] = info;
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
        if (m_data.count(id) == 0)
        {
            return false;
        }
        m_data.erase(id);
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
        if (m_data.count(id) == 0)
        {
            return nullptr;
        }
        return m_data[id];
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

const char* BasicDataTypeFile::idlpath = "${RTM_IDL_PATH}/BasicDatatype.idl";
const char* ExtendedDataTypesFile::idlpath = "${RTM_IDL_PATH}/ExtendedDataTypes.idl";
const char* InterfaceDataTypesFile::idlpath = "${RTM_IDL_PATH}/InterfaceDataTypes.idl";

void OpenSpliceMessageInfoInit(const coil::Properties& prop)
{
    std::string datalist = prop.getProperty("opensplice.datatypes", "ALL");
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
