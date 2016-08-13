(use sicp-eval sicp-machine sicp-machine-monitor sicp-compile)

;;----------------------------------------------------------------------
;; op primitives for register machine
;; These were specially defined for Ch5
(define (empty-arglist) '())
(define (adjoin-arg arg arglist) (append arglist (list arg)))
(define (last-operand? ops) (null? (cdr ops)))
(define (no-more-exps? seq) (null? seq))
(define (get-global-environment) (the-global-environment))

;;----------------------------------------------------------------------
;; Comment these out, since these are included from (sicp-compile)

;; (786) footnote 38 : entry point in dispatch
;; (define (compiled-procedure? proc)
;;   (tagged-list? proc 'compiled-procedure))
;; (define (compiled-procedure-entry c-proc) (cadr c-proc))

;;----------------------------------------------------------------------
;; (825)
(define (user-print object)
  (cond ((compound-procedure? object)
	 (display (list 'compound-procedure
			(procedure-parameters object)
			(procedure-body object)
			'<procedure-env>)))
	((compiled-procedure? object)
	 (display '<compiled-procedure>))
	(else (display object))))

;;----------------------------------------------------------------------
;; For my debugging, prints either global env or (reg env)
(define (user-print-env env)
  (cond ((list? env)
	 (pp (list "Number of frames:" (length env)))
	 (for-each
	  (lambda (frame)
	    (pp (car frame)))
	  env)
	 )
	(else
	 (pp "Env: *unassigned*")
	 )))
;;----------------------------------------------------------------------
;; compile-and-run support (5.48)
;; I put it in here, because it makes it easier to to
;; problems 5.45 and 5.46

(define (compile-and-run? exp)
  (tagged-list? exp 'compile-and-run))

;; General expression (compile-and-run '(etc))
;; Note that the first cadr returns '(etc))
;; And the second cadr returns etc (i.e. gets rid of quote)
(define (compile-and-run-exp exp)
  (cadadr exp))

;; If compile is extended to have a compile-time environment
;; ctenv you must also change the arguments to this compile
;; (compile expression 'val 'return)
;; Note: This can not be defined here since it requires
;; the machine to be involved, we will have to added it
;; as print-stack-statistics, i.e. defined within the machine
;; itself, but this requires replicating too many of the
;; instructions, we will cheat here and just pass a global
;; machine

(define eceval-machine (make-parameter '()))

(define (prim-compile-and-run expression)
  (let* ((compiled-expr (statements (compile expression 'val 'return)))
	 (out (assemble compiled-expr (eceval-machine))))
    out))

;;----------------------------------------------------------------------
;; op primitives from sicp-eval used in the register machine
;; Use makecomma.pl
(define eceval-operations
  (make-parameter
   `((self-evaluating? ,self-evaluating?)
     (empty-arglist ,empty-arglist)
     (adjoin-arg ,adjoin-arg)
     (last-operand? ,last-operand?)
     (no-more-exps? ,no-more-exps?)
     (prompt-for-input ,prompt-for-input)
     (read ,read)
     (get-global-environment ,get-global-environment)
     (announce-output ,announce-output)
     (user-print ,user-print)
     ;; My own debugging code
     (user-print-env ,user-print-env)
     (self-evaluating? ,self-evaluating?)
     (variable? ,variable?)
     (quoted? ,quoted?)
     (assignment? ,assignment?)
     (definition? ,definition?)
     (if? ,if?)
     (lambda? ,lambda?)
     (begin? ,begin?)
     (application? ,application?)
     (lookup-variable-value ,lookup-variable-value)
     (text-of-quotation ,text-of-quotation)
     (lambda-parameters ,lambda-parameters)
     (lambda-body ,lambda-body)
     (make-procedure ,make-procedure)
     (operands ,operands)
     (operator ,operator)
     (no-operands? ,no-operands?)
     (first-operand ,first-operand)
     (rest-operands ,rest-operands)
     (primitive-procedure? ,primitive-procedure?)
     (compound-procedure? ,compound-procedure?)
     (apply-primitive-procedure ,apply-primitive-procedure)
     (procedure-parameters ,procedure-parameters)
     (procedure-environment ,procedure-environment)
     (extend-environment ,extend-environment)
     (procedure-body ,procedure-body)
     (begin-actions ,begin-actions)
     (last-exp? ,last-exp?)
     (first-exp ,first-exp)
     (rest-exps ,rest-exps)
     (if-predicate ,if-predicate)
     (true? ,true?)
     (if-alternative ,if-alternative)
     (if-consequent ,if-consequent)
     (assignment-variable ,assignment-variable)
     (assignment-value ,assignment-value)
     (set-variable-value! ,set-variable-value!)
     (definition-variable ,definition-variable)
     (definition-value ,definition-value)
     (define-variable! ,define-variable!)
     ;; Added here for compiled code support (824)
     (compiled-procedure? ,compiled-procedure?)     
     (compiled-procedure-entry ,compiled-procedure-entry)
     (false? ,false?)
     (make-compiled-procedure ,make-compiled-procedure)
     (compiled-procedure-env ,compiled-procedure-env)
     (list ,list)
     (cons ,cons)
     ;; Note this is a primitive op, not to be confused with a primitive procedure declared in the env
     ;; Added here for compile-and-run (5.48)
     (compile-and-run? ,compile-and-run?)
     (compile-and-run-exp ,compile-and-run-exp)
     (prim-compile-and-run ,prim-compile-and-run)
     )))

;;----------------------------------------------------------------------
;; Break up the register machine into sections
;; Added compile-and-run from community.schemewiki.org
;; Really neat solution to Ex 5.48
(define ec-eval-dispatch
  '(eval-dispatch
    (test (op self-evaluating?) (reg exp)) (branch (label ev-self-eval))
    (test (op variable?) (reg exp)) (branch (label ev-variable))
    (test (op quoted?) (reg exp)) (branch (label ev-quoted))
    (test (op assignment?) (reg exp)) (branch (label ev-assignment))
    (test (op definition?) (reg exp)) (branch (label ev-definition))
    (test (op if?) (reg exp)) (branch (label ev-if))
    (test (op lambda?) (reg exp)) (branch (label ev-lambda))
    (test (op begin?) (reg exp)) (branch (label ev-begin))
    ;; (compile-and-run) (5.48)
    (test (op compile-and-run?) (reg exp)) (branch (label ev-compile-and-run))
    (test (op application?) (reg exp)) (branch (label ev-application))
    ;; interestingly application only checks if exp is a pair
    (goto (label unknown-expression-type))
    ;;
    ))
(define ec-ev-self-eval
  '(ev-self-eval
    (assign val (reg exp))
    (goto (reg continue))
    ;;
    ))
(define ec-ev-variable
  '(ev-variable
    (assign val (op lookup-variable-value) (reg exp) (reg env))
    (goto (reg continue))
    ;;
    ))
(define ec-ev-quoted
  '(ev-quoted
    (assign val (op text-of-quotation) (reg exp))
    (goto (reg continue))
    ;;
    ))
(define ec-ev-lambda
  '(ev-lambda
    (assign unev (op lambda-parameters) (reg exp))
    (assign exp (op lambda-body) (reg exp))
    (assign val (op make-procedure) (reg unev) (reg exp) (reg env))
    (goto (reg continue))
    ;;
    ))

;;----------------------------------------------------------------------
;; Added to support compile-and-run (5.48)
;; Note external-entry is just a place
;; to initialize the environment and
;; to initialize the statistics prior to
;; (goto (reg val))
(define ec-ev-compile-and-run
  '(ev-compile-and-run
    (assign val (op compile-and-run-exp) (reg exp))
    (assign val (op prim-compile-and-run) (reg val))
    (goto (label external-entry))
    ;;
    ))
(define ec-ev-application
  '(ev-application
    (save continue)
    (save env)
    (assign unev (op operands) (reg exp))
    (save unev)
    (assign exp (op operator) (reg exp))
    (assign continue (label ev-appl-did-operator))
    (goto (label eval-dispatch))
    ;;
    ))
(define ec-ev-appl-did-operator
  '(ev-appl-did-operator
    (restore unev)			; the operands
    (restore env)
    (assign argl (op empty-arglist))
    (assign proc (reg val))		; the operator
    (test (op no-operands?) (reg unev))
    (branch (label apply-dispatch))
    (save proc)
    ev-appl-operand-loop
    (save argl)
    (assign exp (op first-operand) (reg unev))
    (test (op last-operand?) (reg unev))
    (branch (label ev-appl-last-arg))
    (save env)
    (save unev)
    (assign continue (label ev-appl-accumulate-arg))
    (goto (label eval-dispatch))
    ;;
    ))
(define ec-ev-appl-accumulate-arg
  '(ev-appl-accumulate-arg
    (restore unev)
    (restore env)
    (restore argl)
    (assign argl (op adjoin-arg) (reg val) (reg argl))
    (assign unev (op rest-operands) (reg unev))
    (goto (label ev-appl-operand-loop))
    ;;
    ))
(define ec-ev-appl-last-arg
  '(ev-appl-last-arg
    (assign continue (label ev-appl-accum-last-arg))
    (goto (label eval-dispatch))
    ;;
    ))
(define ec-ev-appl-accum-last-arg
  '(ev-appl-accum-last-arg
    (restore argl)
    (assign argl (op adjoin-arg) (reg val) (reg argl))
    (restore proc)
    (goto (label apply-dispatch))
    ;;
    ))

(define ec-apply-dispatch
  '(apply-dispatch
    (test (op primitive-procedure?) (reg proc))
    (branch (label primitive-apply))
    (test (op compound-procedure?) (reg proc))
    (branch (label compound-apply))
    ;; (824)
    (test (op compiled-procedure?) (reg proc))
    (branch (label compiled-apply))
    (goto (label unknown-procedure-type))
    compiled-apply
    (restore continue)
    (assign val (op compiled-procedure-entry) (reg proc))
    (goto (reg val))
    ;;
    ))
;; Note that in the above the continuation was on top of the stack
;; We restore into continue since compiled code expects the continuation
;; to be in the continue register. (824, 1st paragraph)

(define ec-primitive-apply
  '(primitive-apply
    (assign val (op apply-primitive-procedure)
	    (reg proc)
	    (reg argl))
    (restore continue)
    (goto (reg continue))
    ;;
    ))
(define ec-compound-apply
  '(compound-apply
    (assign unev (op procedure-parameters) (reg proc))
    (assign env (op procedure-environment) (reg proc))
    (assign env (op extend-environment)
	    (reg unev) (reg argl) (reg env))
    (assign unev (op procedure-body) (reg proc))
    (goto (label ev-sequence))
    ;;
    ))
(define ec-ev-begin    
  '(ev-begin
    (assign unev (op begin-actions) (reg exp))
    (save continue)
    (goto (label ev-sequence))
    ;;
    ))
;; tail recursive
(define ec-ev-sequence
  '(ev-sequence
    (assign exp (op first-exp) (reg unev))
    (test (op last-exp?) (reg unev))
    (branch (label ev-sequence-last-exp))
    (save unev)
    (save env)
    (assign continue (label ev-sequence-continue))
    (goto (label eval-dispatch))

    ev-sequence-continue
    (restore env)
    (restore unev)
    (assign unev (op rest-exps) (reg unev))
    (goto (label ev-sequence))

    ev-sequence-last-exp
    (restore continue)
    (goto (label eval-dispatch))
    ;;
    ))
;; non-tail recursive
(define ec-ev-sequence-non-tail
  '(ev-sequence
    (test (op no-more-exps?) (reg unev))
    (branch (label ev-sequence-end))
    (assign exp (op first-exp) (reg unev))
    (save unev)
    (save env)
    (assign continue (label ev-sequence-continue))
    (goto (label eval-dispatch))
    ev-sequence-continue
    (restore env)
    (restore unev)
    (assign unev (op rest-exps) (reg unev))
    (goto (label ev-sequence))
    ev-sequence-end
    (restore continue)
    (goto (reg continue))
    ;;
    ))
(define ec-ev-if
  '(ev-if
    (save exp)		              ; save expression for later
    (save env)
    (save continue)
    (assign continue (label ev-if-decide))
    (assign exp (op if-predicate) (reg exp))
    (goto (label eval-dispatch))     ; evaluate the predicate
    ;;
    ))
(define ec-ev-if-decide
  '(ev-if-decide
    (restore continue)
    (restore env)
    (restore exp)
    (test (op true?) (reg val))
    (branch (label ev-if-consequent))
    
    ev-if-alternative
    (assign exp (op if-alternative) (reg exp))
    (goto (label eval-dispatch))
    
    ev-if-consequent
    (assign exp (op if-consequent) (reg exp))
    (goto (label eval-dispatch))
    ;;
    ))
(define ec-ev-assignment
  '(ev-assignment
    (assign unev (op assignment-variable) (reg exp))
    (save unev)		         ; save variable for later
    (assign exp (op assignment-value) (reg exp))
    (save env)
    (save continue)
    (assign continue (label ev-assignment-1))
    (goto (label eval-dispatch)) ; evaluate the assignment value
    ;;
    ))
(define ec-ev-assignment-1
  '(ev-assignment-1
    (restore continue)
    (restore env)
    (restore unev)
    (perform
     (op set-variable-value!) (reg unev) (reg val) (reg env))
    (assign val (const ok))
    (goto (reg continue))
    ;;
    ))
(define ec-ev-definition
  '(ev-definition
    (assign unev (op definition-variable) (reg exp))
    (save unev)			; save variable for later
    (assign exp (op definition-value) (reg exp))
    (save env)
    (save continue)
    (assign continue (label ev-definition-1))
    (goto (label eval-dispatch))     ; evaluate the definition value
    ;;
    ))
(define ec-ev-definition-1
  '(ev-definition-1
    (restore continue)
    (restore env)
    (restore unev)
    (perform
     (op define-variable!) (reg unev) (reg val) (reg env))
    (assign val (const ok))
    (goto (reg continue))
    ;;
    ))
(define ec-unknown-expression-type
  '(unknown-expression-type
    (assign val (const unknown-expression-type-error))
    (goto (label signal-error))
    ;;
    ))
(define ec-unknown-procedure-type
  '(unknown-procedure-type
    (restore continue)       ; clean up stack (from apply-dispatch )
    (assign val (const unknown-procedure-type-error))
    (goto (label signal-error))
    ;;
    ))

;;----------------------------------------------------------------------
;; This assembly allows one to change one of the modules
;; and remake the machine
;; This set is used both in the repl and the non-repl versionn
;; Example : suppose you wanted to change ev-variable
;; (eceval-common (append .. ec-ev-variable)

(define eceval-common
  (make-parameter
   (append
    ec-eval-dispatch
    ec-ev-self-eval
    ec-ev-variable
    ec-ev-quoted
    ec-ev-lambda
    ec-ev-compile-and-run ;; (5.48)
    ec-ev-application
    ec-ev-appl-did-operator
    ec-ev-appl-accumulate-arg
    ec-ev-appl-last-arg
    ec-ev-appl-accum-last-arg
    ec-apply-dispatch
    ec-primitive-apply
    ec-compound-apply
    ec-ev-begin
    ec-ev-sequence
    ec-ev-if
    ec-ev-if-decide
    ec-ev-assignment
    ec-ev-assignment-1
    ec-ev-definition
    ec-ev-definition-1
    ec-unknown-expression-type
    ec-unknown-procedure-type))
  ;;
  )

;;----------------------------------------------------------------------
(define eceval-common-non-tail
  (make-parameter
   (append
    ec-eval-dispatch
    ec-ev-self-eval
    ec-ev-variable
    ec-ev-quoted
    ec-ev-lambda
    ec-ev-compile-and-run ;; (5.48)
    ec-ev-application
    ec-ev-appl-did-operator
    ec-ev-appl-accumulate-arg
    ec-ev-appl-last-arg
    ec-ev-appl-accum-last-arg
    ec-apply-dispatch
    ec-primitive-apply
    ec-compound-apply
    ec-ev-begin
    ec-ev-sequence-non-tail
    ec-ev-if
    ec-ev-if-decide
    ec-ev-assignment
    ec-ev-assignment-1
    ec-ev-definition
    ec-ev-definition-1
    ec-unknown-expression-type
    ec-unknown-procedure-type))
  ;;
  )

;;----------------------------------------------------------------------
;; REPL version
(define (make-eceval-repl)
  (make-machine
   '(exp env val continue proc argl unev compapp) ;; compapp Ex 5.47
   (eceval-operations)
   (append
    '(
      (assign compapp (label compound-apply)) ;; Ex 5.47
      
      (branch (label external-entry-init)) ;; branches if flag is set (824)

      read-eval-print-loop
      (perform (op initialize-stack))
      ;; (perform (op user-print) (const "Env:\n"))
      ;; (perform (op user-print-env) (reg env))
      (perform (op prompt-for-input) (const ";;EC-Eval input:"))
      (assign exp (op read))
      (assign env (op get-global-environment))
      (assign continue (label print-result))
      (goto (label eval-dispatch))

      print-result
      (perform (op print-stack-statistics))
      (perform (op announce-output) (const ";;EC-Eval value:"))
      (perform (op user-print) (reg val))      
      (goto (label read-eval-print-loop))

      signal-error
      (perform (op user-print) (reg val))
      (goto (label read-eval-print-loop))

      ;; (825)
      ;; also called from compile-and-run (5.48)
      ;; compiled routine will return and print
      ;; it's result, and then return to the
      ;; read-eval-print-loop
      external-entry-init
      (perform (op initialize-stack))
      (assign env (op get-global-environment))
      (assign continue (label print-result))
      (goto (reg val))

      external-entry
      (assign continue (label print-result))
      (goto (reg val))
      ;;
      )

    (eceval-common)

    ) ;; append
   ;;
   ))

(define eceval-repl
  (make-parameter (make-eceval-repl)))

;;----------------------------------------------------------------------
;; (824)
;; Book mod:
;; . set the initial flag to branch for external-entry to #f
;; Non-book mod:
;; . add primitive-procedures to the environment

(define (start-eceval procedures)
  (parameterize ((primitive-procedures
		  (append procedures (primitive-procedures)))
		 (eceval-repl (make-eceval-repl)))
    (parameterize ((eceval-machine (eceval-repl)))
      (the-global-environment (setup-environment))
      (set-register-contents! (eceval-repl) 'flag #f)
      (start (eceval-repl))))
  )

;;----------------------------------------------------------------------
;; (826)
;; Note: we may also want to create a version that
;; adds to the machine's set of eceval-operations,
;; this means we need to modify eceval-repl into
;; function that returns a new machine...
(define (compile-and-go procedures expression)
  (parameterize ((primitive-procedures
		  (append procedures (primitive-procedures))
		  (eceval-machine (eceval-repl))))
  (let ((instructions
	 (assemble
	  (statements
	   (compile expression 'val 'return))
	  (eceval-repl))))
    (the-global-environment (setup-environment))
    (set-register-contents! (eceval-repl) 'val instructions)
    (set-register-contents! (eceval-repl) 'flag #t)
    ;; This relies of the branch at the beginning of the repl machine
    (start (eceval-repl)))))

;;----------------------------------------------------------------------
;; Non-repl version
;; - inits the stack

(define (make-eceval-non-repl common-section)
  (append

   '(read-eval
     (perform (op initialize-stack))
     (assign continue (label done))    ;; return to done
     (goto (label eval-dispatch))      ;; (exp)(env)->(val)

     external-entry

     ;; (perform (op initialize-stack))
     ;;  - removed: should initialize-stack only if we need to

     ;; (assign env (op get-global-environment))
     ;;
     ;; Note: the above line has to be removed, because it creates a
     ;;       whole new env which is not the same as in eceval*
     ;; Debug notes:
     ;;   1) I had to insert a lot of
     ;;
     ;;      (perform (op user-print) (const "before executing compiled code:\n"))
     ;;      (perform (op user-print-env) (reg env))
     ;;
     ;;       into machine code fragments,
     ;; 
     ;;   2) and insert
     ;;
     ;;      (pp "external env:") (user-print-env env)
     ;;      (pp "(reg env):") (user-print-env (get-register-contents machine 'env))
     ;;
     ;;      into regular code
     ;;
     ;;   3) I also added a machine-id: just to make sure I was always dealing
     ;;      with the same machine:
     ;;
     ;;      (pp (list "Machine-id:" ((eceval-machine) 'machine-id)))

     (assign continue (label done))
     (goto (reg val))
     ;;
     )
   
   common-section

   '(signal-error
     (perform (op user-print) (reg val))
     (perform (op user-print) (reg exp))
     (goto (label done)))

   '(
     done
     ;; Add code here for debugging
     )
   
   );; append
  )

;;----------------------------------------------------------------------
;; Keep tail optimized and non-tailed optimized solutions

(define eceval-non-repl-tail
  (make-eceval-non-repl (eceval-common)))

(define eceval-non-repl-non-tail
  (make-eceval-non-repl (eceval-common-non-tail)))

;; By default it is with tail optimization
(define eceval-non-repl
  (make-parameter eceval-non-repl-tail))

;;----------------------------------------------------------------------
;; evaluate a single expression
;; exp and env must be populated before starting this machine
(define (make-eceval-single-exp)
  (make-machine
   ;; Registers:
   ;; argl : an argument list of evaluated expressions
   ;;        holds the results from evaluation the unevaluted expressions
   ;; unev : an list of unevaluated expressions
   '(exp env val continue proc argl unev)
   (eceval-operations)
   (eceval-non-repl)
   ;;
   ))

(define eceval-single-exp
  (make-parameter (make-eceval-single-exp)))

;;----------------------------------------------------------------------
;; wrapper that starts the machine
;; populates (exp) and (env) registers
;; return results from (val) register

(define (eceval* exp env)
  (parameterize ((eceval-machine (eceval-single-exp)))
    (let ((machine (eceval-machine)))
      (set-register-contents! machine 'exp exp)
      (set-register-contents! machine 'env env)
      (start machine)
      (get-register-contents machine 'val)
      ))
  )

;;----------------------------------------------------------------------
;; define extra primitive functions declared in the env
(define (with-primitive-procedures procedures receive-env)
  (parameterize ((primitive-procedures
		  (append procedures (primitive-procedures))))
    ;; The (setup-environment) below picks up the additions above
    ;; to the primitive-procedures
    (let ((env (setup-environment)))
      (receive-env env))))

;;----------------------------------------------------------------------
;; Non-tailed entry

(define (with-primitive-procedures-non-tail procedures receive-env)
  (parameterize ((primitive-procedures
		  (append procedures (primitive-procedures)))
		 (eceval-non-repl eceval-non-repl-non-tail))     ;; choose the non-tail code
    (parameterize ((eceval-single-exp (make-eceval-single-exp))) ;; make into a new machine
      (let ((env (setup-environment)))
	(receive-env env)))))

;;----------------------------------------------------------------------
;; define extra primitive operations recognized by the machine
;; we need to remake the machine in order to change the ops
(define (with-primitive-operations ops receive-env)
  (parameterize ((eceval-operations
		  (append ops (eceval-operations))))
    (parameterize ((eceval-single-exp (make-eceval-single-exp)))
      (let ((env (setup-environment)))
	(receive-env env)
	))))

;;----------------------------------------------------------------------
;; Sample usage:
;;
;; Use the following header for your examples
;; (use test
;;      sicp
;;      sicp-machine sicp-machine-monitor
;;      sicp-eval 
;;      sicp-compile
;;      sicp-explicit-control)
;; See Exercises/Ch5/eceval-append.scm

(define (eceval-test)
  (with-primitive-procedures
   `((+ ,+)
     (* ,*))
   (lambda (env)
     (eceval*
      '(define (square x) (* x x))
      env)
     (eceval*
      '(square 3)
      env)
     )))
;;(eceval-test)

;; This doesn't work:
;; 1. don't confuse 'primitive-procedures' in scheme
;; 2. with 'primitive-operations' used by the machine
;; 3. explicit-control sees (* x x) and interprets it as
;;    an application, so it looks for '*' in the
;;    environment, it doesn't look through the
;;    set of primitive-operations in the machine,
;;    as a matter-of-fact, we may want to also
;;    define '*' both as a primitive-procedure,
;;    which may be picked up by the compiler
;;    resulting in creating code which will call
;;    on the primitive-operator.

(define (eceval-test-2)
  (with-primitive-operations
   `((+ ,+)
     (* ,*))
   (lambda (env)
     (eceval*
      '(define (square x) (* x x))
      env)
     (eceval*
      '(square 3)
      env)
     )))
;;(eceval-test-2)

(define (compile-and-go-test)
  (compile-and-go
   `((= ,=)
     (+ ,+)
     (- ,-)
     (* ,*)
     (/ ,/))
   '(define (factorial n)
      (if (= n 1)
	  1
	  (* (factorial (- n 1)) n)))))

;;----------------------------------------------------------------------
;; (831) instead of compile-and-go Ex 5.48
;; Suggests adding a compile-and-run primitive
;; so you can just (start-eceval) and run compiled
;; code as follow:
;;
;; $ csi
;;
;;   #;1> (load "use-general.scm") ;; (use ..)
;;   #;2> (start-eceval `((= ,=) (- ,-) (* ,*)))
;;   ;;; EC-Eval input:
;;   (compile-and-run
;;    '(define (factorial n)
;;       (if (= n 1) 1 (* (factorial (- n 1)) n))))
;;   ;;; EC-Eval value:
;;   ok
;;   ;;; EC-Eval input:
;;   (factorial 5)
;;   ;;; EC-Eval value:
;;   120
;;
;; How does this work?
;;   Note that a compiled procedure will return with
;;   ('compiled-procedure entry env) list
;;   This list will actually make it to the env (see eg below)
;;
;; (pp (compile '(define (s x) x) 'val 'return))
;;
;; ((env continue)
;;  (val)
;;  ((assign val (op make-compiled-procedure) (label entry25) (reg env))
;;   (goto (label after-lambda26))
;;   entry25
;;   (assign env (op compiled-procedure-env) (reg proc))
;;   (assign env (op extend-environment) (const (x)) (reg argl) (reg env))
;;   (assign val (op lookup-variable-value) (const x) (reg env))
;;   (goto (reg continue))
;;   after-lambda26
;;   (perform (op define-variable!) (const s) (reg val) (reg env))
;;   (assign val (const ok))
;;   (goto (reg continue))))
;;
;;   Since this made it to the environment it should be
;;   callable by the repl
 
;;----------------------------------------------------------------------
;; Allows one to insert compiled code and call it from interpreted env
(define (get-push-depth-statistics)
  (((eceval-single-exp) 'stack) 'statistics))

(define (compile-and-run-non-repl-test)
  (with-primitive-procedures
   `((+ ,+)
     (* ,*)
     (= ,=)
     (- ,-))
   (lambda (env)
     (eceval*
      '(compile-and-run
	'(define (factorial n)
	   (if (= n 1) 1 (* (factorial (- n 1)) n))))
      env)
     ;;
     (pp (eceval* '(factorial 5) env))
     (user-print-env env) (((eceval-single-exp) 'stack) 'print-statistics) (newline)
     ;;
     ;; Note each call to eceval* starts the machine, which clears the stack statistics
     (pp (eceval* '(factorial 5) env))
     (user-print-env env) (((eceval-single-exp) 'stack) 'print-statistics) (newline)
     ;;
     )))

;;----------------------------------------------------------------------
;; Checks if compiled code and interpreted code both affect the env

(define (compile-and-run-non-repl-test0)
  (with-primitive-procedures
   `((+ ,+)
     (* ,*)
     (= ,=)
     (- ,-))
   (lambda (env)
     (eceval*
      '(compile-and-run
     	'(define x 1))
      env)
     (eceval*
      '(define y 2)
      env)
     (pp (list (eceval* 'x env)))
     (pp (list (eceval* 'y env)))
     ;;
     )))

;; (compile-and-run-non-repl-test0)


