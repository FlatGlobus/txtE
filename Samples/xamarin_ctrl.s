//declare property //[prop name type def]  //[prop BackgroundColor Color Color.White]
//====================================================
def gen_property(class_name, prop_name, prop_type, def_val)
{
	var str = "\tpublic static readonly BindableProperty " + prop_name + "Property = \n";
        str += "\t\tBindableProperty.Create(nameof(" + prop_name+"), typeof(" + prop_type + "), typeof(" + class_name + "), " + def_val + ", propertyChanged: OnPropertyChanged);\n";
	str += "\tpublic " + prop_type + " " + prop_name + "\n";
	str += "\t{\n";
	str += "\t\tget => (" + prop_type +")GetValue(" + prop_name + "Property);\n";
  	str += "\t\tset => SetValue(" + prop_name + "Property, value);\n\t}\n";

        return str;
}

if(program_options.size() == 0)
{
	print("Error: no command line options");
	return;
}

var source_file_name = get_options_value("-f");
print("start file : " + source_file_name);

var source = Text();
if(source.load(source_file_name) == 0)
{
	print(ERROR + "File " + source_file_name + " is empty.");
	return;
}

var cursor = Cursor(source);
var query = Query(cursor);
var class_name = "";
var changed = false;

while(cursor)
{
	if(SkipSpace(query, true) && Match(query, "public") && Match(query, "class") && Is(query, iscsym, -1, class_name) && Match(query, ":") && Match(query, "SKCanvasView"))
	{
		print("found class : " + class_name);
		cursor.label("class_decl");
		break;
	} 
	class_name = "";
	cursor.next_line();
}

cursor = 0;
while(cursor)
{
	var prop_name = "";
	var prop_type = "";
	var def_val = "";

	cursor.label("line");
	if(SkipSpace(query, true) && Match(query,"//[prop") && Is(query, iscsym, -1, prop_name) && Is(query, iscsym, -1, prop_type) && AnyNot(query, " ]", -1, def_val) && Match(query,"]"))
	{
		print("property : " + prop_name);
		if(source.contains("BindableProperty " + prop_name + "Property") == false)
		{		
			print("generate property : " + prop_name);
			source.insert_line(cursor, gen_property(class_name, prop_name, prop_type, def_val));
			changed = true;
		}
		else
		{
			cursor.goto_label("line");
		}

	}
	else
	{
		cursor.goto_label("line");
	}
	cursor.next_line();
}

if(source.contains("OnPropertyChanged(BindableObject") == false && cursor.goto_label("class_decl"))
{
	var str	= "\n\tprivate static void OnPropertyChanged(BindableObject bindable, object oldVal, object newVal)\n";
	str += "\t{\n";
	str += "\t\tvar bar = bindable as GradientBar;\n";
	str += "\t\tbar?.InvalidateSurface();\n";
	str += "\t}";

	if(cursor.move_to("{", "}", find))
	{
		cursor.prev_line();
		source.insert_line(cursor, str);
		changed = true;
	}
}

if(changed == true)
{
	source.write(source_file_name);
}
print("end");