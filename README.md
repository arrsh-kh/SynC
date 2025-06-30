# SynC
---
A C based language with syntax of python!
---
Syntax:
```bash
def test() {
    a = 1 + 2
    b = 3 + 4
    c = a + b
    return c
}
---
test()
```
---
# How to run
```python
make
./syncc.out main.sync > output.c
gcc output.c -o run

```
---
# Why?
I want to learn bootstrapping
---
# Progress report
- lexer works
- parser works
- interpreter works
- you can now do addition
- added other mathematical operations
- parser fails to parse because of the above

![](https://i.imgflip.com/9yxdcw.jpg)
