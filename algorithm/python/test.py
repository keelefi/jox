import unittest

from parameterized import parameterized

from os import listdir
from os.path import isfile, join

import json

import algorithm
import job

PATH_TO_TESTS='../tests/'

ERROR_TYPE_WARNING='WARNING'
ERROR_TYPE_ERROR='ERROR'

def test_vector_filenames():
    def is_valid_file(filename):
        if not isfile(join(PATH_TO_TESTS, filename)):
            return False
        if filename.startswith('.'):
            return False
        if not filename.endswith('.json'):
            return False
        return True
    test_vector_filenames = [f for f in listdir(PATH_TO_TESTS) if is_valid_file(f)]
    return test_vector_filenames

class TestAlgorithm(unittest.TestCase):
    @parameterized.expand(test_vector_filenames)
    def testAlgorithm(self, test_vector_filename):
        # read file
        with open(join(PATH_TO_TESTS, test_vector_filename), 'r') as test_vector_file:
            test_vector_json = test_vector_file.read()

        # parse JSON
        test_data = json.loads(test_vector_json)

        # setup input jobs
        jobs = {}
        for input_job in test_data['input']:
            jobs[input_job] = job.Job(input_job, test_data['input'][input_job])

        # setup targets
        targets = test_data['targets']

        # setup output jobs
        jobs_expected = {}
        for output_job in test_data['output']:
            jobs_expected[output_job] = job.Job(output_job, test_data['output'][output_job])

        # setup expected errors
        exception_expected = None
        warnings_expected = []
        for error in test_data['errors']:
            if error['type'] == ERROR_TYPE_WARNING:
                warnings_expected.append(algorithm.AlgorithmWarning(error['message']))
            elif error['type'] == ERROR_TYPE_ERROR:
                if exception_expected:
                    # TODO: exception redifined, this is not allowed
                    pass
                exception_expected = algorithm.AlgorithmException(error['message'])
            else:
                # TODO: report error
                pass

        # run algorithm
        if exception_expected:
            with self.assertRaises(algorithm.AlgorithmException,
                                   msg='Expected AlgorithmException, but none was thrown') as context_manager:
                algorithm.algorithm(jobs, targets)
            the_exception = context_manager.exception
            self.assertEqual(the_exception, exception_expected, 'Actual exception does not match expected exception')
        else:
            warnings = algorithm.algorithm(jobs, targets)

            self.assertEqual(jobs, jobs_expected, 'Actual jobs do not match expected output jobs')
            self.assertEqual(warnings, warnings_expected, 'Actual warnings do not match expected warnings')
