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
    };
}
