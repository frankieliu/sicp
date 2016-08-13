(module sicp-machine-monitor
  *
  (import scheme chicken srfi-1)
  (use extras)
  (use sicp-eval)
  (use sicp-machine)
  ;; (define (square x) (* x x))
  (include "sicp-machine-monitor-core.scm")
  )

;; Tracing registers
;;  (register-trace-toggle! machine reg-name)
;;  (register-trace-on! machine reg-name)
;;  (register-trace-off! machine reg-name)
;; Instruction Count
;;  (get-instruction-count machine)
;;  (clear-instruction-count! machine)
;; Trace
;;  (trace-toggle machine)
;; Breakpoints
;;  (set-breakpoint machine label offset)
;;  (cancel-breakpoint machine label offset)
;;  (cancel-all-breakpoints machine)
;;  (get-breakpoint-count machine)
;;  (proceed-machine machine)





