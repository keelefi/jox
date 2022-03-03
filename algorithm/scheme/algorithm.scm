#!/usr/bin/guile -s
!#

(use-modules
    (ice-9 exceptions)
    (ice-9 format)
    (srfi srfi-9))

(define-record-type job
    (make-job name after before)
    job?
    (name job-name)
    (after job-after set-job-after!)
    (before job-before set-job-before!))

(define-exception-type
    &algorithm-exception        ; name
    &error                      ; parent
    make-algorithm-exception    ; constructor
    algorithm-exception?        ; predicate
    (message algorithm-exception-message))

(define-exception-type
    &cyclic-dependency-exception
    &algorithm-exception
    make-cyclic-dependency-exception-internal
    cyclic-dependency-exception?)
(define (make-cyclic-dependency-exception)
    (make-cyclic-dependency-exception-internal "Cyclic dependency detected"))

(define-exception-type
    &job-depends-on-itself-exception
    &algorithm-exception
    make-job-depends-on-itself-exception-internal
    job-depends-on-itself-exception?)
(define (make-job-depends-on-itself-exception job-name)
    (make-job-depends-on-itself-exception-internal (format #f "Job '~a' depends on itself" job-name)))

(define-exception-type
    &no-targets-exception
    &algorithm-exception
    make-no-targets-exception-internal
    no-targets-exception?)
(define (make-no-targets-exception)
    (make-no-targets-exception-internal "No targets"))

(define-exception-type
    &job-not-found-exception
    &algorithm-exception
    make-job-not-found-exception-internal
    job-not-found-exception?)
(define (make-job-not-found-exception job-depender job-dependee)
    (make-job-not-found-exception-internal
        (format #f "Job '~a' references job '~a', but job '~a' does not exist" job-depender job-dependee job-dependee)))

(define (algorithm jobs targets)
    (define warnings '())
    ; TODO: implementation
    (values jobs warnings))
