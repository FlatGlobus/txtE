var text = Text();
text.load("txt.txt");
var cursor  = Cursor(text);

var difference = "";
text.diff(cursor, text2, difference);
print(difference);