# Unit-testing with Ceedling

## Tools
In order to be able to run the available unit-tests, please install Ceedling on your machine from: http://www.throwtheswitch.org/ceedling

## Running tests with Ceedling
In order to run the available unit-tests, go to the desired test folder and run the following command:
```
ceedling test:all
```

## Generating coverage reports with Ceedling
In order to generate coverage reports for the files which are tested, go to the desired test folder and run the following command:
```
ceedling gcov:all utils:gcov
```
The reports will be available in the test folder at the following path:
```
build/artifacts/gcov
```

## Clean the testing workspace with Ceedling
In order to clean the Ceedling testing workspace, go to the desired test folder and run the following command:
```
ceedling clean
```

## Examples in No-OS
### Running tests with Ceedling for IMU drivers:

```
no-OS/tests/drivers/imu> ceedling test:all
```

Generating coverage reports with Ceedling for IMU drivers:

```
no-OS/tests/drivers/imu> ceedling gcov:all utils:gcov
```

The generated reports will be available at the following path:

```
no-OS/tests/drivers/imu/build/artifacts/gcov
```
