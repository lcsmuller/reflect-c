# KNOWN BUGS

This file outlines the known bugs that are yet to be fixed

## Continuation line inside comments
- The C preprocessor will automatically expand any `\` character it encounters, regardless if it was inside of a comment or not,
so in the following:
```c
/*#! hello \
     whatsup?
*/
```
Will end up expanding to:
```c
/*#! hello     whatsup? 
*/
```
Which has been shown to something result in issues once `Reflect-C` performs its post-expansion. A possible solution is running a `pre-expansion` script that converts all `\` characters it encounters inside of a `Reflect-C style comment` to some other character, and then the `post-expansion` should convert it back to the original.
