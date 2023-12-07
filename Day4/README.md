Problem site: https://adventofcode.com/2023/day/4

## Part a

The approach for this solution has been similar to the one taken for the Day2 problem. In this case, the parsing of the document has been done reading it line by line. The functions from the _string_ library have been really useful.

The expected result for the data of the provided example (example_a.txt) is **13**

## Part b

The solution seemed easy, but I made two mistakes during the implementation:

- 1. The linked list data structure requiered some modifications in order to read it from the beginning and not from the end

- 2. At first, the _amount_ field of the _Card_ struct was of type _uint8_t_. This resulted in an overflow, thus the final sum was way below the spected

The expected result for the data of the provided example (example_a.txt) is **30**