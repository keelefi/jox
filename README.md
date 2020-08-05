# Job Executor

Job Executor is a program that starts other programs. It takes as input argument
a yaml file containing the configuration which programs to start. The programs
will be started in parallel (when possible).

## Known bugs

### Programs that fork and create further children are not waited on correctly.

Instead of waiting on all the children that a spawned process has created, Job
Executor only waits on the main process which it spawned itself.

## YAML

The yaml input argument specifies which programs to start.

Example yaml file:

    jobs:
    - name: init
      exec: echo init
    - name: hello
      after: [init]
      exec: echo hello world

### Job

Every configuration file must include one `jobs` yaml node. Under this node,
specify all the programs to run.

#### `name`: *string*

Set the name of the job. The name is used by other jobs to reference this job.

Note: This field is mandatory.

#### `after`: *array of strings*

Set the jobs that must complete before this job can be started.

#### `before`: *array of strings*

Set the jobs that must wait for this job to complete.

#### `exec`: *string*

Command to execute.

Note: This field is mandatory.

## Building

Prerequisites:

* C++17
* CMake
* `yaml-cpp`

To build:

    $ mkdir bin
    $ cd bin
    $ cmake ..
    $ cd ..
    $ make -Cbin

## Running

To run:

    $ bin/src/job-executor <yaml-file>

## Testing

Currently, there are no tests for Job Executor.
