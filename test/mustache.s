
var tmpl = Mustache("{{#employees}}{{name}}, {{/employees}}");
var employees = Data([Data("name", "Steve"),Data("name", "Bill")]);
employees.push_back(Data("name", "Steve2"));

print(tmpl.render(Data("employees", employees)));
