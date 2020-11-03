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
//
var NEXT_RESOURCE_VALUE = 101;
var NEXT_RESOURCE_VALUE_MAX = 0x6FFF;

var NEXT_COMMAND_VALUE = 40001;
var NEXT_COMMAND_VALUE_MAX = 0xDFFF;

var NEXT_CONTROL_VALUE = 1000;
var NEXT_CONTROL_VALUE_MAX = 0xDFFF;

var NEXT_SYMED_VALUE = 101;

//
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

	cursor = 0;
	var q = Query(cursor);

	while(cursor.next_line())
	{
		if(SkipSpace(q, true) && Match(q, "#define") && Is(q, iscsym, "ID") && Number(q, "ID_NUM"))
                {
			IDs[q.get("ID")] = ResItem(q.get("ID"),	to_int(q.get("ID_NUM")),++line);
		}
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

def to_map(query, IDs, type)
{
	print("ID : "  + query.get("ID"));
	if(IDs.count(query.get("ID")) > 0)
	{
		IDs[query.get("ID")].type = type;
	}
	return true;
}
//////////////////////////////////////////////////
def load_rc(cursor, IDs)
{
	var dlg_ctrl1 = ["AUTO3STATE","AUTOCHECKBOX","AUTORADIOBUTTON","CHECKBOX","CONTROL","CTEXT","DEFPUSHBUTTON","GROUPBOX","ICON","LTEXT","PUSHBOX","PUSHBUTTON","RADIOBUTTON","RTEXT","STATE3","CONTROL"];
	var dlg_ctrl2 = ["COMBOBOX","EDITTEXT","LISTBOX","SCROLLBAR"];

	cursor.begin();
	var q = Query(cursor);
	SkipSpace(q, true);

	while(cursor)
	{
		var found = false;
		//BITMAP
		found = cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "BITMAP") && Match(q, "\"") && to_map(q, IDs, BITMAP);
		//ICON
		found = found == false && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "ICON") && Match(q, "\"") && to_map(q, IDs, ICON);
		//CURSOR
		found = found == false && cursor.move_to_line_begin() && Is(q, iscsym,-1, "ID") && Match(q, "CURSOR") && Match(q, "\"") && to_map(q, IDs, CURSOR);
		//MENU
		found = found == false && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "MENU") && to_map(q, IDs, MENU);
		//MENUITEM
		found = found == false && cursor.move_to_line_begin() && Match(q, "MENUITEM") && AnyOf(q, "\"") && AnyNot(q, "\"") && Match(q, ",") && Is(q, iscsym, "ID") && to_map(q, IDs, MENUITEM);
		//TOOLBAR
		found = found == false && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "TOOLBAR") && to_map(q, IDs, TOOLBAR);
		//TOOLBAR BUTTON
		found = found == false && cursor.move_to_line_begin() && Match(q, "BUTTON") && Is(q, iscsym, "ID") && to_map(q, IDs, BUTTON);
		//DIALOG
		found = found == false && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "DIALOGEX") && to_map(q, IDs, DIALOG);
		//CONTROL
		found = found == false && (cursor.move_to_line_begin() && Set(q, dlg_ctrl1) && cursor.move_to_end("\",", find) && Is(q, iscsym, "ID") ||
		                      cursor.move_to_line_begin() && Set(q, dlg_ctrl2) && Is(q, iscsym, "ID")) && to_map(q, IDs, CONTROL);
		//STRING
		found = found == false && cursor.move_to_line_begin() && Match(q, "IDS_") && Word(q, "ID") && to_map(q, IDs, STRING);
		
		cursor.next_line();
	}
	//trace(false);
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
print("start");

var rc_path = get_options_value("-f");

var h_fn = find_file(rc_path, "resource.h");
exit_if(exists(h_fn) == false, rc_path + " " + "resource.h file not found");
var header = Text();
header.load(h_fn);
var h_cursor = Cursor(header);

var rc_fn = find_file(rc_path, "*.rc");
exit_if(exists(rc_fn) == false, rc_path + " " + "rc file not found");
var rc = Text();
rc.load(rc_fn);
var rc_cursor = Cursor(rc);

print("load h file");
var IDs = load_ids(h_cursor);

print("load rc file");
load_rc(rc_cursor, IDs);
//trace(true);
h_cursor.begin();
(h_cursor.move_to("_APS_NEXT_RESOURCE_VALUE", find))
{
	NEXT_RESOURCE_VALUE = to_int(header.get_word(h_cursor.next_word()));
}

h_cursor.begin();
if(h_cursor.move_to("_APS_NEXT_COMMAND_VALUE", find))
{
	NEXT_COMMAND_VALUE = to_int(header.get_word(h_cursor.next_word()));
}

h_cursor.begin();
if(h_cursor.move_to("_APS_NEXT_CONTROL_VALUE", find))
{
	NEXT_CONTROL_VALUE = to_int(header.get_word(h_cursor.next_word()));
}

h_cursor.begin();
if(h_cursor.move_to("_APS_NEXT_SYMED_VALUE", find))
{
	NEXT_SYMED_VALUE = to_int(header.get_word(h_cursor.next_word()));
}


print("end.");
