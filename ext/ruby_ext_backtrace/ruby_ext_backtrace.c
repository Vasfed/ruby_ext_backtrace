#include <ruby.h>

#define TRUE 1
#define FALSE 0

#ifdef RUBY_VM
#include "vm_core.h"
//#include "eval_intern.h"
#else
#error Need RubyVM, use another ruby
#endif

#ifdef HAVE_RUBY_CURRENT_THREAD
extern VALUE ruby_engine_name;
#else
#define ruby_engine_name Qnil
#define ruby_current_thread ((rb_thread_t *)RTYPEDDATA_DATA(rb_thread_current()))
#endif

typedef int (rb_backtrace_iter_ext_func)(void *arg, VALUE file, int line, VALUE method_name, int argc, VALUE* argv);

static int
vm_backtrace_each_ext(rb_thread_t *th, int lev, void (*init)(void *), rb_backtrace_iter_ext_func *iter, void *arg)
{
  const rb_control_frame_t *limit_cfp = th->cfp;
  const rb_control_frame_t *cfp = (void *)(th->stack + th->stack_size);
  VALUE file = Qnil;
  int line_no = 0;

  cfp -= 2;
  //skip lev frames:
  while (lev-- >= 0) {
    if (++limit_cfp > cfp)
        return FALSE;
  }

  if (init) (*init)(arg);

  limit_cfp = RUBY_VM_NEXT_CONTROL_FRAME(limit_cfp);
  if (th->vm->progname) file = th->vm->progname;

  while (cfp > limit_cfp) {
    if (cfp->iseq != 0) {
        if (cfp->pc != 0) {
          rb_iseq_t *iseq = cfp->iseq;

          line_no = rb_vm_get_sourceline(cfp);
          file = iseq->filename;

          //arguments pushed this way: *reg_cfp->sp++ = recv; for (i = 0; i < argc; i++) *reg_cfp->sp++ = argv[i];
          //local vars = cfp->iseq->local_size - cfp->iseq->arg_size;
          //in memory: receiver params locals (bp(incremented))
          VALUE* argv = &cfp->bp[- cfp->iseq->local_size - 1];
          if ((*iter)(arg, file, line_no, iseq->name, cfp->iseq->arg_size, argv)) break;
        }
    } else
      if (RUBYVM_CFUNC_FRAME_P(cfp)) {
        ID id = cfp->me->def? cfp->me->def->original_id : cfp->me->called_id;

        if (NIL_P(file)) file = ruby_engine_name;

        if (id != ID_ALLOCATOR){
          VALUE* argv = NULL;
          // when argc==-1/-2(variable length params without/with splat) - the cfp has no info on params count :(
          //TODO: infere from somewhere ex. find self in stack? (not guaranted btw, for example: obj.method(obj, 123, obj) - will find last param instead of self)
          if(cfp->me->def->body.cfunc.argc >= 0){ //only fixed args
            argv = &cfp->bp[- cfp->me->def->body.cfunc.argc - 2]; // args+self, bp was incremented thus minus 2
          }
          //file+line no from previous iseq frame
          if((*iter)(arg, file, line_no, rb_id2str(id), cfp->me->def->body.cfunc.argc, argv)) break;
        }
      }
    cfp = RUBY_VM_NEXT_CONTROL_FRAME(cfp);
  }
  return TRUE;
}

VALUE sym_file, sym_line, sym_method, sym_argc;

static int backtrace_iter(void *arg, VALUE file, int line, VALUE method_name, int argc, VALUE* argv)
{
    VALUE arr = (VALUE)arg;

    const char *filename = NIL_P(file) ? "ruby" : RSTRING_PTR(file);
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, sym_file, file); //may be nil
    rb_hash_aset(hash, sym_line, INT2FIX(line));
    rb_hash_aset(hash, sym_method, method_name); //may be nil
    rb_hash_aset(hash, sym_argc, INT2FIX(argc));

    int i;
    for(i = 0; i < argc; i++){
      rb_hash_aset(hash, INT2FIX(i), argv[i]);
    }
    rb_ary_unshift(arr, hash);
    return FALSE;
}

// trim(1 by default, but may pass 0 to include current method in trace)
// like in 'Kernel#caller'
static VALUE rb_caller_ext(int argc, VALUE argv[], VALUE self){
  int trim = 1;
  if(argc > 1)
    rb_raise(rb_eArgError, "wrong number of arguments(%d for 0-1)", argc);
  if(argc > 0){
    trim = FIX2INT(argv[0]);
  }

  rb_thread_t *th = GET_THREAD();
  //we know frames count, thus array size is known too, eliminate relocations
  int frames = (int)((rb_control_frame_t*)(th->stack + th->stack_size) - th->cfp - 3 - trim);

  VALUE res = rb_ary_new2(frames);
  vm_backtrace_each_ext(th, trim, NULL/*init func*/, backtrace_iter, (void*)res /*param*/);
  return res;
}

void Init_ruby_ext_backtrace(){
  sym_file = ID2SYM(rb_intern("file"));
  sym_line = ID2SYM(rb_intern("line"));
  sym_method = ID2SYM(rb_intern("method"));
  sym_argc = ID2SYM(rb_intern("argc"));
  rb_define_global_function("caller_ext", rb_caller_ext, -1);
}
