#ifndef PND_BGZD_STORE_H
#define PND_BGZD_STORE_H
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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_BgzdStore.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of BGZD info storage classes                                  */
/*                                                                           */
/*****************************************************************************/

//---------------------------------------------------------------------------
// BGZD submod specifc states
#define PND_BGZD_SUB_OK          0x0000  // BGZD OK for a subslot
#define PND_BGZD_SUB_BAD_IOXS    0x0001  // BAD IOXS
#define PND_BGZD_SUB_NOT_EXISTS  0x0002  // Not exists
#define PND_BGZD_SUB_WRONG       0x0004  // Wrong submod
#define PND_BGZD_SUB_ARP         0x0008  // submod has A.R.P
#define PND_BGZD_SUB_LOCKED_BY   0x0010  // submod is locked
#define PND_BGZD_SUB_DIAG_ERR    0x0020  // submod has error diag
#define PND_BGZD_SUB_DIAG_MR     0x0040  // submod has MR diag
#define PND_BGZD_SUB_DIAG_MD     0x0080  // submod has MD diag
#define PND_BGZD_SUB_DIAG_QUA    0x0100  // submod has Qualified diag

// BGZD PNIO user specifc states
// Note: not all states are used by specific user
#define PND_BGZD_USER_OK            0x0000  // BGZD OK over all PNIO user
#define PND_BGZD_OHA_IP_BAD         0x0001  // IP-Suite conflict
#define PND_BGZD_OHA_NOS_BAD        0x0002  // NameOfStation conflict
#define PND_BGZD_PD_NO_LINK_UP      0x0004  // All ports has link down
#define PND_BGZD_IOD_AR_NOT_INDATA  0x0008  // No AR, or any AR not INDATA
#define PND_BGZD_IOC_DEV_PASSIVATED 0x0010  // Any device passivated
#define PND_BGZD_IOC_AR_NOT_INDATA  0x0020  // Any AR not INDATA
#define PND_BGZD_IOM_DEV_PASSIVATED 0x0040  // Any MC Device passivated
#define PND_BGZD_IOM_AR_NOT_INDATA  0x0080  // Any MC AR not INDATA

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

typedef LSA_VOID (*PND_BGZD_LED_FCT)(LSA_UINT16, LSA_UINT16, LSA_BOOL);

//---------------------------------------------------------------------------
typedef enum pnd_led_category_enum
{
	LED_CATEGORY_SF    = 0x00,  // SF LED
	LED_CATEGORY_BF    = 0x01,  // BF LED
	LED_CATEGORY_MAINT = 0x03,  // MAINT LED
	LED_CATEGORY_MAX 
} PND_LED_CATEGORY_TYPE;

//---------------------------------------------------------------------------
typedef enum pnd_led_state_enum
{
	LED_STATE_OFF      = 0x00,  // LED off
	LED_STATE_ON       = 0x01,  // LED on
	LED_STATE_TOGGLE   = 0x02,  // LED toggles between off/on
	LED_STATE_MAX 
} PND_LED_STATE_TYPE;

//---------------------------------------------------------------------------
typedef struct pnd_led_tag  // LED info storage type
{
	PND_LED_CATEGORY_TYPE led_category;   // category of LED
	PND_LED_STATE_TYPE    led_state;      // state of LED
	LSA_UINT16            gpio_nr;        // GPIO nr for triggering the LED
	PND_BGZD_LED_FCT      p_led_fct;      // LED function to set to on/off
} PND_LED_STORE_TYPE;

//---------------------------------------------------------------------------
typedef enum bgzd_type_enum // Possible BGZD states
{
	BGZD_OK                  = 0,   // OK
	BGZD_IOD_AR_FAULT        = 1,   // NoAr or One AR not INDATA on IOD
	BGZD_MAINTENACE_DEMANDED = 2,   // Faulty (Diag: MD)
	BGZD_MAINTENACE_REQUIRED = 3,   // Faulty (Diag: MR)
	BGZD_FAULTY              = 4,   // Faulty (Diag: ERROR/MD/MR)
	BGZD_NOT_AVAILABLE       = 5,   // Not avaliable
	BGZD_NOT_EXIST           = 6,   // State not exist
	BGZD_DEACTIVATED         = 7,   // State deactivated
	BGZD_STATE_MAX                  // Enum max value
} PND_BGZD_STATE_TYPE;

/////////////////////////////////////////////////////////////////////////////
// CBgzdInfoStore  class to holds the BGZD information
/////////////////////////////////////////////////////////////////////////////

class CBgzdInfoStore
{
	// Attributes
protected:
	LSA_UINT16 m_subslotState;       // State field for possible subslot states
	LSA_UINT16 m_userState;          // State field for possible PNIO user (OHA,PD,IOC,IOM,IOD) states

	// Construction/ Destruction
public:
	CBgzdInfoStore();
	virtual ~CBgzdInfoStore();

	// Accessors Write for BGZD Info States
	LSA_VOID Reset();
	LSA_VOID UpdateBgzdInfo( CBgzdInfoStore& bgzdInfo );
	LSA_VOID UpdateFromSubslotState( LSA_UINT16 submodState );
	LSA_VOID UpdateFromDiagnosisList( LSA_UINT16 cntError, LSA_UINT16 cntMR, LSA_UINT16 cntMD, LSA_UINT16 cntQual );
	LSA_VOID UpdateFromAlarm( LSA_BOOL hasDiag, LSA_UINT32 maintState );
	LSA_VOID UpdateFromLedInfo( LSA_VOID* pLedInfo );
	LSA_VOID SetNotExists( LSA_BOOL state );
	LSA_VOID SetWrong( LSA_BOOL state );
	LSA_VOID SetARP( LSA_BOOL state );
	LSA_VOID SetLocked( LSA_BOOL state );
	LSA_VOID SetBadIOXS( LSA_BOOL state );
	LSA_VOID SetNoLinkUp( LSA_BOOL state );
	LSA_VOID SetBadIP( LSA_BOOL state );
	LSA_VOID SetBadNoS( LSA_BOOL state );
	LSA_VOID SetArStateIOC( LSA_BOOL notIndata, LSA_BOOL passivated );
	LSA_VOID SetArStateIOM( LSA_BOOL notIndata, LSA_BOOL passivated );

	// Accesors Read for BGZD Info States
	LSA_BOOL             IsNotPlugged();
	PND_BGZD_STATE_TYPE  GetBgzdState();
	string               GetBgzdSubslot();
	string               GetBgzdUser();

	// Accessor for summary BGZD
	LSA_BOOL IsOkState();
	LSA_BOOL IsDeactivatedState();
	LSA_BOOL IsNotExistsState();
	LSA_BOOL IsNotAvaliableState();
	LSA_BOOL IsFaultyState();

private:
	LSA_VOID dumpBgzd();
};

/////////////////////////////////////////////////////////////////////////////
// CBgzdLedStore  class to holds the BGZD LED states
/////////////////////////////////////////////////////////////////////////////

class CBgzdLedStore : public CBgzdInfoStore
{
	// Attributes
private:
	LSA_UINT16          m_boardNr;          // Board number
	LSA_BOOL            m_toggleOn;         // Toggle state for blink mode
	LSA_BOOL            m_isShutdown;       // Shutdown active yes/no
	LSA_TIMER_ID_TYPE   m_timerId;          // Timer-ID for Toggle trigger
	PND_LED_STORE_TYPE  m_LedSF;            // SF LED storage
	PND_LED_STORE_TYPE  m_LedBF;            // BF LED storage
	PND_LED_STORE_TYPE  m_LedMAINT;         // MAINT LED storage

	// Construction/ Destruction
public:
	CBgzdLedStore();
	virtual ~CBgzdLedStore();

	// Accessors
	LSA_VOID SetBoardNr( LSA_UINT16 boardNr );
	LSA_VOID RegisterLedBF( PND_BGZD_LED_FCT pFunc, LSA_UINT16 gpioNr );
	LSA_VOID RegisterLedSF( PND_BGZD_LED_FCT pFunc, LSA_UINT16 gpioNr );
	LSA_VOID RegisterLedMAINT( PND_BGZD_LED_FCT pFunc, LSA_UINT16 gpioNr );

	// methodes
	LSA_VOID Startup();
	LSA_VOID Shutdown();
	LSA_VOID UpdateLED();
	LSA_VOID SetLED( PND_LED_STORE_TYPE& led, LSA_BOOL state );

	// Class methodes for timer (toggle on/off)
	static LSA_VOID EventTimeout( LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id );

private:
	PND_LED_STATE_TYPE getLedStateBF();
	PND_LED_STATE_TYPE getLedStateSF();
	PND_LED_STATE_TYPE getLedStateMAINT();
	LSA_BOOL isLedOn( PND_LED_STATE_TYPE& ledState );
};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_BGZD_STORE_H */
