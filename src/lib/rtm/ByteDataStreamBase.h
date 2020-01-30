// -*- C++ -*-
/*!
 * @file ByteDataStreamBase.h
 * @brief Data Stream Buffer Base class
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

#ifndef RTC_BYTEDATASTREAMBASE_H
#define RTC_BYTEDATASTREAMBASE_H

#include <coil/Properties.h>
#include <coil/Factory.h>
#include <rtm/Typename.h>

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
   * @class ByteDataStreamBase
   * @brief シリアライザの基底クラス
   *
   *
   * @param 
   *
   * @since 2.0.0
   *
   * @else
   * @class ByteDataStreamBase
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  class ByteDataStreamBase
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
     ByteDataStreamBase();

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
     virtual ~ByteDataStreamBase();

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
     virtual void init(const coil::Properties &prop);
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
     virtual void writeData(const unsigned char *buffer, unsigned long length) = 0;
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
     virtual void readData(unsigned char *buffer, unsigned long length) const = 0;
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
     virtual unsigned long getDataLength() const = 0;
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
     virtual void isLittleEndian(bool little_endian);
  };

  /*!
   * @if jp
   * @class ByteDataStream
   * @brief シリアライザのテンプレートクラス
   * シリアライザを実装する場合は必ずこのクラスを継承する必要がある
   * coil::GlobalFactory <::RTC::ByteDataStream>にシリアライザを登録すると使用可能
   * 使用するデータ型全てに対してファクトリに登録する必要がある
   *
   *
   * @param 
   *
   * @since 2.0.0
   *
   * @else
   * @class ByteDataStream
   * @brief 
   *
   *
   * @since 2.0.0
   *
   * @endif
   */
  template <typename DataType>
  class ByteDataStream : public ByteDataStreamBase
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
     ByteDataStream() = default;

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
     ~ByteDataStream() override = default;

     /*!
      * @if jp
      * @brief データの符号化
      *
      * @param data 符号化前のデータ
      * @return True：成功、False：失敗
      *
      * @else
      * @brief
      *
      * @param data 
      * @return
      *
      * @endif
      */
     virtual bool serialize(const DataType &data) = 0;
     /*!
      * @if jp
      * @brief データの復号化
      *
      * @param data 復号前のデータ
      * @return True：成功、False：失敗
      *
      * @else
      * @brief
      *
      * @param data
      * @return
      *
      * @endif
      */
     virtual bool deserialize(DataType &data) = 0;
  };

  using SerializerFactory = coil::GlobalFactory<ByteDataStreamBase>;

  /*!
   * @if jp
   *
   * @brief シリアライザの名前にデータ型名を追加する
   * 例えば、"corba:RTC/TimedShort:RTC/TimedDouble"という名前のシリアライザ、
   * データ型がTimedDouble型の場合以下のようになる。
   * IDL:RTC/TimedDouble:1.0:corba:RTC/TimedShort:RTC/TimedDouble
   * 
   * @param marshalingtype シリアライザの名称
   * @return 型名追加後のシリアライザ名
   *
   * @else
   *
   * @brief 
   *
   * @param marshalingtype 
   * @return 
   *
   * @endif
   */
  template <class DataType>
  std::string addDataTypeToMarshalingType(const std::string &marshalingtype)
  {
     std::string mtype{std::string(::CORBA_Util::toRepositoryId<DataType>()) + ":" + marshalingtype};
     return mtype;
  }

  /*!
   * @if jp
   *
   * @brief GlobalFactoryにシリアライザを追加する
   * 
   * @param marshalingtype シリアライザの名称
   *
   * @else
   *
   * @brief 
   *
   * @param marshalingtype 
   *
   * @endif
   */
  template <class DataType, class SerializerType>
  void addSerializer(const std::string &marshalingtype)
  {
     std::string mtype = addDataTypeToMarshalingType<DataType>(marshalingtype);
     SerializerFactory::instance()
             .addFactory(mtype,
                         ::coil::Creator<::RTC::ByteDataStreamBase,
                                         SerializerType>,
                         ::coil::Destructor<::RTC::ByteDataStreamBase,
                                            SerializerType>);
  }

  /*!
   * @if jp
   *
   * @brief GlobalFactoryからシリアライザを削除する
   * 
   * @param marshalingtype シリアライザの名称
   *
   * @else
   *
   * @brief 
   *
   * @param marshalingtype 
   *
   * @endif
   */
  template <class DataType>
  void removeSerializer(const std::string &marshalingtype)
  {
     std::string mtype = addDataTypeToMarshalingType<DataType>(marshalingtype);
     SerializerFactory::instance().removeFactory(mtype);
  }

  /*!
   * @if jp
   *
   * @brief GlobalFactoryからシリアライザを生成する
   * 
   * @param marshalingtype シリアライザの名称
   *
   * @else
   *
   * @brief 
   *
   * @param marshalingtype 
   *
   * @endif
   */
  template <class DataType>
  ::RTC::ByteDataStreamBase *createSerializer(const std::string &marshalingtype)
  {
     std::string mtype = addDataTypeToMarshalingType<DataType>(marshalingtype);
     return SerializerFactory::instance().createObject(mtype);
  }

  /*!
   * @if jp
   *
   * @brief 使用可能なシリアライザの一覧を取得する
   * 
   * @return 使用可能なシリアライザの一覧
   *
   * @else
   *
   * @brief 
   *
   * @return
   *
   * @endif
   */
  template <class DataType>
  std::vector<std::string> getSerializerList()
  {
     std::vector<std::string> available_types;
     std::vector<std::string> types = SerializerFactory::instance().getIdentifiers();

     for (auto type : types)
     {
        std::string id{::CORBA_Util::toRepositoryId<DataType>()};
        if (type.size() >= id.size() && std::equal(std::begin(id), std::end(id), std::begin(type)))
        {
           type.erase(0, id.size() + 1);
           available_types.push_back(type);
        }
     }
     return available_types;
  }

} // namespace RTC

EXTERN template class DLL_PLUGIN coil::GlobalFactory<::RTC::ByteDataStreamBase>;

#endif // RTC_BYTEDATASTREAMBASE_H
