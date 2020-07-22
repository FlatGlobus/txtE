var text1 = "abcd klmn";
var text2 = "abcd koprst";
var text = Text(text1);
var cursor  = Cursor(text);

var difference = "";
text.diff(cursor, text2, difference);
print(difference);