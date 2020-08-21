print("----------------Query---------------------------");

var str_txt = Text(" Abcd 123z4a  5678-44?");
var cursor = Cursor(str_txt);
var ret = "";

if(Query(cursor) && Is(isspace, -1) && Exact("Abcd 123z4a") && Is(isspace, -1) && Is(isdigit, 4, ret) && Exact("-"))
{
	print("true \"" + ret +"\"");
}
else
{
	print("false");
}

cursor = 0;
if(Query(cursor) && cursor.move_to_end(" Abcd ", find)  && Is(isdigit, 3, ret))
{
	print("Is: true \"" + ret +"\"");
}
else
{
	print("Is : false");
}

cursor.begin();
if(Query(cursor) && Any("A ", 2, ret))
{ 
	print("Any:true \"" + ret +"\"");
}
else
{
	print("Any:false");
}

if(Query(cursor) && Any( 5, ret))
{ 
	print("Any:true \"" + ret +"\"");
}
else
{
	print("Any:false");
}

cursor.begin();
if(Query(cursor) && cursor.move_to_end("Abcd ", find) && Range('0', '9', 3, ret))
{ 
	print("Range:true \"" + ret +"\"");
}
else
{
	print("Range:false");
}

var txt = Text();
txt.load("txt.txt");
var txt_cur = Cursor(txt);

if(Query(txt_cur) && Word(ret))
{
	print("Word:true \"" + ret +"\"");	
}
else
{
	print("Word:false");
}

if(Query(txt_cur) && Space())
{
	print("Space:true ");	
}
else
{
	print("Space:false");
}

print("print 5 words");
txt_cur = 0;
Query(txt_cur);
for(var i =0; i < 5; ++i)
{
	if(Word(ret) && Space())
	{
		print(ret);
	}
}

if(Starts("Section") && Space() && Number(ret))
{
	print("Starts:true \"" + ret + "\"");
}
else
{
	print("Starts:false");
}

txt_cur = 0;
var w = [];

if( Group(-1, [Word(w), Space()]) )
{
	print("Group:true "+ "size = " + to_string(w.size()) + " words = " + to_string(w));
}

txt_cur = 0;
var n = [];
if( Group(-1, true, [Number(n)]) )
{
	print("Group:true "+ "size = " + to_string(n.size()) + " numbers = " + to_string(n));
}
