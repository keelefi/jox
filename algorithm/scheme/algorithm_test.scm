#!/usr/bin/guile -s
!#

(use-modules (srfi srfi-11)
             (srfi srfi-64))

(use-modules (json))

;;; auxiliary function to test equality of contents in alists
(define (alist-equal? alist-left alist-right)
    (define (alist-equal-internal? alist-left alist-right)
        (define (alist-equal-element? pair alist)
            (define second-pair (assoc (car pair) alist))
            (cond
                ((boolean? second-pair) #f)
                (else (equal? (cdr pair) (cdr second-pair)))))
        (cond
            ((null? alist-left) #t)
            ((alist-equal-element? (car alist-left) alist-right) (alist-equal-internal? (cdr alist-left) alist-right))
            (else #f)))
    (cond
        ((equal? (length alist-left) (length alist-right)) (alist-equal-internal? alist-left alist-right))
        (else #f)))

(include "algorithm.scm")

(define (get-test-file-names)
    (define result '())
    (define test-file-dir "../tests")
    (define dir-obj (opendir test-file-dir))
    (do ((file-entry (readdir dir-obj) (readdir dir-obj)))
        ((eof-object? file-entry))
        (set! result (cons file-entry result)))
    (closedir dir-obj)
    result)

(define (is-valid-file basename)
    (let ((filename (string-append "../tests/" basename)))
        (cond
            ((not (file-exists? filename)) #f)
            ((equal? "." (string-take basename 1)) #f)
            ((not (equal? ".json" (string-take-right basename 5))) #f)
            (else #t))))

(define (is-error error-obj)
    (equal? "ERROR" (cdr (assoc "type" error-obj))))
(define (is-warning error-obj)
    (equal? "WARNING" (cdr (assoc "type" error-obj))))

(define (run-test-case filename)
    (define json-document
        (with-input-from-file (string-append "../tests/" filename)
            (lambda () (json->scm (current-input-port)))))

    (define jobs-input (assoc "input" json-document))
    (define jobs-output (assoc "output" json-document))
    (define targets (assoc "targets" json-document))
    (define errors (assoc "errors" json-document))

    (define output-expected (cdr jobs-output))

    (define warnings-expected (filter is-warning (array->list (cdr errors))))
    (define error-expected (filter is-error (array->list (cdr errors))))

    (cond 
        ((null? error-expected)
            (let-values (((output-actual warnings-actual) (algorithm (cdr jobs-input) targets)))
                (test-assert "output" (alist-equal? output-expected output-actual))
                (test-equal "warnings" warnings-expected warnings-actual)))
        (else
            (with-exception-handler
                (lambda (exception-obj)
                    (test-equal "exception message"
                        (cdr (assoc "message" (car error-expected)))
                        (algorithm-exception-message exception-obj)))
                (lambda ()
                    (algorithm (cdr jobs-input) targets)
                    (test-assert "no exception was thrown" #f))
                #:unwind? #t
                #:unwind-for-type &algorithm-exception))))

(define (make-test-case-name filename)
    (string-append "test-" (basename filename ".json")))

(define test-filenames (filter is-valid-file (get-test-file-names)))

(define (run-test-cases filenames)
    (cond
        ((not (null? filenames))
            (let* ((filename (car filenames)) (test-case-name (make-test-case-name filename)))
                (test-begin test-case-name)
                (run-test-case filename)
                (test-end test-case-name)
                (run-test-cases (cdr filenames))))
        (else #t)))

(run-test-cases test-filenames)

