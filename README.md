# calc(n)

Simple command line calculator.

## Compile
### Windows
```
gcc src/calc.c -o calc.exe
```

## Usage
Notes:
* **Use `x` rather than `*` for multiplication**
* Currently only supports division, multiplication, addition and subtraction ([see issues](https://github.com/thomseddon/calc/issues) for roadmap)
* On windows substitue `calc` for `calc.exe` below

```
calc <equation>
```

## Example
Most simple usage of `calc(n)`:
```
calc 1 + 1
= 2
```

Precedence is correctly respected:
```
calc 2 + 4 x 50 + 2
= 204
```

## Roadmap
See [feature requests in issues](https://github.com/thomseddon/calc/issues)

## License
[GPL 3](https://github.com/thomseddon/calc/blob/master/LICENSE)