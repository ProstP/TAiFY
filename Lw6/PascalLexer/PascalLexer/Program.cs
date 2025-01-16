using PascalLexer;
using static PascalLexer.Lexems;
using System;

public enum Comment
{
    None,
    Block,
    Line
}
public class Token
{
    public string TokenType { get; set; }
    public string Value { get; set; }
    public int Line { get; set; }
    public int Position { get; set; }
}

public class Program
{
    private static void Main(string[] args)
    {
        Automata automata = new();
        List<Token> tokens = new();
        List<string> errors = new();

        using (StreamReader streamReader = new(args[0]))
        {
            int lineNum = 0;
            string line;
            Comment comment = Comment.None;
            bool isString = false;
            string stringText = "";

            while ((line = streamReader.ReadLine()) != null)
            {
                for (int i = 0; i < line.Length; i++)
                {
                    if (comment == Comment.None && !isString)
                    {
                        HandleSymbol(automata, line[i], tokens, i, lineNum, out comment, out isString);
                    }
                    else if (isString)
                    {
                        if (line[i] == '\'')
                        {
                            isString = false;
                        }

                        stringText += line[i];
                    }
                    else if (comment == Comment.Block)
                    {
                        if (line[i] == '}')
                        {
                            comment = Comment.None;
                        }
                    }
                }

                HandleSymbol(automata, ' ', tokens, line.Length, lineNum, out Comment comment1, out bool isString1);

                if (comment == Comment.Line)
                {
                    comment = Comment.None;
                }

                lineNum++;
            }
        }

        foreach (Token item in tokens)
        {
            Console.WriteLine(item.TokenType + "(" +
                (item.Line + 1).ToString() +
                ", " + (item.Position + 1).ToString() + ") " + item.Value);
        }
    }


    // Комменты должны быть закрыты
    // проверка на макс значения
    // Число с плавающей точкой (порядоу макс двухзначное)
    // посмотреть почему +-
    // ' начать строку
    // 

    private static void HandleSymbol(
        Automata automata,
        char symbol,
        List<Token> tokens,
        int position,
        int lineNum,
        out Comment comment,
        out bool isString)
    {
        if (symbol == '{')
        {
            comment = Comment.Block;
            isString = false;
            return;
        }

        if (symbol == '\'')
        {
            comment = Comment.None;
            isString = true;
            return;
        }

        automata.Handle(symbol);
        OutWord outWord = automata.GetOutWord();
        if (!string.IsNullOrWhiteSpace(outWord.Word))
        {
            if (outWord.Word == "//")
            {
                comment = Comment.Line;
                isString = false;
                return;
            }
            else
            {
                tokens.Add(new()
                {
                    TokenType = GetTokenType(outWord),
                    Line = lineNum,
                    Position = position - outWord.Word.Length,
                    Value = outWord.Word,
                });
            }
        }

        comment = Comment.None;
        isString = false;
    }
    private static string GetTokenType(OutWord word)
    {
        if (word.State == StateTypes.Digit)
        {
            return "DIGIT";
        }
        else if (word.State == StateTypes.Real)
        {
            return "REAL";
        }
        else if (word.State == StateTypes.Identifier || word.State == StateTypes.SpecialWord)
        {
            string needWord = word.Word.ToLower();
            string reservedWord = Lexems.ReservedWords.FirstOrDefault(w => w == needWord);

            if (reservedWord == null)
            {
                return "IDENTIFIER";
            }
            else
            {
                return reservedWord.ToUpper();
            }
        }
        else if (word.State == StateTypes.PermationMark)
        {
            string mark = Lexems.PunctuationMarks.FirstOrDefault(w => w == word.Word);

            if (mark != null)
            {
                return Lexems.GetTokenTypeOfMark(word.Word);
            }
            else
            {
                return "ERROR";
            }
        }
        else
        {
            return "ERROR";
        }
    }
}