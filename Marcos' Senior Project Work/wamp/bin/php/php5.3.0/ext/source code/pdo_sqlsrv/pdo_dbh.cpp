//---------------------------------------------------------------------------------------------------------------------------------
// file: pdo_dbh.cpp
//
// Contents: Implements the PDO object for PDO_SQLSRV
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
#include <psapi.h>
#include <windows.h>
#include <winver.h>

#include <string>
#include <sstream>


typedef const zend_function_entry pdo_sqlsrv_function_entry;

// *** internal variables and constants ***

namespace {

const char LAST_INSERT_ID_QUERY[] = "SELECT @@IDENTITY;";
const char LAST_INSERT_ID_BUFF_LEN = 10;    // size of the buffer to hold the string value of the last insert id integer
const char TABLE_LAST_INSERT_ID_QUERY[] = "SELECT IDENT_CURRENT(%s)";
const int LAST_INSERT_ID_QUERY_MAX_LEN = sizeof( TABLE_LAST_INSERT_ID_QUERY ) + SQL_MAX_SQLSERVERNAME + 2; // include the quotes

// List of PDO supported connection options.
namespace PDOConnOptionNames {

const char Server[] = "Server";
const char APP[] = "APP";
const char ApplicationIntent[] = "ApplicationIntent";
const char AttachDBFileName[] = "AttachDbFileName";
const char ConnectionPooling[] = "ConnectionPooling";
const char Database[] = "Database";
const char Encrypt[] = "Encrypt";
const char Failover_Partner[] = "Failover_Partner";
const char LoginTimeout[] = "LoginTimeout";
const char MARS_Option[] = "MultipleActiveResultSets";
const char MultiSubnetFailover[] = "MultiSubnetFailover";
const char QuotedId[] = "QuotedId";
const char TraceFile[] = "TraceFile";
const char TraceOn[] = "TraceOn";
const char TrustServerCertificate[] = "TrustServerCertificate";
const char TransactionIsolation[] = "TransactionIsolation";
const char WSID[] = "WSID";

}

enum PDO_CONN_OPTIONS {

    PDO_CONN_OPTION_SERVER = SQLSRV_CONN_OPTION_DRIVER_SPECIFIC,

};

enum PDO_STMT_OPTIONS {

    PDO_STMT_OPTION_ENCODING = SQLSRV_STMT_OPTION_DRIVER_SPECIFIC,
    PDO_STMT_OPTION_DIRECT_QUERY,
    PDO_STMT_OPTION_CURSOR_SCROLL_TYPE,
    PDO_STMT_OPTION_CLIENT_BUFFER_MAX_KB_SIZE,
    PDO_STMT_OPTION_EMULATE_PREPARES,
};

// List of all the statement options supported by this driver.
const stmt_option PDO_STMT_OPTS[] = {
 
    { NULL, 0, SQLSRV_STMT_OPTION_QUERY_TIMEOUT, new stmt_option_query_timeout },
    { NULL, 0, SQLSRV_STMT_OPTION_SCROLLABLE, new stmt_option_scrollable },
    { NULL, 0, PDO_STMT_OPTION_ENCODING, new stmt_option_encoding },
    { NULL, 0, PDO_STMT_OPTION_DIRECT_QUERY, new stmt_option_direct_query },
    { NULL, 0, PDO_STMT_OPTION_CURSOR_SCROLL_TYPE, new stmt_option_cursor_scroll_type },
    { NULL, 0, PDO_STMT_OPTION_CLIENT_BUFFER_MAX_KB_SIZE, new stmt_option_buffered_query_limit },
    { NULL, 0, PDO_STMT_OPTION_EMULATE_PREPARES, new stmt_option_emulate_prepares },

    { NULL, 0, SQLSRV_STMT_OPTION_INVALID, NULL},
};

// boolean connection string
struct pdo_bool_conn_str_func 
{
    static void func( connection_option const* option, zval* value, sqlsrv_conn* /*conn*/, std::string& conn_str TSRMLS_DC );
};

struct pdo_txn_isolation_conn_attr_func 
{
    static void func( connection_option const* /*option*/, zval* value_z, sqlsrv_conn* conn, std::string& /*conn_str*/ TSRMLS_DC );
};

template <unsigned int Attr>
struct pdo_int_conn_attr_func {

    static void func( connection_option const* /*option*/, zval* value, sqlsrv_conn* conn, std::string& /*conn_str*/ TSRMLS_DC )
    {
        try {
        
            SQLSRV_ASSERT( Z_TYPE_P( value ) == IS_STRING, "pdo_int_conn_attr_func: Unexpected zval type." );
            
            int val = atoi( Z_STRVAL_P( value ));
            core::SQLSetConnectAttr( conn, Attr, reinterpret_cast<SQLPOINTER>( val ), SQL_IS_UINTEGER TSRMLS_CC );
        }
        catch( core::CoreException& ) {
            throw;
        }
    }
};

template <unsigned int Attr>
struct pdo_bool_conn_attr_func {

    static void func( connection_option const* /*option*/, zval* value, sqlsrv_conn* conn, std::string& /*conn_str*/ TSRMLS_DC )
    {
         try {
        
            core::SQLSetConnectAttr( conn, Attr, reinterpret_cast<SQLPOINTER>( core_str_zval_is_true( value )), 
                                     SQL_IS_UINTEGER TSRMLS_CC );
        }
        catch( core::CoreException& ) {
            throw;
        }
    }
};

// statement options related functions
void add_stmt_option_key( sqlsrv_context& ctx, unsigned long key, HashTable* options_ht, 
                         zval** data TSRMLS_DC );
void validate_stmt_options( sqlsrv_context& ctx, zval* stmt_options, __inout HashTable* pdo_stmt_options_ht TSRMLS_DC );

}       // namespace


// List of all connection options supported by this driver.
const connection_option PDO_CONN_OPTS[] = {
    { 
        PDOConnOptionNames::Server,
        sizeof( PDOConnOptionNames::Server ),
        PDO_CONN_OPTION_SERVER,
        NULL,
        0,
        CONN_ATTR_STRING,
        conn_str_append_func::func 
    },
    { 
        PDOConnOptionNames::APP,
        sizeof( PDOConnOptionNames::APP ),
        SQLSRV_CONN_OPTION_APP,
        ODBCConnOptions::APP,
        sizeof( ODBCConnOptions::APP ),
        CONN_ATTR_STRING,
        conn_str_append_func::func 
    },
    { 
        PDOConnOptionNames::ApplicationIntent,
        sizeof( PDOConnOptionNames::ApplicationIntent ),
        SQLSRV_CONN_OPTION_APPLICATION_INTENT,
        ODBCConnOptions::ApplicationIntent,
        sizeof( ODBCConnOptions::ApplicationIntent ),
        CONN_ATTR_STRING,
        conn_str_append_func::func 
    },
    { 
        PDOConnOptionNames::AttachDBFileName,
        sizeof( PDOConnOptionNames::AttachDBFileName ),
        SQLSRV_CONN_OPTION_ATTACHDBFILENAME,
        ODBCConnOptions::AttachDBFileName,
        sizeof( ODBCConnOptions::AttachDBFileName ),
        CONN_ATTR_STRING,
        conn_str_append_func::func 
    },
    {
        PDOConnOptionNames::ConnectionPooling,
        sizeof( PDOConnOptionNames::ConnectionPooling ),
        SQLSRV_CONN_OPTION_CONN_POOLING,
        ODBCConnOptions::ConnectionPooling,
        sizeof( ODBCConnOptions::ConnectionPooling ),
        CONN_ATTR_BOOL,
        conn_null_func::func
    },
    {
        PDOConnOptionNames::Database,
        sizeof( PDOConnOptionNames::Database ),
        SQLSRV_CONN_OPTION_DATABASE,
        ODBCConnOptions::Database,
        sizeof( ODBCConnOptions::Database ),
        CONN_ATTR_STRING,
        conn_str_append_func::func
    },
    {
        PDOConnOptionNames::Encrypt,
        sizeof( PDOConnOptionNames::Encrypt ),
        SQLSRV_CONN_OPTION_ENCRYPT,
        ODBCConnOptions::Encrypt, 
        sizeof( ODBCConnOptions::Encrypt ),
        CONN_ATTR_BOOL,
        pdo_bool_conn_str_func::func
    },
    { 
        PDOConnOptionNames::Failover_Partner,
        sizeof( PDOConnOptionNames::Failover_Partner ),
        SQLSRV_CONN_OPTION_FAILOVER_PARTNER,
        ODBCConnOptions::Failover_Partner,
        sizeof( ODBCConnOptions::Failover_Partner ), 
        CONN_ATTR_STRING,
        conn_str_append_func::func
    },
    {
        PDOConnOptionNames::LoginTimeout,
        sizeof( PDOConnOptionNames::LoginTimeout ),
        SQLSRV_CONN_OPTION_LOGIN_TIMEOUT,
        ODBCConnOptions::LoginTimeout,
        sizeof( ODBCConnOptions::LoginTimeout ),
        CONN_ATTR_INT,
        pdo_int_conn_attr_func<SQL_ATTR_LOGIN_TIMEOUT>::func 
    },
    {
        PDOConnOptionNames::MARS_Option,
        sizeof( PDOConnOptionNames::MARS_Option ),
        SQLSRV_CONN_OPTION_MARS,
        ODBCConnOptions::MARS_ODBC,
        sizeof( ODBCConnOptions::MARS_ODBC ),
        CONN_ATTR_BOOL,
        pdo_bool_conn_str_func::func
    },
    {
        PDOConnOptionNames::MultiSubnetFailover,
        sizeof( PDOConnOptionNames::MultiSubnetFailover ),
        SQLSRV_CONN_OPTION_MULTI_SUBNET_FAILOVER,
        ODBCConnOptions::MultiSubnetFailover,
        sizeof( ODBCConnOptions::MultiSubnetFailover ),
        CONN_ATTR_BOOL,
        pdo_bool_conn_str_func::func
    },
    {
        PDOConnOptionNames::QuotedId,
        sizeof( PDOConnOptionNames::QuotedId ),
        SQLSRV_CONN_OPTION_QUOTED_ID,
        ODBCConnOptions::QuotedId,
        sizeof( ODBCConnOptions::QuotedId ),
        CONN_ATTR_BOOL,
        pdo_bool_conn_str_func::func
    },
    {
        PDOConnOptionNames::TraceFile,
        sizeof( PDOConnOptionNames::TraceFile ),
        SQLSRV_CONN_OPTION_TRACE_FILE,
        ODBCConnOptions::TraceFile,
        sizeof( ODBCConnOptions::TraceFile ), 
        CONN_ATTR_STRING,
        str_conn_attr_func<SQL_ATTR_TRACEFILE>::func 
    },
    {
        PDOConnOptionNames::TraceOn,
        sizeof( PDOConnOptionNames::TraceOn ),
        SQLSRV_CONN_OPTION_TRACE_ON,
        ODBCConnOptions::TraceOn,
        sizeof( ODBCConnOptions::TraceOn ),
        CONN_ATTR_BOOL,
        pdo_bool_conn_attr_func<SQL_ATTR_TRACE>::func
    },
    {
        PDOConnOptionNames::TransactionIsolation,
        sizeof( PDOConnOptionNames::TransactionIsolation ),
        SQLSRV_CONN_OPTION_TRANS_ISOLATION,
        ODBCConnOptions::TransactionIsolation,
        sizeof( ODBCConnOptions::TransactionIsolation ),
        CONN_ATTR_INT,
        pdo_txn_isolation_conn_attr_func::func
    },
    {
        PDOConnOptionNames::TrustServerCertificate,
        sizeof( PDOConnOptionNames::TrustServerCertificate ),
        SQLSRV_CONN_OPTION_TRUST_SERVER_CERT,
        ODBCConnOptions::TrustServerCertificate,
        sizeof( ODBCConnOptions::TrustServerCertificate ),
        CONN_ATTR_BOOL,
        pdo_bool_conn_str_func::func
    },
    {
        PDOConnOptionNames::WSID,
        sizeof( PDOConnOptionNames::WSID ),
        SQLSRV_CONN_OPTION_WSID,
        ODBCConnOptions::WSID,
        sizeof( ODBCConnOptions::WSID ),
        CONN_ATTR_STRING, 
        conn_str_append_func::func
    },
    { NULL, 0, SQLSRV_CONN_OPTION_INVALID, NULL, 0 , CONN_ATTR_INVALID, NULL },  //terminate the table
};


// close the connection
int pdo_sqlsrv_dbh_close( pdo_dbh_t *dbh TSRMLS_DC );

// execute queries
int pdo_sqlsrv_dbh_prepare( pdo_dbh_t *dbh, const char *sql,
                            long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC );
long pdo_sqlsrv_dbh_do( pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC );

// transaction support functions
int pdo_sqlsrv_dbh_commit( pdo_dbh_t *dbh TSRMLS_DC );
int pdo_sqlsrv_dbh_begin( pdo_dbh_t *dbh TSRMLS_DC );
int pdo_sqlsrv_dbh_rollback( pdo_dbh_t *dbh TSRMLS_DC );

// attribute functions
int pdo_sqlsrv_dbh_set_attr( pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC );
int pdo_sqlsrv_dbh_get_attr( pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC );

// return more information
int pdo_sqlsrv_dbh_return_error( pdo_dbh_t *dbh, pdo_stmt_t *stmt,
                                 zval *info TSRMLS_DC);

// return the last id generated by an executed SQL statement
char * pdo_sqlsrv_dbh_last_id( pdo_dbh_t *dbh, const char *name, unsigned int* len TSRMLS_DC );

// additional methods are supported in this function
pdo_sqlsrv_function_entry *pdo_sqlsrv_get_driver_methods( pdo_dbh_t *dbh, int kind TSRMLS_DC );

// quote a string, meaning put quotes around it and escape any quotes within it
int pdo_sqlsrv_dbh_quote( pdo_dbh_t* dbh, const char* unquoted, int unquotedlen, char **quoted, int* quotedlen, 
                          enum pdo_param_type paramtype TSRMLS_DC );

struct pdo_dbh_methods pdo_sqlsrv_dbh_methods = {

    pdo_sqlsrv_dbh_close,
    pdo_sqlsrv_dbh_prepare,
    pdo_sqlsrv_dbh_do,
    pdo_sqlsrv_dbh_quote,
    pdo_sqlsrv_dbh_begin,
    pdo_sqlsrv_dbh_commit,
    pdo_sqlsrv_dbh_rollback,
    pdo_sqlsrv_dbh_set_attr,
    pdo_sqlsrv_dbh_last_id,
    pdo_sqlsrv_dbh_return_error,
    pdo_sqlsrv_dbh_get_attr,
    NULL,                           // check liveness not implemented
    pdo_sqlsrv_get_driver_methods,
    NULL                            // request shutdown not implemented
};


// log a function entry point
#define PDO_LOG_DBH_ENTRY \
{ \
    pdo_sqlsrv_dbh* driver_dbh = reinterpret_cast<pdo_sqlsrv_dbh*>( dbh->driver_data ); \
    driver_dbh->set_func( __FUNCTION__ ); \
    LOG( SEV_NOTICE, __FUNCTION__ ## ": entering" ); \
}

// constructor for the internal object for connections
pdo_sqlsrv_dbh::pdo_sqlsrv_dbh( SQLHANDLE h, error_callback e, void* driver TSRMLS_DC ) :
    sqlsrv_conn( h, e, driver, SQLSRV_ENCODING_UTF8 TSRMLS_CC ),
    stmts( NULL ),
    direct_query( false ),
    query_timeout( QUERY_TIMEOUT_INVALID ),
    client_buffer_max_size( PDO_SQLSRV_G( client_buffer_max_size ) )
{
    if( client_buffer_max_size < 0 ) {
        client_buffer_max_size = sqlsrv_buffered_result_set::BUFFERED_QUERY_LIMIT_DEFAULT;
        LOG( SEV_WARNING, INI_PDO_SQLSRV_CLIENT_BUFFER_MAX_SIZE " set to a invalid value.  Resetting to default value." );
    }
}

// pdo_sqlsrv_db_handle_factory
// Maps to PDO::__construct. 
// Factory method called by the PDO driver manager to create a SQLSRV PDO connection.
// Does the following things:
//   1.Sets the error handling temporarily to PDO_ERRMODE_EXCEPTION.  
//     (If an error occurs in this function, the PDO specification mandates that 
//     an exception be thrown, regardless of the error mode setting.)
//   2. Processes the driver options.
//   3. Creates a core_conn object by calling core_sqlsrv_connect.
//   4. Restores the previous error mode on success.
// alloc_own_columns is set to 1 to tell the PDO driver manager that we manage memory
// Parameters:
// dbh - The PDO managed structure for the connection.
// driver_options - A HashTable (within the zval) of options to use when creating the connection.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_db_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) 
{
    LOG( SEV_NOTICE, "pdo_sqlsrv_db_handle_factory: entering" );

    hash_auto_ptr pdo_conn_options_ht;
    pdo_error_mode prev_err_mode = dbh->error_mode;

    // must be done in all cases so that even a failed connection can query the
    // object for errors.
    dbh->methods = &pdo_sqlsrv_dbh_methods;
    dbh->driver_data = NULL;
    zval** temp_server_z = NULL;
    sqlsrv_malloc_auto_ptr<conn_string_parser> dsn_parser;
    zval_auto_ptr server_z;

    try {
 
    // no matter what the error mode, we want exceptions thrown if the connection fails
    // to happen (per the PDO spec)
    dbh->error_mode = PDO_ERRMODE_EXCEPTION;

    g_henv_cp->set_driver( dbh );
    g_henv_ncp->set_driver( dbh );

    CHECK_CUSTOM_ERROR( driver_options && Z_TYPE_P( driver_options ) != IS_ARRAY, *g_henv_cp, SQLSRV_ERROR_CONN_OPTS_WRONG_TYPE ) {
        throw core::CoreException();
    }

    // Initialize the options array to be passed to the core layer
    ALLOC_HASHTABLE( pdo_conn_options_ht );

    core::sqlsrv_zend_hash_init( *g_henv_cp, pdo_conn_options_ht, 10 /* # of buckets */, NULL /*hashfn*/, 
                                 ZVAL_PTR_DTOR, 0 /*persistent*/ TSRMLS_CC );

    // Either of g_henv_cp or g_henv_ncp can be used to propogate the error.
    dsn_parser = new ( sqlsrv_malloc( sizeof( conn_string_parser ))) conn_string_parser( *g_henv_cp, dbh->data_source, 
                                                                                          dbh->data_source_len, pdo_conn_options_ht );
    dsn_parser->parse_conn_string( TSRMLS_C );
    
    // Extract the server name
    zend_hash_index_find( pdo_conn_options_ht, PDO_CONN_OPTION_SERVER, (void**)&temp_server_z );

    CHECK_CUSTOM_ERROR(( temp_server_z == NULL ), g_henv_cp, PDO_SQLSRV_ERROR_SERVER_NOT_SPECIFIED ) {
        
        throw pdo::PDOException();
    }

    server_z = *temp_server_z;

    // Add a reference to the option value since we are deleting it from the hashtable
    zval_add_ref( &server_z );
    zend_hash_index_del( pdo_conn_options_ht, PDO_CONN_OPTION_SERVER );

    sqlsrv_conn* conn = core_sqlsrv_connect( *g_henv_cp, *g_henv_ncp, core::allocate_conn<pdo_sqlsrv_dbh>, Z_STRVAL_P( server_z ), 
                                             dbh->username, dbh->password, pdo_conn_options_ht, pdo_sqlsrv_handle_dbh_error, 
                                             PDO_CONN_OPTS, dbh, "pdo_sqlsrv_db_handle_factory" TSRMLS_CC );
                                
    SQLSRV_ASSERT( conn != NULL, "Invalid connection returned.  Exception should have been thrown." );

    // set the driver_data and methods to complete creation of the PDO object
    dbh->driver_data = conn;
    dbh->error_mode = prev_err_mode;    // reset the error mode
    dbh->alloc_own_columns = 1;         // we do our own memory management for columns
    dbh->native_case = PDO_CASE_NATURAL;// SQL Server supports mixed case types

    }
    catch( core::CoreException& ) {

        dbh->error_mode = prev_err_mode;    // reset the error mode

        return 0;
    }
    catch( ... ) {

        DIE( "pdo_sqlsrv_db_handle_factory: Unknown exception caught" );
    }
        
    return 1;
}

// pdo_sqlsrv_dbh_close
// Maps to PDO::__destruct.
// Called when a PDO object is to be destroyed.
// By the time this function is called, PDO has already made sure that 
// all statements are disposed and the PDO object is the last item destroyed. 
// Parameters:
// dbh - The PDO managed connection object.
// Return:
// Always returns 1 for success.
int pdo_sqlsrv_dbh_close( pdo_dbh_t *dbh TSRMLS_DC )
{
    LOG( SEV_NOTICE, "pdo_sqlsrv_dbh_close: entering" );

    // if the connection didn't complete properly, driver_data isn't initialized.
    if( dbh->driver_data == NULL ) {

        return 1;
    }

    PDO_RESET_DBH_ERROR;

    // call the core layer close
    core_sqlsrv_close( reinterpret_cast<sqlsrv_conn*>( dbh->driver_data ) TSRMLS_CC );
    dbh->driver_data = NULL;

    // always return success that the connection is closed
    return 1;
}

// pdo_sqlsrv_dbh_prepare
// Called by PDO::prepare and PDOStatement::__construct.
// Creates a statement and prepares it for execution by PDO
// Paramters:
// dbh - The PDO managed connection object.
// sql - SQL query to be prepared.
// sql_len - Length of the sql query
// stmt - The PDO managed statement object.
// driver_options - User provided list of statement options.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_dbh_prepare( pdo_dbh_t *dbh, const char *sql,
                            long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    hash_auto_ptr pdo_stmt_options_ht;
    sqlsrv_malloc_auto_ptr<char> sql_rewrite;
    int sql_rewrite_len = 0;
    sqlsrv_malloc_auto_ptr<pdo_sqlsrv_stmt> driver_stmt;

    pdo_sqlsrv_dbh* driver_dbh = reinterpret_cast<pdo_sqlsrv_dbh*>( dbh->driver_data );
    SQLSRV_ASSERT(( driver_dbh != NULL ), "pdo_sqlsrv_dbh_prepare: dbh->driver_data was null");

    try {

        // assign the methods for the statement object.  This is necessary even if the 
        // statement fails so the user can retrieve the error information.
        stmt->methods = &pdo_sqlsrv_stmt_methods;
        stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;   // we support parameterized queries with ?, not names

        // Initialize the options array to be passed to the core layer
        ALLOC_HASHTABLE( pdo_stmt_options_ht );
        core::sqlsrv_zend_hash_init( *driver_dbh , pdo_stmt_options_ht, 3 /* # of buckets */, NULL /*hashfn*/, 
                                     ZVAL_PTR_DTOR, 0 /*persistent*/ TSRMLS_CC );
        
        // Either of g_henv_cp or g_henv_ncp can be used to propogate the error.
        validate_stmt_options( *driver_dbh, driver_options, pdo_stmt_options_ht TSRMLS_CC );

        driver_stmt = static_cast<pdo_sqlsrv_stmt*>( core_sqlsrv_create_stmt( driver_dbh, core::allocate_stmt<pdo_sqlsrv_stmt>,
                                                                              pdo_stmt_options_ht, PDO_STMT_OPTS, 
                                                                              pdo_sqlsrv_handle_stmt_error, stmt TSRMLS_CC ));

        // if the user didn't set anything in the prepare options, then set the buffer limit
        // to the value set on the connection.
        if( driver_stmt->buffered_query_limit== sqlsrv_buffered_result_set::BUFFERED_QUERY_LIMIT_INVALID ) {
            
             driver_stmt->buffered_query_limit = driver_dbh->client_buffer_max_size;
        }

        // if the user didn't set anything in the prepare options, then set the query timeout 
        // to the value set on the connection.
        if(( driver_stmt->query_timeout == QUERY_TIMEOUT_INVALID ) && ( driver_dbh->query_timeout != QUERY_TIMEOUT_INVALID )) {
            
            core_sqlsrv_set_query_timeout( driver_stmt, driver_dbh->query_timeout TSRMLS_CC );
        }

        // rewrite named parameters in the query to positional parameters if we aren't letting PDO do the
        // parameter substitution for us
        if( stmt->supports_placeholders != PDO_PLACEHOLDER_NONE ) {

            // rewrite the query to map named parameters to positional parameters.  We do this rather than use the ODBC named
            // parameters for consistency with the PDO MySQL and PDO ODBC drivers.
            int zr = pdo_subst_named_params( stmt, const_cast<char*>( sql ), sql_len, &sql_rewrite, &sql_rewrite_len TSRMLS_CC );
            CHECK_ZEND_ERROR( zr, driver_dbh, PDO_SQLSRV_ERROR_PARAM_PARSE ) {
                throw core::CoreException();
            }
            // if parameter substitution happened, use that query instead of the original
            if( !!sql_rewrite ) {
                sql = sql_rewrite;
                sql_len = sql_rewrite_len;
            }
        }

        if( !driver_stmt->direct_query && stmt->supports_placeholders != PDO_PLACEHOLDER_NONE ) {
  
            core_sqlsrv_prepare( driver_stmt, sql, sql_len TSRMLS_CC );
        }
        else if( driver_stmt->direct_query ) {

            if( driver_stmt->direct_query_subst_string ) {
                // we use efree rather than sqlsrv_free since sqlsrv_free may wrap another allocation scheme
                // and we use estrdup below to allocate the new string, which uses emalloc
                efree( reinterpret_cast<void*>( const_cast<char*>( driver_stmt->direct_query_subst_string )));
            }
            driver_stmt->direct_query_subst_string = estrdup( sql );
            driver_stmt->direct_query_subst_string_len = sql_len;
        }
        // else if stmt->support_placeholders == PDO_PLACEHOLDER_NONE means that stmt->active_query_string will be
        // set to the substituted query

   
        stmt->driver_data = driver_stmt;
        driver_stmt.transferred();                   
    }
    // everything is cleaned up by this point
    // catch everything so the exception doesn't spill into the calling PDO code
    catch( core::CoreException& ) {

		driver_stmt = NULL;
        /* if( driver_stmt ) {

            driver_stmt->~pdo_sqlsrv_stmt();
        } */

        // in the event that the statement caused an error that was copied to the connection, update the
        // connection with the error's SQLSTATE.
        if( driver_dbh->last_error() ) {

            strcpy_s( dbh->error_code, sizeof( dbh->error_code ), 
                      reinterpret_cast<const char*>( driver_dbh->last_error()->sqlstate ));
        }

        return 0;
    }

    // catch any errant exception and die
    catch(...) {

        DIE( "pdo_sqlsrv_dbh_prepare: Unknown exception caught." );
    }

    return 1;
}


// pdo_sqlsrv_dbh_do
// Maps to PDO::exec.
// Execute a SQL statement, such as an insert, update or delete, and return 
// the number of rows affected.
// Parameters:
// dbh - the PDO connection object, which contains the ODBC handle
// sql - the query to execute
// sql_len - length of sql query
// Return
// # of rows affected, -1 for an error.
long pdo_sqlsrv_dbh_do( pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    pdo_sqlsrv_dbh* driver_dbh = static_cast<pdo_sqlsrv_dbh*>( dbh->driver_data );

    sqlsrv_malloc_auto_ptr<sqlsrv_stmt> driver_stmt;
    SQLLEN rows = 0;

    // verify that the data type sizes are the same.  If we ever upgrade to 64 bit we don't want the wrong
    // thing to happen here.
    SQLSRV_STATIC_ASSERT( sizeof( rows ) == sizeof( SQLLEN ) );

    try {
 
        SQLSRV_ASSERT( sql != NULL, "NULL or empty SQL string passed." );

        // temp PDO statement used for error handling if something happens
        pdo_stmt_t temp_stmt;
        temp_stmt.dbh = dbh;
        // allocate a full driver statement to take advantage of the error handling
        driver_stmt = core_sqlsrv_create_stmt( driver_dbh, core::allocate_stmt<pdo_sqlsrv_stmt>, NULL /*options_ht*/, 
                          NULL /*valid_stmt_opts*/, pdo_sqlsrv_handle_stmt_error, &temp_stmt TSRMLS_CC );
        driver_stmt->set_func( __FUNCTION__ );

        core_sqlsrv_execute( driver_stmt TSRMLS_CC, sql, sql_len );

        // since the user can give us a compound statement, we return the row count for the last set, and since the row count
        // isn't guaranteed to be valid until all the results have been fetched, we fetch them all first.
        if( core_sqlsrv_has_any_result( driver_stmt TSRMLS_CC )) {

            SQLRETURN r = SQL_SUCCESS;

            do {
                
                rows = core::SQLRowCount( driver_stmt TSRMLS_CC );

                r = core::SQLMoreResults( driver_stmt TSRMLS_CC );
        
            } while( r != SQL_NO_DATA );
        }

        // returning -1 forces PDO to return false, which signals an error occurred.  SQLRowCount returns -1 for a number of cases
        // naturally, so we override that here with no rows returned.
        if( rows == -1 ) {
            rows = 0;
        }
    }
    catch( core::CoreException& ) {

        // copy any errors on the statement to the connection so that the user sees them, since the statement is released
        // before this method returns
        strcpy_s( dbh->error_code, sizeof( dbh->error_code ),
                  reinterpret_cast<const char*>( driver_stmt->last_error()->sqlstate ));
        driver_dbh->set_last_error( driver_stmt->last_error() );
        
        /* if( driver_stmt ) {
            driver_stmt->~sqlsrv_stmt();
        } */
        
        return -1;
    }
    catch( ... ) {

        DIE( "pdo_sqlsrv_dbh_do: Unknown exception caught." );
    }

    /* if( driver_stmt ) {
        driver_stmt->~sqlsrv_stmt();
    }*/

    return (long)rows;
}


// transaction support functions

// pdo_sqlsrv_dbh_begin
// Maps to PDO::beginTransaction.
// Begins a transaction. Turns off auto-commit mode. The pdo_dbh_t::in_txn 
// flag is maintained by PDO so we dont have to worry about it.
// Parameters:
// dbh - The PDO managed connection object.
// Return:
// 0 for failure and 1 for success.
int pdo_sqlsrv_dbh_begin( pdo_dbh_t *dbh TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    try {
     
        SQLSRV_ASSERT( dbh != NULL, "pdo_sqlsrv_dbh_begin: pdo_dbh_t object was null" );

        sqlsrv_conn* driver_conn = reinterpret_cast<sqlsrv_conn*>( dbh->driver_data );
            
        SQLSRV_ASSERT( driver_conn != NULL, "pdo_sqlsrv_dbh_begin: driver_data object was null" );

        DEBUG_SQLSRV_ASSERT( !dbh->in_txn, "pdo_sqlsrv_dbh_begin: Already in transaction" );

        core_sqlsrv_begin_transaction( driver_conn TSRMLS_CC );
    
        return 1;
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {
        
        DIE ("pdo_sqlsrv_dbh_begin: Uncaught exception occurred.");
    }
}



// pdo_sqlsrv_dbh_commit
// Maps to PDO::commit.
// Commits a transaction. Returns the connection to auto-commit mode.
// PDO throws error if PDO::commit is called on a connection that is not in an active
// transaction. The pdo_dbh_t::in_txn flag is maintained by PDO so we dont have 
// to worry about it here.
// Parameters:
// dbh - The PDO managed connection object.
// Return:
// 0 for failure and 1 for success.
int pdo_sqlsrv_dbh_commit( pdo_dbh_t *dbh TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    try {
        
        SQLSRV_ASSERT( dbh != NULL, "pdo_sqlsrv_dbh_commit: pdo_dbh_t object was null" );
        
        sqlsrv_conn* driver_conn = reinterpret_cast<sqlsrv_conn*>( dbh->driver_data );
            
        SQLSRV_ASSERT( driver_conn != NULL, "pdo_sqlsrv_dbh_commit: driver_data object was null" );
        
        DEBUG_SQLSRV_ASSERT( dbh->in_txn, "pdo_sqlsrv_dbh_commit: Not in transaction" );
        
        core_sqlsrv_commit( driver_conn TSRMLS_CC );
                
        return 1;
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {
        
        DIE ("pdo_sqlsrv_dbh_commit: Uncaught exception occurred.");
    }
}

// pdo_sqlsrv_dbh_rollback
// Maps to PDO::rollback.
// Rolls back a transaction. Returns the connection in auto-commit mode.
// PDO throws error if PDO::rollBack is called on a connection that is not in an active
// transaction. The pdo_dbh_t::in_txn flag is maintained by PDO so we dont have 
// to worry about it here.
// Parameters:
// dbh - The PDO managed connection object.
// Return:
// 0 for failure and 1 for success.
int pdo_sqlsrv_dbh_rollback( pdo_dbh_t *dbh TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    try {
        SQLSRV_ASSERT( dbh != NULL, "pdo_sqlsrv_dbh_rollback: pdo_dbh_t object was null" );
        
        sqlsrv_conn* driver_conn = reinterpret_cast<sqlsrv_conn*>( dbh->driver_data );
            
        SQLSRV_ASSERT( driver_conn != NULL, "pdo_sqlsrv_dbh_rollback: driver_data object was null" );
        
        DEBUG_SQLSRV_ASSERT( dbh->in_txn, "pdo_sqlsrv_dbh_rollback: Not in transaction" );
        
        core_sqlsrv_rollback( driver_conn TSRMLS_CC );
    
        return 1;
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {
        
        DIE ("pdo_sqlsrv_dbh_rollback: Uncaught exception occurred.");
    }
}

// pdo_sqlsrv_dbh_set_attr
// Maps to PDO::setAttribute. Sets an attribute on the PDO connection object.
// PDO driver manager calls this function directly after calling the factory
// method for PDO, for any attribute which is specified in the PDO constructor. 
// Parameters:
// dbh - The PDO connection object maintained by PDO.
// attr - The attribute to be set.
// val - The value of the attribute to be set.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_dbh_set_attr( pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    pdo_sqlsrv_dbh* driver_dbh = static_cast<pdo_sqlsrv_dbh*>( dbh->driver_data );

    try {

        switch( attr ) {

            case SQLSRV_ATTR_ENCODING:
            {
                long attr_value;
                if( Z_TYPE_P( val ) != IS_LONG ) {
                    THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_INVALID_ENCODING );
                }
                attr_value = Z_LVAL_P( val );
                switch( attr_value ) {

                    case SQLSRV_ENCODING_DEFAULT:
                        // when default is applied to a connection, that means use UTF-8 encoding
                        driver_dbh->set_encoding( SQLSRV_ENCODING_UTF8 );
                        break;
                    case SQLSRV_ENCODING_SYSTEM:
                    case SQLSRV_ENCODING_UTF8:
                        driver_dbh->set_encoding( static_cast<SQLSRV_ENCODING>( attr_value ));
                        break;
                    default:
                        THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_INVALID_ENCODING );
                        break;
                }
            }
            break;

            case SQLSRV_ATTR_DIRECT_QUERY:
                driver_dbh->direct_query = ( zend_is_true( val ) ) ? true : false;
                break;

            case SQLSRV_ATTR_QUERY_TIMEOUT:
                if( Z_TYPE_P( val ) != IS_LONG || Z_LVAL_P( val ) < 0 ) {
                    convert_to_string( val );
                    THROW_PDO_ERROR( driver_dbh, SQLSRV_ERROR_INVALID_QUERY_TIMEOUT_VALUE, Z_STRVAL_P( val ));
                }
                driver_dbh->query_timeout = Z_LVAL_P( val );
                break;

            case SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE:
                if( Z_TYPE_P( val ) != IS_LONG || Z_LVAL_P( val ) <= 0 ) {
                    convert_to_string( val );
                    THROW_PDO_ERROR( driver_dbh, SQLSRV_ERROR_INVALID_BUFFER_LIMIT, Z_STRVAL_P( val ));
                }
                driver_dbh->client_buffer_max_size = Z_LVAL_P( val );
                break;

            // Not supported
            case PDO_ATTR_FETCH_TABLE_NAMES: 
            case PDO_ATTR_FETCH_CATALOG_NAMES: 
            case PDO_ATTR_PREFETCH:
            case PDO_ATTR_MAX_COLUMN_LEN:
            case PDO_ATTR_CURSOR_NAME:	
            case PDO_ATTR_AUTOCOMMIT:
            case PDO_ATTR_PERSISTENT:
            case PDO_ATTR_TIMEOUT:
            {
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_UNSUPPORTED_DBH_ATTR );
            }

            // Read-only
            case PDO_ATTR_SERVER_VERSION:
            case PDO_ATTR_SERVER_INFO:	
            case PDO_ATTR_CLIENT_VERSION:
            case PDO_ATTR_DRIVER_NAME:
            case PDO_ATTR_CONNECTION_STATUS:	
            {
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_READ_ONLY_DBH_ATTR );
            }

            // Statement level only
            case PDO_ATTR_EMULATE_PREPARES:
            case PDO_ATTR_CURSOR:
            case SQLSRV_ATTR_CURSOR_SCROLL_TYPE:	
            {
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_STMT_LEVEL_ATTR );
            }

            default:
            {
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_INVALID_DBH_ATTR );
                break;
            }
        }
    }
    catch( pdo::PDOException& ) {

        return 0;
    }

    return 1;
}


// pdo_sqlsrv_dbh_get_attr
// Maps to PDO::getAttribute. Gets an attribute on the PDO connection object.
// Parameters:
// dbh - The PDO connection object maintained by PDO.
// attr - The attribute to get.
// return_value - zval in which to return the attribute value.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_dbh_get_attr( pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    pdo_sqlsrv_dbh* driver_dbh = static_cast<pdo_sqlsrv_dbh*>( dbh->driver_data );

    try {

        switch( attr ) {

            // Not supported
            case PDO_ATTR_FETCH_TABLE_NAMES: 
            case PDO_ATTR_FETCH_CATALOG_NAMES: 
            case PDO_ATTR_PREFETCH:
            case PDO_ATTR_MAX_COLUMN_LEN:
            case PDO_ATTR_CURSOR_NAME:
            case PDO_ATTR_AUTOCOMMIT:
            case PDO_ATTR_TIMEOUT:
            {
                // PDO does not throw "not supported" error message for these attributes. 
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_UNSUPPORTED_DBH_ATTR );
            }

             // Statement level only
            case PDO_ATTR_EMULATE_PREPARES:
            case PDO_ATTR_CURSOR:
            case SQLSRV_ATTR_CURSOR_SCROLL_TYPE:	
            {
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_STMT_LEVEL_ATTR );
            }
        
            case PDO_ATTR_STRINGIFY_FETCHES:
            {
                // For this attribute, if we dont set the return_value than PDO returns NULL.
                ZVAL_BOOL(return_value, ( dbh->stringify ? 1 : 0 ) );
                break;
            }

            case PDO_ATTR_SERVER_INFO:
            {
                core_sqlsrv_get_server_info( driver_dbh, return_value TSRMLS_CC );
                break;
            }
            
            case PDO_ATTR_SERVER_VERSION:
            {
                core_sqlsrv_get_server_version( driver_dbh, return_value TSRMLS_CC );
                break;
            }
            
            case PDO_ATTR_CLIENT_VERSION:
            {
                core_sqlsrv_get_client_info( driver_dbh, return_value TSRMLS_CC );

                //Add the PDO SQLSRV driver's file version 
                core::sqlsrv_add_assoc_string( *driver_dbh, return_value, "ExtensionVer", VER_FILEVERSION_STR, 1 /*duplicate*/
                                               TSRMLS_CC );
                break;
            }

            case SQLSRV_ATTR_ENCODING:
            { 
                ZVAL_LONG( return_value, driver_dbh->encoding() );
                break;
            }

            case SQLSRV_ATTR_QUERY_TIMEOUT:
            { 
                ZVAL_LONG( return_value, ( driver_dbh->query_timeout == QUERY_TIMEOUT_INVALID ? 0 : driver_dbh->query_timeout ));
                break;
            }

            case SQLSRV_ATTR_DIRECT_QUERY:
            {
                ZVAL_BOOL( return_value, driver_dbh->direct_query );
                break;
            }

            case SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE:
            { 
                ZVAL_LONG( return_value, driver_dbh->client_buffer_max_size );
                break;
            }

            default: 
            {
                THROW_PDO_ERROR( driver_dbh, PDO_SQLSRV_ERROR_INVALID_DBH_ATTR );
                break;
            }
        }

        return 1;
    }
    catch( core::CoreException& ) {
        return 0;
    }
}

// Called by PDO::errorInfo and PDOStatement::errorInfo.
// Returns the error info.
// Parameters:
// dbh - The PDO managed connection object.
// stmt - The PDO managed statement object.
// info - zval in which to return the error info.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_dbh_return_error( pdo_dbh_t *dbh, pdo_stmt_t *stmt,
                                 zval *info TSRMLS_DC)
{
    SQLSRV_ASSERT( dbh != NULL || stmt != NULL, "Either dbh or stmt must not be NULL to dereference the error." );

    sqlsrv_error* ctx_error = NULL;
    if( stmt ) {
        ctx_error = static_cast<sqlsrv_stmt*>( stmt->driver_data )->last_error();
    }
    else {
        ctx_error = static_cast<sqlsrv_conn*>( dbh->driver_data )->last_error();
    }
    
    pdo_sqlsrv_retrieve_context_error( ctx_error, info );

    return 1;
}

// pdo_sqlsrv_dbh_last_id
// Maps to PDO::lastInsertId.
// Returns the last id generated by an executed SQL statement
// Parameters:
// dbh  - The PDO managed connection object.
// name - Table name.
// len  - Length of the name.
// Return:
// Returns the last insert id as a string.
char * pdo_sqlsrv_dbh_last_id( pdo_dbh_t *dbh, const char *name, __out unsigned int* len TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

	SQLSRV_ASSERT(len != NULL, "pdo_sqlsrv_dbh_last_id invalid parameter: len");

    // turn off any error handling for last_id
    pdo_error_mode prev_err_mode = dbh->error_mode;
    dbh->error_mode = PDO_ERRMODE_SILENT;

    sqlsrv_malloc_auto_ptr<sqlsrv_stmt> driver_stmt;

    pdo_sqlsrv_dbh* driver_dbh = reinterpret_cast<pdo_sqlsrv_dbh*>( dbh->driver_data );

    sqlsrv_malloc_auto_ptr<char> id_str;
    id_str = reinterpret_cast<char*>( sqlsrv_malloc( LAST_INSERT_ID_BUFF_LEN ));

    try {

        char last_insert_id_query[ LAST_INSERT_ID_QUERY_MAX_LEN ];
        if( name == NULL ) {
            strcpy_s( last_insert_id_query, sizeof( last_insert_id_query ), LAST_INSERT_ID_QUERY );
        }
        else {
            char* quoted_table = NULL;
            int quoted_len = 0;
            int quoted = pdo_sqlsrv_dbh_quote( dbh, name, (int)strlen( name ), &quoted_table, &quoted_len, PDO_PARAM_NULL TSRMLS_CC );
            SQLSRV_ASSERT( quoted, "PDO::lastInsertId failed to quote the table name." );
            sprintf_s( last_insert_id_query, LAST_INSERT_ID_QUERY_MAX_LEN, TABLE_LAST_INSERT_ID_QUERY, quoted_table );
            sqlsrv_free( quoted_table );
        }

        // temp PDO statement used for error handling if something happens
        pdo_stmt_t temp_stmt;
        temp_stmt.dbh = dbh;

        // allocate a full driver statement to take advantage of the error handling
        driver_stmt = core_sqlsrv_create_stmt( driver_dbh, core::allocate_stmt<pdo_sqlsrv_stmt>, NULL /*options_ht*/, 
                          NULL /*valid_stmt_opts*/, pdo_sqlsrv_handle_stmt_error, &temp_stmt TSRMLS_CC );
        driver_stmt->set_func( __FUNCTION__ );

        // execute the last insert id query
        core::SQLExecDirect( driver_stmt, last_insert_id_query TSRMLS_CC );

        core::SQLFetchScroll( driver_stmt, SQL_FETCH_NEXT, 0 TSRMLS_CC );
		SQLLEN tmplen = (SQLLEN)*len;
        SQLRETURN r = core::SQLGetData( driver_stmt, 1, SQL_C_CHAR, id_str, LAST_INSERT_ID_BUFF_LEN, 
                                        &tmplen, false TSRMLS_CC );
		*len = (uint)tmplen;

        CHECK_CUSTOM_ERROR( (!SQL_SUCCEEDED( r ) || *len == SQL_NULL_DATA || *len == SQL_NO_TOTAL), driver_stmt,
                            PDO_SQLSRV_ERROR_LAST_INSERT_ID ) {
            throw core::CoreException();
        }

        /* driver_stmt->~sqlsrv_stmt(); */
		driver_stmt = NULL;
    }
    catch( core::CoreException& ) {

        // copy any errors on the statement to the connection so that the user sees them, since the statement is released
        // before this method returns
        strcpy_s( dbh->error_code, sizeof( dbh->error_code ),
                  reinterpret_cast<const char*>( driver_stmt->last_error()->sqlstate ));
        driver_dbh->set_last_error( driver_stmt->last_error() );
        
        /* if( driver_stmt ) {
            driver_stmt->~sqlsrv_stmt();
        } */

        strcpy_s( id_str.get(), 1, "" );
        *len = 0;
    }

    char* ret_id_str = id_str.get();
    id_str.transferred();

    // restore error handling to its previous mode
    dbh->error_mode = prev_err_mode;

    return ret_id_str;
}

// pdo_sqlsrv_dbh_quote
// Maps to PDO::quote. As the name says, this function quotes a string.
// Always returns a valid string unless memory allocation fails.
// Parameters:
// dbh          - The PDO managed connection object.
// unquoted     - The unquoted string to be quoted.
// unquoted_len - Length of the unquoted string.
// quoted       - Buffer for output string.
// quoted_len   - Length of the output string.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_dbh_quote( pdo_dbh_t* dbh, const char* unquoted, int unquoted_len, char **quoted, int* quoted_len, 
                          enum pdo_param_type /*paramtype*/ TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    // count the number of quotes needed
    unsigned int quotes_needed = 2;  // the initial start and end quotes of course
    for( int index = 0; index < unquoted_len && unquoted[ index ] != '\0'; ++index ) {
        if( unquoted[ index ] == '\'' ) {
            ++quotes_needed;
        }
    }

    *quoted_len = unquoted_len + quotes_needed;  // length returned to the caller should not account for null terminator.
    *quoted = reinterpret_cast<char*>( sqlsrv_malloc( *quoted_len, sizeof( char ), 1 )); // include space for null terminator. 
    unsigned int out_current = 0;

    // insert initial quote
    (*quoted)[ out_current++ ] ='\'';

    for( int index = 0; index < unquoted_len && unquoted[ index ] != '\0'; ++index ) {

        if( unquoted[ index ] == '\'' ) {
            (*quoted)[ out_current++ ] = '\'';
            (*quoted)[ out_current++ ] = '\'';
        }
        else {
            (*quoted)[ out_current++ ] = unquoted[ index ];
        }
    }

    // trailing quote and null terminator
    (*quoted)[ out_current++ ] ='\'';
    (*quoted)[ out_current ] = '\0';

    return 1;
}

// This method is not implemented by this driver.
pdo_sqlsrv_function_entry *pdo_sqlsrv_get_driver_methods( pdo_dbh_t *dbh, int kind TSRMLS_DC )
{
    PDO_RESET_DBH_ERROR;
    PDO_VALIDATE_CONN;
    PDO_LOG_DBH_ENTRY;

    sqlsrv_conn* driver_conn = reinterpret_cast<sqlsrv_conn*>( dbh->driver_data );
    CHECK_CUSTOM_ERROR( true, driver_conn, PDO_SQLSRV_ERROR_FUNCTION_NOT_IMPLEMENTED ) {
        return NULL;
    }

    return NULL;	// to avoid a compiler warning
}

namespace {

// Maps the PDO driver specific statement option/attribute constants to the core layer 
// statement option/attribute constants.
void add_stmt_option_key( sqlsrv_context& ctx, unsigned long key, HashTable* options_ht, 
                         zval** data TSRMLS_DC )
{
     unsigned long option_key = -1;
     switch( key ) {
  
         case PDO_ATTR_CURSOR:
             option_key = SQLSRV_STMT_OPTION_SCROLLABLE;
             break;
            
         case SQLSRV_ATTR_ENCODING:
             option_key = PDO_STMT_OPTION_ENCODING;
             break;

         case SQLSRV_ATTR_QUERY_TIMEOUT:
             option_key = SQLSRV_STMT_OPTION_QUERY_TIMEOUT;
             break;

         case PDO_ATTR_STATEMENT_CLASS:
             break;

         case SQLSRV_ATTR_DIRECT_QUERY:
             option_key = PDO_STMT_OPTION_DIRECT_QUERY;
             break;

         case SQLSRV_ATTR_CURSOR_SCROLL_TYPE:
             option_key = PDO_STMT_OPTION_CURSOR_SCROLL_TYPE;
             break;

         case SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE:
             option_key = PDO_STMT_OPTION_CLIENT_BUFFER_MAX_KB_SIZE;
             break;

         case PDO_ATTR_EMULATE_PREPARES:
             option_key = PDO_STMT_OPTION_EMULATE_PREPARES;
             break;

         default:
             CHECK_CUSTOM_ERROR( true, ctx, PDO_SQLSRV_ERROR_INVALID_STMT_OPTION ) {
                 throw core::CoreException();
             }
             break;
    }

    // if a PDO handled option makes it through (such as PDO_ATTR_STATEMENT_CLASS, just skip it
    if( option_key != -1 ) {
        zval_add_ref( data );
        core::sqlsrv_zend_hash_index_update(ctx, options_ht, option_key, (void**)data, sizeof(zval*) TSRMLS_CC );
    }
}


// validate_stmt_options
// Iterates through the list of statement options provided by the user and validates them 
// against the list of statement options provided by this driver. After validation
// creates a Hashtable of statement options to be sent to the core layer for processing.
// Parameters:
// ctx - The current context.
// stmt_options - The user provided list of statement options.
// pdo_stmt_options_ht - Output hashtable of statement options. 
void validate_stmt_options( sqlsrv_context& ctx, zval* stmt_options, __inout HashTable* pdo_stmt_options_ht TSRMLS_DC )
{
    try {
        
        if( stmt_options ) {
           
            HashTable* options_ht = Z_ARRVAL_P( stmt_options );
    
            for( zend_hash_internal_pointer_reset( options_ht ); zend_hash_has_more_elements( options_ht ) == SUCCESS;
                 zend_hash_move_forward( options_ht )) {

                int type = HASH_KEY_NON_EXISTANT;
                char *key = NULL;
                unsigned int key_len = 0;
                unsigned long int_key = -1;
                zval** data;
                int result = 0;

                type = zend_hash_get_current_key_ex( options_ht, &key, &key_len, &int_key, 0, NULL );
                CHECK_CUSTOM_ERROR(( type != HASH_KEY_IS_LONG ), ctx, PDO_SQLSRV_ERROR_INVALID_STMT_OPTION ) {
                    throw core::CoreException();
                }  
                
                core::sqlsrv_zend_hash_get_current_data( ctx, options_ht, (void**) &data TSRMLS_CC );    
                
                add_stmt_option_key( ctx, int_key, pdo_stmt_options_ht, data TSRMLS_CC );
            } 
        }
    }
    catch( core::CoreException& ) {

        throw;
    }
}


void pdo_bool_conn_str_func::func(connection_option const* option, zval* value, sqlsrv_conn* /*conn*/, std::string& conn_str TSRMLS_DC )
{
    TSRMLS_C;
    char const* val_str = "no";
   
    if( core_str_zval_is_true( value ) ) {
        
        val_str = "yes";
    }
     
    conn_str += option->odbc_name;
    conn_str += "={";
    conn_str += val_str;
    conn_str += "};";
}

void pdo_txn_isolation_conn_attr_func::func( connection_option const* /*option*/, zval* value_z, sqlsrv_conn* conn, 
                                             std::string& /*conn_str*/ TSRMLS_DC )
{
    try {
    
        SQLSRV_ASSERT( Z_TYPE_P( value_z ) == IS_STRING, "pdo_txn_isolation_conn_attr_func: Unexpected zval type." );
        const char* val = Z_STRVAL_P( value_z );
        int val_len = Z_STRLEN_P( value_z );
        long out_val = SQL_TXN_READ_COMMITTED;

        // READ_COMMITTED
        if(( val_len == ( sizeof( PDOTxnIsolationValues::READ_COMMITTED ) - 1 ) 
             && !_stricmp( val, PDOTxnIsolationValues::READ_COMMITTED ))) {
            
            out_val = SQL_TXN_READ_COMMITTED;    
        }

        // READ_UNCOMMITTED
        else if(( val_len == ( sizeof( PDOTxnIsolationValues::READ_UNCOMMITTED ) - 1 ) 
            && !_stricmp( val, PDOTxnIsolationValues::READ_UNCOMMITTED ))) {
        
            out_val = SQL_TXN_READ_UNCOMMITTED;
        }

        // REPEATABLE_READ
        else if(( val_len == ( sizeof( PDOTxnIsolationValues::REPEATABLE_READ ) - 1 ) 
            && !_stricmp( val, PDOTxnIsolationValues::REPEATABLE_READ ))) {
        
            out_val = SQL_TXN_REPEATABLE_READ;
        }
        
        // SERIALIZABLE
        else if(( val_len == ( sizeof( PDOTxnIsolationValues::SERIALIZABLE ) - 1 ) 
            && !_stricmp( val, PDOTxnIsolationValues::SERIALIZABLE ))) {
        
            out_val = SQL_TXN_SERIALIZABLE;
        }

        // SNAPSHOT
        else if(( val_len == ( sizeof( PDOTxnIsolationValues::SNAPSHOT ) - 1 ) 
            && !_stricmp( val, PDOTxnIsolationValues::SNAPSHOT ))) {
        
            out_val = SQL_TXN_SS_SNAPSHOT;
        }
        
        else {
         
            CHECK_CUSTOM_ERROR( true, conn, PDO_SQLSRV_ERROR_INVALID_DSN_VALUE, PDOConnOptionNames::TransactionIsolation ) {

                throw core::CoreException();
            }
        }
        
        core::SQLSetConnectAttr( conn, SQL_COPT_SS_TXN_ISOLATION, reinterpret_cast<SQLPOINTER>( out_val ), SQL_IS_UINTEGER TSRMLS_CC );

    }
    catch( core::CoreException& ) {

        throw;
    }
}

}       // namespace
