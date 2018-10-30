/*
2018-2 전공기초프로젝트2(2040) 중간평가
6조 : 김나경, 방승희, 왕윤성
주제 : 음료자판기 가상 시뮬레이션 프로그램
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#define DRINK_NAME_MAX_LENGTH 20            //음료 이름 최대 길이 영문기준 20자
#define DRINK_MAX_SLOT 10               //자판기 슬롯 최대 10개
#define DRINK_MAX_STOCK 15                  //자판기 슬롯 한 개당 최대 재고 수 15개
#define MONEY_100 100                  //100원
#define MONEY_500 500                  //500원
#define MONEY_1000 1000                     //1000원
#define MONEY_100_MAX 100               //자판기 금고에 담을 수 있는 100원 최대 개수
#define MONEY_500_MAX 50               //자판기 금고에 담을 수 있는 500원 최대 개수
#define MONEY_1000_MAX 50               //자판기 금고에 담을 수 있는 1000원 최대 개수
#define TIME_LIMIT 30                  //사용자 슬롯 번호 입력 제한 시간
const char* filename = "machine_data.txt";  //자판기 데이터를 저장할 파일 이름
using namespace std;

bool exception_test(int num, int max, int min);

//음료 클래스
class Drink {
public:
	//멤버 변수
	bool _isRegist;                     //등록된 슬롯 음료인가를 검사
	char _name[DRINK_NAME_MAX_LENGTH + 1];  //음료의 이름
	int _price;                        //음료의 가격
	int _stock;                        //음료의 재고

					//멤버 함수
					//생성자 - 아무것도 값을 넣지 않은 상태로 초기화하는 역할
	Drink() {
		_isRegist = false;                  //아무런 값을 넣지 않은 즉, 슬롯에 등록되지 상태
						//이름을 저장하는 char배열을 21자짜리 null로 초기화 함.
		memset(_name, 0, sizeof(char) * (DRINK_NAME_MAX_LENGTH + 1));
		_price = 0;
		_stock = 0;
	}

	//생성자 오버로딩 - 음료에 값을 등록할 시 넘겨진 파라미터로 값 변경해주는 역할
	Drink(char* name, int price, int stock) {
		_isRegist = true;               //값을 넣었으므로 true
					  /*for (int i = 0; i < DRINK_NAME_MAX_LENGTH; i++)
					  _name[i] = name[i];*/
		strcpy_s(_name, name);
		_price = price;
		_stock = stock;
	}

	//재고를 변경하는 함수
	void set_stock(int stock) {
		_stock = stock;
	}
};

//돈 클래스
class Money {
public:
	//멤버 변수
	int _100 = 0;                     //100원의 개수
	int _500 = 0;                     //500원의 개수
	int _1000 = 0;                     //1000원의 개수
	int _total = 0;                     //토탈 가격

					//멤버 함수
					//생성자 - 초기화
	Money() {
		_100 = 0;
		_500 = 0;
		_1000 = 0;
		_total = 0;
	}

	//생성자 오버로딩
	Money(int input_100, int input_500, int input_1000) {
		_100 = input_100;
		_500 = input_500;
		_1000 = input_1000;
		_total = MONEY_100 * _100 + MONEY_500 * _500 + MONEY_1000 * _1000;
	}

	//돈의 정보를 변경하는 함수. 멤버 변수의 값을 변경함.
	//원래 있던 값에서 들어오는 파라미터 만큼 추가/제거하는 형식.
	void edit_money(Money* income) {
		_100 += income->_100;
		_500 += income->_500;
		_1000 += income->_1000;
		_total += income->_total;
	}
};

//기계 클래스 - 싱글톤 패턴
class Machine {
private:
	//멤버 함수
	//생성자 - 초기화
	Machine() {
		_drink = new Drink[DRINK_MAX_SLOT]();
		_mmoney = new Money();
		input = new Money();
		change = new Money();
		mode = 0;
	};
	Machine(const Machine& m);
	~Machine() {};

	//멤버 변수

	static Machine* instance;
	//최초로 호출한 인스턴스를 가리키는 포인터
	//이후 Machine 인스턴스를 다시 생성시 항상 이 포인터로 리턴해 줌.
	//Machine클래스 정의 아래에서 초기화함.

public:
	//멤버 변수
	Drink * _drink;                     //Machine이 판매하는 음료
	Money* _mmoney;                     //Machine의 금고
	bool available;                     //Machine의 잔돈 여부
	Money *input;                     //사용자가 입력한 금액 저장
	Money *change;                     //사용자에게 거슬러줄 금액
	int mode;                        //사용자 모드, 관리자 모드

				   //멤버 함수
				   //싱글톤 패턴 - 이후 생성시에도 계속 처음의 instance만 가리키게해 새로운 인스턴스 생성을 막음.
	static Machine* getInstance() {
		if (instance == NULL)               //instance가 NULL이면 새 인스턴스를 생성.
			instance = new Machine();       //instance가 새 인스턴스를 가리키게 한 후
		return instance;               //instance 리턴. NULL이 아니면 그 instance 리턴.
	}

	//첫 번째로 나오는 빈 슬롯 리턴 - 이 프로그램에서는 전체에 빈 슬롯이 있는지를 체크하는 용도로만 사용.
	//즉, 리턴 값은 -1이 아니면 의미가 없음.
	int check_blank_slot() {
		for (int i = 0; i < DRINK_MAX_SLOT; i++) {
			if (!_drink[i]._isRegist) {
				return i;
			}
		}
		return -1;
	}

	//입력받은 슬롯 번호로부터 투입금액과 선택한 음료가격 차액 계산
	int calculate(int slot_num) {
		int change_total = input->_total - _drink[slot_num - 1]._price;
		return change_total;
	}

	//잔돈의 토탈 금액을 파라미터로 받고, 금고 잔돈 상황을 고려한 최적 반환 방법 계산 후 돌려줄 수 있는지를 판단
	//돌려줄 수 있다면 return true, 돌려줄 수 없다면 return false
	bool optimal_change(int tmp) {
		change = new Money();                  //돌려줄 돈 인스턴스 초기화. (Machine 생성시에도, main에 사용자에게 투입받기 전후에 초기화를 하지만, 안전 상 여기서도 한번 더 초기화 해줌.)
		int change_1000, change_500, change_100;   //돌려줄 돈 갯수 저장할 변수


		change_1000 = tmp / MONEY_1000;            //1000원으로 돌려줄 수 있는 최대 개수 계산
		if (_mmoney->_1000 < change_1000) {         //금고 안의 1000원 개수와 비교해 금고 1000원이 적으면
			change_1000 = _mmoney->_1000;         //금고의 1000원 개수 만큼만 돌려준다 (능력만큼)
		}
		tmp -= change_1000 * MONEY_1000;         //돌려줄 수 있는 금액을 tmp에서 뺀다


						  //1000원과 마찬가지의 과정으로 500원 계산
		change_500 = tmp / MONEY_500;
		if (_mmoney->_500 < change_500) {
			change_500 = _mmoney->_500;
		}
		tmp -= change_500 * MONEY_500;

		//1000원과 마찬가지의 과정으로 100원 계산
		change_100 = tmp / MONEY_100;
		if (_mmoney->_100 < change_100) {
			//이때는 마지막 100원 단위가 돌려줄 개수가 모자란 것이므로 잔돈이 없는게 됨
			//투입된 돈 돌려주기 - 리턴한 후에 밖에서
			//잔돈 부족
			//cout << "optimal function : 잔돈 없어!!" << endl;
			return false;
		}
		else {
			tmp -= change_100 * MONEY_100;
		}

		if (tmp == 0) {
			//tmp==0인 것은 돈을 돌려줄 수 있다는 것을 나타냄.
			//change를 돌려줄 돈으로 set
			change->edit_money(new Money(change_100, change_500, change_1000));
			return true;
		}
		else {
			//사실 상 들어올 일 없는 분기. 잔돈을 줄 수 있으면서 tmp==0 일 수 없음.
			//투입된 돈 돌려주기 - 리턴한 후에 밖에서
			//Something Wrong
			cout << "optimal function : Something Wrong!!" << endl;
			return false;
		}

	}

	//금고 상태를 보여줌.
	void show_money() {
		cout << "금고" << endl;
		cout << "100원 " << _mmoney->_100 << "개 ";
		cout << "500원 " << _mmoney->_500 << "개 ";
		cout << "1000원 " << _mmoney->_1000 << "개 " << endl;
		cout << "Total : " << _mmoney->_total << "원" << endl << endl;
	}

	//자판기 음료 리스트를 보여줌.
	void show_drink() {
		cout << endl;
		for (int i = 0; i < DRINK_MAX_SLOT; i++) {
			//슬롯 번호 출력부
			//width를 사용해서 표를 만들었다.
			cout.width(3);
			cout.fill(' ');
			cout << i + 1 << ". ";

			//등록된 슬롯일 경우
			if (_drink[i]._isRegist) {
				//음료 이름 출력부
				cout.width(DRINK_NAME_MAX_LENGTH + 1);
				cout.fill(' ');
				cout << _drink[i]._name;
				cout << " | ";
				//음료 가격 출력부
				cout.width(5);
				cout.fill(' ');
				cout << _drink[i]._price << "원 ";
				cout << " | ";
				//관리자라면, 재고 상황까지 출력
				if (mode == 1) {
					cout.width(5);
					cout.fill(' ');
					cout << _drink[i]._stock << "/" << DRINK_MAX_STOCK;
				}
				//사용자라면, 재고 없을 때만 알려줌
				else if (mode == 2 && _drink[i]._stock == 0) {
					cout.width(5);
					cout.fill(' ');
					cout << "재고 없음";
				}
			}
			//등록되지 않은 슬롯일 경우
			else {
				cout.width(DRINK_NAME_MAX_LENGTH + 1);
				cout.fill(' ');
				cout << "미등록 슬롯";
				cout << " | ";
			}
			cout << endl;
		}
		//사용자 모드라면, 반환 레버까지 출력함.
		if (mode == 2) {
			cout.width(3);
			cout.fill(' ');
			cout << DRINK_MAX_SLOT + 1 << ". ";
			cout.width(DRINK_NAME_MAX_LENGTH + 1);
			cout.fill(' ');
			cout << "잔돈 반환 레버" << endl;
		}
		cout << endl;
	}

	//사용자로부터 투입 금액 갱신
	void get_money(Money* m) {
		//사용자로부터 투입
		input->edit_money(m);
		//기계 금고에 추가
		_mmoney->edit_money(input);
	}

	//멤버 변수 change에 따라서 반환을 해주는 함수
	//현재 금고에서 그만큼의 돈을 뺌
	//그 후 멤버 변수 input과 change를 초기화
	void return_change() {
		//임시로 뺄 돈의 정보를 (-)로 저장한 Money 객체.
		Money* tmp = new Money(-change->_100, -change->_500, -change->_1000);

		//금고의 정보를 편집함. tmp만큼 변경.
		_mmoney->edit_money(tmp);

		//돌려준 잔액 상황을 보여줌.
		cout << endl;
		cout << "잔돈 : " << change->_total << "원" << endl << endl;

		cout << "동전류 잔돈 출구" << endl;
		cout << "100원 : " << change->_100 << "개 ";
		cout << "500원 : " << change->_500 << "개 " << endl << endl;
		cout << "지폐 투입구(지류 잔돈 출구)" << endl;
		cout << "1000원 : " << change->_1000 << "개 " << endl << endl;
		//초기화
		input = new Money();
		change = new Money();
	}
};
Machine* Machine::instance = nullptr;

//사용자 클래스
class User {
public:
	//멤버 함수
	//사용자가 돈을 입력하는 함수
	Money * input_money() {
		Money* tmp;
		Machine* M = Machine::getInstance();

		bool input_test = false;            //입력 예외 처리용 flag
		int tmp100 = 0, tmp500 = 0, tmp1000 = 0;

		cout << "동전 투입구 (100원/500원)" << endl;
		//예외처리용 반복문
		while (input_test == false) {
			cout << "100원 개수>";
			cin >> tmp100;
			if ((tmp100 + M->_mmoney->_100) > MONEY_100_MAX) {
				cout << "기계에 들어갈 수 있는 100원 동전 양을 초과하였습니다." << endl << endl;
				//tmp100을 음수로 설정함 : 사용자의 투입이 음수인 경우는 금고가 넘쳐서 돌려줘야함을 뜻한다.
				tmp100 = -tmp100;
				//더이상 500원이나 1000원 입력을 받지 않고 바로 return
				return tmp = new Money(tmp100, 0, 0);
			}
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(tmp100, MONEY_100_MAX - (M->_mmoney->_100), 0);
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;

		//예외처리용 반복문
		while (input_test == false) {
			cout << "500원 개수>";
			cin >> tmp500;
			if ((tmp500 + M->_mmoney->_500) > MONEY_500_MAX) {
				cout << "기계에 들어갈 수 있는 500원 동전 양을 초과하였습니다." << endl;
				//tmp500을 음수로 설정함 : 사용자의 투입이 음수인 경우는 금고가 넘쳐서 돌려줘야함을 뜻한다.
				tmp500 = -tmp500;
				//더이상 1000원의 입력을 받지 않고 바로 return
				return tmp = new Money(tmp100, tmp500, 0);
			}
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(tmp500, MONEY_500_MAX - (M->_mmoney->_500), 0);
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;
		cout << endl << "지폐 투입구 (1000원)" << endl;

		//예외처리용 반복문
		while (input_test == false) {
			cout << "1000원 개수>";
			cin >> tmp1000;
			if ((tmp1000 + M->_mmoney->_1000) > MONEY_1000_MAX) {
				cout << "기계에 들어갈 수 있는 지폐 양을 초과하였습니다." << endl;
				//tmp1000을 음수로 설정함 : 사용자의 투입이 음수인 경우는 금고가 넘쳐서 돌려줘야함을 뜻한다.
				tmp1000 = -tmp1000;
				//바로 return
				return tmp = new Money(tmp100, tmp500, tmp1000);
			}
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(tmp1000, MONEY_1000_MAX - (M->_mmoney->_1000), 0);
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;

		tmp = new Money(tmp100, tmp500, tmp1000);
		return tmp;
	}

	//사용자가 주문할 슬롯의 번호를 입력하는 함수
	bool select_slot(int *slot) {
		bool input_test = false;            //입력 예외 처리용 flag	

		//시간을 재기 위해서 만든 time변수, 이때 스타트 타임 시작!(엔드 타임과 스타트 타임의 차이를 사용)
		int start_time = (unsigned)time(0);
		int end_time;

		char tmp_slot[3];
		//string temp_s_slot;

		cout << "슬롯 번호>";
		while (1) {
			end_time = (unsigned)time(0);            //이 때 엔드 타임 시작!
			//키보드 입력이 들어오면 아래의 if문 안으로 들어온다.
			if (_kbhit()) {
				//cin >> *slot;
				//getline함수는 두번째 매개변수로 받는 사이즈만큼의 입력만 첫번째 매개변수의 주소로 받고, 나머지는 버린다. 
				cin.getline(tmp_slot, 3);

				if (cin.fail()) {
					cin.clear();
					//사용자가 INT_MAX(2147483647)개 이상의 알파벳을 치지 않는 이상 버퍼 문제를 해결해준다.
					//cin.ignore함수의 첫번째 매개변수는 버릴 문자의 수, 두번째 매개변수는 
					cin.ignore(INT_MAX, '\n');
					//cout << "her ost" << endl;
					//스타트 타임을 다시 지정해준다.
					start_time = (unsigned)time(0);
					cout << "다시 입력해주세요" << endl;
					cout << "슬롯 번호>";
					//반복문의 남은 코드를 무시하고 반복문 가장 위로 점프한다. 
					continue;
				}
				//temp_s_slot = tmp_slot;

				//입력받은 값의 첫번째 문자가 0일때
				if (tmp_slot[0] == '0') {
					//cout << "ㅗㅑㅗㅑㅗㅑ" << endl;
					//cout << "test = " << temp_s_slot.at(1) << endl;
					//cout << "test = " << (int)(temp_s_slot.at(1))-'0' << endl;
					//01~09입력 경우
					if (tmp_slot[1] > '0' && tmp_slot[1] <= '9') {
						*slot = (int)tmp_slot[1] - '0';
						//cout << *slot << endl;
						return true;
					}
				}
				//입력받은 값의 첫번째 문자가 1~9일때
				else if (tmp_slot[0] > '0' && tmp_slot[0] <= '9' && tmp_slot[1] == NULL) {
					*slot = (int)tmp_slot[0] - '0';
					//cout << *slot << endl;
					return true;
				}
				//10입력 한 경우
				if (tmp_slot[0] == '1'&&tmp_slot[1] == '0') {
					*slot = 10;
					return true;
				}
				//10입력 한 경우
				if (tmp_slot[0] == '1'&&tmp_slot[1] == '1') {
					*slot = 11;
					return true;
				}
				//스타트 타임을 다시 지정해준다.
				start_time = (unsigned)time(0);
				cout << "다시 입력해주세요" << endl;
				cout << "슬롯 번호>";
			}
			//지정해놓은 시간이 지나면 false를 리턴한다.
			if (end_time - start_time > TIME_LIMIT) {
				*slot = 0;
				//false를 리턴하면 사용자가 넣은 돈이 그대로 반환되게 한다.
				return false;
			}

		}
	}
};

//관리자 클래스
class Admin {
public:
	//멤버함수
	//관리자가 기계금고의 돈 정보를 바꿀 때 호출되는 함수
	Money * edit_money() {
		//입력한 수정 정보를 저장할 변수
		Money* tmp = new Money();
		//메인에서 미리 생성해둔 Machine 인스턴스
		Machine* M = Machine::getInstance();

		bool input_test = false;            //입력 예외 처리용 flag
		int tmp100, tmp500, tmp1000;

		cout << "(100 500 1000) 순서로, 추가는 양의 정수, 제거는 음의 정수 입력\n";
		while (input_test == false) {
			cout << "100원 개수>";
			cin >> tmp100;
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(tmp100, MONEY_100_MAX - (M->_mmoney->_100), -(M->_mmoney->_100));
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;

		while (input_test == false) {
			cout << "500원 개수>";
			cin >> tmp500;
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(tmp500, MONEY_500_MAX - (M->_mmoney->_500), -(M->_mmoney->_500));
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;

		while (input_test == false) {
			cout << "1000원 개수>";
			cin >> tmp1000;
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(tmp1000, MONEY_1000_MAX - (M->_mmoney->_1000), -(M->_mmoney->_1000));
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;

		tmp->edit_money(new Money(tmp100, tmp500, tmp1000));
		return tmp;
	}

	//관리자가 기계 슬롯에 음료를 등록할 때 호출되는 함수 - 주석처리 더 세심하게 해주세요. 각 변수가 뭘 뜻하는지
	Drink regist_slot() {
		char name[DRINK_NAME_MAX_LENGTH + 1];
		int price;
		int stock;

		bool input_test = false;            //입력 예외 처리용 flag

		while (1) {
			memset(name, 0, sizeof(char)*(DRINK_NAME_MAX_LENGTH + 1));
			cout << "name> ";
			//getline함수는 두번째 매개변수로 받는 사이즈만큼의 입력만 첫번째 매개변수의 주소로 받고, 나머지는 버린다. 
			cin.getline(name, DRINK_NAME_MAX_LENGTH + 1);

			/*
			if (1)
			{
			cout << "hihi" << endl;
			continue;
			}*/

			//입력된 값이 NULL일 때 strcmp함수에서 첫번째 매개변수와
			//두번째 매개변수가 같으면 0을 리턴하므로 if문으로 처리
			if (strcmp(name, "") == 0) {
				cout << "이름을 입력하지 않으셨습니다." << endl;
				//다시 반복문 맨 위로 올라옴
				continue;
			}

			if (cin.fail()) {
				cin.clear();
				//사용자가 INT_MAX(2147483647)개 이상의 알파벳을 치지 않는 이상 버퍼 문제를 해결해준다.
				//cin.ignore함수의 첫번째 매개변수는 버릴 문자의 수, 두번째 매개변수는 
				cin.ignore(INT_MAX, '\n');
			}

			//cout << "====" << name << endl;

			while (input_test == false) {
				cout << "price> ";
				cin >> price;
				//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
				input_test = exception_test(price, 2000, 100);

				//입력한 가격이 100원단위가 아니면
				if (price % 100 != 0) {
					cout << "가격은 100원 단위로 입력해주세요." << endl;
					input_test = false;
				}
			}
			//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
			input_test = false;

			while (input_test == false) {
				cout << "stock> ";
				cin >> stock;
				//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
				input_test = exception_test(stock, INT_MAX, 0);
			}
			//만약 관리자가 재고를 15보다 많게 입력했을 경우
			if (stock > DRINK_MAX_STOCK) {
				cout << "재고의 최대값(15개)보다 많이 입력하여 재고는 15로 편집됩니다." << endl;
				stock = 15;
			}
			//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
			input_test = false;
			break;
		}
		cout << endl;

		cout << "결과" << endl;
		cout << "이름 : " << name << endl;
		cout << "가격 : " << price << endl;
		cout << "재고 : " << stock << endl << endl;

		return Drink(name, price, stock);

	}

	//관리자가 슬롯의번호를 입력해야 할 때, 재고의 수를 입력해야할 때 호출되는 함수
	int select(int choice) {
		int select_num;
		bool input_test = false;            //입력 예외 처리용 flag

		//예외처리를 위한 루프
		while (input_test == false) {
			if (choice == 0) {
				cout << "슬롯 번호>";
				cin >> select_num;
				//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
				input_test = exception_test(select_num, DRINK_MAX_SLOT, 1);
			}
			else if (choice == 1) {
				cout << "변경할 재고 수>";
				cin >> select_num;
				//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
				input_test = exception_test(select_num, INT_MAX, 0);
			}

		}

		//만약 재고 입력이면서, 관리자가 재고를 15보다 많게 입력했을 경우
		if (choice == 1 && select_num > DRINK_MAX_STOCK) {
			cout << "재고의 최대값(15개)보다 많이 입력하여 재고는 15로 편집됩니다." << endl;
			select_num = 15;
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;
		/*
		while (1){
		cin.get(ch);
		if (cin.fail() || isalpha(ch) || ispunct(ch) || isblank(ch)){
		cin.clear();
		while ((ch = cin.get() != '\n') && ch != EOF);
		cout << "잘못입력하셨습니다." << endl << endl;
		}
		break;
		}*/

		return select_num;
	}
};

//모듈 1 예외처리 함수(num이 min~max사이의(이상, 이하) int형 값이면 true
bool exception_test(int num, int max, int min) {
	//test1, test2, test3, test4라는 0이 아닌 int형 변수를 선언한다.
	//만약 예외 상황의 조건문에 들어가면 4개중의 하나의 변수의 값을 0으로 바꾼다.
	//어떠한 예외 상황에도 들어가지 않은 입력값만이 test1~4의 값이 모두 0이 아닐 것이다.
	//임의의 수*0은 무조건 0이 나오는 수학적 개념을 이용하여 test1~4를 모두 곱한 final_test라는 변수를 선언
	//final_test의 값이 0이 아니면 맞는 값이 입력받은것으로 판단하여 return true를 한다!
	int test1 = 1, test2 = 1, test3 = 1, test4 = 1;
	int final_test;

	if (cin.fail()) {
		//cout << "땡" << endl;
		test1 = 0;
		while (cin.get() != '\n') {
			//cout << "땡땡땡땡" << endl;
			cin.clear();   //버퍼 비워주고
			test2 = 0;
		}
	}
	else {
		while (cin.get() != '\n') {
			//cout << "땡땡땡" << endl;
			test3 = 0;
			cin.clear();   //버퍼 비워주고
		}
		//cout << "뚱뚱뚱" << endl;
		test4 = 1;
	}

	final_test = test1 * test2 * test3 * test4;
	if (final_test != 0) {
		if (num >= min && num <= max) {
			//cout << "맞는값!!!!!!!!!" << endl;
			return true;
		}
		else {
			//범위 오류일 때 올바른 범위가 무엇인지 알려준다. 
			if (max == INT_MAX)
				max = 15;
			cout << min << "~" << max << "사이의 값을 입력해주세요" << endl << endl;
			return false;
		}
	}
	else {
		//아예 정수를 입력하지 않았을 때
		cout << "숫자 입력해주세요" << endl << endl;
		return false;
	}
}

//모듈 2-(1) 처음 프로그램 실행시 데이터 파일을 읽어와서 Machine 객체에 저장하는 함수.
//파일이 정상적으로 읽어와졌다면 return true, 그렇지 않다면 return false
bool file_read_function() {

	//읽기용 파일 변수
	ifstream datafile(filename);

	//파일로부터 한줄씩 읽어올 데이터를 임시저장할 char배열
	char buffer[DRINK_NAME_MAX_LENGTH + 1];      //파일에서 제일 긴 항목이 최대 20자인 음료이름이므로 그를 기준으로 버퍼 크기 잡기.
						//+1은 공백

						//처음으로 프로젝트를 돌려보면, 파일이 없으므로 바로 return true
	if (!datafile) {
		cout << "데이터를 정상적으로 불러왔습니다." << endl << endl;
		return true;
	}

	//파일이 정상적으로 열리지 않았다면 return false
	if (!datafile.is_open()) {
		cout << "데이터 불러오기에 실패했습니다.. 정상적인 작동을 위해 다시 실행해주세요." << endl;
		return false;
	}

	//Machine 인스턴스 가져옴. main함수에서 맨 처음 생성한 그 인스턴스임.
	Machine* M = Machine::getInstance();

	//아래는 쭈욱 미리 설계해 둔 파일 형식에 맞게 읽어오는 코드
	//한줄씩 읽어오는 코드의 반복

	//금고 정보 저장
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	int _100 = atoi(buffer);               //금고 100원 갯수
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	int _500 = atoi(buffer);               //금고 500원 갯수
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	int _1000 = atoi(buffer);               //금고 1000원 갯수
	M->_mmoney = new Money(_100, _500, _1000);


	//음료 정보 저장
	for (int i = 0; i < DRINK_MAX_SLOT; i++) {
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		M->_drink[i]._isRegist = atoi(buffer);   //등록된 음료인가 아닌가의 정보. bool형태의 정보이므로 atoi로 int로 변환
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		if (M->_drink[i]._isRegist)            //등록된 음료라면 이름 읽어와서 저장. 아니라면 어짜피 null이므로 저장할 필요가 없음
			strcpy_s(M->_drink[i]._name, buffer);
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		if (M->_drink[i]._isRegist)            //등록된 음료라면 가격 읽어와서 저장.
			M->_drink[i]._price = atoi(buffer);
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		if (M->_drink[i]._isRegist)            //등록된 음료라면 재고 읽어와서 저장.
			M->_drink[i]._stock = atoi(buffer);
	}
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	//마지막 개행 의미없이 읽음 -> 마지막까지 읽었는지 확인하는 분기를 위해

	//파일의 마지막까지 읽었는지 검사하는 분기.
	//여기가 마지막이 아니라면 파일이 비정상적으로 저장된 것.
	if (datafile.eof()) {
		//파일 닫기
		datafile.close();
		cout << "데이터를 정상적으로 불러왔습니다." << endl << endl;
		return true;
	}
	else {
		//파일 닫기
		datafile.close();
		cout << "파일이 비정상적으로 저장됐습니다. 확인바랍니다." << endl << endl;
		return false;
	}
}

//모듈 2-(2) 마지막 프로그램 종료시 데이터 파일에 Machine 객체의 변경사항을 쓰는 함수.
//파일이 정상적으로 열려 쓰는것 까지 마무리됐다면 return true, 그렇지 않다면 return false
bool file_write_function() {
	//쓰기용 파일 변수
	ofstream datafile(filename);

	//파일이 정상적으로 열렸는가
	if (datafile.is_open()) {
		//Machine 인스턴스 가져옴. main함수에서 맨 처음 생성한 그 인스턴스임.
		Machine* M = Machine::getInstance();

		//금고 정보 한줄씩 쓰기
		datafile << M->_mmoney->_100 << endl;
		datafile << M->_mmoney->_500 << endl;
		datafile << M->_mmoney->_1000 << endl;

		//음료 정보 한줄씩 쓰기
		for (int i = 0; i < DRINK_MAX_SLOT; i++) {
			Drink temp = M->_drink[i];

			datafile << (int)(temp._isRegist) << endl;
			datafile << temp._name << endl;
			datafile << temp._price << endl;
			datafile << temp._stock << endl;
		}
		//파일 닫기
		datafile.close();
		cout << "데이터 저장이 완료되었습니다...." << endl << endl;
		return true;
	}
	else {
		//파일 닫기
		datafile.close();
		cout << "파일 열기 실패! 파일 비정상종료 확인바람" << endl << endl;
		return false;
	}
}

int main() {

	//Machine 첫 인스턴스 생성 - 이후 모든 Machine* 생성은 모드 이 인스턴스를 가리킴.
	Machine* M = Machine::getInstance();

	int mode = 0;                     //사용자, 관리자 모드
	bool input_test = false;                        //입력 예외 처리용 flag

	if (!file_read_function())            //데이터 파일을 읽어와서 Machine 인스턴스에 저장하는 함수.
		return 0;                     //파일이 제대로 읽어와지지 않았다면 그대로 프로그램 종료

	  //기능 시작.
	while (mode != 3) {

		//1. 모드 선택
		while (input_test == false) {      //입력예외처리를 위한 loop
			cout << "모드 1. 관리자 2. 사용자 3. 종료\n>";
			cin >> mode;
			//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
			input_test = exception_test(mode, 3, 1);   //mode는 int형, 최대값 3, 최소값 1으로 예외처리
		}
		//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
		input_test = false;

		M->mode = mode;                  //Machine의 mode변수 갱신

		switch (mode) {
			//1-1.관리자 모드. 메뉴선택 - 1.음료 관리, 2.금고 관리, 3.돌아가기   
		case 1:
		{
			int admin_menu = 0;            //Admin 메뉴 변수
			Admin* A = new Admin();         //Admin 인스턴스 생성

			while (admin_menu != 3)
			{
				cout << endl;
				while (input_test == false)
				{
					cout << "메뉴 1. 음료관리 2. 금고관리 3. 돌아가기\n>";
					cin >> admin_menu;
					//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
					input_test = exception_test(admin_menu, 3, 1);
				}
				//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
				input_test = false;

				cout << endl;
				switch (admin_menu)
				{
					//1-1-1.관리자 모드 음료 관리 - 1. 슬롯 등록 2. 슬롯 제거 3. 재고편집 4. 재고확인 5. 돌아가기
				case 1:
				{
					//관리자 음료 관리 메뉴 변수
					int admin_drink_menu = 0;
					//돌아가기를 받기 전까지 while문을 돈다
					while (admin_drink_menu != 5) {
						while (input_test == false) {
							cout << "메뉴 1. 슬롯 등록 2. 슬롯 제거 3. 재고편집 4. 재고확인 5. 돌아가기\n>";
							cin >> admin_drink_menu;
							//exception_test함수는 첫번째 매개변수가 올바른 값이면 true, 아니면 false를 리턴해서 input_test에 넣는다.
							input_test = exception_test(admin_drink_menu, 5, 1);
						}
						//다음 반복문으로 들어가기 위해 다시 false로 바꿔준다.
						input_test = false;
						switch (admin_drink_menu) {
							//1-1-1.관리자 모드 음료 관리 - 1. 슬롯 등록
						case 1:
						{
							//등록할 슬롯 선택을 받는 변수
							int slot_num = -1;

							//첫번째로 빈 슬롯을 찾는 변수 (빈 슬롯을 알아서 찾아서 등록해 주는 기능 구현에 대비)
							int first_empty_slot = M->check_blank_slot();
							//빈 슬롯이 없을 경우
							if (first_empty_slot == -1) {
								cout << "!미등록 슬롯 없음!" << endl << endl;
							}
							//빈 슬롯이 있을 경우
							else {
								//빈 슬롯을 입력받을 때까지 반복
								while (1) {
									cout << "슬롯 번호>";
									cin >> slot_num;
									//슬롯 번호 예외처리 - 한글, 최대최소 거름
									if (exception_test(slot_num, DRINK_MAX_SLOT, 1)) {
										//입력한 슬롯이 등록된 슬롯이면 다시 입력 받음
										if (M->_drink[slot_num - 1]._isRegist)
											cout << "!이미 등록된 슬롯 입니다! 슬롯 번호를 확인하세요!" << endl << endl;
										//입력한 슬롯이 빈 슬롯이면 while문 탈출
										else
											break;
									}
								}
								//선택된 슬롯으로 관리자가 슬롯 정보를 입력해 Machine에 저장함.
								M->_drink[slot_num - 1] = A->regist_slot();
							}
						}
						break;
						//1-1-1.관리자 모드 음료 관리 - 2. 슬롯 제거
						case 2:
						{
							//관리자의 select(0)에서 0의 역할은 슬롯을 받는다는 의미
							int slot_num = A->select(0);
							//슬롯 제거는 음료의 생성자로 초기화함
							M->_drink[slot_num - 1] = Drink();
							cout << endl;
						}
						break;
						//1-1-1.관리자 모드 음료 관리 - 3. 재고편집
						case 3:
						{
							M->show_drink();
							int slot, change;
							//cout << "슬롯 번호>";
							slot = A->select(0);

							if (M->_drink[slot - 1]._isRegist) {
								//cout << "변경할 재고 수>";
								//관리자의 select(1)에서 1의 역할은 재고를 받는다는 의미
								change = A->select(1);
								M->_drink[slot - 1].set_stock(change);
							}
							else
								cout << "미등록 슬롯" << endl;
						}
						break;
						//1-1-1.관리자 모드 음료 관리 - 4. 재고확인
						case 4:
						{
							M->show_drink();
						}
						break;
						//1-1-1.관리자 모드 음료 관리 - 5. 돌아가기
						case 5:
						{
						}
						break;
						default:
						{
							cout << "1,2,3,4,5만 입력 가능" << endl;
							continue;
						}
						break;
						}
					}
				}
				break;
				//1-1-2.관리자 모드 금고 관리
				case 2:
				{
					//현재 금고 상태를 보여줌
					M->show_money();
					//관리자가 추가, 제거하고 싶은 만큼의 정보를 담은 Money인스턴스 생성
					Money* tmp = A->edit_money();
					//tmp만큼 Machine의 금고 정보를 edit함.
					M->_mmoney->edit_money(tmp);
					cout << endl;
					//바뀐 금고 상태를 보여줌
					M->show_money();
				}
				break;
				//1-1-2.관리자 모드 - 돌아가기
				case 3:
				{

				}
				break;
				default:
				{
					cout << "1,2,3만 입력 가능." << endl << endl;
					continue;
				}
				break;
				}
			}
		}
		break;
		//사용자 모드
		case 2:
		{
			User* U = new User();
			//사용자가 투입한 돈(input), 거스름돈(change) 초기화
			M->input = new Money();
			M->change = new Money();

			//while문. -> 사용자가 종료 가능하도록 해야함.
			while (1) {
				//input을 change의 값으로 셋팅 -> 이전 거래에서의 change를 다시 input으로 간주하고 새 거래를 시작
				M->input = M->change;
				//이때 change는 다시 초기화한다.
				M->change = new Money();

				//음료 보여주기
				M->show_drink();

				//잔돈 없음을 알려주는 창의 역할 (이때 잔돈은 100원과 500원)
				cout << "기계 잔돈 상황 : ";
				if (M->_mmoney->_100 == 0 || M->_mmoney->_500 == 0)
					cout << "없음" << endl;                        //금고 내 잔돈이 하나도 없는 상황
				else
					cout << "있음" << endl;                        //금고 내 잔돈이 있는 상황         


				  //현재 투입된 금액을 보여주는 창의 역할
				cout << "투입된 금액 : " << M->input->_total << endl << endl;

				//사용자가 투입한 금액을 저장할 변수
				Money *user_input_money = new Money();
				//사용자가 투입한 금액으로 edit함.
				user_input_money->edit_money(U->input_money());

				//분기. 만약 사용자가 투입한 돈 때문에 기계의 금고가 수용량을 초과할 경우 투입한 돈 그대로 돌려주고 거래 중지.
				//문제!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (user_input_money->_100 < 0 || user_input_money->_500 < 0 || user_input_money->_1000 < 0) {
					cout << endl;
					cout << "초과된 돈" << endl;
					cout << "동전류 잔돈 출구" << endl;
					cout << "100원 : " << abs(user_input_money->_100) << "개 ";
					cout << "500원 : " << abs(user_input_money->_500) << "개 " << endl;

					cout << endl << "지폐 투입구(지류 잔돈 출구)" << endl;
					cout << "1000원 : " << abs(user_input_money->_1000) << "개 " << endl << endl;
					//input의 값을 그대로 change로 옮기고(여러번째 거래일 경우 input에 이전 change가 남아있기때문)
					M->change = M->input;

					//change의 값을 user_input_money의 절대 값으로(이 경우에 Money의 파라미터 중 음수가 반드시 있으므로) 추가한다.
					//M->change->edit_money(new Money(abs(user_input_money->_100), abs(user_input_money->_500), abs(user_input_money->_1000)));
					//돈을 돌려준다. 
					M->return_change();
					break;
				}
				//사용자가 투입한 돈이 금고의 수용량을 초과하지 않은 경우
				else {
					//사용자가 투입한 금액을 Machine의 기계로 추가한다.
					M->get_money(user_input_money);

					cout << "투입된 금액 : " << M->input->_total << endl << endl;
					int slot_num = 0;

					//사용자의 슬롯 선택을 받으며 이때부터 시간을 잰다. 
					//U->select_slot이 false를 반환할 경우 시간초과를 알리고 돈을 최적으로 바꿔 돌려준다.
					if (!U->select_slot(&slot_num)) {
						cout << endl;
						cout << "!" << TIME_LIMIT << "초 초과!" << endl << endl;
						//input을 그대로 change로 바꿔
						M->change = M->input;
						//최적을 계산한 후
						M->optimal_change(M->change->_total);
						//돌려준다.
						M->return_change();
						break;
					}

					//분기 1. 반환 레버가 아닌가?
					if (slot_num != 11 && slot_num != 0) {
						//분기 2. 등록된 슬롯인가?
						if (M->_drink[slot_num - 1]._isRegist) {
							//분기 3. 재고가 있는가?
							if (M->_drink[slot_num - 1]._stock > 0) {
								//분기 4. 투입금액이 충분한가?
								if (M->_drink[slot_num - 1]._price <= M->input->_total) {
									//잔돈계산
									int return_to_user = M->calculate(slot_num);
									//cout << return_to_user << endl;

									//기계내에서 가능한 최적 잔돈 반환 방법 계산
									//분기 5. & 돈을 돌려줄 수 있는가?
									if (M->optimal_change(return_to_user)) {
										//자판기 음료 재고 줄이기
										M->_drink[slot_num - 1].set_stock(M->_drink[slot_num - 1]._stock - 1);
										cout << M->_drink[slot_num - 1]._name << " 이(가) 정상 거래되었습니다." << endl;
										cout << "음료 출구" << endl;
										cout << "음료를 받으세요!*^^*" << endl << endl;
										//다음 거래를 계속 이어간다.
									}
									else {
										cout << "!기계 내 잔돈 부족!" << endl << endl;
										//돈을 그대로 돌려주고 거래 중지
										M->change = M->input;
										M->return_change();
										break;
									}
								}
								else {
									cout << "!금액 부족!" << endl << endl;
									M->change = M->input;
								}
							}
							else {
								cout << "!재고 없음!" << endl << endl;
								M->change = M->input;
							}
						}
						else {
							cout << "!미등록 슬롯!" << endl << endl;
							M->change = M->input;
						}
					}
					//분기 1에서 반환 레버인 경우
					else {
						if (slot_num == 11) {
							cout << "!반환 레버!" << endl << endl;
							//돈을 돌려준 후 거래 중지
							M->change = M->input;
							M->optimal_change(M->change->_total);
							M->return_change();
							break;
						}
					}
				}
			}
		}
		break;
		//종료
		case 3:
		{
			cout << "종료되었습니다." << endl << endl;
		}
		break;
		default:
		{
			cout << "1,2,3만 입력 가능." << endl << endl;
			continue;
		}
		break;
		}
	}

	//변경사항 저장 후 종료
	file_write_function();

	return 0;
}