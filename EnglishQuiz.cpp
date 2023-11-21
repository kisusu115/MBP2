#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <locale>
#include <conio.h> // _getch()
#include <sstream>

#define KOR_MAX 5 // 한글 뜻 최대 5개 제한

using namespace std;

//함수 헤더선언
void edit();
void totalQuiz();
void wrongQuiz();
void quiz();
void viewWrong();
bool checkEng(string& str);
bool checkKor(wstring& str);

class Word { // 한글 뜻, 영어단어, 음절, 강세를 가진 Class Word
public:
    string eng;
    string kor[KOR_MAX]; // 한글 뜻 배열
    int cnt = 0; // 한글 뜻 개수
    int syll; // 음절 수
    int acc; // 강세 위치

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
Word끼리 같은 지 체크하는 메소드
@param word 체크하는 Word
*/
bool Word::equals(Word word) {
    if (this->eng == word.eng && this->cnt == word.cnt && this->syll == word.syll && this->acc == word.acc) { // 기본 필드 체크
        bool exist;
        for (int i = 0; i < this->cnt; i++) { // 한글 뜻이 서로 모두 같은 것이 있는지 체크
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

class QuizWord { // Class Word를 Quiz에서 다루기 쉽도록 만든 Class QuizWord 
public:
    string eng;
    string kor; // 한글 뜻 1개
    int syll; // 음절
    int acc; // 강세 위치

    QuizWord(const string& eng, const string& kor, int s, int a) : eng(eng), kor(kor), syll(s), acc(a) {}
};

vector<Word> wordList; // Word 객체 저장 vector인 wordList
vector<Word> wrongWordList; // 오답 Word 객체 저장 vector인 wrongWordList
vector<Word> wrongWordHistoryList; // 오답노트에 한 번이라도 들어간 Word 객체 저장 vector인 wrongWordHistoryList
vector<Word> wrongAccList; // 강세 오답 Word 객체 저장 vector인 wrongAccList
vector<Word> wrongAccHistoryList; // 강세 오답노트에 한 번이라도 들어간 Word 객체 저장 vector 인 wrongAccHistoryList

// 각종 유틸리티 함수들
/*
narrow character string (std::string)을 wide character string (std::wstring)으로 변환하여 한글 뜻을 검사하는 checkKor(wstring& str)에 사용
*/
wstring s2w(const string& str)
{
    static locale loc("");
    auto& facet = use_facet<codecvt<wchar_t, char, mbstate_t>>(loc);
    return wstring_convert<remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(str);
}

/*
wide character string (std::wstring)을 narrow character string (std::string)으로 변환하여 Word Class에서 다루기 쉽게 함
*/
string w2s(const wstring& wstr)
{
    static locale loc("");
    auto& facet = use_facet<codecvt<wchar_t, char, mbstate_t>>(loc);
    return wstring_convert<remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstr);
}

/*
영문자열이 담긴 string 전체를 lowercase로 변환 (string끼리 비교하기 쉽도록)
@param str 영문자열가 담긴 narrow character string (std::string)
@return 모든 대문자가 소문자로 변환된 영문자열이 담긴 narrow character string (std::string)
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
입력값으로 들어온 문자열의 구성요소가 모두 숫자인지 판별하여, stoi적용 가능 시 적용한 해당 정수를 반환하고, 적용 불가능 시 -1을 반환함
EX) str가 "123"이나 "  123 "이면 123리턴, str이 "a357"이나 "Apple"이면 -1 리턴
@param str 인자로 받는 string 보통 메뉴에서의 입력값이나 퀴즈 문제 풀이에서 문제수를 설정하기위해 입력 받았던 string이 들어감
@return 입력받은 string을 정수로 변환, 정수로 변환 불가능한 문자열은 걸러내어 -1 리턴
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
영어단어가 문법 형식에 올바른지 확인하는 함수
@param str 영어단어가 담긴 narrow character string (std::string)
@return 조건을 충족하면 true, 그렇지 않으면 false
*/
bool checkEng(string& str) {

    str = lowerString(str);

    if (str.size() > 30) {
        return 0;            //문자열 길이 최대 30 제한
    }

    bool hasEng = false;      //알파벳 하나없이 공백이랑 '-'만 있는지 체크하기위해
    for (char c : str) {
        if (!((c >= 'a' && c <= 'z') || (c == ' ') || (c == '-'))) {
            return 0;
        }
        if (c >= 'a' && c <= 'z') {
            hasEng = true;      //알파벳 최소 1개라도 발견하면 hasEng를 true로 변환
        }
    }

    if (!hasEng) return 0;      //알파벳 하나도 없으면 영단어 조건 만족하지 못하므로

    return 1;  // 영어 알파벳 조건을 만족시킴
}

/*
한글 뜻이 문법 형식에 올바른지 확인하는 함수
@param str 한글 뜻이 담긴 wide character string (std::wstring)
@return 조건을 충족하면 true, 그렇지 않으면 false
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
문자열 배열을 일렬로 ','로 구분하여 출력
@param arr 문자열 배열
@param newline 줄바꿈 여부
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
하나의 wstring을 분리하여 벡터로 반환하는 함수
@param input 분리할 wstring
@param delimiter 구분 문자
@return 분리된 wstring들을 요소로 가진 벡터
*/
vector<wstring> splitTrimWString(const wstring& input, wchar_t delimiter) {
    vector<wstring> tokens;
    wstringstream ss(input);
    wstring token;

    while (getline(ss, token, delimiter)) {
        // 양쪽 공백 제거
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
wordList가 비어있는지 검사하는 함수
@return wordList가 비어있으면 true, 요소가 한 개 이상이면 false

bool checkWordListEmpty() {
   if (wordList.empty()) {
      cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
      cin.ignore(); // 버퍼 비우기
      return true;
   }
   return false;
}
*/

/*
txt파일에서 string을 읽어와서 Word 객체로 변환 후 저장
@param fileName 읽을 txt파일
@param wordVector 읽은 string을 Word 객체로 변환 후 저장할 vector
@param checkDuplicate 같은 영단어가 있는지 체크 여부
*/
void loadWordsFromFile(const string& fileName, vector<Word>& wordVector, bool checkDuplicate) {
    ifstream file(fileName);
    if (file.is_open()) {
        wordVector.clear(); // 기존 wordVector 비우기
        string line;
        while (getline(file, line)) {
            size_t pos = line.find("/");

            if (pos == string::npos)
            {
                cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
                exit(0);
            }
            else {
                string eng = line.substr(0, pos);
                if (checkEng(eng) == 0) {
                    cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");

                // 음절 읽어오기
                string s = line.substr(0, pos);
                int syll = input2int(s);
                if (syll == -1) {
                    cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
                    exit(0);
                }

                line = line.substr(pos + 1);
                pos = line.find("/");

                // 강세 위치 읽어오기
                string a = line.substr(0, pos);
                int acc = input2int(a);
                if (acc == -1) {
                    cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
                    exit(0);
                }

                // 한글 뜻 읽어오기
                line = line.substr(pos + 1);
                pos = line.find(",");
                string kor[KOR_MAX];
                int cnt = 0;

                while (pos != string::npos) {
                    string ko = line.substr(0, pos);
                    wstring k = s2w(ko);
                    if (checkKor(k) == 0) {
                        cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
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
                cnt++; // 길이 = 마지막 인덱스 + 1

                wordVector.push_back(Word(eng, kor, cnt, syll, acc));
            }
        }
        file.close();

        // 겹치는 영어단어가 있는지 체크
        if (checkDuplicate) {
            if (!wordVector.empty()) { // wordVector가 비어있을 때 무한 루프 해결
                for (int i = 0; i < wordVector.size() - 1; i++) {
                    for (int j = i + 1; j < wordVector.size(); j++) {
                        if (wordVector[i].eng == wordVector[j].eng) {
                            cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
                            exit(0);
                        }
                    }
                }
            }
        }

    }
    else {
        cerr << "Error: " << fileName << "를 불러올 수 없습니다." << endl;
        return;
    }
}

/*
Word.txt, Wrong.txt, WrongHistory.txt를 모두 불러오는 함수
*/
void loadWordsFromFiles() {
    loadWordsFromFile("Word.txt", wordList, true);
    loadWordsFromFile("Wrong.txt", wrongWordList, false);
    loadWordsFromFile("WrongHistory.txt", wrongWordHistoryList, false);
    loadWordsFromFile("WrongAccent.txt", wrongAccList, true);
    loadWordsFromFile("WrongAccentHistory.txt", wrongAccHistoryList, true);
}

/*
전체 단어 퀴즈에서 문제를 틀린 경우 해당 단어를 wrongWordList, wrongWordHistoryList에 추가하는 함수
영단어가 같은 단어가 있을 경우, 그 단어의 한글 뜻을 주어진 kor로 바꿈 - 일단 이 기능은 없앰(요구분석 필요)
@param wrongWord 틀린 단어
*/
void pushWrongQuizWord(QuizWord wrongWord) {
    bool exist = false;
    auto it = wrongWordList.begin();

    // 같은 영단어가 wrongWordList에 존재하는지 검사
    for (it = wrongWordList.begin(); it != wrongWordList.end(); ++it) {
        if (it->eng == wrongWord.eng) {
            exist = true;
            break;
        }
    }

    if (!exist) { // 영단어가 존재하지 않으면 오답노트에 추가
        string kor[KOR_MAX];
        kor[0] = wrongWord.kor;
        wrongWordList.push_back(Word(wrongWord.eng, kor, 1, wrongWord.syll, wrongWord.acc));
    }
    else { // 영단어가 존재할 때, 해당 한글 뜻이 없으면 추가
        bool e = false; // 체크용
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
    exist = false;

    // 영어와 한글 뜻이 같은 영단어가 wrongWordHistoryList에 존재하는지 검사
    for (it = wrongWordHistoryList.begin(); it != wrongWordHistoryList.end(); ++it) {
        if (it->eng == wrongWord.eng) {
            exist = true;
            break;
        }
    }

    if (!exist) { // 영단어가 존재하지 않으면 오답노트에 추가
        string kor[KOR_MAX];
        kor[0] = wrongWord.kor;
        wrongWordHistoryList.push_back(Word(wrongWord.eng, kor, 1, wrongWord.syll, wrongWord.acc));
    }
    else { // 영단어가 존재할 때, 해당 한글 뜻이 없으면 추가bool e = false; // 체크용
        bool e = false;
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
전체 강세 퀴즈에서 문제를 틀린 경우 해당 단어를 wrongAccList, wrongAccHistoryList에 추가하는 함수
영단어가 같은 단어가 있을 경우, 그 단어의 한글 뜻을 주어진 kor로 바꿈
@param wrongWord 틀린 단어
*/
void pushWrongAccentQuizWord(QuizWord wrongWord) {
    bool exist = false;
    auto it = wrongAccList.begin();

    // 같은 영단어가 wrongWordList에 존재하는지 검사
    for (it = wrongAccList.begin(); it != wrongAccList.end(); ++it) {
        if (it->eng == wrongWord.eng) {
            exist = true;
            break;
        }
    }

    if (!exist) { // 영단어가 존재하지 않으면 오답노트에 추가
        string kor[KOR_MAX];
        kor[0] = wrongWord.kor;
        wrongAccList.push_back(Word(wrongWord.eng, kor, 1, wrongWord.syll, wrongWord.acc));
    }
    else { // 영단어가 존재할 때, 해당 한글 뜻이 없으면 추가
        bool e = false; // 체크용
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
    exist = false;

    // 영어와 한글 뜻이 같은 영단어가 wrongWordHistoryList에 존재하는지 검사
    for (it = wrongAccHistoryList.begin(); it != wrongAccHistoryList.end(); ++it) {
        if (it->eng == wrongWord.eng) {
            exist = true;
            break;
        }
    }

    if (!exist) { // 영단어가 존재하지 않으면 오답노트에 추가
        string kor[KOR_MAX];
        kor[0] = wrongWord.kor;
        wrongAccHistoryList.push_back(Word(wrongWord.eng, kor, 1, wrongWord.syll, wrongWord.acc));
    }
    else { // 영단어가 존재할 때, 해당 한글 뜻이 없으면 추가bool e = false; // 체크용
        bool e = false;
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
현재 프로그램 내의 Word 객체들을 호출된 시점 기준으로 txt파일에 저장하는 함수
@param fileName 저장할 txt파일
@param wordVector 저장할 Word 객체가 담긴 vector
*/
void saveWordsToFile(const string& fileName, vector<Word>& wordVector) {
    ofstream file(fileName);
    if (file.is_open()) {
        for (const Word& word : wordVector) {
            //file << word.eng << "/" << word.kor << "/" << word.bookmarked << endl;
            file << word.eng << "/" << word.syll << "/" << word.acc << "/";
            for (int i = 0; i < word.cnt; i++) {
                file << word.kor[i];
                if (i != word.cnt - 1) {
                    file << ",";
                }
            }
            file << endl;
        }
        file.close();
    }
    else {
        cerr << "Error: " << fileName << "를 저장할 수 없습니다." << endl;
    }
}

/*
Word.txt, Wrong.txt, WrongHistory.txt를 모두 저장하는 함수
*/
void saveWordsToFiles() {
    saveWordsToFile("Word.txt", wordList);
    saveWordsToFile("Wrong.txt", wrongWordList);
    saveWordsToFile("WrongHistory.txt", wrongWordHistoryList);
    saveWordsToFile("WrongAccent.txt", wrongAccList);
    saveWordsToFile("WrongAccentHistory.txt", wrongAccHistoryList);
}

int mainFirst = 0; // main메뉴 상태 초기값
int viewFirst = 0; // 

int main() {


    setlocale(LC_ALL, "korean");
    loadWordsFromFiles();
    system("cls");


    while (1) {

        if (mainFirst == 0)
        {
            cout << "1. 영단어 퀴즈     2. 영단어 수정     3. 오답단어 조회 \n4. 종료\n";
            mainFirst++;
        }
        int menuSelect; // 메뉴 입력받으려고 선언

        string mainipt;
        getline(cin, mainipt);
        menuSelect = input2int(mainipt);

        if (menuSelect == 1) {         // 영단어 퀴즈 실행
            quiz();
        }
        else if (menuSelect == 2) {      // 영단어장 수정 메뉴 실행
            edit();
        }
        else if (menuSelect == 3) {      // 오답단어 조회
            viewWrong();
        }
        else if (menuSelect == 4) {      // 종료
            cout << "프로그램을 종료합니다.";
            break;
        }
        else {
            system("cls");
            cout << "1. 영단어 퀴즈     2. 영단어 수정     3. 오답단어 조회 \n4. 종료\n";
            cout << "올바르지 못한 입력입니다. 다시 입력해주세요 : ";
        }

    }

    return 0;
}



/*
영단어 추가 및 삭제 메뉴 edit()
*/
void edit() {
    //loadwordsfromfiles(); // 여기서 이 함수를 호출하지 않아도 큰 문제가 발생하지 않았습니다.
    system("cls");

    while (true) {

        string s;   // 영단어 검색한거 저장하려고 선언
        string userinput;

        bool exist = false;
        auto it = wordList.begin();
        cout << "영단어 추가 - 추가할 단어 검색" << endl;
        cout << "영단어 수정 - 수정할 단어 검색" << endl;
        cout << "영단어 삭제 - 삭제할 단어 검색" << endl;
        cout << "영단어 입력(종료-q): ";
        getline(cin, s);
        s = lowerString(s); // s : 입력 받은 영단어

        while (!checkEng(s)) {
            cout << "올바르지 못한 입력입니다." << endl;
            cout << "영단어 입력(종료-Q): ";
            getline(cin, s);
            s = lowerString(s);
        }

        if (s == "q") {      // 종료
            system("cls");
            break;
        }

        // 영단어가 존재하는지 검사
        for (it = wordList.begin(); it != wordList.end(); ++it) {
            if (it->eng == s) {
                exist = true;
                break;
            }
        }
        
        system("cls");
        if (!exist) {      // 영단어가 존재하지 않음

            cout << "[" << s << "] 라는 영단어는 존재하지 않습니다." << endl;
            cout << "[" << s << "] 단어를 추가하시겠습니까? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);

                if (choice == "y") {
                    cout << s << "의 뜻을 ,로 나누어 입력하세요. (최대 " << KOR_MAX << "개 가능): ";
                    wstring kor;
                    bool checkedKor = false;
                    wchar_t delim = L',';
                    vector<wstring> korVector;
                    // 한글 뜻 입력
                    while (!checkedKor) {
                        getline(wcin, kor);
                        korVector = splitTrimWString(kor, delim);

                        if (korVector.size() > KOR_MAX) {
                            cout << "최대 한글 뜻 개수보다 많습니다. 다시 입력해 주세요." << endl;
                        }
                        else if (korVector.size() == 0) {
                            cout << "한글 뜻이 없습니다. 다시 입력해 주세요." << endl;
                        }
                        else {
                            bool hasBadKor = false;
                            for (int i = 0; i < korVector.size(); i++) {
                                if (checkKor(korVector[i]) != 1) {
                                    cout << "입력한 한글 뜻 중 문법 형식에 맞지 않는 것이 있습니다. 다시 입력해 주세요." << endl;
                                    hasBadKor = true;
                                    break;
                                }
                            }
                            if (hasBadKor) continue;

                            // 중복 체크
                            bool hasDuplicate = false;
                            for (int i = 0; i < korVector.size() - 1; i++) {
                                for (int j = i + 1; j < korVector.size(); j++) {
                                    if (korVector[i] == korVector[j]) {
                                        cout << "입력한 한글 뜻 중 중복되는 것이 있습니다. 다시 입력해 주세요." << endl;
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

                    // 음절 수 입력
                    bool checkedSyll = false;
                    int syll;
                    while (!checkedSyll) {
                        cout << s << "의 음절 수를 입력하세요: ";
                        string sInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, sInput);
                        syll = input2int(sInput);

                        if (syll <= 0) {
                            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
                        }
                        else {
                            checkedSyll = true;
                            break;
                        }
                    }
                    // 강세 위치 입력
                    bool checkedAcc = false;
                    int acc;
                    while (!checkedAcc) {
                        cout << s << "의 강세 위치를 입력하세요: ";
                        string aInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, aInput);
                        acc = input2int(aInput);

                        if (acc <= 0) {
                            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
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
                    cout << " - " << syll << " - " << acc << "] 단어를 추가했습니다.(메뉴-Q)" << endl;

                    getline(cin, userinput);
                    userinput = lowerString(userinput);
                    while (userinput != "q")
                    {
                        system("cls");
                        cout << "[" << s << " - ";
                        printStringArray(korArr, korVector.size(), false);
                        cout << " - " << syll << " - " << acc << "] 단어를 추가했습니다.(메뉴-Q)" << endl;
                        cout << "Q만 입력해주세요." << endl;
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
                    cout << "올바르지 못한 입력입니다." << endl;
                    cout << "[" << s << "] 단어를 추가하시겠습니까? (Y/N)" << endl;
                }
            }
        }
        else {   // 영단어가 존재
            cout << "현재 단어장에 있는 단어: " << endl;
            cout << "[" << it->eng << " - ";
            printStringArray(it->kor, it->cnt, false);
            cout << " - " << it->syll << " - " << it->acc << "]" << endl;

            cout << "해당 단어를 삭제, 또는 수정하시겠습니까? (D/E/Q)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "d") {

                    string tmpKor[KOR_MAX];
                    int size = sizeof(it->kor) / sizeof(it->kor[0]);
                    copy(it->kor, it->kor + size, tmpKor);
                    int tmpCnt = it->cnt;
                    int tmpSyll = it->syll;
                    int tmpAcc = it->acc;

                    wordList.erase(it);
                    saveWordsToFiles();

                    cout << "[" << s << " - ";
                    printStringArray(tmpKor, tmpCnt, false);
                    cout << " - " << tmpSyll << " - " << tmpAcc << "] 단어가 삭제되었습니다.(메뉴-Q)" << endl;
                    getline(cin, userinput);
                    userinput = lowerString(userinput);
                    while (userinput != "q")
                    {
                        system("cls");
                        cout << "[" << s << " - ";
                        printStringArray(tmpKor, tmpCnt, false);
                        cout << " - " << tmpSyll << " - " << tmpAcc << "] 단어가 삭제되었습니다.(메뉴-Q)" << endl;
                        cout << "Q만 입력해주세요." << endl;
                        getline(cin, userinput);
                        userinput = lowerString(userinput);
                    }
                    break;
                }
                else if (choice == "e") {

                    cout << s << "의 새로운 뜻을 ,로 나누어 입력하세요. (최대 " << KOR_MAX << "개 가능): ";
                    wstring kor;
                    bool checkedKor = false;
                    wchar_t delim = L',';
                    vector<wstring> korVector;
                    // 한글 뜻 입력
                    while (!checkedKor) {
                        getline(wcin, kor);
                        korVector = splitTrimWString(kor, delim);

                        if (korVector.size() > KOR_MAX) {
                            cout << "최대 한글 뜻 개수보다 많습니다. 다시 입력해 주세요." << endl;
                        }
                        else if (korVector.size() == 0) {
                            cout << "한글 뜻이 없습니다. 다시 입력해 주세요." << endl;
                        }
                        else {
                            bool hasBadKor = false;
                            for (int i = 0; i < korVector.size(); i++) {
                                if (checkKor(korVector[i]) != 1) {
                                    cout << "입력한 한글 뜻 중 문법 형식에 맞지 않는 것이 있습니다. 다시 입력해 주세요." << endl;
                                    hasBadKor = true;
                                    break;
                                }
                            }
                            if (hasBadKor) continue;

                            // 중복 체크
                            bool hasDuplicate = false;
                            for (int i = 0; i < korVector.size() - 1; i++) {
                                for (int j = i + 1; j < korVector.size(); j++) {
                                    if (korVector[i] == korVector[j]) {
                                        cout << "입력한 한글 뜻 중 중복되는 것이 있습니다. 다시 입력해 주세요." << endl;
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

                    // 음절 수 입력
                    bool checkedSyll = false;
                    int syll;
                    while (!checkedSyll) {
                        cout << s << "의 음절 수를 입력하세요: ";
                        string sInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, sInput);
                        syll = input2int(sInput);

                        if (syll <= 0) {
                            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
                        }
                        else {
                            checkedSyll = true;
                            break;
                        }
                    }
                    // 강세 위치 입력
                    bool checkedAcc = false;
                    int acc;
                    while (!checkedAcc) {
                        cout << s << "의 강세 위치를 입력하세요: ";
                        string aInput;
                        /*cin.ignore(INT_MAX, '\n');*/
                        getline(cin, aInput);
                        acc = input2int(aInput);

                        if (acc <= 0) {
                            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
                        }
                        else {
                            checkedAcc = true;
                            break;
                        }
                    }

                    // 실제 수정
                    int size = sizeof(korArr) / sizeof(korArr[0]);
                    copy(korArr, korArr + size, it->kor);
                    it->cnt = korVector.size();
                    it->syll = syll;
                    it->acc = acc;

                    // 오답노트에서 수정
                    // 오답노트에 수정한 단어가 존재한다면
                    bool e = false;
                    auto wwL = wrongWordList.begin();
                    for (wwL; wwL != wrongWordList.end(); ++wwL) {
                        if (wwL->eng == s) {
                            vector<string>wwLKor;
                            for (int i = 0; i < wwL->cnt; i++) {
                                wwLKor.push_back(wwL->kor[i]);
                            }
                            // 오답노트에 있는 한글 뜻과 수정된 한글 뜻을 비교
                            for (int i = 0; i < wwL->cnt; i++) {
                                // 오답노트에 있는 한글 뜻이 수정된 한글 뜻과 일치하는 것이 없다면 삭제, 있으면 남겨둠
                                for (int j = 0; j < it->cnt; j++) {
                                    if (wwLKor[i] == it->kor[j]) {
                                        e = true;
                                        break;
                                    }                                    
                                }
                                if (!e) {
                                    wwLKor.erase(wwLKor.begin() + i);
                                    i--;
                                    wwL->cnt--;
                                }
                                e = false;
                            }
                            for (int i = 0; i < wwLKor.size(); i++) {
                                wwL->kor[i] = wwLKor[i];
                            }
                            // 음절 및 강세 위치도 반영
                            wwL->syll = it->syll;
                            wwL->acc = it->acc;
                        }
                    }

                    // 오답기록에서 수정
                    // 오답기록에 수정한 단어가 존재한다면
                    wwL = wrongWordHistoryList.begin();
                    for (wwL; wwL != wrongWordHistoryList.end(); ++wwL) {
                        if (wwL->eng == s) {
                            vector<string>wwLKor;
                            for (int i = 0; i < wwL->cnt; i++) {
                                wwLKor.push_back(wwL->kor[i]);
                            }
                            // 오답노트에 있는 한글 뜻과 수정된 한글 뜻을 비교
                            for (int i = 0; i < wwL->cnt; i++) {
                                // 오답노트에 있는 한글 뜻이 수정된 한글 뜻과 일치하는 것이 없다면 삭제, 있으면 남겨둠
                                for (int j = 0; j < it->cnt; j++) {
                                    if (wwLKor[i] == it->kor[j]) {
                                        e = true;
                                        break;
                                    }
                                }
                                if (!e) {
                                    wwLKor.erase(wwLKor.begin() + i);
                                    i--;
                                    wwL->cnt--;
                                }
                                e = false;
                            }
                            for (int i = 0; i < wwLKor.size(); i++) {
                                wwL->kor[i] = wwLKor[i];
                            }
                            // 음절 및 강세 위치도 반영
                            wwL->syll = it->syll;
                            wwL->acc = it->acc;
                        }
                    }

                    // 강세오답에서 수정
                    // 강세오답에 수정한 단어가 존재한다면
                    wwL = wrongAccList.begin();
                    for (wwL; wwL != wrongAccList.end(); ++wwL) {
                        if (wwL->eng == s) {
                            vector<string>wwLKor;
                            for (int i = 0; i < wwL->cnt; i++) {
                                wwLKor.push_back(wwL->kor[i]);
                            }
                            // 오답노트에 있는 한글 뜻과 수정된 한글 뜻을 비교
                            for (int i = 0; i < wwL->cnt; i++) {
                                // 오답노트에 있는 한글 뜻이 수정된 한글 뜻과 일치하는 것이 없다면 삭제, 있으면 남겨둠
                                for (int j = 0; j < it->cnt; j++) {
                                    if (wwLKor[i] == it->kor[j]) {
                                        e = true;
                                        break;
                                    }
                                }
                                if (!e) {
                                    wwLKor.erase(wwLKor.begin() + i);
                                    i--;
                                    wwL->cnt--;
                                }
                                e = false;
                            }
                            for (int i = 0; i < wwLKor.size(); i++) {
                                wwL->kor[i] = wwLKor[i];
                            }
                            // 음절 및 강세 위치도 반영
                            wwL->syll = it->syll;
                            wwL->acc = it->acc;
                        }
                    }

                    // 강세기록에서 수정
                    // 강세기록에서 수정한 단어가 존재한다면
                    wwL = wrongAccHistoryList.begin();
                    for (wwL; wwL != wrongAccHistoryList.end(); ++wwL) {
                        if (wwL->eng == s) {
                            vector<string>wwLKor;
                            for (int i = 0; i < wwL->cnt; i++) {
                                wwLKor.push_back(wwL->kor[i]);
                            }
                            // 오답노트에 있는 한글 뜻과 수정된 한글 뜻을 비교
                            for (int i = 0; i < wwL->cnt; i++) {
                                // 오답노트에 있는 한글 뜻이 수정된 한글 뜻과 일치하는 것이 없다면 삭제, 있으면 남겨둠
                                for (int j = 0; j < it->cnt; j++) {
                                    if (wwLKor[i] == it->kor[j]) {
                                        e = true;
                                        break;
                                    }
                                }
                                if (!e) {
                                    wwLKor.erase(wwLKor.begin() + i);
                                    i--;
                                    wwL->cnt--;
                                }
                                e = false;
                            }
                            for (int i = 0; i < wwLKor.size(); i++) {
                                wwL->kor[i] = wwLKor[i];
                            }
                            // 음절 및 강세 위치도 반영
                            wwL->syll = it->syll;
                            wwL->acc = it->acc;
                        }
                    }

                    saveWordsToFiles();

                    cout << "[" << s << " - ";
                    printStringArray(korArr, korVector.size(), false);
                    cout << " - " << syll << " - " << acc << "] 단어로 수정되었습니다.(메뉴-Q)" << endl;

                    getline(cin, userinput);
                    userinput = lowerString(userinput);
                    while (userinput != "q")
                    {
                        system("cls");
                        cout << "[" << s << " - ";
                        printStringArray(korArr, korVector.size(), false);
                        cout << " - " << syll << " - " << acc << "] 단어로 수정되었습니다.(메뉴-Q)" << endl;
                        cout << "Q만 입력해주세요." << endl;
                        getline(cin, userinput);
                        userinput = lowerString(userinput);
                    }
                    break;
                }
                else if (choice == "q") {
                    break;
                }
                else {
                    cout << "다시 입력해주세요." << endl;
                }
            }
        }
        system("cls");
    }

    mainFirst = 0;
    return;
}


/*
전체 영단어에 대한 퀴즈
*/
void totalQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // 퀴즈 취소 확인 여부
    int goalCount; // 퀴즈에서 완료할 단어 수

    int right = 0; // 맞춘 문제 수
    int wrong = 0; // 틀린 문제 수

    if (wordList.empty()) {
        cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
        cout << "메인메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
            cout << "메인메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    vector<QuizWord> quizWordVector;

    // 각 영단어와 그 단어의 여러 한글 뜻을 분리해서 추가
    for (int i = 0; i < wordList.size(); i++) {
        Word tempWord = wordList[i];
        for (int j = 0; j < tempWord.cnt; j++) {
            if (tempWord.kor[j].length() > 0)
                quizWordVector.push_back(QuizWord(tempWord.eng, tempWord.kor[j], tempWord.syll, tempWord.acc));
        }
    }

    while (true) {
        cout << "퀴즈 문제 수를 입력하세요: ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);

        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
            continue;
        }
        else if (goalCount <= quizWordVector.size())
            break;
        else
        {
            system("cls");
            cout << "가능한 문제 수 (" << quizWordVector.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
        }
    }

    // quizWordVector를 섞어서 인덱스가 랜덤한 순서로 등장하게 만들기
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);

    for (int i = 0; i < goalCount; i++) {
        system("cls"); // 화면 지우기 (Windows)

        QuizWord randomQuizWord = quizWordVector[i];
        cout << "문제 " << (i + 1) << "." << endl;
        cout << "한글 뜻: " << randomQuizWord.kor << endl;
        cout << "음절 수: " << randomQuizWord.syll << endl;
        cout << "강세 위치: " << randomQuizWord.acc << endl;
        cout << "영어 단어를 입력하세요 (메뉴로 돌아가려면 'Q' 입력): ";
        string userInput;
        getline(cin, userInput);
        while (!checkEng(userInput)) {
            cout << "규칙에 위배되는 입력입니다. 다시 입력해주세요 : ";
            getline(cin, userInput);
        }
        userInput = lowerString(userInput);

        if (userInput == "q") {
            qCheck = true;
            system("cls");
            break;
        }

        if (userInput == lowerString(randomQuizWord.eng)) {
            cout << "정답!" << endl;
            right++;
            cout << "다음문제로 - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "정답!" << endl;
                cout << "다음 문제로 - Q" << endl;
                cout << "Q만 입력해주세요." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
        else {
            cout << "틀렸습니다." << endl;
            wrong++;
            cout << "틀린 단어의 정답: " << randomQuizWord.eng << endl;

            // 필요 없음
            /*// 오답노트에 추가 여부를 알기 위해 해당 영단어를 wrongWordList에서 찾음
            bool exist = false;
            auto it = wrongWordList.begin();

            // 영단어가 wrongWordList에 존재하는지 검사
            for (it = wrongWordList.begin(); it != wrongWordList.end(); ++it) {
                if (it->eng == wordList[randomIndex].eng) {
                    exist = true;
                    break;
                }
            }*/

            cout << "해당 단어를 오답노트에 추가 하시겠습니까? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    pushWrongQuizWord(randomQuizWord); // 오답 노트에 추가 (+ 뜻 덮어쓰기)
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "다시 입력해주세요." << endl;
                }
            }


            cout << "다음 문제로 - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "틀렸습니다." << endl;
                cout << "틀린 단어의 정답: " << randomQuizWord.eng << endl;
                cout << "해당 단어는 이미 오답노트에 존재합니다." << endl;
                cout << "다음 문제로 - Q" << endl;
                cout << "Q만 입력해주세요." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
            saveWordsToFiles();   // 즉시 txt 수정
        }
    }
    if (!qCheck) {
        system("cls");
        cout << "퀴즈 종료" << endl;
        cout << "전체 문제 수: " << goalCount << endl;
        cout << "맞은 문제 수: " << right << endl;
        cout << "틀린 문제 수: " << wrong << endl;
        cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "퀴즈 종료" << endl;
            cout << "전체 문제 수: " << goalCount << endl;
            cout << "맞은 문제 수: " << right << endl;
            cout << "틀린 문제 수: " << wrong << endl;
            cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");

    }

    return;
}
/*
오답 영단어에 대한 퀴즈
*/
void wrongQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // 중간에 퀴즈를 취소했는지 확인
    int goalCount; // 퀴즈에서 완료할 단어 수

    int right = 0; // 맞춘 문제 수
    int wrong = 0; // 틀린 문제 수

    if (wrongWordList.empty()) {
        cout << "현재 오답노트에 단어가 없습니다." << endl;
        cout << "메인메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "현재 오답노트에 단어가 없습니다." << endl;
            cout << "메인메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    while (true) {
        cout << "퀴즈 문제 수를 입력하세요: ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);
        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
        }

        else if (goalCount <= wrongWordList.size())
            break;
        else
        {
            system("cls");
            cout << "오답노트에 있는 단어 수 (" << wrongWordList.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
        }
    }

    vector<QuizWord> quizWordVector;

    for (int i = 0; i < wrongWordList.size(); i++) {
        Word tempWord = wrongWordList[i];
        quizWordVector.push_back(QuizWord(tempWord.eng, tempWord.kor[0], tempWord.syll, tempWord.acc));
    }

    // quizWordVector를 섞어서 단어가 랜덤한 순서로 등장하게 만들기
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);


    for (int i = 0; i < goalCount; i++) {
        system("cls"); // 화면 지우기 (Windows)

        QuizWord randomQuizWord = quizWordVector[i];
        cout << "문제 " << (i + 1) << "." << endl;
        cout << "한글 뜻: " << randomQuizWord.kor << endl;
        cout << "음절 수: " << randomQuizWord.syll << endl;
        cout << "강세 위치: " << randomQuizWord.acc << endl;
        cout << "영어 단어를 입력하세요 (메뉴로 돌아가려면 'Q' 입력): ";

        string userInput;
        getline(cin, userInput);
        while (!checkEng(userInput)) {
            cout << "규칙에 위배되는 입력입니다. 다시 입력해주세요 : ";
            getline(cin, userInput);
        }
        userInput = lowerString(userInput);

        if (userInput == "q") {
            qCheck = true;
            system("cls");
            break;
        }

        if (userInput == lowerString(randomQuizWord.eng)) {
            cout << "정답!" << endl;
            right++;
            cout << "해당 단어를 오답노트에서 제거하시겠습니까? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    string removeEng = randomQuizWord.eng;
                    string removeKor = randomQuizWord.kor;
                    auto removeIt = remove_if(wrongWordList.begin(), wrongWordList.end(),
                        [removeEng, removeKor](const Word& word) { return (word.eng == removeEng && word.kor[0] == removeKor); });

                    wrongWordList.erase(removeIt, wrongWordList.end()); // 오답 노트에서 삭제

                    saveWordsToFiles();                       // Word 객체 변환 후 즉시 txt 수정
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "다시 입력해주세요." << endl;
                }
            }
        }
        else {
            cout << "틀렸습니다." << endl;
            wrong++;
            cout << "틀린 단어의 정답: " << randomQuizWord.eng << endl; // 오답노트 퀴즈이므로 따로 설정 X
            cout << "다음 문제로 - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "틀렸습니다." << endl;
                cout << "틀린 단어의 정답: " << randomQuizWord.eng << endl;
                cout << "다음 문제로 - Q" << endl;
                cout << "Q만 입력해주세요." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }

        }
    }

    if (!qCheck) {
        system("cls");
        cout << "퀴즈 종료" << endl;
        cout << "전체 문제 수: " << goalCount << endl;
        cout << "맞은 문제 수: " << right << endl;
        cout << "틀린 문제 수: " << wrong << endl;
        cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "퀴즈 종료" << endl;
            cout << "전체 문제 수: " << goalCount << endl;
            cout << "맞은 문제 수: " << right << endl;
            cout << "틀린 문제 수: " << wrong << endl;
            cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);

        }
        system("cls");
    }
    return;
}

/*
전체 영단어에 대한 강세 퀴즈
*/
void totalAccentQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // 퀴즈 취소 확인 여부
    int goalCount; // 퀴즈에서 완료할 단어 수

    int right = 0; // 맞춘 문제 수
    int wrong = 0; // 틀린 문제 수

    if (wordList.empty()) {
        cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
        cout << "메인메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
            cout << "메인메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    vector<QuizWord> quizWordVector;

    // 각 영단어와 그 단어의 여러 한글 뜻을 분리해서 추가
    for (int i = 0; i < wordList.size(); i++) {
        Word tempWord = wordList[i];
        for (int j = 0; j < tempWord.cnt; j++) {
            quizWordVector.push_back(QuizWord(tempWord.eng, tempWord.kor[j], tempWord.syll, tempWord.acc));
        }
    }

    while (true) {
        cout << "퀴즈 문제 수를 입력하세요: ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);

        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
            continue;
        }
        else if (goalCount <= quizWordVector.size())
            break;
        else
        {
            system("cls");
            cout << "저장된 단어 수 (" << quizWordVector.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
        }
    }



    // quizIndexVector를 섞어서 인덱스가 랜덤한 순서로 등장하게 만들기
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizWordVector.begin(), quizWordVector.end(), g);

    for (int i = 0; i < goalCount; i++) {
        system("cls"); // 화면 지우기 (Windows)

        QuizWord randomQuizWord = quizWordVector[i];
        cout << "문제 " << (i + 1) << "." << endl;
        cout << "영단어: " << randomQuizWord.eng << endl;
        cout << "한글 뜻: " << randomQuizWord.kor << endl;
        cout << "음절 수: " << randomQuizWord.syll << endl;
        cout << "강세 위치를 입력하세요 (메뉴로 돌아가려면 'Q' 입력): ";
        string userInput;
        int userInputInt = 0;

        while (!qCheck && userInputInt <= 0) {
            getline(cin, userInput);
            userInput = lowerString(userInput);
            if (userInput == "q") {
                qCheck = true;
                system("cls");
                break;
            }

            userInputInt = input2int(userInput);
            if (userInputInt <= 0)
                cout << "규칙에 위배되는 입력입니다. 다시 입력해주세요 : ";
        }

        if (qCheck) break;

        if (userInputInt == randomQuizWord.acc) {
            cout << "정답!" << endl;
            right++;
            cout << "다음문제로 - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "정답!" << endl;
                cout << "다음 문제로 - Q" << endl;
                cout << "Q만 입력해주세요." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
        else {
            cout << "틀렸습니다." << endl;
            wrong++;
            cout << "틀린 단어의 강세 위치 정답: " << randomQuizWord.acc << endl;

            cout << "해당 단어를 오답노트에 추가 하시겠습니까? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    pushWrongAccentQuizWord(randomQuizWord); // 오답 노트에 추가 (+ 뜻 덮어쓰기)
                    saveWordsToFiles();   // 즉시 txt 수정
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "다시 입력해주세요." << endl;
                }
            }


            cout << "다음 문제로 - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "틀렸습니다." << endl;
                cout << "틀린 단어의 강세 위치 정답: " << randomQuizWord.acc << endl;
                cout << "해당 단어는 이미 오답노트에 존재합니다." << endl;
                cout << "다음 문제로 - Q" << endl;
                cout << "Q만 입력해주세요." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
            saveWordsToFiles();   // 즉시 txt 수정
        }
    }
    if (!qCheck) {
        system("cls");
        cout << "퀴즈 종료" << endl;
        cout << "전체 문제 수: " << goalCount << endl;
        cout << "맞은 문제 수: " << right << endl;
        cout << "틀린 문제 수: " << wrong << endl;
        cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "퀴즈 종료" << endl;
            cout << "전체 문제 수: " << goalCount << endl;
            cout << "맞은 문제 수: " << right << endl;
            cout << "틀린 문제 수: " << wrong << endl;
            cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");

    }

    return;
}

/*
오답 영단어에 대한 강세 퀴즈
*/
void wrongAccentQuiz() {
    system("cls");
    string userinput;
    bool qCheck; // 퀴즈 취소 확인 여부
    int goalCount; // 퀴즈에서 완료할 단어 수

    int right = 0; // 맞춘 문제 수
    int wrong = 0; // 틀린 문제 수

    if (wrongAccList.empty()) {
        cout << "현재 오답노트에 단어가 없습니다." << endl;
        cout << "메인메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "현재 오답노트에 단어가 없습니다." << endl;
            cout << "메인메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");
        return;
    }

    while (true) {
        cout << "퀴즈 문제 수를 입력하세요: ";
        qCheck = false;

        string quizipt;
        /*cin.ignore(INT_MAX, '\n');*/
        getline(cin, quizipt);

        goalCount = input2int(quizipt);

        if (goalCount <= 0) {
            system("cls");
            cout << "1 이상의 정수값을 입력해야 합니다. 다시 입력해주세요." << endl;
            continue;
        }
        else if (goalCount <= wrongAccList.size())
            break;
        else
        {
            system("cls");
            cout << "오답노트에 있는 단어 수 (" << wrongAccList.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
        }
    }

    vector<int> quizIndexVector;

    // 각 영단어를 추가
    for (int i = 0; i < wrongAccList.size(); i++) {
        quizIndexVector.push_back(i);
    }

    // quizIndexVector를 섞어서 인덱스가 랜덤한 순서로 등장하게 만들기
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister pseudo - random generator
    shuffle(quizIndexVector.begin(), quizIndexVector.end(), g);

    for (int i = 0; i < goalCount; i++) {
        system("cls"); // 화면 지우기 (Windows)

        Word randomWord = wordList[quizIndexVector[i]];
        cout << "문제 " << (i + 1) << "." << endl;
        cout << "영단어: " << randomWord.eng << endl;
        cout << "한글 뜻: ";
        printStringArray(randomWord.kor, randomWord.cnt, true);
        cout << "음절 수: " << randomWord.syll << endl;
        cout << "강세 위치를 입력하세요 (메뉴로 돌아가려면 'Q' 입력): ";
        string userInput;
        int userInputInt = 0;

        while (!qCheck && userInputInt <= 0) {
            getline(cin, userInput);
            userInput = lowerString(userInput);
            if (userInput == "q") {
                qCheck = true;
                system("cls");
                break;
            }

            userInputInt = input2int(userInput);
            if (userInputInt <= 0)
                cout << "규칙에 위배되는 입력입니다. 다시 입력해주세요 : ";
        }

        if (qCheck) break;

        if (userInputInt == randomWord.acc) {
            cout << "정답!" << endl;
            right++;

            cout << "해당 단어를 오답노트에서 제거하시겠습니까? (Y/N)" << endl;
            string choice;
            while (true) {
                getline(cin, choice);
                choice = lowerString(choice);
                if (choice == "y") {
                    string removeEng = randomWord.eng;
                    auto removeIt = remove_if(wrongAccList.begin(), wrongAccList.end(),
                        [removeEng](const Word& word) { return word.eng == removeEng; });

                    wrongAccList.erase(removeIt, wrongAccList.end()); // 오답 노트에서 삭제

                    saveWordsToFiles();                       // Word 객체 변환 후 즉시 txt 수정
                    break;
                }
                else if (choice == "n") {
                    break;
                }
                else {
                    cout << "다시 입력해주세요." << endl;
                }
            }
        }
        else {
            cout << "틀렸습니다." << endl;
            wrong++;
            cout << "틀린 단어의 강세 위치 정답: " << randomWord.acc << endl;

            cout << "다음 문제로 - Q" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
            while (userinput != "q")
            {
                system("cls");
                cout << "틀렸습니다." << endl;
                cout << "틀린 단어의 강세 위치 정답: " << randomWord.acc << endl;
                cout << "다음 문제로 - Q" << endl;
                cout << "Q만 입력해주세요." << endl;
                getline(cin, userinput);
                userinput = lowerString(userinput);
            }
        }
    }
    if (!qCheck) {
        system("cls");
        cout << "퀴즈 종료" << endl;
        cout << "전체 문제 수: " << goalCount << endl;
        cout << "맞은 문제 수: " << right << endl;
        cout << "틀린 문제 수: " << wrong << endl;
        cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
        cout << "메뉴로 이동-Q" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
        while (userinput != "q")
        {
            system("cls");
            cout << "퀴즈 종료" << endl;
            cout << "전체 문제 수: " << goalCount << endl;
            cout << "맞은 문제 수: " << right << endl;
            cout << "틀린 문제 수: " << wrong << endl;
            cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;
            cout << "메뉴로 이동-Q" << endl;
            cout << "Q만 입력해주세요" << endl;
            getline(cin, userinput);
            userinput = lowerString(userinput);
        }
        system("cls");

    }

    return;
}

/*
퀴즈 메뉴 출력 및 선택용 함수 quiz()
*/
void quiz() {
    system("cls");
    int menuSelect;

    while (1) {

        cout << "1. 전체 단어 퀴즈     2. 오답노트 퀴즈     3. 전체 강세 퀴즈     4. 오답 강세 퀴즈\n";
        cout << "메인 메뉴로 나가려면 0을 입력해주세요.\n";

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
            cout << "올바르지 못한 입력입니다." << endl;
        }
    }
}

/*
오답노트 조회 함수 viewWrongWordList()
@param wrongWordList wrongWordList 또는 wrongAccList
*/
void viewWrongWordList(const vector<Word>& wrongWordList) {
    system("cls");
    auto it = wrongWordList.begin();
    string userinput;

    if (wrongWordList.empty()) {
        cout << "현재 오답노트에 단어가 없습니다." << endl;
    }
    else {
        // 단어 모두 출력
        cout << "[영단어 - 한글 뜻 - 음절 수 - 강세 위치]" << endl;
        for (it = wrongWordList.begin(); it != wrongWordList.end(); ++it) {
            cout << "[" << it->eng << " - ";
            printStringArray(it->kor, it->cnt, false);
            cout << " - " << it->syll << " - " << it->acc << "]" << endl;
        }
    }

    cout << "조회 나가기 - Q" << endl;
    getline(cin, userinput);
    userinput = lowerString(userinput);
    while (userinput != "q")
    {
        system("cls");
        cout << "조회 나가기 - Q" << std::endl;
        cout << "Q만 입력해주세요" << std::endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
    }
    system("cls");
}


/*
오답노트에 한 번이라도 들어갔던 단어 조회 함수 viewWrongWordHistoryList()
@param wrongWordHistoryList wrongWordHistoryList 또는 wrongAccHistoryList
*/
void viewWrongWordHistoryList(const vector<Word>& wrongWordHistoryList) {
    system("cls");
    auto it = wrongWordHistoryList.begin();
    string userinput;
    if (wrongWordHistoryList.empty()) {
        cout << "현재 한 번이라도 오답노트에 들어갔던 단어가 없습니다." << endl;
    }
    else {
        // 단어 모두 출력
        cout << "[영단어 - 한글 뜻 - 음절 수 - 강세 위치]" << endl;
        for (it = wrongWordHistoryList.begin(); it != wrongWordHistoryList.end(); ++it) {
            cout << "[" << it->eng << " - ";
            printStringArray(it->kor, it->cnt, false);
            cout << " - " << it->syll << " - " << it->acc << "]" << endl;
        }
    }

    cout << "조회 나가기 - Q" << endl;
    getline(cin, userinput);
    userinput = lowerString(userinput);
    while (userinput != "q")
    {
        system("cls");
        cout << "조회 나가기 - Q" << endl;
        cout << "Q만 입력해주세요" << endl;
        getline(cin, userinput);
        userinput = lowerString(userinput);
    }
    system("cls");

}


/*
오답단어 조회 메뉴 viewWrong()
*/
void viewWrong() {

    int menuSelect;
    system("cls");

    while (1) {
        if (viewFirst == 0)
        {
            cout << "1. 오답노트 단어 조회     2. 오답노트 기록 조회     3. 강세 오답노트 단어 조회     4. 강세 오답노트 기록 조회\n";
            cout << "메인 메뉴로 나가려면 0을 입력해주세요.\n";
            viewFirst++;
        }

        string viewipt;
        getline(cin, viewipt);
        menuSelect = input2int(viewipt);

        if (menuSelect == 1) {
            viewWrongWordList(wrongWordList);
            viewFirst = 0;
        }
        else if (menuSelect == 2) {
            viewWrongWordHistoryList(wrongWordHistoryList);
            viewFirst = 0;
        }
        else if (menuSelect == 3) {
            viewWrongWordList(wrongAccList);
            viewFirst = 0;
        }
        else if (menuSelect == 4) {
            viewWrongWordHistoryList(wrongAccHistoryList);
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
            cout << "1. 오답노트 단어 조회     2. 오답노트 기록 조회     3. 강세 오답노트 단어 조회     4. 강세 오답노트 기록 조회\n";
            cout << "메인 메뉴로 나가려면 0을 입력해주세요.\n";
            cout << "올바르지 못한 입력입니다." << endl;
        }

    }
}