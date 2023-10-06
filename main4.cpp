#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

//함수 헤더선언
void edit();
void totalQuiz();
void bookmarkQuiz();
void quiz();

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
			if (pos != string::npos) {
				string eng = line.substr(0, pos);
				string kor = line.substr(pos + 1, pos2-pos-1);
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

	int menuSelect;	// 메뉴 입력받으려고 선언
	loadWordsFromFile();

	while (1) {
		system("cls");
		cout << "1. 영단어 퀴즈     2. 영단어 수정 \n3. 종료\n";
		cin >> menuSelect;

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
		}

		if (!cin) {				//정수 입력 안했을 때 처리
			cin.clear();
			cin.ignore(INT_MAX, '\n');
		}
	}

	return 0;
}

/*
	string 전체를 lowercase로 변환 (string끼리 비교하기 쉽도록)
*/
string lowerString(string& str) {

	size_t start = str.find_first_not_of(" \t");
	if (start == string::npos) { return ""; }
	size_t end = str.find_last_not_of(" \t");
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
		cin >> s;
		s = lowerString(s);

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
				cin >> choice;
				choice = lowerString(choice);

				if (choice == "y") {
					cout << s << "의 뜻을 입력하세요: ";
					string k;
					cin >> k;

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
				cin >> choice;
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

		if (goalCount <= 0) {
			system("cls");
			cin.ignore();
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
		cout << "문제 " << (i+1) << "." << endl;
		cout << "한글 뜻: " << wordList[randomIndex].kor << endl;
		cout << "영어 단어를 입력하세요 (메뉴로 돌아가려면 'Q' 입력): ";
		string userInput;
		cin >> userInput;
		userInput = lowerString(userInput);

		if (userInput == "q") {
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "정답!" << endl;
			right++;
			cin.ignore();
			cin.get();
		}
		else {
			cout << "틀렸습니다." << endl;
			wrong++;
			cout << "틀린 단어의 정답: " << wordList[randomIndex].eng << endl;
			if (!wordList[randomIndex].bookmarked) {
				cout << "해당 단어를 북마크에 추가 하시겠습니까? (Y/N)" << endl;
				string choice;
				while (true) {
					cin >> choice;
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
			}
			cin.ignore();
			cin.get();
		}
	}
	
	system("cls");
	cout << "퀴즈 종료" << endl;
	cout << "전체 문제 수: " << goalCount << endl;
	cout << "맞은 문제 수: " << right << endl;
	cout << "틀린 문제 수: " << wrong << endl;
	cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;

	cin.ignore(); // 버퍼 비우기
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
		if(wordList[i].bookmarked)
			bookmarkQuizIndexVector.push_back(i);
	}

	if (bookmarkQuizIndexVector.size() == 0) {
		cout << "현재 오답 단어가 없습니다." << endl;
		cin.ignore(); // 버퍼 비우기
		cin.get();
		return;
	}

	while (true) {
		cout << "퀴즈 문제 수를 입력하세요: ";
		cin >> goalCount;

		if (goalCount <= 0) {
			system("cls");
			cin.ignore();
			return;
		}
		else if (goalCount <= bookmarkQuizIndexVector.size())
			break;
		else
			cout << "오답 문제 수 (" << bookmarkQuizIndexVector.size() << ") 보다 퀴즈 문제 수가 더 많습니다. 다시 입력해주세요." << endl;
	}

	// bookmarkQuizIndexVector를 섞어서 인덱스가 랜덤한 순서로 등장하게 만들기
	random_device rd;
	mt19937 g(rd()); // Mersenne Twister pseudo - random generator
	shuffle(bookmarkQuizIndexVector.begin(), bookmarkQuizIndexVector.end(), g);

	for (int i = 0; i < bookmarkQuizIndexVector.size(); i++) {
		system("cls"); // 화면 지우기 (Windows)
		int randomIndex = bookmarkQuizIndexVector[i];

		cout << "문제 " << (i + 1) << "." << endl;
		cout << "한글 뜻: " << wordList[randomIndex].kor << endl;
		cout << "영어 단어를 입력하세요 (메뉴로 돌아가려면 'Q' 입력): ";
		string userInput;
		cin >> userInput;
		userInput = lowerString(userInput);

		if (userInput == "q") {
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "정답!" << endl;
			right++;
			wordList[randomIndex].bookmarked = false; // 오답 노트에서 제거 // <-- 맞추면 제거하는 방식으로 할 것인지?
			cin.ignore();
			cin.get();
		}
		else {
			cout << "틀렸습니다." << endl;
			wrong++;
			cout << "틀린 단어의 정답: " << wordList[randomIndex].eng << endl; // 오답노트 퀴즈기에 따로 설정 X
			cin.ignore();
			cin.get();
		}
	}

	system("cls");
	cout << "퀴즈 종료" << endl;
	cout << "전체 문제 수: " << goalCount << endl;
	cout << "맞은 문제 수: " << right << endl;
	cout << "틀린 문제 수: " << wrong << endl;
	cout << "정답률: " << (right / (double)goalCount) * 100 << "%" << endl;

	cin.ignore(); // 버퍼 비우기
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
		}

		if (!cin) {				//정수 입력 안했을 때 처리
			cin.clear();
			cin.ignore(INT_MAX, '\n');
		}
	}
}
