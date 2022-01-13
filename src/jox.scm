(use-modules (srfi srfi-9))

(define-record-type job
    (make-job-impl name after before script)
    employee?
    (name       job-name)
    (after      job-after       set-job-after!)
    (before     job-before      set-job-before!)
    (script     job-script      set-job-script!)
    (result     job-result      set-job-result!))

(define* (make-job name #:key (after '()) (before '()) (script '()))
    (make-job-impl name after before script))

;;; These getters are necessary so that the generated functions can be called from C code
(define (job-name-proxy     job) (job-name      job))
(define (job-after-proxy    job) (job-after     job))
(define (job-before-proxy   job) (job-before    job))
(define (job-script-proxy   job) (job-script    job))
(define (job-result-proxy   job) (job-result    job))

;;; Same as above, but for setters
(define (set-job-after-proxy!   job after)  (set-job-after!    job after))
(define (set-job-before-proxy!  job before) (set-job-before!   job before))
(define (set-job-script-proxy!  job script) (set-job-script!   job script))
(define (set-job-result-proxy!  job result) (set-job-result!   job result))

(define targets '())

(define (add-job-before! job add)
    (if (not (member add (job-before job)))
        (if (not (null? (job-before job)))
            (set-job-before! job (cons (job-before job) (cons add '())))
            (set-job-before! job (list add)))))

(define (set-job-after-reversals! job jobs)
    (if (not (null? jobs))
        (begin
            (add-job-before! (car jobs) job)
            (set-job-after-reversals! job (cdr jobs)))))

(define (add-job-after! job add)
    (if (not (member add (job-after job)))
        (if (not (null? (job-after job)))
            (set-job-after! job (cons (job-after job) (cons add '())))
            (set-job-after! job (list add)))))

(define (set-job-before-reversals! job jobs)
    (if (not (null? jobs))
        (begin
            (add-job-after! (car jobs) job)
            (set-job-before-reversals! job (cdr jobs)))))

(define (set-dependencies job)
    (set-job-after-reversals! job (job-after job))
    (set-job-before-reversals! job (job-before job)))

(define (make-job-graph targets)
    (cond
        ((null? targets) '())
        (else
            (cons (set-dependencies (car targets))
                  (make-job-graph (cdr targets))))))
