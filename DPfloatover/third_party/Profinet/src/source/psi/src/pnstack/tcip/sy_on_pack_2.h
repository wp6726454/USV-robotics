/*** system adaption for pcIOX ***/

#ifdef TOOL_CHAIN_MICROSOFT
# pragma warning(disable:4103) /* used #pragma pack to change alignment */
# pragma pack(push, IPSTACK_PACK2, 2) /* 2 byte packing */

#elif defined(TOOL_CHAIN_GNU)
//# pragma pack(push, IPSTACK_PACK2, 2) /* 2 byte packing */  //(LaM) commented because of rmos gnu
#pragma pack(2)

#else
# error "check this"
#endif

/*** end of file ***/
