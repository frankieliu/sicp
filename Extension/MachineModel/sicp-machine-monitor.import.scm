;;;; sicp-machine-monitor.import.scm - GENERATED BY CHICKEN 4.10.0 -*- Scheme -*-

(eval '(import
         scheme
         chicken
         srfi-1
         extras
         sicp-eval
         sicp-machine
         sicp-compile))
(##sys#register-compiled-module
  'sicp-machine-monitor
  (list)
  '((print-stack-statistics . sicp-machine-monitor#print-stack-statistics)
    (get-breakpoint-count . sicp-machine-monitor#get-breakpoint-count)
    (cancel-all-breakpoints . sicp-machine-monitor#cancel-all-breakpoints)
    (cancel-breakpoint . sicp-machine-monitor#cancel-breakpoint)
    (proceed-machine . sicp-machine-monitor#proceed-machine)
    (set-breakpoint . sicp-machine-monitor#set-breakpoint)
    (trace-toggle . sicp-machine-monitor#trace-toggle)
    (clear-instruction-count! . sicp-machine-monitor#clear-instruction-count!)
    (get-instruction-count . sicp-machine-monitor#get-instruction-count)
    (make-new-machine . sicp-machine#make-new-machine)
    (make-machine-label . sicp-machine-monitor#make-machine-label)
    (new-machine-label-number . sicp-machine-monitor#new-machine-label-number)
    (machine-label-counter . sicp-machine-monitor#machine-label-counter)
    (assemble . sicp-machine#assemble)
    (make-execution-procedure . sicp-machine#make-execution-procedure)
    (make-label-entry . sicp-machine#make-label-entry)
    (make-assign . sicp-machine#make-assign)
    (register-trace . sicp-machine-monitor#register-trace)
    (register-trace-off! . sicp-machine-monitor#register-trace-off!)
    (register-trace-on! . sicp-machine-monitor#register-trace-on!)
    (register-trace-toggle! . sicp-machine-monitor#register-trace-toggle!)
    (make-register . sicp-machine#make-register)
    (instruction-breakpoint? . sicp-machine-monitor#instruction-breakpoint?)
    (instruction-cancel-breakpoint!
      .
      sicp-machine-monitor#instruction-cancel-breakpoint!)
    (instruction-set-breakpoint!
      .
      sicp-machine-monitor#instruction-set-breakpoint!)
    (set-instruction-execution-proc!
      .
      sicp-machine#set-instruction-execution-proc!)
    (instruction-execution-proc . sicp-machine#instruction-execution-proc)
    (instruction-text . sicp-machine#instruction-text)
    (make-instruction . sicp-machine#make-instruction)
    (tagged-list? . sicp-eval#tagged-list?))
  (list)
  (list))

;; END OF FILE
