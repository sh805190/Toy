package com.krgamestudios.toy;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public class Toy {
	private static final Interpreter interpreter = new Interpreter();
	private static boolean hadError = false;
	private static boolean hadRuntimeError = false;

	public static void main(String[] args) throws IOException {
		if (args.length > 1) {
			System.out.println("Usage: toybox [script]");
		} else if (args.length == 1) {
			runFile(args[0]);
		} else {
			runPrompt();
		}
	}

	private static void runFile(String path) throws IOException {
		byte[] bytes = Files.readAllBytes(Paths.get(path));
		run(new String(bytes, Charset.defaultCharset()));
		//error
		if (hadError) {
			System.exit(-1);
		}
		if (hadRuntimeError) {
			System.exit(-2);
		}
	}

	private static void runPrompt() throws IOException {
		InputStreamReader input = new InputStreamReader(System.in);
		BufferedReader reader = new BufferedReader(input);

		for (;;) {
			System.out.print("> ");
			run(reader.readLine());

			if (hadError) {
				System.out.println("parse error");
			}
			if (hadRuntimeError) {
				System.out.println("runtime error");
			}
			hadError = false;
			hadRuntimeError = false;
		}
	}

	private static void run(String source) {
		Scanner scanner = new Scanner(source);
		Parser parser = new Parser(scanner.scanTokens());

		List<Stmt> statements = parser.parse();

		//syntax error
		if (hadError) return;

//		System.out.println(new AstPrinter().print(expression));

		interpreter.interpret(statements);
	}

	public static void error(int line, String message) {
		report(line, "", message);
	}

	public static void error(Token token, String message) {
		if (token.type == TokenType.EOF) {
			report(token.line, " at end", message);
		} else {
			report(token.line, " at '" + token.lexeme + "'", message);
		}
	}

	public static void runtimeError(RuntimeError error) {
		System.err.println(error.getMessage() + "\n[line " + error.token.line + "]");
		hadRuntimeError = true;
	}

	private static void report(int line, String where, String message) {
		System.err.println("[line " + line + "] Error" + where + ": " + message);
		hadError = true;
	}
}