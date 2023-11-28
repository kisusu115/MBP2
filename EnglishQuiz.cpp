#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <locale>
#include <conio.h> // _getch()
#include <sstream>

#define KOR_MAX 5 // �ѱ� �� �ִ� 5�� ����

using namespace std;

//�Լ� �������
void edit();
void totalQuiz();
void wrongQuiz();
void quiz();
void viewWrong();
bool checkEng(string& str);
bool checkKor(wstring& str);

class Word { // �ѱ� ��, ����ܾ�, ����, ������ ���� Class Word
public:
    string eng;
    string kor[KOR_MAX]; // �ѱ� �� �迭
    int cnt = 0; // �ѱ� �� ����
    int syll; // ���� ��
    int acc; // ���� ��ġ

    Word(const string& eng, const string* ko, int c, int s, int a) : eng(eng), syll(s), acc(a) {
        while (c) {
            kor[cnt] = ko[cnt];
            cnt++;
            c--;
        }
    }
    bool equals(Word word);
};

/*
Word���� ���� �� üũ�ϴ� �޼ҵ�
@param word üũ�ϴ� Word
*/
bool Word::equals(Word word) {
    if (this->eng == word.eng && this->cnt == word.cnt && this->syll == word.syll && this->acc == word.acc) { // �⺻ �ʵ� üũ
        bool exist;
        for (int i = 0; i < this->cnt; i++) { // �ѱ� ���� ���� ��� ���� ���� �ִ��� üũ
            exist = false;
            string testKor = this->kor[i];
            for (int j = 0; j < this->cnt; j++) {
                if (testKor == word.kor[j])
                    exist = true;
            }
            if (!exist)
                return false;
        }
        return true;
    }
    return false;
}

class QuizWord { // Class Word�� Quiz���� �ٷ�� ������ ���� Class QuizWord 
public:
    string eng;
    string kor; // �ѱ� �� 1��
    int syll; // ����
    int acc; // ���� ��ġ

    QuizWord(const string& eng, const string& kor, int s, int a) : eng(eng), kor(kor), syll(s), acc(a) {}
};

vector<Word> wordList; // Word ��ü ���� vector�� wordList
vector<Word> wrongWordList; // ���� Word ��ü ���� vector�� wrongWordList
vector<Word> wrongWordHistoryList; // �����Ʈ�� �� ���̶� �� Word ��ü ���� vector�� wrongWordHistoryList
vector<Word> wrongAccList; // ���� ���� Word ��ü ���� vector�� wrongAccList
vector<Word> wrongAccHistoryList; // ���� �����Ʈ�� �� ���̶� �� Word ��ü ���� vector �� wrongAccHistoryList

// ���� ��ƿ��Ƽ �Լ���
/*
narrow character string (std::string)�� wide character string (std::wstring)���� ��ȯ�Ͽ� �ѱ� ���� �˻��ϴ� checkKor(wstring& str)�� ���
*/
wstring s2w(const string& str)
{
    static locale loc("");
    auto& facet = use_facet<codecvt<wchar_t, char, mbstate_t>>(loc);
    return wstring_convert<remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(str);
}

/*
wide character string (std::wstring)�� narrow character string (std::string)���� ��ȯ�Ͽ� Word Class���� �ٷ�� ���� ��
*/
string w2s(const wstring& wstr)
{
    static locale loc("");
    auto& facet = use_facet<codecvt<wchar_t, char, mbstate_t>>(loc);
    return wstring_convert<remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstr);
}

/*
�����ڿ��� ��� string ��ü�� lowercase�� ��ȯ (string���� ���ϱ� ������)
@param str �����ڿ��� ��� narrow character string (std::string)
@return ��� �빮�ڰ� �ҹ��ڷ� ��ȯ�� �����ڿ��� ��� narrow character string (std::string)
*/
string lowerString(string& str) {

    size_t start = str.find_first_not_of(" ");
    if (start == string::npos) { return ""; }
    size_t end = str.find_last_not_of(" ");
    str = str.substr(start, end - start + 1);

    string out = str;
    for (char& c : out) {
        c = tolower(c);
    }
    return out;
}

/*
�Է°����� ���� ���ڿ��� ������Ұ� ��� �������� �Ǻ��Ͽ�, stoi���� ���� �� ������ �ش� ������ ��ȯ�ϰ�, ���� �Ұ��� �� -1�� ��ȯ��
EX) str�� "123"�̳� "  123 "�̸� 123����, str�� "a357"�̳� "Apple"�̸� -1 ����
@param str ���ڷ� �޴� string ���� �޴������� �Է°��̳� ���� ���� Ǯ�̿��� �������� �����ϱ����� �Է� �޾Ҵ� string�� ��
@return �Է¹��� string�� ������ ��ȯ, ������ ��ȯ �Ұ����� ���ڿ��� �ɷ����� -1 ����
*/
int input2int(string& str) {
    int npos;
    npos = str.find_first_not_of(' ');
    str.erase(0, npos);
    npos = str.find_last_not_of(' ');
    str.erase(npos + 1);

    int optint;
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] != '0' && str[i] != '1' && str[i] != '2' && str[i] != '3' && str[i] != '4'
            && str[i] != '5' && str[i] != '6' && str[i] != '7' && str[i] != '8' && str[i] != '9')
            return -1;
    }
    try {
        optint = stoi(str);
    }
    catch (exception e) {
        optint = -1;
    }

    return optint;
}

/*
����ܾ ���� ���Ŀ� �ùٸ��� Ȯ���ϴ� �Լ�
@param str ����ܾ ��� narrow character string (std::string)
@return ������ �����ϸ� true, �׷��� ������ false
*/
bool checkEng(string& str) {

    str = lowerString(str);

    if (str.size() > 30) {
        return 0;            //���ڿ� ���� �ִ� 30 ����
    }

    bool hasEng = false;      //���ĺ� �ϳ����� �����̶� '-'�� �ִ��� üũ�ϱ�����
    for (char c : str) {
        if (!((c >= 'a' && c <= 'z') || (c == ' ') || (c == '-'))) {
            return 0;
        }
        if (c >= 'a' && c <= 'z') {
            hasEng = true;      //���ĺ� �ּ� 1���� �߰��ϸ� hasEng�� true�� ��ȯ
        }
    }

    if (!hasEng) return 0;      //���ĺ� �ϳ��� ������ ���ܾ� ���� �������� ���ϹǷ�

    return 1;  // ���� ���ĺ� ������ ������Ŵ
}

/*
�ѱ� ���� ���� ���Ŀ� �ùٸ��� Ȯ���ϴ� �Լ�
@param str �ѱ� ���� ��� wide character string (std::wstring)
@return ������ �����ϸ� true, �׷��� ������ false
*/
bool checkKor(wstring& str) {
    int npos;
    npos = str.find_first_not_of(' ');
    str.erase(0, npos);
    npos = str.find_last_not_of(' ');
    str.erase(npos + 1);

    if (str.size() > 30 || str.size() == 0) {
        return 0;
    }
    for (int i = 0; i < str.size(); i++) {
        if (str[i] != 32 && (str[i] < 44032 || str[i] > 55199))
            return 0;
    }
    return 1;
}

/*
���ڿ� �迭�� �Ϸķ� ','�� �����Ͽ� ���
@param arr ���ڿ� �迭
@param newline �ٹٲ� ����
*/
void printStringArray(const string* arr, int count, bool newline) {
    for (int i = 0; i < count; i++) {
        if (i == count - 1)
            cout << arr[i];
        else
            cout << arr[i] << ", ";
    }
    if (newline) cout << endl;
}

/*
�ϳ��� wstring�� �и��Ͽ� ���ͷ� ��ȯ�ϴ� �Լ�
@param input �и��� wstring
@param delimiter ���� ����
@return �и��� wstring���� ��ҷ� ���� ����
*/
vector<wstring> splitTrimWString(const wstring& input, wchar_t delimiter) {
    vector<wstring> tokens;
    wstringstream ss(input);
    wstring token;

    while (getline(ss, token, delimiter)) {
        // ���� ���� ����
        token.erase(token.begin(), find_if(token.begin(), token.end(), [](wint_t ch) {
            return !iswspace(ch);
            }));
        token.erase(find_if(token.rbegin(), token.rend(), [](wint_t ch) {
            return !iswspace(ch);
            }).base(), token.end());

        if (token.length() > 0)
            tokens.push_back(token);
    }

    return tokens;
}

/*
wordVector���� �־��� eng ���ܾ �ش��ϴ� Word��ü���� iterator�� �̵������ִ� �Լ�
@param wordVector wordList, wrongWordList, wrongWordHistoryList, wrongAccList, wrongAccHistoryList
@param it vector<Word>�� �ش��ϴ� iterator
@param eng ���ܾ�
@return �־��� ���ܾ ������ true, ������ false
*/
bool findInWordList(vector<Word>& wordVector, vector<Word>::iterator& it, const string eng) {
    for (it = wordVector.begin(); it != wordVector.end(); ++it) {
        if (it->eng == eng) {
            return true;
        }
    }
    return false;
}


/*
txt���Ͽ��� string�� �о�ͼ� Word ��ü�� ��ȯ �� ����
@param fileName ���� txt����
@param wordVector ���� string�� Word ��ü�� ��ȯ �� ������ vector
@param checkDuplicate ���� ���ܾ �ִ��� üũ ����

void loadWordsFromFile(const string& fileName, vector<Word>& wordVector, bool checkDuplicate) {
    ifstream file(fileName);
    if (file.is_open()) {
        wordVector.clear(); // ���� wordVector ����
        string line;
        while (getline(file, line)) {
            size_t pos = line.find("/");

            if (pos == string::npos)
            {
                cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                exit(0);
            }
            else {
                string eng = line.substr(0, pos);
                if (checkEng(eng) == 0) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");

                // ���� �о����
                string s = line.substr(0, pos);
                int syll = input2int(s);
                if (syll == -1) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");

                // ���� ��ġ �о����
                string a = line.substr(0, pos);
                int acc = input2int(a);
                if (acc == -1) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // �ѱ� �� �о����
                line = line.substr(pos + 1);
                pos = line.find(",");
                string kor[KOR_MAX];
                int cnt = 0;

                while (pos != string::npos) {
                    string ko = line.substr(0, pos);
                    wstring k = s2w(ko);
                    if (checkKor(k) == 0) {
                        cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                        exit(0);
                    }
                    ko = w2s(k);
                    kor[cnt] = ko;
                    cnt++;
                    line = line.substr(pos + 1);
                    pos = line.find(",");
                    cout << "cnt: " << cnt << endl;
                }

                kor[cnt] = line;
                cnt++; // ���� = ������ �ε��� + 1

                wordVector.push_back(Word(eng, kor, cnt, syll, acc));
            }
        }
        file.close();

        // ��ġ�� ����ܾ �ִ��� üũ
        if (checkDuplicate) {
            if (!wordVector.empty()) { // wordVector�� ������� �� ���� ���� �ذ�
                for (int i = 0; i < wordVector.size() - 1; i++) {
                    for (int j = i + 1; j < wordVector.size(); j++) {
                        if (wordVector[i].eng == wordVector[j].eng) {
                            cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                            exit(0);
                        }
                    }
                }
            }
        }

    }
    else {
        cerr << "Error: " << fileName << "�� �ҷ��� �� �����ϴ�." << endl;
        return;
    }
}
*/

/*
Word.txt���Ͽ��� string�� �о�ͼ� Word ��ü�� ��ȯ �� ����
@param fileName ���� txt����
*/
void loadWordsFromWordFile(const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        wordList.clear(); // ���� wordList ����
        string line;
        while (getline(file, line)) {
            size_t pos = line.find("/");

            if (pos == string::npos)
            {
                cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                exit(0);
            }
            else {
                string eng = line.substr(0, pos);
                if (checkEng(eng) == 0) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");

                // ���� �о����
                string s = line.substr(0, pos);
                int syll = input2int(s);
                if (syll == -1) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");

                // ���� ��ġ �о����
                string a = line.substr(0, pos);
                int acc = input2int(a);
                if (acc == -1) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // ������ġ�� ���� ������ Ŭ �� ����
                if (acc > syll) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // �ѱ� �� �о����
                line = line.substr(pos + 1);
                pos = line.find(",");
                string kor[KOR_MAX];
                int cnt = 0;

                while (pos != string::npos) {
                    string ko = line.substr(0, pos);
                    wstring k = s2w(ko);
                    if (checkKor(k) == 0) {
                        cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                        exit(0);
                    }
                    ko = w2s(k);
                    kor[cnt] = ko;
                    cnt++;
                    line = line.substr(pos + 1);
                    pos = line.find(",");
                }

                kor[cnt] = line;
                cnt++; // ���� = ������ �ε��� + 1

                wordList.push_back(Word(eng, kor, cnt, syll, acc));
            }
        }
        file.close();

        // ��ġ�� ����ܾ �ִ��� üũ
        if (!wordList.empty()) { // wordList�� ������� �� ���� ���� �ذ�
            for (int i = 0; i < wordList.size() - 1; i++) {
                for (int j = i + 1; j < wordList.size(); j++) {
                    if (wordList[i].eng == wordList[j].eng) {
                        cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                        exit(0);
                    }
                }
            }
        }

    }
    else {
        cerr << "Error: " << fileName << "�� �ҷ��� �� �����ϴ�." << endl;
        return;
    }
}

/*
Wrong.txt �Ǵ� WrongHistory.txt���Ͽ��� string�� �о�ͼ� Word ��ü�� ��ȯ �� ����
@param fileName ���� txt����
@param wordVector ���� string�� Word ��ü�� ��ȯ �� ������ vector - wrongWordList, wrongWordHistoryList
*/
void loadWordsFromWrongWordFile(const string& fileName, vector<Word>& wordVector) {
    ifstream file(fileName);
    if (file.is_open()) {
        wordVector.clear(); // ���� wordVector ����
        string line;
        while (getline(file, line)) {
            size_t pos = line.find("/");

            if (pos == string::npos)
            {
                cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                exit(0);
            }
            else {
                string eng = line.substr(0, pos);
                if (checkEng(eng) == 0) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                pos = line.find("/");

                if (pos == string::npos)
                {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // �ѱ� �� �о����
                line = line.substr(pos + 1);
                pos = line.find(",");
                string kor[KOR_MAX];
                int cnt = 0;

                while (pos != string::npos) {
                    string ko = line.substr(0, pos);
                    wstring k = s2w(ko);
                    if (checkKor(k) == 0) {
                        cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                        exit(0);
                    }
                    ko = w2s(k);
                    kor[cnt] = ko;
                    cnt++;
                    line = line.substr(pos + 1);
                    pos = line.find(",");
                }

                kor[cnt] = line;
                cnt++; // ���� = ������ �ε��� + 1

                std::vector<Word>::iterator it;
                if (findInWordList(wordList, it, eng)) {
                    int syll = it->syll;
                    int acc = it->acc;

                    wordVector.push_back(Word(eng, kor, cnt, syll, acc));
                }
                else {
                    wordVector.push_back(Word(eng, kor, cnt, -1, -1));
                }
            }
        }
        file.close();

        // ��ġ�� ����ܾ �ִ��� üũ
        if (!wordVector.empty()) { // wordVector�� ������� �� ���� ���� �ذ�
            for (int i = 0; i < wordVector.size() - 1; i++) {
                for (int j = i + 1; j < wordVector.size(); j++) {
                    if (wordVector[i].eng == wordVector[j].eng) {
                        cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                        exit(0);
                    }
                }
            }
        }

    }
    else {
        cerr << "Error: " << fileName << "�� �ҷ��� �� �����ϴ�." << endl;
        return;
    }
}

/*
WrongAccent.txt �Ǵ� WrongAccentHistory.txt���Ͽ��� string�� �о�ͼ� Word ��ü�� ��ȯ �� ����
@param fileName ���� txt����
@param wordVector ���� string�� Word ��ü�� ��ȯ �� ������ vector - wrongAccList, wrongAccHistoryList
*/
void loadWordsFromWrongAccentFile(const string& fileName, vector<Word>& wordVector) {
    ifstream file(fileName);
    if (file.is_open()) {
        wordVector.clear(); // ���� wordVector ����
        string line;
        while (getline(file, line)) {
            size_t pos = line.find("/");

            if (pos == string::npos)
            {
                cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                exit(0);
            }
            else {
                string eng = line.substr(0, pos);
                if (checkEng(eng) == 0) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");
                if (pos == string::npos)
                {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // ���� �о����
                string s = line.substr(0, pos);
                int syll = input2int(s);
                if (syll == -1) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // ���� ��ġ �о����
                string a = line.substr(pos + 1);
                int acc = input2int(a);
                if (acc == -1) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                // ������ġ�� ���� ������ Ŭ �� ����
                if (acc > syll) {
                    cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                    exit(0);
                }

                std::vector<Word>::iterator it;
                string tempKor[KOR_MAX];
                for (int i = 0; i < KOR_MAX; i++) {
                    tempKor[i] = "";
                }
                if (findInWordList(wordList, it, eng)) {
                    for (int i = 0; i < KOR_MAX; i++) {
                        tempKor[i] = it->kor[i];
                    }

                    wordVector.push_back(Word(eng, tempKor, it->cnt, syll, acc));
                }
                else {
                    wordVector.push_back(Word(eng, tempKor, 0, syll, acc));
                }
            }
        }
        file.close();

        // ��ġ�� ����ܾ �ִ��� üũ
        if (!wordList.empty()) { // wordList�� ������� �� ���� ���� �ذ�
            for (int i = 0; i < wordList.size() - 1; i++) {
                for (int j = i + 1; j < wordList.size(); j++) {
                    if (wordList[i].eng == wordList[j].eng) {
                        cout << fileName << " ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
                        exit(0);
                    }
                }
            }
        }

    }
    else {
        cerr << "Error: " << fileName << "�� �ҷ��� �� �����ϴ�." << endl;
        return;
    }
}

/*
Word.txt, Wrong.txt, WrongHistory.txt�� ��� �ҷ����� �Լ�
*/
void loadWordsFromFiles() {
    loadWordsFromWordFile("Word.txt");
    // Word.txt�� ���� �ҷ��;� �Ʒ� ���ϵ��� �ҷ����� ����
    loadWordsFromWrongWordFile("Wrong.txt", wrongWordList);
    loadWordsFromWrongWordFile("WrongHistory.txt", wrongWordHistoryList);
    loadWordsFromWrongAccentFile("WrongAccent.txt", wrongAccList);
    loadWordsFromWrongAccentFile("WrongAccentHistory.txt", wrongAccHistoryList);
}

/*
��ü �ܾ� ����� ������ Ʋ�� ��� �ش� �ܾ wrongWordList, wrongWordHistoryList�� �߰��ϴ� �Լ�
@param wrongWord Ʋ�� �ܾ�
*/
void pushWrongQuizWord(QuizWord wrongWord) {
    bool exist = false;
    vector<Word>::iterator it;
    
    // ���� ���ܾ wrongWordList�� �����ϴ��� �˻�
    if (findInWordList(wrongWordList, it, wrongWord.eng))
        exist = true;

    if (!exist) { // ���ܾ �������� ������ �����Ʈ�� �߰�
        string kor[KOR_MAX];
        kor[0] = wrongWord.kor;
        wrongWordList.push_back(Word(wrongWord.eng, kor, 1, wrongWord.syll, wrongWord.acc));
    }
    else { // ���ܾ ������ ��, �ش� �ѱ� ���� ������ �߰�
        bool e = false; // üũ��
        for (int i = 0; i < it->cnt; i++) {
            if (it->kor[i] == wrongWord.kor) {
                e = true;
                break;
            }
        }
        if (!e) {
            it->kor[it->cnt] = wrongWord.kor;
            it->cnt++;
        }
        else {
            cout << "�ش� ���ܾ �̹� �����մϴ�." << endl;
        }
    }
    exist = false;

    // ���� ���ܾ wrongWordHistoryList�� �����ϴ��� �˻�
    if (findInWordList(wrongWordHistoryList, it, wrongWord.eng))
        exist = true;

    if (!exist) { // ���ܾ �������� ������ �����Ʈ�� �߰�
        string kor[KOR_MAX];
        kor[0] = wrongWord.kor;
        wrongWordHistoryList.push_back(Word(wrongWord.eng, kor, 1, wrongWord.syll, wrongWord.acc));
    }
    else { // ���ܾ ������ ��, �ش� �ѱ� ���� ������ �߰�
        bool e = false; // üũ��
        for (int i = 0; i < it->cnt; i++) {
            if (it->kor[i] == wrongWord.kor) {
                e = true;
                break;
            }
        }
        if (!e) {
            it->kor[it->cnt] = wrongWord.kor;
            it->cnt++;
        }
    }
}

/*
��ü ���� ����� ������ Ʋ�� ��� �ش� �ܾ wrongAccList, wrongAccHistoryList�� �߰��ϴ� �Լ�
@param wrongWord Ʋ�� �ܾ�
*/
void pushWrongAccentQuizWord(Word wrongWord) {
    bool exist = false;
    vector<Word>::iterator it;

    // ���� ���ܾ wrongWordList�� �����ϴ��� �˻�
    if (findInWordList(wrongAccList, it, wrongWord.eng))
        exist = true;

    if (!exist) { // ���ܾ �������� ������ �����Ʈ�� �߰�
        string kor[KOR_MAX];
        for (int i = 0; i < KOR_MAX; i++)
            kor[i] = "";
        wrongAccList.push_back(Word(wrongWord.eng, kor, 0, wrongWord.syll, wrongWord.acc));
    } else {
        cout << "�ش� ���ܾ �̹� �����մϴ�." << endl;
    }
    exist = false;

    // ���� ���ܾ wrongWordHistoryList�� �����ϴ��� �˻�
    if (findInWordList(wrongAccHistoryList, it, wrongWord.eng))
        exist = true;

    if (!exist) { // ���ܾ �������� ������ �����Ʈ�� �߰�
        string kor[KOR_MAX];
        for (int i = 0; i < KOR_MAX; i++)
            kor[i] = "";
        wrongAccHistoryList.push_back(Word(wrongWord.eng, kor, 0, wrongWord.syll, wrongWord.acc));
    }
}


/*
���� ���α׷� ���� Word ��ü���� ȣ��� ���� �������� txt���Ͽ� �����ϴ� �Լ�
@param fileName ������ txt����
@param wordVector ������ Word ��ü�� ��� vector
@param type wordVector�� Ÿ�� - 0�̸� wordList, 1�̸� wrongWordList �Ǵ� wrongWordHistoryList, 2�̸� wrongAccList �Ǵ� wrongAccHistoryList
*/
void saveWordsToFile(const string& fileName, vector<Word>& wordVector, int type) {
    ofstream file(fileName);
    if (file.is_open()) {
        for (const Word& word : wordVector) {
            file << word.eng << "/";
            if (type == 0) { // Word.txt ���� ����
                file << word.syll << "/" << word.acc << "/";
                for (int i = 0; i < word.cnt; i++) {
                    file << word.kor[i];
                    if (i != word.cnt - 1) {
                        file << ",";
                    }
                }
            }
            else if (type == 1) { // Wrong.txt, WrongHistory.txt ���� ����
                for (int i = 0; i < word.cnt; i++) {
                    file << word.kor[i];
                    if (i != word.cnt - 1) {
                        file << ",";
                    }
                }
            }
            else if (type == 2) { // WrongAccent.txt, WrongAccentHistory.txt ���� ����
                file << word.syll << "/" << word.acc;
            }
            file << endl;
        }
        file.close();
    }
    else {
        cerr << "Error: " << fileName << "�� ������ �� �����ϴ�." << endl;
    }
}

/*
Word.txt, Wrong.txt, WrongHistory.txt�� ��� �����ϴ� �Լ�
*/
void saveWordsToFiles() {
    saveWordsToFile("Word.txt", wordList, 0);
    saveWordsToFile("Wrong.txt", wrongWordList, 1);
    saveWordsToFile("WrongHistory.txt", wrongWordHistoryList, 1);
    saveWordsToFile("WrongAccent.txt", wrongAccList, 2);
    saveWordsToFile("WrongAccentHistory.txt", wrongAccHistoryList, 2);
}


int mainFirst = 0; // main�޴� ���� �ʱⰪ
int viewFirst = 0; // 

int main() {


    setlocale(LC_ALL, "korean");
    loadWordsFromFiles();
    system("cls");


    while (1) {

        if (mainFirst == 0)
        {
            cout << "1. ���ܾ� ����     2. ���ܾ� ����     3. ����ܾ� ��ȸ \n4. ����\n";
            mainFirst++;
        }
        int menuSelect; // �޴� �Է¹������� ����

        string mainipt;
        getline(cin, mainipt);
        menuSelect = input2int(mainipt);

        if (menuSelect == 1) {         // ���ܾ� ���� ����
            quiz();
        }
        else if (menuSelect == 2) {      // ���ܾ��� ���� �޴� ����
            edit();
        }
        else if (menuSelect == 3) {      // ����ܾ� ��ȸ
            viewWrong();
        }
        else if (menuSelect == 4) {      // ����
            cout << "���α׷��� �����մϴ�.";
            break;
        }
        else {
            system("cls");
            cout << "1. ���ܾ� ����     2. ���ܾ� ����     3. ����ܾ� ��ȸ \n4. ����\n";
            cout << "�ùٸ��� ���� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
        }

    }

    return 0;
}


/*
edit()�ȿ��� ���ܾ��� �ѱ� ��, ���� ��, ���� ��ġ ���� �� ���� ������ ���� ó���ϴ� �Լ�
@param wordVector wrongWordList, wrongWordHistory, wrongAccList, wrongAccHistoryList
@param editedWord edit()�ȿ��� ������ Word
@param acc �Ϲ� �����̸� false, ���� �����̸� true
*/
void editWrongList(vector<Word>& wordVector, const Word& editedWord, bool acc) {
    if (acc) { // ���� ������ ���
        // �����Ʈ�� ������ �ܾ �����Ѵٸ�
        vector<Word>::iterator wordIt;
        if (findInWordList(wordVector, wordIt, editedWord.eng)) {
            // ���� �� ���� ��ġ �ݿ�
            wordIt->syll = editedWord.syll;
            wordIt->acc = editedWord.acc;
        }
    }
    else { // �Ϲ� ������ ���
        // �����Ʈ�� ������ �ܾ �����Ѵٸ�
        vector<Word>::iterator wordIt;
        if (findInWordList(wordVector, wordIt, editedWord.eng)) {
            string tempKor[KOR_MAX];
            for (int i = 0; i < KOR_MAX; i++)
                tempKor[i] = "";
            // �����Ʈ�� �ִ� �ѱ� ��� ������ �ѱ� ���� ��
            int tempKorIndex = 0;
            for (int i = 0; i < KOR_MAX; i++) {
                // �����Ʈ�� �ִ� �ѱ� ���� ������ �ѱ� ��� ��ġ�ϴ� ���� ���ٸ� ����, ������ ���ܵ�
                bool exist = false;
                for (int j = 0; j < KOR_MAX; j++) {
                    if (wordIt->kor[i] == editedWord.kor[j] && wordIt->kor[i] != "") {
                        exist = true;
                        break;
                    }
                }
                if (exist)
                    tempKor[tempKorIndex++] = wordIt->kor[i];
            }

            if (tempKor[0] == "") { // �ѱ� ���� �ϳ��� �������� ������ �ܾ� ���� (��� ���� ����)
                string tempEng = wordIt->eng;
                auto removeIt = remove_if(wordVector.begin(), wordVector.end(),
                    [tempEng](const Word& word) { return (word.eng == tempEng); });
                wordVector.erase(removeIt, wordVector.end());
            }
            else {
                for (int i = 0; i < KOR_MAX; i++)
                    wordIt->kor[i] = tempKor[i];

                wordIt->cnt = tempKorIndex;
                // ���� �� ���� ��ġ�� �ݿ�
                wordIt->syll = editedWord.syll;
                wordIt->acc = editedWord.acc;
            }
        }
    }
    
}


/*
���ܾ� �߰� �� ���� �޴� edit()
*/
void edit() {
    //loadwordsfromfiles(); // ���⼭ �� �Լ��� ȣ������ �ʾƵ� ū ������ �߻����� �ʾҽ��ϴ�.
    system("cls");

    while (true) {

        string s;   // ���ܾ� �˻��Ѱ� �����Ϸ��� ����
        string userinput;

        bool exist = false;
        auto it = wordList.begin();
        cout << "���ܾ� �߰� - �߰��� �ܾ� �˻�" << endl;
        cout << "���ܾ� ���� - ������ �ܾ� �˻�" << endl;
        cout << "���ܾ� ���� - ������ �ܾ� �˻�" << endl;
        cout << "���ܾ� �Է�(����-q): ";
        getline(cin, s);
        s = lowerString(s); // s : �Է� ���� ���ܾ�

        while (!checkEng(s)) {
            cout << "�ùٸ��� ���� �Է��Դϴ�." << endl;
            cout << "���ܾ� �Է�(����-Q): ";
            getline(cin, s);
            s = lowerString(s);
        }

        if (s == "q") {      // ����
            system("cls");
            break;
        }

        // ���ܾ �����ϴ��� �˻�
        for (it = wordList.begin(); it != wordList.end(); ++it) {
            if (it->eng == s) {
                exist = true;
                break;
            }
        }
        
        system("cls");
        if (!exist) {      // ���ܾ �������� ����

            cout << "[" << s << "] ��� ���ܾ�� �������� �ʽ��ϴ�." << endl;
            cout << "[" << s << "] �ܾ �߰��Ͻðڽ��ϱ�? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);

                if (choice == "y") {
                    cout << s << "�� ���� ,�� ������ �Է��ϼ���. (�ִ� " << KOR_MAX << "�� ����): ";
                    wstring kor;
                    bool checkedKor = false;
                    wchar_t delim = L',';
                    vector<wstring> korVector;
                    // �ѱ� �� �Է�
                    while (!checkedKor) {
                        getline(wcin, kor);
                        korVector = splitTrimWString(kor, delim);

                        if (korVector.size() > KOR_MAX) {
                            cout << "�ִ� �ѱ� �� �������� �����ϴ�. �ٽ� �Է��� �ּ���." << endl;
                        }
                        else if (korVector.size() == 0) {
                            cout << "�ѱ� ���� �����ϴ�. �ٽ� �Է��� �ּ���." << endl;
                        }
                        else {
                            bool hasBadKor = false;
                            for (int i = 0; i < korVector.size(); i++) {
                                if (checkKor(korVector[i]) != 1) {
                                    cout << "�Է��� �ѱ� �� �� ���� ���Ŀ� ���� �ʴ� ���� �ֽ��ϴ�. �ٽ� �Է��� �ּ���." << endl;
                                    hasBadKor = true;
                                    break;
                                }
                            }
                            if (hasBadKor) continue;

                            // �ߺ� üũ
                            bool hasDuplicate = false;
                            for (int i = 0; i < korVector.size() - 1; i++) {
                                for (int j = i + 1; j < korVector.size(); j++) {
                                    if (korVector[i] == korVector[j]) {
                                        cout << "�Է��� �ѱ� �� �� �ߺ��Ǵ� ���� �ֽ��ϴ�. �ٽ� �Է��� �ּ���." << endl;
                                        hasDuplicate = true;
                                        break;
                                    }
                                }
                                if (hasDuplicate) break;
                            }
                            if (hasDuplicate) continue;
                            checkedKor = true;
                        }
                    }

                    string korArr[KOR_MAX];
                    for (int i = 0; i < korVector.size(); i++)
                        korArr[i] = w2s(korVector[i]);

                    // ���� �� �Է�
                    bool checkedSyll = false;
                    int syll;
                    while (!checkedSyll) {
                        cout << s << "�� ���� ���� �Է��ϼ���: ";
                        string sInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, sInput);
                        syll = input2int(sInput);

                        if (syll <= 0) {
                            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else if (syll > 30) {
                            cout << "���� ���� 30���� Ŭ �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else {
                            checkedSyll = true;
                            break;
                        }
                    }
                    // ���� ��ġ �Է�
                    bool checkedAcc = false;
                    int acc;
                    while (!checkedAcc) {
                        cout << s << "�� ���� ��ġ�� �Է��ϼ���: ";
                        string aInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, aInput);
                        acc = input2int(aInput);

                        if (acc <= 0) {
                            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else if (acc > syll) {
                            cout << "���� ��ġ�� ���� ������ Ŭ �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else {
                            checkedAcc = true;
                            break;
                        }
                    }

                    wordList.push_back(Word(s, korArr, korVector.size(), syll, acc));
                    saveWordsToFiles();

                    cout << "[" << s << " - ";
                    printStringArray(korArr, korVector.size(), false);
                    cout << " - " << syll << " - " << acc << "] �ܾ �߰��߽��ϴ�.(�޴�-Q)" << endl;

                    getline(cin, userinput);
                    userinput = lowerString(userinput);
                    while (userinput != "q")
                    {
                        system("cls");
                        cout << "[" << s << " - ";
                        printStringArray(korArr, korVector.size(), false);
                        cout << " - " << syll << " - " << acc << "] �ܾ �߰��߽��ϴ�.(�޴�-Q)" << endl;
                        cout << "Q�� �Է����ּ���." << endl;
                        getline(cin, userinput);
                        userinput = lowerString(userinput);
                    }
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    system("cls");
                    cout << "�ùٸ��� ���� �Է��Դϴ�." << endl;
                    cout << "[" << s << "] �ܾ �߰��Ͻðڽ��ϱ�? (Y/N)" << endl;
                }
            }
        }
        else {   // ���ܾ ����
            cout << "���� �ܾ��忡 �ִ� �ܾ�: " << endl;
            cout << "[" << it->eng << " - ";
            printStringArray(it->kor, it->cnt, false);
            cout << " - " << it->syll << " - " << it->acc << "]" << endl;

            cout << "�ش� �ܾ ����, �Ǵ� �����Ͻðڽ��ϱ�? (D/E/Q)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "d") {

                    string tmpKor[KOR_MAX];
                    for (int i = 0; i < KOR_MAX; i++)
                        tmpKor[i] = it->kor[i];
                    int tmpCnt = it->cnt;
                    int tmpSyll = it->syll;
                    int tmpAcc = it->acc;

                    wordList.erase(it);
                    saveWordsToFiles();

                    cout << "[" << s << " - ";
                    printStringArray(tmpKor, tmpCnt, false);
                    cout << " - " << tmpSyll << " - " << tmpAcc << "] �ܾ �����Ǿ����ϴ�.(�޴�-Q)" << endl;
                    getline(cin, userinput);
                    userinput = lowerString(userinput);
                    while (userinput != "q")
                    {
                        system("cls");
                        cout << "[" << s << " - ";
                        printStringArray(tmpKor, tmpCnt, false);
                        cout << " - " << tmpSyll << " - " << tmpAcc << "] �ܾ �����Ǿ����ϴ�.(�޴�-Q)" << endl;
                        cout << "Q�� �Է����ּ���." << endl;
                        getline(cin, userinput);
                        userinput = lowerString(userinput);
                    }
                    break;
                }
                else if (choice == "e") {

                    cout << s << "�� ���ο� ���� ,�� ������ �Է��ϼ���. (�ִ� " << KOR_MAX << "�� ����): ";
                    wstring kor;
                    bool checkedKor = false;
                    wchar_t delim = L',';
                    vector<wstring> korVector;
                    // �ѱ� �� �Է�
                    while (!checkedKor) {
                        getline(wcin, kor);
                        korVector = splitTrimWString(kor, delim);

                        if (korVector.size() > KOR_MAX) {
                            cout << "�ִ� �ѱ� �� �������� �����ϴ�. �ٽ� �Է��� �ּ���." << endl;
                        }
                        else if (korVector.size() == 0) {
                            cout << "�ѱ� ���� �����ϴ�. �ٽ� �Է��� �ּ���." << endl;
                        }
                        else {
                            bool hasBadKor = false;
                            for (int i = 0; i < korVector.size(); i++) {
                                if (checkKor(korVector[i]) != 1) {
                                    cout << "�Է��� �ѱ� �� �� ���� ���Ŀ� ���� �ʴ� ���� �ֽ��ϴ�. �ٽ� �Է��� �ּ���." << endl;
                                    hasBadKor = true;
                                    break;
                                }
                            }
                            if (hasBadKor) continue;

                            // �ߺ� üũ
                            bool hasDuplicate = false;
                            for (int i = 0; i < korVector.size() - 1; i++) {
                                for (int j = i + 1; j < korVector.size(); j++) {
                                    if (korVector[i] == korVector[j]) {
                                        cout << "�Է��� �ѱ� �� �� �ߺ��Ǵ� ���� �ֽ��ϴ�. �ٽ� �Է��� �ּ���." << endl;
                                        hasDuplicate = true;
                                        break;
                                    }
                                }
                                if (hasDuplicate) break;
                            }
                            if (hasDuplicate) continue;
                            checkedKor = true;
                        }
                    }

                    string korArr[KOR_MAX];
                    for (int i = 0; i < korVector.size(); i++)
                        korArr[i] = w2s(korVector[i]);

                    // ���� �� �Է�
                    bool checkedSyll = false;
                    int syll;
                    while (!checkedSyll) {
                        cout << s << "�� ���� ���� �Է��ϼ���: ";
                        string sInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, sInput);
                        syll = input2int(sInput);

                        if (syll <= 0) {
                            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else if (syll > 30) {
                            cout << "���� ���� 30���� Ŭ �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else {
                            checkedSyll = true;
                            break;
                        }
                    }
                    // ���� ��ġ �Է�
                    bool checkedAcc = false;
                    int acc;
                    while (!checkedAcc) {
                        cout << s << "�� ���� ��ġ�� �Է��ϼ���: ";
                        string aInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, aInput);
                        acc = input2int(aInput);

                        if (acc <= 0) {
                            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else if (acc > syll) {
                            cout << "���� ��ġ�� ���� ������ Ŭ �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
                        }
                        else {
                            checkedAcc = true;
                            break;
                        }
                    }

                    // ���� ����
                    for (int i = 0; i < KOR_MAX; i++) {
                        it->kor[i] = korArr[i];
                    }
                    it->cnt = korVector.size();
                    it->syll = syll;
                    it->acc = acc;

                    // �����Ʈ���� ����
                    editWrongList(wrongWordList, *it, false); 
                    // ���� ��Ͽ��� ����
                    editWrongList(wrongWordHistoryList, *it, false);
                    // ���� ���信�� ����
                    editWrongList(wrongAccList, *it, true);
                    // ���� ���� ��Ͽ��� ����
                    editWrongList(wrongAccHistoryList, *it, true);

                    saveWordsToFiles();

                    cout << "[" << s << " - ";
                    printStringArray(korArr, korVector.size(), false);
                    cout << " - " << syll << " - " << acc << "] �ܾ�� �����Ǿ����ϴ�.(�޴�-Q)" << endl;

                    getline(cin, userinput);
                    userinput = lowerString(userinput);
                    while (userinput != "q")
                    {
                        system("cls");
                        cout << "[" << s << " - ";
                        printStringArray(korArr, korVector.size(), false);
                        cout << " - " << syll << " - " << acc << "] �ܾ�� �����Ǿ����ϴ�.(�޴�-Q)" << endl;
                        cout << "Q�� �Է����ּ���." << endl;
                        getline(cin, userinput);
                        userinput = lowerString(userinput);
                    }
                    break;
                }
                else if (choice == "q") {
                    break;
                }
                else {
                    cout << "�ٽ� �Է����ּ���." << endl;
                }
            }
        }
        system("cls");
    }

    mainFirst = 0;
    return;
}


/*
��ü ���ܾ ���� ����
*/
void totalQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // ���� ��� Ȯ�� ����
    int goalCount; // ����� �Ϸ��� �ܾ� ��

    int right = 0; // ���� ���� ��
    int wrong = 0; // Ʋ�� ���� ��

    if (wordList.empty()) {
        cout << "�ܾ �����ϴ�. ���� �ܾ �߰��ϼ���." << endl;
        cout << "���θ޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "�ܾ �����ϴ�. ���� �ܾ �߰��ϼ���." << endl;
            cout << "���θ޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    vector<QuizWord> quizWordVector;

    // �� ���ܾ�� �� �ܾ��� ���� �ѱ� ���� �и��ؼ� �߰�
    for (int i = 0; i < wordList.size(); i++) {
        Word tempWord = wordList[i];
        for (int j = 0; j < tempWord.cnt; j++) {
            if (tempWord.kor[j].length() > 0)
                quizWordVector.push_back(QuizWord(tempWord.eng, tempWord.kor[j], tempWord.syll, tempWord.acc));
        }
    }

    while (true) {
        cout << "���� ���� ���� �Է��ϼ��� (���� �޴��� ���ư����� 0 �Է�): ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);

        goalCount = input2int(quizipt);

        if (goalCount < 0) {
            system("cls");
            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
            continue;
        }
        else if (goalCount == 0) {
            system("cls");
            return;
        }
        else if (goalCount <= quizWordVector.size())
            break;
        else
        {
            system("cls");
            cout << "������ ���� �� (" << quizWordVector.size() << ") ���� ���� ���� ���� �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
        }
    }

    // quizWordVector�� ��� �ε����� ������ ������ �����ϰ� �����
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);

    for (int i = 0; i < goalCount; i++) {
        system("cls"); // ȭ�� ����� (Windows)

        QuizWord randomQuizWord = quizWordVector[i];
        cout << "���� " << (i + 1) << "." << endl;
        cout << "�ѱ� ��: " << randomQuizWord.kor << endl;
        cout << "���� ��: " << randomQuizWord.syll << endl;
        cout << "���� ��ġ: " << randomQuizWord.acc << endl;
        cout << "���� �ܾ �Է��ϼ��� (�޴��� ���ư����� 'Q' �Է�): ";
        string userInput;
        getline(cin, userInput);
        while (!checkEng(userInput)) {
            cout << "��Ģ�� ����Ǵ� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
            getline(cin, userInput);
        }
        userInput = lowerString(userInput);

        if (userInput == "q") {
            qCheck = true;
            system("cls");
            break;
        }

        if (userInput == lowerString(randomQuizWord.eng)) {
            cout << "����!" << endl;
            right++;
            cout << "���������� - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "����!" << endl;
                cout << "���� ������ - Q" << endl;
                cout << "Q�� �Է����ּ���." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
        else {
            cout << "Ʋ�Ƚ��ϴ�." << endl;
            wrong++;
            cout << "Ʋ�� �ܾ��� ����: " << randomQuizWord.eng << endl;

            cout << "�ش� �ܾ �����Ʈ�� �߰� �Ͻðڽ��ϱ�? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    pushWrongQuizWord(randomQuizWord); // ���� ��Ʈ�� �߰� (+ �� �����)
                    saveWordsToFiles();   // ��� txt ����
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "�ٽ� �Է����ּ���." << endl;
                }
            }

            cout << "���� ������ - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "Ʋ�Ƚ��ϴ�." << endl;
                cout << "Ʋ�� �ܾ��� ����: " << randomQuizWord.eng << endl;
                cout << "���� ������ - Q" << endl;
                cout << "Q�� �Է����ּ���." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
    }
    if (!qCheck) {
        system("cls");
        cout << "���� ����" << endl;
        cout << "��ü ���� ��: " << goalCount << endl;
        cout << "���� ���� ��: " << right << endl;
        cout << "Ʋ�� ���� ��: " << wrong << endl;
        cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "�޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "���� ����" << endl;
            cout << "��ü ���� ��: " << goalCount << endl;
            cout << "���� ���� ��: " << right << endl;
            cout << "Ʋ�� ���� ��: " << wrong << endl;
            cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "�޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");

    }

    return;
}
/*
���� ���ܾ ���� ����
*/
void wrongQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // �߰��� ��� ����ߴ��� Ȯ��
    int goalCount; // ����� �Ϸ��� �ܾ� ��

    int right = 0; // ���� ���� ��
    int wrong = 0; // Ʋ�� ���� ��

    if (wrongWordList.empty()) {
        cout << "���� �����Ʈ�� �ܾ �����ϴ�." << endl;
        cout << "���θ޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "���� �����Ʈ�� �ܾ �����ϴ�." << endl;
            cout << "���θ޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    vector<QuizWord> quizWordVector;

    // �� ���ܾ�� �� �ܾ��� ���� �ѱ� ���� �и��ؼ� �߰�
    for (int i = 0; i < wrongWordList.size(); i++) {
        Word tempWord = wrongWordList[i];
        vector<Word>::iterator wordIt;
        int tempSyll = -1;
        int tempAcc = -1;
        if (findInWordList(wordList, wordIt, tempWord.eng)) { // Word.txt�� ���ܾ ������ ���� ��, ���� ��ġ ���
            tempSyll = wordIt->syll;
            tempAcc = wordIt->acc;
        }
        for (int j = 0; j < tempWord.cnt; j++) {
            if (tempWord.kor[j].length() > 0) {
                quizWordVector.push_back(QuizWord(tempWord.eng, tempWord.kor[j], tempSyll, tempAcc));
            }
                
        }
    }

    while (true) {
        cout << "���� ���� ���� �Է��ϼ��� (���� �޴��� ���ư����� 0 �Է�): ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);
        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
        }
        else if (goalCount == 0) {
            system("cls");
            return;
        }
        else if (goalCount <= quizWordVector.size())
            break;
        else
        {
            system("cls");
            cout << "������ ���� �� (" << quizWordVector.size() << ") ���� ���� ���� ���� �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
        }
    }


    // quizWordVector�� ��� �ܾ ������ ������ �����ϰ� �����
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);


    for (int i = 0; i < goalCount; i++) {
        system("cls"); // ȭ�� ����� (Windows)

        QuizWord randomQuizWord = quizWordVector[i];
        cout << "���� " << (i + 1) << "." << endl;
        cout << "�ѱ� ��: " << randomQuizWord.kor << endl;
        // Word.txt�� ���ܾ �ִ°��� ���� ���� ���� ���� ��ġ ����� �޶���
        if(randomQuizWord.syll < 0)
            cout << "���� ��: " << "?" << endl;
        else
            cout << "���� ��: " << randomQuizWord.syll << endl;
        if(randomQuizWord.acc < 0)
            cout << "���� ��ġ: " << "?" << endl;
        else
            cout << "���� ��ġ: " << randomQuizWord.acc << endl;
        cout << "���� �ܾ �Է��ϼ��� (�޴��� ���ư����� 'Q' �Է�): ";

        string userInput;
        getline(cin, userInput);
        while (!checkEng(userInput)) {
            cout << "��Ģ�� ����Ǵ� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
            getline(cin, userInput);
        }
        userInput = lowerString(userInput);

        if (userInput == "q") {
            qCheck = true;
            system("cls");
            break;
        }

        if (userInput == lowerString(randomQuizWord.eng)) {
            cout << "����!" << endl;
            right++;
            cout << "�ش� �ܾ �����Ʈ���� �����Ͻðڽ��ϱ�? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    string removeEng = randomQuizWord.eng;
                    string removeKor = randomQuizWord.kor;

                    vector<Word>::iterator wordIt;
                    if (findInWordList(wrongWordList, wordIt, removeEng)) {
                        string korArr[KOR_MAX];
                        int korArrIndex = 0;

                        for (int i = 0; i < KOR_MAX; i++)
                            korArr[i] = "";
                        for (int i = 0; i < KOR_MAX; i++) {
                            if (wordIt->kor[i] != removeKor && wordIt->kor[i] != "") { // ������ �ѱ� ���� �����ϰ� ������ �ѱ� ���� korArr�� �߰�
                                korArr[korArrIndex++] = wordIt->kor[i];
                            }
                        }
                        bool exist = false; // ���� ���ܾ �ѱ� ���� �� �� �̻� ����
                        for (int i = 0; i < KOR_MAX; i++) {
                            if (korArr[i] != "") {
                                exist = true;
                            }
                        }
                        if (!exist) { // ���� ���ܾ �ѱ� ���� �� �̻� ������ �����Ʈ���� ����
                            auto removeIt = remove_if(wrongWordList.begin(), wrongWordList.end(),
                                [removeEng](const Word& word) { return (word.eng == removeEng); });
                            wrongWordList.erase(removeIt, wrongWordList.end());
                        }
                        else { // ���� ���ܾ��� ���� �ѱ� �� ����
                            for (int i = 0; i < KOR_MAX; i++) {
                                wordIt->kor[i] = korArr[i];
                            }
                            wordIt->cnt = korArrIndex;
                        }
                    }

                    saveWordsToFiles();                       // Word ��ü ��ȯ �� ��� txt ����
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "�ٽ� �Է����ּ���." << endl;
                }
            }
        }
        else {
            cout << "Ʋ�Ƚ��ϴ�." << endl;
            wrong++;
            cout << "Ʋ�� �ܾ��� ����: " << randomQuizWord.eng << endl; // �����Ʈ �����̹Ƿ� ���� ���� X
            cout << "���� ������ - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "Ʋ�Ƚ��ϴ�." << endl;
                cout << "Ʋ�� �ܾ��� ����: " << randomQuizWord.eng << endl;
                cout << "���� ������ - Q" << endl;
                cout << "Q�� �Է����ּ���." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }

        }
    }

    if (!qCheck) {
        system("cls");
        cout << "���� ����" << endl;
        cout << "��ü ���� ��: " << goalCount << endl;
        cout << "���� ���� ��: " << right << endl;
        cout << "Ʋ�� ���� ��: " << wrong << endl;
        cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "�޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "���� ����" << endl;
            cout << "��ü ���� ��: " << goalCount << endl;
            cout << "���� ���� ��: " << right << endl;
            cout << "Ʋ�� ���� ��: " << wrong << endl;
            cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "�޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);

        }
        system("cls");
    }
    return;
}

/*
��ü ���ܾ ���� ���� ����
*/
void totalAccentQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // ���� ��� Ȯ�� ����
    int goalCount; // ����� �Ϸ��� �ܾ� ��

    int right = 0; // ���� ���� ��
    int wrong = 0; // Ʋ�� ���� ��

    if (wordList.empty()) {
        cout << "�ܾ �����ϴ�. ���� �ܾ �߰��ϼ���." << endl;
        cout << "���θ޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "�ܾ �����ϴ�. ���� �ܾ �߰��ϼ���." << endl;
            cout << "���θ޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    vector<Word> quizWordVector;

    // �� ���ܾ�� �� �ܾ��� ���� �ѱ� ���� �и��ؼ� �߰�
    for (int i = 0; i < wordList.size(); i++) {
        Word tempWord = wordList[i];
        string tempKor[KOR_MAX];
        for (int i = 0; i < KOR_MAX; i++)
            tempKor[i] = tempWord.kor[i];
        quizWordVector.push_back(Word(tempWord.eng, tempKor, tempWord.cnt, tempWord.syll, tempWord.acc));
    }

    while (true) {
        cout << "���� ���� ���� �Է��ϼ��� (���� �޴��� ���ư����� 0 �Է�): ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);

        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
            continue;
        }
        else if (goalCount == 0) {
            system("cls");
            return;
        }
        else if (goalCount <= quizWordVector.size())
            break;
        else
        {
            system("cls");
            cout << "����� �ܾ� �� (" << quizWordVector.size() << ") ���� ���� ���� ���� �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
        }
    }

    // quizWordVector�� ��� �ε����� ������ ������ �����ϰ� �����
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);

    for (int i = 0; i < goalCount; i++) {
        system("cls"); // ȭ�� ����� (Windows)

        Word randomWord = quizWordVector[i];
        cout << "���� " << (i + 1) << "." << endl;
        cout << "���ܾ�: " << randomWord.eng << endl;
        cout << "�ѱ� ��: ";
        printStringArray(randomWord.kor, randomWord.cnt, false);
        cout << endl;
        cout << "���� ��: " << randomWord.syll << endl;
        cout << "���� ��ġ�� �Է��ϼ��� (�޴��� ���ư����� 'Q' �Է�): ";
        string userInput;
        int userInputInt = 0;

        while (!qCheck && (userInputInt <= 0 || userInputInt > randomWord.syll)) {
            getline(cin, userInput);
            userInput = lowerString(userInput);
            if (userInput == "q") {
                qCheck = true;
                system("cls");
                break;
            }

            userInputInt = input2int(userInput);
            if (userInputInt <= 0 || userInputInt > randomWord.syll)
                cout << "��Ģ�� ����Ǵ� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
        }

        if (qCheck) break;

        if (userInputInt == randomWord.acc) {
            cout << "����!" << endl;
            right++;
            cout << "���������� - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "����!" << endl;
                cout << "���� ������ - Q" << endl;
                cout << "Q�� �Է����ּ���." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
        else {
            cout << "Ʋ�Ƚ��ϴ�." << endl;
            wrong++;
            cout << "Ʋ�� �ܾ��� ���� ��ġ ����: " << randomWord.acc << endl;

            cout << "�ش� �ܾ �����Ʈ�� �߰� �Ͻðڽ��ϱ�? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    pushWrongAccentQuizWord(randomWord); // ���� ��Ʈ�� �߰� (+ �� �����)
                    saveWordsToFiles();   // ��� txt ����
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "�ٽ� �Է����ּ���." << endl;
                }
            }

            cout << "���� ������ - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "Ʋ�Ƚ��ϴ�." << endl;
                cout << "Ʋ�� �ܾ��� ���� ��ġ ����: " << randomWord.acc << endl;
                cout << "���� ������ - Q" << endl;
                cout << "Q�� �Է����ּ���." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
    }
    if (!qCheck) {
        system("cls");
        cout << "���� ����" << endl;
        cout << "��ü ���� ��: " << goalCount << endl;
        cout << "���� ���� ��: " << right << endl;
        cout << "Ʋ�� ���� ��: " << wrong << endl;
        cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "�޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "���� ����" << endl;
            cout << "��ü ���� ��: " << goalCount << endl;
            cout << "���� ���� ��: " << right << endl;
            cout << "Ʋ�� ���� ��: " << wrong << endl;
            cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "�޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");

    }

    return;
}

/*
���� ���ܾ ���� ���� ����
*/
void wrongAccentQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // ���� ��� Ȯ�� ����
    int goalCount; // ����� �Ϸ��� �ܾ� ��

    int right = 0; // ���� ���� ��
    int wrong = 0; // Ʋ�� ���� ��

    if (wrongAccList.empty()) {
        cout << "���� �����Ʈ�� �ܾ �����ϴ�." << endl;
        cout << "���θ޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "���� �����Ʈ�� �ܾ �����ϴ�." << endl;
            cout << "���θ޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    while (true) {
        cout << "���� ���� ���� �Է��ϼ��� (���� �޴��� ���ư����� 0 �Է�): ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);

        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 �̻��� �������� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���." << endl;
            continue;
        }
        else if (goalCount == 0) {
            system("cls");
            return;
        }
        else if (goalCount <= wrongAccList.size())
            break;
        else
        {
            system("cls");
            cout << "�����Ʈ�� �ִ� �ܾ� �� (" << wrongAccList.size() << ") ���� ���� ���� ���� �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
        }
    }

    vector<Word> quizWordVector;

    // �� ���ܾ�� �� �ܾ��� ���� �ѱ� ���� �и��ؼ� �߰�
    for (int i = 0; i < wrongAccList.size(); i++) {
        Word tempWord = wrongAccList[i];
        string tempKor[KOR_MAX];
        for (int i = 0; i < KOR_MAX; i++)
            tempKor[i] = tempWord.kor[i];
        quizWordVector.push_back(Word(tempWord.eng, tempKor, tempWord.cnt, tempWord.syll, tempWord.acc));
    }

    // quizWordVector�� ��� �ε����� ������ ������ �����ϰ� �����
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);

    for (int i = 0; i < goalCount; i++) {
        system("cls"); // ȭ�� ����� (Windows)

        Word randomWord = quizWordVector[i];
        cout << "���� " << (i + 1) << "." << endl;
        cout << "���ܾ�: " << randomWord.eng << endl;
        // Word.txt�� ���ܾ �ִ°��� ���� �ѱ� �� ����� �޶���
        vector<Word>::iterator wordIt;
        cout << "�ѱ� ��: ";
        if (findInWordList(wordList, wordIt, randomWord.eng))
            printStringArray(wordIt->kor, wordIt->cnt, false);
        else
            cout << "?";
        cout << endl;
        cout << "���� ��: " << randomWord.syll << endl;
        cout << "���� ��ġ�� �Է��ϼ��� (�޴��� ���ư����� 'Q' �Է�): ";
        string userInput;
        int userInputInt = 0;

        while (!qCheck && (userInputInt <= 0 || userInputInt > randomWord.syll)) {
            getline(cin, userInput);
            userInput = lowerString(userInput);
            if (userInput == "q") {
                qCheck = true;
                system("cls");
                break;
            }

            userInputInt = input2int(userInput);
            if (userInputInt <= 0 || userInputInt > randomWord.syll)
                cout << "��Ģ�� ����Ǵ� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
        }

        if (qCheck) break;

        if (userInputInt == randomWord.acc) {
            cout << "����!" << endl;
            right++;

            cout << "�ش� �ܾ �����Ʈ���� �����Ͻðڽ��ϱ�? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    string removeEng = randomWord.eng;
                    auto removeIt = remove_if(wrongAccList.begin(), wrongAccList.end(),
                        [removeEng](const Word& word) { return word.eng == removeEng; });

                    wrongAccList.erase(removeIt, wrongAccList.end()); // ���� ��Ʈ���� ����

                    saveWordsToFiles();                       // Word ��ü ��ȯ �� ��� txt ����
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "�ٽ� �Է����ּ���." << endl;
                }
            }
        }
        else {
            cout << "Ʋ�Ƚ��ϴ�." << endl;
            wrong++;
            cout << "Ʋ�� �ܾ��� ���� ��ġ ����: " << randomWord.acc << endl;

            cout << "���� ������ - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "Ʋ�Ƚ��ϴ�." << endl;
                cout << "Ʋ�� �ܾ��� ���� ��ġ ����: " << randomWord.acc << endl;
                cout << "���� ������ - Q" << endl;
                cout << "Q�� �Է����ּ���." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
    }
    if (!qCheck) {
        system("cls");
        cout << "���� ����" << endl;
        cout << "��ü ���� ��: " << goalCount << endl;
        cout << "���� ���� ��: " << right << endl;
        cout << "Ʋ�� ���� ��: " << wrong << endl;
        cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "�޴��� �̵�-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "���� ����" << endl;
            cout << "��ü ���� ��: " << goalCount << endl;
            cout << "���� ���� ��: " << right << endl;
            cout << "Ʋ�� ���� ��: " << wrong << endl;
            cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "�޴��� �̵�-Q" << endl;
            cout << "Q�� �Է����ּ���" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");

    }

    return;
}

/*
���� �޴� ��� �� ���ÿ� �Լ� quiz()
*/
void quiz() {
    system("cls");
    int menuSelect;

    while (1) {

        cout << "1. ��ü �ܾ� ����     2. �����Ʈ ����     3. ��ü ���� ����     4. ���� ���� ����\n";
        cout << "���� �޴��� �������� 0�� �Է����ּ���.\n";

        string mainipt;
        getline(cin, mainipt);
        menuSelect = input2int(mainipt);


        if (menuSelect == 1) {
            totalQuiz();
            mainFirst = 0;
            return;
        }
        else if (menuSelect == 2) {
            wrongQuiz();
            mainFirst = 0;
            return;
        }
        else if (menuSelect == 3) {
            totalAccentQuiz();
            mainFirst = 0;
            return;
        }
        else if (menuSelect == 4) {
            wrongAccentQuiz();
            mainFirst = 0;
            return;
        }
        else if (menuSelect == 0) {
            system("cls");
            mainFirst = 0;
            return;
        }
        else {
            system("cls");
            cout << "�ùٸ��� ���� �Է��Դϴ�." << endl;
        }
    }
}


/*
�����Ʈ ��ȸ �Լ� viewWrongList()
@param wrongList wrongWordList, wrongWordHistoryList, wrongAccList, wrongAccHistoryList
@param isHistory �����Ʈ�� false, �����Ʈ ����̸� true
@param isAcc �Ϲ� �����̸� false, ���� �����̸� true
*/
void viewWrongList(const vector<Word>& wrongList, bool isHistory, bool isAcc) {
    system("cls");
    auto it = wrongList.begin();
    string userinput;

    if (wrongList.empty()) {
        if (isHistory)
            cout << "���� �� ���̶� �����Ʈ�� ���� �ܾ �����ϴ�." << endl;
        else
            cout << "���� �����Ʈ�� �ܾ �����ϴ�." << endl;
    }
    else {
        // �ܾ� ��� ���
        cout << "[���ܾ� - �ѱ� �� - ���� �� - ���� ��ġ]" << endl;
        if (isAcc) { // ���� �����Ʈ
            for (it = wrongList.begin(); it != wrongList.end(); ++it) {
                vector<Word>::iterator wordIt;
                if (findInWordList(wordList, wordIt, it->eng)) { // Word.txt�� ���ܾ �ִ� ���, �� ���Ͽ��� �����ͼ� �ѱ� �� ���
                    cout << "[" << it->eng << " - ";
                    printStringArray(wordIt->kor, wordIt->cnt, false);
                    cout << " - " << it->syll << " - " << it->acc << "]" << endl;
                }
                else { // Word.txt�� ���ܾ ���� ���, �ѱ� ���� ?�� ���
                    cout << "[" << it->eng << " - ";
                    cout << "?";
                    cout << " - " << it->syll << " - " << it->acc << "]" << endl;
                }

            }
        }
        else { // �Ϲ� �����Ʈ
            for (it = wrongList.begin(); it != wrongList.end(); ++it) {
                vector<Word>::iterator wordIt;
                if (findInWordList(wordList, wordIt, it->eng)) { // Word.txt�� ���ܾ �ִ� ���, �� ���Ͽ��� �����ͼ� ���� ��, ���� ��ġ ���
                    cout << "[" << it->eng << " - ";
                    printStringArray(it->kor, it->cnt, false);
                    cout << " - " << wordIt->syll << " - " << wordIt->acc << "]" << endl;
                }
                else { // Word.txt�� ���ܾ ���� ���, ���� ���� ���� ��ġ�� ?�� ���
                    cout << "[" << it->eng << " - ";
                    printStringArray(it->kor, it->cnt, false);
                    cout << " - " << "?" << " - " << "?" << "]" << endl;
                }

            }
        }
    }

    cout << "��ȸ ������ - Q" << endl;
    getline(cin, userinput);
    userinput = lowerString(userinput);
    while (userinput != "q")
    {
        system("cls");
        cout << "��ȸ ������ - Q" << std::endl;
        cout << "Q�� �Է����ּ���" << std::endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
    }
    system("cls");
}

/*
����ܾ� ��ȸ �޴� viewWrong()
*/
void viewWrong() {

    int menuSelect;
    system("cls");

    while (1) {
        if (viewFirst == 0)
        {
            cout << "1. �����Ʈ �ܾ� ��ȸ     2. �����Ʈ ��� ��ȸ     3. ���� �����Ʈ �ܾ� ��ȸ     4. ���� �����Ʈ ��� ��ȸ\n";
            cout << "���� �޴��� �������� 0�� �Է����ּ���.\n";
            viewFirst++;
        }

        string viewipt;
        getline(cin, viewipt);
        menuSelect = input2int(viewipt);

        if (menuSelect == 1) {
            viewWrongList(wrongWordList, false, false);
            viewFirst = 0;
        }
        else if (menuSelect == 2) {
            viewWrongList(wrongWordHistoryList, true, false);
            viewFirst = 0;
        }
        else if (menuSelect == 3) {
            viewWrongList(wrongAccList, false, true);
            viewFirst = 0;
        }
        else if (menuSelect == 4) {
            viewWrongList(wrongAccHistoryList, true, true);
            viewFirst = 0;
        }
        else if (menuSelect == 0) {
            system("cls");
            mainFirst = 0;
            viewFirst = 0;
            return;
        }
        else {
            system("cls");
            cout << "1. �����Ʈ �ܾ� ��ȸ     2. �����Ʈ ��� ��ȸ     3. ���� �����Ʈ �ܾ� ��ȸ     4. ���� �����Ʈ ��� ��ȸ\n";
            cout << "���� �޴��� �������� 0�� �Է����ּ���.\n";
            cout << "�ùٸ��� ���� �Է��Դϴ�." << endl;
        }

    }
}