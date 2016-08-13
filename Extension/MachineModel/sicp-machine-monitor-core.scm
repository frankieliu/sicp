;; (use test sicp-machine)
(use sicp-compile)

;;----------------------------------------------------------------------
;; Common functions
(define (tagged-list? exp tag)
  (and (pair? exp)
       (eq? (car exp) tag)))

;;----------------------------------------------------------------------
;; Creating a instruction "object"
;; with proper access functions
;;
;; We decided to expand on the instruction to save
;; breakpoint information
;;
;; Instructions
(define (make-instruction text-orig)
  (let ((text text-orig)
	(label #f)
	(breakpoint #f)
	(proc #f))
    (define (dispatch m)
      (cond
       ((eq? m 'text) text)
       ((eq? m 'label) label)
       ((eq? m 'breakpoint) breakpoint)
       ((eq? m 'proc) proc)
       ((eq? m 'text!) (lambda (x) (set! text x)))
       ((eq? m 'label!) (lambda (x) (set! label x)))
       ((eq? m 'breakpoint!) (lambda (x) (set! breakpoint x)))
       ((eq? m 'proc!) (lambda (x) (set! proc x)))
       (else (error "Unknown instruction dispatch: MAKE-INSTRUCTION" m))
       ;;
       ))
    dispatch))
(define (instruction-text inst) (inst 'text))
(define (instruction-execution-proc inst) (inst 'proc))
(define (set-instruction-execution-proc! inst proc)
  ((inst 'proc!) proc))
(define (instruction-set-breakpoint! inst)
  ((inst 'breakpoint!) #t))
(define (instruction-cancel-breakpoint! inst)
  ((inst 'breakpoint!) #f))
(define (instruction-breakpoint? inst) (inst 'breakpoint))

;;-----------------------------------------------------------------------
;; Retain this part from Ex 5.18
;; 5.18 : trace registers during assign
;; This section is unchanged from 5.18
;;
;; Add a field in registers
;; Registers that takes kepts track of whether it is being
;; traced or not
(define (make-register name)
  (let ((contents '*unassigned*)
	(trace #f))
    (define (dispatch message)
      (cond ((eq? message 'get) contents)
	    ((eq? message 'set)
	     (lambda (value) (set! contents value)))
	    ((eq? message 'trace-toggle) (set! trace (not trace)))
	    ((eq? message 'trace-on) (set! trace #t))
	    ((eq? message 'trace-off) (set! trace #f))
	    ((eq? message 'trace) trace)
	    (else
	     (error "Unknown request: REGISTER" message))))
    dispatch))
(define (register-trace-toggle! machine name)
  ((get-register machine name) 'trace-toggle))
(define (register-trace-on! machine name)
  ((get-register machine name) 'trace-on))
(define (register-trace-off! machine name)
  ((get-register machine name) 'trace-off))
(define (register-trace register)
  (register 'trace))

;; During an assign
;; Check if the register's trace flag is on
(define (make-assign inst machine labels operations pc)
  (let* ((name (assign-reg-name inst))
	(target
	 (get-register machine name))
	(value-exp (assign-value-exp inst)))
    (let ((value-proc
	   (if (operation-exp? value-exp)
	       (make-operation-exp
		value-exp machine labels operations)
	       (make-primitive-exp
		(car value-exp) machine labels))))
      (lambda () ; execution procedure for assign
	(let ((val (value-proc)))
	  (set-contents! target val)
	  (if (register-trace target)           ;; Check is done here
	      (pp (list 'reg name ': val)))
	  (advance-pc pc))))))

;;----------------------------------------------------------------------
;;
;; 5.17 Adding labels during instruction trace
;;
;; Note extract-labels works bottom-up!
;;
;; So before label is there, the instruction after it
;; has already been made, that means we can add extra
;; information to the immediate instruction that follows
;; a label

;; Simple test case for 
;; (define (test-extract)
;;   (pp "test-extract")
;;   (extract-labels
;;    '(a
;;      (goto (label a))
;;      b
;;      (goto (label b)))
;;    (lambda (insts labels)
;;      (pp (list 'insts insts))
;;      (pp (list 'labels labels)))))
;; (test-extract)

;; When making a label, modify the instruction
;; right after it adding a tagged 'label information
(define (make-label-entry label-name insts)
  (if (not (null? insts))
      (let ((head (car insts)))
	((head 'label!) label-name)))
  (cons label-name insts))

;; Since we added a 'tag containing the label, we
;; need to remove it before making an execution procedure

;; Eval
(define (make-execution-procedure
         inst labels machine pc flag stack ops)
  ;; (pp (list inst (car inst)))
  (cond
   ((eq? (car inst) 'assign)
    (make-assign inst machine labels ops pc))
   ((eq? (car inst) 'test)
    (make-test inst machine labels ops flag pc))
   ((eq? (car inst) 'branch)
    (make-branch inst machine labels flag pc))
   ((eq? (car inst) 'goto)
    (make-goto inst machine labels pc))
   ((eq? (car inst) 'save)
    (make-save inst machine stack pc))
   ((eq? (car inst) 'restore)
    (make-restore inst machine stack pc))
   ((eq? (car inst) 'perform)
    (make-perform inst machine labels ops pc))
   (else
    (error "Unknown instruction type: ASSEMBLE"
           inst))))

;;----------------------------------------------------------------------
;; Rather than return a single value, or return a value at all
;; we decided to use the same trick as extract-labels, we pass
;; it a "continuation" function that makes use of the data generated
;; in assemble

;; Assemble
(define (assemble controller-text machine)
  (extract-labels
   controller-text
   (lambda (insts labels)
     (update-insts! insts labels machine)
     ((machine 'append-labels!) labels)
     insts)))

;;----------------------------------------------------------------------
;; Making labels (783)

(define machine-label-counter 0)
(define (new-machine-label-number)
  (set! machine-label-counter (+ 1 machine-label-counter))
  machine-label-counter)
(define (make-machine-label name)
  (string->symbol
   (string-append (symbol->string name)
		  (number->string (new-machine-label-number)))))

;; New machine
(define (make-new-machine)
  (let (
	;;
	(pc (make-register 'pc))
        (flag (make-register 'flag))
        (stack (make-stack))
        (the-instruction-sequence '())
        (instruction-count 0)
        (trace #f)
	(labels '())
	(breakpoints '())
	(breakpoint-count 0)
	(proceed-flag #f)
	(machine-id (make-machine-label 'mac))	
	;;
	)
    (let ((the-ops
           (list
	    (list 'initialize-stack
		  (lambda () (stack 'initialize)))
	    (list 'print-stack-statistics
		  (lambda () (stack 'print-statistics)))
		 ;;
                 ))
          (register-table
           (list (list 'pc pc) (list 'flag flag))))

      (define (allocate-register name)
        (if (assoc name register-table)
            (error "Multiply defined register: " name)
            (set! register-table
              (cons (list name (make-register name))
                    register-table)))
        'register-allocated)

      (define (lookup-register name)
        (let ((val (assoc name register-table)))
          (if val
              (cadr val)
              (error "Unknown register:" name))))

      (define (trace-toggle) (set! trace (not trace)))
      ;; We modify execute to print out the
      ;; instruction-count, any labels preceeding the
      ;; instruction

      (define (execute)
        (let ((insts (get-contents pc)))
          (if (null? insts)
              'done
              (let ((head (car insts)))
		(if (and (instruction-breakpoint? head) (not proceed-flag)) 
		    (pp "breakpoint")
		    (begin
		      (set! proceed-flag #f)
		      (set! instruction-count (+ instruction-count 1))
		      (if trace
			  (pp
			   (let ((text (instruction-text head)))
			     (if (head 'label)
				 (list
				  instruction-count
				  (head 'label)
				  text)
				 (list
				  instruction-count
				  text)))))
		      ((instruction-execution-proc head))
		      (execute)))))))
      (define (proceed)
	(set! proceed-flag #t)
	(execute))
      (define (lookup-label-offset label offset)
	(list-ref (lookup-label labels label) offset))
      (define (get-breakpoint-count) breakpoint-count)
      (define (break! label offset)
	;; we need to find the instruction and modify it to
	;; add a break, we should also increment the number
	;; or breaks
	(let ((inst (lookup-label-offset label offset)))
	  (instruction-set-breakpoint! inst)
	  (set! breakpoints (cons inst breakpoints)))
	(set! breakpoint-count (+ breakpoint-count 1)))
      (define (break-cancel! label offset)
	(let ((inst (lookup-label-offset label offset)))
	  ;; (check-list inst breakpoints)
	  (set! breakpoints (filter (lambda (x) (not (equal? x inst))) breakpoints))
	  (instruction-cancel-breakpoint! inst))
	(if (> breakpoint-count 0)
	    (set! breakpoint-count (- breakpoint-count 1))))
      (define (break-cancel-all!)
	(cond ((null? breakpoints)
	       (begin
		 (if (not (= breakpoint-count 0))
		     (error "break-cancel-all! count != 0" breakpoint-count))
		 'ok))
	      (else
	       (instruction-cancel-breakpoint! (car breakpoints))
	       (set! breakpoint-count (- breakpoint-count 1))
	       (set! breakpoints (cdr breakpoints))
	       (break-cancel-all!))))
      (define (dispatch message)
        (cond ((eq? message 'start)
               (set-contents! pc the-instruction-sequence)
               (execute))
              ((eq? message 'install-instruction-sequence)
               (lambda (seq)
                 (set! the-instruction-sequence seq)))
              ((eq? message 'allocate-register)
               allocate-register)
              ((eq? message 'get-register)
               lookup-register)
              ((eq? message 'install-operations)
               (lambda (ops)
                 (set! the-ops (append the-ops ops))))
              ((eq? message 'stack) stack)
              ((eq? message 'operations) the-ops)
              ((eq? message 'instruction-count) instruction-count)
              ((eq? message 'clear-instruction-count!) (set! instruction-count 0))
              ((eq? message 'trace-toggle) (trace-toggle))
	      ((eq? message 'break!) break!)
	      ((eq? message 'proceed) (proceed))
	      ((eq? message 'break-cancel!) break-cancel!)
	      ((eq? message 'break-cancel-all!) (break-cancel-all!) )
	      ((eq? message 'proceed) (proceed) )
	      ((eq? message 'breakpoint-count) (get-breakpoint-count) )
              ((eq? message 'labels!)
               (lambda (x)
                 (set! labels x)))
	      ((eq? message 'append-labels!)
	       (lambda (x)
		 (set! labels (append labels x))))
	      ((eq? message 'machine-id) machine-id)
              (else (error "Unknown request: MACHINE"
                           message))))
      dispatch)))

(define (get-instruction-count machine) (machine 'instruction-count))
(define (clear-instruction-count! machine) (machine 'clear-instruction-count!))
(define (trace-toggle machine) (machine 'trace-toggle))
(define (set-breakpoint machine label offset) ((machine 'break!) label offset))
(define (proceed-machine machine) (machine 'proceed))
(define (cancel-breakpoint machine label offset) ((machine 'break-cancel!) label offset))
(define (cancel-all-breakpoints machine) (machine 'break-cancel-all!))
(define (get-breakpoint-count machine) (machine 'breakpoint-count))
(define (print-stack-statistics machine)
  ((machine 'stack) 'print-statistics))

;;----------------------------------------------------------------------
;; We need to modify assemble to pull out label information
;; for breakpoints.
;;
;; Note: it is interesting that machine doesn't really touch
;; the label information, the label list is carried around for
;; creating an execution-procedure, but it is not "stored" by
;; the machine
;;
;; Make
;;
;; Note: I took out the following routine modification to
;; make-machine because I can just modify assemble directly,
;; to add to the label structure store in machine
;; The associated assemble (previous version) that
;; goes along with this code is
;; added below.
;;
;; (define (assemble controller-text machine receive-labels)
;;   (extract-labels
;;    controller-text
;;    (lambda (insts labels)
;;      (update-insts! insts labels machine)
;;      (receive-labels labels)
;;      insts)))
;;
;; (define (make-machine register-names ops controller-text)
;;   (let ((machine (make-new-machine)))
;;     (for-each
;;      (lambda (register-name)
;;        ((machine 'allocate-register) register-name))
;;      register-names)
;;     ((machine 'install-operations) ops)
;;     ((machine 'install-instruction-sequence)
;;      (assemble controller-text machine
;; 	       (lambda (labels)	((machine 'labels!) labels))))
;;     machine))
