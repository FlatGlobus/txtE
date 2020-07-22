//
class ResItem
{
	var id;
	var val;
	var line;

	def ResItem()
	{
		this.id = "";
		this.val = -1;
		this.line = -1;
	}

	def ResItem(_id, _val, _line)
	{
		this.id = _id;
		this.val = _val;
		this.line = _line;
	}
}

def load_ids(cursor)
{
	var IDs = Map();
	var line = 0;

	cursor.begin();
	while(cursor)
	{
		var ID = "";
		var ID_NUM = "";

		cursor.label("line");
		if(Query(cursor) && Exact("#define") && Is(isspace) && Is(iscsym,-1, ID) && Is(isspace) && Is(isdigit,-1, ID_NUM))
		{
			IDs[ID] = ResItem(ID, to_int(ID_NUM), line);
		}
		cursor.goto_label("line");
		cursor.next_line();
		++line;
	}
	return IDs;
}
//
//var NEXT_RESOURCE_VALUE = 101;
//var NEXT_COMMAND_VALUE = 40001;
//var NEXT_CONTROL_VALUE = 1000;
//var NEXT_SYMED_VALUE = 101;
//
if(program_options.size() == 0)
{
	print("Error: no command line options");
	return;
}
//from right to left
var left_path = get_options_value("-l");
var right_path = get_options_value("-r");

var left_fn = find_file(left_path, "resource.h");
exit_if(exists(left_fn) == false, left_path + " " + "resource.h not found");

var right_fn = find_file(right_path, "resource.h");
exit_if(exists(right_fn) == false, right_path + " " + "resource.h not found");

var left_rc_h = Text();
left_rc_h.load(left_fn);
var left_cursor = Cursor(left_rc_h);

var right_rc_h = Text();
right_rc_h.load(right_fn);
var right_cursor = Cursor(right_rc_h);

var left_IDs = load_ids(left_cursor);
var right_IDs = load_ids(right_cursor);

for(item : left_IDs)
{
	if(right_IDs.count(item.first) > 0)
	{
		right_IDs.erase(item.first);
	}
}

def find_item_line(IDs, line_num)
{
	var num = 0;
	for(i : IDs)
	{
		if(i.second.line == line_num)
		{
			break;
		}
		++num;
	}

	return num;
}

for(item : right_IDs)
{
	left_rc_h.insert_line(left_cursor.goto_line(item.second.line), "#define " + item.second.id + " " + to_string(item.second.val));
}


/*left_cursor.begin();
if(left_cursor.move_to("_APS_NEXT_RESOURCE_VALUE", find))
{
	NEXT_RESOURCE_VALUE = to_int(left_rc_h.get_word(left_cursor.next_word()));
}

left_cursor.begin();
if(left_cursor.move_to("_APS_NEXT_COMMAND_VALUE", find))
{
	NEXT_COMMAND_VALUE = to_int(left_rc_h.get_word(left_cursor.next_word()));
}

left_cursor.begin();
if(left_cursor.move_to("_APS_NEXT_CONTROL_VALUE", find))
{
	NEXT_CONTROL_VALUE = to_int(left_rc_h.get_word(left_cursor.next_word()));
}

left_cursor.begin();
if(left_cursor.move_to("_APS_NEXT_SYMED_VALUE", find))
{
	NEXT_SYMED_VALUE = to_int(left_rc_h.get_word(left_cursor.next_word()));
}
*/
left_rc_h.write(left_fn + ".new");
