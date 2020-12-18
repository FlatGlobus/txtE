//txtE -e xamarin_ctrl.s -f <filename.cs>
//declare property //[prop name type def]  //[prop BackgroundColor Color Color.White]
//====================================================

var source = Text();
source.load(get_options_value("-f"));

var cursor = Cursor(source);
var query = Query(cursor);

//starts from first line, class declaration can't be here
while(cursor.next_line())
{
	if(SkipSpace(query, true) && ZeroOne(query, Match(query, "public")) && ZeroOne(query, Match(query, "partial")) && Match(query, "class") && Is(query, iscsym, "class_name") && Match(query, ":") && Is(query, iscsym))
	{
		print("found class : " + query.get("class_name"));
		cursor.label("class_decl");
		break;
	} 
}

var templ = "\tpublic static readonly BindableProperty {{prop_name}}Property = \n";
        templ += "\t\tBindableProperty.Create(nameof({{prop_name}}), typeof({{prop_type}}), typeof({{class_name}}), {{def_val}}, propertyChanged: OnPropertyChanged);\n";
	templ += "\tpublic {{prop_type}} {{prop_name}}\n";
	templ += "\t{\n";
	templ += "\t\tget => ({{prop_type}})GetValue({{prop_name}}Property);\n";
  	templ += "\t\tset => SetValue({{prop_name}}Property, value);\n\t}\n";

while(cursor.next_line())
{

	cursor.label("line");
	if(SkipSpace(query, true) && Match(query,"//[prop") && Is(query, iscsym, "prop_name") && Is(query, iscsym, "prop_type") && AnyNot(query, "]", "def_val") && Match(query, "]") && source.contains("BindableProperty " + query.get("prop_name") + "Property") == false)
	{
		print("generate property : " + query.get("prop_name"));
		source.insert_line(cursor, Mustache(templ).render(query));
	}
	else
	{
		cursor.goto_label("line");
	}
}

var templ2 = "\n\tprivate static void OnPropertyChanged(BindableObject bindable, object oldVal, object newVal)\n";
templ2 += "\t{\n";
templ2 += "\t\tvar ctrl = bindable as {{class_name}};\n";
templ2 += "\t\tctrl?.InvalidateSurface();\n";
templ2 += "\t}";

source.contains("OnPropertyChanged(BindableObject") == false && cursor.goto_label("class_decl") && cursor.move_to("{", "}", find) && cursor.prev_line() && source.insert_line(cursor, Mustache(templ2).render(query));

source.write();
print("end");