# tUtil
Very basic timing library, for measuring code performance.
Timing information is saved to chrome trace event format.

## Usage

*FunctionTimer* macro should be added to function which duration we want to measure.

*generateRaport()* is used do create raport file.

To view the raport, open chrome browser, go to *chrome://tracing*, and load generated file.

## Example 
main.cpp shows typical tUtil usage.