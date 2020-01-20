#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main()
{
    while (true)
    {
        printf("Type a sentence (then hit return): ");
        string text;
        string reversed;
        string f;
        int e = 0;

        getline(cin, text);
        if (!cin.good())
        {
            printf("Done\n");
            return 0;
        }

        int length = 0;
        while (true)
        {
            if (text[length] == '\0')
                break;
            length = 1 + length;
        }

        int pos = 0;

        for (string::size_type i = 0; i < text.size(); ++i)
        {
            e = text.find(" ", pos);
            string m = text.substr(pos, e - pos);
            pos = e + 1;
            string reversed(m.rbegin(), m.rend());
            reversed.append(1, ' ');
            f.append(reversed);
        }

        string fin = f.substr(0, length);
        cout << fin << "\n";
        cout << endl;
    }
}