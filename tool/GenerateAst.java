package com.krgamestudios.tool;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.List;

public class GenerateAst {
	public static void main(String[] args) throws IOException {
		if (args.length != 1) {
			System.err.println("Usage: generate_ast <output directory>");
			System.exit(-1);
		}
		String outputDir = args[0];

		defineAst(outputDir, "Expr", Arrays.asList(
			"Assign   : Token name, Expr value",
			"Binary   : Expr left, Token operator, Expr right",
			"Grouping : Expr expression",
			"Literal  : Object value",
			"Postfix  : Expr left, Token operator",
			"Prefix   : Token operator, Expr right",
			"Variable : Token name"
		));

		defineAst(outputDir, "Stmt", Arrays.asList(
			"Block      : List<Stmt> statements",
			"Expression : Expr expression",
			"Log        : Expr expression",
			"Var        : Token name, Expr initializer"
		));
	}

	private static void defineAst(String outputDir, String baseName, List<String> types) throws IOException {
		String path = outputDir + "/" + baseName + ".java";
		PrintWriter writer = new PrintWriter(path, "UTF-8");

		writer.println("package com.krgamestudios.toy;");
		writer.println("");
		writer.println("import java.util.List;");
		writer.println("");
		writer.println("abstract class " + baseName + " {");

		//the visitor pattern
		defineVisitor(writer, baseName, types);

		//the base accept method
		writer.println("");
		writer.println("\tabstract <R> R accept(Visitor<R> visitor);");

		//the AST classes
		for (String type : types) {
			String className = type.split(":")[0].trim();
			String fields = type.split(":")[1].trim();
			defineType(writer, baseName, className, fields);
		}

		writer.println("}");
		writer.close();
	}

	private static void defineType(PrintWriter writer, String baseName, String className, String fieldList) {
		writer.println("\tstatic class " + className + " extends " + baseName + " {");

		//store parameters in fields
		String[] fields = fieldList.split(", ");
		for (String field : fields) {
			field.trim();
			writer.println("\t\tfinal " + field + ";");
		}

		writer.println("");
		
		//constructor
		writer.println("\t\t" + className + "(" + fieldList + ") {");
		for (String field : fields) {
			String name = field.split(" ")[1];
			writer.println("\t\t\tthis." + name + " = " + name + ";");
		}
		writer.println("\t\t}");

		//visitor accept function
		writer.println("");
		writer.println("\t\t<R> R accept(Visitor<R> visitor) {");
		writer.println("\t\t\treturn visitor.visit(this);");
		writer.println("\t\t}");

		writer.println("\t}");
	}

	private static void defineVisitor(PrintWriter writer, String baseName, List<String> types) {
		writer.println("\tinterface Visitor<R> {");

		for (String type : types) {
			String typeName = type.split(":")[0].trim();
			writer.println("\t\tR visit(" + typeName + " " + baseName.toLowerCase() + ");");
		}

		writer.println("\t}");
	}
}