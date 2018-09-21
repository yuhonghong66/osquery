/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under both the Apache 2.0 license (found in the
 *  LICENSE file in the root directory of this source tree) and the GPLv2 (found
 *  in the COPYING file in the root directory of this source tree).
 *  You may select, at your option, one of the above-listed licenses.
 */

#include <osquery/database.h>

#include <osquery/core/sql/diff_results.h>
#include <osquery/core/sql/query_data.h>
#include <osquery/query.h>

#include <gtest/gtest.h>

#include <string>

namespace osquery {

namespace {

ColumnNames getSerializedRowColumnNames(bool unordered_and_repeated) {
  ColumnNames cn;
  if (unordered_and_repeated) {
    cn.push_back("repeated_column");
  }
  cn.push_back("alphabetical");
  cn.push_back("foo");
  cn.push_back("meaning_of_life");
  cn.push_back("repeated_column");
  return cn;
}

std::pair<JSON, Row> getSerializedRow(bool unordered_and_repeated = false) {
  auto cns = getSerializedRowColumnNames(unordered_and_repeated);

  Row r;
  auto doc = JSON::newObject();
  for (const auto& cn : cns) {
    auto c_value = cn + "_value";
    r[cn] = c_value;
    doc.addCopy(cn, c_value);
  }
  return std::make_pair(std::move(doc), r);
}

std::pair<JSON, QueryData> getSerializedQueryData() {
  auto r = getSerializedRow(false);
  QueryData q = {r.second, r.second};

  JSON doc = JSON::newArray();
  auto arr1 = doc.getArray();
  doc.copyFrom(r.first.doc(), arr1);
  doc.push(arr1);

  auto arr2 = doc.getArray();
  doc.copyFrom(r.first.doc(), arr2);
  doc.push(arr2);

  return std::make_pair(std::move(doc), q);
}

std::pair<std::string, QueryData> getSerializedQueryDataJSON() {
  auto results = getSerializedQueryData();
  std::string output;
  results.first.toString(output);
  return std::make_pair(output, results.second);
}

std::pair<JSON, DiffResults> getSerializedDiffResults() {
  auto qd = getSerializedQueryData();
  DiffResults diff_results;
  diff_results.added = qd.second;
  diff_results.removed = qd.second;

  JSON doc = JSON::newObject();
  doc.add("removed", qd.first.doc());
  doc.add("added", qd.first.doc());

  return std::make_pair(std::move(doc), std::move(diff_results));
}

std::pair<JSON, QueryLogItem> getSerializedQueryLogItem() {
  std::pair<JSON, QueryLogItem> p;
  QueryLogItem i;
  JSON doc = JSON::newObject();
  auto dr = getSerializedDiffResults();
  i.results = std::move(dr.second);
  i.name = "foobar";
  i.calendar_time = "Mon Aug 25 12:10:57 2014";
  i.time = 1408993857;
  i.identifier = "foobaz";
  i.epoch = 0L;
  i.counter = 0L;

  auto diff_doc = doc.getObject();
  diff_doc.Swap(dr.first.doc());
  doc.add("diffResults", diff_doc);
  doc.addRef("name", "foobar");
  doc.addRef("hostIdentifier", "foobaz");
  doc.addRef("calendarTime", "Mon Aug 25 12:10:57 2014");
  doc.add("unixTime", 1408993857);
  doc.add("epoch", std::size_t{0});
  doc.add("counter", std::size_t{0});

  return std::make_pair(std::move(doc), std::move(i));
}

std::pair<JSON, QueryData> getSerializedQueryDataWithColumnOrder() {
  auto r = getSerializedRow(true);
  QueryData q = {r.second, r.second};
  JSON doc = JSON::newArray();
  auto arr1 = doc.getArray();
  doc.copyFrom(r.first.doc(), arr1);
  doc.push(arr1);

  auto arr2 = doc.getArray();
  doc.copyFrom(r.first.doc(), arr2);
  doc.push(arr2);

  return std::make_pair(std::move(doc), q);
}

std::pair<std::string, DiffResults> getSerializedDiffResultsJSON() {
  auto results = getSerializedDiffResults();
  std::string output;
  results.first.toString(output);
  return std::make_pair(output, std::move(results.second));
}

std::pair<std::string, QueryLogItem> getSerializedQueryLogItemJSON() {
  auto results = getSerializedQueryLogItem();
  std::string output;
  results.first.toString(output);
  return std::make_pair(output, std::move(results.second));
}

} // namespace

class ResultsTests : public testing::Test {};

TEST_F(ResultsTests, test_simple_diff) {
  QueryDataSet os;
  QueryData o;
  QueryData n;

  Row r1;
  r1["foo"] = "bar";
  n.push_back(r1);

  auto results = diff(os, n);
  EXPECT_EQ(results.added, n);
  EXPECT_EQ(results.removed, o);
}

TEST_F(ResultsTests, test_serialize_row) {
  auto results = getSerializedRow();
  auto doc = JSON::newObject();
  auto s = serializeRow(results.second, {}, doc, doc.doc());
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(doc.doc()["meaning_of_life"], "meaning_of_life_value");
  EXPECT_EQ(doc.doc()["alphabetical"], "alphabetical_value");
}

TEST_F(ResultsTests, test_deserialize_row_json) {
  auto results = getSerializedRow();
  std::string input;
  serializeRowJSON(results.second, input);

  // Pull the serialized JSON back into a Row output container.
  Row output;
  auto s = deserializeRowJSON(input, output);
  EXPECT_TRUE(s.ok());
  // The output container should match the input row.
  EXPECT_EQ(output, results.second);
}

TEST_F(ResultsTests, test_serialize_query_data) {
  auto results = getSerializedQueryData();
  auto doc = JSON::newArray();
  auto s = serializeQueryData(results.second, {}, doc, doc.doc());
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first.doc(), doc.doc());
}

TEST_F(ResultsTests, test_serialize_query_data_in_column_order) {
  auto results = getSerializedQueryDataWithColumnOrder();
  auto column_names = getSerializedRowColumnNames(true);
  auto doc = JSON::newArray();
  auto s = serializeQueryData(results.second, column_names, doc, doc.doc());
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first.doc(), doc.doc());
}

TEST_F(ResultsTests, test_serialize_query_data_json) {
  auto results = getSerializedQueryDataJSON();
  std::string json;
  auto s = serializeQueryDataJSON(results.second, json);
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first, json);
}

TEST_F(ResultsTests, test_deserialize_query_data_json) {
  auto results = getSerializedQueryDataJSON();

  // Pull the serialized JSON back into a QueryData output container.
  QueryData output;
  auto s = deserializeQueryDataJSON(results.first, output);
  EXPECT_TRUE(s.ok());
  // The output container should match the input query data.
  EXPECT_EQ(output, results.second);
}

TEST_F(ResultsTests, test_serialize_diff_results) {
  auto results = getSerializedDiffResults();
  auto doc = JSON::newObject();
  auto s = serializeDiffResults(results.second, {}, doc, doc.doc());
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first.doc(), doc.doc());
}

TEST_F(ResultsTests, test_serialize_diff_results_json) {
  auto results = getSerializedDiffResultsJSON();
  std::string json;
  auto s = serializeDiffResultsJSON(results.second, json);
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first, json);
}

TEST_F(ResultsTests, test_serialize_query_log_item) {
  auto results = getSerializedQueryLogItem();
  auto doc = JSON::newObject();
  auto s = serializeQueryLogItem(results.second, doc);
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first.doc(), doc.doc());
}

TEST_F(ResultsTests, test_serialize_query_log_item_json) {
  auto results = getSerializedQueryLogItemJSON();
  std::string json;
  auto s = serializeQueryLogItemJSON(results.second, json);
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(s.toString(), "OK");
  EXPECT_EQ(results.first, json);
}

TEST_F(ResultsTests, test_deserialize_query_log_item_json) {
  auto results = getSerializedQueryLogItemJSON();

  // Pull the serialized JSON back into a QueryLogItem output container.
  QueryLogItem output;
  auto s = deserializeQueryLogItemJSON(results.first, output);
  EXPECT_TRUE(s.ok());
  // The output container should match the input query data.
  EXPECT_EQ(output, results.second);
}

TEST_F(ResultsTests, test_adding_duplicate_rows_to_query_data) {
  Row r1, r2, r3;
  r1["foo"] = "bar";
  r1["baz"] = "boo";

  r2["foo"] = "baz";
  r2["baz"] = "bop";

  r3["foo"] = "baz";
  r3["baz"] = "bop";

  QueryData q;
  bool s;

  s = addUniqueRowToQueryData(q, r1);
  EXPECT_TRUE(s);
  EXPECT_EQ(q.size(), 1U);

  s = addUniqueRowToQueryData(q, r2);
  EXPECT_TRUE(s);
  EXPECT_EQ(q.size(), 2U);

  s = addUniqueRowToQueryData(q, r3);
  EXPECT_FALSE(s);
  EXPECT_EQ(q.size(), 2U);
}
}
