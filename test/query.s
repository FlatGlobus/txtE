print("----------------Query---------------------------");

var text_q = Text(" Abcd 123z4a  5678-44?");
var cursor_q = Cursor(text_q);
var q = Query(cursor_q);

cursor_q = 1;
if(Match(q, "Abcd"))
{
	print("Query: Match true");
}
else
{
	print("Query: Match false");
}

cursor_q = 1;
if(Match(q, "bcd") == false)
{
	print("Query: Match true");
}
else
{
	print("Query: Match false");
}

cursor_q = 1;
if(AnyOf(q, "v") == false)
{
	print("Query: AnyOf true");
}
else
{
	print("Query: AnyOf false");
}

cursor_q = 1;
if(AnyOf(q, "dcbA"))
{
	print("Query: AnyOf true");
}
else
{
	print("Query: AnyOf false");
}

cursor_q = 1;
if(AnyOf(q, "dcbA", 3, "ret"))
{
	print("Query: AnyOf true " + q.get("ret"));
}
else
{
	print("Query: AnyOf false " + q.get("ret"));
}

cursor_q = 1;
if(AnyOf(q, "dcbA", "ret"))
{
	print("Query: AnyOf true " + q.get("ret") );
}
else
{
	print("Query: AnyOf false " + q.get("ret"));
}

cursor_q = 0;
if(Is(q, isspace))
{
	print("Query: Is isspace true");
}
else
{
	print("Query: Is isspace false");
}

cursor_q = 0;
if(Is(q, isspace, 1))
{
	print("Query: Is isspace true");
}
else
{
	print("Query: Is isspace false");
}

cursor_q = 0;
if(Is(q, isspace, 3) == false)
{
	print("Query: Is isspace true");
}
else
{
	print("Query: Is isspace false");
}

cursor_q = 3;
if(Is(q, isspace, 0))
{
	print("Query: Is isspace true");
}
else
{
	print("Query: Is isspace false");
}

cursor_q =	2;
if(Range(q, 'a', 'z'))
{
	print("Query: Range true");
}
else
{
	print("Query: Range false");
}

cursor_q =	2;
if(Range(q, 'a', 'z', -1, "ret"))
{
	print("Query: Range true " + q.get("ret"));
}
else
{
	print("Query: Range false" + q.get("ret"));
}

cursor_q =	2;
if(Range(q, '1', '9') == false)
{
	print("Query: Range true");
}
else
{
	print("Query: Range false");
}

cursor_q =	1;
if(Set(q, ["fgtr","Abcd"], "ret"))
{
	print("Query: Set true  " + q.get("ret"));
}
else
{
	print("Query: Set false");
}

cursor_q =	1;
if(Set(q, ["fgtr","12243"]) == false)
{
	print("Query: Set true");
}
else
{
	print("Query: Set false");
}

cursor_q =	1;
if(Word(q, "ret"))
{
	print("Query: Word true " + q.get("ret"));
}
else
{
	print("Query: Word false");
}

//cursor_q =	6;
//if(Number(q, "ret"))
//{
//	print("Query: Number1 true " + q.get("ret"));
//}
//else
//{
//	print("Query: Number1 false");
//}

if(Number(q) == false)
{
	print("Query: Number2 true");
}
else
{
	print("Query: Number2 false");
}

cursor_q = 0;
if(Space(q))
{
	print("Query: Space true");
}
else
{
	print("Query: Space false");
}

var txt_date = Text("01/Jan/2000 01/01/2000 2008-09-31"); 
var cur_date = Cursor(txt_date);
var q_date = Query(cur_date);

if(Date(q_date) && Space(q_date) && Date(q_date) && Space(q_date) && Date(q_date))
{
	print("Query: Date true");
}
else
{
	print("Query: Date false");
}

txt_date = " 12:08 22:08 time to go"; 
cur_date = 1;
if(Time(q_date) && Space(q_date) && Time(q_date, "ret"))
{
	print("Query: Time true " + q.get("ret") );
}
else
{
	print("Query: Time false");
}

txt_date = "192.88.100.0 0.0.0.0";
cur_date=0;
if(IPv4(q_date) && Space(q_date) && IPv4(q_date))
{
	print("Query: IPv4 true");
}
else
{
	print("Query: IPv4 false");
}

txt_date = "1200:0000:AB00:1234:0000:2552:7777:1313 1200:0000:AB00:1234:O000:2552:7777:1313";
cur_date=0;
if(IPv6(q_date) && Space(q_date) && IPv6(q_date) == false)
{
	print("Query: IPv6 true");
}
else
{
	print("Query: IPv6 false");
}

txt_date = "<h2 class=\"offscreen\">Webontwikkeling leren</h2>";
cur_date=0;
/////////////////////////////////////////////////////////////////////////

cursor_q = 0; //" Abcd 123z4a  5678-44?"
if(Is(q, isspace) && Match(q, "Abcd 123z4a") && Is(q, isspace, 2) && Is(q, isdigit, 4, "ret") && Match(q, "-"))
{
	if(q.get("ret") == "5678")
	{
		print("Query: query 1 true " + q.get("ret"));
	}
	else
	{
		print("Query: query 1 false" + q.get("ret"));
	}
}
else
{
	print("Query: query 1 false");
}

cursor_q = 0; //" Abcd 123z4a  5678-44?"
if(Space(q) && Word(q) && Space(q) && Word(q, "ret") && Space(q) && Match(q, "5678-44?"))
{
	if(q.get("ret") == "123z4a")
	{
		print("Query: query 2 ret true " + q.get("ret"));
	}
	else
	{
		print("Query: query 2 ret false" + q.get("ret"));
	}
}
else
{
	print("Query: query 1 false");
}

var txt_q = Text();
txt_q.load("txt.txt");
var txt_cur_q = Cursor(txt_q);

print("print 5 words");
txt_cur_q = 0;
var qq = Query(txt_cur_q);
for(var i = 0; i < 5; ++i)
{
	if(Word(qq, "ret") && Space(qq))
	{
		print(qq.get("ret"));
	}
}

txt_cur_q = 0;
qq.reset();
if( Group(qq, -1, true, [Word(qq, "word"), Space(qq), NextData(qq)]))
{
	print("Group word:true "+ "size = " + to_string(qq.size()) + " words = " + to_string(qq.get_vector("word")));
}

txt_cur_q = 0;
var timer = Timer();
timer.start();
qq.reset();
if( Group(qq, -1, true, [Number(qq, "num"), NextData(qq)]) )
{
	print("Group num:true "+ "size = " + to_string(qq.size()) + " numbers = " + to_string(qq.get_vector("num")));
}
timer.stop();
print("time : " + to_string(timer.time()));
