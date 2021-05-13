//---------------------------------------------------------------------------------------------------------------------------------
// File: pdo_util.cpp
//
// Contents: Utility functions used by both connection or statement functions
// 
// Copyright Microsoft Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//---------------------------------------------------------------------------------------------------------------------------------

#include "pdo_sqlsrv.h"

#include "zend_exceptions.h"


// *** internal constants ***
namespace {

const char WARNING_TEMPLATE[] = "SQLSTATE: %1!s!\nError Code: %2!d!\nMError Message: %3!s!\n";
const char EXCEPTION_MSG_TEMPLATE[] = "SQLSTATE[%s]: %s";
char EXCEPTION_PROPERTY_MSG[] = "message";
char EXCEPTION_PROPERTY_CODE[] = "code";
char EXCEPTION_PROPERTY_ERRORINFO[] = "errorInfo";
const int MAX_DIGITS = 11; // +-2 billion = 10 digits + 1 for the sign if negative

// buffer used to hold a formatted log message prior to actually logging it.
const int LOG_MSG_SIZE = 2048;
char log_msg[ LOG_MSG_SIZE ];

// internal error that says that FormatMessage failed
SQLCHAR INTERNAL_FORMAT_ERROR[] = "An internal error occurred.  FormatMessage failed writing an error message.";

// build the object and throw the PDO exception
void pdo_sqlsrv_throw_exception( sqlsrv_error_const* error TSRMLS_DC );

}

// pdo driver error messages
// errors have 3 components, the SQLSTATE (always 'IMSSP'), the error message, and an error code, which for us is always < 0
pdo_error PDO_ERRORS[] = {
    
    {
        SQLSRV_ERROR_DRIVER_NOT_INSTALLED,
        { IMSSP, (SQLCHAR*) "This extension requires either the Microsoft SQL Server 2012 Native Client, the "
        "Microsoft SQL Server 2008 Native Client or the Microsoft SQL Server 2005 Native Client to communicate with SQL Server.\r\n"
        "Access the following URL to download the Microsoft SQL Server 2012 Native Client ODBC driver for %1!s!: "
        "http://go.microsoft.com/fwlink/?LinkId=163712", -1, true }
    },  
    {
        SQLSRV_ERROR_ZEND_HASH,
        { IMSSP, (SQLCHAR*) "An error occurred creating or accessing a Zend hash table.", -2, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_OUTPUT_PARAM_TYPE,
        { IMSSP, (SQLCHAR*) "An invalid PHP type was specified as an output parameter. DateTime objects, NULL values, and streams "
        "cannot be specified as output parameters.", -3, false }
    },
    {
        SQLSRV_ERROR_INVALID_PARAMETER_PHPTYPE,
        { IMSSP, (SQLCHAR*) "An invalid type for parameter %1!d! was specified.  Only booleans, integers, floating point "
          "numbers, strings, and streams may be used as parameters.", -4, true }
    },
    {
        SQLSRV_ERROR_INVALID_PARAMETER_SQLTYPE,
        { IMSSP, (SQLCHAR*) "An invalid SQL Server type for parameter %1!d! was specified.", -5, true }
    },
    {
        SQLSRV_ERROR_INVALID_PARAMETER_ENCODING,
        { IMSSP, (SQLCHAR*) "An invalid encoding was specified for parameter %1!d!.", -6, true }
    },
    {
        SQLSRV_ERROR_INPUT_PARAM_ENCODING_TRANSLATE,
        { IMSSP, (SQLCHAR*) "An error occurred translating string for input param %1!d! to UCS-2: %2!s!", -7, true }
    },
    {
        SQLSRV_ERROR_OUTPUT_PARAM_ENCODING_TRANSLATE,
        { IMSSP, (SQLCHAR*) "An error occurred translating string for an output param to UTF-8: %1!s!", -8, true }
    },
    {
        SQLSRV_ERROR_CONNECT_STRING_ENCODING_TRANSLATE,
        { IMSSP, (SQLCHAR*) "An error occurred translating the connection string to UTF-16: %1!s!", -9, true }
    },
    {
        SQLSRV_ERROR_ZEND_STREAM,
        { IMSSP, (SQLCHAR*) "An error occurred reading from a PHP stream.", -10, false }
    },
    {
        SQLSRV_ERROR_INPUT_STREAM_ENCODING_TRANSLATE,
        { IMSSP, (SQLCHAR*) "An error occurred translating a PHP stream from UTF-8 to UTF-16: %1!s!", -11, true }
    },
    {
        SQLSRV_ERROR_UNKNOWN_SERVER_VERSION,
        { IMSSP, (SQLCHAR*) "Failed to retrieve the server version.  Unable to continue.", -12, false }
    },
    {
        SQLSRV_ERROR_FETCH_PAST_END,
        { IMSSP, (SQLCHAR*) "There are no more rows in the active result set.  Since this result set is not scrollable, "
          "no more data may be retrieved.", -13, false }
    },
    {
        SQLSRV_ERROR_STATEMENT_NOT_EXECUTED,
        { IMSSP, (SQLCHAR*) "The statement must be executed before results can be retrieved.", -14, false }
    },
    {
        SQLSRV_ERROR_NO_FIELDS,
        { IMSSP, (SQLCHAR*) "The active result for the query contains no fields.", -15, false }
    },
 
    {
        SQLSRV_ERROR_FETCH_NOT_CALLED,
        { IMSSP, (SQLCHAR*) "Internal pdo_sqlsrv error: Tried to retrieve a field before one of the PDOStatement::fetch "
          "functions was called.", -16, false }
    },
    {
        SQLSRV_ERROR_NO_DATA,
        { IMSSP, (SQLCHAR*)"Field %1!d! returned no data.", -17, true }
    },
    {
        SQLSRV_ERROR_FIELD_ENCODING_TRANSLATE,
        { IMSSP, (SQLCHAR*)"An error occurred translating string for a field to UTF-8: %1!s!", -18, true }
    },
    {
        SQLSRV_ERROR_ZEND_HASH_CREATE_FAILED,
        { IMSSP, (SQLCHAR*) "Zend returned an error when creating an associative array.", -19, false }
    },
    {
        SQLSRV_ERROR_NEXT_RESULT_PAST_END,
        { IMSSP, (SQLCHAR*)"There are no more results returned by the query.", -20, false }
    },
    {
        SQLSRV_ERROR_UID_PWD_BRACES_NOT_ESCAPED,
        { IMSSP, (SQLCHAR*) "An unescaped right brace (}) was found in either the user name or password.  All right braces must be"
        " escaped with another right brace (}}).", -21, false }
    },
    {
        SQLSRV_ERROR_UNESCAPED_RIGHT_BRACE_IN_DSN,
        { IMSSP, (SQLCHAR*) "An unescaped right brace (}) was found in the DSN string for keyword  '%1!s!'.  All right braces "
          "must be escaped with another right brace (}}).", -22, true }
    },
    {
        SQLSRV_ERROR_INVALID_OPTION_TYPE_INT,
        { IMSSP, (SQLCHAR*) "Invalid value type for option %1!s! was specified.  Integer type was expected.", -23, true }
    },
    {
        SQLSRV_ERROR_INVALID_OPTION_TYPE_STRING,
        { IMSSP, (SQLCHAR*) "Invalid value type for option %1!s! was specified.  String type was expected.", -24, true }
    },
    {
        SQLSRV_ERROR_CONN_OPTS_WRONG_TYPE,
        { IMSSP, (SQLCHAR*) "Expected an array of options for the connection. Connection options must be passed as an array of "
        "key/value pairs.", -25, false }
    },
    {
        SQLSRV_ERROR_INVALID_CONNECTION_KEY,
        { IMSSP, (SQLCHAR*) "An invalid connection option key type was received. Option key types must be strings.", -26, false }
    },
             
    {
        SQLSRV_ERROR_INVALID_TYPE,
        { IMSSP, (SQLCHAR*) "Invalid type.", -27, false }
    },

    {
        PDO_SQLSRV_ERROR_INVALID_COLUMN_INDEX,
        {IMSSP, (SQLCHAR*)"An invalid column number was specified.", -28, false }
    },

    {
        SQLSRV_ERROR_MAX_PARAMS_EXCEEDED,
        { IMSSP, (SQLCHAR*) "Tried to bind parameter number %1!d!.  SQL Server supports a maximum of 2100 parameters.", -29, true }
    },
    {
        SQLSRV_ERROR_INVALID_OPTION_KEY,
        { IMSSP, (SQLCHAR*) "Invalid option key %1!s! specified.", -30, true }
    },
    {
        SQLSRV_ERROR_INVALID_QUERY_TIMEOUT_VALUE,
        { IMSSP, (SQLCHAR*) "Invalid value %1!s! specified for option PDO::SQLSRV_ATTR_QUERY_TIMEOUT.", -31, true }
    },
    {
        SQLSRV_ERROR_INVALID_OPTION_SCROLLABLE,
        { IMSSP, (SQLCHAR*) "The value passed for the 'Scrollable' statement option is invalid.", -32, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DBH_ATTR,
        { IMSSP, (SQLCHAR*) "An invalid attribute was designated on the PDO object.", -33, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_STMT_ATTR,
        { IMSSP, (SQLCHAR*) "An invalid attribute was designated on the PDOStatement object.", -34, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_ENCODING,
        { IMSSP, (SQLCHAR*) "An invalid encoding was specified for SQLSRV_ATTR_ENCODING.", -35, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DRIVER_PARAM,
        { IMSSP, (SQLCHAR*) "An invalid type or value was given for the parameter driver data.  Only encoding constants "
          "such as PDO::SQLSRV_ENCODING_UTF8 may be used as parameter driver options.", -36, false }
    },
    {
        PDO_SQLSRV_ERROR_PDO_STMT_UNSUPPORTED,
        { IMSSP, (SQLCHAR*) "PDO::PARAM_STMT is not a supported parameter type.", -37, false }
    },
    {
        PDO_SQLSRV_ERROR_UNSUPPORTED_DBH_ATTR,
        { IMSSP, (SQLCHAR*) "An unsupported attribute was designated on the PDO object.", -38, false }
    },
    {
        PDO_SQLSRV_ERROR_STMT_LEVEL_ATTR,
        { IMSSP, (SQLCHAR*) "The given attribute is only supported on the PDOStatement object.", -39, false }
    },
    {
        PDO_SQLSRV_ERROR_READ_ONLY_DBH_ATTR,
        { IMSSP, (SQLCHAR*) "A read-only attribute was designated on the PDO object.", -40, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DSN_STRING,
        {IMSSP, (SQLCHAR*)"An invalid DSN string was specified.", -41, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DSN_KEY,
        { IMSSP, (SQLCHAR*) "An invalid keyword '%1!s!' was specified in the DSN string.", -42, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_STMT_OPTION,
        { IMSSP, (SQLCHAR*) "An invalid statement option was specified.", -43, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_CURSOR_TYPE,
        { IMSSP, (SQLCHAR*) "An invalid cursor type was specified for either PDO::ATTR_CURSOR or "
          "PDO::SQLSRV_ATTR_CURSOR_SCROLL_TYPE", -44, false }
    },
    {
        PDO_SQLSRV_ERROR_PARAM_PARSE,
        { IMSSP, (SQLCHAR*) "An error occurred substituting the named parameters.", -45, false }
    },
    {
        PDO_SQLSRV_ERROR_LAST_INSERT_ID,
        { IMSSP, (SQLCHAR*) "An error occurred retrieving the last insert id.", -46, false }
    },
    {
        SQLSRV_ERROR_QUERY_STRING_ENCODING_TRANSLATE,
        { IMSSP, (SQLCHAR*) "An error occurred translating the query string to UTF-16: %1!s!.", -47, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_COLUMN_DRIVER_DATA,
        { IMSSP, (SQLCHAR*) "An invalid type or value was given as bound column driver data for column %1!d!.  Only "
          "encoding constants such as PDO::SQLSRV_ENCODING_UTF8 may be used as bound column driver data.", -48, true }
    },
    {
        PDO_SQLSRV_ERROR_COLUMN_TYPE_DOES_NOT_SUPPORT_ENCODING,
        { IMSSP, (SQLCHAR*) "An encoding was specified for column %1!d!.  Only PDO::PARAM_LOB and PDO::PARAM_STR column types "
          "can take an encoding option.", -49, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DRIVER_COLUMN_ENCODING,
        { IMSSP, (SQLCHAR*) "Invalid encoding specified for column %1!d!.", -50, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DRIVER_PARAM_TYPE,
        { IMSSP, (SQLCHAR*) "An encoding was specified for parameter %1!d!.  Only PDO::PARAM_LOB and PDO::PARAM_STR can take an "
          "encoding option.", -51, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DRIVER_PARAM_ENCODING,
        { IMSSP, (SQLCHAR*) "Invalid encoding specified for parameter %1!d!.", -52, true }
    },
    {
        PDO_SQLSRV_ERROR_CURSOR_ATTR_AT_PREPARE_ONLY,
        { IMSSP, (SQLCHAR*) "The PDO::ATTR_CURSOR and PDO::SQLSRV_ATTR_CURSOR_SCROLL_TYPE attributes may only be set in the "
          "$driver_options array of PDO::prepare.", -53, false }
    },
    {
        SQLSRV_ERROR_OUTPUT_PARAM_TRUNCATED,
        { IMSSP, (SQLCHAR*) "String data, right truncated for output parameter %1!d!.", -54, true }
    },
    {
        SQLSRV_ERROR_INPUT_OUTPUT_PARAM_TYPE_MATCH,
        { IMSSP, (SQLCHAR*) "Types for parameter value and PDO::PARAM_* constant must be compatible for input/output "
          "parameter %1!d!.", -55, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_PARAM_DIRECTION,
        { IMSSP, (SQLCHAR*) "Invalid direction specified for parameter %1!d!.  Input/output parameters must have a length.",
          -56, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_OUTPUT_STRING_SIZE,
        { IMSSP, (SQLCHAR*) "Invalid size for output string parameter %1!d!.  Input/output string parameters must have an "
          "explicit length.", -57, true }
    },
    {
        PDO_SQLSRV_ERROR_FUNCTION_NOT_IMPLEMENTED,
        { IMSSP, (SQLCHAR*) "This function is not implemented by this driver.", -58, false }
    },      
    {
        /* The stream related errors are not currently used in PDO, but the core layer can throw the stream related 
           errors so having a mapping here */

        SQLSRV_ERROR_STREAMABLE_TYPES_ONLY,
        { IMSSP, (SQLCHAR*) "Only char, nchar, varchar, nvarchar, binary, varbinary, and large object types can be read by using "
          "streams.", -59, false}
    },
    {
        SQLSRV_ERROR_STREAM_CREATE,
        { IMSSP, (SQLCHAR*)"An error occurred while retrieving a SQL Server field as a stream.", -60, false }
    },
    {
        SQLSRV_ERROR_MARS_OFF,
        { IMSSP, (SQLCHAR*)"The connection cannot process this operation because there is a statement with pending results.  "
          "To make the connection available for other queries, either fetch all results or cancel or free the statement.  "
          "For more information, see the product documentation about the MultipleActiveResultSets connection option.", -61, false }
    },
    {
        SQLSRV_ERROR_FIELD_INDEX_ERROR,
        { IMSSP, (SQLCHAR*)"Fields within a row must be accessed in ascending order.  Cannot retrieve field %1!d! because its "
          "index is less than the index of a field that has already been retrieved (%2!d!).", -62, true }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_DSN_VALUE,
        { IMSSP, (SQLCHAR*) "An invalid value was specified for the keyword '%1!s!' in the DSN string.", -63, true }
    },
    {
        PDO_SQLSRV_ERROR_SERVER_NOT_SPECIFIED,
        { IMSSP, (SQLCHAR*) "Server keyword was not specified in the DSN string.", -64, false }
    },
    {
        PDO_SQLSRV_ERROR_DSN_STRING_ENDED_UNEXPECTEDLY,
        { IMSSP, (SQLCHAR*) "The DSN string ended unexpectedly.", -65, false }
    },
    {
        PDO_SQLSRV_ERROR_EXTRA_SEMI_COLON_IN_DSN_STRING,
        { IMSSP, (SQLCHAR*) "An extra semi-colon was encountered in the DSN string at character (byte-count) position '%1!d!' .",
          -66, true }
    },
    {
        PDO_SQLSRV_ERROR_RCB_MISSING_IN_DSN_VALUE,
        { IMSSP, (SQLCHAR*) "An expected right brace (}) was not found in the DSN string for the value of the keyword '%1!s!'.",
          -67, true }
    },
    {
        PDO_SQLSRV_ERROR_DQ_ATTR_AT_PREPARE_ONLY,
        { IMSSP, (SQLCHAR*) "The PDO::SQLSRV_ATTR_DIRECT_QUERY attribute may only be set in the $driver_options array of "
          "PDO::prepare.", -68, false }
    },
    {
        PDO_SQLSRV_ERROR_INVALID_CURSOR_WITH_SCROLL_TYPE,
        { IMSSP, (SQLCHAR*) "The PDO::SQLSRV_ATTR_CURSOR_SCROLL_TYPE attribute may only be set when PDO::ATTR_CURSOR is set to "
          "PDO::CURSOR_SCROLL in the $driver_options array of PDO::prepare.", -69, false }
    },
    {
        SQLSRV_ERROR_INVALID_BUFFER_LIMIT,
        { IMSSP, (SQLCHAR*) "The PDO::SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE attribute is not a number or the number is not "
          "positive. Only positive numbers are valid for this attribute.", -70, false }
    },
    {
        SQLSRV_ERROR_BUFFER_LIMIT_EXCEEDED,
        { IMSSP, (SQLCHAR*) "Memory limit of %1!d! KB exceeded for buffered query", -71, true }
    },

    { -1, {} }
};

// Returns a sqlsrv_error for a given error code.
sqlsrv_error_const* get_error_message( unsigned int sqlsrv_error_code ) {
    
    sqlsrv_error_const *error_message = NULL;
    int zr = zend_hash_index_find( g_pdo_errors_ht, sqlsrv_error_code, reinterpret_cast<void**>( &error_message ));
    if( zr == FAILURE ) {
        DIE( "get_error_message: zend_hash_index_find returned failure for sqlsrv_error_code = %1!d!", sqlsrv_error_code );   
    }
    
    SQLSRV_ASSERT( error_message != NULL, "get_error_message: error_message was null");

    return error_message;
}

// PDO error handler for the environment context.
bool pdo_sqlsrv_handle_env_error( sqlsrv_context& ctx, unsigned int sqlsrv_error_code, bool warning TSRMLS_DC, 
                                  va_list* print_args )
{
    SQLSRV_ASSERT(( ctx != NULL ), "pdo_sqlsrv_handle_env_error: sqlsrv_context was null" );
    pdo_dbh_t* dbh = reinterpret_cast<pdo_dbh_t*>( ctx.driver());    
    SQLSRV_ASSERT(( dbh != NULL ), "pdo_sqlsrv_handle_env_error: pdo_dbh_t was null" );
    
    sqlsrv_error_auto_ptr error;

    if( sqlsrv_error_code != SQLSRV_ERROR_ODBC ) {

        core_sqlsrv_format_driver_error( ctx, get_error_message( sqlsrv_error_code ), error, SEV_ERROR TSRMLS_CC, print_args );
    }
    else {

        bool err = core_sqlsrv_get_odbc_error( ctx, 1, error, SEV_ERROR TSRMLS_CC );
        SQLSRV_ASSERT( err == true, "No ODBC error was found" );
    }

    strcpy_s( dbh->error_code, sizeof( pdo_error_type ), reinterpret_cast<const char*>( error->sqlstate ));

    switch( dbh->error_mode ) {

        case PDO_ERRMODE_EXCEPTION:
            if( !warning ) {

                pdo_sqlsrv_throw_exception( error TSRMLS_CC );
            }
            ctx.set_last_error( error );
            break;

        default:
            DIE( "pdo_sqlsrv_handle_env_error: Unexpected error mode. %1!d!", dbh->error_mode );
            break;
    }
    
    // we don't transfer the zval_auto_ptr since set_last_error increments the zval ref count
    // return error ignored = true for warnings.
    return ( warning ? true : false );

}

// pdo error handler for the dbh context.
bool pdo_sqlsrv_handle_dbh_error( sqlsrv_context& ctx, unsigned int sqlsrv_error_code, bool warning TSRMLS_DC, 
                                  va_list* print_args )
{
    pdo_dbh_t* dbh = reinterpret_cast<pdo_dbh_t*>( ctx.driver());
    SQLSRV_ASSERT( dbh != NULL, "pdo_sqlsrv_handle_dbh_error: Null dbh passed" );

    sqlsrv_error_auto_ptr error;

    if( sqlsrv_error_code != SQLSRV_ERROR_ODBC ) {
        
        core_sqlsrv_format_driver_error( ctx, get_error_message( sqlsrv_error_code ), error, SEV_ERROR TSRMLS_CC, print_args );
    }
    else {
        bool err = core_sqlsrv_get_odbc_error( ctx, 1, error, SEV_ERROR TSRMLS_CC );
        SQLSRV_ASSERT( err == true, "No ODBC error was found" );
    }

    SQLSRV_STATIC_ASSERT( sizeof( error->sqlstate ) <= sizeof( dbh->error_code ));
    strcpy_s( dbh->error_code, sizeof( dbh->error_code ), reinterpret_cast<const char*>( error->sqlstate ));

    switch( dbh->error_mode ) {
        case PDO_ERRMODE_EXCEPTION:
            if( !warning ) {

                pdo_sqlsrv_throw_exception( error TSRMLS_CC );
            }
            ctx.set_last_error( error );
            break;
        case PDO_ERRMODE_WARNING:
            if( !warning ) {
                unsigned int msg_len = strlen( reinterpret_cast<const char*>( error->native_message )) + SQL_SQLSTATE_BUFSIZE 
                    + MAX_DIGITS + 1;
                sqlsrv_malloc_auto_ptr<char> msg;
                msg = static_cast<char*>( sqlsrv_malloc( msg_len ));
                core_sqlsrv_format_message( msg, msg_len, WARNING_TEMPLATE, error->sqlstate, error->native_code, 
                                            error->native_message );
                php_error( E_WARNING, msg );
                sqlsrv_free( msg );
            }
            ctx.set_last_error( error );
            break;
        case PDO_ERRMODE_SILENT:
            ctx.set_last_error( error );
            break;
        default:
            DIE( "Unknown error mode. %1!d!", dbh->error_mode );
            break;
    }

    // return error ignored = true for warnings.
    return ( warning ? true : false );
}

// PDO error handler for the statement context.
bool pdo_sqlsrv_handle_stmt_error( sqlsrv_context& ctx, unsigned int sqlsrv_error_code, bool warning TSRMLS_DC,
                                   va_list* print_args )
{
    pdo_stmt_t* pdo_stmt = reinterpret_cast<pdo_stmt_t*>( ctx.driver());
    SQLSRV_ASSERT( pdo_stmt != NULL && pdo_stmt->dbh != NULL, "pdo_sqlsrv_handle_stmt_error: Null statement or dbh passed" );

    sqlsrv_error_auto_ptr error;

    if( sqlsrv_error_code != SQLSRV_ERROR_ODBC ) {
        core_sqlsrv_format_driver_error( ctx, get_error_message( sqlsrv_error_code ), error, SEV_ERROR TSRMLS_CC, print_args );
    }
    else {
        bool err = core_sqlsrv_get_odbc_error( ctx, 1, error, SEV_ERROR TSRMLS_CC );
        SQLSRV_ASSERT( err == true, "No ODBC error was found" );
    }

    SQLSRV_STATIC_ASSERT( sizeof( error->sqlstate ) <= sizeof( pdo_stmt->error_code ));
    strcpy_s( pdo_stmt->error_code, sizeof( pdo_stmt->error_code ), reinterpret_cast<const char*>( error->sqlstate ));

    switch( pdo_stmt->dbh->error_mode ) {
        case PDO_ERRMODE_EXCEPTION:
            if( !warning ) {

                pdo_sqlsrv_throw_exception( error TSRMLS_CC );
            }
            ctx.set_last_error( error );
            break;
        case PDO_ERRMODE_WARNING:
            if( !warning ) {
                unsigned int msg_len = strlen( reinterpret_cast<const char*>( error->native_message )) + SQL_SQLSTATE_BUFSIZE 
                    + MAX_DIGITS + 1;
                sqlsrv_malloc_auto_ptr<char> msg;
                msg = static_cast<char*>( sqlsrv_malloc( msg_len ));
                core_sqlsrv_format_message( msg, msg_len, WARNING_TEMPLATE, error->sqlstate, error->native_code, 
                                            error->native_message );
                php_error( E_WARNING, msg );
                sqlsrv_free( msg );
            }
            ctx.set_last_error( error );
            break;
        case PDO_ERRMODE_SILENT:
            ctx.set_last_error( error );
            break;
        default:
            DIE( "Unknown error mode. %1!d!", pdo_stmt->dbh->error_mode );
            break;
    }

    // return error ignored = true for warnings.
    return ( warning ? true : false );
}


// Transfer a sqlsrv_context's error to a PDO zval.  The standard format for a zval error is 3 elements:
// 0, native code
// 1, native message
// 2, SQLSTATE of the error (driver specific error messages are 'IMSSP')

void pdo_sqlsrv_retrieve_context_error( sqlsrv_error const* last_error, zval* pdo_zval )
{
    if( last_error ) {

        // SQLSTATE is already present in the zval.
        add_next_index_long( pdo_zval, last_error->native_code );
        add_next_index_string( pdo_zval, reinterpret_cast<char*>( last_error->native_message ), 1 /*dup*/ );
    }
    else {
        add_next_index_null( pdo_zval ); /* native code */
        add_next_index_null( pdo_zval ); /* native message */
    }

}

// Formats the error message and writes to the php error log.
void pdo_sqlsrv_log( unsigned int severity TSRMLS_DC, const char* msg, va_list* print_args )
{
    if( (severity & PDO_SQLSRV_G( log_severity )) == 0 ) {
        return;
    }

    DWORD rc = FormatMessage( FORMAT_MESSAGE_FROM_STRING, msg, 0, 0, log_msg, LOG_MSG_SIZE, print_args );
    // if an error occurs for FormatMessage, we just output an internal error occurred.
    if( rc == 0 ) {
        SQLSRV_STATIC_ASSERT( sizeof( INTERNAL_FORMAT_ERROR ) < sizeof( log_msg ));
        std::copy( INTERNAL_FORMAT_ERROR, INTERNAL_FORMAT_ERROR + sizeof( INTERNAL_FORMAT_ERROR ), log_msg );
    }

    php_log_err( log_msg TSRMLS_CC );
}

namespace {

void pdo_sqlsrv_throw_exception( sqlsrv_error_const* error TSRMLS_DC )
{
    zval_auto_ptr ex_obj;
    MAKE_STD_ZVAL( ex_obj );
    zend_class_entry* ex_class = pdo_get_exception_class();

    int zr = object_init_ex( ex_obj, ex_class );
    SQLSRV_ASSERT( zr != FAILURE, "Failed to initialize exception object" );

    sqlsrv_malloc_auto_ptr<char> ex_msg;
    size_t ex_msg_len = strlen( reinterpret_cast<const char*>( error->native_message )) + SQL_SQLSTATE_BUFSIZE +
        12 + 1; // 12 = "SQLSTATE[]: "
    ex_msg = reinterpret_cast<char*>( sqlsrv_malloc( ex_msg_len ));
    snprintf( ex_msg, ex_msg_len, EXCEPTION_MSG_TEMPLATE, error->sqlstate, error->native_message );
    zend_update_property_string( ex_class, ex_obj, EXCEPTION_PROPERTY_MSG, sizeof( EXCEPTION_PROPERTY_MSG ) - 1, 
                                 ex_msg TSRMLS_CC );
    zend_update_property_string( ex_class, ex_obj, EXCEPTION_PROPERTY_CODE, sizeof( EXCEPTION_PROPERTY_CODE ) - 1,
                                 reinterpret_cast<char*>( error->sqlstate ) TSRMLS_CC );

    zval_auto_ptr ex_error_info;
    MAKE_STD_ZVAL( ex_error_info );
    array_init( ex_error_info );
    add_next_index_string( ex_error_info, reinterpret_cast<char*>( error->sqlstate ), 1 /* dup */ );
    add_next_index_long( ex_error_info, error->native_code );
    add_next_index_string( ex_error_info, reinterpret_cast<char*>( error->native_message ), 1 /* dup */ );
    zend_update_property( ex_class, ex_obj, EXCEPTION_PROPERTY_ERRORINFO, sizeof( EXCEPTION_PROPERTY_ERRORINFO ) - 1, 
                          ex_error_info TSRMLS_CC );

    zend_throw_exception_object( ex_obj TSRMLS_CC );
    ex_msg.transferred();
    ex_obj.transferred();
}

}
