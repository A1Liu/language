# Ideas to Include in a new Programming Language
Focus is on programmer ergonomics. Should be as easy to use as python and as fast
as Java.

1. Heavy type inference; type inference of variables, and also of function parameters
   and return type; interfaces are made for every individual method, and also for
   every method combination that is inferred. Classes don't inherit from other classes,
   instead they inherit from interfaces, and the implementation is copied over.
2. Operator overloads - supported via interfaces, uses function with special name
   plus decorator
3. Python syntax
4. No generics
5. Inheritance uses interfaces, allows for multiple inheritance
6. Compile-time errors are reported but only stop execution if they're on a potential
   runtime path.
7. Error-reporting can be turned on and off again at will
8. Imports work like python
9. Execution order works like python, but function names do not have to be
   written in order
0. Casts are inserted whenever necessary, but emit a warning.
1. Class meta-types
2. Match statements; supports match on class of object
3. Generators, coroutines, asyncio, all supported
4. implicit return of None
5. Shadowing is not allowed, except accross function boundaries
6. Variables are implicitly initialized to none
7. If, Else, etc. do not produce a new scope.
