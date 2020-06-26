var str = " Abcd 123z4a  5678-44?\n";
var cursor = Cursor(str);
var ret = "";

if(Query(cursor) && Is(isspace, -1) && IsExact("Abcd 123z4a") && Is(isspace, -1) && Is(isdigit, 4, ret) && IsExact("-"))
{
	print("true \"" + ret +"\"");
}
else
{
	print("false");
}

ret = "";
cursor.begin();
if(Query(cursor) && IsAny("18", 2, ret))
{ 
	print("true \"" + ret +"\"");
}
else
{
	print("false");
}
