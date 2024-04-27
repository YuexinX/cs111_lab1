## UID: 605816980

## Pipe Up

This pipe c program replicates the behavior of pipe (|) operator in shells.

## Building
always run make clean first to make sure all old binary files are removed
then run make to build the program

```shell
make clean
make
```


## Running

the output of this program should be the same as the output you get from a pipe(|) operator

when my working directory is '/w/home.25/home/yxu404/cs111/lab1'
```shell
./pipe pwd cat wc
```
shoudld output the same result as that of
```shell
pwd|cat|wc
```
which is

1       1      34

For the following pipeline:
```shell
ls|sort|sort|sort|sort|sort|sort|wc
8       8      73
```
our program should also have the same output
```shell
./pipe ls sort sort sort sort sort sort cat wc
8       8      73
```



## Cleaning up

To clean up all binary files, we use the following command
```shell
make clean
```
under our makefile directory

## python unittest result

..      8       8      73
.
----------------------------------------------------------------------
Ran 3 tests in 8.840s

OK