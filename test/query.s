var str = " Abcd 123z4a  5678-44?\n";
var cursor = Cursor(str);
var ret = "";

if(Query(cursor) && Is(isspace, -1) && Exact("Abcd 123z4a") && Is(isspace, -1) && Is(isdigit, 4, ret) && Exact("-"))
{
	print("true \"" + ret +"\"");
}
else
{
	print("false");
}

cursor.begin();
if(Query(cursor) && cursor.move_to_end(" Abcd ", find)  && Is(isdigit, 3, ret))
{
	print("Is: true \"" + ret +"\"");
}
else
{
	print("Is : false");
}

ret = "";
cursor.begin();

if(Query(cursor) && Any("A ", 2, ret))
{ 
	print("Any:true \"" + ret +"\"");
}
else
{
	print("Any:false");
}

ret = "";
cursor.begin();

if(Query(cursor) && cursor.move_to_end("Abcd ", find) && Range('0', '9', 3, ret))
{ 
	print("Range:true \"" + ret +"\"");
}
else
{
	print("Range:false");
}
