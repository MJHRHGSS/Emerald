# Emerald

Emerald is a kid-friendly interpreted scripting language built to be clean, expressive, and powerful. It's designed for quick scripts, configuration, and teaching.

---

## Install

### Linux/Mac

```bash
git clone https://github.com/MJRHGSS/Emerald.git
sudo sh "$HOME/Emerald/scripts/install/install.sh"
```

### Windows

Run your chosen CLI as administrator then these commands:

#### PowerShell

```ps1
git clone https://github.com/MJHRHGSS/Emerald.git
& "C:\Users\$env:USERNAME\Emerald\scripts\install\install.ps1"
```

#### Batch

```bat
git clone https://github.com/MJHRHGSS/Emerald.git
C:\Users\%USERNAME%\Emerald\scripts\install\install.bat
```

---

## Run

The Emerald interpreter follow this simple command syntax:

```bash
emr <file.emr> [--log file.log] [--version] [--help]
```

Of course, excluding the <>

The `--version` and `--help` flags cannot be applied with other flags

---

## Example code

### Comment

```emr
comment this is ignored
```

### Hello world

```emr
say "Hello, world!"
```

### Variables

```emr
text str = "Hi"
number num = 87
yesno bool = yes
char c = 'c'
list l = ("Hi", 8, ('\n', 4.3), no)

say "str=[str], num=[num], bool=[bool], c=[c], l=[l]"
```

### Functions (actions)

```emr
number action number a, number b is
    give a + b

say "1+1=[add 1, 1]"

comment actions have return types, you can remove the type to make a void action
```

### if statements

```emr
if CONDITION or (CONDITION and CONDITION) then
    ...
otherwise if CONDITION then
    ...
otherwise then
    ...
```

### loops

```emr
repeat 8 times
    number n = times.now
    say "[n]"

number n = 0
until n == 100 do
    say "[n]"
    n = n + 1
    
repeat unlimited times
    say "this is a forever loop"
```

---

## Contribution

Contributions, whether in pull requests or issues, are always welcome.

---

## License

[MIT](https://mit-license.org)
