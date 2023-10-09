#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <locale>

using namespace std;

//함수 헤더선언
void edit();
void totalQuiz();
void bookmarkQuiz();
void quiz();
bool checkEng(string str);
bool checkKor(wstring& str);

wstring s2w(const string& str)
{
	static locale loc("");
	auto& facet = use_facet<codecvt<wchar_t, char, mbstate_t>>(loc);
	return wstring_convert<remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(str);
}


string w2s(const wstring& wstr)
{
	static locale loc("");
	auto& facet = use_facet<codecvt<wchar_t, char, mbstate_t>>(loc);
	return wstring_convert<remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstr);
}


class Word { // 한국어, 영어, 북마크 여부를 가진 Class Word
public:
	string eng;
	string kor;
	bool bookmarked;

	Word(const string& eng, const string& kor) : eng(eng), kor(kor), bookmarked(0) {}
	Word(const string& eng, const string& kor, bool& bookmarked) : eng(eng), kor(kor), bookmarked(bookmarked) {}
};

vector<Word> wordList; // Word 객체 저장 vector인 wordList


/*
txt파일에서 string을 읽어와서 Word 객체로 변환 후 저장
*/
void loadWordsFromFile() {
	ifstream file("Word.txt");
	if (file.is_open()) {
		wordList.clear(); // 기존 wordList 비우기
		string line;
		while (getline(file, line)) {
			size_t pos = line.find("/");
			size_t pos2 = line.rfind("/");

			if (pos == string::npos || pos2 == string::npos)
			{
				cout << "Word.txt 파일 형식에 문제가 있습니다.\n프로그램을 종료합니다." << endl;
				file.close();
				return;
			}


			if (pos != string::npos) {
				string eng = line.substr(0, pos);
				if (checkEng(eng) == 0) {
					cout << "데이터 파일의 문법 규칙이 잘못되었습니다.";
					exit(0);
				}
				string kor = line.substr(pos + 1, pos2 - pos - 1);
				wstring k = s2w(kor);
				if (checkKor(k) == 0) {
					cout << "데이터 파일의 문법 규칙이 잘못되었습니다.";
					exit(0);
				}
				kor = w2s(k);
				string b_marked = line.substr(pos2);
				bool bookmarked;
				if (b_marked == "true") bookmarked = true;
				else bookmarked = false;
				wordList.push_back(Word(eng, kor, bookmarked));
			}
		}
		file.close();
	}
	else {
		cerr << "Error: Word.txt를 불러올 수 없습니다." << endl;
	}
}


/*
현재 프로그램 내의 Word 객체들을 호출된 시점 기준으로 txt파일에 저장하는 함수
*/
void saveWordsToFile() {
	ofstream file("Word.txt");
	if (file.is_open()) {
		for (const Word& word : wordList) {
			file << word.eng << "/" << word.kor << "/" << word.bookmarked << endl;
		}
		file.close();
	}
	else {
		cerr << "Error: Word.txt를 저장할 수 없습니다." << endl;
	}
}

int main() {
	setlocale(LC_ALL, "korean");

	int menuSelect;	// 메뉴 입력받으려고 선언
	loadWordsFromFile();

	while (1) {
		system("cls");
		cout << "1. 영단어 퀴즈     2. 영단어 수정 \n3. 종료\n";
		cin >> menuSelect;
		cin.ignore();

		if (menuSelect == 1) {			// 영단어 퀴즈 실행
			quiz();
		}
		else if (menuSelect == 2) {		// 영단어장 수정 메뉴 실행
			edit();
		}
		else if (menuSelect == 3) {		// 종료
			cout << "프로그램을 종료합니다.";
			break;
		}
		else {
			cout << "1 ~ 3 중 입력하세요." << endl;
			cin.ignore();
		}

		if (!cin) {				//정수 입력 안했을 때 처리
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cin.get();
		}
	}

	return 0;
}

/*
	string 전체를 lowercase로 변환 (string끼리 비교하기 쉽도록)
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
영단어 추가 및 삭제 메뉴 edit()
*/
void edit() {
	loadWordsFromFile();

	cout << "영단어 추가 - 추가할 단어 검색" << endl;
	cout << "영단어 삭제 - 삭제할 단어 검색" << endl;

	while (true) {

		string s;	// 영단어 검색한거 저장하려고 선언
		bool exist = false;
		auto it = wordList.begin();

		cout << "영단어 입력(종료-Q): ";
		getline(cin, s);
		s = lowerString(s);

		if (!checkEng(s)) {  // 영어단어 문법검사, 일단 break 로 구현해놨으나 경고창을 띄울지 토의
			continue;
		}

		if (s == "q") {		// 종료
			break;
		}

		// 영단어가 존재하는지 검사
		for (it = wordList.begin(); it != wordList.end(); ++it) {
			if (it->eng == s) {
				exist = true;
				break;
			}
		}

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
					cin.ignore();
					getline(wcin, kor);

					while (checkKor(kor) != 1) {
						cout << "다시 입력해주세요." << endl;
						getline(wcin, kor);
					}

					string k = w2s(kor);

					wordList.push_back(Word(s, k));
					saveWordsToFile();

					cout << "[" << s << " - " << k << "] 단어를 추가했습니다." << endl;
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
		else {	// 영단어가 존재

			cout << "[" << it->eng << " - " << it->kor << "]" << endl;

			cout << "해당 단어를 삭제하시겠습니까? (Y/N)" << endl;
			string choice;
			while (true) {
				getline(cin, choice);
				choice = lowerString(choice);
				if (choice == "y") {

					string tmpkor = it->kor;

					wordList.erase(it);
					saveWordsToFile();

					cout << "[" << s << " - " << tmpkor << "] 단어가 삭제되었습니다." << endl;

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
	}

	return;
}

/*
	wordList가 비어있는지 검사하는 함수
*/
bool checkWordListEmpty() {
	if (wordList.empty()) {
		cout << "단어가 없습니다. 먼저 단어를 추가하세요." << endl;
		cin.ignore(); // 버퍼 비우기
		return true;
	}
	return false;
}

/*
	전체 영단어에 대한 퀴즈
*/
void totalQuiz() {
	int goalCount; // 퀴즈에서 완료할 단어 수

	int right = 0; // 맞춘 문제 수
	int wrong = 0; // 틀린 문제 수

	if (checkWordListEmpty()) {
		return;
	}

	while (true) {
		cout << "퀴즈 문제 수를 입력하세요: ";
		cin >> goalCount;
		if (!cin) {
			cout << "정수를 입력해주세요." << endl;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			continue;
		}
		cin.ignore();

		if (goalCount <= 0) {
			system("cls");
			return;
		}
		else if (goalCount <= wordList.size())
			break;
		else
			cout << "저장된 단어 수 (" << wordList.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
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
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "정답!" << endl;
			cin.ignore();
			right++;
		}
		else {
			cout << "틀렸습니다." << endl;
			cin.ignore();
			wrong++;
			cout << "틀린 단어의 정답: " << wordList[randomIndex].eng << endl;
			if (!wordList[randomIndex].bookmarked) {
				cout << "해당 단어를 북마크에 추가 하시겠습니까? (Y/N)" << endl;
				string choice;
				while (true) {
					getline(cin, choice);
					choice = lowerString(choice);
					if (choice == "y") {
						wordList[randomIndex].bookmarked = true; // 오답 노트에 추가
						saveWordsToFile();                       // Word 객체 변환 후 즉시 txt 수정
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
				cout << "해당 단어는 이미 북마크에 존재합니다." << endl;
				cin.ignore();
			}
		}
	}

	system("cls");
	cout << "퀴즈 종료" << endl;
	cout << "전체 문제 수: " << goalCount << endl;
	cout << "맞은 문제 수: " << right << endl;
	cout << "틀린 문제 수: " << wrong << endl;
	cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;

	cin.ignore();
	return;
}
/*
	오답 영단어에 대한 퀴즈
*/
void bookmarkQuiz() {

	int goalCount; // 퀴즈에서 완료할 단어 수

	int right = 0; // 맞춘 문제 수
	int wrong = 0; // 틀린 문제 수

	if (checkWordListEmpty()) {
		return;
	}

	vector<int> bookmarkQuizIndexVector;

	for (int i = 0; i < wordList.size(); i++) {
		if (wordList[i].bookmarked)
			bookmarkQuizIndexVector.push_back(i);
	}

	if (bookmarkQuizIndexVector.size() == 0) {
		cout << "현재 오답 단어가 없습니다." << endl;
		cin.ignore();
		cin.get();
		return;
	}

	while (true) {
		cout << "퀴즈 문제 수를 입력하세요: ";
		cin >> goalCount;
		if (!cin) {
			cout << "정수를 입력해주세요." << endl;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			continue;
		}
		cin.ignore();

		if (goalCount <= 0) {
			system("cls");
			return;
		}
		else if (goalCount <= bookmarkQuizIndexVector.size())
			break;
		else {
			cout << "오답 문제 수 (" << bookmarkQuizIndexVector.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
		}
	}

	// bookmarkQuizIndexVector를 섞어서 인덱스가 랜덤한 순서로 등장하게 만들기
	random_device rd;
	mt19937 g(rd()); // Mersenne Twister pseudo - random generator
	shuffle(bookmarkQuizIndexVector.begin(), bookmarkQuizIndexVector.end(), g);

	for (int i = 0; i < goalCount; i++) {
		system("cls"); // 화면 지우기 (Windows)

		int randomIndex = bookmarkQuizIndexVector[i];
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
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "정답!" << endl;
			cin.ignore();
			right++;
			wordList[randomIndex].bookmarked = false; // 오답 노트에서 제거 맞추면 제거하는 방식
		}
		else {
			cout << "틀렸습니다." << endl;
			wrong++;
			cout << "틀린 단어의 정답: " << wordList[randomIndex].eng << endl; // 오답노트 퀴즈기에 따로 설정 X
			cin.ignore();
		}
	}

	system("cls");
	cout << "퀴즈 종료" << endl;
	cout << "전체 문제 수: " << goalCount << endl;
	cout << "맞은 문제 수: " << right << endl;
	cout << "틀린 문제 수: " << wrong << endl;
	cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;

	cin.ignore();
	return;
}

/*
퀴즈 메뉴 출력 및 선택용 함수 quiz()
*/
void quiz() {
	int menuSelect;

	while (1) {

		cout << "1. 전체 단어 퀴즈     2. 오답노트 퀴즈\n";
		cout << "메인 메뉴로 나가려면 0을 입력해주세요.\n";
		cin >> menuSelect;

		if (!cin) {				//정수 입력 안했을 때 처리
			cout << "정수를 입력해주세요." << endl;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			continue;
		}

		if (menuSelect == 1) {
			totalQuiz();
			return;
		}
		else if (menuSelect == 2) {
			bookmarkQuiz();
			return;
		}
		else if (menuSelect == 0) {
			return;
		}
		else {
			cout << "올바른 수를 입력하세요." << endl;
			cin.ignore();
		}

	}
}


/*
영어단어인지 확인하는 함수 (true 반환 시 조건 충족)
*/
bool checkEng(string str) {

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
한글 뜻인지 확인하는 함수 (true 반환 시 조건 충족)
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