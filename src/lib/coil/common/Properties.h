// -*- C++ -*-
/*!
 * @file Properties.h
 * @brief Property list class (derived from Java Properties)
 * @date $Date: 2007-12-31 03:08:06 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_PROPERTIES_H
#define COIL_PROPERTIES_H


#include <string>
#include <vector>
#include <map>
#include <climits>


/*!
 * @if jp
 * @namespace coil
 *
 * @brief Common Object Interface Layer
 *
 *
 * @else
 * @namespace coil
 *
 * @brief Common Object Interface Layer
 *
 *
 * @endif
 */
namespace coil
{
  /*!
   * @if jp
   *
   * @class Properties
   * @brief プロパティセットを表現するクラス
   *
   * Properties クラスは、不変のプロパティセットを表す。 Properties をストリーム
   * に保管したり、ストリームからロードしたりすることができる。
   * プロパティリストの各キー、およびそれに対応する値は文字列となっている。
   *
   * プロパティリストには、その「デフォルト値」として別のプロパティリストを持つ
   * ことができる。元のプロパティリストでプロパティキーが見つからないと、この
   * 2番目のプロパティリストが検索される。
   *
   * プロパティの取得には getProperty() 、プロパティのセットには setProperty() と
   * いったメソッドを使用することが推奨される。
   *
   * プロパティをストリームに保存するとき、またはストリームからロードするとき
   * に、ISO 8859-1 文字エンコーディングが使用される。このエンコーディングに
   * 直接表示できない文字は、扱うことができない。
   *
   * このクラスは、Java の Properties クラス (java.util.Properties) とほぼ同様の
   * メソッドを持つ。また、入出力されるファイルは Java の Properties クラスが
   * 出力するものと互換性があるが、Unicode を含むものは扱うことができない。
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class Properties
   * @brief Class represents a set of properties
   *
   * The Properties class represents a persistent set of properties. The
   * Properties can be saved to a stream or loaded from a stream. Each key and
   * its corresponding value in the property list is a string.
   *
   * A property list can contain another property list as its "defaults". This
   * second property list is searched if the property key is not found in the
   * original property list.
   *
   * It is recommended to use these method; setProperty() to get properties,
   * setProperty() to set properties.
   *
   * When propertis are stored in a stream or when they are loaded from
   * the stream, the ISO 8859-1 character encoding is used.
   * Characters that cannot be directly represented in this encoding can be used.
   *
   * This class has almost same methods of Java's Properties class
   * (java.util.Properties). Also, input and output files are compatible with
   * outputs of Java's Properties class, but Unicode encoded property file
   * is not be supported.
   *
   * @endif
   */
  class Properties
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ(rootノードのみ作成)
     *
     * key と value のみを与えて Property のルートノードを作成する。
     * 値は全てデフォルト値として設定される。
     *
     * @param key プロパティのキー(デフォルト値:"")
     * @param value プロパティの値(デフォルト値:"")
     *
     * @else
     *
     * @brief Constructor(Create only root node)
     *
     * Create a root node of Property with root's key and value.
     * All values are set as default value.
     *
     * @param key Properties's keys(The default values:"")
     * @param value Properties's values(The default values:"")
     *
     * @endif
     */
    explicit Properties(const char* key = "", const char* value = "");

    /*!
     * @if jp
     *
     * @brief コンストラクタ(mapでデフォルト値を与える)
     *
     * std::string の std::map をデフォルト値にもつ Properties を作成する。
     * 値は全てデフォルト値として設定される。
     *
     * @param defaults デフォルト値として指定されるmap
     *
     * @else
     *
     * @brief Constructor(Give the default value with map)
     *
     * Create Properties with default value of std::string map.
     * All values are set as default value.
     *
     * @param defaults map that is spcified as the default value
     *
     * @endif
     */
    explicit Properties(std::map<std::string, std::string>& defaults);

    /*!
     * @if jp
     *
     * @brief コンストラクタ(char*[] でデフォルト値を与える)
     *
     * 指定されたデフォルト値を持つ空のプロパティリストを作成する。
     * 値は全てデフォルト値として設定される。
     * デフォルト値は char* の配列により与えられ、key と value の対になって
     * おり、リストの終端は配列の数を表す引数 num か、空文字の key で与えらられ
     * なければならない。
     * 以下に例を示す。
     *
     * <pre>
     * const char* defaults = {
     *     "key1", "value1",
     *     "key2", "value2",
     *     "key3", "value3",
     *     "key4", "value4",
     *     "key5", "value5",
     *     "" };
     * Properties p(defaults);
     * // もしくは
     * Properties p(defaults, 10);
     * </pre>
     *
     * @param defaults デフォルト値を指定する配列
     * @param num デフォルト値を設定する要素数(デフォルト値:LONG_MAX)
     *
     * @else
     *
     * @brief Constructor(Give the default value with char*[])
     *
     * Creates an empty property list with the specified defaults.
     * All values are set as the default values.
     * The default values are given by array of char*, which should be pairs
     * of "key" and "value". The end of list is specified by argument "num"
     * which specifies number of array, or null character of key.
     * The following is an example.
     *
     * <pre>
     * const char* defaults = {
     *     "key1", "value1",
     *     "key2", "value2",
     *     "key3", "value3",
     *     "key4", "value4",
     *     "key5", "value5",
     *     "" };
     * Properties p(defaults);
     * // or
     * Properties p(defaults, 10);
     * </pre>
     *
     * @param defaults Array that specifies the default values
     * @param num Number of elements that specifies the default value
     *            (The default value:LONG_MAX)
     *
     * @endif
     */
    explicit Properties(const char** defaults, long num = LONG_MAX);

    /*!
     * @if jp
     * @brief コピーコンストラクタ
     *
     * 引数に与えられた Properties のキー、値およびデフォルト値が
     * 全てそのままコピーされる。
     *
     * @else
     *
     * @brief Copy Constructor
     *
     * All of given Properties's keys, values and default values
     * are copied to new Properties.
     *
     * @endif
     */
    Properties(const Properties& prop);

    /*!
     * @if jp
     * @brief 代入演算子
     *
     * 左辺値の Properties のキー、値およびデフォルト値は全て削除され、
     * 右辺値の Properties のキー、値およびデフォルト値が全てそのまま
     * コピーされる。
     *
     * @else
     * @brief Assignment operator
     *
     * All Properties's keys, values and default values of left side
     * are deleted, all Properties's keys, values and default values of
     * right side are copied to new Properties.
     *
     * @endif
     */
    Properties& operator=(const Properties& prop);

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
    virtual ~Properties();

    //============================================================
    // public functions
    //============================================================

    /*!
     * @if jp
     * @brief Name の取得
     *
     * プロパティの名称を取得する。
     *
     * @return プロパティ名
     *
     * @else
     * @brief Get Names
     *
     * Get Properties's names.
     *
     * @return Properties's names
     *
     * @endif
     */
    inline const char* getName() const          {return name.c_str();}

    /*!
     * @if jp
     * @brief 値の取得
     *
     * プロパティの値を取得する。
     *
     * @return プロパティ値
     *
     * @else
     * @brief Get values
     *
     * Get Properties's values.
     *
     * @return Properties's values
     *
     * @endif
     */
    inline const char* getValue() const         {return value.c_str();}

    /*!
     * @if jp
     * @brief デフォルト値の取得
     *
     * プロパティのデフォルト値を取得する。
     *
     * @return プロパティデフォルト値
     *
     * @else
     * @brief Get default values
     *
     * Get Properties's default values.
     *
     * @return Properties's default values
     *
     * @endif
     */
    inline const char* getDefaultValue() const
    {
      return default_value.c_str();
    }

    /*!
     * @if jp
     * @brief 子要素の取得
     *
     * プロパティの子要素を取得する。
     *
     * @return 子要素
     *
     * @else
     * @brief Get elements of leaf
     *
     * Get Properties's elements of leaf.
     *
     * @return Elements of leaf
     *
     * @endif
     */
    inline const std::vector<Properties*>& getLeaf() const {return leaf;}

    /*!
     * @if jp
     * @brief ルート要素の取得
     *
     * プロパティのルート要素を取得する。
     *
     * @return ルート要素
     *
     * @else
     * @brief Get root element
     *
     * Get Properties's root element.
     *
     * @return Root element
     *
     * @endif
     */
    inline const Properties* getRoot() const    {return root;}

    /*!
     * @if jp
     *
     * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
     *
     * 指定されたキーを持つプロパティを、プロパティリストから探す。
     * そのキーがプロパティリストにないと、デフォルトのプロパティリスト、
     * さらにそのデフォルト値が繰り返し調べられる。
     * そのプロパティが見つからない場合は、null が返される。
     *
     * @param key プロパティキー
     *
     * @return 指定されたキー値を持つこのプロパティリストの値
     *
     * @else
     *
     * @brief Search for the property with the specified key in this property
     *
     * Search for the property with the specified key in this property list.
     * If the key is not found in this property list, the default property list,
     * and its defaults, recursively, are then checked. The method returns null
     * if the property is not found.
     *
     * @param key The property key.
     *
     * @return The value in this property list with the specified key value.
     *
     * @endif
     */
    const std::string& getProperty(const std::string& key) const;

    /*!
     * @if jp
     *
     * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
     *
     * 指定されたキーを持つプロパティを、プロパティリストから探す。
     * そのキーがプロパティリストにない場合は、デフォルト値の引数が返される。
     *
     * @param key プロパティキー
     * @param def デフォルト値
     *
     * @return 指定されたキー値を持つこのプロパティリストの値
     *
     * @else
     *
     * @brief Search for the property with the specified key in property list
     *
     * Search for the property with the specified key in this property list.
     * The method returns the default value argument if the property is not
     * found.
     *
     * @param key The property key
     * @param def The  default value.
     *
     * @return The value in this property list with the specified key value.
     *
     * @endif
     */
    const std::string& getProperty(const std::string& key,
                                   const std::string& def) const;

    /*!
     * @if jp
     *
     * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
     *
     * 指定されたキーを持つプロパティを返す。
     * そのキーがプロパティリストになければデフォルト値を返す。
     * さらに見つからなければ、空文字を返す。
     *
     * @param key プロパティキー
     *
     * @return 指定されたキー値を持つこのプロパティリストの値
     *
     * @else
     *
     * @brief Search for the property with the specified key in property list
     *
     * Search for the property with the specified key in this property list.
     * If the key is not found in this property list, the default property list,
     * and its defaults, recursively, are then checked. The method returns
     * empty string if the property is not found.
     *
     * @param key The property key
     *
     * @return The value in this property list with the specified key value.
     *
     * @endif
     */
    const std::string& operator[](const std::string& key) const;

    /*!
     * @if jp
     *
     * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
     *
     * 指定されたキーを持つプロパティを返す。
     * そのキーの値がプロパティリストになければデフォルト値を返す。
     * さらに見つからなければ、空文字を返す。
     * 左辺値になる場合に、同じ値を持つ要素がないときは与えられたキー
     * に対応するプロパティに右辺値を挿入。
     *
     * @param key プロパティキー
     *
     * @return 指定されたキー値を持つこのプロパティリストの値
     *
     * @else
     *
     * @brief Search for the property with the specified key in property list
     *
     * Search for the property with the specified key in this property list.
     * If the key is not found in this property list, the default property list,
     * and its defaults, recursively, are then checked. The method returns
     * empty string if the property is not found.
     * If there is no element with the same value in the left value, insert the
     * right value in corresponding property.
     *
     * @param key The property key
     *
     * @return The value in this property list with the specified key value.
     *
     * @endif
     */
    std::string& operator[](const std::string& key);

    /*!
     * @if jp
     * @brief 指定されたキーに対してデフォルト値を取得する
     *
     * 指定されたキーを持つプロパティのデフォルト値を返す。
     * 指定されたキーを持つプロパティが存在しない場合には空文字を返す。
     *
     * @param key プロパティキー
     *
     * @return 指定されたキー値を持つプロパティのデフォルト値
     *
     * @else
     * @brief Get the default values with specified key.
     *
     * Return the default values with specified key.
     * If the property with specified key does not exist, the method returns
     * empty string.
     *
     * @param key The property key
     *
     * @return The value in this property list with the specified key value.
     *
     * @endif
     */
    const std::string& getDefault(const std::string& key) const;

    /*!
     * @if jp
     *
     * @brief Properties に value を key について登録する
     *
     * Properties に value を key について登録する。
     * すでに key に対する値を持っている場合、戻り値に古い値を返す。
     *
     * @param key プロパティリストに配置されるキー
     * @param value key に対応する値
     *
     * @return プロパティリストの指定されたキーの前の値。それがない場合は null
     *
     * @else
     *
     * @brief Set a value associated with key in the property list
     *
     * This method sets the "value" associated with "key" in the property list.
     * If the property list has a value of "key", old value is returned.
     *
     * @param key The key to be placed into this property list.
     * @param value The value corresponding to key.
     *
     * @return The previous value of the specified key in this property list,
     *         or null if it did not have one.
     *
     *@endif
     */
    std::string setProperty(const std::string& key, const std::string& invalue);

    /*!
     * @if jp
     * @brief デフォルト値を登録する
     *
     * key で指定される要素にデフォルト値を登録する。
     *
     * @param key デフォルト値を登録するプロパティのキー
     * @param value 登録されるデフォルト値
     *
     * @return 指定されたデフォルト値
     *
     * @else
     * @brief Set a default value associated with key in the property list
     *
     * Set the default value to element specified by "key".
     *
     * @param key Property's key to set the default value
     * @param value The default value that is set
     *
     * @return The specified default value
     *
     * @endif
     */
    std::string setDefault(const std::string& key, const std::string& invalue);

    /*!
     * @if jp
     * @brief Properties にデフォルト値をまとめて登録する
     *
     * 配列で指定された要素にデフォルト値をまとめて登録する。
     * デフォルト値は char* の配列により与えられ、key と value の対になって
     * おり、リストの終端は配列の数を表す引数 num か、空文字の key で与えらられ
     * なければならない。
     *
     * @param defaults デフォルト値を指定する配列
     * @param num デフォルト値を設定する要素数(デフォルト値:LONG_MAX)
     *
     * @else
     * @brief Set a default value together in the property list
     *
     * Set the default value to the element specified by array together in the
     * property list.
     * The default values are given by array of char*, which should be pairs
     * of "key" and "value". The end of list is specified by argument "num",
     * which specifies number of array or null character of key.
     *
     * @param defaults Array that specifies the default values
     * @param num Number of elements that specifies the default value
     *            (Default value:LONG_MAX)
     *
     * @endif
     */
    void setDefaults(const char* defaults[], long num = LONG_MAX);

    //============================================================
    // load and save functions
    //============================================================
    /*!
     * @if jp
     *
     * @brief 指定された出力ストリームに、プロパティリストを出力する
     *
     * 指定された出力ストリームに、プロパティリストを出力する。
     * このメソッドは主にデバッグに用いられる。
     *
     * @param out 出力ストリーム
     *
     * @else
     *
     * @brief Prints this property list out to the specified output stream
     *
     * Prints this property list out to the specified output stream.
     * This method is useful for debugging.
     *
     * @param out Output stream.
     *
     * @endif
     */
    void list(std::ostream& out);

    /*!
     * @if jp
     *
     * @brief 入力ストリームからキーと要素が対になったプロパティリストを読み込む
     *
     * 入力ストリームからキーと要素が対になったプロパティリストを読み込む。
     * ストリームは、ISO 8859-1 文字エンコーディングを使用しているとみなされる。
     * 各プロパティは、入力ストリームに行単位で登録されているものとみなされ、
     * 各行は行区切り文字 (\\n、\\r、または \\r\\n) で終わる。
     * 入力ストリームから読み込んだ行は、入力ストリームでファイルの終わりに
     * 達するまで処理される。
     *
     * 空白文字だけの行、または最初の非空白文字が ASCII 文字 # または ! である
     * 行は無視される。つまり、# または ! はコメント行を示す。
     *
     * 空白行またはコメント行以外のすべての行は、テーブルに追加されるプロパティ
     * を記述する。ただし、行の終わりが \ の場合は、次の行があれば継続行として
     * 扱われる (下記を参照)。 キーは、最初の非空白文字から、最初の ASCII 文字
     * =、:、または空白文字の直前までの、行内のすべての文字から構成される。
     *
     * キーの終わりを示す文字は、前に \ を付けることによりキーに含めることも
     * できる。キーの後ろの空白はすべてスキップされる。
     * キーの後ろの最初の非空白文字が = または : である場合は、これらのキーは
     * 無視され、そのあとの空白文字もすべてスキップされる。
     * 行内のそれ以外の文字はすべて、関連した要素文字列の一部となる。
     * 要素文字列内では、ASCII エスケープシーケンス \\t、\\n、\\r、\\\\、\\"、
     * \\'、\\ (円記号とスペース)、および \\uxxxx は認識され、単独の文字に変換
     * される。
     * また、行の最後の文字が \ である場合は、次の行は現在の行の継続として
     * 扱われる。その場合、\ と行区切り文字が破棄され、継続行の先頭に空白が
     * あればそれもすべて破棄され、要素文字列の一部にはならない。
     *
     * たとえば、次の 3 行はそれぞれキー Truth と関連した要素値 Beauty を表す。
     *
     * Truth = Beauty <BR>
     * Truth:Beauty <BR>
     * Truth\\t\\t\\t:Beauty <BR>
     *
     * また、次の 3 行は 1 つのプロパティを表す。
     *
     * fruits\\t\\t\\t\\tapple, banana, pear, \ <BR>
     *                                  cantaloupe, watermelon, \ <BR>
     *                                  kiwi, mango <BR>
     * キーは fruits で、次の要素に関連付けれられる。
     * "apple, banana, pear, cantaloupe, watermelon, kiwi, mango"
     * 最終的な結果でコンマのあとに必ずスペースが表示されるように、
     * 各 \ の前にスペースがある。行の終わりを示す \ と、継続行の先頭にある
     * 空白は破棄され、他の文字に置換されない。
     * また、次の 3 番目の例では、キーが cheeses で、関連した要素が空の文字列
     * であることを表す。
     *
     * cheeses <BR>
     * キーは、cheeses で、関連要素は空の文字列であることを指定している。
     *
     * @param inStream 入力ストリーム
     *
     * @else
     *
     * @brief Loads property list that consists of key:value from input stream
     *
     * Reads a property list (key and element pairs) from the input stream.
     * The stream is assumed to be using the ISO 8859-1 character encoding.
     * Each property is assumed to be registered in the input stream by each
     * line, and each line terminator is should be a line break characters
     * (\\n or \\r or \\r\\n).
     * Lines are read from the input stream until end of file is reached.
     *
     * A line that contains only white space characters or a line that its
     * first non-white space character is an ASCII '#' or '!' is ignored.
     * In a word, '#' or '!' represents comment lines.
     *
     * All lines except the blank line or comment line is described the property
     * that added to the table. However, if the line terminator is '\' and the
     * next line continues, it is treated as a continuation line (See below).
     * The key is composed of all characters.
     * All of these key termination characters in the line starting with the
     * first non-white space character and up to, but not including, the first
     * unescaped '=', ':', or white space character other than a line
     * terminator.
     *
     * Line terminator characters can be included using \ escape sequences.
     * Any white space after the key is skipped.
     * If the first non-white space character after the key is '=' or ':',
     * then it is ignored and any white space characters after it are also
     * skipped.
     * All remaining characters on the line become part of the associated element
     * string.
     * In element string, ASCII escape sequence such as \\t and \\n and \\r
     * and \\\\ and \\" and \\' and \\ (backslash character and space)
     * and \\uxxxx have affect and they will be converted into a single
     * character.
     * Also, if termination character in the line is \, the next line will be
     * treated as continuing. In that case, \ and break character will be
     * destroyed, and also its first space character will be destroyed,
     * so these characters on the line will not become part of the element
     * string.
     *
     * As an example, each of the following three lines specifies the key
     * "Truth" and the associated element value "Beauty":
     *
     * Truth = Beauty <BR>
     * Truth:Beauty <BR>
     * Truth\\t\\t\\t:Beauty <BR>
     *
     * As another example, the following three lines specify a single
     * property:
     *
     * fruits\\t\\t\\t\\tapple, banana, pear, \ <BR>
     *                                  cantaloupe, watermelon, \ <BR>
     *                                  kiwi, mango <BR>
     * The key is "fruits" and the associated element is:
     * "apple, banana, pear, cantaloupe, watermelon, kiwi, mango".
     * Note that a space appears before each \ so that a space will
     * each comma in the final result; the \, line terminator, and leading white
     * space on the continuation line are merely discarded and are not replaced
     * by one or more other characters.
     * As a third example, the line:
     *
     * cheeses <BR>
     * specifies that the key is "cheeses" and the associated element is the
     * empty string "".
     *
     * @param inStream the input stream.
     *
     * @endif
     */
    void load(std::istream& inStream);

    /*!
     * @if jp
     *
     * @brief プロパティリストを指定されたストリームに保存する
     *
     * 推奨されていません。プロパティリストの保存方法としては、
     * store(ostream out, string header) メソッドの使用が推奨される。
     * このメソッドは Java Properties との互換性のために定義されている。
     *
     * @param out 出力ストリーム
     * @param header プロパティリストの記述
     *
     * @else
     *
     * @brief Save the property list to the specified stream
     *
     * It is not recommended. To save the property list,
     * the store(ostream out, string header) method is recommended.
     * This method is defined for compatibility of Java Properties.
     *
     * @param out The output stream
     * @param header A description of the property list
     *
     * @endif
     */
    void save(std::ostream& out, const std::string& header);

    /*!
     * @if jp
     *
     * @brief プロパティリストを出力ストリームへ保存する
     *
     * Properties テーブル内のプロパティリスト (キーと要素のペア) を、load
     * メソッドを使って Properties テーブルにロードするのに適切なフォーマットで
     * 出力ストリームに書き込む。
     *
     * Properties テーブル内のプロパティリスト (キーと要素のペア) を、load
     * メソッドを使って Properties テーブルにロードするのに適切なフォーマットで
     * 出力ストリームに書き込む。ストリームは、ISO 8859-1 文字
     * エンコーディングを使用して書き込まれる。
     * Properties テーブル (存在する場合) のデフォルトテーブルからの
     * プロパティは、このメソッドによっては書き込まれない。
     *
     * header 引数が null でない場合は、ASCII 文字の #、header の文字列、
     * および行区切り文字が最初に出力ストリームに書き込まれます。このため、
     * header は識別コメントとして使うことができる。
     *
     * 次に、ASCII 文字の #、現在の日時 (Date の toString メソッドによって
     * 現在時刻が生成されるのと同様)、および Writer によって生成される行区切り
     * からなるコメント行が書き込まれる。
     *
     * 続いて、 Properties テーブル内のすべてのエントリが 1 行ずつ書き出される。
     * 各エントリのキー文字列、ASCII 文字の=、関連した要素文字列が書き込まれる。
     * 要素文字列の各文字は、エスケープシーケンスとして描画する必要があるか
     * どうか確認される。ASCII 文字の \、タブ、改行、および復帰はそれぞれ \\\\、
     * \\t、\\n、および \\r として書き込まれる。\\u0020 より小さい文字および
     * \\u007E より大きい文字は、対応する 16 進値 xxxx を使って \\uxxxx として
     * 書き込まれる。埋め込み空白文字でも後書き空白文字でもない先行空白文字は、
     * 前に \ を付けて書き込まれる。キーと値の文字 #、!、=、および : は、
     * 必ず正しくロードされるように、前にスラッシュを付けて書き込まれる。
     *
     * エントリが書き込まれたあとで、出力ストリームがフラッシュされる。
     * 出力ストリームはこのメソッドから復帰したあとも開いたままとなる。
     *
     * @param out 出力ストリーム
     * @param header プロパティリストの記述
     *
     * @else
     *
     * @brief Stores property list to the output stream
     *
     * Write this property list (key and element pairs) in this Properties
     * table to the output stream in a format suitable for loading into a
     * Properties table using the load method. The stream is written using the
     * ISO 8859-1 character encoding.
     * Properties from the defaults table of this Properties table (if any) are
     * not written out by this method.
     *
     * If the header argument is not null, then an ASCII # character, the
     * comments string, and a line separator are first written to the output
     * stream. Thus, the header can serve as an identifying comment.
     *
     * Next, a comment line is always written, consisting of an ASCII #
     * character, the current date and time (as if produced by the toString
     * method of Date for the current time), and a line separator as generated
     * by the Writer.
     *
     * Then every entry in this Properties table is written out, one per line.
     * For each entry the key string is written, then an ASCII =, then the
     * associated element string. Each character of the key and element strings
     * is examined to see whether it should be rendered as an escape sequence.
     * The ASCII characters \, tab, form feed, newline, and carriage return are
     * written as \\\\, \\t, \\f \\n, and \\r, respectively. Characters less than
     * \\u0020 and characters greater than \\u007E are written as \\uxxxx for the
     * appropriate hexadecimal value xxxx. For the key, all space characters are
     * written with a preceding \ character. For the element, leading space
     * characters, but not embedded or trailing space characters, are written
     * with a preceding \ character. The key and element characters #, !, =, and
     * : are written with a preceding backslash to ensure that they are properly
     * loaded.
     *
     * After the entries have been written, the output stream is flushed. The
     * output stream remains open after this method returns.
     *
     * @param out An output stream.
     * @param header The description of the property list.
     *
     * @endif
     */
    void store(std::ostream& out, const std::string& header);

    //============================================================
    // other util functions
    //============================================================
    /*!
     * @if jp
     *
     * @brief プロパティのキーのリストを vector で返す
     *
     * メインプロパティリストに同じ名前のキーが見つからない場合は、デフォルトの
     * プロパティリストにある個別のキーを含む、このプロパティリストにあるすべて
     * のキーのリストを返す。
     *
     * @return プロパティリストにあるすべてのキーのリスト。
     *         デフォルトのプロパティリストにあるキーを含む
     *
     * @else
     *
     * @brief Return an vector of all the keys in this property
     *
     * Returns an enumeration of all the keys in this property list, including
     * distinct keys in the default property list if a key of the same name has
     * not already been found from the main properties list.
     *
     * @return A vector of all the keys in this property list, including the
     *         keys in the default property list.
     *
     * @endif
     */
    std::vector<std::string> propertyNames() const;

    /*!
     * @if jp
     * @brief プロパティの数を取得する
     *
     * 設定済みのプロパティ数を取得する。
     *
     * @return プロパティ数
     *
     * @else
     * @brief Get the number of Properties
     *
     * Get the number of Properties that has already set.
     *
     * @return Number of Properties
     *
     * @endif
     */
    int size() const;

    /*!
     * @if jp
     * @brief ノードを取得する
     *
     * 指定したキーを持つノードを取得する。
     * 存在しないキー、および空文字の場合 0 を返す。
     *
     * @param key 取得対象ノードのキー
     *
     * @return 対象ノード
     *
     * @else
     * @brief Get node of properties
     *
     * Get node with specified key.
     *
     * @param key Target node key for getting
     *
     * @return Target node
     *
     * @endif
     */
    Properties* findNode(const std::string& key) const;
    /*!
     * @if jp
     * @brief ノードを取得する
     *
     * 指定したキーを持つノードを取得する。
     * 存在しないキー、および空文字の場合 0 を返す。
     *
     * @param key 取得対象ノードのキー
     *
     * @return 対象ノード
     *
     * @else
     * @brief Get node of properties
     *
     * Get node with specified key.
     *
     * @param key Target node key for getting
     *
     * @return Target node
     *
     * @endif
     */
    Properties& getNode(const std::string& key);

    /*!
     * @if jp
     * @brief 新規ノードを生成する
     *
     * 指定したキーを持つ新規ノードを生成する。
     * 既に同一キーを持つノードが登録済みの場合にはエラーを返す。
     *
     * @param key 新規ノードのキー
     *
     * @return 新規ノード生成結果
     *         指定したキーを持つノードが既に存在する場合にはfalse
     *
     * @else
     * @brief Create newly node of Properties
     *
     * Create nowly node with specified key.
     * If the node with the same key has been registered, error will be returned.
     *
     * @param key Newly node key
     *
     * @return Newly node creation result.
     *         false will be returned if the node with specified key has already
     *         existed.
     *
     * @endif
     */
    bool createNode(const std::string& key);

    /*!
     * @if jp
     * @brief ノードを削除する
     *
     * 指定した名称を持つプロパティを削除する。
     * 削除したプロパティを返す。
     *
     * @param leaf_name 削除対象プロパティ名称
     *
     * @return 削除したプロパティ
     *
     * @else
     * @brief Remove node of Properties
     *
     * Remove properties with specified name.
     * Properties that were deleted will be returned.
     *
     * @param leaf_name Target property's name for the delete
     *
     * @return Deleted properties
     *
     * @endif
     */
    Properties* removeNode(const char* leaf_name);

    /*!
     * @if jp
     * @brief 子ノードにkeyがあるかどうか
     *
     * 指定したキーを持つ子ノードが存在するかどうか確認する。
     * 存在する場合、子ノードを返す。
     *
     * @param key 確認対象のキー
     *
     * @return 子ノード
     *
     * @else
     * @brief Check whether key exists in the children
     *
     * Check whether the children with specified key exist.
     * If the children exist, they will be returned.
     *
     * @param key Check key
     *
     * @return Children node
     *
     * @endif
     */
    Properties* hasKey(const char* key) const;

    /*!
     * @if jp
     * @brief 子ノードを全て削除する
     * @else
     * @brief Clear the children
     * @endif
     */
    void clear();

    /*!
     * @if jp
     * @brief Propertyをマージする
     *
     * 現在のプロパティに設定したプロパティをマージする。
     *
     * @param prop マージするプロパティ
     *
     * @return プロパティマージ結果
     *
     * @else
     * @brief Merge properties
     *
     * Merge properties that have set to the current properties.
     *
     * @param prop Properties for the merge
     *
     * @return Property merge result
     *
     * @endif
     */
    Properties& operator<<(const Properties& prop);



    /*!
     * @if jp
     * @brief Propertyをstd::vector<std::string>への型変換演算子
     *
     *
     * @else
     * @brief
     *
     *
     * @endif
     */
    operator std::vector<std::string>() const;
    /*!
     * @if jp
     * @brief Propertyの内容を文字列に格納する
     *
     * @param out 文字列
     * @param curr 対象のプロパティ
     * @param index 現在のプロパティ階層
     *
     * @else
     * @brief
     *
     * @param out 
     * @param curr 
     * @param index 
     *
     * @endif
     */
    void _dump(std::string& out, const Properties& curr, int index) const;

    /*!
     * @if jp
     * @brief Propertyの内容を文字列のリストに格納する
     *
     * @param out 文字列のリスト
     * @param curr 対象のプロパティ
     * @param index 現在のプロパティ階層
     *
     * @else
     * @brief
     *
     * @param out
     * @param curr
     * @param index
     *
     * @endif
     */
    void _dump(std::vector<std::string>& out, const Properties& curr, int index) const;

  protected:
    /*!
     * @if jp
     * @brief 文字列をキーと値のペアに分割する
     *
     * 与えられた文字列を、設定されたデリミタでキーと値のペアに分割する。
     * まず最初に与えられた文字列に':'もしくは'='が含まれるかを検索し、
     * どちらかの文字が含まれている場合にはそれをデリミタとして使用する。
     * 両方とも含まれていない場合には、' '(スペース)を用いて分割を試みる。
     * 全てのデリミタ候補が含まれていない場合には、与えられた文字列をキーとして
     * 設定し、値に空の文字列を設定する。
     * どのデリミタ候補についてもエスケープされている(直前に'\'が設定されている)
     * 場合には、デリミタとして使用しない。
     *
     * @param str 分割対象文字列
     * @param key 分割結果キー
     * @param value 分割結果値
     *
     * @else
     * @brief Split the string into a pair of the key and the value.
     *
     * Split the given string into a pair of the key and the value with
     * the set delimiter.
     * First, search whether the fist given string includes ':' or '=', and
     * if either character is included, it is used as delimiter.
     * If neither is included, try to divide it with ' '(space).
     * When all delimiter candidates are not included, set the given string
     * as key then set empty string to the its value.
     * If any delimiter candidate is escaped ('\' is set right before it),
     * this method does not use as delimiter.
     *
     * @param str Target string for the division
     * @param key Division result key
     * @param value Division result value
     *
     * @endif
     */
    static void splitKeyValue(const std::string& str, std::string& key,
                              std::string& value);

    /*!
     * @if jp
     * @brief 文字列を分割する
     *
     * 与えられた文字列を、与えられたデリミタで分割する。
     * 与えられた文字列が空の場合は、エラーを返す。
     * 与えられたデリミタがエスケープされている(直前に'\'が設定されている)場合
     * には、デリミタとして使用しない。
     *
     * @param str 分割対象文字列
     * @param delim デリミタ
     * @param value 分割結果値リスト
     *
     * @return 分割処理結果
     *
     * @else
     * @brief Split the string
     *
     * Divide given string with given delimiter.
     * If the given string is empty, error will be returned.
     * When the given delimiter is escaped ('\' is set right before it)
     * this method does not use as delimiter.
     *
     * @param str Target string for the division
     * @param delim Delimiter
     * @param value Division result list
     *
     * @return Division result
     *
     * @endif
     */
    static bool split(const std::string& str, char delim,
                      std::vector<std::string>& value);

    /*!
     * @if jp
     * @brief プロパティを取得する
     *
     * キーリストで指定されたプロパティを取得する。
     * キーリストでは、指定するキーのプロパティでの階層関係をリスト形式で表現
     * する。
     * 指定したキーリストに該当するプロパティが存在しない場合はNULLを返す。
     *
     * @param keys 取得対象プロパティのキーのリスト表現
     * @param index キーリストの階層数
     * @param curr 検索対象プロパティ
     *
     * @return 検索対象プロパティ
     *
     * @else
     * @brief Get properties
     *
     * Get properties specified by key list.
     * In the key list, hierarchical relation in the properties is represented
     * by a list format.
     * If properties corresponding to specified key list do not exist,
     * null will be returned.
     *
     * @param keys Target properties's key list representation for getting
     * @param index Number of hierarchy of key list
     * @param curr Target properties for the search
     *
     * @return Target properties for the search
     *
     * @endif
     */
    static Properties* _getNode(std::vector<std::string>& keys,
                                std::vector<Properties*>::size_type index,
                                const Properties* curr);

    /*!
     * @if jp
     * @brief プロパティの名称リストを取得する
     *
     * プロパティの名称を'.'区切りで表現したリストを取得する。
     *
     * @param names プロパティの名称リスト
     * @param curr_name 現在のプロパティ名
     * @param curr 対象プロパティ
     *
     * @else
     * @brief Get property name list
     *
     * Get a list expressed by separating each property name with '.'.
     *
     * @param names Name list of property
     * @param curr_name Current property's name
     * @param curr Target properties
     *
     * @endif
     */
    static void _propertiyNames(std::vector<std::string>& names,
                                std::string curr_name,
                                const Properties* curr);

    /*!
     * @if jp
     * @brief プロパティの名称リストを保存する
     *
     * プロパティの名称を'.'区切りで表現したリストを保存する。
     *
     * @param out プロパティの名称リスト保存先の出力ストリーム
     * @param curr_name 現在のプロパティ名
     * @param curr 対象プロパティ
     *
     * @else
     * @brief Store the property name list
     *
     * Store a list expressed by separating each property name with '.'.
     *
     * @param out Output stream of property's name list of save destination.
     * @param curr_name Current property's name
     * @param curr Target properties
     *
     * @endif
     */
    static void _store(std::ostream& out, std::string curr_name,
                       Properties* curr);

    /*!
     * @if jp
     * @brief プロパティの内容を保存する
     *
     * プロパティに設定された内容を保存する。
     * 保存時にはプロパティ階層の深さを表す数字が付加される。
     * 値が設定されていないプロパティについては、デフォルト値が出力される。
     *
     * @param out プロパティ内容保存先の出力ストリーム
     * @param curr 対象プロパティ
     * @param index 現在のプロパティ階層
     *
     * @else
     * @brief Save property's contents
     *
     * Save the contents that were set to the property.
     * The figure represented the depth of the property hierarchy is
     * added when saving.
     * If property with the value that is not set, the default value will
     * be output.
     *
     * @param out Output stream of property's contents of save destination.
     * @param curr Target property
     * @param index Current property hierarchy
     *
     * @endif
     */
    static std::ostream& _dump(std::ostream& out, const Properties& curr,
                               int index);

    /*!
     * @if jp
     * @brief インデントを生成する
     *
     * 指定された数字に従って生成したインデントを返す。
     * 返されるインデントは、指定数字×2つの空白。
     *
     * @param index インデント数の指定
     *
     * @return 生成されたインデント
     *
     * @else
     * @brief Create indents
     *
     * Return indents according to specified figure.
     * Returned indents are specified figure x two blanks.
     *
     * @param index The specification of Number of indent
     *
     * @return Created indent
     *
     * @endif
     */
    static std::string indent(int index);

  private:
    std::string name;
    std::string value;
    std::string default_value;
    Properties* root;
    std::vector<Properties*> leaf;
    const std::string m_empty;

    /*!
     * @if jp
     * @brief Propertyをストリームに出力する
     *
     * Propertyをストリームに出力する。
     *
     * @param lhs 出力ストリーム
     * @param rhs プロパティ
     *
     * @return 出力ストリーム
     *
     * @else
     * @brief Output Properties to stream
     *
     * Output Properties to stream.
     *
     * @param lhs Output stream
     * @param rhs Properties
     *
     * @return Output stream
     *
     * @endif
     */
    friend std::ostream& operator<<(std::ostream& lhs, const Properties& rhs);

  };  // class Properties
} // namespace coil
#endif  // COIL_PROPERTIES_H

