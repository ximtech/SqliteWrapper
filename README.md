# SqliteWrapper

Sqlite lightweight wrapper library written in C. Easy and lean to make queries and handle results

### Dependencies
- [Collections](https://github.com/ximtech/Collections)

### Features

- Pure C implementation and OOP like design
- Simple configuration and user-friendly API
- Named query parameters
- Advanced parameter resolving and binding
- Iterating over the `ResultSet` and returning results
- Column by name resolving in `ResultSet`
- Suitable for embedded applications

### TODO

- Transaction support


### Add as CPM project dependency

How to add CPM to the project, check the [link](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage(
        NAME SqliteWrapper
        GITHUB_REPOSITORY ximtech/SqliteWrapper
        GIT_TAG origin/main)

target_link_libraries(${PROJECT_NAME} SqliteWrapper)
```

```cmake
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
# For Clion STM32 plugin generated Cmake use 
target_link_libraries(${PROJECT_NAME}.elf SqliteWrapper)
```

## Usage

### Single header include

```c
#include "SqliteWrapper.h"
```

### Queries With Named Parameters

Wrapper provides an alternative to the traditional syntax using `?` and format specifiers to specify query parameters.

```c
str_DbValueMap *params = SQL_PARAM_MAP("id", 1);
ResultSet *rs = executeQuery(db, "SELECT FIRST_NAME FROM EMPLOYEE WHERE ID = :id", params);
```

#### Parameters also can be inlined
Parameter map macro `SQL_PARAM_MAP()` auto resolves db value types. Keys ***must*** be a string literals.

```c
ResultSet *rs = executeQuery(db, "SELECT FIRST_NAME FROM EMPLOYEE WHERE ID = :id", SQL_PARAM_MAP("id", 1));
```

***Note:*** `SQL_PARAM_MAP()` - can store up to 12 key/value pairs. If there is need to store more parameters, use alternative solution

#### Alternative way to create query parameter map

```c
str_DbValueMap *paramMap = NEW_SQL_PARAM_MAP(16);   // Parameter map capacity must be a compile known constant
str_DbValueMapAdd(paramMap, "param_1", DB_VALUE("value"));
str_DbValueMapAdd(paramMap, "param_2", DB_VALUE(1234));
str_DbValueMapAdd(paramMap, "param_3", DB_VALUE(1.234));
assert(str_DbValueMapSize(paramMap) == 3);

DbValue dbValue_1 = str_DbValueMapGet(paramMap, "param_1"); // get parameter by key
DbValue dbValue_2 = str_DbValueMapGetOrDefault(paramMap, "not_param", DB_VALUE("default"));
str_DbValueMapClear(paramMap);  // remove all params
assert(is_str_DbValueMapEmpty(paramMap) == true);
assert(str_DbValueMapContains(paramMap, "param_2") == false); // check for element existence
```

***Note:*** No need to free `str_DbValueMap` params, heap memory is not used.

### Full example

```c
// Open a database file
sqlite3 *db = sqliteDbInit("embedded.db");

// Create a table
executeUpdate(db, "CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, value INTEGER, data TEXT NOT NULL)", NULL);

// Insert values to the table using named params
executeUpdate(db, "INSERT INTO test VALUES (NULL, :int_val, :data_text)", SQL_PARAM_MAP("int_val", 2, "data_text", "test"));

// Insert values using query string
QueryString *query = queryStringOf("INSERT INTO test VALUES (NULL, %d, %s)", 3, "some text value"):
executeUpdate(db, query->value, NULL);
deleteQueryString(query);   // free query string

// Parameters as separate key/value pairs map
str_DbValueMap *params = SQL_PARAM_MAP("int_val", 2);
ResultSet *rs = executeQuery(db, "SELECT * FROM test WHERE value = :int_val", params);

while (nextResultSet(rs)) {
    int id = rsGetInt(rs, "id");
    int value = rsGetInt(rs, "value");
    const char *data = rsGetString(rs, "data");
    
    printf("Id: [%d], Value: [%d], Data: [%s]\n", id, value, data);
}

executeUpdate(db, "DROP TABLE test", NULL);

// Free resources
resultSetDelete(rs);
sqliteDbClose(db);
```
