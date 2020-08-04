//
//enum 
global UNKNOWN = -1;
global BITMAP = 1;
global ICON = 2;
global MENU = 3;
global MENUITEM = 4;
global DIALOG = 5;
global CONTROL = 6;
global STRING = 7;
global CURSOR = 8;
global TOOLBAR = 9;
global BUTTON = 10;

class ResItem
{
	var id;
	var val;
	var line;
	var type; 

	def ResItem()
	{
		this.id = "";
		this.val = -1;
		this.line = -1;
		this.type = UNKNOWN;
	}

	def ResItem(_id, _val, _line)
	{
		this.id = _id;
		this.val = _val;
		this.line = _line;
		this.type = UNKNOWN;
	}
}
////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////
def make_rc_id_string(rc_item)
{
	var count = 40 - (8 + rc_item.second.id.size());
	if(count <=0)
	{
		count = 1;
	}

	return "#define "+ rc_item.second.id + replicate(count, " ") + to_string(rc_item.second.val);
}
//////////////////////////////////////////////////
def load_rc(cursor, IDs)
{
	//type text id
	var dlg_ctrl1 = ["AUTO3STATE","AUTOCHECKBOX","AUTORADIOBUTTON","CHECKBOX","CONTROL","CTEXT","DEFPUSHBUTTON","GROUPBOX","ICON","LTEXT","PUSHBOX","PUSHBUTTON","RADIOBUTTON","RTEXT","STATE3","CONTROL"];
	//type id
	var dlg_ctrl2 = ["COMBOBOX","EDITTEXT","LISTBOX","SCROLLBAR"];

	cursor.begin();
	while(cursor)
	{
		var ID = "";
		var found = false;
		//BITMAP
		if(Query(cursor.move_to_begin_of_line()) && Is(iscsym,-1, ID) && Is(isspace) && Exact("BITMAP") && Is(isspace) && Exact("\""))
		{
			print("BITMAP : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = BITMAP;
			}
			found = true;
		}
		//ICON
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(iscsym,-1, ID) && Is(isspace) && Exact("ICON") && Is(isspace) && Exact("\""))
		{
			print("ICON : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = ICON;
			}
			found = true;
		}
		//CURSOR
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(iscsym,-1, ID) && Is(isspace) && Exact("CURSOR") && Is(isspace) && Exact("\""))
		{
			print("CURSOR : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = CURSOR;
			}
			found = true;
		}
		//MENU
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(iscsym,-1, ID) && Is(isspace) && Exact("MENU"))
		{
			print("MENU : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = MENU;
			}
			found = true;
		}
		//MENUITEM
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(isspace) && Exact("MENUITEM") && cursor.move_to_end("\",", find) && Is(isspace) && Is(iscsym,-1, ID))
		{
			print("MENUITEM : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = MENUITEM;
			}
			found = true;
		}
		//TOOLBAR
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(iscsym,-1, ID) && Is(isspace) && Exact("TOOLBAR"))
		{
			print("TOOLBAR : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = TOOLBAR;
			}
			found = true;
		}
		//TOOLBAR BUTTON
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(isspace) && Exact("BUTTON") && Is(isspace) && Is(iscsym,-1, ID))
		{
			print("BUTTON : " + ID);			
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = BUTTON;
			}
		}
		//DIALOG
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(iscsym,-1, ID) && Is(isspace) && Exact("DIALOGEX"))
		{
			print("DIALOG : " + ID);
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = DIALOG;
			}
			found = true;
		}
		//CONTROL
		if(found == false && (Query(cursor.move_to_begin_of_line()) && Is(isspace) && Set(dlg_ctrl1) && cursor.move_to_end("\",", find) && Is(iscsym,-1, ID) ||
		                      Query(cursor.move_to_begin_of_line()) && Is(isspace) && Set(dlg_ctrl2) && Is(isspace) && Is(iscsym,-1, ID)))
		{
			print("CONTROL : " + ID);	
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = CONTROL;
			}
			found = true;
		}
		//STRING
		if(found == false && Query(cursor.move_to_begin_of_line()) && Is(isspace) && Exact("IDS_") && Is(iscsym,-1, ID) && Is(isspace))
		{
			ID = "IDS_"+ID;
			print("STRING : " + ID);			
			if(IDs.count(ID) > 0)
			{
				IDs[ID].type = STRING;
			}
		}
		cursor.next_line();
	}
}
//////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////
def find_item_by_val(IDs, val)
{
	for(i : IDs)
	{
		if(i.second.val == val)
		{
			return i.id;
		}
	}

	return "";
}
//////////////////////////////////////////////////////
//
//var NEXT_RESOURCE_VALUE = 101;
//var NEXT_COMMAND_VALUE = 40001;
//var NEXT_CONTROL_VALUE = 1000;
//var NEXT_SYMED_VALUE = 101;
//
print("start");
if(program_options.size() == 0)
{
	print("Error: no command line");
	return;
}
//from right to left

var left_path = get_options_value("-l");
var right_path = get_options_value("-r");

var left_h_fn = find_file(left_path, "resource.h");
exit_if(exists(left_h_fn) == false, left_path + " " + "resource.h not found");
var left_h = Text();
left_h.load(left_h_fn);
var left_h_cursor = Cursor(left_h);

var right_h_fn = find_file(right_path, "resource.h");
exit_if(exists(right_h_fn) == false, right_path + " " + "resource.h not found");
var right_h = Text();
right_h.load(right_h_fn);
var right_h_cursor = Cursor(right_h);

var left_rc_fn = find_file(left_path, "*.rc");
exit_if(exists(left_rc_fn) == false, left_path + " " + "rc file not found");
var left_rc = Text();
left_rc.load(left_rc_fn);
var left_rc_cursor = Cursor(left_rc);

var right_rc_fn = find_file(right_path, "*.rc");
exit_if(exists(right_rc_fn) == false, right_path + " " + "rc file not found");
var right_rc = Text();
right_rc.load(right_rc_fn);
var right_rc_cursor = Cursor(right_rc);

print("load h files");
var left_IDs = load_ids(left_h_cursor);
var right_IDs = load_ids(right_h_cursor);

print("load rc files");
load_rc(left_rc_cursor, left_IDs);
load_rc(right_rc_cursor, right_IDs);

for(item : left_IDs)
{
	if(right_IDs.count(item.first) > 0)
	{
		right_IDs.erase(item.first);
	}
}

//merge h
for(item : right_IDs)
{
	left_h.insert_line(left_h_cursor.goto_line(item.second.line), make_rc_id_string(item));
}
//merge rc


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
left_h.write(left_h_fn + ".new");
print("end.");
