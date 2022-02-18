## Barber Shop Synchronization Problem

`main_with_id.cpp` - Customer Threads has a unique id and Barber thread uses that to serve customer thread.
`main_without_id.cpp` - Barber Thread may signal different customer thread that is not being served.

## Compile Instruction

`g++ <filename.cpp> -o <filename> -pthread`
