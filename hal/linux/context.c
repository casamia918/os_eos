#include <core/eos.h>
#include <core/eos_internal.h>

typedef struct _os_context {
	int32u_t reg[10];
} _os_context_t;


void print_context(_os_context_t *context) {
	if(context == NULL) return;
	PRINT("edi  =0x%x\n", context->reg[0]);
	PRINT("esi  =0x%x\n", context->reg[1]);
	PRINT("ebp  =0x%x\n", context->reg[2]);
	PRINT("esp  =0x%x\n", context->reg[3]);
	PRINT("ebx  =0x%x\n", context->reg[4]);
	PRINT("edx  =0x%x\n", context->reg[5]);
	PRINT("ecx  =0x%x\n", context->reg[6]);
	PRINT("eax  =0x%x\n", context->reg[7]);
	PRINT("eflags  =0x%x\n", context->reg[8]);
	PRINT("eip  =0x%x\n", context->reg[9]);
}

void dbg_save_context(addr_t stack_top, _os_context_t* ctx){
	int i;
	for(i=0; i<10; i++) {
		ctx->reg[i] = *((int32u_t*)stack_top +i);
	}
}

_os_context_t ctx_queue[3];
int ctx_queue_count = 0;

addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg) {

	int index;
	
//	int32u_t size_32u = 8096; //stack_size/4;

	int32u_t* stack_end = (int32u_t*)stack_base + stack_size -1;

	*stack_end = (int32u_t)arg;
	*(stack_end-1) = (int32u_t)NULL;
	*(stack_end-2) = (int32u_t)entry;
	for(index=3; index<=11; index++){
		*(stack_end-index) = (int32u_t)NULL;
	}
	ctx_queue_count++;
	return (stack_end-11);

}


void _os_restore_context(addr_t sp){
	__asm__ __volatile__(
		"movl %0, %%esp															\n\t"
		"popa																		\n\t"
		"popf 																\n\t"
		"ret" 
		:: "m"(sp)	
		);
}


addr_t _os_save_context() {

	__asm__ __volatile__ ("\
		push $resume_point;\
		pushf;\
		movl $0, %eax;\
		pusha;\
		movl %esp, %eax;\
		push 4(%ebp);\
		push (%ebp);\
		movl %esp, %ebp;\
		resume_point:\
		leave;\
		ret;"

	);
	return NULL;
}
