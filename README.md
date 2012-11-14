# calc(n)

Simple command line calculator.

## Compile
### Unix
```
gcc src/calc.c -lm -o calc
```

### Windows
```
gcc src/calc.c -o calc.exe
```

## Usage

```
calc <equation>
```
Currently Supports: **powers**, **division**, **multiplication**, **addition** and **subtraction** ([see issues](https://github.com/thomseddon/calc/issues) for roadmap)

Windows Notes:
* **Use `x` rather than `*` for multiplication,** or surround the whole argument in speech marks: `"<equation>"`
* **Use `'` rather than `^` for powers,** or surround the whole argument in speech marks: `"<equation>"`
* Substitue `./calc` for `calc.exe` below

## Example
Most simple usage of `calc(n)`:
```
$ ./calc 1 + 1
= 2
```

Precedence is correctly respected:
```
$ ./calc 2 + 4 x 50 + 2
= 204
```

Powers also work
```
$ ./calc 2^3
= 9
```

A few constants (e, pi and phi) are built in:
```
$ ./calc 2 * pi
= 6.28318530717959
```

## Roadmap
See [feature requests in issues](https://github.com/thomseddon/calc/issues)

## License
[GPL 3](https://github.com/thomseddon/calc/blob/master/LICENSE)