(define init (make-job "init" #:script (delay (system "echo init"))))
(define hello (make-job "hello" #:after (list "init") #:script (delay (system "echo hello"))))

(set! targets (list hello))
