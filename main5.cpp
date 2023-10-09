#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <locale>

using namespace std;

//�Լ� �������
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


class Word { // �ѱ���, ����, �ϸ�ũ ���θ� ���� Class Word
public:
	string eng;
	string kor;
	bool bookmarked;

	Word(const string& eng, const string& kor) : eng(eng), kor(kor), bookmarked(0) {}
	Word(const string& eng, const string& kor, bool& bookmarked) : eng(eng), kor(kor), bookmarked(bookmarked) {}
};

vector<Word> wordList; // Word ��ü ���� vector�� wordList


/*
txt���Ͽ��� string�� �о�ͼ� Word ��ü�� ��ȯ �� ����
*/
void loadWordsFromFile() {
	ifstream file("Word.txt");
	if (file.is_open()) {
		wordList.clear(); // ���� wordList ����
		string line;
		while (getline(file, line)) {
			size_t pos = line.find("/");
			size_t pos2 = line.rfind("/");

			if (pos == string::npos || pos2 == string::npos)
			{
				cout << "Word.txt ���� ���Ŀ� ������ �ֽ��ϴ�.\n���α׷��� �����մϴ�." << endl;
				file.close();
				return;
			}


			if (pos != string::npos) {
				string eng = line.substr(0, pos);
				if (checkEng(eng) == 0) {
					cout << "������ ������ ���� ��Ģ�� �߸��Ǿ����ϴ�.";
					exit(0);
				}
				string kor = line.substr(pos + 1, pos2 - pos - 1);
				wstring k = s2w(kor);
				if (checkKor(k) == 0) {
					cout << "������ ������ ���� ��Ģ�� �߸��Ǿ����ϴ�.";
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
		cerr << "Error: Word.txt�� �ҷ��� �� �����ϴ�." << endl;
	}
}


/*
���� ���α׷� ���� Word ��ü���� ȣ��� ���� �������� txt���Ͽ� �����ϴ� �Լ�
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
		cerr << "Error: Word.txt�� ������ �� �����ϴ�." << endl;
	}
}

int main() {
	setlocale(LC_ALL, "korean");

	int menuSelect;	// �޴� �Է¹������� ����
	loadWordsFromFile();

	while (1) {
		system("cls");
		cout << "1. ���ܾ� ����     2. ���ܾ� ���� \n3. ����\n";
		cin >> menuSelect;
		cin.ignore();

		if (menuSelect == 1) {			// ���ܾ� ���� ����
			quiz();
		}
		else if (menuSelect == 2) {		// ���ܾ��� ���� �޴� ����
			edit();
		}
		else if (menuSelect == 3) {		// ����
			cout << "���α׷��� �����մϴ�.";
			break;
		}
		else {
			cout << "1 ~ 3 �� �Է��ϼ���." << endl;
			cin.ignore();
		}

		if (!cin) {				//���� �Է� ������ �� ó��
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cin.get();
		}
	}

	return 0;
}

/*
	string ��ü�� lowercase�� ��ȯ (string���� ���ϱ� ������)
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
���ܾ� �߰� �� ���� �޴� edit()
*/
void edit() {
	loadWordsFromFile();

	cout << "���ܾ� �߰� - �߰��� �ܾ� �˻�" << endl;
	cout << "���ܾ� ���� - ������ �ܾ� �˻�" << endl;

	while (true) {

		string s;	// ���ܾ� �˻��Ѱ� �����Ϸ��� ����
		bool exist = false;
		auto it = wordList.begin();

		cout << "���ܾ� �Է�(����-Q): ";
		getline(cin, s);
		s = lowerString(s);

		if (!checkEng(s)) {  // ����ܾ� �����˻�, �ϴ� break �� �����س����� ���â�� ����� ����
			continue;
		}

		if (s == "q") {		// ����
			break;
		}

		// ���ܾ �����ϴ��� �˻�
		for (it = wordList.begin(); it != wordList.end(); ++it) {
			if (it->eng == s) {
				exist = true;
				break;
			}
		}

		if (!exist) {		// ���ܾ �������� ����

			cout << "[" << s << "] ��� ���ܾ�� �������� �ʽ��ϴ�." << endl;
			cout << "[" << s << "] �ܾ �߰��Ͻðڽ��ϱ�? (Y/N)" << endl;
			string choice;
			while (true) {
				getline(cin, choice);
				choice = lowerString(choice);

				if (choice == "y") {
					cout << s << "�� ���� �Է��ϼ���: ";
					wstring kor;
					cin.ignore();
					getline(wcin, kor);

					while (checkKor(kor) != 1) {
						cout << "�ٽ� �Է����ּ���." << endl;
						getline(wcin, kor);
					}

					string k = w2s(kor);

					wordList.push_back(Word(s, k));
					saveWordsToFile();

					cout << "[" << s << " - " << k << "] �ܾ �߰��߽��ϴ�." << endl;
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
		else {	// ���ܾ ����

			cout << "[" << it->eng << " - " << it->kor << "]" << endl;

			cout << "�ش� �ܾ �����Ͻðڽ��ϱ�? (Y/N)" << endl;
			string choice;
			while (true) {
				getline(cin, choice);
				choice = lowerString(choice);
				if (choice == "y") {

					string tmpkor = it->kor;

					wordList.erase(it);
					saveWordsToFile();

					cout << "[" << s << " - " << tmpkor << "] �ܾ �����Ǿ����ϴ�." << endl;

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
	}

	return;
}

/*
	wordList�� ����ִ��� �˻��ϴ� �Լ�
*/
bool checkWordListEmpty() {
	if (wordList.empty()) {
		cout << "�ܾ �����ϴ�. ���� �ܾ �߰��ϼ���." << endl;
		cin.ignore(); // ���� ����
		return true;
	}
	return false;
}

/*
	��ü ���ܾ ���� ����
*/
void totalQuiz() {
	int goalCount; // ����� �Ϸ��� �ܾ� ��

	int right = 0; // ���� ���� ��
	int wrong = 0; // Ʋ�� ���� ��

	if (checkWordListEmpty()) {
		return;
	}

	while (true) {
		cout << "���� ���� ���� �Է��ϼ���: ";
		cin >> goalCount;
		if (!cin) {
			cout << "������ �Է����ּ���." << endl;
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
			cout << "����� �ܾ� �� (" << wordList.size() << ") ���� ���� ���� ���� �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
	}

	vector<int> quizIndexVector;

	for (int i = 0; i < wordList.size(); i++) {
		quizIndexVector.push_back(i);
	}

	// quizIndexVector�� ��� �ε����� ������ ������ �����ϰ� �����
	random_device rd;
	mt19937 g(rd()); // Mersenne Twister pseudo - random generator
	shuffle(quizIndexVector.begin(), quizIndexVector.end(), g);

	for (int i = 0; i < goalCount; i++) {
		system("cls"); // ȭ�� ����� (Windows)

		int randomIndex = quizIndexVector[i];
		cout << "���� " << (i + 1) << "." << endl;
		cout << "�ѱ� ��: " << wordList[randomIndex].kor << endl;
		cout << "���� �ܾ �Է��ϼ��� (�޴��� ���ư����� 'Q' �Է�): ";
		string userInput;
		getline(cin, userInput);
		while (!checkEng(userInput)) {
			cout << "��Ģ�� ����Ǵ� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
			getline(cin, userInput);
		}
		userInput = lowerString(userInput);

		if (userInput == "q") {
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "����!" << endl;
			cin.ignore();
			right++;
		}
		else {
			cout << "Ʋ�Ƚ��ϴ�." << endl;
			cin.ignore();
			wrong++;
			cout << "Ʋ�� �ܾ��� ����: " << wordList[randomIndex].eng << endl;
			if (!wordList[randomIndex].bookmarked) {
				cout << "�ش� �ܾ �ϸ�ũ�� �߰� �Ͻðڽ��ϱ�? (Y/N)" << endl;
				string choice;
				while (true) {
					getline(cin, choice);
					choice = lowerString(choice);
					if (choice == "y") {
						wordList[randomIndex].bookmarked = true; // ���� ��Ʈ�� �߰�
						saveWordsToFile();                       // Word ��ü ��ȯ �� ��� txt ����
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
				cout << "�ش� �ܾ�� �̹� �ϸ�ũ�� �����մϴ�." << endl;
				cin.ignore();
			}
		}
	}

	system("cls");
	cout << "���� ����" << endl;
	cout << "��ü ���� ��: " << goalCount << endl;
	cout << "���� ���� ��: " << right << endl;
	cout << "Ʋ�� ���� ��: " << wrong << endl;
	cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;

	cin.ignore();
	return;
}
/*
	���� ���ܾ ���� ����
*/
void bookmarkQuiz() {

	int goalCount; // ����� �Ϸ��� �ܾ� ��

	int right = 0; // ���� ���� ��
	int wrong = 0; // Ʋ�� ���� ��

	if (checkWordListEmpty()) {
		return;
	}

	vector<int> bookmarkQuizIndexVector;

	for (int i = 0; i < wordList.size(); i++) {
		if (wordList[i].bookmarked)
			bookmarkQuizIndexVector.push_back(i);
	}

	if (bookmarkQuizIndexVector.size() == 0) {
		cout << "���� ���� �ܾ �����ϴ�." << endl;
		cin.ignore();
		cin.get();
		return;
	}

	while (true) {
		cout << "���� ���� ���� �Է��ϼ���: ";
		cin >> goalCount;
		if (!cin) {
			cout << "������ �Է����ּ���." << endl;
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
			cout << "���� ���� �� (" << bookmarkQuizIndexVector.size() << ") ���� ���� ���� ���� �� �����ϴ�. �ٽ� �Է����ּ���." << endl;
		}
	}

	// bookmarkQuizIndexVector�� ��� �ε����� ������ ������ �����ϰ� �����
	random_device rd;
	mt19937 g(rd()); // Mersenne Twister pseudo - random generator
	shuffle(bookmarkQuizIndexVector.begin(), bookmarkQuizIndexVector.end(), g);

	for (int i = 0; i < goalCount; i++) {
		system("cls"); // ȭ�� ����� (Windows)

		int randomIndex = bookmarkQuizIndexVector[i];
		cout << "���� " << (i + 1) << "." << endl;
		cout << "�ѱ� ��: " << wordList[randomIndex].kor << endl;
		cout << "���� �ܾ �Է��ϼ��� (�޴��� ���ư����� 'Q' �Է�): ";

		string userInput;
		getline(cin, userInput);
		while (!checkEng(userInput)) {
			cout << "��Ģ�� ����Ǵ� �Է��Դϴ�. �ٽ� �Է����ּ��� : ";
			getline(cin, userInput);
		}
		userInput = lowerString(userInput);

		if (userInput == "q") {
			break;
		}

		if (userInput == lowerString(wordList[randomIndex].eng)) {
			cout << "����!" << endl;
			cin.ignore();
			right++;
			wordList[randomIndex].bookmarked = false; // ���� ��Ʈ���� ���� ���߸� �����ϴ� ���
		}
		else {
			cout << "Ʋ�Ƚ��ϴ�." << endl;
			wrong++;
			cout << "Ʋ�� �ܾ��� ����: " << wordList[randomIndex].eng << endl; // �����Ʈ ����⿡ ���� ���� X
			cin.ignore();
		}
	}

	system("cls");
	cout << "���� ����" << endl;
	cout << "��ü ���� ��: " << goalCount << endl;
	cout << "���� ���� ��: " << right << endl;
	cout << "Ʋ�� ���� ��: " << wrong << endl;
	cout << "�����: " << (right / (double)goalCount) * 100 << "%" << endl;

	cin.ignore();
	return;
}

/*
���� �޴� ��� �� ���ÿ� �Լ� quiz()
*/
void quiz() {
	int menuSelect;

	while (1) {

		cout << "1. ��ü �ܾ� ����     2. �����Ʈ ����\n";
		cout << "���� �޴��� �������� 0�� �Է����ּ���.\n";
		cin >> menuSelect;

		if (!cin) {				//���� �Է� ������ �� ó��
			cout << "������ �Է����ּ���." << endl;
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
			cout << "�ùٸ� ���� �Է��ϼ���." << endl;
			cin.ignore();
		}

	}
}


/*
����ܾ����� Ȯ���ϴ� �Լ� (true ��ȯ �� ���� ����)
*/
bool checkEng(string str) {

	str = lowerString(str);

	if (str.size() > 30) {
		return 0;				//���ڿ� ���� �ִ� 30 ����
	}

	bool hasEng = false;		//���ĺ� �ϳ����� �����̶� '-'�� �ִ��� üũ�ϱ�����
	for (char c : str) {
		if (!((c >= 'a' && c <= 'z') || (c == ' ') || (c == '-'))) {
			return 0;
		}
		if (c >= 'a' && c <= 'z') {
			hasEng = true;		//���ĺ� �ּ� 1���� �߰��ϸ� hasEng�� true�� ��ȯ
		}
	}

	if (!hasEng) return 0;		//���ĺ� �ϳ��� ������ ���ܾ� ���� �������� ���ϹǷ�

	return 1;  // ���� ���ĺ� ������ ������Ŵ
}

/*
�ѱ� ������ Ȯ���ϴ� �Լ� (true ��ȯ �� ���� ����)
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