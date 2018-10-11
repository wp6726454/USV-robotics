/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_mem.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation interface for mem3 memory management                      */
/*  including locale and fast memory pools                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20008
#define EPS_MODULE_ID LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include "eps_sys.h" /* Global EPS Header */
#include "eps_locks.h"
#include "eps_mem.h" /* Exported functionality Mem_Lock/-Unlock */
#include "mem3if.h"  /* MEM3 Interface */
#include <eps_trc.h> /* Tracing                 */


#define EPS_LOCAL_MEMORY_POOL_ID  1
#define EPS_FAST_MEMORY_POOL_ID   2

#define EPS_MEM_ALIGN4(x) (((x) + 3) & ~3) /* 4 Byte Alignment Makro */

//#define EPS_USE_MEM_STATISTIC                      /* Define if EPS Mem Statistic should be used */
//#define EPS_USE_MEM_TRACE                          /* Define if EPS Mem Trace should be used (only in combination with statistic */

typedef LSA_VOID* (*EPS_MEM_MEMSET_FCT)( LSA_VOID_PTR_TYPE, LSA_INT, size_t );

typedef struct eps_mem_block_tag
{
	LSA_BOOL          bInitialized;
	LSA_UINT32        uSize;         // Size of memory block [bytes]
	LSA_VOID_PTR_TYPE pBase;         // Base address of memory block
	LSA_VOID_PTR_TYPE pHelperMem;    // pointer to helper memory for data pool
	LSA_UINT32        uHelperSize;   // Size of helper memory
	LSA_INT           pool_nr;       // Creation number for data pool or alias nr
	LSA_UINT16        pool_id;
} EPS_MEM_BLOCK_TYPE, *EPS_MEM_BLOCK_PTR_TYPE;

typedef struct eps_mem_instance_tag
{
	LSA_BOOL            bInitialized;
	LSA_BOOL            bFastMemFromNormalMem;
	LSA_UINT16          hMemLock;
	EPS_MEM_BLOCK_TYPE  sStdMem;
	EPS_MEM_BLOCK_TYPE  sFastMem;
} EPS_MEM_INSTANCE_TYPE, *EPS_MEM_INSTANCE_PTR_TYPE;

static EPS_MEM_INSTANCE_TYPE g_EpsMem;
static EPS_MEM_INSTANCE_PTR_TYPE g_pEpsMem;

static LSA_VOID eps_mempool_undo_init( EPS_MEM_BLOCK_PTR_TYPE pMem );
static LSA_RESPONSE_TYPE eps_mempool_init( LSA_UINT16 pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, EPS_MEM_BLOCK_PTR_TYPE pMem );

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef EPS_USE_MEM_STATISTIC

#define EPS_LSA_COMP_ID_LAST      PSI_COMP_ID_LAST   /* Max Comp Id used by Mem Statistic */
#define EPS_LSA_COUNT_MEM_TYPES   PSI_MTYPE_MAX      /* Max Mem Type */

typedef struct eps_mem_statistic_prefix_tag *EPS_MEM_STATISTIC_PREFIX_PTR_TYPE;

typedef struct eps_mem_statistic_prefix_tag
{
	LSA_UINT32 magic;
	LSA_UINT32 comp_id;
	LSA_UINT32 mem_type;
	LSA_UINT32 pool;
	LSA_UINT32 size;
	LSA_UINT32 isPrefix;
	#ifdef EPS_USE_MEM_TRACE
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE next;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE prev;
	#endif
} EPS_MEM_STATISTIC_PREFIX_TYPE;

typedef struct
{
	LSA_UINT32  uAllocCount;
	LSA_UINT32  uActAllocCount;
	LSA_UINT32  uActAllocSize;
	LSA_UINT32  uMaxAllocSize;
	LSA_UINT32  uMinAllocSizeElement;
	LSA_UINT32  uMaxAllocSizeElement;
	LSA_UINT32  uSumAllocSize;
	#ifdef EPS_USE_MEM_TRACE
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE first;
	#endif
} EPS_MEM_STATISTIC_TYPE, *EPS_MEM_STATISTIC_PTR_TYPE;

EPS_MEM_STATISTIC_TYPE g_EpsMemStat[EPS_LSA_COMP_ID_LAST+1][EPS_LSA_COUNT_MEM_TYPES + 1] = { {{0}}, {{0}} }; /* +1 => sum */

static LSA_VOID_PTR_TYPE eps_mem_alloc_with_statistic( LSA_UINT32 size , LSA_CHAR* file, LSA_UINT16 line, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type , LSA_INT pool_name)
{
	LSA_UINT32 align4 = 0;
	LSA_UINT8* ptr;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE prefix;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE postfix;

	#ifdef EPS_USE_MEM_TRACE
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE* ppFirst;
	#endif

	LSA_UNUSED_ARG(file);
	LSA_UNUSED_ARG(line);

	EPS_ASSERT(lsa_comp_id <= EPS_LSA_COMP_ID_LAST);    /* Comp Id must be within range  */
	EPS_ASSERT(mem_type    <  EPS_LSA_COUNT_MEM_TYPES); /* Mem Type must be within range */

	if(size%4)
		align4 = (4-size%4);

	ptr = (LSA_UINT8*) Mem3palloc ( sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) + size + align4 + sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) , pool_name );
	EPS_ASSERT(ptr != 0);

	prefix  = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE) (ptr);
	postfix = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE) (ptr + sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) + size + align4);

	prefix->comp_id  = lsa_comp_id;
	prefix->mem_type = mem_type;
	prefix->pool     = (LSA_UINT32)pool_name;
	prefix->size     = size;
	prefix->magic    = 0x12344321;
	prefix->isPrefix = 1;

	*postfix = *prefix;
	postfix->isPrefix = 0;

	eps_mem_lock();

	/* Do Statistics */
	if(mem_type != PSI_MTYPE_NRT_MEM)
	{
		g_EpsMemStat[lsa_comp_id][mem_type].uActAllocSize += size;
		g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize += size;
	}
	g_EpsMemStat[0][mem_type].uActAllocSize += size;
	g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize += size;

	g_EpsMemStat[0][mem_type].uAllocCount++;
	g_EpsMemStat[0][mem_type].uActAllocCount++;
	g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uAllocCount++;
	g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount++;
	g_EpsMemStat[lsa_comp_id][mem_type].uAllocCount++;
	g_EpsMemStat[lsa_comp_id][mem_type].uActAllocCount++;
	g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uAllocCount++;
	g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount++;

	/* element statistics */
	if(g_EpsMemStat[0][mem_type].uMinAllocSizeElement == 0)
		g_EpsMemStat[0][mem_type].uMinAllocSizeElement = size;
	else if(g_EpsMemStat[0][mem_type].uMinAllocSizeElement > size)
		g_EpsMemStat[0][mem_type].uMinAllocSizeElement = size;

	if(g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement == 0)
		g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement = size;
	else if(g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement > size)
		g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement = size;

	if(g_EpsMemStat[0][mem_type].uMaxAllocSizeElement < size)
		g_EpsMemStat[0][mem_type].uMaxAllocSizeElement = size;

	if(g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSizeElement < size)
		g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSizeElement = size;

	/* alloc sums */
	g_EpsMemStat[0][mem_type].uSumAllocSize += size;
	g_EpsMemStat[lsa_comp_id][mem_type].uSumAllocSize += size;

	/* worst */
	if(g_EpsMemStat[0][mem_type].uActAllocSize > g_EpsMemStat[0][mem_type].uMaxAllocSize)
		g_EpsMemStat[0][mem_type].uMaxAllocSize = g_EpsMemStat[0][mem_type].uActAllocSize;

	if(g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize > g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize)
		g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize = g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize;

	if(g_EpsMemStat[lsa_comp_id][mem_type].uActAllocSize > g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSize)
		g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSize = g_EpsMemStat[lsa_comp_id][mem_type].uActAllocSize;

	if(g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize > g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize)
		g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize = g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize;

	#ifdef EPS_USE_MEM_TRACE
	/* do enqueuing */
	ppFirst = &g_EpsMemStat[lsa_comp_id][mem_type].first;
	if(mem_type == PSI_MTYPE_NRT_MEM) ppFirst = &g_EpsMemStat[0][mem_type].first;
	prefix->next = *ppFirst;
	prefix->prev = LSA_NULL;
	if(*ppFirst != LSA_NULL)
		(*ppFirst)->prev = prefix;
	*ppFirst = prefix;
	postfix->next = prefix->next;
	postfix->prev = prefix->prev;
	#endif

	eps_mem_unlock();

	return (LSA_VOID_PTR_TYPE)(ptr+sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE));
}

/**
 * free memory and update memory statistic
 * 
 * @param ptr       pointer to memory to be freed
 * @param comp_id   
 * @param mem_type  type of memory
 * @param pool
 * @return LSA_RET_OK
 * @return LSA_RET_ERR_RESOURCE
 */
static LSA_UINT16 eps_mem_free_with_statistic ( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 comp_id, LSA_UINT32 mem_type, LSA_INT pool )
{
	LSA_UINT32 size;
	LSA_UINT32 align4;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE prefix;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE postfix;
	
	#ifdef EPS_USE_MEM_TRACE
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE *ppFirst;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE cprefix;
	#endif

	EPS_ASSERT(ptr != 0);

	ptr = (LSA_UINT8*)ptr - sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE);
	prefix = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE)(ptr);

	size = GetBufSize( ptr );

	if ( size > 0 )
	{
		EPS_ASSERT(size > sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE));

		EPS_ASSERT(prefix->comp_id  <= EPS_LSA_COMP_ID_LAST);     /* Comp Id must be within range  */
		EPS_ASSERT(prefix->mem_type <  EPS_LSA_COUNT_MEM_TYPES);  /* Mem Type must be within range */

		size = prefix->size;

		align4 = 0;
		if(prefix->size%4)
			align4 = (4-prefix->size%4);

		postfix = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE) ((LSA_UINT8*)ptr + sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) + size + align4);

		eps_mem_lock();

		if(comp_id != 0 && mem_type != PSI_MTYPE_NRT_MEM)
		{
			EPS_ASSERT(prefix->comp_id == comp_id);
		}
		/* prefix / postfix check => check overwrites */
		EPS_ASSERT(prefix->mem_type == mem_type);
		EPS_ASSERT(prefix->pool     == pool);
		EPS_ASSERT(prefix->magic    == 0x12344321);
		EPS_ASSERT(prefix->magic    == postfix->magic);
		EPS_ASSERT(prefix->comp_id  == postfix->comp_id);
		EPS_ASSERT(prefix->mem_type == postfix->mem_type);
		EPS_ASSERT(prefix->pool     == postfix->pool);
		EPS_ASSERT(prefix->size     == postfix->size);

		/* Memory statistic checks */
		EPS_ASSERT(g_EpsMemStat[0][mem_type].uActAllocCount > 0);
		EPS_ASSERT(g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount > 0);
		EPS_ASSERT(g_EpsMemStat[comp_id][mem_type].uActAllocCount > 0);
		EPS_ASSERT(g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount > 0);

		if(mem_type != PSI_MTYPE_NRT_MEM)
		{
			EPS_ASSERT(g_EpsMemStat[comp_id][mem_type].uActAllocSize >= size);
			EPS_ASSERT(g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize >= size);
		}

		EPS_ASSERT(g_EpsMemStat[0][mem_type].uActAllocSize >= size);
		EPS_ASSERT(g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize >= size);

		/* Update Memory Statistic */
		if(mem_type != PSI_MTYPE_NRT_MEM)
		{
			g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize -= size;
			g_EpsMemStat[comp_id][mem_type].uActAllocSize -= size;
		}
		
		g_EpsMemStat[0][mem_type].uActAllocSize -= size;
		g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize -= size;

		g_EpsMemStat[0][mem_type].uActAllocCount--;
		g_EpsMemStat[comp_id][mem_type].uActAllocCount--;
		g_EpsMemStat[0][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount--;
		g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount--;

		#ifdef EPS_USE_MEM_TRACE
		/* do dequeuing */
		ppFirst = &g_EpsMemStat[comp_id][mem_type].first;
		if(mem_type == PSI_MTYPE_NRT_MEM) ppFirst = &g_EpsMemStat[0][mem_type].first;
		cprefix = *ppFirst;
		while(cprefix != prefix && cprefix != 0)
			cprefix = cprefix->next;
		EPS_ASSERT(cprefix == prefix); /* not found */
		if(cprefix->prev != LSA_NULL)
			cprefix->prev->next = cprefix->next;
		else
			*ppFirst = cprefix->next;
		if(cprefix->next != LSA_NULL)
			cprefix->next->prev = cprefix->prev;
		#endif

		eps_mem_unlock();

		Mem3free( ptr );
		return LSA_RET_OK;
	}

	EPS_FATAL("Size of memory to be freed must be greater then zero");

	return LSA_RET_ERR_RESOURCE;
}

#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE eps_mem_init(LSA_UINT32* pStdMemBase, LSA_UINT32 uStdMemSize, LSA_UINT32* pFastMemBase, LSA_UINT32 uFastMemSize)
{
	LSA_RESPONSE_TYPE retVal = LSA_RET_OK;

	// Initialize Global Mem Structure
	eps_memset( &g_EpsMem, 0 , sizeof( EPS_MEM_INSTANCE_TYPE ));
	g_pEpsMem = &g_EpsMem;

	uStdMemSize  = uStdMemSize - uStdMemSize%4;
	uFastMemSize = uFastMemSize - uFastMemSize%4;

	// Init Locks
	retVal = eps_alloc_critical_section(&g_pEpsMem->hMemLock, LSA_TRUE); //TODO !!!AH eps_alloc_critical_section(&g_pEpsMem->hMemLock, LSA_FALSE);
	EPS_ASSERT( retVal == LSA_RET_OK );

	// Fast Mem configuration

	if( (pFastMemBase == LSA_NULL) &&
		(uFastMemSize != 0) )
	{
		uStdMemSize -= uFastMemSize;
		pFastMemBase = (LSA_UINT32*)((LSA_UINT8*)pStdMemBase + uStdMemSize);
	}

	retVal = eps_mempool_init(EPS_FAST_MEMORY_POOL_ID, pFastMemBase, uFastMemSize, &g_pEpsMem->sFastMem);
	EPS_ASSERT( retVal == LSA_RET_OK );

	// Local Mem configuration

	EPS_ASSERT( pStdMemBase != 0 );
	EPS_ASSERT( uStdMemSize != 0 );
	
	retVal = eps_mempool_init(EPS_LOCAL_MEMORY_POOL_ID, pStdMemBase, uStdMemSize, &g_pEpsMem->sStdMem);
	EPS_ASSERT( retVal == LSA_RET_OK );

	g_pEpsMem->bInitialized = LSA_TRUE;

	return retVal;
}

LSA_VOID eps_mem_undo_init( LSA_VOID )
{
	LSA_UINT8* pFastMemBase;

	EPS_ASSERT( g_pEpsMem != LSA_NULL );
	EPS_ASSERT( g_pEpsMem->bInitialized == LSA_TRUE );

	pFastMemBase = g_pEpsMem->sFastMem.pBase;

	eps_mempool_undo_init(&g_pEpsMem->sFastMem);

	eps_mempool_undo_init(&g_pEpsMem->sStdMem);

	eps_free_critical_section(g_pEpsMem->hMemLock);

	g_pEpsMem->bInitialized = LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID eps_mem_lock( LSA_VOID )
{
	eps_enter_critical_section(g_pEpsMem->hMemLock);
}

LSA_VOID eps_mem_unlock( LSA_VOID )
{
	eps_exit_critical_section(g_pEpsMem->hMemLock);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE eps_mempool_init( LSA_UINT16 pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, EPS_MEM_BLOCK_PTR_TYPE pMem )
{
	LSA_UINT32 granSize, fftThreshold;
	LSA_VOID *pHelpMem;
	LSA_VOID *pBaseMem;

	EPS_ASSERT( pBase != LSA_NULL );
	EPS_ASSERT( pMem  != LSA_NULL );
	EPS_ASSERT( uSize != 0 );
	EPS_ASSERT( pMem->bInitialized == LSA_FALSE );

	// create mem3 pool
	pMem->pBase       = pBase;
	pMem->pool_id     = pool_id;
	pMem->pHelperMem  = LSA_NULL;
	pMem->uHelperSize = 0;
	pMem->pool_nr     = -1;

	granSize     = 0x20; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 0x0;

	pMem->uHelperSize = PoolHelperSize3( uSize, granSize, Fft2Bound, LSA_TRUE, 8 );
	pMem->uHelperSize = EPS_MEM_ALIGN4( pMem->uHelperSize );

	EPS_ASSERT( pMem->uHelperSize < uSize ); 

	pMem->pHelperMem = (LSA_UINT32*)((LSA_UINT8*)pBase + uSize - pMem->uHelperSize);
	pMem->uSize = uSize - pMem->uHelperSize;

	// Note: assignment is necessary because ptr are modified in mem3!
	pHelpMem = pMem->pHelperMem;
	pBaseMem = pMem->pBase;

	SetMemSetFunc( pool_id, (EPS_MEM_MEMSET_FCT)eps_memset );
	SetCheckOverwrite( pool_id, LSA_TRUE );   // enable overwrite check
	SetPoolAlignment( pool_id, 8 );           // DWord allignment

	pMem->pool_nr  = CreatePool ( &pBaseMem, &pHelpMem, pMem->uSize, pMem->uHelperSize,
		granSize, fftThreshold, Fft2Bound, pMem->pool_id);

	EPS_ASSERT( pMem->pool_nr >= 0 );
	EPS_ASSERT( PoolValid( pMem->pool_nr ) );

	pMem->bInitialized = LSA_TRUE;

	return LSA_RET_OK;
}

static LSA_VOID eps_mempool_undo_init( EPS_MEM_BLOCK_PTR_TYPE pMem )
{
	int result;
	int BufNumUsed = 0;

	// delete the memory pool, and the helper memory
	EPS_ASSERT( pMem != LSA_NULL );
	EPS_ASSERT( pMem->bInitialized == LSA_TRUE );

	BufNumUsed = BufNumUsedInPool( pMem->pool_nr );

	EPS_ASSERT( BufNumUsed == 0 ) /* Are there any buffers not freed? */
	
	result = DeletePool( pMem->pool_nr );
	EPS_ASSERT( result == 0 );
		
	pMem->pHelperMem   = 0;
	pMem->pool_nr      = -1;
	pMem->pBase        = 0;
	pMem->uSize        = 0;
	pMem->bInitialized = LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE eps_mem_alloc_from_pool( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool )
{
	LSA_VOID_PTR_TYPE ptr;

  if (size == 0)
  {
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL,
        "eps_mem_alloc_from_pool: size(%u)!!!, lsa_comp_id(%u), mem_type(%u), pool(%d)", 
        size, lsa_comp_id, mem_type, pool);

    EPS_FATAL("eps_mem_alloc_from_pool(): Allocation with size == 0");
  }

	#ifdef EPS_USE_MEM_STATISTIC
	ptr = eps_mem_alloc_with_statistic(size, 0, 0, lsa_comp_id, mem_type, pool);
	#else
	LSA_UNUSED_ARG(mem_type);
	LSA_UNUSED_ARG(lsa_comp_id);

	ptr = Mem3palloc ( size, pool );
	#endif

	if (ptr == LSA_NULL)
	{
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,
        "eps_mem_alloc_from_pool(): Allocation returns 0-ptr!!!  size(%u), lsa_comp_id(%u), mem_type(%u), pool(%d)", 
        size, lsa_comp_id, mem_type, pool);
	}

	return ( ptr );
}

LSA_UINT16 eps_mem_free_from_pool( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool )
{
	#ifdef EPS_USE_MEM_STATISTIC
	eps_mem_free_with_statistic(ptr, lsa_comp_id, mem_type, pool);
	return LSA_RET_OK;
	#else
	LSA_UNUSED_ARG(mem_type);
	LSA_UNUSED_ARG(lsa_comp_id);
	LSA_UNUSED_ARG(pool);

	EPS_ASSERT( ptr != 0 );
	if ( GetBufSize( ptr ) > 0 ) // we have a valid buffer ?
	{
		Mem3free( ptr );
		return LSA_RET_OK;
	}
	return LSA_RET_ERR_RESOURCE;
	#endif
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE eps_mem_alloc( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type )
{
	return eps_mem_alloc_from_pool(size, lsa_comp_id, mem_type, g_pEpsMem->sStdMem.pool_id);
}

LSA_UINT16 eps_mem_free( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type )
{
	return eps_mem_free_from_pool(ptr, lsa_comp_id, mem_type, g_pEpsMem->sStdMem.pool_id);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE eps_fast_mem_alloc( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type )
{
	LSA_UNUSED_ARG(mem_type); /* Mem Types not supported for fast mem */
	LSA_UNUSED_ARG(lsa_comp_id);

	return eps_mem_alloc_from_pool(size, 0 /* Don't Check CompId */, 0, g_pEpsMem->sFastMem.pool_id);
}

LSA_UINT16 eps_fast_mem_free(LSA_VOID_PTR_TYPE ptr )
{
	return eps_mem_free_from_pool(ptr, 0 /* Don't Check CompId */, 0, g_pEpsMem->sFastMem.pool_id);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
