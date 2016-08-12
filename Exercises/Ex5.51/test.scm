(define (tagged-list? exp tag)
  (if (pair? exp)
      (eq? (car exp) tag)
      #f))

(define (quoted? exp) (tagged-list? exp 'quote))

(define (my-eval exp env)
 (cond ((quoted? exp) (pp exp))
	(else (pp "done"))))

(my-eval '(quote 5 132) 1)
