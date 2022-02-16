package algorithm

import (
    "errors"
    "fmt"
)

type Job struct {
    name    string
    after   []string
    before  []string
}

type ErrorType int

const (
    ErrorTypeNone ErrorType = iota
    ErrorTypeError
    ErrorTypeWarning
)

type ErrorEnumeration int

const (
    ErrorNone ErrorEnumeration = iota
    ErrorCyclicDependency
    ErrorJobDependsOnItself
    ErrorNoTargets
    ErrorJobNotFound
)

type WarningEnumeration int

const (
    WarningJobNotRequired = iota
)

func GetErrorType(errorType string) (ErrorType, error) {
    if errorType == "ERROR" {
        return ErrorTypeError, nil
    }
    if errorType == "WARNING" {
        return ErrorTypeWarning, nil
    }
    return ErrorTypeNone, errors.New(fmt.Sprintf("Unknown error type: %s", errorType))
}

func (e ErrorEnumeration) String() string {
    switch e {
    case ErrorCyclicDependency:
        return "CYCLIC DEPENDENCY"
    case ErrorJobDependsOnItself:
        return "JOB DEPENDS ON ITSELF"
    case ErrorNoTargets:
        return "NO TARGETS"
    case ErrorJobNotFound:
        return "JOB NOT FOUND"
    }
    return "ERROR"
}

func GetErrorEnumeration(errorString string) (ErrorEnumeration, error) {
    if errorString == "CYCLIC DEPENDENCY" {
        return ErrorCyclicDependency, nil
    }
    if errorString == "JOB DEPENDS ON ITSELF" {
        return ErrorJobDependsOnItself, nil
    }
    if errorString == "NO TARGETS" {
        return ErrorNoTargets, nil
    }
    if errorString == "JOB NOT FOUND" {
        return ErrorJobNotFound, nil
    }
    return 0, errors.New(fmt.Sprintf("Unknown error enumeration: %s", errorString))
}

func (w WarningEnumeration) String() string {
    switch w {
    case WarningJobNotRequired:
        return "JOB NOT REQUIRED"
    }
    return "ERROR"
}

func GetWarningEnumeration(warningString string) (WarningEnumeration, error) {
    if warningString == "JOB NOT REQUIRED" {
        return WarningJobNotRequired, nil
    }
    return 0, errors.New(fmt.Sprintf("Unknown warning enumeration: %s", warningString))
}

type AlgorithmError struct {
    errorEnumeration    ErrorEnumeration
    message             string
}

type AlgorithmWarning struct {
    warningEnumeration  WarningEnumeration
    message             string
}

type CyclicDependencyError struct {}
type JobDependsOnItselfError struct {
    jobName string
}
type NoTargetsError struct {}
type JobNotFoundError struct {
    depender    string
    dependee    string
}

type JobNotRequiredWarning struct {
    jobName string
}

func (e *CyclicDependencyError) Error() string {
    return "Cyclic dependency detected"
}

func (e *JobDependsOnItselfError) Error() string {
    return fmt.Sprintf("Job '%s' depends on itself", e.jobName)
}

func (e *NoTargetsError) Error() string {
    return "No targets"
}

func (e *JobNotFoundError) Error() string {
    return fmt.Sprintf("Job '%s' references job '%s', but job '%s' does not exist", e.depender, e.dependee, e.dependee)
}

func (w *JobNotRequiredWarning) Warning() string {
    return fmt.Sprintf("Job '%s' is not required", w.jobName)
}

func Algorithm(input []Job, targets []string) ([]Job, []AlgorithmWarning, error) {
    var output []Job
    var warnings []AlgorithmWarning

    // TODO: implementation

    return output, warnings, nil
}

