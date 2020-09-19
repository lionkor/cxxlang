# cxxlang

I'm trying to make a scripting language :)

## Examples

### 1

Input:

```c
if (true or false) {
    print("it's true!");
    print("isn't this cool?!");
}
if ((false and false) or true) {
    print("wowzie, we can do boolean expressions!?");
}
```

Output: 

```
→ running...
it's true!
isn't this cool?!
wowzie, we can do boolean expressions!?
→ returned OK
```

### 2

Input:

```c
print(string_concat("im so ", string_concat("damn " , "happy")));
```

which is silly, but is used to demonstrate that functions work as expected!

Output:

```
→ running...
im so damn happy
→ returned OK
```

### 3

Input: 

```c
Number my_number := 4.65;
String str := "My number is: ";
print(string_concat(str, number_to_string(my_number)));
```

Output:

```
Hello, World! My number is: 4.650000
```

