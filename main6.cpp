#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <locale>
#include <conio.h> // _getch()

using namespace std;

//함수 헤더선언
void edit();
void totalQuiz();
void wrongQuiz();
void quiz();
void viewWrong();
bool checkEng(string& str);
bool checkKor(wstring& str);

class Word { // 한글 뜻, 영어단어를 가진 Class Word
public:
	string eng;
	string kor;

	Word(const string& eng, const string& kor) : eng(eng), kor(kor) {}
};

vector<Word> wordList; // Word 객체 저장 vector인 wordList
vector<Word> wrongWordList; // 오답 Word 객체 저장 vector인 wrongWordList
vector<Word> wrongWordHistoryList; // 오답노트에 한 번이라도 들어간 Word 객체 저장 vector인 wrongWordHistoryList

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
		return 0;				//문자열 길이 최대 30 제한
	}

	bool hasEng = false;		//알파벳 하나없이 공백이랑 '-'만 있는지 체크하기위해
	for (char c : str) {
		if (!((c >= 'a' && c <= 'z') || (c == ' ') || (c == '-'))) {
			return 0;
		}
		if (c >= 'a' && c <= 'z') {
			hasEng = true;		//알파벳 최소 1개라도 발견하면 hasEng를 true로 변환
		}
	}

	if (!hasEng) return 0;		//알파벳 하나도 없으면 영단어 조건 만족하지 못하므로

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
*/
void loadWordsFromFile(const string& fileName, vector<Word>& wordVector) {
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

				string kor = line.substr(pos + 1);
				wstring k = s2w(kor);
				if (checkKor(k) == 0) {
					cout << fileName << " 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
					exit(0);
				}
				kor = w2s(k);
				wordVector.push_back(Word(eng, kor));
			}
		}
		file.close();

		// 겹치는 영어단어가 있는지 체크
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
	else {
		cerr << "Error: " << fileName << "를 불러올 수 없습니다." << endl;
		return;
	}
}

/*
Word.txt, Wrong.txt, WrongHistory.txt를 모두 불러오는 함수
*/
void loadWordsFromFiles() {
	loadWordsFromFile("Word.txt", wordList);
	loadWordsFromFile("Wrong.txt", wrongWordList);
	loadWordsFromFile("WrongHistory.txt", wrongWordHistoryList);
}

/*
전체 단어 퀴즈에서 문제를 틀린 경우 해당 단어를 wrongWordList, wrongWordHistoryList에 추가하는 함수
영단어가 같은 단어가 있을 경우, 그 단어의 한글 뜻을 주어진 kor로 바꿈
@param eng 틀린 단어의 현재 영어단어
@param kor 틀린 단어의 현재 한글 뜻
@param push 틀린 단어를 wrongWordList와 wrongWordHistoryList에 추가할지 여부
*/
void pushWrongWordLists(string& eng, string& kor, bool push) {
	bool exist = false;
	auto it = wrongWordList.begin();

	// 영단어가 wrongWordList에 존재하는지 검사
	for (it = wrongWordList.begin(); it != wrongWordList.end(); ++it) {
		if (it->eng == eng) {
			exist = true;
			break;
		}
	}

	if (!exist) { // 영단어가 존재하지 않으면 오답노트에 추가
		if (push)
			wrongWordList.push_back(Word(eng, kor));
	}
	else { // 영단어가 있으면 뜻을 업데이트
		it->kor = kor;
	}
	exist = false;

	// 영단어가 wrongWordHistoryList에 존재하는지 검사
	for (it = wrongWordHistoryList.begin(); it != wrongWordHistoryList.end(); ++it) {
		if (it->eng == eng) {
			exist = true;
			break;
		}
	}

	if (!exist) { // 영단어가 존재하지 않으면 오답노트에 추가
		if (push)
			wrongWordHistoryList.push_back(Word(eng, kor));
	}
	else { // 영단어가 있으면 뜻을 업데이트
		it->kor = kor;
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
			file << word.eng << "/" << word.kor << endl;
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

		if (menuSelect == 1) {			// 영단어 퀴즈 실행
			quiz();
		}
		else if (menuSelect == 2) {		// 영단어장 수정 메뉴 실행
			edit();
		}
		else if (menuSelect == 3) {		// 오답단어 조회
			viewWrong();
		}
		else if (menuSelect == 4) {		// 종료
			cout << "프로그램을 종료합니다.";
			break;
		}
		else {
			system("cls");
			cout << "1. 영단어 퀴즈     2. 영단어 수정     3. 오답단어 조회 \n4. 종료\n";
			cout << "올바르지 못한 입력입니다." << endl;
		}

	}

	return 0;
}



/*
영단어 추가 및 삭제 메뉴 edit()
*/
void edit() {
	//loadWordsFromFiles(); // 여기서 이 함수를 호출하지 않아도 큰 문제가 발생하지 않았습니다.
	system("cls");

	while (true) {

		string s;	// 영단어 검색한거 저장하려고 선언
		string userinput;

		bool exist = false;
		auto it = wordList.begin();
		cout << "영단어 추가 - 추가할 단어 검색" << endl;
		cout << "영단어 수정 - 수정할 단어 검색" << endl;
		cout << "영단어 삭제 - 삭제할 단어 검색" << endl;
		cout << "영단어 입력(종료-Q): ";
		getline(cin, s);
		s = lowerString(s);

		while (!checkEng(s)) {
			cout << "올바르지 못한 입력입니다." << endl;
			cout << "영단어 입력(종료-Q): ";
			getline(cin, s);
			s = lowerString(s);
		}

		if (s == "q") {		// 종료
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
		if (!exist) {		// 영단어가 존재하지 않음

			cout << "[" << s << "] 라는 영단어는 존재하지 않습니다." << endl;
			cout << "[" << s << "] 단어를 추가하시겠습니까? (Y/N)" << endl;
			string choice;
			while (true) {
				getline(cin, choice);
				choice = lowerString(choice);

				if (choice == "y") {
					cout << s << "의 뜻을 입력하세요: ";
					wstring kor;
					getline(wcin, kor);

					while (checkKor(kor) != 1) {
						cout << "다시 입력해 주세요." << endl;
						getline(wcin, kor);
					}

					string k = w2s(kor);

					wordList.push_back(Word(s, k));
					saveWordsToFiles();

					cout << "[" << s << " - " << k << "] 단어를 추가했습니다.(메뉴-Q)" << endl;
					getline(cin, userinput);
					userinput = lowerString(userinput);
					while (userinput != "q")
					{
						system("cls");
						cout << "[" << s << " - " << k << "] 단어를 추가했습니다.(메뉴-Q)" << endl;
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
		else {	// 영단어가 존재

			cout << "현재 단어장에 있는 단어: " << endl;
			cout << "[" << it->eng << " - " << it->kor << "]" << endl;

			cout << "해당 단어를 삭제, 또는 수정하시겠습니까? (D/E/Q)" << endl;
			string choice;
			while (true) {
				getline(cin, choice);
				choice = lowerString(choice);
				if (choice == "d") {

					string tmpkor = it->kor;

					wordList.erase(it);
					saveWordsToFiles();

					cout << "[" << s << " - " << tmpkor << "] 단어가 삭제되었습니다.(메뉴-Q)" << endl;
					getline(cin, userinput);
					userinput = lowerString(userinput);
					while (userinput != "q")
					{
						system("cls");
						cout << "[" << s << " - " << tmpkor << "] 단어가 삭제되었습니다.(메뉴-Q)" << endl;
						cout << "Q만 입력해주세요." << endl;
						getline(cin, userinput);
						userinput = lowerString(userinput);
					}
					break;
				}
				else if (choice == "e") {
					cout << it->eng << "의 새로운 뜻을 입력하세요: ";
					wstring kor;
					getline(wcin, kor);

					while (checkKor(kor) != 1) {
						cout << "다시 입력해주세요." << endl;
						getline(wcin, kor);
					}

					string k = w2s(kor);

					it->kor = k;

					saveWordsToFiles();

					cout << "[" << s << " - " << k << "] 단어로 뜻이 수정되었습니다.(메뉴-Q)" << endl;
					getline(cin, userinput);
					userinput = lowerString(userinput);
					while (userinput != "q")
					{
						system("cls");
						cout << "[" << s << " - " << k << "] 단어로 뜻이 수정되었습니다.(메뉴-Q)" << endl;
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
		cout << "수정 메뉴-Q" << endl;
		getline(cin, userinput);
		userinput = lowerString(userinput);
		while (userinput != "q")
		{
			system("cls");
			cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
			cout << "수정 메뉴-Q" << endl;
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
		else if (goalCount <= wordList.size())
			break;
		else
		{
			system("cls");
			cout << "저장된 단어 수 (" << wordList.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
		}
	}

	vector<int> quizIndexVector;

	for (int i = 0; i < wordList.size(); i++) {
		quizIndexVector.push_back(i);
	}

	// quizIndexVector를 섞어서 인덱스가 랜덤한 순서로 등장하게 만들기
	random_device rd;
	mt19937 g(rd()); // Mersenne Twister pseudo - random generator
	shuffle(quizIndexVector.begin(), quizIndexVector.end(), g);

	for (int i = 0; i < goalCount; i++) {
		system("cls"); // 화면 지우기 (Windows)

		int randomIndex = quizIndexVector[i];
		cout << "문제 " << (i + 1) << "." << endl;
		cout << "한글 뜻: " << wordList[randomIndex].kor << endl;
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
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "정답!" << endl;
			right++;
			cout << "다음문제로 - Q" << endl;
			getline(cin, userinput);
			userinput = lowerString(userinput);
			while (userinput != "q")
			{
				system("cls");
				cout << "정답!" << endl;
				cout << "다음문제로 - Q" << endl;
				cout << "Q만 입력해주세요." << endl;
				getline(cin, userinput);
				userinput = lowerString(userinput);
			}
		}
		else {
			cout << "틀렸습니다." << endl;
			wrong++;
			cout << "틀린 단어의 정답: " << wordList[randomIndex].eng << endl;

			// 오답노트에 추가 여부를 알기 위해 해당 영단어를 wrongWordList에서 찾음
			bool exist = false;
			auto it = wrongWordList.begin();

			// 영단어가 wrongWordList에 존재하는지 검사
			for (it = wrongWordList.begin(); it != wrongWordList.end(); ++it) {
				if (it->eng == wordList[randomIndex].eng) {
					exist = true;
					break;
				}
			}

			if (!exist) {
				cout << "해당 단어를 오답노트에 추가 하시겠습니까? (Y/N)" << endl;
				string choice;
				while (true) {
					getline(cin, choice);
					choice = lowerString(choice);
					if (choice == "y") {
						pushWrongWordLists(wordList[randomIndex].eng, wordList[randomIndex].kor, true); // 오답 노트에 추가 (+ 뜻 덮어쓰기)
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
			}
			else {
				cout << "해당 단어는 이미 오답노트에 존재합니다." << endl;
				pushWrongWordLists(wordList[randomIndex].eng, wordList[randomIndex].kor, false); // 오답 노트에 추가하지 않고 뜻만 덮어쓰기
				saveWordsToFiles();   // 즉시 txt 수정
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
		cout << "수정 메뉴-Q" << endl;
		getline(cin, userinput);
		userinput = lowerString(userinput);
		while (userinput != "q")
		{
			system("cls");
			cout << "현재 오답노트에 단어가 없습니다." << endl;
			cout << "수정 메뉴-Q" << endl;
			cout << "Q만 입력해주세요" << endl;
			getline(cin, userinput);
			userinput = lowerString(userinput);
		}
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

	vector<Word> quizWordVector;

	for (int i = 0; i < wrongWordList.size(); i++) {
		quizWordVector.push_back(Word(wrongWordList[i].eng, wrongWordList[i].kor));
	}

	// quizWordVector를 섞어서 단어가 랜덤한 순서로 등장하게 만들기
	random_device rd;
	mt19937 g(rd()); // Mersenne Twister pseudo - random generator
	shuffle(quizWordVector.begin(), quizWordVector.end(), g);


	for (int i = 0; i < goalCount; i++) {
		system("cls"); // 화면 지우기 (Windows)

		Word randomWord = quizWordVector[i];
		cout << "문제 " << (i + 1) << "." << endl;
		cout << "한글 뜻: " << randomWord.kor << endl;
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
			break;
		}

		if (userInput == lowerString(randomWord.eng)) {
			cout << "정답!" << endl;
			right++;
			cout << "해당 단어를 오답노트에서 제거하시겠습니까? (Y/N)" << endl;
			string choice;
			while (true) {
				getline(cin, choice);
				choice = lowerString(choice);
				if (choice == "y") {
					string removeEng = randomWord.eng;
					auto removeIt = remove_if(wrongWordList.begin(), wrongWordList.end(),
						[removeEng](const Word& word) { return word.eng == removeEng; });

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
			cout << "틀린 단어의 정답: " << randomWord.eng << endl; // 오답노트 퀴즈이므로 따로 설정 X

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
			cout << "메뉴로 이동-Q" << endl;
			cout << "Q만 입력해주세요" << endl;
			getline(cin, userinput);
			userinput = lowerString(userinput);
		}
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

		cout << "1. 전체 단어 퀴즈     2. 오답노트 퀴즈\n";
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
*/
void viewWrongWordList() {
	system("cls");
	auto it = wrongWordList.begin();
	string userinput;

	if (wrongWordList.empty()) {
		cout << "현재 오답노트에 단어가 없습니다." << endl;
	}
	else {
		// 단어 모두 출력
		for (it = wrongWordList.begin(); it != wrongWordList.end(); ++it) {
			cout << "[" << it->eng << " - " << it->kor << "]" << endl;
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
오답노트에 한 번이라도 들어갔던 단어 조회 함수 viewWrongWordHistoryList()
*/
void viewWrongWordHistoryList() {
	system("cls");
	auto it = wrongWordHistoryList.begin();
	string userinput;
	if (wrongWordHistoryList.empty()) {
		cout << "현재 한 번이라도 오답노트에 들어갔던 단어가 없습니다." << endl;
	}
	else {
		// 단어 모두 출력
		for (it = wrongWordHistoryList.begin(); it != wrongWordHistoryList.end(); ++it) {
			cout << "[" << it->eng << " - " << it->kor << "]" << endl;
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
			cout << "1. 오답노트 단어 조회     2. 오답노트 기록 조회\n";
			cout << "메인 메뉴로 나가려면 0을 입력해주세요.\n";
			viewFirst++;
		}

		string viewipt;
		getline(cin, viewipt);
		menuSelect = input2int(viewipt);

		if (menuSelect == 1) {
			viewWrongWordList();
			viewFirst = 0;
		}
		else if (menuSelect == 2) {
			viewWrongWordHistoryList();
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
			cout << "1. 오답노트 단어 조회     2. 오답노트 기록 조회\n";
			cout << "메인 메뉴로 나가려면 0을 입력해주세요.\n";
			cout << "올바르지 못한 입력입니다." << endl;
		}

	}
}

