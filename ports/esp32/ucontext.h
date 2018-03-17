
// Dummy file to satisfy the go runtime written in C.

struct stack_t {
	void  *ss_sp;
	int    ss_flags;
	size_t ss_size;
};

typedef struct stack_t stack_t;

struct ucontext_t {
	struct ucontext_t *uc_link;
	stack_t uc_stack;
};

typedef struct ucontext_t ucontext_t;

int  setcontext(const ucontext_t *);
int  getcontext(ucontext_t *);
void makecontext(ucontext_t *, void *, int, ...);
int  swapcontext(ucontext_t *, const ucontext_t *);
