;;;; sicp-compile.import.scm - GENERATED BY CHICKEN 4.10.0 -*- Scheme -*-

(eval '(import scheme chicken extras sicp-eval))
(##sys#register-compiled-module
  'sicp-compile
  (list)
  '((compile-test . sicp-compile#compile-test)
    (compile . sicp-compile#compile)
    (parallel-instruction-sequences
      .
      sicp-compile#parallel-instruction-sequences)
    (tack-on-instruction-sequence . sicp-compile#tack-on-instruction-sequence)
    (preserving . sicp-compile#preserving)
    (list-difference . sicp-compile#list-difference)
    (list-union . sicp-compile#list-union)
    (append-instruction-sequences . sicp-compile#append-instruction-sequences)
    (modifies-register? . sicp-compile#modifies-register?)
    (needs-register? . sicp-compile#needs-register?)
    (statements . sicp-compile#statements)
    (registers-modified . sicp-compile#registers-modified)
    (registers-needed . sicp-compile#registers-needed)
    (compile-proc-appl . sicp-compile#compile-proc-appl)
    (all-regs . sicp-compile#all-regs)
    (compile-procedure-call . sicp-compile#compile-procedure-call)
    (code-to-get-rest-args . sicp-compile#code-to-get-rest-args)
    (construct-arglist . sicp-compile#construct-arglist)
    (compile-application . sicp-compile#compile-application)
    (compile-lambda-body . sicp-compile#compile-lambda-body)
    (compile-lambda . sicp-compile#compile-lambda)
    (compiled-procedure-env . sicp-compile#compiled-procedure-env)
    (compiled-procedure-entry . sicp-compile#compiled-procedure-entry)
    (compiled-procedure? . sicp-compile#compiled-procedure?)
    (make-compiled-procedure . sicp-compile#make-compiled-procedure)
    (compile-sequence . sicp-compile#compile-sequence)
    (compile-if . sicp-compile#compile-if)
    (compile-definition . sicp-compile#compile-definition)
    (compile-assignment . sicp-compile#compile-assignment)
    (compile-variable . sicp-compile#compile-variable)
    (compile-quoted . sicp-compile#compile-quoted)
    (compile-self-evaluating . sicp-compile#compile-self-evaluating)
    (end-with-linkage . sicp-compile#end-with-linkage)
    (compile-linkage . sicp-compile#compile-linkage)
    (empty-instruction-sequence . sicp-compile#empty-instruction-sequence)
    (make-instruction-sequence . sicp-compile#make-instruction-sequence)
    (make-label . sicp-compile#make-label)
    (new-label-number . sicp-compile#new-label-number)
    (label-counter . sicp-compile#label-counter))
  (list)
  (list))

;; END OF FILE
