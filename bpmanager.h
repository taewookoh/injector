void setup_bp(int pid, unsigned long bp, unsigned long after_bp, void* after_bp_addr, void* org_inst_addr, unsigned target, void* target_addr);
void suppress_bp(int pid, unsigned long bp);
void setup_sigtrap_handler_ret_bp(int pid, unsigned long bp);
void suppress_sigtrap_handler_ret_bp(int pid, unsigned long bp);
