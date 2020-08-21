print("----------------File---------------------------");

var temp_dir = temp_directory_path();
print("temp dir: " + temp_dir);

temp_dir /= "txte_test";
print("Create directory : " + temp_dir);
create_directory(temp_dir);

if(exists(temp_dir))
{
	print("Directory " + temp_dir + " created.");

	print("Remove " + temp_dir + " directory.");
	remove(temp_dir);

	if(exists(temp_dir) == false)
	{
		print("Directory " + temp_dir + " removed.");
	}
}

var tmp_file_name = tmpnam();
print("tmpnam :" + tmp_file_name);

temp_dir = temp_directory_path();
print("List files : " + temp_dir);
var files = get_files_from_folder(temp_dir, true, ["*.txt"]);
print(to_string(files));

