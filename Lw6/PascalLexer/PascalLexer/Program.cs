using PascalLexer;

public class Program
{
    private static void Main(string[] args)
    {
        Automata automata = new();

        using (StreamReader streamReader = new(args[0]))
        {
            string line;

            while ((line = streamReader.ReadLine()) != null)
            {
                for (int i = 0; i < line.Length; i++)
                {
                    automata.Handle(line[i]);

                    if (automata.GetOutWord().State != StateTypes.Error)
                    {
                        Console.WriteLine(automata.GetOutWord().State.ToString() + "-" + automata.GetOutWord().Word);
                    }
                }

                automata.Handle(' ');

                if (automata.GetOutWord().State != StateTypes.Error)
                {
                    Console.WriteLine(automata.GetOutWord().State.ToString() + "-" + automata.GetOutWord().Word);
                }
            }
        }
    }
}