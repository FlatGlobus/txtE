//07.07.2020
//txtE -e copyright.s -m *.h *.cpp -s <source_files_dir> -f header.txt -r

var ERROR = "Error: ";

if(program_options.size() == 0)
{
	print("Error: no command line options");
	print("-f <header_file_name>: -f mycopyrightsfile");
	print("-m <mask_files>: -m *.h *.cpp");
	print("-s <source directory>: -s d:\\myproject");
	print("-r: recursive ");
	return;
}

var header_file = get_options_value(program_options, "-f");
var file_mask = get_options_values(program_options, "-m");

var source_files = "";
if(is_options_key_exist(program_options, "-s") == false)
{
	source_files = current_path();
}
else
{
	source_files = get_options_value(program_options, "-s");
}

var recursive = is_options_key_exist(program_options, "-r");

var header = Text();
if(header.load(header_file) == 0)
{
	print(ERROR + "File " + header_file + " is empty.");
	return;
}

var files = get_files_from_folder(source_files, recursive, file_mask);
if(files.size() == 0)
{
	print(ERROR + "No files found in source directory");
	return;
}

var source_file = Text();
var cursor = Cursor(source_file);
var pattern_vector = ["Copyrights","(C)","All rights reserved"];
pattern_vector.push_back(header_file);

for(f : files)
{
	if(source_file.load(f) > 0)
	{
		cursor.begin();
		if(cursor.move_to(pattern_vector, find) == false)
		{
			cursor.begin();
			source_file.insert(cursor, "\n");
			source_file.insert(cursor, header);
			source_file.write(f + ".new");
			print("processed file : " + f);
		}

	}
}