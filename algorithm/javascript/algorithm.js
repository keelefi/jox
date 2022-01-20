class AlgorithmError extends Error {
    constructor(message) {
        super(message);
        this.name = 'AlgorithmError';
    }
}

class CyclicDependencyError extends AlgorithmError {
    constructor() {
        super('Cyclic dependency detected');
        this.name = 'CyclicDependencyError';
    }
}

class JobDependsOnItselfError extends AlgorithmError {
    constructor(job_name) {
        super(`Job '${job_name}' depends on itself`);
        this.name = 'JobDependsOnItselfError';
        this.job_name = job_name;
    }
}

class NoTargetsError extends AlgorithmError {
    constructor(job_name) {
        super('No targets');
        this.name = 'NoTargetsError';
    }
}

class JobNotFoundError extends AlgorithmError {
    constructor(job_depender, job_dependee) {
        super(`Job '${job_depender}' depends on job '${job_dependee}', but job '${job_dependee}' does not exist`);
        this.name = 'JobNotFoundError';
        this.job_depender = job_depender;
        this.job_dependee = job_dependee;
    }
}

class AlgorithmWarning {
    constructor(message) {
        this.message = message;
    }
}

exports.algorithm = function (jobs, targets) {
    let jobs_actual = jobs;
    let warnings = [];

    // TODO: implementation

    return {
        jobs_actual: jobs_actual,
        warnings: warnings,
    };
}

exports.AlgorithmError = AlgorithmError;
