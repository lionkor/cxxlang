# cxxlang

I'm trying to make a scripting language :)

Currently working example:

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


Another one: 

```c
print(string_concat("im so ", string_concat("damn " , "happy")));
```

which is silly, but is used to demonstrate that functions work as expected!

output: 
```
→ running...
im so damn happy
→ returned OK
```
