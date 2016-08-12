;; In this study, we are trying to determine if the value of the
;; stack copies the environment or just a reference to the environment
;;
;; Conclusion:
;; A "reference" to the environment is store in the stack
;;
;; This means that if
;; 1) save env
;; 2) the environment and later
;; 3) restore env
;;
;; We will get a different result from the saved one
;; This is ok, because only coumpound-apply
;; changes the environment register, it doesn't overwrite
;; the saved environment, at most is just extends the
;; environment, i.e. it wraps a frame around it.
;;
;; So when restore the environment we just restore to
;; which ever "frame" it was saved at, which could be
;; the global-environment
;;
;; 699 register
(define (make-register name)
  (let ((contents '*unassigned*))
    (define (dispatch message)
      (cond ((eq? message 'get) contents)
	    ((eq? message 'set)
	     (lambda (value) (set! contents value)))
	    (else
	     (error "Unknown request: REGISTER" message))))
    dispatch))
(define (get-contents register) (register 'get))
(define (set-contents! register value) ((register 'set) value))

;; 700 stack
(define (make-stack)
  (let ((s '()))
    (define (push x) (set! s (cons x s)))
    (define (pop)
      (if (null? s)
	  (error "Empty stack: POP")
	  (let ((top (car s)))
	    (set! s (cdr s))
	    top)))
    (define (initialize)
      (set! s '())
      'done)
    (define (dispatch message)
      (cond ((eq? message 'push) push)
	    ((eq? message 'pop) (pop))
	    ((eq? message 'initialize) (initialize))
	    (else (error "Unknown request: STACK" message))))
    dispatch))
(define (pop stack) (stack 'pop))
(define (push stack value) ((stack 'push) value))

;; 701
(define (make-new-machine)
  (let ((pc (make-register 'pc))
	(flag (make-register 'flag))
	(stack (make-stack))
	(the-instruction-sequence '()))
    (let ((the-ops
	   (list (list 'initialize-stack
		       (lambda () (stack 'initialize)))))
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
	  (pp (list "looking up register" name (cadr val)))
	  (if val
	      (cadr val)
	      (error "Unknown register:" name))))
      (define (execute)
	(let ((insts (get-contents pc)))
	  (if (null? insts)
	      'done
	      (begin
		((instruction-execution-proc (car insts)))
		(execute)))))
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
	      (else (error "Unknown request: MACHINE"
			   message))))
      dispatch)))

;; 713
(define (make-save inst machine stack pc)
  (let ((reg (get-register machine
			   (stack-inst-reg-name inst))))
    (lambda ()
      (push stack (get-contents reg))
      (advance-pc pc))))
(define (make-restore inst machine stack pc)
  (let ((reg (get-register machine
			   (stack-inst-reg-name inst))))
    (lambda ()
      (set-contents! reg (pop stack))
      (advance-pc pc))))
(define (stack-inst-reg-name stack-instruction)
  (cadr stack-instruction))

;; 698 Machine
(define (make-machine register-names ops controller-text)
  (let ((machine (make-new-machine)))
    (for-each
     (lambda (register-name)
       ((machine 'allocate-register) register-name))
     register-names)
    ((machine 'install-operations) ops)
    ;; ((machine 'install-instruction-sequence)
    ;;  (assemble controller-text machine))
    machine))

;; 709 From make-execution-procedure
;; (make-execution-procedure inst labels machine pc flag stack ops)
;;
;; 707 This is called withing update-insts!
;; Here stack:
;; stack (machine 'stack)
;; inst is something line '(save env)

;; 703
(define (get-register machine reg-name)
  (pp (list "in get-register" reg-name ((machine 'get-register) reg-name)))
  ((machine 'get-register) reg-name))
(define (start machine) (machine 'start))
(define (get-register-contents machine register-name)
  (get-contents (get-register machine register-name)))
(define (set-register-contents! machine register-name value)
  (pp (list "in set-register-contents!"
	    machine
	    register-name
	    value
	    (get-register machine register-name)))
  (set-contents! (get-register machine register-name)
		 value)
  'done)

(let*
    (
     (machine (make-machine '(env) '() '()))
     (inst '(save env))
     (stack (machine 'stack))
     (pc (get-register machine 'pc))
     )
  (pp "start here")
  (set-register-contents! machine 'env '(1 2 3))
  (make-save inst machine stack pc)
  (set-register-contents! machine 'env '(2 3))
  (make-restore '(restore env) machine stack pc)
  (pp (get-register-contents machine 'env))
  (pp 'done))

;;(define s (make-stack))

;; 751 assign env in compound-apply
