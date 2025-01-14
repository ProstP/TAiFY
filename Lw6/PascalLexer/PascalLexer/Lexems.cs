namespace PascalLexer;

public static class Lexems
{
    public enum TokenType
    {
    }

    public static string[] ReservedWords { get; } =
    {
        "program",
        "var",
        "begin",
        "end",
        "if",
        "then",
        "else",
        "and",
        "or",
        "integer",
        "boolean",
        "char",
        "array",
        "not",
        "real",
        "of"
    };

    public static string[] PunctuationMarks { get; } =
    {
        "+",
        "-",
        "*",
        "/",
        ";",
        ",",
        "(",
        ")",
        "[",
        "]",
        "=",
        ">",
        "<",
        ">=",
        "<=",
        "<>",
        ":",
        ":=",
        ".",
        "//",
        "{",
        "}",
        "..",
    };

    public static string GetTokenType(string word)
    {
        switch (word)
        {
            case "program":
                return "PROGRAM";
            case "var":
                return "VAR";
            case "begin":
                return "BEGIN";
            case "end":
                return "END";
            case "if":
                return "IF";
            case "then":
                return "THEN";
            case "else":
                return "ELSE";
            case "and":
                return "AND";
            case "or":
                return "OR";
            case "integer":
                return "INTEGER";
            case "boolean":
                return "BOOLEAN";
            case "char":
                return "CHAR";
            case "array":
                return "ARRAY";
            case "not":
                return "NOT";
            case "real":
                return "REAL";
            case "of":
                return "OF";
            case "+":
                return "PLUS";
            case "-":
                return "MINUS";
            case "*":
                return "ASTERISK";
            case "/":
                return "SLASH";
            case ";":
                return "SEMICOLON";
            case ",":
                return "COMMA";
            case "(":
                return "LEFT_PARENTHESIS";
            case ")":
                return "RIGHT_PARENTHESIS";
            case "[":
                return "LEFT_BRACKET";
            case "]":
                return "RIGHT_BRACKET";
            case "=":
                return "EQUALS";
            case ">":
                return "GREATER_THAN";
            case "<":
                return "LESS_THAN";
            case ">=":
                return "GREATER_THAN_OR_EQUAL";
            case "<=":
                return "LESS_THAN_OR_EQUAL";
            case "<>":
                return "NOT_EQUAL";
            case ":":
                return "COLON";
            case ":=":
                return "ASSIGNMENT";
            case ".":
                return "DOT";
            case "//":
                return "COMMENT";
        case "{":
                return "COMMENT";
            case "}":
                return "END_COMMENT";
            case "..":
                return "DOUBLE_DOT";
            default:
                return "IDENTIFIER";
        }
    }
}
