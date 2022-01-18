typedef struct pcb_t {
/* process queue */
    struct list_head p_next;
/* process tree fields */
    struct pcb_t *p_parent;  /* ptr to parent */
   struct list_head p_child;  /* children list   */
   struct list_head p_sib;     /* sibling list    */
/* process status information */
    state_t p_s; /* processor state */
    cpu_t p_time; /* cpu time used by proc */
    int *p_semAdd; /* ptr to semaphore on which proc is blocked*/
}  pcb_t, *pcb_PTR;
