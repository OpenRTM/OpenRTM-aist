// -*- C++ -*-
/*!
 * @file CORBA_CdrMemoryStream.h
 * @brief CORBA CDR Stream Buffer class
 * @date $Date: 2019-1-26 03:08:06 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2006-2019
 *     Noriaki Ando
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_CORBA_CDRMEMORYSTREAM_H
#define RTC_CORBA_CDRMEMORYSTREAM_H

#include <rtm/RTC.h>
#include <rtm/idl/DataPort_OpenRTMSkel.h>
#include <rtm/ByteDataStreamBase.h>



 /*!
  * @if jp
  * @namespace RTC
  *
  * @brief RTコンポーネント
  *
  * @else
  *
  * @namespace RTC
  *
  * @brief RT-Component
  *
  * @endif
  */
namespace RTC
{
    /*!
     * @if jp
     * @class CORBA_CdrMemoryStream
     * @brief CDRシリアライザ
     * CDRマーシャリングに関わる関数を提供
     *
     *
     * @param 
     *
     * @since 2.0.0
     *
     * @else
     * @class CORBA_CdrMemoryStream
     * @brief 
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class CORBA_CdrMemoryStream
    {
    public:
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         *
         *
         * @else
         *
         * @brief Constructor
         *
         *
         * @endif
         */
        CORBA_CdrMemoryStream();

        /*!
         * @if jp
         *
         * @brief CDR符号化のテンプレート関数
         *
         * @param data 符号化するデータ
         * @param little_endian リトルエンディアン(True)、ビッグエンディアン(False)
         *
         * @return True：符号化に成功、False：失敗
         *
         * @else
         *
         * @brief 
         *
         * @param data 
         * @param little_endian 
         *
         * @return 
         *
         * @endif
         */
        template<class ExDataType>
        bool serializeCDR(const ExDataType& data)
        {
#ifdef ORB_IS_ORBEXPRESS
            try
            {
                m_cdr.rewind();
                m_cdr.is_little_endian(m_endian);
                m_cdr << data;
                return true;
            }
            catch (...)
            {
                return false;
            }
#elif defined(ORB_IS_TAO)
            try
            {
                m_cdr.reset();
                m_cdr << data;
                return true;
            }
            catch (...)
            {
                return false;
            }
#else
            try
            {
                m_cdr.rewindPtrs();
                m_cdr.setByteSwapFlag(m_endian);
                data >>= m_cdr;
                return true;
            }
            catch (...)
            {
                return false;
            }
#endif
        }

        /*!
         * @if jp
         *
         * @brief CDR復号化のテンプレート関数
         *
         * @param data 格納先の変数
         *
         * @return True：復号化に成功、False：失敗
         *
         * @else
         *
         * @brief 
         *
         * @param data 
         *
         * @return 
         *
         * @endif
         */
        template<class ExDataType>
        bool deserializeCDR(ExDataType& data)
        {
#ifdef ORB_IS_ORBEXPRESS
            try
            {
                m_cdr >> data;
                return true;
            }
            catch (...)
            {
                return false;
            }
#elif defined(ORB_IS_TAO)
            try
            {
                TAO_InputCDR tao_cdr = TAO_InputCDR(m_cdr);
                tao_cdr >> data;
                return true;
            }
            catch (...)
            {
                return false;
            }
#else
            try
            {
                data <<= m_cdr;
                return true;
            }
            catch (...)
            {
                return false;
            }
#endif
        }

        /*!
         * @if jp
         *
         * @brief エンディアンの設定
         *
         * @param little_endian リトルエンディアン(True)、ビッグエンディアン(False)
         *
         *
         * @else
         *
         * @brief 
         *
         * @param little_endian 
         *
         *
         * @endif
         */
        void setEndian(bool little_endian);

        /*!
         * @if jp
         *
         * @brief バッファのポインタ取得
         *
         * @return バッファのポインタ
         *
         *
         * @else
         *
         * @brief 
         *
         * @return 
         *
         *
         * @endif
         */
        const unsigned char* getBuffer();
        /*!
         * @if jp
         *
         * @brief バッファの長さ取得
         *
         * @return バッファの長さ
         *
         *
         * @else
         *
         * @brief
         *
         * @return
         *
         *
         * @endif
         */
        unsigned long getCdrDataLength() const;

        /*!
         * @if jp
         *
         * @brief cdrMemoryStreamオブジェクト取得
         *
         * @return cdrMemoryStream
         *
         *
         * @else
         *
         * @brief
         *
         * @return
         *
         *
         * @endif
         */
#ifdef ORB_IS_ORBEXPRESS
        CORBA::Stream& getCdr();
#elif defined(ORB_IS_TAO)
        TAO_OutputCDR& getCdr();
#else
        cdrMemoryStream& getCdr();
#endif

        /*!
         * @if jp
         *
         * @brief このインスタンスのバッファにデータを書き込む
         *
         * @param buffer 書き込み元のバッファ
         * @param length バッファの長さ
         *
         *
         * @else
         *
         * @brief
         *
         * @param buffer 
         * @param length 
         *
         *
         * @endif
         */
        void writeCdrData(const unsigned char* buffer, unsigned long length);

        /*!
         * @if jp
         *
         * @brief 引数のバッファにデータを書き込む
         *
         * @param buffer 書き込み先のバッファ
         * @param length バッファの長さ
         *
         *
         * @else
         *
         * @brief
         *
         * @param buffer
         * @param length
         *
         *
         * @endif
         */
        void readCdrData(unsigned char* buffer, unsigned long length) const;

        /*!
         * @if jp
         * @brief コピーコンストラクタ
         *
         * @param rhs
         *
         * @else
         * @brief
         *
         * @param rhs
         *
         * @endif
         */
        CORBA_CdrMemoryStream(const CORBA_CdrMemoryStream &rhs)
        {
#ifdef ORB_IS_ORBEXPRESS
            m_cdr.copy(rhs.m_cdr);
#elif defined(ORB_IS_TAO)
        for (const ACE_Message_Block *i = rhs.m_cdr.begin(); i != 0; i = i->cont())
        {
            m_cdr.write_octet_array_mb(i);
        }
#else
            m_cdr = rhs.m_cdr;
#endif
        }


        /*!
         * @if jp
         * @brief 代入演算子
         *
         * @param rhs
         * @return
         *
         * @else
         * @brief
         *
         * @param rhs
         * @return
         *
         * @endif
         */
        CORBA_CdrMemoryStream& operator= (const CORBA_CdrMemoryStream &rhs)
        {
#ifdef ORB_IS_ORBEXPRESS
            m_cdr.copy(rhs.m_cdr);
            return *this;
#elif defined(ORB_IS_TAO)
            for (const ACE_Message_Block *i = rhs.m_cdr.begin(); i != 0; i = i->cont())
            {
                m_cdr.write_octet_array_mb(i);
            }
            return *this;
#else
            m_cdr = rhs.m_cdr;
            return *this;
#endif
        }

    protected:
#ifdef ORB_IS_ORBEXPRESS
        CORBA::Stream m_cdr;
#elif defined(ORB_IS_TAO)
        TAO_OutputCDR m_cdr;
#else
        cdrMemoryStream m_cdr;
#endif
        bool m_endian;
    };
    /*!
     * @if jp
     * @class CORBA_CdrSerializer
     * @brief CORBAのCDRシリアライザの実装
     *
     *
     * @param
     *
     * @since 2.0.0
     *
     * @else
     * @class CORBA_CdrSerializer
     * @brief 
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    template <class DataType>
    class CORBA_CdrSerializer : public ByteDataStream<DataType>
    {
    public:
        /*!
         * @if jp
         *
         * @brief コンストラクタ
         *
         *
         *
         * @else
         *
         * @brief Constructor
         *
         *
         * @endif
         */
        CORBA_CdrSerializer()
        {
        }

        /*!
         * @if jp
         *
         * @brief 仮想デストラクタ
         *
         * 仮想デストラクタ。
         *
         * @else
         *
         * @brief Virtual destractor
         *
         * Virtual destractor
         *
         * @endif
         */
        ~CORBA_CdrSerializer() override
        {
        }

        /*!
         * @if jp
         * @brief 初期化関数(未使用)
         *
         * @param prop プロパティ(コネクタプロファイルから取得)
         *
         * @else
         * @brief
         *
         * @param prop
         *
         * @endif
         */
        void init(const coil::Properties& /*prop*/) override
        {
        }
        /*!
         * @if jp
         * @brief 保持しているバッファにデータを書き込む
         *
         * @param buffer 書き込み元のバッファ
         * @param length データのサイズ
         *
         * @else
         * @brief
         *
         * @param buffer
         * @param length
         *
         *
         * @endif
         */
        void writeData(const unsigned char* buffer, unsigned long length) override
        {
            m_cdr.writeCdrData(buffer, length);
        }

        /*!
         * @if jp
         * @brief 引数のバッファにデータを書き込む
         *
         * @param buffer 書き込み先のバッファ
         * @param length データのサイズ
         *
         * @else
         * @brief
         *
         * @param buffer
         * @param length
         *
         *
         * @endif
         */
        void readData(unsigned char* buffer, unsigned long length) const override
        {
            m_cdr.readCdrData(buffer, length);
        }

        /*!
         * @if jp
         * @brief データの長さを取得
         *
         * @return データの長さ
         *
         * @else
         * @brief
         *
         * @return
         *
         * @endif
         */
        unsigned long getDataLength() const override
        {
            return m_cdr.getCdrDataLength();
        }

        /*!
         * @if jp
         * @brief データの符号化
         *
         * @param data 符号化前のデータ
         * @param little_endian　リトルエンディアン(True)、ビッグエンディアン(False)
         *
         * @else
         * @brief
         *
         * @param data
         * @param little_endian　
         *
         * @endif
         */
        bool serialize(const DataType& data) override
        {
            return m_cdr.serializeCDR(data);
        }

        /*!
         * @if jp
         * @brief データの復号化
         *
         * @param data 復号前のデータ
         *
         * @else
         * @brief
         *
         * @param data
         *
         * @endif
         */
        bool deserialize(DataType& data) override
        {
            return m_cdr.deserializeCDR(data);
        }

        /*!
         * @if jp
         * @brief コピーコンストラクタ
         *
         * @param rhs 
         *
         * @else
         * @brief
         *
         * @param rhs
         *
         * @endif
         */
        CORBA_CdrSerializer<DataType>(const CORBA_CdrSerializer<DataType> &rhs)
        {
            m_cdr = rhs.m_cdr;
        }

        /*!
         * @if jp
         * @brief 代入演算子
         *
         * @param rhs
         * @return
         *
         * @else
         * @brief
         *
         * @param rhs
         * @return
         *
         * @endif
         */
        CORBA_CdrSerializer<DataType>& operator= (const CORBA_CdrSerializer<DataType> &rhs)
        {
            m_cdr = rhs.m_cdr;
            return *this;
        }

        /*!
         * @if jp
         * @brief エンディアンの設定
         *
         * @param little_endian リトルエンディアン(True)、ビッグエンディアン(False)
         *
         * @else
         * @brief
         *
         * @param little_endian
         *
         * @endif
         */
        void isLittleEndian(bool little_endian) override
        {
            m_cdr.setEndian(little_endian);
        }
    private:
        CORBA_CdrMemoryStream m_cdr;
        

    };





} // namespace RTC






/*!
 * @if jp
 * @brief CDRシリアライザの初期化関数
 *
 *
 * @else
 * @brief
 *
 *
 * @endif
 */
template <class DataType>
void CdrMemoryStreamInit()
{
    coil::GlobalFactory < ::RTC::ByteDataStream<DataType> > ::
        instance().addFactory("corba",
            ::coil::Creator< ::RTC::ByteDataStream<DataType>,
            ::RTC::CORBA_CdrSerializer<DataType> >,
            ::coil::Destructor< ::RTC::ByteDataStream<DataType>,
            ::RTC::CORBA_CdrSerializer<DataType> > );
};





#endif  // RTC_CORBA_CDRMEMORYSTREAM_H
