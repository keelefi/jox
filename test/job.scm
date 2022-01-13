(use-modules (srfi srfi-64))

(load "../src/jox.scm")

(define (job-debug-print job)
    (display "name: ") (display (job-name-proxy job)) (newline)
    (display "after: ") (display (job-after-proxy job)) (newline)
    (display "before: ") (display (job-before-proxy job)) (newline)
    (display "script: ") (display (job-script-proxy job)) (newline)
    (display "result: ") (display (job-result-proxy job)) (newline))

(test-begin "jobs")

(let*
    ((jobA (make-job "jobA" #:script (delay (system "echo jobA"))))
    (jobB (make-job "jobB" #:after (list jobA) #:script (delay (system "echo jobB")))))
    
    (set! targets (list jobB))

    (job-debug-print jobA)
    (job-debug-print jobB)

    (define job-graph (make-job-graph targets))

    (test-equal "jobB is after jobA" (list jobA) (job-after-proxy jobB))
    (test-equal "jobA is before jobB" (list jobB) (job-before-proxy jobA))

    (job-debug-print jobA)
    (job-debug-print jobB))

(let*
    ((jobA (make-job "jobA" #:script (delay (system "echo jobA"))))
    (jobB (make-job "jobB" #:script (delay (system "echo jobB")))))

    (set-job-before-proxy! jobA (list jobB))
    (set! targets (list jobB))

    (job-debug-print jobA)
    (job-debug-print jobB)

    (define job-graph (make-job-graph targets))

    (test-equal "jobA is before jobB" (list jobB) (job-before-proxy jobA))
    (test-equal "jobB is after jobA" (list jobA) (job-after-proxy jobB))

    (job-debug-print jobA)
    (job-debug-print jobB))

(test-end "jobs")


