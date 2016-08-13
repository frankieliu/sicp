(define a '(a b c))
(define b (append '(1 2) a '(3)))
(pp b)
(define exp
  '(compile-and-run 
    '(define (factorial n) 
       (if (= n 1) 
	   1 
	   (* (factorial (- n 1)) n)))))
(pp (cadadr exp))
