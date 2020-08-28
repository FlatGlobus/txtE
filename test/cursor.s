print("----------------Cursor---------------------------");

var text_c = Text();
text_c.load("txt.txt");
var cursor_c  = Cursor(text_c);

cursor_c = 2;
if(cursor_c.move_to_word_begin() == 0)
{
	print("Cursor: move_to_word_begin true");
}
else
{
	print("Cursor: move_to_word_begin false");
}

cursor_c = 10;
if(cursor_c.move_to_word_begin() == 6)
{
	print("Cursor: move_to_word_begin true");
}
else
{
	print("Cursor: move_to_word_begin false");
}

cursor_c = 0;
if(cursor_c.inc(1) == 1)
{
	print("Cursor: inc true");
}
else
{
	print("Cursor: inc false");
}

if(cursor_c.end().inc(1).is_eof())
{
	print("Cursor: inc true");
}
else
{
	print("Cursor: inc false");
}

cursor_c = npos - 1;
if(cursor_c.inc(2).is_eof())
{
	print("Cursor: inc (npos -1) + 2 true");
}
else
{
	print("Cursor: inc (npos -1) + 2 false");
}

cursor_c = 0;
if(cursor_c.dec(2).is_eof())
{
	print("Cursor: dec 0 - 2 true");
}
else
{
	print("Cursor: dec 0 - 2 true");
}

cursor_c = 0;
if(cursor_c.move_to("abcd", find) == false)
{
	print("Cursor: move_to abcd true");
}
else
{
	print("Cursor: move_to abcd false");
}

cursor_c = 0;
if(cursor_c.move_to("voluptatem", find) == true)
{
	print("Cursor: move_to voluptatem true");
}
else
{
	print("Cursor: move_to voluptatem false");
}

cursor_c = 0;
if(cursor_c.move_to(["abcd", "accusantium"], find) == true)
{
	print("Cursor: move_to abcd or accusantium true");
}
else
{
	print("Cursor: move_to abcd or accusantium false");
}

if(text_c.get(cursor_c.move_to("{", "}", find), 7)  == "} dicta")
{
	print("Cursor: move_to { } true");
}
else
{
	print("Cursor: move_to { } false 2");
}

var text_m = Text("aaaaabbbbbcccc");
var cursor_m  = Cursor(text_m);

if(cursor_m.move_while("ba") == 9)
{
	print("Cursor: move_while true");
}
else
{
	print("Cursor: move_while false");
}

cursor_m = 0;
if(cursor_m.move_while("bac") == 13)
{
	print("Cursor: move_while true");
}
else
{
	print("Cursor: move_while false");
}

cursor_m = 0;
if(cursor_m.move_until("c") == 9)
{
	print("Cursor: move_until true");
}
else
{
	print("Cursor: move_until false");
}

cursor_c = 0;
if(cursor_c.move_to_end("Lorem", find) == 5)
{
	print("Cursor: move_to_end true");
}
else
{
	print("Cursor: move_to_end false");
}

cursor_c = 0;
if(cursor_c.move_to_end(["Lorem","ipsum"], find) == 5)
{
	print("Cursor: move_to_end true");
}
else
{
	print("Cursor: move_to_end false");
}

cursor_c.next_word();
if(text_c.get_word(cursor_c) == "ipsum")
{
	print("Cursor: next_word true");
}
else
{
	print("Cursor: next_word false");
}

text_m = "Lorem ipsum";
cursor_m = 0;
cursor_m.move_to_end("Lorem", find);
cursor_m.next_word();
if(text_m.get_word(cursor_m) == "ipsum")
{
	print("Cursor: next_word true");
}
else
{
	print("Cursor: next_word false");
}

if(cursor_m.move_to_word_begin() == 6)
{
	print("Cursor: move_to_word_begin true");
}
else
{
	print("Cursor: move_to_word_begin false");
}

if(cursor_m.move_to_word_end() == 10)
{
	print("Cursor: move_to_word_end true");
}
else
{
	print("Cursor: move_to_word_end false");
}

if(text_c.get_line(cursor_c.goto_line(21)) == "voluptas nulla pariatur?\n")
{
	print("Cursor: goto_line true");
}
else
{
	print("Cursor: goto_line false");
}

if(text_c.get_line(cursor_c.prev_line(2)) == "consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit \n")
{
	print("Cursor: prev_line true");
}
else
{
	print("Cursor: prev_line false");
}

cursor_c.move_to_line_begin();
if(text_c.get(cursor_c.goto_col(5), 6) == "quatur")
{
	print("Cursor: goto_col true");
}
else
{
	print("Cursor: goto_col false");
}

cursor_c = 0;
cursor_c.move_to_line_begin();
if( cursor_c == 0)
{
	print("Cursor: move_to_line_begin true");
}
else
{
	print("Cursor: move_to_line_begin false");
}

cursor_c.goto_line(2);
cursor_c.move_to_line_begin();
if( text_c.get_line(cursor_c) == "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis \n")
{
	print("Cursor: move_to_line_begin true");
}
else
{
	print("Cursor: move_to_line_begin false");
}

cursor_c = 0;
cursor_c.move_to_line_end();
if( cursor_c == 79)
{
	print("Cursor: move_to_line_end true");
}
else
{
	print("Cursor: move_to_line_end false");
}

cursor_c = 0;
cursor_c.goto_line(24);
cursor_c.move_to_line_end();
if( cursor_c == 1451)
{
	print("Cursor: move_to_line_end true");
}
else
{
	print("Cursor: move_to_line_end false");
}