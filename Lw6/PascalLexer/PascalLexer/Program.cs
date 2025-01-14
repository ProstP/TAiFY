using PascalLexer;

public class Program
{
    private static void Main(string[] args)
    {
        Automata automata = new();

        using (StreamReader streamReader = new(args[0]))
        {
            int lineNum = 0;
            string line;
            int commentLineNum = 0;
            int commentPos = 0;
            bool commentBlock = false;
            bool commentLine = false;
            string commentText = "";

            while ((line = streamReader.ReadLine()) != null)
            {
                for (int i = 0; i < line.Length; i++)
                {
                    if (!commentLine && !commentBlock)
                    {
                        automata.Handle(line[i]);

                        if (!string.IsNullOrEmpty(automata.GetOutWord().Word))
                        {
                            string tokenType = GetTokenType(automata.GetOutWord());
                            string word = automata.GetOutWord().Word;

                            if (tokenType != "COMMENT")
                            {
                                WriteTokenToConsole(tokenType, lineNum, i - word.Length -
                                    (tokenType == "DIGIT" && line[i] == '.' ? 1 : 0), word);
                            }
                            else
                            {
                                if (word == "{")
                                {
                                    commentBlock = true;
                                }
                                else if (word == "//")
                                {
                                    commentLine = true;
                                }

                                commentLineNum = lineNum;
                                commentPos = i - word.Length;
                                commentText = word;
                            }
                        }
                    }

                    if (commentBlock || commentLine)
                    {
                        commentText += line[i];

                        if (line[i] == '}')
                        {
                            commentBlock = false;

                            WriteTokenToConsole("COMMENT", commentLineNum, commentPos, commentText);
                        }
                    }
                }

                if (!commentLine && !commentBlock)
                {
                    automata.Handle(' ');

                    if (!string.IsNullOrEmpty(automata.GetOutWord().Word))
                    {
                        string tokenType = GetTokenType(automata.GetOutWord());
                        string word = automata.GetOutWord().Word;

                        if (tokenType != "COMMENT")
                        {
                            WriteTokenToConsole(tokenType, lineNum, line.Length - word.Length, word);
                        }
                        else
                        {
                            if (word == "{")
                            {
                                commentBlock = true;
                            }
                            else if (word == "//")
                            {
                                commentLine = true;
                            }

                            commentLineNum = lineNum;
                            commentPos = line.Length - word.Length;
                            commentText = word;
                        }
                    }
                }
                else if (commentLine)
                {
                    commentLine = false;

                    WriteTokenToConsole("COMMENT", commentLineNum, commentPos, commentText);
                }

                lineNum++;
            }
        }
    }

    private static string GetTokenType(OutWord word)
    {
        if (word.State == StateTypes.Error)
        {
            return "ERROR";
        }
        else if (word.State == StateTypes.Digit)
        {
            return "DIGIT";
        }
        else if (word.State == StateTypes.Real)
        {
            return "REAL";
        }
        else
        {
            return Lexems.GetTokenType(word.Word.ToLower());
        }
    }

    private static void WriteTokenToConsole(string tokenType, int line, int num, string token)
    {
        Console.WriteLine(tokenType + "(" + (line + 1).ToString() + ", " + (num + 1).ToString() + ") " + token);
    }
}