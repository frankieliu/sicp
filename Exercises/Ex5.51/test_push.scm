(define s1 '(1 2))
(define s2 '(2 3))
(define s3 (cons s1 s2))
(eq? (car s3) s1)
