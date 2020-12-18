//txtE -e rc_check.s  -f <path to resource.h and *.rc dir>
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
var RESOURCE_GROUP =[DIALOG, MENU];

var NEXT_COMMAND_VALUE = 40001;
var NEXT_COMMAND_VALUE_MAX = 0xDFFF;
var COMMAND_GROUP =[];

var NEXT_CONTROL_VALUE = 1000;
var NEXT_CONTROL_VALUE_MAX = 0xDFFF;
var CONTROL_GROUP =[CONTROL];

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
			var d = ResItem(q.get("ID"), to_int(q.get("ID_NUM")), ++line);
			IDs[q.get("ID")] = d;
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
	if(IDs.count(query.get("ID")) > 0)
	{
		//print("ID found : "  + query.get("ID"));
		IDs[query.get("ID")].type = type;
		return true;
	}
	//print("ID not found : "  + query.get("ID"));
	return true;
}
//////////////////////////////////////////////////
def load_rc(cursor, IDs, rc)
{
	var dlg_ctrl1 = ["AUTO3STATE","AUTOCHECKBOX","AUTORADIOBUTTON","CHECKBOX","CONTROL","CTEXT","DEFPUSHBUTTON","GROUPBOX","ICON","LTEXT","PUSHBOX","PUSHBUTTON","RADIOBUTTON","RTEXT","STATE3","CONTROL"];
	var dlg_ctrl2 = ["COMBOBOX","EDITTEXT","LISTBOX","SCROLLBAR"];

	cursor.begin();
	var q = Query(cursor);
	SkipSpace(q, true);

	var found = false;
	//trace(true);
	while(cursor)
	{
		if(rc.get_line_size(cursor) > 16)//filter lines
		{
			//"found ||=" gives an error
			//BITMAP
			found = cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "BITMAP") && Match(q, "\"") && to_map(q, IDs, BITMAP) ;
			//ICON
			found = !found && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "ICON") && Match(q, "\"") && to_map(q, IDs, ICON) || found;
			//CURSOR
			found = !found && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "CURSOR") && Match(q, "\"") && to_map(q, IDs, CURSOR) || found;
			//MENU
			found = !found && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "MENU") && to_map(q, IDs, MENU) || found;
			//MENUITEM
			found = !found && cursor.move_to_line_begin() && Match(q, "MENUITEM") && CStr(q) && Match(q, ",") && Is(q, iscsym, "ID") && to_map(q, IDs, MENUITEM) || found;
			//TOOLBAR
			found = !found && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "TOOLBAR") && to_map(q, IDs, TOOLBAR) || found;
			//TOOLBAR BUTTON
			found = !found && cursor.move_to_line_begin() && Match(q, "BUTTON") && Is(q, iscsym, "ID") && to_map(q, IDs, BUTTON) || found;
			//DIALOG
			found = !found && cursor.move_to_line_begin() && Is(q, iscsym, "ID") && Match(q, "DIALOGEX") && to_map(q, IDs, DIALOG) || found;
			//CONTROL
			found = !found && cursor.move_to_line_begin() && (Set(q, dlg_ctrl1) && cursor.move_to_end("\",", find) && Is(q, iscsym, "ID")) ||
		                      (cursor.move_to_line_begin() && Set(q, dlg_ctrl2) && Is(q, iscsym, "ID") && to_map(q, IDs, CONTROL))  || found;
		//STRING
			!found && cursor.move_to_line_begin() && Match(q, "IDS_") && Word(q, "ID") && to_map(q, IDs, STRING);
		}
		found = false;
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
def find_item_by_val(IDs, id, val)
{
	for(i : IDs)
	{
		if(i.second().val == val && i.second().id != id)
		{
			return i.second();
		}
	}

	return ResItem()//empty;
}

def check_header(IDs)
{
	var printed = false;
	var dup;
	for(i : IDs)
	{
		dup = find_item_by_val(IDs, i.second().id, i.second().val);

		//if(dup.is_var_undef() == false)
		if(dup.id != "" && dup.type == i.second().type)
		{
			if(printed == false)
			{
			 	print("Duplicated items found");
				printed = true;
			}
			print("\t" + i.second().id + " / " + dup.id + " value " + to_string(i.second().val));
		}
	}
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
load_rc(rc_cursor, IDs, rc);

h_cursor.begin() && h_cursor.move_to("_APS_NEXT_RESOURCE_VALUE", find) && to_true(NEXT_RESOURCE_VALUE = to_int(header.get_word(h_cursor.next_word())));
h_cursor.begin() && h_cursor.move_to("_APS_NEXT_COMMAND_VALUE", find) && to_true(NEXT_COMMAND_VALUE = to_int(header.get_word(h_cursor.next_word())));
h_cursor.begin() && h_cursor.move_to("_APS_NEXT_CONTROL_VALUE", find) && to_true(NEXT_CONTROL_VALUE = to_int(header.get_word(h_cursor.next_word())));
h_cursor.begin() && h_cursor.move_to("_APS_NEXT_SYMED_VALUE", find) && to_true(NEXT_SYMED_VALUE = to_int(header.get_word(h_cursor.next_word()))) ;

check_header(IDs);

print("end.");
