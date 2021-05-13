//---------------------------------------------------------------------------------------------------------------------------------
// File: pdo_stmt.cpp
//
// Contents: Implements the PDOStatement object for the PDO_SQLSRV
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

// *** internal variables and constants ***

namespace {

// Maps to the list of PDO::FETCH_ORI_* constants
SQLSMALLINT odbc_fetch_orientation[] =
{
    SQL_FETCH_NEXT,         // PDO_FETCH_ORI_NEXT
    SQL_FETCH_PRIOR,        // PDO_FETCH_ORI_PRIOR
    SQL_FETCH_FIRST,        // PDO_FETCH_ORI_FIRST
    SQL_FETCH_LAST,         // PDO_FETCH_ORI_LAST
    SQL_FETCH_ABSOLUTE,     // PDO_FETCH_ORI_ABS
    SQL_FETCH_RELATIVE      // PDO_FETCH_ORI_REL
};

// max length of a field type
const int SQL_SERVER_IDENT_SIZE_MAX = 128;

inline SQLSMALLINT pdo_fetch_ori_to_odbc_fetch_ori (enum pdo_fetch_orientation ori)
{
    SQLSRV_ASSERT( ori >= PDO_FETCH_ORI_NEXT && ori <= PDO_FETCH_ORI_REL, "Fetch orientation out of range." );
    OACR_WARNING_SUPPRESS( 26001, "Buffer length verified above" );
    OACR_WARNING_SUPPRESS( 26000, "Buffer length verified above" );
    return odbc_fetch_orientation[ori];
}

// Returns SQLSRV data type for a given PDO type. See pdo_param_type
// for list of supported pdo types.
SQLSRV_PHPTYPE pdo_type_to_sqlsrv_php_type( sqlsrv_stmt* driver_stmt, enum pdo_param_type pdo_type TSRMLS_DC )
{
    switch( pdo_type ) {

        case PDO_PARAM_BOOL:
        case PDO_PARAM_INT:
            return SQLSRV_PHPTYPE_INT;

        case PDO_PARAM_STR:
            return SQLSRV_PHPTYPE_STRING;
        
        case PDO_PARAM_NULL:
            return SQLSRV_PHPTYPE_NULL;
        
        case PDO_PARAM_LOB:
            // TODO: This will eventually be changed to SQLSRV_PHPTYPE_STREAM when output streaming is implemented.
            return SQLSRV_PHPTYPE_STRING;

        case PDO_PARAM_STMT:
            THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_PDO_STMT_UNSUPPORTED );
            break;

        default:
            DIE( "pdo_type_to_sqlsrv_php_type: Unexpected pdo_param_type encountered" );
    }
    
    return SQLSRV_PHPTYPE_INVALID; // to prevent compiler warning 
}

// Returns a pdo type for a given SQL type. See pdo_param_type
// for list of supported pdo types.
inline pdo_param_type sql_type_to_pdo_type( SQLSMALLINT sql_type )
{
    pdo_param_type return_type = PDO_PARAM_STR;

    switch( sql_type ) {

        case SQL_BIT:
        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:
        case SQL_BIGINT:
        case SQL_BINARY:
        case SQL_CHAR:
        case SQL_DECIMAL:
        case SQL_DOUBLE: 
        case SQL_FLOAT:
        case SQL_GUID:
        case SQL_LONGVARBINARY:
        case SQL_LONGVARCHAR:
        case SQL_NUMERIC:
        case SQL_REAL:
        case SQL_SS_TIME2:
        case SQL_SS_TIMESTAMPOFFSET:
        case SQL_SS_UDT:
        case SQL_SS_VARIANT:
        case SQL_SS_XML:
        case SQL_TYPE_DATE:
        case SQL_TYPE_TIMESTAMP:
        case SQL_VARBINARY:
        case SQL_VARCHAR:
        case SQL_WCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_WVARCHAR:
            return_type = PDO_PARAM_STR;
            break;

        default: {
            DIE( "sql_type_to_pdo_type: Invalid SQL type provided." );
            break;
        }
    }

    return return_type;
}

// Calls core_sqlsrv_set_scrollable function to set cursor.
// PDO supports two cursor types: PDO_CURSOR_FWDONLY, PDO_CURSOR_SCROLL.
void set_stmt_cursors( sqlsrv_stmt* stmt, zval* value_z TSRMLS_DC )
{
    if( Z_TYPE_P( value_z ) != IS_LONG ) {

        THROW_PDO_ERROR( stmt, PDO_SQLSRV_ERROR_INVALID_CURSOR_TYPE );
    }

    long pdo_cursor_type = Z_LVAL_P( value_z );
    int odbc_cursor_type = -1;

    switch( pdo_cursor_type ) {

        case PDO_CURSOR_FWDONLY:
            odbc_cursor_type = SQL_CURSOR_FORWARD_ONLY;
            break;

        case PDO_CURSOR_SCROLL:
            odbc_cursor_type = SQL_CURSOR_STATIC;
            break;

        default:
            THROW_PDO_ERROR( stmt, PDO_SQLSRV_ERROR_INVALID_CURSOR_TYPE );
    }   

    core_sqlsrv_set_scrollable( stmt, odbc_cursor_type TSRMLS_CC ); 
}

void set_stmt_cursor_scroll_type( sqlsrv_stmt* stmt, zval* value_z TSRMLS_DC )
{
    if( Z_TYPE_P( value_z ) != IS_LONG ) {

        THROW_PDO_ERROR( stmt, PDO_SQLSRV_ERROR_INVALID_CURSOR_TYPE );
    }

    if( stmt->cursor_type == SQL_CURSOR_FORWARD_ONLY ) {

        THROW_PDO_ERROR( stmt, PDO_SQLSRV_ERROR_INVALID_CURSOR_WITH_SCROLL_TYPE );
    }

    long odbc_cursor_type = Z_LVAL_P( value_z );

    core_sqlsrv_set_scrollable( stmt, odbc_cursor_type TSRMLS_CC ); 

    return;
}

// Sets the statement encoding. Default encoding on the statement 
// implies use the connection's encoding.
void set_stmt_encoding( sqlsrv_stmt* stmt, zval* value_z TSRMLS_DC )
{
    // validate the value
    if( Z_TYPE_P( value_z ) != IS_LONG ) {

        THROW_PDO_ERROR( stmt, PDO_SQLSRV_ERROR_INVALID_ENCODING );
    }
        
    long attr_value = Z_LVAL_P( value_z );
    
    switch( attr_value ) {

        // when the default encoding is applied to a statement, it means use the creating connection's encoding
        case SQLSRV_ENCODING_DEFAULT:
        case SQLSRV_ENCODING_BINARY:
        case SQLSRV_ENCODING_SYSTEM:
        case SQLSRV_ENCODING_UTF8:
            stmt->set_encoding( static_cast<SQLSRV_ENCODING>( attr_value ));
            break;

        default:
            THROW_PDO_ERROR( stmt, PDO_SQLSRV_ERROR_INVALID_ENCODING );
            break;
    }
}

// internal helper function to free meta data structures allocated
void meta_data_free( field_meta_data* meta )
{
    sqlsrv_free( meta );
}

zval* convert_to_zval( SQLSRV_PHPTYPE sqlsrv_php_type, void** in_val, SQLLEN field_len )
{
    zval* out_zval = NULL;

    switch( sqlsrv_php_type ) {
       
        case SQLSRV_PHPTYPE_INT:
        case SQLSRV_PHPTYPE_FLOAT:       
        {
            ALLOC_INIT_ZVAL( out_zval );
            if( *in_val == NULL ) {
                ZVAL_NULL( out_zval );
            }
            else {

                if( sqlsrv_php_type == SQLSRV_PHPTYPE_INT ) {
                    ZVAL_LONG( out_zval, **( reinterpret_cast<long**>( in_val )));
                }
                else {
                    ZVAL_DOUBLE( out_zval, **( reinterpret_cast<double**>( in_val )));    
                }
            }

            if( *in_val ) {
                sqlsrv_free( *in_val );
            }

            break;
        }

        case SQLSRV_PHPTYPE_STRING:
        case SQLSRV_PHPTYPE_STREAM:     // TODO: this will be moved when output streaming is implemented
         {
            ALLOC_INIT_ZVAL( out_zval );

            if( *in_val == NULL ) {

                ZVAL_NULL( out_zval );
            }
            else {

                ZVAL_STRINGL( out_zval, reinterpret_cast<char*>( *in_val ), field_len, 0 /*duplicate*/ );
            }
            break;
        }
            
        case SQLSRV_PHPTYPE_DATETIME:
            DIE( "Unsupported php type" );
            out_zval = ( reinterpret_cast<zval*>( *in_val ));
            break;

        case SQLSRV_PHPTYPE_NULL:
            ALLOC_INIT_ZVAL( out_zval );
            ZVAL_NULL( out_zval );
            break;

        default:
            DIE( "Unknown php type" );
            break;
    }

    return out_zval;
}

}       // namespace 

int pdo_sqlsrv_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC);
int pdo_sqlsrv_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC);
int pdo_sqlsrv_stmt_fetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori,
                          long offset TSRMLS_DC);
int pdo_sqlsrv_stmt_param_hook(pdo_stmt_t *stmt,
                               struct pdo_bound_param_data *param, enum pdo_param_event event_type TSRMLS_DC);
int pdo_sqlsrv_stmt_describe_col(pdo_stmt_t *stmt, int colno TSRMLS_DC);
int pdo_sqlsrv_stmt_get_col_data(pdo_stmt_t *stmt, int colno,
                                 char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC);
int pdo_sqlsrv_stmt_set_attr(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC);
int pdo_sqlsrv_stmt_get_attr(pdo_stmt_t *stmt, long attr, zval *return_value TSRMLS_DC);
int pdo_sqlsrv_stmt_get_col_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC);
int pdo_sqlsrv_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC);
int pdo_sqlsrv_stmt_close_cursor(pdo_stmt_t *stmt TSRMLS_DC);

struct pdo_stmt_methods pdo_sqlsrv_stmt_methods = {

    pdo_sqlsrv_stmt_dtor,
    pdo_sqlsrv_stmt_execute,
    pdo_sqlsrv_stmt_fetch,
    pdo_sqlsrv_stmt_describe_col,
    pdo_sqlsrv_stmt_get_col_data,
    pdo_sqlsrv_stmt_param_hook,
    pdo_sqlsrv_stmt_set_attr,
    pdo_sqlsrv_stmt_get_attr,
    pdo_sqlsrv_stmt_get_col_meta,
    pdo_sqlsrv_stmt_next_rowset,
    pdo_sqlsrv_stmt_close_cursor

};

void stmt_option_scrollable:: operator()( sqlsrv_stmt* stmt, stmt_option const* /*opt*/, zval* value_z TSRMLS_DC )
{
    set_stmt_cursors( stmt, value_z TSRMLS_CC ); 
}

void stmt_option_encoding:: operator()( sqlsrv_stmt* stmt, stmt_option const* /*opt*/, zval* value_z TSRMLS_DC )
{
    set_stmt_encoding( stmt, value_z TSRMLS_CC );
}

void stmt_option_direct_query:: operator()( sqlsrv_stmt* stmt, stmt_option const* /*opt*/, zval* value_z TSRMLS_DC )
{
    pdo_sqlsrv_stmt *pdo_stmt = static_cast<pdo_sqlsrv_stmt*>( stmt );
    pdo_stmt->direct_query = ( zend_is_true( value_z )) ? true : false;
}

void stmt_option_cursor_scroll_type:: operator()( sqlsrv_stmt* stmt, stmt_option const* /*opt*/, zval* value_z TSRMLS_DC )
{
    set_stmt_cursor_scroll_type( stmt, value_z TSRMLS_CC );
}

void stmt_option_emulate_prepares:: operator()( sqlsrv_stmt* stmt, stmt_option const* /*opt*/, zval* value_z TSRMLS_DC )
{
    pdo_stmt_t *pdo_stmt = static_cast<pdo_stmt_t*>( stmt->driver() );
    pdo_stmt->supports_placeholders = ( zend_is_true( value_z )) ? PDO_PLACEHOLDER_NONE : PDO_PLACEHOLDER_POSITIONAL;
}


// log a function entry point
#define PDO_LOG_STMT_ENTRY \
{ \
    pdo_sqlsrv_stmt* driver_stmt = reinterpret_cast<pdo_sqlsrv_stmt*>( stmt->driver_data ); \
    driver_stmt->set_func( __FUNCTION__ ); \
    LOG( SEV_NOTICE, __FUNCTION__ ## ": entering" ); \
}

// PDO SQLSRV statement destructor
pdo_sqlsrv_stmt::~pdo_sqlsrv_stmt( void )
{
    std::for_each( current_meta_data.begin(), current_meta_data.end(), meta_data_free );
    current_meta_data.clear();

    if( bound_column_param_types ) {
        sqlsrv_free( bound_column_param_types );
        bound_column_param_types = NULL;
    }

    if( direct_query_subst_string ) {
        // we use efree rather than sqlsrv_free since sqlsrv_free may wrap another allocation scheme
        // and we use estrdup to allocate this string, which uses emalloc
        efree( reinterpret_cast<void*>( const_cast<char*>( direct_query_subst_string )));
    }
}


// pdo_sqlsrv_stmt_close_cursor
// Close any open cursors on the statement. Maps to PDO function PDOStatement::closeCursor.
// Parameters:
// *stmt - Pointer to current statement
// Return:
// Returns 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_close_cursor(pdo_stmt_t *stmt TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

    try {

        SQLSRV_ASSERT( stmt != NULL, "pdo_sqlsrv_stmt_next_rowset: pdo_stmt object was null" );

        sqlsrv_stmt* driver_stmt = reinterpret_cast<sqlsrv_stmt*>( stmt->driver_data );

        SQLSRV_ASSERT( driver_stmt != NULL, "pdo_sqlsrv_stmt_next_rowset: driver_data object was null" );

        // to "close the cursor" means we make the statement ready for execution again.  To do this, we 
        // skip all the result sets on the current statement.
        while( driver_stmt->past_next_result_end == false ) {

            core_sqlsrv_next_result( driver_stmt TSRMLS_CC );
        }
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {

        DIE( "pdo_sqlsrv_stmt_next_rowset: Unknown exception occurred while advanding to the next result set." );
    }

    return 1;
}

// pdo_sqlsrv_stmt_describe_col
// Gets the metadata for a column based on the column number. 
// Calls the core_sqlsrv_field_metadata function present in the core layer.
// Parameters:
// *stmt - pointer to current statement
// colno - Index of the column which requires description.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_describe_col(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

    SQLSRV_ASSERT(( colno >= 0 ), "pdo_sqlsrv_stmt_describe_col: Column number should be >= 0." );

    sqlsrv_malloc_auto_ptr<field_meta_data> core_meta_data;

    try {
        
        core_meta_data = core_sqlsrv_field_metadata( reinterpret_cast<sqlsrv_stmt*>( stmt->driver_data ), colno TSRMLS_CC );
    }

    catch( core::CoreException& ) {
        return 0;
    }

    catch(...) {
        DIE( "pdo_sqlsrv_stmt_describe_col: Unexpected exception occurred." );
    }

    pdo_column_data* column_data = &(stmt->columns[colno]);

    SQLSRV_ASSERT( column_data != NULL, "pdo_sqsrv_stmt_describe_col: pdo_column_data was null" );

    // Set the name 
    column_data->name = reinterpret_cast<char*>( core_meta_data->field_name.get());
    core_meta_data->field_name.transferred();
    
    // Set the namelen
    column_data->namelen = core_meta_data->field_name_len;

    // Set the maxlen
    column_data->maxlen = ( core_meta_data->field_precision > 0 ) ? core_meta_data->field_precision : core_meta_data->field_size;
        
    // Set the precision
    column_data->precision = core_meta_data->field_scale;

    // Set the param_type    
    column_data->param_type = PDO_PARAM_ZVAL; 
    
    // store the field data for use by pdo_sqlsrv_stmt_get_col_data
    pdo_sqlsrv_stmt* driver_stmt = reinterpret_cast<pdo_sqlsrv_stmt*>( stmt->driver_data );
    SQLSRV_ASSERT( driver_stmt != NULL, "Invalid driver statement in pdo_sqlsrv_stmt_describe_col" );
    driver_stmt->current_meta_data.push_back( core_meta_data.transferred() );
    SQLSRV_ASSERT( driver_stmt->current_meta_data.size() == colno + 1, "Meta data vector out of sync with column numbers" );

    return 1;
}


// pdo_sqlsrv_stmt_dtor
// Maps to PDOStatement::__destruct. Destructor for the PDO Statement. 
// Parameters:
// *stmt - pointer to current statement
// Return:
// 1 for success.
int pdo_sqlsrv_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
    sqlsrv_stmt* driver_stmt = reinterpret_cast<sqlsrv_stmt*>( stmt->driver_data );

    LOG( SEV_NOTICE, "pdo_sqlsrv_stmt_dtor: entering" );

    // if a PDO statement didn't complete preparation, its driver_data can be NULL
    if( driver_stmt == NULL ) {

        return 1;
    }

    driver_stmt->~sqlsrv_stmt();

    sqlsrv_free( driver_stmt );

    stmt->driver_data = NULL;

    return 1;
}

// pdo_sqlsrv_stmt_execute
// Maps to PDOStatement::Execute. Executes the prepared statement. 
// Parameters:
// *stmt - pointer to the current statement.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

    try {

        pdo_sqlsrv_stmt* driver_stmt = reinterpret_cast<pdo_sqlsrv_stmt*>( stmt->driver_data );
        SQLSRV_ASSERT( driver_stmt != NULL, "pdo_sqlsrv_stmt_execute: driver_data object was null" );

        // prepare for execution by flushing anything remaining in the result set if it wasn't already
        // done before binding parameters
        if( driver_stmt->executed && !driver_stmt->past_next_result_end ) {

            while( driver_stmt->past_next_result_end == false ) {

                core_sqlsrv_next_result( driver_stmt TSRMLS_CC, false );
            }
        }
        
        const char* query = NULL;
        unsigned int query_len = 0;

        // if the user is doing a direct query (PDO::SQLSRV_ATTR_DIRECT_QUERY), set the query here
        if( driver_stmt->direct_query ) {

            query = driver_stmt->direct_query_subst_string;
            query_len = driver_stmt->direct_query_subst_string_len;
        }

        // if the user is using prepare emulation (PDO::ATTR_EMULATE_PREPARES), set the query to the 
        // subtituted query provided by PDO
        if( stmt->supports_placeholders == PDO_PLACEHOLDER_NONE ) {

            query = stmt->active_query_string;
            query_len = stmt->active_query_stringlen;
        }

        core_sqlsrv_execute( driver_stmt TSRMLS_CC, query, query_len );

        stmt->column_count = core::SQLNumResultCols( driver_stmt TSRMLS_CC );

        // return the row count regardless if there are any rows or not
        stmt->row_count = core::SQLRowCount( driver_stmt TSRMLS_CC );

        // workaround for a bug in the PDO driver manager.  It is fairly simple to crash the PDO driver manager with 
        // the following sequence:
        // 1) Prepare and execute a statement (that has some results with it)
        // 2) call PDOStatement::nextRowset until there are no more results
        // 3) execute the statement again
        // 4) call PDOStatement::getColumnMeta
        // It crashes from what I can tell because there is no metadata because there was no call to 
        // pdo_stmt_sqlsrv_describe_col and stmt->columns is NULL on the second call to
        // PDO::execute.  My guess is that because stmt->executed is true, it optimizes away a necessary call to
        // pdo_sqlsrv_stmt_describe_col.  By setting the stmt->executed flag to 0, this call is not optimized away
        // and the crash disappears.
        if( stmt->columns == NULL ) {
            stmt->executed = 0;
        }
    }
    catch( core::CoreException& /*e*/ ) {

        return 0;

    }
    catch( ... ) {

        DIE( "pdo_sqlsrv_stmt_execute: Unexpected exception occurred." );

    }

    // success
    return 1;
}



// pdo_sqlsrv_stmt_fetch
// Maps to PDOStatement::fetch
// Move the cursor to the record indicated. If the cursor is moved off the end, 
// or before the beginning if a scrollable cursor is created, then FAILURE is returned.  
// Parameters:
// *stmt - pointer to current statement for which the cursor should be moved.
// ori - cursor orientation. Maps to the list of PDO::FETCH_ORI_* constants
// offset - For orientations that use it, offset to move to.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_fetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori,
                          long offset TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

    try {
        
        SQLSRV_ASSERT( stmt != NULL, "pdo_sqlsrv_stmt_fetch: pdo_stmt object was null" );

        pdo_sqlsrv_stmt* driver_stmt = reinterpret_cast<pdo_sqlsrv_stmt*>( stmt->driver_data );

        SQLSRV_ASSERT( driver_stmt != NULL, "pdo_sqlsrv_stmt_fetch: driver_data object was null" );

        // set the types for bound columns to zval so that PDO does no conversion when the value
        // is returned by pdo_sqlsrv_get_col_data.  Remember the types that were bound by the user
        // and use it to manually convert data types
        if( stmt->bound_columns ) {

            pdo_bound_param_data* bind_data = NULL;

            if( !driver_stmt->bound_column_param_types ) {
                driver_stmt->bound_column_param_types = 
                    reinterpret_cast<pdo_param_type*>( sqlsrv_malloc( stmt->column_count, sizeof( pdo_param_type ), 0 ));
                std::fill( driver_stmt->bound_column_param_types, driver_stmt->bound_column_param_types + stmt->column_count,
                           PDO_PARAM_ZVAL );
            }

            for( long i = 0; i < stmt->column_count; ++i ) {

                if( zend_hash_index_find( stmt->bound_columns, i, (void**) &bind_data ) == FAILURE && 
                    zend_hash_find( stmt->bound_columns, stmt->columns[ i ].name, stmt->columns[ i ].namelen, 
                                    (void**) &bind_data ) == FAILURE ) {

                    driver_stmt->bound_column_param_types[ i ] = PDO_PARAM_ZVAL;
                    continue;
                }

                if( bind_data->param_type != PDO_PARAM_ZVAL ) {

                    driver_stmt->bound_column_param_types[ i ] = bind_data->param_type;
                    bind_data->param_type = PDO_PARAM_ZVAL;
                }
            }
        }

        SQLSMALLINT odbc_fetch_ori = pdo_fetch_ori_to_odbc_fetch_ori( ori );
        bool data = core_sqlsrv_fetch( driver_stmt, odbc_fetch_ori, offset TSRMLS_CC );

        // support for the PDO rowCount method.  Since rowCount doesn't call a method, PDO relies on us to fill the 
        // pdo_stmt_t::row_count member
        if( driver_stmt->past_fetch_end || driver_stmt->cursor_type != SQL_CURSOR_FORWARD_ONLY ) {

            stmt->row_count = core::SQLRowCount( driver_stmt TSRMLS_CC );

            // a row_count of -1 means no rows, but we change it to 0
            if( stmt->row_count == -1 ) {

                stmt->row_count = 0;
            }
        }

        // if no data was returned, then return false so data isn't retrieved
        if( !data ) {
            return 0;
        }

        return 1;
    }

    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {
        
        DIE ("pdo_sqlsrv_stmt_fetch: Unexpected exception occurred.");
    }
}

// pdo_sqlsrv_stmt_get_col_data
// Called by the set of PDO Fetch functions.
// Retrieves a single column. PDO driver manager is responsible for freeing the 
// returned buffer. Because PDO can request fields out of order and ODBC does not 
// support out of order field requests, this function should also cache fields.  
// Parameters:
// stmt         - Statement to retrive the column for.
// colno        - Index of the column that needs to be retrieved. Starts with 0.
// ptr          - Returns the buffer containing the column data.
// len          - Length of the buffer returned.
// caller_frees - Flag to let the PDO driver manager know that it is responsible for
//                freeing the memory.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_get_col_data(pdo_stmt_t *stmt, int colno,
                                 char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;
   
    try {

        SQLSRV_ASSERT( stmt != NULL, "pdo_sqlsrv_stmt_get_col_data: pdo_stmt object was null" );

        pdo_sqlsrv_stmt* driver_stmt = reinterpret_cast<pdo_sqlsrv_stmt*>( stmt->driver_data );
        
        SQLSRV_ASSERT( driver_stmt != NULL, "pdo_sqlsrv_stmt_get_col_data: driver_data object was null" );

        CHECK_CUSTOM_ERROR((colno < 0), driver_stmt, PDO_SQLSRV_ERROR_INVALID_COLUMN_INDEX ) {
            return 0;
        }
        
        // Let PDO free the memory after use. 
        *caller_frees = 1;

        // set the metadata for the current column
        pdo_column_data* column_data = &(stmt->columns[colno]);
        
        // translate the pdo type to a type the core layer understands
        sqlsrv_phptype sqlsrv_php_type;
        SQLSRV_ASSERT( colno >= 0 && colno < static_cast<int>( driver_stmt->current_meta_data.size()),
                       "Invalid column number in pdo_sqlsrv_stmt_get_col_data" );
        sqlsrv_php_type = driver_stmt->sql_type_to_php_type( driver_stmt->current_meta_data[ colno ]->field_type,
                                                             driver_stmt->current_meta_data[ colno ]->field_size, true );

        // set the encoding if the user specified one via bindColumn, otherwise use the statement's encoding
        sqlsrv_php_type.typeinfo.encoding = driver_stmt->encoding();

        // if a column is bound to a type different than the column type, figure out a way to convert it to the 
        // type they want
        if( stmt->bound_columns && driver_stmt->bound_column_param_types[ colno ] != PDO_PARAM_ZVAL ) {

            sqlsrv_php_type.typeinfo.type = pdo_type_to_sqlsrv_php_type( driver_stmt, 
                                                                         driver_stmt->bound_column_param_types[ colno ] 
                                                                         TSRMLS_CC );

            pdo_bound_param_data* bind_data = NULL;
            int zr = zend_hash_index_find( stmt->bound_columns, colno, (void**) &bind_data );

            if( bind_data != NULL && bind_data->driver_params != NULL ) {

                CHECK_CUSTOM_ERROR( Z_TYPE_P( bind_data->driver_params ) != IS_LONG, driver_stmt, 
                                    PDO_SQLSRV_ERROR_INVALID_COLUMN_DRIVER_DATA, colno + 1 ) {
                    throw pdo::PDOException();
                }

                CHECK_CUSTOM_ERROR( driver_stmt->bound_column_param_types[ colno ] != PDO_PARAM_STR 
                                    && driver_stmt->bound_column_param_types[ colno ] != PDO_PARAM_LOB, driver_stmt,
                                    PDO_SQLSRV_ERROR_COLUMN_TYPE_DOES_NOT_SUPPORT_ENCODING, colno + 1 ) {

                        throw pdo::PDOException();
                }

                sqlsrv_php_type.typeinfo.encoding = Z_LVAL_P( bind_data->driver_params );

                switch( sqlsrv_php_type.typeinfo.encoding ) {
                    case SQLSRV_ENCODING_SYSTEM:
                    case SQLSRV_ENCODING_BINARY:
                    case SQLSRV_ENCODING_UTF8:
                        break;
                    default:
                        THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_INVALID_DRIVER_COLUMN_ENCODING, colno );
                        break;
                }
            }
        }
        
        SQLSRV_PHPTYPE sqlsrv_phptype_out = SQLSRV_PHPTYPE_INVALID;
        core_sqlsrv_get_field( driver_stmt, colno, sqlsrv_php_type, false, reinterpret_cast<void**>( ptr ), 
                               reinterpret_cast<SQLLEN*>( len ), true, &sqlsrv_phptype_out TSRMLS_CC );
        zval** zval_ptr = reinterpret_cast<zval**>( sqlsrv_malloc( sizeof( zval* )));
        *zval_ptr = reinterpret_cast<zval*>( convert_to_zval( sqlsrv_phptype_out, reinterpret_cast<void**>( ptr ), *len ));
        *ptr = reinterpret_cast<char*>( zval_ptr );

        *len = sizeof( zval );

        return 1;        
    }
    
    catch ( core::CoreException& ) {
        return 0;
    }
    catch ( ... ) {
        DIE ("pdo_sqlsrv_stmt_get_col_data: Unexpected exception occurred.");
    }
}

// pdo_sqlsrv_stmt_set_attr
// Maps to the PDOStatement::setAttribute. Sets the attribute on a statement.
// Parameters:
// stmt - Current statement on which the attribute should be set.
// attr - Represents any valid set of attribute constants supported by this driver.
// val - Attribute value.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_set_attr(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

    sqlsrv_stmt* driver_stmt = static_cast<sqlsrv_stmt*>( stmt->driver_data );

    try {

        switch( attr ) {

            case SQLSRV_ATTR_DIRECT_QUERY:
                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_DQ_ATTR_AT_PREPARE_ONLY );
                break;

            case SQLSRV_ATTR_ENCODING:
                set_stmt_encoding( driver_stmt, val TSRMLS_CC );
                break;
            
            case PDO_ATTR_CURSOR:
                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_CURSOR_ATTR_AT_PREPARE_ONLY );
                break;
           
            case SQLSRV_ATTR_QUERY_TIMEOUT:
                core_sqlsrv_set_query_timeout( driver_stmt, val TSRMLS_CC );
                break;

            case SQLSRV_ATTR_CURSOR_SCROLL_TYPE:
                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_CURSOR_ATTR_AT_PREPARE_ONLY );
                break;

            case SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE:
                core_sqlsrv_set_buffered_query_limit( driver_stmt, val TSRMLS_CC );
                break;

            default:
                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_INVALID_STMT_ATTR );
                break;
        }
    }
    catch( core::CoreException& ) {
        return 0;
    }

    catch ( ... ) {
        DIE ("pdo_sqlsrv_stmt_set_attr: Unexpected exception occurred.");
    }

    return 1;
}


// pdo_sqlsrv_stmt_get_attr
// Maps to the PDOStatement::getAttribute. Gets the value of a given attribute on a statement.
// Parameters:
// stmt - Current statement for which the attribute value is requested.
// attr - Represents any valid set of attribute constants supported by this driver.
// return_value - Attribute value.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_get_attr( pdo_stmt_t *stmt, long attr, zval *return_value TSRMLS_DC )
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;
    
    pdo_sqlsrv_stmt* driver_stmt = static_cast<pdo_sqlsrv_stmt*>( stmt->driver_data );
    SQLSRV_ASSERT(( driver_stmt != NULL ), "pdo_sqlsrv_stmt_get_attr: stmt->driver_data was null" );

    try {

        switch( attr ) {

            case SQLSRV_ATTR_DIRECT_QUERY:
            {
                ZVAL_BOOL( return_value, driver_stmt->direct_query );
                break;
            }

           case SQLSRV_ATTR_ENCODING:
            { 
                ZVAL_LONG( return_value, driver_stmt->encoding() );
                break;
            }
            
            case PDO_ATTR_CURSOR:
            {
                ZVAL_LONG( return_value, ( driver_stmt->cursor_type != SQL_CURSOR_FORWARD_ONLY ? 
                                           PDO_CURSOR_SCROLL : PDO_CURSOR_FWDONLY )); 
                break;
            }

            case SQLSRV_ATTR_CURSOR_SCROLL_TYPE:
            {
                ZVAL_LONG( return_value, driver_stmt->cursor_type ); 
                break;
            }

            case SQLSRV_ATTR_CLIENT_BUFFER_MAX_KB_SIZE:
            {
                ZVAL_LONG( return_value, driver_stmt->buffered_query_limit ); 
                break;
            }

            case SQLSRV_ATTR_QUERY_TIMEOUT:
            {
                ZVAL_LONG( return_value, ( driver_stmt->query_timeout == QUERY_TIMEOUT_INVALID ? 0 : driver_stmt->query_timeout ));
                break;
            }

            default:
                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_INVALID_STMT_ATTR );
                break;
        }
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch ( ... ) {

        DIE ("pdo_sqlsrv_stmt_get_attr: Unexpected exception occurred.");
    }

    return 1;
}

// pdo_sqlsrv_stmt_get_col_meta
// Maps to PDOStatement::getColumnMeta. Return extra metadata.  
// Though we don't return any extra metadata, PDO relies on us to 
// create the associative array that holds the standard information, 
// so we create one and return it for PDO's use.
// Parameters:
// stmt         - Current statement.
// colno        - The index of the field for which to return the metadata.
// return_value - zval* consisting of the metadata.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_get_col_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

	if ( !stmt->columns )
		DIE( "pdo_sqlsrv_stmt_get_col_meta: This method only works after the statement is executed." );

    try {
		
        SQLSRV_ASSERT( Z_TYPE_P( return_value ) == IS_NULL, "Metadata already has value.  Must be NULL." );

        sqlsrv_stmt* driver_stmt = static_cast<sqlsrv_stmt*>( stmt->driver_data );

        SQLSRV_ASSERT( colno >= 0 && colno < SHRT_MAX, "pdo_sqlsrv_stmt_get_col_meta: tried to overflow a short" );
        sqlsrv_malloc_auto_ptr<field_meta_data> core_meta_data( core_sqlsrv_field_metadata( driver_stmt, (SQLSMALLINT) colno TSRMLS_CC ) );
        
		// initialize the array to nothing, as PDO requires us to create it
        core::sqlsrv_array_init( *driver_stmt, return_value TSRMLS_CC );
        
		// add the following fields: flags, native_type, driver:decl_type, table
        add_assoc_long( return_value, "flags", 0 );

        // get the name of the data type
        char field_type_name[ SQL_SERVER_IDENT_SIZE_MAX*2+1 ];
        SQLSMALLINT out_buff_len;
        SQLLEN not_used;
        core::SQLColAttribute( driver_stmt, (SQLUSMALLINT) colno + 1, SQL_DESC_TYPE_NAME, field_type_name,
                              ARRAYELEMENTS( field_type_name ), &out_buff_len, &not_used TSRMLS_CC );
        add_assoc_string( return_value, "sqlsrv:decl_type", field_type_name, 1 );

        // get the PHP type of the column.  The types returned here mirror the types returned by debug_zval_dump when 
        // given a variable of the same type.  However, debug_zval_dump also gives the length of a string, and we only
        // say string, since the length is given in another field of the metadata array.
        long pdo_type = sql_type_to_pdo_type( core_meta_data->field_type );
        switch( pdo_type ) {
            case PDO_PARAM_STR:
                add_assoc_string( return_value, "native_type", "string", 1 );
                break;
            default:
                DIE( "pdo_sqlsrv_stmt_get_col_data: Unknown PDO type returned" );
                break;
        }

        // add the table name of the field.  All the tests so far show this to always be "", but we adhere to the PDO spec
        char table_name[ SQL_SERVER_IDENT_SIZE_MAX*2+1 ];
        SQLLEN field_type_num;
        core::SQLColAttribute( driver_stmt, (SQLUSMALLINT) colno + 1, SQL_DESC_TABLE_NAME, table_name, ARRAYELEMENTS(table_name),
                               &out_buff_len, &field_type_num TSRMLS_CC );
        add_assoc_string( return_value, "table", table_name, 1 );

        if( stmt->columns[ colno ].param_type == PDO_PARAM_ZVAL ) {
			add_assoc_long( return_value, "pdo_type", pdo_type );
        }

        // this will ensure that the field_name field, which is an auto pointer gets freed.
        //(*core_meta_data).~field_meta_data();  
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch(...) {

        DIE( "pdo_sqlsrv_stmt_get_col_meta: Unknown exception occurred while retrieving metadata." );
    }
    
    return 1;
}


// pdo_sqlsrv_stmt_next_rowset
// Maps to PDOStatement::nextRowset.
// Move the cursor to the beginning of the next rowset in a multi-rowset result.
// Clears the field cache from the last row retrieved using pdo_sqlsrv_stmt_get_col_data.  
// Calls core_sqlsrv_next_result using the core_stmt found within stmt->driver_data.
// If another result set is available, this function returns 1.  Otherwise it returns 0.
// Parameters:
// stmt - PDOStatement object containing the result set.
// Return:
// 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;
    PDO_VALIDATE_STMT;
    PDO_LOG_STMT_ENTRY;

    try {

        SQLSRV_ASSERT( stmt != NULL, "pdo_sqlsrv_stmt_next_rowset: pdo_stmt object was null" );

        pdo_sqlsrv_stmt* driver_stmt = reinterpret_cast<pdo_sqlsrv_stmt*>( stmt->driver_data );

        SQLSRV_ASSERT( driver_stmt != NULL, "pdo_sqlsrv_stmt_next_rowset: driver_data object was null" );

        core_sqlsrv_next_result( static_cast<sqlsrv_stmt*>( stmt->driver_data ) TSRMLS_CC );

        // clear the current meta data since the new result will generate new meta data
        std::for_each( driver_stmt->current_meta_data.begin(), driver_stmt->current_meta_data.end(), meta_data_free );
        driver_stmt->current_meta_data.clear();

        // if there are no more result sets, return that it failed.
        if( driver_stmt->past_next_result_end == true ) {
            return 0;
        }

        stmt->column_count = core::SQLNumResultCols( driver_stmt TSRMLS_CC );

        // return the row count regardless if there are any rows or not
        stmt->row_count = core::SQLRowCount( driver_stmt TSRMLS_CC );
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {

        DIE( "pdo_sqlsrv_stmt_next_rowset: Unknown exception occurred while advancing to the next result set." );
    }

    return 1;
}

// pdo_sqlsrv_stmt_param_hook
// Maps to PDOStatement::bindColumn.
// Called by PDO driver manager to bind a parameter or column. 
// This function pulls several duties for binding parameters and columns.  
// It takes an event as a parameter that explains what the function should do on 
// behalf of a parameter or column.  We only use one of these events, 
// PDO_PARAM_EVT_EXEC_PRE, the remainder simply return.
// Paramters:
// stmt - PDO Statement object to bind a parameter.
// param - paramter to bind.
// event_type - Event to bind a parameter
// Return: 
// Returns 0 for failure, 1 for success.
int pdo_sqlsrv_stmt_param_hook(pdo_stmt_t *stmt,
                               struct pdo_bound_param_data *param, enum pdo_param_event event_type TSRMLS_DC)
{
    PDO_RESET_STMT_ERROR;

    try {

        switch( event_type ) {

            // since the param isn't reliable, we don't do anything here
            case PDO_PARAM_EVT_ALLOC:
                break;
            case PDO_PARAM_EVT_FREE:
                break;
            // bind the parameter in the core layer
            case PDO_PARAM_EVT_EXEC_PRE:
                {
                    PDO_VALIDATE_STMT;
                    PDO_LOG_STMT_ENTRY;

                    // skip column bindings
                    if( !param->is_param ) {
                        break;
                    }

                    sqlsrv_stmt* driver_stmt = reinterpret_cast<sqlsrv_stmt*>( stmt->driver_data );
                    SQLSRV_ASSERT( driver_stmt != NULL, "pdo_sqlsrv_stmt_param_hook: driver_data object was null" );

                    // prepare for binding parameters by flushing anything remaining in the result set
                    if( driver_stmt->executed && !driver_stmt->past_next_result_end ) {

                        while( driver_stmt->past_next_result_end == false ) {

                            core_sqlsrv_next_result( driver_stmt TSRMLS_CC, false );
                        }
                    }

                    int direction = SQL_PARAM_INPUT;
                    SQLSMALLINT sql_type = SQL_UNKNOWN_TYPE;
                    SQLULEN column_size = SQLSRV_UNKNOWN_SIZE;
                    SQLSMALLINT decimal_digits = 0;
                    // determine the direction of the parameter.  By default it's input, but if the user specifies a size
                    // that means they want output, and if they include the flag, then it's input/output.
                    // It's invalid to specify the input/output flag but not specify a length
                    CHECK_CUSTOM_ERROR( (param->param_type & PDO_PARAM_INPUT_OUTPUT) && (param->max_value_len == 0),
                                        driver_stmt, PDO_SQLSRV_ERROR_INVALID_PARAM_DIRECTION, param->paramno + 1 ) {
                        throw pdo::PDOException();
                    }
                    if( param->max_value_len > 0 || param->max_value_len == SQLSRV_DEFAULT_SIZE ) {
                        if( param->param_type & PDO_PARAM_INPUT_OUTPUT ) {
                            direction = SQL_PARAM_INPUT_OUTPUT;
                        }
                        else {
                            direction = SQL_PARAM_OUTPUT;
                        }
                    }
                    // if the parameter is output or input/output, translate the type between the PDO::PARAM_* constant
                    // and the SQLSRV_PHPTYPE_* constant
                    int pdo_type = param->param_type;
                    SQLSRV_PHPTYPE php_out_type = SQLSRV_PHPTYPE_INVALID;
                    switch( pdo_type & ~PDO_PARAM_INPUT_OUTPUT ) {
                        case PDO_PARAM_BOOL:
                        case PDO_PARAM_INT:
                            php_out_type = SQLSRV_PHPTYPE_INT;
                            break;
                        case PDO_PARAM_STR:
                            php_out_type = SQLSRV_PHPTYPE_STRING;
                            break;
                        // when the user states PDO::PARAM_NULL, they mean send a null no matter what the variable is
                        // since the core layer keys off the zval type, we substitute a null for what they gave us
                        case PDO_PARAM_NULL:
                        {
                            zval* null_zval;
                            php_out_type = SQLSRV_PHPTYPE_NULL;
                            MAKE_STD_ZVAL( null_zval );
                            ZVAL_NULL( null_zval );
                            zval_ptr_dtor( &param->parameter );
                            param->parameter = null_zval;
                            break;
                        }
                        case PDO_PARAM_LOB:
                            php_out_type = SQLSRV_PHPTYPE_STREAM;
                            break;
                        case PDO_PARAM_STMT:
                            THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_PDO_STMT_UNSUPPORTED );
                            break;
                        default:
                            SQLSRV_ASSERT( false, "Unknown PDO::PARAM_* constant given." );
                            break;
                    }
                    // set the column size parameter for bind_param if we are expecting something back
                    if( direction != SQL_PARAM_INPUT ) {
                        switch( php_out_type ) {
                            case SQLSRV_PHPTYPE_NULL:
                            case SQLSRV_PHPTYPE_STREAM:
                                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_INVALID_OUTPUT_PARAM_TYPE );
                                break;
                            case SQLSRV_PHPTYPE_INT:
                                column_size = SQLSRV_UNKNOWN_SIZE;
                                break;
                            case SQLSRV_PHPTYPE_STRING:
                            {
                                CHECK_CUSTOM_ERROR( param->max_value_len <= 0, driver_stmt, 
                                                    PDO_SQLSRV_ERROR_INVALID_OUTPUT_STRING_SIZE, param->paramno + 1 ) {
                                    throw pdo::PDOException();
                                }
                                column_size = param->max_value_len;
                                break;
                            }
                            default:
                                SQLSRV_ASSERT( false, "Invalid PHP type for output parameter.  Should have been caught already." );
                                break;
                        }
                    }
                    // block all objects from being bound as input or input/output parameters since there is a
                    // weird case: 
                    // $obj = date_create(); 
                    // $s->bindParam( n, $obj, PDO::PARAM_INT ); // anything different than PDO::PARAM_STR
                    // that succeeds since the core layer implements DateTime object handling for the sqlsrv
                    // 2.0 driver.  To be consistent and avoid surprises of one object type working and others
                    // not, we block all objects here.
                    CHECK_CUSTOM_ERROR( direction != SQL_PARAM_OUTPUT && Z_TYPE_P( param->parameter ) == IS_OBJECT,
                                        driver_stmt, SQLSRV_ERROR_INVALID_PARAMETER_PHPTYPE, param->paramno + 1 ) {
                        throw pdo::PDOException();
                    }
                    // the encoding by default is that set on the statement
                    SQLSRV_ENCODING encoding = driver_stmt->encoding();
                    // if the statement's encoding is the default, then use the one on the connection
                    if( encoding == SQLSRV_ENCODING_DEFAULT ) {
                        encoding = driver_stmt->conn->encoding();
                    }
                    // if the user provided an encoding, use it instead
                    if( param->driver_params != NULL ) {
                        CHECK_CUSTOM_ERROR( Z_TYPE_P( param->driver_params ) != IS_LONG, driver_stmt, 
                                            PDO_SQLSRV_ERROR_INVALID_DRIVER_PARAM ) {
                            throw pdo::PDOException();
                        }
                        CHECK_CUSTOM_ERROR( pdo_type != PDO_PARAM_STR && pdo_type != PDO_PARAM_LOB, driver_stmt,
                                            PDO_SQLSRV_ERROR_INVALID_DRIVER_PARAM_TYPE, param->paramno + 1 ) {
                            throw pdo::PDOException();
                        }
                        encoding = static_cast<SQLSRV_ENCODING>( Z_LVAL_P( param->driver_params ));

                        switch( encoding ) {
                            case SQLSRV_ENCODING_SYSTEM:
                            case SQLSRV_ENCODING_BINARY:
                            case SQLSRV_ENCODING_UTF8:
                                break;
                            default:
                                THROW_PDO_ERROR( driver_stmt, PDO_SQLSRV_ERROR_INVALID_DRIVER_PARAM_ENCODING,
                                                 param->paramno + 1 );
                                break;
                        }
                    }
                    // and bind the parameter
                    core_sqlsrv_bind_param( driver_stmt, param->paramno, direction, param->parameter, php_out_type, encoding,
                                            sql_type, column_size, decimal_digits TSRMLS_CC );
                }
                break;
            // undo any work done by the core layer after the statement is executed
            case PDO_PARAM_EVT_EXEC_POST:
                {
                    PDO_VALIDATE_STMT;
                    PDO_LOG_STMT_ENTRY;

                    // skip column bindings
                    if( !param->is_param ) {
                        break;
                    }

                    core_sqlsrv_post_param( reinterpret_cast<sqlsrv_stmt*>( stmt->driver_data ), param->paramno, 
                                            param->parameter TSRMLS_CC );
                }
                break;
            case PDO_PARAM_EVT_FETCH_PRE:
                break;
            case PDO_PARAM_EVT_FETCH_POST:
                break;
            case PDO_PARAM_EVT_NORMALIZE:
                break;
            default:
                DIE( "pdo_sqlsrv_stmt_param_hook: Unknown event type" );
                break;
        }
    }
    catch( core::CoreException& ) {

        return 0;
    }
    catch( ... ) {

        DIE( "pdo_sqlsrv_stmt_param_hook: Unknown exception" );
    }

    return 1;
}


// Returns a sqlsrv_phptype for a given SQL Server data type.
sqlsrv_phptype pdo_sqlsrv_stmt::sql_type_to_php_type( SQLINTEGER sql_type, SQLUINTEGER size, bool prefer_string_over_stream )
{
    sqlsrv_phptype sqlsrv_phptype;
    int local_encoding = this->encoding();
    // if the encoding on the connection changed
    if( this->encoding() == SQLSRV_ENCODING_DEFAULT ) {
        local_encoding = conn->encoding();
        SQLSRV_ASSERT( conn->encoding() != SQLSRV_ENCODING_DEFAULT || conn->encoding() == SQLSRV_ENCODING_INVALID,
                       "Invalid encoding on the connection.  Must not be invalid or default." );
    }                

    switch( sql_type ) {
        case SQL_BIT:
        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:
        case SQL_BIGINT:
        case SQL_CHAR:
        case SQL_DECIMAL:
        case SQL_FLOAT:
        case SQL_REAL:
        case SQL_GUID:
        case SQL_NUMERIC:
        case SQL_WCHAR:
        case SQL_VARCHAR:
        case SQL_WVARCHAR:
        case SQL_TYPE_DATE:
        case SQL_SS_TIMESTAMPOFFSET:
        case SQL_SS_TIME2:
        case SQL_TYPE_TIMESTAMP:
        case SQL_LONGVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_SS_XML:
            sqlsrv_phptype.typeinfo.type = SQLSRV_PHPTYPE_STRING;
            sqlsrv_phptype.typeinfo.encoding = local_encoding;
            break;
        case SQL_BINARY:
        case SQL_LONGVARBINARY:
        case SQL_VARBINARY:
        case SQL_SS_UDT:
            sqlsrv_phptype.typeinfo.type = SQLSRV_PHPTYPE_STRING;
            sqlsrv_phptype.typeinfo.encoding = SQLSRV_ENCODING_BINARY;
            break;
        default:
            sqlsrv_phptype.typeinfo.type = SQLSRV_PHPTYPE_INVALID;
            sqlsrv_phptype.typeinfo.encoding = SQLSRV_ENCODING_INVALID;
            break;
    }
    
    return sqlsrv_phptype;
}

