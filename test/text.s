print("----------------Text---------------------------");
var text_t = Text();
text_t.load("txt.txt");
var cursor_t  = Cursor(text_t);

cursor_t = 0;
if(text_t.get(cursor_t, 5) == "Lorem")
{
	print("Text: get true");
}
else
{
	print("Text: get false");
}

cursor_t.end();
cursor_t -= 1;
if(text_t.get(cursor_t, 2) == "03")
{
	print("Text: get true");
}
else
{
	print("Text: get false");
}

if(text_t.get_to_endl(cursor_t) == "03")
{
	print("Text: get_to_endl true");
}
else
{
	print("Text: get_to_endl false");
}

cursor_t = 0
if(text_t.get_to_endl(cursor_t) == "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor \n")
{
	print("Text: get_to_endl true");
}
else
{
	print("Text: get_to_endl false");
}

cursor_t.end() -= 1;
if(text_t.get_line(cursor_t) == "+3 3.2e23 -4.70e+9 -.2E-4 -7.6603")
{
	print("Text: get_line true");
}
else
{
	print("Text: get_line false");
}

cursor_t = 34;
if(text_t.get_word(cursor_t) == "consectetur")
{
	print("Text: get_word true");
}
else
{
	print("Text: get_word false");
}

cursor_t.end();
cursor_t -= 3;
if(text_t.get_word(cursor_t) == "6603")
{
	print("Text: get_word true");
}
else
{
	print("Text: get_word false");
}

cursor_t = 0;
cursor_t.move_to("{laudantium", find);
var cursor_t_1 = Cursor(cursor_t);
cursor_t_1.move_to("{", "}", find);
if(text_t.get_between(cursor_t, cursor_t_1) == "{laudantium, totam rem }")
{
	print("Text: get_between true");
}
else
{
	print("Text: get_between false");
}

text_t.set(cursor_t, "!laudantium, totam rem !");
if(text_t.get_line(cursor_t) == "{doloremque !laudantium, totam rem !aperiam, eaque ipsa quae ab illo inventore \n")
{
	print("Text: set true");
}
else
{
	print("Text: set false");
}

text_t.set_line(cursor_t, "consequuntur Neque");
if(text_t.get_line(cursor_t) == "consequuntur Neque\n")
{
	print("Text: set_line true");
}
else
{
	print("Text: set_line false");
}

cursor_t = 0;
text_t.insert(cursor_t, "is ")
if( text_t.get_line(cursor_t) == "is Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor \n")
{
	print("Text: insert true");
}
else
{
	print("Text: insert false");
}

cursor_t.end();
text_t.insert(cursor_t, " is")
if( text_t.get_line(cursor_t) == "+3 3.2e23 -4.70e+9 -.2E-4 -7.660 is3")
{
	print("Text: insert true");
}
else
{
	print("Text: insert false");
}

text_t.add(" Quis autem");
if( text_t.get_line(cursor_t) == "+3 3.2e23 -4.70e+9 -.2E-4 -7.660 is3 Quis autem")
{
	print("Text: add true");
}
else
{
	print("Text: add false");
}

cursor_t = 0;
text_t = "consequatur? Quis autem vel eum \nesse quam nihil molestiae";
text_t.erase(cursor_t, 12);
if(text_t.get_line(cursor_t) == " Quis autem vel eum \nesse quam nihil molestiae")
{
	print("Text: erase true");
}
else
{
	print("Text: erase true");
}

cursor_t_1 = 5;
text_t.erase_between(cursor_t, cursor_t_1);
if(text_t.get_line(cursor_t) == " autem vel eum \nesse quam nihil molestiae")
{
	print("Text: erase_between true");
}
else
{
	print("Text: erase_between true");
}

text_t.erase_line(cursor_t);
if(text_t.get_line(cursor_t) == "esse quam nihil molestiae")
{
	print("Text: erase_line true");
}
else
{
	print("Text: erase_line true");
}

var txt1 = Text("Lorem ipsum dolor sit amet\nconsectetur adipiscing elit, sed do eiusmod tempor");
var txt2 = Text();
txt2 = txt1;

var cur1 = Cursor(txt1);
var cur2 = Cursor(txt2);

cur1.move_to_end("\n", find);
txt1.insert_line(cur1, "nostrud exercitation");
cur1 = 0;
if(txt1.get(cur1, txt1.size()) == "Lorem ipsum dolor sit amet\nostrud exercitation\nnconsectetur adipiscing elit, sed do eiusmod tempor")
{
	print("Text: insert_line true");
}
else
{
	print("Text: insert_line true");
}

//var str = "";
//cur1 = txt1.diff(cur1, txt2, str);
//print(str);