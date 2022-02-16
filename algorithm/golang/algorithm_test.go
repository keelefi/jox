package algorithm

import (
    "encoding/json"
    "fmt"
    "io/ioutil"
    "os"
    "strings"
    "testing"

    "github.com/google/go-cmp/cmp"
)

const TEST_FILES_PATH = "../tests"

type TestCaseData struct {
    input           []Job
    targets         []string
    output          []Job
    warnings        []AlgorithmWarning
    errorExpected   AlgorithmError
}

func readStringSlice(data interface{}, key string) []string {
    var result []string

    valueInterface, ok := data.(map[string]interface{})[key]
    if !ok {
        return make([]string, 0)
    }

    valueInterfaceSlice := valueInterface.([]interface{})
    result = make([]string, len(valueInterfaceSlice))
    for i, v := range valueInterfaceSlice {
        result[i] = fmt.Sprintf("%s", v)
    }

    return result
}

func jsonToJob(key string, value interface{}) Job {
    var result Job

    result.name = key
    result.after = readStringSlice(value, "after")
    result.before = readStringSlice(value, "before")

    return result
}

func filterFiles(files []os.FileInfo) []os.FileInfo {
    var result []os.FileInfo

    for _, v := range files {
        if v.IsDir() {
            continue
        }
        if strings.HasPrefix(v.Name(), ".") {
            continue
        }
        if !strings.HasSuffix(v.Name(), ".json") {
            continue
        }
        result = append(result, v)
    }

    return result
}

func getTestFiles(t *testing.T) ([]os.FileInfo, error) {
    files, err := ioutil.ReadDir(TEST_FILES_PATH)
    if err != nil {
        t.Fatalf(fmt.Sprintf("Could not read test files path: %s", TEST_FILES_PATH))
        return nil, err
    }

    result := filterFiles(files)

    return result, nil
}

func parseJobs(t *testing.T, data map[string]interface{}) []Job {
    result := make([]Job, len(data))
    i := 0
    for key, value := range data {
        result[i] = jsonToJob(key, value)
        i = i + 1
    }
    return result
}

func parseTargets(t *testing.T, data []interface{}) []string {
    result := make([]string, len(data))
    for i, v := range data {
        result[i] = fmt.Sprintf("%s", v)
    }
    return result
}

func parseErrors(t *testing.T, data []interface{}) ([]AlgorithmWarning, AlgorithmError) {
    var warnings []AlgorithmWarning
    var errorExpected AlgorithmError

    for _, v := range data {
        v_map := v.(map[string]interface{})
        errorType, err := GetErrorType(v_map["type"].(string))
        if err != nil {
            t.Fatalf("error: %s", err)
        }
        if errorType == ErrorTypeError {
            if errorExpected.errorEnumeration != ErrorNone {
                t.Fatalf("Error set twice")
            }
            errorEnumeration, err := GetErrorEnumeration(v_map["error"].(string))
            if err != nil {
                t.Fatalf("error: %s", err)
            }
            errorExpected.errorEnumeration = errorEnumeration
            errorExpected.message = v_map["message"].(string)
        } else if errorType == ErrorTypeWarning {
            warningEnumeration, err := GetWarningEnumeration(v_map["error"].(string))
            if err != nil {
                t.Fatalf("error: %s", err)
            }
            newWarning := AlgorithmWarning{
                warningEnumeration: warningEnumeration,
                message: v_map["message"].(string),
            }
            warnings = append(warnings, newWarning)
        }
    }

    return warnings, errorExpected
}

func parseFile(t *testing.T, filename string) TestCaseData {
    content, err := ioutil.ReadFile(fmt.Sprintf("%s/%s", TEST_FILES_PATH, filename))
    if err != nil {
        t.Fatalf("Could not read JSON file: %s", filename)
    }

    var result TestCaseData

    var data map[string]interface{}
    err = json.Unmarshal(content, &data)
    if err != nil {
        t.Fatalf("Unmarshalling %s failed", filename)
    }

    result.input = parseJobs(t, data["input"].(map[string]interface{}))
    result.output = parseJobs(t, data["output"].(map[string]interface{}))
    result.targets = parseTargets(t, data["targets"].([]interface{}))
    result.warnings, result.errorExpected = parseErrors(t, data["errors"].([]interface{}))

    return result
}

func TestAlgorithm(t *testing.T) {
    testFiles, err := getTestFiles(t)
    if err != nil {
        return
    }

    for _, testFile := range testFiles {
        t.Run(testFile.Name(), func (t *testing.T) {
            testCaseData := parseFile(t, testFile.Name())

            // TODO: remove these
            for _, v := range testCaseData.input {
                t.Logf("Job name: %s\n", v.name)
                t.Logf("  after: %s\n", v.after)
                t.Logf("  before: %s\n", v.before)
            }
            for _, v := range testCaseData.output {
                t.Logf("Job name: %s\n", v.name)
                t.Logf("  after: %s\n", v.after)
                t.Logf("  before: %s\n", v.before)
            }
            for _, v := range testCaseData.targets {
                t.Logf("  %s\n", v)
            }
            for _, v := range testCaseData.warnings {
                t.Logf("        enum: %s\n", v.warningEnumeration)
                t.Logf("     message: %s\n", v.message)
            }
            if testCaseData.errorExpected.errorEnumeration != ErrorNone {
                t.Logf("      enum: %s\n", testCaseData.errorExpected.errorEnumeration)
                t.Logf("   message: %s\n", testCaseData.errorExpected.message)
            }

            jobsActual, warningsActual, err := Algorithm(testCaseData.input, testCaseData.targets)

            if testCaseData.errorExpected.errorEnumeration == ErrorNone {
                // No error expected, check jobsActual and warningsActual
                jobsDiff := cmp.Diff(jobsActual, testCaseData.output)
                if jobsDiff != "" {
                    t.Errorf("jobsActual does not match jobsExpected:\n%s", jobsDiff)
                }
                warningsDiff := cmp.Diff(warningsActual, testCaseData.warnings)
                if warningsDiff != "" {
                    t.Errorf("warningsActual does not match warningsExpected:\n%s", warningsDiff)
                }
            } else {
                // Error expected, ignore other values and only check err
                if err != nil {
                    if err.Error() != testCaseData.errorExpected.message {
                        t.Errorf("Expected error '%s', but got '%s'", testCaseData.errorExpected.message, err.Error())
                    }
                } else {
                    t.Errorf("Expected error '%s', but got none", testCaseData.errorExpected.message)
                }
            }
        })
    }

    return
}

