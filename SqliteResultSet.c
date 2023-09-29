#include "SqliteResultSet.h"

#define NO_VALUE_INDEX (-1)

struct ResultSet {
    sqlite3 *db;    // sqlite3* db is used to print errmsg
    sqlite3_stmt *stmt;
    HashMap columnMap;
};

static ResultSet *mapColumnNames(ResultSet *resultSet);
static inline int getIndexByColumnName(ResultSet *resultSet, const char *columnName);
static const char *resultSetGetColumnName(ResultSet *resultSet, int column);
static int resultSetColumnCount(ResultSet *resultSet);


ResultSet *newSqliteResultSet(sqlite3 *db, sqlite3_stmt *stmt) {
    ResultSet *resultSet = malloc(sizeof(struct ResultSet));
    if (resultSet == NULL) return NULL;
    resultSet->stmt = stmt;
    resultSet->db = db;
    return mapColumnNames(resultSet);
}

bool nextResultSet(ResultSet *resultSet) {
    if (resultSet == NULL) return false;

    int rc = sqlite3_step(resultSet->stmt);
    if (rc == SQLITE_ROW) {
        return true;
    }

    sqlite3_finalize(resultSet->stmt);
    return false;
}

int rsGetInt(ResultSet *resultSet, const char *columnName) {
    return sqlite3_column_int(resultSet->stmt, getIndexByColumnName(resultSet, columnName));
}

int64_t rsGetI64(ResultSet *resultSet, const char *columnName) {
    return sqlite3_column_int64(resultSet->stmt, getIndexByColumnName(resultSet, columnName));
}

const char *rsGetString(ResultSet *resultSet, const char *columnName) {
    return (const char *) sqlite3_column_text(resultSet->stmt, getIndexByColumnName(resultSet, columnName));
}

double rsGetDouble(ResultSet *resultSet, const char *columnName) {
    return sqlite3_column_double(resultSet->stmt, getIndexByColumnName(resultSet, columnName));
}

int rsGetIntByIndex(ResultSet *resultSet, int columnIndex) {
    return sqlite3_column_int(resultSet->stmt, columnIndex);
}

int64_t rsGetI64ByIndex(ResultSet *resultSet, int columnIndex) {
    return sqlite3_column_int64(resultSet->stmt, columnIndex);
}

const char *rsGetStringByIndex(ResultSet *resultSet, int columnIndex) {
    return (const char *) sqlite3_column_text(resultSet->stmt, columnIndex);
}

double rsGetDoubleByIndex(ResultSet *resultSet, int columnIndex) {
    return sqlite3_column_double(resultSet->stmt, columnIndex);
}

DbValueType resultSetGetColumnType(ResultSet *resultSet, const char *columnName) {
    int columnIndex = (int) hashMapGet(resultSet->columnMap, columnName);
    int columnType = sqlite3_column_type(resultSet->stmt, columnIndex);
    switch (columnType) {
        case SQLITE_INTEGER:
            return DB_VALUE_INT;
        case SQLITE_TEXT:
            return DB_VALUE_TEXT;
        case SQLITE_FLOAT:
            return DB_VALUE_DOUBLE;
        case SQLITE_BLOB:
            return DB_VALUE_BLOB;
        default:
            return DB_VALUE_NULL;
    }
}

void resultSetDelete(ResultSet *resultSet) {
    if (resultSet != NULL) {
        hashMapDelete(resultSet->columnMap);
        free(resultSet);
    }
}

static ResultSet *mapColumnNames(ResultSet *resultSet) {
    HashMap columnMap = getHashMapInstance(resultSetColumnCount(resultSet) * 2);
    for (int i = 0; i < resultSetColumnCount(resultSet); i++) {
        const char *name = resultSetGetColumnName(resultSet, i);
        hashMapPut(columnMap, name, (MapValueType) i);
    }
    
    resultSet->columnMap = columnMap;
    return resultSet;
}

static inline int getIndexByColumnName(ResultSet *resultSet, const char *columnName) {
    MapEntry *entry = hashMapGetEntry(resultSet->columnMap, columnName);
    return entry != NULL ? (long) entry->value : NO_VALUE_INDEX;
}

static const char *resultSetGetColumnName(ResultSet *resultSet, int column) {
    return sqlite3_column_name(resultSet->stmt, column);
}

static int resultSetColumnCount(ResultSet *resultSet) {
    return sqlite3_column_count(resultSet->stmt);
}
