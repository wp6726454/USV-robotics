#ifndef CM_LIST_H
#define CM_LIST_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_list.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  List macros                                                              */
/*                                                                           */
/*****************************************************************************/


/*------------------------------------------------------------------------------
// circularly linked list macros
// (copy/paste from CLRPC)
//---------------------------------------------------------------------------*/

struct cm_list_entry_tag {
	struct cm_list_entry_tag CM_COMMON_MEM_ATTR * Flink; /* forward link */
	struct cm_list_entry_tag CM_COMMON_MEM_ATTR * Blink; /* backward link */
};

typedef struct cm_list_entry_tag  CM_LIST_ENTRY_TYPE;

typedef struct cm_list_entry_tag  CM_COMMON_MEM_ATTR *CM_LIST_ENTRY_PTR_TYPE;

/*----------------------------------------------------------------------------*/

#if 0 /*! not yet !*/
#define CM_LIST_TYPED_ENTRY_TYPE(tag) \
	union { /* a union of a typed and an untyped list-linkage */ \
		struct { \
			struct tag CM_COMMON_MEM_ATTR * Flink; \
			struct tag CM_COMMON_MEM_ATTR * Blink; \
		} t; /* typed part */ \
		struct cm_list_entry_tag u; /* untyped part */ \
	} /*name*/

typedef struct cm_list_empty_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_list_empty_tag) link;
} CM_LIST_EMPTY_TYPE;

typedef CM_LIST_EMPTY_TYPE  CM_COMMON_MEM_ATTR * CM_LIST_EMPTY_PTR_TYPE;

#else

#define CM_LIST_TYPED_ENTRY_TYPE(tag)  CM_LIST_ENTRY_TYPE

#endif

/*----------------------------------------------------------------------------*/

#define CmListInitialize(ListHead_) {\
	(ListHead_)->Flink = (ListHead_)->Blink = (ListHead_);\
	}

#define CmListIsEmpty(ListHead_) (\
	LSA_HOST_PTR_ARE_EQUAL((ListHead_)->Flink, (ListHead_))\
	)

#define CmListRemoveEntry(Entry_) {\
	CM_LIST_ENTRY_PTR_TYPE EX_Entry_;\
	EX_Entry_ = (Entry_);\
	EX_Entry_->Blink->Flink = EX_Entry_->Flink;\
	EX_Entry_->Flink->Blink = EX_Entry_->Blink;\
	}

#define CmListRemoveHead(ListHead_, Entry_, Type_) {\
	(Entry_) = (Type_)((ListHead_)->Flink);\
	CmListRemoveEntry((ListHead_)->Flink);\
	}

#define CmListInsertHead(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_)->Flink;\
	(Entry_)->Blink = (ListHead_);\
	(ListHead_)->Flink->Blink = (Entry_);\
	(ListHead_)->Flink = (Entry_);\
	}

#define CmListInsertTail(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_);\
	(Entry_)->Blink = (ListHead_)->Blink;\
	(ListHead_)->Blink->Flink = (Entry_);\
	(ListHead_)->Blink = (Entry_);\
	}

#define CmListInsertBefore(Existing_, Entry_) {\
	CmListInsertTail (Existing_, Entry_);\
	}

#define CmListFirst(ListHead_, Type_) (\
	(Type_)(CmListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Flink)\
	)

#define CmListNext(ListHead_, Entry_, Type_) (\
	(Type_)(LSA_HOST_PTR_ARE_EQUAL((Entry_)->Flink, (ListHead_)) ? LSA_NULL : (Entry_)->Flink)\
	)

#define CmListPrevOrHead(Entry_, Type_) (\
	(Type_)((Entry_)->Blink)\
	)

#define CmListLength(ListHead_, LenPtr_) {\
	CM_LIST_ENTRY_PTR_TYPE el = CmListFirst (ListHead_, CM_LIST_ENTRY_PTR_TYPE);\
	*(LenPtr_) = 0;\
	while (! LSA_HOST_PTR_ARE_EQUAL(el,LSA_NULL)) {\
		*(LenPtr_) += 1;\
		el = CmListNext (ListHead_, el, CM_LIST_ENTRY_PTR_TYPE);\
	}}

#define CmListAppend(ListHead_, ListHeadToAppend_) {\
	if (! CmListIsEmpty (ListHeadToAppend_)) {\
		(ListHead_)->Blink->Flink = (ListHeadToAppend_)->Flink;\
		(ListHeadToAppend_)->Flink->Blink = (ListHead_)->Blink;\
		(ListHead_)->Blink = (ListHeadToAppend_)->Blink;\
		(ListHead_)->Blink->Flink = (ListHead_);\
		CmListInitialize (ListHeadToAppend_);\
	}}

/*----------------------------------------------------------------------------*/

#define CmListInitializeT(ListHead_) {\
	CmListInitialize (/*TypedList*/(CM_LIST_ENTRY_PTR_TYPE)(ListHead_));\
	}

#define CmListInsertTailT(ListHead_, Entry_) {\
	CmListInsertTail (/*TypedList*/(CM_LIST_ENTRY_PTR_TYPE)(ListHead_), (CM_LIST_ENTRY_PTR_TYPE)(Entry_)); \
	}

#define CmListRemoveHeadT(ListHead_, Entry_, Type_) {\
	CmListRemoveHead (/*TypedList*/(CM_LIST_ENTRY_PTR_TYPE)(ListHead_), Entry_, Type_); \
	}

/*----------------------------------------------------------------------------*/

/* usage: for (CmListForeach(...)) { ... } -- variant 1 */
#define CmListForeach(Var_, ListHead_, Type_) \
		(Var_) = CmListFirst (ListHead_, Type_); \
		! LSA_HOST_PTR_ARE_EQUAL((Var_), LSA_NULL); \
		(Var_) = CmListNext (ListHead_, (CM_LIST_ENTRY_PTR_TYPE)(Var_), Type_)

/* usage: for( CmListEach(...) ) { ... } -- variant 2 */
#define CmListEach(Var_, ListHead_, Type_) \
	(Var_) = CmListFirst(ListHead_, Type_); \
	! LSA_HOST_PTR_ARE_EQUAL((Var_), LSA_NULL); \
	(Var_) = CmListNext(ListHead_, (CM_LIST_ENTRY_PTR_TYPE)(Var_), Type_)

/*----------------------------------------------------------------------------*/

typedef  LSA_BOOL  CM_LOCAL_FCT_ATTR	/* return (ListEntry->field < NewEntry->field); */
CmListCmpFunctionType (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	);
/* naming convention: cm_list_cmp__<struct>__<field> */

LSA_VOID  CM_LOCAL_FCT_ATTR
CmListInsertSorted (
	CM_LIST_ENTRY_PTR_TYPE  ListHead,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry,
	CmListCmpFunctionType  Compare_ListEntry_LT_NewEntry
	); /* implemented in cm_cmn.c */

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_LIST_H */
