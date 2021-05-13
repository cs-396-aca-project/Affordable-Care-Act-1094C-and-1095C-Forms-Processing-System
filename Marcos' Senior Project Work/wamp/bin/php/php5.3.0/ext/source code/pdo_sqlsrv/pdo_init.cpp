//---------------------------------------------------------------------------------------------------------------------------------
// File: pdo_init.cpp
//
// Contents: initialization routines for PDO_SQLSRV
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

ZEND_GET_MODULE(g_pdo_sqlsrv)

extern "C" {

ZEND_DECLARE_MODULE_GLOBALS(pdo_sqlsrv);

}

// module global variables (initialized in minit and freed in mshutdown)
HashTable* g_pdo_errors_ht = NULL;

// henv context for creating connections
sqlsrv_context* g_henv_cp;
sqlsrv_context* g_henv_ncp;

namespace {

pdo_driver_t pdo_sqlsrv_driver = {

    PDO_DRIVER_HEADER(sqlsrv),
    pdo_sqlsrv_db_handle_factory
};

const char PDO_DLL_NAME[] = "php_pdo.dll";

// PHP_DEBUG is always defined as either 0 or 1
#if PHP_DEBUG == 1 && !defined(ZTS)

const char PHP_DLL_NAME[] = "php5_debug.dll";

#elif PHP_DEBUG == 1 && defined(ZTS)

const char PHP_DLL_NAME[] = "php5ts_debug.dll";

#elif PHP_DEBUG == 0 && !defined(ZTS)

const char PHP_DLL_NAME[] = "php5.dll";

#elif PHP_DEBUG == 0 && defined(ZTS)

const char PHP_DLL_NAME[] = "php5ts.dll";

#else

#error Invalid combination of PHP_DEBUG and ZTS macros

#endif

typedef PDO_API int (*pdo_register_func)(pdo_driver_t *);

// function pointers to register and unregister this driver
pdo_register_func pdo_register_driver;
pdo_register_func pdo_unregister_driver;

// functions to register SQLSRV constants with the PDO class
// (It's in all CAPS so it looks like the Zend macros that do similar work)
void REGISTER_PDO_SQLSRV_CLASS_CONST_LONG( char const* name, long value TSRMLS_DC );
void REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( char const* name, char const* value TSRMLS_DC );

// return the Zend class entry for the PDO dbh (connection) class
zend_class_entry* (*pdo_get_dbh_class)( void );

struct sqlsrv_attr_pdo_constant {
    const char *name;
    int value;
};

// forward decl for table
extern sqlsrv_attr_pdo_constant pdo_attr_constants[];

}

static zend_module_dep pdo_sqlsrv_depends[] = {
	ZEND_MOD_REQUIRED("pdo")
	{NULL, NULL, NULL}
};


// argument info structures for functions, arranged alphabetically.
// see zend_API.h in the PHP sources for more information about these macros

// function table with associated arginfo structures
zend_function_entry pdo_sqlsrv_functions[] = {
    {NULL, NULL, NULL}   // no functions directly defined by this driver
};

// the structure returned to Zend that exposes the extension to the Zend engine.
// this structure is defined in zend_modules.h in the PHP sources

zend_module_entry g_pdo_sqlsrv_module_entry = 
{
    STANDARD_MODULE_HEADER_EX,
    NULL,
	pdo_sqlsrv_depends,
    "pdo_sqlsrv", 
    pdo_sqlsrv_functions,           // exported function table
    // initialization and shutdown functions
    PHP_MINIT(pdo_sqlsrv),
    PHP_MSHUTDOWN(pdo_sqlsrv), 
    PHP_RINIT(pdo_sqlsrv), 
    PHP_RSHUTDOWN(pdo_sqlsrv), 
    PHP_MINFO(pdo_sqlsrv),
    // version of the extension.  Matches the version resource of the extension dll
    VER_FILEVERSION_STR,
    PHP_MODULE_GLOBALS(pdo_sqlsrv),
    NULL,           
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};

// functions dynamically linked from the PDO (or PHP) dll and called by other parts of the driver
zend_class_entry* (*pdo_get_exception_class)( void );
int (*pdo_subst_named_params)(pdo_stmt_t *stmt, char *inquery, int inquery_len, 
                              char **outquery, int *outquery_len TSRMLS_DC);

// Module initialization
// This function is called once per execution of the Zend engine

PHP_MINIT_FUNCTION(pdo_sqlsrv)
{
    SQLSRV_UNUSED( type );

    // our global variables are initialized in the RINIT function
#if defined(ZTS)
    if( ts_allocate_id( &pdo_sqlsrv_globals_id,
                    sizeof( zend_pdo_sqlsrv_globals ),
                    (ts_allocate_ctor) NULL,
                    (ts_allocate_dtor) NULL ) == 0 )
        return FAILURE;
#endif

    core_sqlsrv_register_logger( pdo_sqlsrv_log );

    REGISTER_INI_ENTRIES();
    
    LOG( SEV_NOTICE, "pdo_sqlsrv: entering minit" );

    // PHP extensions may be either external DLLs loaded by PHP or statically compiled into the PHP dll
    // This becomes an issue when we are dependent on other extensions, e.g. PDO.  Normally this is solved
    // by the build process by linking our extension to the appropriate import library (either php*.dll or php_pdo.dll)
    // However, this leaves us with a problem that the extension has a dependency on that build type.
    // Since we don't distribute our extension with PHP directly (yet), it would mean that we would have to have SKUs
    // for both types of PDO builds, internal and external.  Rather than this, we just dynamically link the PDO routines we call
    // against either the PDO dll if it exists and is loaded, otherwise against the PHP dll directly.

    HMODULE pdo_hmodule = GetModuleHandle( PDO_DLL_NAME );
    if( pdo_hmodule == 0 ) {

        pdo_hmodule = GetModuleHandle( PHP_DLL_NAME );
        if( pdo_hmodule == NULL ) {
            LOG( SEV_ERROR, "Failed to get PHP module handle." );
            return FAILURE;
        }
    }

    pdo_register_driver = reinterpret_cast<pdo_register_func>( GetProcAddress( pdo_hmodule, "php_pdo_register_driver" ));
    if( pdo_register_driver == NULL ) {
        LOG( SEV_ERROR, "Failed to register driver." );
        return FAILURE;
    }

    pdo_unregister_driver = reinterpret_cast<pdo_register_func>( GetProcAddress( pdo_hmodule, "php_pdo_unregister_driver" ));
    if( pdo_unregister_driver == NULL ) {
        LOG( SEV_ERROR, "Failed to register driver." );
        return FAILURE;
    }

    pdo_get_exception_class = reinterpret_cast<zend_class_entry* (*)(void)>( GetProcAddress( pdo_hmodule, 
                                                                                             "php_pdo_get_exception" ));
    if( pdo_get_exception_class == NULL ) {
        LOG( SEV_ERROR, "Failed to register driver." );
        return FAILURE;
    }

    pdo_get_dbh_class = reinterpret_cast<zend_class_entry* (*)(void)>( GetProcAddress( pdo_hmodule, "php_pdo_get_dbh_ce" ));
    if( pdo_get_dbh_class == NULL ) {
        LOG( SEV_ERROR, "Failed to register driver." );
        return FAILURE;
    }

    pdo_subst_named_params = 
        reinterpret_cast<int (*)(pdo_stmt_t *stmt, char *inquery, int inquery_len, 
                                 char **outquery, int *outquery_len TSRMLS_DC)>( 
                                     GetProcAddress( pdo_hmodule, "pdo_parse_params" ));
    if( pdo_subst_named_params == NULL ) {
        LOG( SEV_ERROR, "Failed to register driver." );
        return FAILURE;
    }

    // initialize list of pdo errors
    g_pdo_errors_ht = reinterpret_cast<HashTable*>( pemalloc( sizeof( HashTable ), 1 ));
    int zr = ::zend_hash_init( g_pdo_errors_ht, 50, NULL, NULL, 1 );
    if( zr == FAILURE ) {
        LOG( SEV_ERROR, "Failed to initialize the PDO errors hashtable." );
        return FAILURE;
    }

    for( int i = 0; PDO_ERRORS[ i ].error_code != -1; ++i ) {
        
        zr = ::zend_hash_index_update( g_pdo_errors_ht, PDO_ERRORS[ i ].error_code, 
                                       &( PDO_ERRORS[ i ].sqlsrv_error ), sizeof( PDO_ERRORS[ i ].sqlsrv_error ), NULL );
        if( zr == FAILURE ) {
                
            LOG( SEV_ERROR, "Failed to insert data into PDO errors hashtable." );
            return FAILURE;
        }
    }

    try {

    // register all attributes supported by this driver.
    for( int i= 0; pdo_attr_constants[i].name != NULL; ++i ) {
        
        REGISTER_PDO_SQLSRV_CLASS_CONST_LONG( pdo_attr_constants[i].name, pdo_attr_constants[i].value TSRMLS_CC );
    
    }

    REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( "SQLSRV_TXN_READ_UNCOMMITTED", PDOTxnIsolationValues::READ_UNCOMMITTED  TSRMLS_CC );
    REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( "SQLSRV_TXN_READ_COMMITTED", PDOTxnIsolationValues::READ_COMMITTED TSRMLS_CC );
    REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( "SQLSRV_TXN_REPEATABLE_READ", PDOTxnIsolationValues::REPEATABLE_READ TSRMLS_CC );
    REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( "SQLSRV_TXN_SERIALIZABLE", PDOTxnIsolationValues::SERIALIZABLE TSRMLS_CC );
    REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( "SQLSRV_TXN_SNAPSHOT", PDOTxnIsolationValues::SNAPSHOT TSRMLS_CC );

    // retrieve the handles for the environments
    core_sqlsrv_minit( &g_henv_cp, &g_henv_ncp, pdo_sqlsrv_handle_env_error, "PHP_MINIT_FUNCTION for pdo_sqlsrv" TSRMLS_CC );

    }
    catch( ... ) {

        return FAILURE;
    }

    pdo_register_driver( &pdo_sqlsrv_driver );

    return SUCCESS;
}


// Module shutdown function

// Module shutdown function
// This function is called once per execution of the Zend engine

PHP_MSHUTDOWN_FUNCTION(pdo_sqlsrv)
{
    try {

        SQLSRV_UNUSED( type );

        UNREGISTER_INI_ENTRIES();

        pdo_unregister_driver( &pdo_sqlsrv_driver );

        // clean up the list of pdo errors
        zend_hash_destroy( g_pdo_errors_ht );
        pefree( g_pdo_errors_ht, 1 /*persistent*/ );

        core_sqlsrv_mshutdown( *g_henv_cp, *g_henv_ncp );

    }
    catch( ... ) {

        LOG( SEV_NOTICE, "Unknown exception caught in PHP_MSHUTDOWN_FUNCTION(pdo_sqlsrv)" );
        return FAILURE;
    }

    return SUCCESS;
}


// Request initialization function
// This function is called once per PHP script execution

PHP_RINIT_FUNCTION(pdo_sqlsrv)
{
    SQLSRV_UNUSED( module_number );
    SQLSRV_UNUSED( type );

    LOG( SEV_NOTICE, "pdo_sqlsrv: entering rinit" );
 
    // verify memory at the end of the request (in debug mode only)
    full_mem_check(MEMCHECK_SILENT);
    return SUCCESS;
}


// Request shutdown
// Called at the end of a script's execution

PHP_RSHUTDOWN_FUNCTION(pdo_sqlsrv)
{
    SQLSRV_UNUSED( module_number );
    SQLSRV_UNUSED( type );

    LOG( SEV_NOTICE, "pdo_sqlsrv: entering rshutdown" );

    // verify memory at the end of the request (in debug mode only)
    full_mem_check(MEMCHECK_SILENT);

    return SUCCESS;
}

// Called for php_info();  
// Displays the INI settings registered and their current values

PHP_MINFO_FUNCTION(pdo_sqlsrv)
{
#if defined(ZTS)
    SQLSRV_UNUSED( tsrm_ls );
#endif

    php_info_print_table_start();
    php_info_print_table_header(2, "pdo_sqlsrv support", "enabled");
    php_info_print_table_row(2, "Version", VER_FILEVERSION_STR " (Unofficial)");
    php_info_print_table_row(2, "Unofficial Changes", "Works with Native Clients 11/10/9");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

// *** internal init functions ***

namespace {

    // mimic the functionality of the REGISTER_PDO_CLASS_CONST_LONG.  We use this instead of the macro because
    // we dynamically link the pdo_get_dbh_class function rather than use the static php_pdo_get_dbh_ce (see MINIT)

    void REGISTER_PDO_SQLSRV_CLASS_CONST_LONG( char const* name, long value TSRMLS_DC )
    {
        zend_class_entry* zend_class = pdo_get_dbh_class();
        SQLSRV_ASSERT( zend_class != NULL, "REGISTER_PDO_SQLSRV_CLASS_CONST_LONG: pdo_get_dbh_class failed" );
        int zr = zend_declare_class_constant_long( zend_class, const_cast<char*>( name ), strlen( name ), value TSRMLS_CC );
        if( zr == FAILURE ) {
            throw core::CoreException(); 
        }
    }

    void REGISTER_PDO_SQLSRV_CLASS_CONST_STRING( char const* name, char const* value TSRMLS_DC )
    {
        zend_class_entry* zend_class = pdo_get_dbh_class();
        SQLSRV_ASSERT( zend_class != NULL, "REGISTER_PDO_SQLSRV_CLASS_CONST_STRING: pdo_get_dbh_class failed" );
        int zr = zend_declare_class_constant_string( zend_class, const_cast<char*>( name ), strlen( name ), const_cast<char*>( value ) TSRMLS_CC );
        if( zr == FAILURE ) {

            throw core::CoreException();
        }
    }

    // array of pdo constants.
    sqlsrv_attr_pdo_constant pdo_attr_constants[] = {

        // driver specific attributes
        { "SQLSRV_ATTR_ENCODING"            , SQLSRV_ATTR_ENCODING },
        { "SQLSRV_ATTR_QUERY_TIMEOUT"       , SQLSRV_ATTR_QUERY_TIMEOUT },
        { "SQLSRV_ATTR_DIRECT_QUERY"        , SQLSRV_ATTR_DIRECT_QUERY },
        { "SQLSRV_ATTR_CURSOR_SCROLL_TYPE"  , SQLSRV_ATTR_CURSOR_SCROLL_TYPE },
        { "SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE", SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE },

        // used for the size for output parameters: PDO::PARAM_INT and PDO::PARAM_BOOL use the default size of int,
        // PDO::PARAM_STR uses the size of the string in the variable
        { "SQLSRV_PARAM_OUT_DEFAULT_SIZE"   , -1 },

        // encoding attributes
        { "SQLSRV_ENCODING_DEFAULT"         , SQLSRV_ENCODING_DEFAULT },
        { "SQLSRV_ENCODING_SYSTEM"          , SQLSRV_ENCODING_SYSTEM },
        { "SQLSRV_ENCODING_BINARY"          , SQLSRV_ENCODING_BINARY },
        { "SQLSRV_ENCODING_UTF8"            , SQLSRV_ENCODING_UTF8 },

        // cursor types (can be assigned to SQLSRV_ATTR_CURSOR_SCROLL_TYPE
        { "SQLSRV_CURSOR_STATIC"            , SQL_CURSOR_STATIC },
        { "SQLSRV_CURSOR_DYNAMIC"           , SQL_CURSOR_DYNAMIC },
        { "SQLSRV_CURSOR_KEYSET"            , SQL_CURSOR_KEYSET_DRIVEN },
        { "SQLSRV_CURSOR_BUFFERED"          , SQLSRV_CURSOR_BUFFERED },

        { NULL , 0 } // terminate the table
    };
}
