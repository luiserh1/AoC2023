Problem site: https://adventofcode.com/2023/day/2

## Part a

The proposed solution makes uses of linked lists as the structure to store the result of parsing the file conntent

A more "efficient" aproach might involve computing if a game is valid while it's being parsed, but I personally found my version more understandable and the cost difference not that relevant.

The expected result for the data of the provided example (example_a.txt) is **8**

## Part b

Thanks to the structure defined in the previous part, the solution for this one has been as easy as updating the code that looped over it.

The expected result for the data of the provided example (example_b.txt) is **2286**