;;;; sicp-machine.import.scm - GENERATED BY CHICKEN 4.10.0 -*- Scheme -*-

(eval '(import scheme chicken extras sicp-eval))
(##sys#register-compiled-module
  'sicp-machine
  (list)
  '((make-execution-procedure . sicp-machine#make-execution-procedure)
    (make-operation-exp . sicp-machine#make-operation-exp)
    (operation-exp-operands . sicp-machine#operation-exp-operands)
    (operation-exp-op . sicp-machine#operation-exp-op)
    (operation-exp? . sicp-machine#operation-exp?)
    (make-primitive-exp . sicp-machine#make-primitive-exp)
    (make-primitive-exp . sicp-machine#make-primitive-exp)
    (lookup-prim . sicp-machine#lookup-prim)
    (label-exp-label . sicp-machine#label-exp-label)
    (label-exp? . sicp-machine#label-exp?)
    (constant-exp-value . sicp-machine#constant-exp-value)
    (constant-exp? . sicp-machine#constant-exp?)
    (register-exp-reg . sicp-machine#register-exp-reg)
    (register-exp? . sicp-machine#register-exp?)
    (make-perform . sicp-machine#make-perform)
    (perform-action . sicp-machine#perform-action)
    (make-restore . sicp-machine#make-restore)
    (make-save . sicp-machine#make-save)
    (stack-inst-reg-name . sicp-machine#stack-inst-reg-name)
    (make-goto . sicp-machine#make-goto)
    (goto-dest . sicp-machine#goto-dest)
    (make-branch . sicp-machine#make-branch)
    (branch-dest . sicp-machine#branch-dest)
    (make-test . sicp-machine#make-test)
    (test-condition . sicp-machine#test-condition)
    (make-assign . sicp-machine#make-assign)
    (assign-value-exp . sicp-machine#assign-value-exp)
    (assign-reg-name . sicp-machine#assign-reg-name)
    (advance-pc . sicp-machine#advance-pc)
    (assemble . sicp-machine#assemble)
    (extract-labels . sicp-machine#extract-labels)
    (lookup-label . sicp-machine#lookup-label)
    (make-label-entry . sicp-machine#make-label-entry)
    (update-insts! . sicp-machine#update-insts!)
    (set-instruction-execution-proc!
      .
      sicp-machine#set-instruction-execution-proc!)
    (instruction-execution-proc . sicp-machine#instruction-execution-proc)
    (instruction-text . sicp-machine#instruction-text)
    (make-instruction . sicp-machine#make-instruction)
    (get-register . sicp-machine#get-register)
    (set-register-contents! . sicp-machine#set-register-contents!)
    (get-register-contents . sicp-machine#get-register-contents)
    (start . sicp-machine#start)
    (make-machine . sicp-machine#make-machine)
    (make-new-machine . sicp-machine#make-new-machine)
    (get-stack . sicp-machine#get-stack)
    (print-stack . sicp-machine#print-stack)
    (push . sicp-machine#push)
    (pop . sicp-machine#pop)
    (make-stack . sicp-machine#make-stack)
    (set-contents! . sicp-machine#set-contents!)
    (get-contents . sicp-machine#get-contents)
    (make-register . sicp-machine#make-register))
  (list)
  (list))

;; END OF FILE