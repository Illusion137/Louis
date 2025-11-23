#!/opt/homebrew/bin/node
const fs = require('fs');

function json_type_to_cpp_type(json_type, name){
    switch(json_type){
        case "string": return "std::string";
        case "number": return "uint32_t";
        case "boolean": return "bool";
        case "object": return name;
    }
}
function pascal_case_to_snake_case(str){
    return str.split(/\.?(?=[A-Z])/).join('_').toLowerCase();
}

if(process.argv.length < 3) {
    console.error("No path found");
    process.exit(1);
}
const path = process.argv[2];

let json = JSON.parse(fs.readFileSync(path, "utf-8").toString());

function write_indent(indent){
    for(let i = 0; i < indent; i++){
        process.stdout.write('\t');
    }
}

function write_json(name, obj, indent){
    write_indent(indent);
    process.stdout.write(`struct ${name} {` + '\n');
    if(Array.isArray(obj)) obj = obj[0];
    for(const [key, value] of Object.entries(obj)){
        if(typeof value === "object"){
            write_json(`${name}_${key}`, value, indent + 1);
        }
        const type = json_type_to_cpp_type(typeof value, `${name}_${key}`);
        write_indent(indent);
        process.stdout.write(`\t${type} ${key};\n`);
    }
    write_indent(indent);
    process.stdout.write('};\n');
}
write_json(process.argv[3] ?? "ROOT0", json, 0);