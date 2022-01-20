const algorithm = require('./algorithm.js')

const fs = require('fs');
const path = require('path');

PATH_TO_TESTS='../tests/'

ERROR_TYPE_WARNING='WARNING'
ERROR_TYPE_ERROR='ERROR'

const test_vector_files = function(){
    const files_in_dir = fs.readdirSync(PATH_TO_TESTS);

    const result = files_in_dir.filter(filename => {
        if (!fs.lstatSync(PATH_TO_TESTS+filename).isFile()) {
            return false;
        } else if (filename.startsWith('.')) {
            return false;
        } else if (!filename.endsWith('.json')) {
            return false;
        }
        return true;
    });

    return result;
}();

test_vector_files.forEach(test_vector_file =>
    it(test_vector_file, () => {
        // read file
        const whole_filename = PATH_TO_TESTS + test_vector_file;
        const test_data = JSON.parse(fs.readFileSync(whole_filename, 'utf8'));

        const jobs = test_data.input;
        const targets = test_data.targets;
        const jobs_expected = test_data.output;

        let exception_expected = null;
        let warnings_expected = [];
        for (const error of test_data.errors) {
            if (error.type === ERROR_TYPE_WARNING) {
                warnings_expected.push(error.message);
            } else if (error.type === ERROR_TYPE_ERROR) {
                if (exception_expected) {
                    throw new Error('exception redifined, this is not allowed');
                }
                exception_expected = new algorithm.AlgorithmError(error.message);
            } else {
                throw new Error('invalid error type: ' + error.type);
            }
        }

        if (exception_expected) {
            expect(() => {algorithm.algorithm(jobs, targets)}).toThrow(exception_expected.message);
        } else {
            const {jobs_actual, warnings} = algorithm.algorithm(jobs, targets);

            expect(jobs_actual).toMatchObject(jobs_expected);
            expect(warnings).toMatchObject(warnings_expected);
        }
    })
);
