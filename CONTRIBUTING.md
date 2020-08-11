# Contribution Guidelines

# Main Rules

* All code must have unit tests written with `cmocka`
* All code must pass valgrind testing with `ctest -T memcheck`
* All code must be formatted with `make format`
* All newly written typedefs, global variables, function declarations, etc.. must having valid doxygen comments
* All commits must be GPG signed

# Memory Allocations

* Where possible conduct memory allocations outside of mutex locks
* This helps minimize the time spend in a lock, thus minimizing the time we block other threads

# Braces


You must always have the opening brace on the same line of the expression. 

For example the following is acceptable

```C
if (abc == true) {
    /* do stuff */
}
```

Whereas the following is not acceptable:

```C
if (abc == true)
{
  /* do stuff */
}
```
