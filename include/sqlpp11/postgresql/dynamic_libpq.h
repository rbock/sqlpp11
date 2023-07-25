#pragma once

/*
 * Copyright (c) 2015, Volker Assmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdexcept>

#include <libpq-fe.h>

#ifdef SQLPP_DYNAMIC_LOADING

// Copied from PostgreSQL Internal include (internal/libpq-int.h) to enable linking
#define CMDSTATUS_LEN 64

struct PGresAttValue;
struct PGresParamDesc;
struct PGEvent;
struct PGMessageField;
struct PGresult_data;

typedef struct
{
   PQnoticeReceiver noticeRec; /* notice message receiver */
   void     *noticeRecArg;
   PQnoticeProcessor noticeProc;    /* notice message processor */
   void     *noticeProcArg;
} PGNoticeHooks;


struct pg_result
{
   int         ntups;
   int         numAttributes;
   PGresAttDesc *attDescs;
   PGresAttValue **tuples;    /* each PGresTuple is an array of
                         * PGresAttValue's */
   int         tupArrSize;    /* allocated size of tuples array */
   int         numParameters;
   PGresParamDesc *paramDescs;
   ExecStatusType resultStatus;
   char     cmdStatus[CMDSTATUS_LEN];     /* cmd status from the query */
   int         binary;        /* binary tuple values if binary == 1,
                         * otherwise text */

   /*
    * These fields are copied from the originating PGconn, so that operations
    * on the PGresult don't have to reference the PGconn.
    */
   PGNoticeHooks noticeHooks;
   PGEvent    *events;
   int         nEvents;
   int         client_encoding;  /* encoding id */

   /*
    * Error information (all NULL if not an error result).  errMsg is the
    * "overall" error message returned by PQresultErrorMessage.  If we have
    * per-field info then it is stored in a linked list.
    */
   char     *errMsg;       /* error message, or NULL if no error */
   PGMessageField *errFields; /* message broken into fields */

   /* All NULL attributes in the query result point to this null string */
   char     null_field[1];

   /*
    * Space management information.  Note that attDescs and error stuff, if
    * not null, point into allocated blocks.  But tuples points to a
    * separately malloc'd block, so that we can realloc it.
    */
   PGresult_data *curBlock;   /* most recently allocated block */
   int         curOffset;     /* start offset of free space in block */
   int         spaceLeft;     /* number of free bytes remaining in block */
};

#endif

// namespace for internal PQ function wrappers - when using this instead of libpq direct linking
// do this:
//    using namespace sqlpp::postgresql::dyn_pg;
// to override the libpq functions with these function pointers

namespace sqlpp
{
namespace postgresql
{
namespace dynamic
{

#define DYNDEFINE(NAME) extern decltype( ::NAME ) * NAME

DYNDEFINE(PQescapeStringConn);
DYNDEFINE(PQescapeString);
DYNDEFINE(PQescapeByteaConn);
DYNDEFINE(PQescapeBytea);
DYNDEFINE(PQfreemem);
DYNDEFINE(PQexec);
DYNDEFINE(PQprepare);
DYNDEFINE(PQexecPrepared);
DYNDEFINE(PQexecParams);
DYNDEFINE(PQresultStatus);
DYNDEFINE(PQresStatus);
DYNDEFINE(PQresultErrorMessage);
DYNDEFINE(PQresultErrorField);
DYNDEFINE(PQcmdTuples);
DYNDEFINE(PQcmdStatus);
DYNDEFINE(PQgetvalue);
DYNDEFINE(PQgetlength);
DYNDEFINE(PQgetisnull);
DYNDEFINE(PQoidValue);
DYNDEFINE(PQoidStatus);
DYNDEFINE(PQfformat);
DYNDEFINE(PQntuples);
DYNDEFINE(PQnfields);
DYNDEFINE(PQnparams);
DYNDEFINE(PQclear);
DYNDEFINE(PQfinish);
DYNDEFINE(PQstatus);
DYNDEFINE(PQconnectdb);
DYNDEFINE(PQerrorMessage);

#undef DYNDEFINE

/// load the PostgreSQL libraries, optionally providing the filename (leave empty for default)
void init_pg(std::string libname);

}
}
}
