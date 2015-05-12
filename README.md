Unit testing an EPICS Database
==============================

An example of writing a unit test of an EPICS process database
using features added in EPICS Base 3.15.1.
Demonstrates interacting with a PDB through get/put as well
as direct inspection of the record structure.

A [subRecord is provided](utestApp/src/dut.c)
(complete with bugs).
The [test code](utestApp/src/unittest.c)
loads one instance of
[a .db file](utestApp/src/dut.db)
and makes some tests.

```bash
git clone https://github.com/mdavidsaver/utestdemo.git
cd utestdemo
# edit EPICS_BASE in [configure/RELEASE](configure/RELEASE)
make -s
make runtests -s
```

The test 'utest' will fail 4 of its test cases.
To see the full test output:

```base
cd utestApp/src/O.linux-x86
./utest
```
