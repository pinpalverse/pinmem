A basic on-runtime memory analysis tool.

Features:
- It tells you how many bytes through pmalloc(and freed through pfree) you've allocated that were never touched
- It tells you what pointers that were allocated through pmalloc were not freed when the program exits


To add:
- calloc
- realloc
- reallocarray