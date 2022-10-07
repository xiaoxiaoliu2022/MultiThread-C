# MultiThread-C
Multi-threaded Searching

## Usage
```
make
```
![](screenshot.png)

## Observations
- The key is to make sure data has been read into array before search
- mutex is a good way to protect result array from overwritten by different threads
- The difference in time consumed using 1 and 4 threads is minimal, maybe due to the small number of data
ls
