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
