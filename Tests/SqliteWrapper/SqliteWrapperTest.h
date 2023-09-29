#pragma once

#include "BaseTestTemplate.h"
#include "SqliteWrapper.h"


static MunitResult sqlLiteParameterTest(const MunitParameter params[], void *data) {
    str_DbValueMap *paramMap = NEW_SQL_PARAM_MAP(16);
    str_DbValueMapAdd(paramMap, "param_1", DB_VALUE("value"));
    str_DbValueMapAdd(paramMap, "param_2", DB_VALUE(1234));
    str_DbValueMapAdd(paramMap, "param_3", DB_VALUE(1.234));
    assert_uint32(3, ==, str_DbValueMapSize(paramMap));


    str_DbValueMap *map_1 = SQL_PARAM_MAP("k1", "text value");
    assert_uint32(1, ==, str_DbValueMapSize(map_1));

    str_DbValueMap *map_2 = SQL_PARAM_MAP("k1", "text value", "k2", 123);
    assert_uint32(2, ==, str_DbValueMapSize(map_2));

    str_DbValueMap *map_3 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234);
    assert_uint32(3, ==, str_DbValueMapSize(map_3));

    str_DbValueMap *map_4 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5");
    assert_uint32(4, ==, str_DbValueMapSize(map_4));

    str_DbValueMap *map_5 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765);
    assert_uint32(5, ==, str_DbValueMapSize(map_5));

    str_DbValueMap *map_6 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234);
    assert_uint32(6, ==, str_DbValueMapSize(map_6));

    str_DbValueMap *map_7 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234, "k7", "v7");
    assert_uint32(7, ==, str_DbValueMapSize(map_7));

    str_DbValueMap *map_8 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234, "k7", "v7", "k8", 1234);
    assert_uint32(8, ==, str_DbValueMapSize(map_8));

    str_DbValueMap *map_9 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234, "k7", "v7", "k8", 1234, "k9", "v9");
    assert_uint32(9, ==, str_DbValueMapSize(map_9));

    str_DbValueMap *map_10 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234, "k7", "v7", "k8", 1234, "k9", "v9", "k10", 678233);
    assert_uint32(10, ==, str_DbValueMapSize(map_10));

    str_DbValueMap *map_11 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234, "k7", "v7", "k8", 1234, "k9", "v9", "k10", 678233, "k11", 1.2345);
    assert_uint32(11, ==, str_DbValueMapSize(map_11));

    str_DbValueMap *map_12 = SQL_PARAM_MAP("k1", "text value", "k2", 123, "k3", 1.234,  "k4", "v5", "k5", 765, "k6", 21.234, "k7", "v7", "k8", 1234, "k9", "v9", "k10", 678233, "k11", 1.2345, "k12", "v12");
    assert_uint32(12, ==, str_DbValueMapSize(map_12));

    return MUNIT_OK;
}

static MunitResult sqlLiteQueryStringTest(const MunitParameter params[], void *data) {
    QueryString *str_1 = newQueryString();
    assert_not_null(str_1);
    assert_not_null(str_1->value);
    assert_uint32(0, ==, str_1->size);
    assert_uint32(DEFAULT_SQLITE_QUERY_STRING_SIZE, ==, str_1->capacity);

    QueryString *str_2 = newQueryStringWithSize(32);
    assert_not_null(str_2);
    assert_not_null(str_2->value);
    assert_uint32(0, ==, str_2->size);
    assert_uint32(32, ==, str_2->capacity);

    QueryString *str_3 = newQueryStringWithSize(8);
    queryStringAppend(str_3, "test", 4);
    queryStringAppendChar(str_3, ' ');
    queryStringAppend(str_3, "foo", 3);
    assert_not_null(str_3);
    assert_string_equal("test foo", str_3->value);

    // test for overflow
    queryStringAppendChar(str_3, ' ');
    queryStringAppend(str_3, "baz", 3);
    assert_string_equal("test foo baz", queryStringGetValue(str_3));
    assert_uint32(16 + 1, ==, str_3->capacity);

    // format
    QueryString *str_4 = queryStringOf("Parameter test: %s, %d, %.3f", "one", 2, 3.333);
    assert_string_equal("Parameter test: one, 2, 3.333", queryStringGetValue(str_4));

    // named params
    str_DbValueMap *queryParams = SQL_PARAM_MAP("default", "one", "camelCaseParam", 2, "snake_case_param", 3, "hyphen-param", "four");
    QueryString *str_5 = namedQueryString("Params -> :default, :camelCaseParam, :snake_case_param, :hyphen-param, :not_exist -> End", queryParams);
    assert_string_equal("Params -> 'one', 2, 3, 'four', NULL -> End", str_5->value);

    deleteQueryString(str_1);
    deleteQueryString(str_2);
    deleteQueryString(str_3);
    deleteQueryString(str_4);
    deleteQueryString(str_5);

    return MUNIT_OK;
}

static MunitResult sqlLiteTableMetadataTest(const MunitParameter params[], void *data) {
    sqlite3 *db = sqliteDbInit("../resources/test.db");
    assert_not_null(db);

    int rc = executeUpdate(db, "CREATE TABLE IF NOT EXISTS test"
                               "("
                               "id    INTEGER PRIMARY KEY,"
                               "value INTEGER,"
                               "data  TEXT NOT NULL,"
                               "param DOUBLE"
                               ")", NULL);
    assert_int(0, ==, rc);

    Vector nameVec = getDbTableColumnNames(db, "test");
    assert_string_equal("id", vectorGet(nameVec, 0));
    assert_string_equal("value", vectorGet(nameVec, 1));
    assert_string_equal("data", vectorGet(nameVec, 2));
    assert_string_equal("param", vectorGet(nameVec, 3));
    deleteDbColumnNameVector(nameVec);

    assert_true(idDbColumnExists(db, "test", "id"));
    assert_true(idDbColumnExists(db, "test", "value"));
    assert_true(idDbColumnExists(db, "test", "data"));
    assert_true(idDbColumnExists(db, "test", "param"));

    assert_false(idDbColumnExists(db, "test", "not_exist"));
    assert_false(idDbColumnExists(db, "test", "error"));

    rc = executeUpdate(db, "DROP TABLE test", NULL);
    assert_int(0, ==, rc);

    sqliteDbClose(db);
    return MUNIT_OK;
}

static MunitResult sqlLiteFullTest(const MunitParameter params[], void *data) {
    sqlite3 *db = sqliteDbInit("../resources/test.db");
    assert_not_null(db);

   int rc = executeUpdate(db, "CREATE TABLE IF NOT EXISTS test"
                      "("
                      "    id    INTEGER PRIMARY KEY,"
                      "    value INTEGER,"
                      "    data  TEXT NOT NULL,"
                      "    param DOUBLE"
                      ")", NULL);

    assert_int(0, ==, rc);

    rc = executeUpdate(db, "INSERT INTO test VALUES (NULL, :int_val, :data_text, :decimal_param)",
                       SQL_PARAM_MAP("int_val", 2, "data_text", "test", "decimal_param", 1.2345));
    assert_int(0, ==, rc);

    rc = executeUpdate(db, "INSERT INTO test VALUES (NULL, :int_val, :data_text, :decimal_param)",
                       SQL_PARAM_MAP("int_val", 3, "data_text", "some other", "decimal_param", 54.345));
    assert_int(0, ==, rc);

    char *queryStr = "SELECT * FROM test WHERE value = :int_val";
    str_DbValueMap *paramMap = SQL_PARAM_MAP("int_val", 2);
    ResultSet *rs = executeQuery(db, queryStr, paramMap);
    assert_not_null(rs);

    nextResultSet(rs);
    int id = rsGetInt(rs, "id");
    int value = rsGetInt(rs, "value");
    double decVal = rsGetDouble(rs, "param");
    const char *dataStr = rsGetString(rs, "data");

    assert_int(1, ==, id);
    assert_int(2, ==, value);
    assert_double_equal(1.2345, decVal, 4);
    assert_string_equal("test", dataStr);

    assert_false(nextResultSet(rs));    // should also finalize work

    rc = executeUpdate(db, "DROP TABLE test", NULL);
    assert_int(0, ==, rc);

    resultSetDelete(rs);
    sqliteDbClose(db);

    return MUNIT_OK;
}

static MunitTest sqlWrapperTests[] = {
        {.name =  "Param map test - should correctly create params and map to db values", .test = sqlLiteParameterTest},
        {.name =  "Query string test - should correctly create and format query string", .test = sqlLiteQueryStringTest},
        {.name =  "Metadata test - should correctly return db table column data", .test = sqlLiteTableMetadataTest},
        {.name =  "Full test - should correctly execute queries and get results", .test = sqlLiteFullTest},
        END_OF_TESTS
};

static const MunitSuite sqliteWrapperTestSuite = {
        .prefix = "SqlWrapper: ",
        .tests = sqlWrapperTests,
        .suites = NULL,
        .iterations = 1,
        .options = MUNIT_SUITE_OPTION_NONE
};