/*** system adaption for pcIOX ***/

#ifdef TOOL_CHAIN_MICROSOFT
# pragma pack(pop, IPSTACK_PACK2) /* restore */

#elif defined(TOOL_CHAIN_GNU)
//# pragma pack(pop, IPSTACK_PACK2) /* restore */  //(LaM) commented because of rmos gnu
#pragma pack()

#else
# error "check this"
#endif

/*** end of file ***/
