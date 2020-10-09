Description will be added later.


Cursor
constructors:
Cursor(Text)
Cursor(Cursor)

inc(int) move position to currpos + int
Cursor += int  move position to currpos + int
size_t operator + (size_t p) add to pos + p and returns sum, Cursor is not changed
Cursor ++  move position to + 1
Cursor += int  move position to currpos - int

moveto(string pattern, findfunc)  finds patten with findfunc and sets pos to begin of matched string
moveto_end(string pattern, findfunc)  finds patten with findfunc and sets pos to end of matched string
+= int move position to currpos + int