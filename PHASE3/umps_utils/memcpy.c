#include "memcpy.h"
void memcpy(state_t *to, state_t *from){
	to->entry_hi = from->entry_hi;
	to->cause = from->cause;
	to->status = from->status;
	to->pc_epc = from->pc_epc;
	to->hi = from->hi;
	to->lo = from->lo;
	for (int i=0; i < STATE_GPR_LEN; i++) {
		to->gpr[i] = from->gpr[i];
	}
}

/* Fatta da noi ma non `e una vera memcpy

void *memcpy(void* dst, void* src, unsigned int cnt)
{
    char *pszDest = (char *)dst;
    const char *pszSource =( const char*)src;
    if((pszDest!= NULL) && (pszSource!= NULL))
    {
        while(cnt) //till cnt
        {
            //Copy byte by byte
            *(pszDest++)= *(pszSource++);
            --cnt;
        }
    }
    return dst;
}

void memcpy (pcb_t* current_process, state_t *state_reg){
	current_process->p_s.entry_hi = state_reg->entry_hi;
	current_process->p_s.cause = state_reg->cause;
	current_process->p_s.status = state_reg->status;
	current_process->p_s.pc_epc = state_reg->pc_epc;
	current_process->p_s.hi = state_reg->hi;
	current_process->p_s.lo = state_reg->lo;
	for (int i=0; i < STATE_GPR_LEN; i++) {
		current_process->p_s.gpr[i] = state_reg->gpr[i];
	}
}

*/

/*
**
 * A simple memcpy implementation. Copies n bytes from the from address to the to address.
 *
 * @param to The source first address 
 * @param from The destination first address
 * @param n How many bytes to copy
 **
void memcpy(void* to, void* from, size_tt n){
  // since we want to copy byte by byte, treat both pointers as char*
  char *cfrom = (char *)from;
  char *cto = (char *)to;
  // copy byte by byte
  for (int i=0; i<n; i++)
    cto[i] = cfrom[i];
}

current_process->entry_hi = state_reg->entry_hi;
	current_process->cause = state_reg->cause;
	current_process->status = state_reg->status;
	current_process->pc_epc = state_reg->pc_epc;
	current_process->hi = state_reg->hi;
	current_process->lo = state_reg->lo;
	for (int i=0; i < STATE_GPR_LEN; i++) {
		current_process->gpr[i] = state_reg->gpr[i];
	}
	
	
	
	--------------------------
*
void memcpy(void *dest, void *src, unsigned long n)
{
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
  
   // Copy contents of src[] to dest[]
   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}

void *memcpy(void* dst, void* src, unsigned int cnt)
{
    char *pszDest = (char *)dst;
    const char *pszSource =( const char*)src;
    if((pszDest!= NULL) && (pszSource!= NULL))
    {
        while(cnt) //till cnt
        {
            //Copy byte by byte
            *(pszDest++)= *(pszSource++);
            --cnt;
        }
    }
    return dst;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}*/


