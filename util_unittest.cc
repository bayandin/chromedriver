// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/base64.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "chrome/test/chromedriver/chrome/status.h"
#include "chrome/test/chromedriver/util.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(UnzipSoleFile, Entry) {
  base::ScopedTempDir temp_dir;
  ASSERT_TRUE(temp_dir.CreateUniqueTempDir());
  std::string data;
  // A zip entry sent from a Java WebDriver client (v2.20) that contains a
  // file with the contents "COW\n".
  const char kBase64ZipEntry[] =
      "UEsDBBQACAAIAJpyXEAAAAAAAAAAAAAAAAAEAAAAdGVzdHP2D+"
      "cCAFBLBwi/wAzGBgAAAAQAAAA=";
  ASSERT_TRUE(base::Base64Decode(kBase64ZipEntry, &data));
  base::FilePath file;
  Status status = UnzipSoleFile(temp_dir.GetPath(), data, &file);
  ASSERT_EQ(kOk, status.code()) << status.message();
  std::string contents;
  ASSERT_TRUE(base::ReadFileToString(file, &contents));
  ASSERT_STREQ("COW\n", contents.c_str());
}

TEST(UnzipSoleFile, Archive) {
  base::ScopedTempDir temp_dir;
  ASSERT_TRUE(temp_dir.CreateUniqueTempDir());
  std::string data;
  // A zip archive sent from a Python WebDriver client that contains a
  // file with the contents "COW\n".
  const char kBase64ZipArchive[] =
      "UEsDBBQAAAAAAMROi0K/wAzGBAAAAAQAAAADAAAAbW9vQ09XClBLAQIUAxQAAAAAAMROi0K/"
      "wAzGBAAAAAQAAAADAAAAAAAAAAAAAACggQAAAABtb29QSwUGAAAAAAEAAQAxAAAAJQAAAAA"
      "A";
  ASSERT_TRUE(base::Base64Decode(kBase64ZipArchive, &data));
  base::FilePath file;
  Status status = UnzipSoleFile(temp_dir.GetPath(), data, &file);
  ASSERT_EQ(kOk, status.code()) << status.message();
  std::string contents;
  ASSERT_TRUE(base::ReadFileToString(file, &contents));
  ASSERT_STREQ("COW\n", contents.c_str());
}

namespace {

const base::StringPiece key = "key";
const int64_t max_safe_int = (1ll << 53) - 1;

void DictNoInit(base::DictionaryValue* dict) {}

void DictInitNull(base::DictionaryValue* dict) {
  dict->Set(key, std::make_unique<base::Value>());
}

class DictInitBool {
  bool init_value;

 public:
  explicit DictInitBool(bool v) : init_value(v) {}
  void operator()(base::DictionaryValue* dict) {
    dict->SetBoolean(key, init_value);
  }
};

class DictInitInt {
  int init_value;

 public:
  explicit DictInitInt(int v) : init_value(v) {}
  void operator()(base::DictionaryValue* dict) {
    dict->SetInteger(key, init_value);
  }
};

class DictInitDouble {
  double init_value;

 public:
  explicit DictInitDouble(double v) : init_value(v) {}
  void operator()(base::DictionaryValue* dict) {
    dict->SetDoubleKey(key, init_value);
  }
};

class DictInitString {
  std::string init_value;

 public:
  explicit DictInitString(const std::string& v) : init_value(v) {}
  void operator()(base::DictionaryValue* dict) {
    dict->SetString(key, init_value);
  }
};

template <typename ResultType, typename DictInitFunc>
void TestGetOptionalValue(bool (*func_to_test)(const base::DictionaryValue*,
                                               base::StringPiece,
                                               ResultType*,
                                               bool*),
                          DictInitFunc dict_init_func,
                          const ResultType& init_result_value,
                          const ResultType& expected_result_value,
                          bool expected_return_value,
                          bool expected_has_value) {
  base::DictionaryValue dict;
  dict_init_func(&dict);

  ResultType result_value = init_result_value;
  bool has_value;
  bool return_value = func_to_test(&dict, key, &result_value, &has_value);
  ASSERT_EQ(return_value, expected_return_value);
  ASSERT_EQ(result_value, expected_result_value);
  if (return_value)
    ASSERT_EQ(has_value, expected_has_value);

  result_value = init_result_value;
  return_value = func_to_test(&dict, key, &result_value, nullptr);
  ASSERT_EQ(return_value, expected_return_value);
  ASSERT_EQ(result_value, expected_result_value);
}

}  // namespace

TEST(GetOptionalValue, BoolNone) {
  TestGetOptionalValue<bool>(GetOptionalBool, DictNoInit, true, true, true,
                             false);
}

TEST(GetOptionalValue, IntNone) {
  TestGetOptionalValue<int>(GetOptionalInt, DictNoInit, 12345, 12345, true,
                            false);
}

TEST(GetOptionalValue, DoubleNone) {
  TestGetOptionalValue<double>(GetOptionalDouble, DictNoInit, 123.45, 123.45,
                               true, false);
}

TEST(GetOptionalValue, StringNone) {
  TestGetOptionalValue<std::string>(GetOptionalString, DictNoInit, "abcde",
                                    "abcde", true, false);
}

TEST(GetOptionalValue, DictionaryNone) {
  base::DictionaryValue dv;
  const base::DictionaryValue* tmp = &dv;
  TestGetOptionalValue<const base::DictionaryValue*>(
      GetOptionalDictionary, DictNoInit, tmp, tmp, true, false);
}

TEST(GetOptionalValue, ListNone) {
  base::ListValue lv;
  const base::ListValue* tmp = &lv;
  TestGetOptionalValue<const base::ListValue*>(GetOptionalList, DictNoInit, tmp,
                                               tmp, true, false);
}

TEST(GetOptionalValue, SafeIntNone) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt, DictNoInit, 12345, 12345,
                                true, false);
}

TEST(GetOptionalValue, BoolNull) {
  TestGetOptionalValue<bool>(GetOptionalBool, DictInitNull, true, true, false,
                             false);
}

TEST(GetOptionalValue, IntNull) {
  TestGetOptionalValue<int>(GetOptionalInt, DictInitNull, 12345, 12345, false,
                            false);
}

TEST(GetOptionalValue, DoubleNull) {
  TestGetOptionalValue<double>(GetOptionalDouble, DictInitNull, 123.45, 123.45,
                               false, false);
}

TEST(GetOptionalValue, StringNull) {
  TestGetOptionalValue<std::string>(GetOptionalString, DictInitNull, "abcde",
                                    "abcde", false, false);
}

TEST(GetOptionalValue, DictionaryNull) {
  base::DictionaryValue dv;
  const base::DictionaryValue* tmp = &dv;
  TestGetOptionalValue<const base::DictionaryValue*>(
      GetOptionalDictionary, DictInitNull, tmp, tmp, false, false);
}

TEST(GetOptionalValue, ListNull) {
  base::ListValue lv;
  const base::ListValue* tmp = &lv;
  TestGetOptionalValue<const base::ListValue*>(GetOptionalList, DictInitNull,
                                               tmp, tmp, false, false);
}

TEST(GetOptionalValue, SafeIntNull) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt, DictInitNull, 12345, 12345,
                                false, false);
}

TEST(GetOptionalValue, BoolWrongType) {
  TestGetOptionalValue<bool>(GetOptionalBool, DictInitString("test"), true,
                             true, false, false);
}

TEST(GetOptionalValue, IntWrongType) {
  TestGetOptionalValue<int>(GetOptionalInt, DictInitString("test"), 12345,
                            12345, false, false);
}

TEST(GetOptionalValue, DoubleWrongType) {
  TestGetOptionalValue<double>(GetOptionalDouble, DictInitString("test"),
                               123.45, 123.45, false, false);
}

TEST(GetOptionalValue, StringWrongType) {
  TestGetOptionalValue<std::string>(GetOptionalString, DictInitBool(false),
                                    "abcde", "abcde", false, false);
}

TEST(GetOptionalValue, DictionaryWrongType) {
  base::DictionaryValue dv;
  const base::DictionaryValue* tmp = &dv;
  TestGetOptionalValue<const base::DictionaryValue*>(
      GetOptionalDictionary, DictInitString("test"), tmp, tmp, false, false);
}

TEST(GetOptionalValue, ListWrongType) {
  base::ListValue lv;
  const base::ListValue* tmp = &lv;
  TestGetOptionalValue<const base::ListValue*>(
      GetOptionalList, DictInitString("test"), tmp, tmp, false, false);
}

TEST(GetOptionalValue, SafeIntWrongType) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt, DictInitString("test"),
                                12345, 12345, false, false);
}

TEST(GetOptionalValue, BoolNoConversion) {
  TestGetOptionalValue<bool>(GetOptionalBool, DictInitBool(false), true, false,
                             true, true);
}

TEST(GetOptionalValue, IntNoConversion) {
  TestGetOptionalValue<int>(GetOptionalInt, DictInitInt(54321), 12345, 54321,
                            true, true);
}

TEST(GetOptionalValue, DoubleNoConversion) {
  TestGetOptionalValue<double>(GetOptionalDouble, DictInitDouble(54.321),
                               123.45, 54.321, true, true);
}

TEST(GetOptionalValue, StringNoConversion) {
  TestGetOptionalValue<std::string>(GetOptionalString, DictInitString("xyz"),
                                    "abcde", "xyz", true, true);
}

TEST(GetOptionalValue, DictionaryNoConversion) {
  base::DictionaryValue dv1;
  dv1.SetString("dv", "1");
  base::DictionaryValue dv2;
  dv2.SetString("dv", "2");

  std::unique_ptr<base::DictionaryValue> params(dv1.DeepCopy());

  base::DictionaryValue dict;
  dict.SetDictionary(key, std::move(params));
  const base::DictionaryValue* res = &dv2;
  bool has_value;
  bool has_dict = GetOptionalDictionary(&dict, key, &res, &has_value);
  ASSERT_EQ(has_value, true);
  ASSERT_EQ(has_dict, true);
  // Cast to base class to ensure print properly if different
  ASSERT_EQ(static_cast<const base::Value&>(*res),
            static_cast<const base::Value&>(dv1));
}

TEST(GetOptionalValue, ListNoConversion) {
  base::ListValue lv1;
  lv1.AppendString("1");
  base::ListValue lv2;
  lv2.AppendString("2");

  base::Value params = lv1.Clone();

  base::DictionaryValue dict;
  dict.SetPath(key, std::move(params));
  const base::ListValue* res = &lv2;
  bool has_value;
  bool has_dict = GetOptionalList(&dict, key, &res, &has_value);
  ASSERT_EQ(has_value, true);
  ASSERT_EQ(has_dict, true);
  // Cast to base class to ensure print properly if different
  ASSERT_EQ(static_cast<const base::Value&>(*res),
            static_cast<const base::Value&>(lv1));
}

TEST(GetOptionalValue, SafeIntNoConversion) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt, DictInitInt(54321), 12345,
                                54321, true, true);
}

TEST(GetOptionalValue, DoubleToInt) {
  TestGetOptionalValue<int>(GetOptionalInt, DictInitDouble(54321.0), 12345,
                            54321, true, true);
}

TEST(GetOptionalValue, DoubleToSafeInt) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt, DictInitDouble(54321.0),
                                12345, 54321, true, true);
}

TEST(GetOptionalValue, DoubleToIntError) {
  TestGetOptionalValue<int>(GetOptionalInt, DictInitDouble(5432.1), 12345,
                            12345, false, false);
}

TEST(GetOptionalValue, DoubleToSafeIntError) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt, DictInitDouble(5432.1),
                                12345, 12345, false, false);
}

TEST(GetOptionalValue, IntToDouble) {
  TestGetOptionalValue<double>(GetOptionalDouble, DictInitInt(54), 123.45, 54.0,
                               true, true);
}

TEST(GetOptionalValue, SafeIntMax) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt,
                                DictInitDouble(max_safe_int), 12345,
                                max_safe_int, true, true);
}

TEST(GetOptionalValue, SafeIntMaxToIntError) {
  TestGetOptionalValue<int>(GetOptionalInt, DictInitDouble(max_safe_int), 12345,
                            12345, false, false);
}

TEST(GetOptionalValue, SafeIntTooLarge) {
  TestGetOptionalValue<int64_t>(GetOptionalSafeInt,
                                DictInitDouble(max_safe_int + 1), 12345, 12345,
                                false, false);
}

TEST(ConvertCentimeterToInch, Zero) {
  ASSERT_EQ(0, ConvertCentimeterToInch(0));
}

TEST(ConvertCentimeterToInch, PositiveDouble) {
  ASSERT_EQ(1, ConvertCentimeterToInch(2.54));
  ASSERT_EQ(0.1, ConvertCentimeterToInch(0.254));
}

TEST(ConvertCentimeterToInch, NegativeDouble) {
  ASSERT_EQ(-1, ConvertCentimeterToInch(-2.54));
  ASSERT_EQ(-0.1, ConvertCentimeterToInch(-0.254));
}
