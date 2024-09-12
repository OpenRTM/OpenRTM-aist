#ifndef RTC_BYTEDATA_H
#define RTC_BYTEDATA_H



namespace RTC
{
    class ByteDataStreamBase;
    /*!
     * @if jp
     * @class ByteData
     * @brief シリアライズ後のバイト列を操作するクラス
     * 
     *
     *
     * @param
     *
     * @since 2.0.0
     *
     * @else
     * @class ByteData
     * @brief
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class ByteData
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
        ByteData();
        /*!
         * @if jp
         *
         * @brief デストラクタ
         *
         *
         *
         * @else
         *
         * @brief Destructor
         *
         *
         * @endif
         */
        ~ByteData();
        /*!
         * @if jp
         *
         * @brief コピーコンストラクタ
         *
         * @param rhs
         *
         *
         * @else
         *
         * @brief Copy Constructor
         *
         * @param rhs
         *
         *
         * @endif
         */
        ByteData(const ByteData &rhs);
        /*!
         * @if jp
         *
         * @brief コピーコンストラクタ
         *
         * @param rhs
         *
         *
         * @else
         *
         * @brief Copy Constructor
         *
         * @param rhs
         *
         * @endif
         */
        ByteData(const ByteDataStreamBase &rhs);
        /*!
         * @if jp
         *
         * @brief 代入演算子
         *
         * @param rhs
         * @return
         *
         *
         * @else
         *
         * @brief 
         *
         * @param rhs
         * @return
         *
         * @endif
         */
        ByteData& operator= (const ByteData &rhs);
        /*!
         * @if jp
         *
         * @brief 代入演算子
         *
         * @param rhs
         * @return
         *
         *
         * @else
         *
         * @brief
         *
         * @param rhs
         * @return
         *
         * @endif
         */
        ByteData& operator= (const ByteDataStreamBase &rhs);
        /*!
         * @if jp
         *
         * @brief 引数の変数にデータを格納
         *
         * @param data 書き込み先の変数
         * @param length データの長さ
         * @return
         *
         *
         * @else
         *
         * @brief
         *
         * @param data 
         * @param length 
         * @return
         *
         * @endif
         */
        void readData(unsigned char* data, unsigned long length) const;
        /*!
         * @if jp
         *
         * @brief 内部の変数にデータを格納
         *
         * @param data 書き込み元の変数
         * @param length データの長さ
         * @return
         *
         *
         * @else
         *
         * @brief
         *
         * @param data
         * @param length
         * @return
         *
         * @endif
         */
        void writeData(const unsigned char* data, unsigned long length);
        /*!
         * @if jp
         *
         * @brief バッファのポインタを取得
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
         * @endif
         */
        unsigned char* getBuffer() const;
        /*!
         * @if jp
         *
         * @brief バッファのサイズを取得
         *
         * @return バッファのサイズ
         *
         *
         * @else
         *
         * @brief
         *
         * @return
         *
         * @endif
         */
        unsigned long getDataLength() const;
        /*!
         * @if jp
         *
         * @brief エンディアンの設定
         *
         * @param little_endian リトルエンディアン(True)、ビッグエンディアン(False)
         *
         *
         *
         * @else
         *
         * @brief
         *
         * @param little_endian
         *
         * @endif
         */
        void isLittleEndian(bool little_endian);
        /*!
         * @if jp
         *
         * @brief データのサイズの設定
         *
         * @param length データのサイズ
         *
         *
         *
         * @else
         *
         * @brief
         *
         * @param length
         *
         * @endif
         */
        void setDataLength(unsigned long length);
        /*!
         * @if jp
         *
         * @brief エンディアンの取得
         *
         * @return リトルエンディアン(True)、ビッグエンディアン(False)
         *
         *
         *
         * @else
         *
         * @brief
         *
         * @return
         *
         * @endif
         */
        bool getEndian();
    private:
        unsigned char* m_buf{nullptr};
        unsigned long m_len{0};
        bool m_little_endian{true};
    };

} // namespace RTC


#endif  // RTC_BYTEDATA_H
