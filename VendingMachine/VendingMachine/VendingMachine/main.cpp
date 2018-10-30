/*
2018-2 ��������������Ʈ2(2040) �߰���
6�� : �質��, �����, ������
���� : �������Ǳ� ���� �ùķ��̼� ���α׷�
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#define DRINK_NAME_MAX_LENGTH 20            //���� �̸� �ִ� ���� �������� 20��
#define DRINK_MAX_SLOT 10               //���Ǳ� ���� �ִ� 10��
#define DRINK_MAX_STOCK 15                  //���Ǳ� ���� �� ���� �ִ� ��� �� 15��
#define MONEY_100 100                  //100��
#define MONEY_500 500                  //500��
#define MONEY_1000 1000                     //1000��
#define MONEY_100_MAX 100               //���Ǳ� �ݰ� ���� �� �ִ� 100�� �ִ� ����
#define MONEY_500_MAX 50               //���Ǳ� �ݰ� ���� �� �ִ� 500�� �ִ� ����
#define MONEY_1000_MAX 50               //���Ǳ� �ݰ� ���� �� �ִ� 1000�� �ִ� ����
#define TIME_LIMIT 30                  //����� ���� ��ȣ �Է� ���� �ð�
const char* filename = "machine_data.txt";  //���Ǳ� �����͸� ������ ���� �̸�
using namespace std;

bool exception_test(int num, int max, int min);

//���� Ŭ����
class Drink {
public:
	//��� ����
	bool _isRegist;                     //��ϵ� ���� �����ΰ��� �˻�
	char _name[DRINK_NAME_MAX_LENGTH + 1];  //������ �̸�
	int _price;                        //������ ����
	int _stock;                        //������ ���

					//��� �Լ�
					//������ - �ƹ��͵� ���� ���� ���� ���·� �ʱ�ȭ�ϴ� ����
	Drink() {
		_isRegist = false;                  //�ƹ��� ���� ���� ���� ��, ���Կ� ��ϵ��� ����
						//�̸��� �����ϴ� char�迭�� 21��¥�� null�� �ʱ�ȭ ��.
		memset(_name, 0, sizeof(char) * (DRINK_NAME_MAX_LENGTH + 1));
		_price = 0;
		_stock = 0;
	}

	//������ �����ε� - ���ῡ ���� ����� �� �Ѱ��� �Ķ���ͷ� �� �������ִ� ����
	Drink(char* name, int price, int stock) {
		_isRegist = true;               //���� �־����Ƿ� true
					  /*for (int i = 0; i < DRINK_NAME_MAX_LENGTH; i++)
					  _name[i] = name[i];*/
		strcpy_s(_name, name);
		_price = price;
		_stock = stock;
	}

	//��� �����ϴ� �Լ�
	void set_stock(int stock) {
		_stock = stock;
	}
};

//�� Ŭ����
class Money {
public:
	//��� ����
	int _100 = 0;                     //100���� ����
	int _500 = 0;                     //500���� ����
	int _1000 = 0;                     //1000���� ����
	int _total = 0;                     //��Ż ����

					//��� �Լ�
					//������ - �ʱ�ȭ
	Money() {
		_100 = 0;
		_500 = 0;
		_1000 = 0;
		_total = 0;
	}

	//������ �����ε�
	Money(int input_100, int input_500, int input_1000) {
		_100 = input_100;
		_500 = input_500;
		_1000 = input_1000;
		_total = MONEY_100 * _100 + MONEY_500 * _500 + MONEY_1000 * _1000;
	}

	//���� ������ �����ϴ� �Լ�. ��� ������ ���� ������.
	//���� �ִ� ������ ������ �Ķ���� ��ŭ �߰�/�����ϴ� ����.
	void edit_money(Money* income) {
		_100 += income->_100;
		_500 += income->_500;
		_1000 += income->_1000;
		_total += income->_total;
	}
};

//��� Ŭ���� - �̱��� ����
class Machine {
private:
	//��� �Լ�
	//������ - �ʱ�ȭ
	Machine() {
		_drink = new Drink[DRINK_MAX_SLOT]();
		_mmoney = new Money();
		input = new Money();
		change = new Money();
		mode = 0;
	};
	Machine(const Machine& m);
	~Machine() {};

	//��� ����

	static Machine* instance;
	//���ʷ� ȣ���� �ν��Ͻ��� ����Ű�� ������
	//���� Machine �ν��Ͻ��� �ٽ� ������ �׻� �� �����ͷ� ������ ��.
	//MachineŬ���� ���� �Ʒ����� �ʱ�ȭ��.

public:
	//��� ����
	Drink * _drink;                     //Machine�� �Ǹ��ϴ� ����
	Money* _mmoney;                     //Machine�� �ݰ�
	bool available;                     //Machine�� �ܵ� ����
	Money *input;                     //����ڰ� �Է��� �ݾ� ����
	Money *change;                     //����ڿ��� �Ž����� �ݾ�
	int mode;                        //����� ���, ������ ���

				   //��� �Լ�
				   //�̱��� ���� - ���� �����ÿ��� ��� ó���� instance�� ����Ű���� ���ο� �ν��Ͻ� ������ ����.
	static Machine* getInstance() {
		if (instance == NULL)               //instance�� NULL�̸� �� �ν��Ͻ��� ����.
			instance = new Machine();       //instance�� �� �ν��Ͻ��� ����Ű�� �� ��
		return instance;               //instance ����. NULL�� �ƴϸ� �� instance ����.
	}

	//ù ��°�� ������ �� ���� ���� - �� ���α׷������� ��ü�� �� ������ �ִ����� üũ�ϴ� �뵵�θ� ���.
	//��, ���� ���� -1�� �ƴϸ� �ǹ̰� ����.
	int check_blank_slot() {
		for (int i = 0; i < DRINK_MAX_SLOT; i++) {
			if (!_drink[i]._isRegist) {
				return i;
			}
		}
		return -1;
	}

	//�Է¹��� ���� ��ȣ�κ��� ���Աݾװ� ������ ���ᰡ�� ���� ���
	int calculate(int slot_num) {
		int change_total = input->_total - _drink[slot_num - 1]._price;
		return change_total;
	}

	//�ܵ��� ��Ż �ݾ��� �Ķ���ͷ� �ް�, �ݰ� �ܵ� ��Ȳ�� ����� ���� ��ȯ ��� ��� �� ������ �� �ִ����� �Ǵ�
	//������ �� �ִٸ� return true, ������ �� ���ٸ� return false
	bool optimal_change(int tmp) {
		change = new Money();                  //������ �� �ν��Ͻ� �ʱ�ȭ. (Machine �����ÿ���, main�� ����ڿ��� ���Թޱ� ���Ŀ� �ʱ�ȭ�� ������, ���� �� ���⼭�� �ѹ� �� �ʱ�ȭ ����.)
		int change_1000, change_500, change_100;   //������ �� ���� ������ ����


		change_1000 = tmp / MONEY_1000;            //1000������ ������ �� �ִ� �ִ� ���� ���
		if (_mmoney->_1000 < change_1000) {         //�ݰ� ���� 1000�� ������ ���� �ݰ� 1000���� ������
			change_1000 = _mmoney->_1000;         //�ݰ��� 1000�� ���� ��ŭ�� �����ش� (�ɷ¸�ŭ)
		}
		tmp -= change_1000 * MONEY_1000;         //������ �� �ִ� �ݾ��� tmp���� ����


						  //1000���� ���������� �������� 500�� ���
		change_500 = tmp / MONEY_500;
		if (_mmoney->_500 < change_500) {
			change_500 = _mmoney->_500;
		}
		tmp -= change_500 * MONEY_500;

		//1000���� ���������� �������� 100�� ���
		change_100 = tmp / MONEY_100;
		if (_mmoney->_100 < change_100) {
			//�̶��� ������ 100�� ������ ������ ������ ���ڶ� ���̹Ƿ� �ܵ��� ���°� ��
			//���Ե� �� �����ֱ� - ������ �Ŀ� �ۿ���
			//�ܵ� ����
			//cout << "optimal function : �ܵ� ����!!" << endl;
			return false;
		}
		else {
			tmp -= change_100 * MONEY_100;
		}

		if (tmp == 0) {
			//tmp==0�� ���� ���� ������ �� �ִٴ� ���� ��Ÿ��.
			//change�� ������ ������ set
			change->edit_money(new Money(change_100, change_500, change_1000));
			return true;
		}
		else {
			//��� �� ���� �� ���� �б�. �ܵ��� �� �� �����鼭 tmp==0 �� �� ����.
			//���Ե� �� �����ֱ� - ������ �Ŀ� �ۿ���
			//Something Wrong
			cout << "optimal function : Something Wrong!!" << endl;
			return false;
		}

	}

	//�ݰ� ���¸� ������.
	void show_money() {
		cout << "�ݰ�" << endl;
		cout << "100�� " << _mmoney->_100 << "�� ";
		cout << "500�� " << _mmoney->_500 << "�� ";
		cout << "1000�� " << _mmoney->_1000 << "�� " << endl;
		cout << "Total : " << _mmoney->_total << "��" << endl << endl;
	}

	//���Ǳ� ���� ����Ʈ�� ������.
	void show_drink() {
		cout << endl;
		for (int i = 0; i < DRINK_MAX_SLOT; i++) {
			//���� ��ȣ ��º�
			//width�� ����ؼ� ǥ�� �������.
			cout.width(3);
			cout.fill(' ');
			cout << i + 1 << ". ";

			//��ϵ� ������ ���
			if (_drink[i]._isRegist) {
				//���� �̸� ��º�
				cout.width(DRINK_NAME_MAX_LENGTH + 1);
				cout.fill(' ');
				cout << _drink[i]._name;
				cout << " | ";
				//���� ���� ��º�
				cout.width(5);
				cout.fill(' ');
				cout << _drink[i]._price << "�� ";
				cout << " | ";
				//�����ڶ��, ��� ��Ȳ���� ���
				if (mode == 1) {
					cout.width(5);
					cout.fill(' ');
					cout << _drink[i]._stock << "/" << DRINK_MAX_STOCK;
				}
				//����ڶ��, ��� ���� ���� �˷���
				else if (mode == 2 && _drink[i]._stock == 0) {
					cout.width(5);
					cout.fill(' ');
					cout << "��� ����";
				}
			}
			//��ϵ��� ���� ������ ���
			else {
				cout.width(DRINK_NAME_MAX_LENGTH + 1);
				cout.fill(' ');
				cout << "�̵�� ����";
				cout << " | ";
			}
			cout << endl;
		}
		//����� �����, ��ȯ �������� �����.
		if (mode == 2) {
			cout.width(3);
			cout.fill(' ');
			cout << DRINK_MAX_SLOT + 1 << ". ";
			cout.width(DRINK_NAME_MAX_LENGTH + 1);
			cout.fill(' ');
			cout << "�ܵ� ��ȯ ����" << endl;
		}
		cout << endl;
	}

	//����ڷκ��� ���� �ݾ� ����
	void get_money(Money* m) {
		//����ڷκ��� ����
		input->edit_money(m);
		//��� �ݰ� �߰�
		_mmoney->edit_money(input);
	}

	//��� ���� change�� ���� ��ȯ�� ���ִ� �Լ�
	//���� �ݰ��� �׸�ŭ�� ���� ��
	//�� �� ��� ���� input�� change�� �ʱ�ȭ
	void return_change() {
		//�ӽ÷� �� ���� ������ (-)�� ������ Money ��ü.
		Money* tmp = new Money(-change->_100, -change->_500, -change->_1000);

		//�ݰ��� ������ ������. tmp��ŭ ����.
		_mmoney->edit_money(tmp);

		//������ �ܾ� ��Ȳ�� ������.
		cout << endl;
		cout << "�ܵ� : " << change->_total << "��" << endl << endl;

		cout << "������ �ܵ� �ⱸ" << endl;
		cout << "100�� : " << change->_100 << "�� ";
		cout << "500�� : " << change->_500 << "�� " << endl << endl;
		cout << "���� ���Ա�(���� �ܵ� �ⱸ)" << endl;
		cout << "1000�� : " << change->_1000 << "�� " << endl << endl;
		//�ʱ�ȭ
		input = new Money();
		change = new Money();
	}
};
Machine* Machine::instance = nullptr;

//����� Ŭ����
class User {
public:
	//��� �Լ�
	//����ڰ� ���� �Է��ϴ� �Լ�
	Money * input_money() {
		Money* tmp;
		Machine* M = Machine::getInstance();

		bool input_test = false;            //�Է� ���� ó���� flag
		int tmp100 = 0, tmp500 = 0, tmp1000 = 0;

		cout << "���� ���Ա� (100��/500��)" << endl;
		//����ó���� �ݺ���
		while (input_test == false) {
			cout << "100�� ����>";
			cin >> tmp100;
			if ((tmp100 + M->_mmoney->_100) > MONEY_100_MAX) {
				cout << "��迡 �� �� �ִ� 100�� ���� ���� �ʰ��Ͽ����ϴ�." << endl << endl;
				//tmp100�� ������ ������ : ������� ������ ������ ���� �ݰ� ���ļ� ����������� ���Ѵ�.
				tmp100 = -tmp100;
				//���̻� 500���̳� 1000�� �Է��� ���� �ʰ� �ٷ� return
				return tmp = new Money(tmp100, 0, 0);
			}
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(tmp100, MONEY_100_MAX - (M->_mmoney->_100), 0);
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;

		//����ó���� �ݺ���
		while (input_test == false) {
			cout << "500�� ����>";
			cin >> tmp500;
			if ((tmp500 + M->_mmoney->_500) > MONEY_500_MAX) {
				cout << "��迡 �� �� �ִ� 500�� ���� ���� �ʰ��Ͽ����ϴ�." << endl;
				//tmp500�� ������ ������ : ������� ������ ������ ���� �ݰ� ���ļ� ����������� ���Ѵ�.
				tmp500 = -tmp500;
				//���̻� 1000���� �Է��� ���� �ʰ� �ٷ� return
				return tmp = new Money(tmp100, tmp500, 0);
			}
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(tmp500, MONEY_500_MAX - (M->_mmoney->_500), 0);
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;
		cout << endl << "���� ���Ա� (1000��)" << endl;

		//����ó���� �ݺ���
		while (input_test == false) {
			cout << "1000�� ����>";
			cin >> tmp1000;
			if ((tmp1000 + M->_mmoney->_1000) > MONEY_1000_MAX) {
				cout << "��迡 �� �� �ִ� ���� ���� �ʰ��Ͽ����ϴ�." << endl;
				//tmp1000�� ������ ������ : ������� ������ ������ ���� �ݰ� ���ļ� ����������� ���Ѵ�.
				tmp1000 = -tmp1000;
				//�ٷ� return
				return tmp = new Money(tmp100, tmp500, tmp1000);
			}
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(tmp1000, MONEY_1000_MAX - (M->_mmoney->_1000), 0);
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;

		tmp = new Money(tmp100, tmp500, tmp1000);
		return tmp;
	}

	//����ڰ� �ֹ��� ������ ��ȣ�� �Է��ϴ� �Լ�
	bool select_slot(int *slot) {
		bool input_test = false;            //�Է� ���� ó���� flag	

		//�ð��� ��� ���ؼ� ���� time����, �̶� ��ŸƮ Ÿ�� ����!(���� Ÿ�Ӱ� ��ŸƮ Ÿ���� ���̸� ���)
		int start_time = (unsigned)time(0);
		int end_time;

		char tmp_slot[3];
		//string temp_s_slot;

		cout << "���� ��ȣ>";
		while (1) {
			end_time = (unsigned)time(0);            //�� �� ���� Ÿ�� ����!
			//Ű���� �Է��� ������ �Ʒ��� if�� ������ ���´�.
			if (_kbhit()) {
				//cin >> *slot;
				//getline�Լ��� �ι�° �Ű������� �޴� �����ŭ�� �Է¸� ù��° �Ű������� �ּҷ� �ް�, �������� ������. 
				cin.getline(tmp_slot, 3);

				if (cin.fail()) {
					cin.clear();
					//����ڰ� INT_MAX(2147483647)�� �̻��� ���ĺ��� ġ�� �ʴ� �̻� ���� ������ �ذ����ش�.
					//cin.ignore�Լ��� ù��° �Ű������� ���� ������ ��, �ι�° �Ű������� 
					cin.ignore(INT_MAX, '\n');
					//cout << "her ost" << endl;
					//��ŸƮ Ÿ���� �ٽ� �������ش�.
					start_time = (unsigned)time(0);
					cout << "�ٽ� �Է����ּ���" << endl;
					cout << "���� ��ȣ>";
					//�ݺ����� ���� �ڵ带 �����ϰ� �ݺ��� ���� ���� �����Ѵ�. 
					continue;
				}
				//temp_s_slot = tmp_slot;

				//�Է¹��� ���� ù��° ���ڰ� 0�϶�
				if (tmp_slot[0] == '0') {
					//cout << "�Ǥ��Ǥ��Ǥ�" << endl;
					//cout << "test = " << temp_s_slot.at(1) << endl;
					//cout << "test = " << (int)(temp_s_slot.at(1))-'0' << endl;
					//01~09�Է� ���
					if (tmp_slot[1] > '0' && tmp_slot[1] <= '9') {
						*slot = (int)tmp_slot[1] - '0';
						//cout << *slot << endl;
						return true;
					}
				}
				//�Է¹��� ���� ù��° ���ڰ� 1~9�϶�
				else if (tmp_slot[0] > '0' && tmp_slot[0] <= '9' && tmp_slot[1] == NULL) {
					*slot = (int)tmp_slot[0] - '0';
					//cout << *slot << endl;
					return true;
				}
				//10�Է� �� ���
				if (tmp_slot[0] == '1'&&tmp_slot[1] == '0') {
					*slot = 10;
					return true;
				}
				//10�Է� �� ���
				if (tmp_slot[0] == '1'&&tmp_slot[1] == '1') {
					*slot = 11;
					return true;
				}
				//��ŸƮ Ÿ���� �ٽ� �������ش�.
				start_time = (unsigned)time(0);
				cout << "�ٽ� �Է����ּ���" << endl;
				cout << "���� ��ȣ>";
			}
			//�����س��� �ð��� ������ false�� �����Ѵ�.
			if (end_time - start_time > TIME_LIMIT) {
				*slot = 0;
				//false�� �����ϸ� ����ڰ� ���� ���� �״�� ��ȯ�ǰ� �Ѵ�.
				return false;
			}

		}
	}
};

//������ Ŭ����
class Admin {
public:
	//����Լ�
	//�����ڰ� ���ݰ��� �� ������ �ٲ� �� ȣ��Ǵ� �Լ�
	Money * edit_money() {
		//�Է��� ���� ������ ������ ����
		Money* tmp = new Money();
		//���ο��� �̸� �����ص� Machine �ν��Ͻ�
		Machine* M = Machine::getInstance();

		bool input_test = false;            //�Է� ���� ó���� flag
		int tmp100, tmp500, tmp1000;

		cout << "(100 500 1000) ������, �߰��� ���� ����, ���Ŵ� ���� ���� �Է�\n";
		while (input_test == false) {
			cout << "100�� ����>";
			cin >> tmp100;
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(tmp100, MONEY_100_MAX - (M->_mmoney->_100), -(M->_mmoney->_100));
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;

		while (input_test == false) {
			cout << "500�� ����>";
			cin >> tmp500;
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(tmp500, MONEY_500_MAX - (M->_mmoney->_500), -(M->_mmoney->_500));
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;

		while (input_test == false) {
			cout << "1000�� ����>";
			cin >> tmp1000;
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(tmp1000, MONEY_1000_MAX - (M->_mmoney->_1000), -(M->_mmoney->_1000));
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;

		tmp->edit_money(new Money(tmp100, tmp500, tmp1000));
		return tmp;
	}

	//�����ڰ� ��� ���Կ� ���Ḧ ����� �� ȣ��Ǵ� �Լ� - �ּ�ó�� �� �����ϰ� ���ּ���. �� ������ �� ���ϴ���
	Drink regist_slot() {
		char name[DRINK_NAME_MAX_LENGTH + 1];
		int price;
		int stock;

		bool input_test = false;            //�Է� ���� ó���� flag

		while (1) {
			memset(name, 0, sizeof(char)*(DRINK_NAME_MAX_LENGTH + 1));
			cout << "name> ";
			//getline�Լ��� �ι�° �Ű������� �޴� �����ŭ�� �Է¸� ù��° �Ű������� �ּҷ� �ް�, �������� ������. 
			cin.getline(name, DRINK_NAME_MAX_LENGTH + 1);

			/*
			if (1)
			{
			cout << "hihi" << endl;
			continue;
			}*/

			//�Էµ� ���� NULL�� �� strcmp�Լ����� ù��° �Ű�������
			//�ι�° �Ű������� ������ 0�� �����ϹǷ� if������ ó��
			if (strcmp(name, "") == 0) {
				cout << "�̸��� �Է����� �����̽��ϴ�." << endl;
				//�ٽ� �ݺ��� �� ���� �ö��
				continue;
			}

			if (cin.fail()) {
				cin.clear();
				//����ڰ� INT_MAX(2147483647)�� �̻��� ���ĺ��� ġ�� �ʴ� �̻� ���� ������ �ذ����ش�.
				//cin.ignore�Լ��� ù��° �Ű������� ���� ������ ��, �ι�° �Ű������� 
				cin.ignore(INT_MAX, '\n');
			}

			//cout << "====" << name << endl;

			while (input_test == false) {
				cout << "price> ";
				cin >> price;
				//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
				input_test = exception_test(price, 2000, 100);

				//�Է��� ������ 100�������� �ƴϸ�
				if (price % 100 != 0) {
					cout << "������ 100�� ������ �Է����ּ���." << endl;
					input_test = false;
				}
			}
			//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
			input_test = false;

			while (input_test == false) {
				cout << "stock> ";
				cin >> stock;
				//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
				input_test = exception_test(stock, INT_MAX, 0);
			}
			//���� �����ڰ� ��� 15���� ���� �Է����� ���
			if (stock > DRINK_MAX_STOCK) {
				cout << "����� �ִ밪(15��)���� ���� �Է��Ͽ� ���� 15�� �����˴ϴ�." << endl;
				stock = 15;
			}
			//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
			input_test = false;
			break;
		}
		cout << endl;

		cout << "���" << endl;
		cout << "�̸� : " << name << endl;
		cout << "���� : " << price << endl;
		cout << "��� : " << stock << endl << endl;

		return Drink(name, price, stock);

	}

	//�����ڰ� �����ǹ�ȣ�� �Է��ؾ� �� ��, ����� ���� �Է��ؾ��� �� ȣ��Ǵ� �Լ�
	int select(int choice) {
		int select_num;
		bool input_test = false;            //�Է� ���� ó���� flag

		//����ó���� ���� ����
		while (input_test == false) {
			if (choice == 0) {
				cout << "���� ��ȣ>";
				cin >> select_num;
				//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
				input_test = exception_test(select_num, DRINK_MAX_SLOT, 1);
			}
			else if (choice == 1) {
				cout << "������ ��� ��>";
				cin >> select_num;
				//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
				input_test = exception_test(select_num, INT_MAX, 0);
			}

		}

		//���� ��� �Է��̸鼭, �����ڰ� ��� 15���� ���� �Է����� ���
		if (choice == 1 && select_num > DRINK_MAX_STOCK) {
			cout << "����� �ִ밪(15��)���� ���� �Է��Ͽ� ���� 15�� �����˴ϴ�." << endl;
			select_num = 15;
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;
		/*
		while (1){
		cin.get(ch);
		if (cin.fail() || isalpha(ch) || ispunct(ch) || isblank(ch)){
		cin.clear();
		while ((ch = cin.get() != '\n') && ch != EOF);
		cout << "�߸��Է��ϼ̽��ϴ�." << endl << endl;
		}
		break;
		}*/

		return select_num;
	}
};

//��� 1 ����ó�� �Լ�(num�� min~max������(�̻�, ����) int�� ���̸� true
bool exception_test(int num, int max, int min) {
	//test1, test2, test3, test4��� 0�� �ƴ� int�� ������ �����Ѵ�.
	//���� ���� ��Ȳ�� ���ǹ��� ���� 4������ �ϳ��� ������ ���� 0���� �ٲ۴�.
	//��� ���� ��Ȳ���� ���� ���� �Է°����� test1~4�� ���� ��� 0�� �ƴ� ���̴�.
	//������ ��*0�� ������ 0�� ������ ������ ������ �̿��Ͽ� test1~4�� ��� ���� final_test��� ������ ����
	//final_test�� ���� 0�� �ƴϸ� �´� ���� �Է¹��������� �Ǵ��Ͽ� return true�� �Ѵ�!
	int test1 = 1, test2 = 1, test3 = 1, test4 = 1;
	int final_test;

	if (cin.fail()) {
		//cout << "��" << endl;
		test1 = 0;
		while (cin.get() != '\n') {
			//cout << "��������" << endl;
			cin.clear();   //���� ����ְ�
			test2 = 0;
		}
	}
	else {
		while (cin.get() != '\n') {
			//cout << "������" << endl;
			test3 = 0;
			cin.clear();   //���� ����ְ�
		}
		//cout << "�׶׶�" << endl;
		test4 = 1;
	}

	final_test = test1 * test2 * test3 * test4;
	if (final_test != 0) {
		if (num >= min && num <= max) {
			//cout << "�´°�!!!!!!!!!" << endl;
			return true;
		}
		else {
			//���� ������ �� �ùٸ� ������ �������� �˷��ش�. 
			if (max == INT_MAX)
				max = 15;
			cout << min << "~" << max << "������ ���� �Է����ּ���" << endl << endl;
			return false;
		}
	}
	else {
		//�ƿ� ������ �Է����� �ʾ��� ��
		cout << "���� �Է����ּ���" << endl << endl;
		return false;
	}
}

//��� 2-(1) ó�� ���α׷� ����� ������ ������ �о�ͼ� Machine ��ü�� �����ϴ� �Լ�.
//������ ���������� �о�����ٸ� return true, �׷��� �ʴٸ� return false
bool file_read_function() {

	//�б�� ���� ����
	ifstream datafile(filename);

	//���Ϸκ��� ���پ� �о�� �����͸� �ӽ������� char�迭
	char buffer[DRINK_NAME_MAX_LENGTH + 1];      //���Ͽ��� ���� �� �׸��� �ִ� 20���� �����̸��̹Ƿ� �׸� �������� ���� ũ�� ���.
						//+1�� ����

						//ó������ ������Ʈ�� ��������, ������ �����Ƿ� �ٷ� return true
	if (!datafile) {
		cout << "�����͸� ���������� �ҷ��Խ��ϴ�." << endl << endl;
		return true;
	}

	//������ ���������� ������ �ʾҴٸ� return false
	if (!datafile.is_open()) {
		cout << "������ �ҷ����⿡ �����߽��ϴ�.. �������� �۵��� ���� �ٽ� �������ּ���." << endl;
		return false;
	}

	//Machine �ν��Ͻ� ������. main�Լ����� �� ó�� ������ �� �ν��Ͻ���.
	Machine* M = Machine::getInstance();

	//�Ʒ��� �޿� �̸� ������ �� ���� ���Ŀ� �°� �о���� �ڵ�
	//���پ� �о���� �ڵ��� �ݺ�

	//�ݰ� ���� ����
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	int _100 = atoi(buffer);               //�ݰ� 100�� ����
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	int _500 = atoi(buffer);               //�ݰ� 500�� ����
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	int _1000 = atoi(buffer);               //�ݰ� 1000�� ����
	M->_mmoney = new Money(_100, _500, _1000);


	//���� ���� ����
	for (int i = 0; i < DRINK_MAX_SLOT; i++) {
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		M->_drink[i]._isRegist = atoi(buffer);   //��ϵ� �����ΰ� �ƴѰ��� ����. bool������ �����̹Ƿ� atoi�� int�� ��ȯ
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		if (M->_drink[i]._isRegist)            //��ϵ� ������ �̸� �о�ͼ� ����. �ƴ϶�� ��¥�� null�̹Ƿ� ������ �ʿ䰡 ����
			strcpy_s(M->_drink[i]._name, buffer);
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		if (M->_drink[i]._isRegist)            //��ϵ� ������ ���� �о�ͼ� ����.
			M->_drink[i]._price = atoi(buffer);
		datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
		if (M->_drink[i]._isRegist)            //��ϵ� ������ ��� �о�ͼ� ����.
			M->_drink[i]._stock = atoi(buffer);
	}
	datafile.getline(buffer, DRINK_NAME_MAX_LENGTH + 1);
	//������ ���� �ǹ̾��� ���� -> ���������� �о����� Ȯ���ϴ� �б⸦ ����

	//������ ���������� �о����� �˻��ϴ� �б�.
	//���Ⱑ �������� �ƴ϶�� ������ ������������ ����� ��.
	if (datafile.eof()) {
		//���� �ݱ�
		datafile.close();
		cout << "�����͸� ���������� �ҷ��Խ��ϴ�." << endl << endl;
		return true;
	}
	else {
		//���� �ݱ�
		datafile.close();
		cout << "������ ������������ ����ƽ��ϴ�. Ȯ�ιٶ��ϴ�." << endl << endl;
		return false;
	}
}

//��� 2-(2) ������ ���α׷� ����� ������ ���Ͽ� Machine ��ü�� ��������� ���� �Լ�.
//������ ���������� ���� ���°� ���� �������ƴٸ� return true, �׷��� �ʴٸ� return false
bool file_write_function() {
	//����� ���� ����
	ofstream datafile(filename);

	//������ ���������� ���ȴ°�
	if (datafile.is_open()) {
		//Machine �ν��Ͻ� ������. main�Լ����� �� ó�� ������ �� �ν��Ͻ���.
		Machine* M = Machine::getInstance();

		//�ݰ� ���� ���پ� ����
		datafile << M->_mmoney->_100 << endl;
		datafile << M->_mmoney->_500 << endl;
		datafile << M->_mmoney->_1000 << endl;

		//���� ���� ���پ� ����
		for (int i = 0; i < DRINK_MAX_SLOT; i++) {
			Drink temp = M->_drink[i];

			datafile << (int)(temp._isRegist) << endl;
			datafile << temp._name << endl;
			datafile << temp._price << endl;
			datafile << temp._stock << endl;
		}
		//���� �ݱ�
		datafile.close();
		cout << "������ ������ �Ϸ�Ǿ����ϴ�...." << endl << endl;
		return true;
	}
	else {
		//���� �ݱ�
		datafile.close();
		cout << "���� ���� ����! ���� ���������� Ȯ�ιٶ�" << endl << endl;
		return false;
	}
}

int main() {

	//Machine ù �ν��Ͻ� ���� - ���� ��� Machine* ������ ��� �� �ν��Ͻ��� ����Ŵ.
	Machine* M = Machine::getInstance();

	int mode = 0;                     //�����, ������ ���
	bool input_test = false;                        //�Է� ���� ó���� flag

	if (!file_read_function())            //������ ������ �о�ͼ� Machine �ν��Ͻ��� �����ϴ� �Լ�.
		return 0;                     //������ ����� �о������ �ʾҴٸ� �״�� ���α׷� ����

	  //��� ����.
	while (mode != 3) {

		//1. ��� ����
		while (input_test == false) {      //�Է¿���ó���� ���� loop
			cout << "��� 1. ������ 2. ����� 3. ����\n>";
			cin >> mode;
			//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
			input_test = exception_test(mode, 3, 1);   //mode�� int��, �ִ밪 3, �ּҰ� 1���� ����ó��
		}
		//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
		input_test = false;

		M->mode = mode;                  //Machine�� mode���� ����

		switch (mode) {
			//1-1.������ ���. �޴����� - 1.���� ����, 2.�ݰ� ����, 3.���ư���   
		case 1:
		{
			int admin_menu = 0;            //Admin �޴� ����
			Admin* A = new Admin();         //Admin �ν��Ͻ� ����

			while (admin_menu != 3)
			{
				cout << endl;
				while (input_test == false)
				{
					cout << "�޴� 1. ������� 2. �ݰ���� 3. ���ư���\n>";
					cin >> admin_menu;
					//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
					input_test = exception_test(admin_menu, 3, 1);
				}
				//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
				input_test = false;

				cout << endl;
				switch (admin_menu)
				{
					//1-1-1.������ ��� ���� ���� - 1. ���� ��� 2. ���� ���� 3. ������� 4. ���Ȯ�� 5. ���ư���
				case 1:
				{
					//������ ���� ���� �޴� ����
					int admin_drink_menu = 0;
					//���ư��⸦ �ޱ� ������ while���� ����
					while (admin_drink_menu != 5) {
						while (input_test == false) {
							cout << "�޴� 1. ���� ��� 2. ���� ���� 3. ������� 4. ���Ȯ�� 5. ���ư���\n>";
							cin >> admin_drink_menu;
							//exception_test�Լ��� ù��° �Ű������� �ùٸ� ���̸� true, �ƴϸ� false�� �����ؼ� input_test�� �ִ´�.
							input_test = exception_test(admin_drink_menu, 5, 1);
						}
						//���� �ݺ������� ���� ���� �ٽ� false�� �ٲ��ش�.
						input_test = false;
						switch (admin_drink_menu) {
							//1-1-1.������ ��� ���� ���� - 1. ���� ���
						case 1:
						{
							//����� ���� ������ �޴� ����
							int slot_num = -1;

							//ù��°�� �� ������ ã�� ���� (�� ������ �˾Ƽ� ã�Ƽ� ����� �ִ� ��� ������ ���)
							int first_empty_slot = M->check_blank_slot();
							//�� ������ ���� ���
							if (first_empty_slot == -1) {
								cout << "!�̵�� ���� ����!" << endl << endl;
							}
							//�� ������ ���� ���
							else {
								//�� ������ �Է¹��� ������ �ݺ�
								while (1) {
									cout << "���� ��ȣ>";
									cin >> slot_num;
									//���� ��ȣ ����ó�� - �ѱ�, �ִ��ּ� �Ÿ�
									if (exception_test(slot_num, DRINK_MAX_SLOT, 1)) {
										//�Է��� ������ ��ϵ� �����̸� �ٽ� �Է� ����
										if (M->_drink[slot_num - 1]._isRegist)
											cout << "!�̹� ��ϵ� ���� �Դϴ�! ���� ��ȣ�� Ȯ���ϼ���!" << endl << endl;
										//�Է��� ������ �� �����̸� while�� Ż��
										else
											break;
									}
								}
								//���õ� �������� �����ڰ� ���� ������ �Է��� Machine�� ������.
								M->_drink[slot_num - 1] = A->regist_slot();
							}
						}
						break;
						//1-1-1.������ ��� ���� ���� - 2. ���� ����
						case 2:
						{
							//�������� select(0)���� 0�� ������ ������ �޴´ٴ� �ǹ�
							int slot_num = A->select(0);
							//���� ���Ŵ� ������ �����ڷ� �ʱ�ȭ��
							M->_drink[slot_num - 1] = Drink();
							cout << endl;
						}
						break;
						//1-1-1.������ ��� ���� ���� - 3. �������
						case 3:
						{
							M->show_drink();
							int slot, change;
							//cout << "���� ��ȣ>";
							slot = A->select(0);

							if (M->_drink[slot - 1]._isRegist) {
								//cout << "������ ��� ��>";
								//�������� select(1)���� 1�� ������ ��� �޴´ٴ� �ǹ�
								change = A->select(1);
								M->_drink[slot - 1].set_stock(change);
							}
							else
								cout << "�̵�� ����" << endl;
						}
						break;
						//1-1-1.������ ��� ���� ���� - 4. ���Ȯ��
						case 4:
						{
							M->show_drink();
						}
						break;
						//1-1-1.������ ��� ���� ���� - 5. ���ư���
						case 5:
						{
						}
						break;
						default:
						{
							cout << "1,2,3,4,5�� �Է� ����" << endl;
							continue;
						}
						break;
						}
					}
				}
				break;
				//1-1-2.������ ��� �ݰ� ����
				case 2:
				{
					//���� �ݰ� ���¸� ������
					M->show_money();
					//�����ڰ� �߰�, �����ϰ� ���� ��ŭ�� ������ ���� Money�ν��Ͻ� ����
					Money* tmp = A->edit_money();
					//tmp��ŭ Machine�� �ݰ� ������ edit��.
					M->_mmoney->edit_money(tmp);
					cout << endl;
					//�ٲ� �ݰ� ���¸� ������
					M->show_money();
				}
				break;
				//1-1-2.������ ��� - ���ư���
				case 3:
				{

				}
				break;
				default:
				{
					cout << "1,2,3�� �Է� ����." << endl << endl;
					continue;
				}
				break;
				}
			}
		}
		break;
		//����� ���
		case 2:
		{
			User* U = new User();
			//����ڰ� ������ ��(input), �Ž�����(change) �ʱ�ȭ
			M->input = new Money();
			M->change = new Money();

			//while��. -> ����ڰ� ���� �����ϵ��� �ؾ���.
			while (1) {
				//input�� change�� ������ ���� -> ���� �ŷ������� change�� �ٽ� input���� �����ϰ� �� �ŷ��� ����
				M->input = M->change;
				//�̶� change�� �ٽ� �ʱ�ȭ�Ѵ�.
				M->change = new Money();

				//���� �����ֱ�
				M->show_drink();

				//�ܵ� ������ �˷��ִ� â�� ���� (�̶� �ܵ��� 100���� 500��)
				cout << "��� �ܵ� ��Ȳ : ";
				if (M->_mmoney->_100 == 0 || M->_mmoney->_500 == 0)
					cout << "����" << endl;                        //�ݰ� �� �ܵ��� �ϳ��� ���� ��Ȳ
				else
					cout << "����" << endl;                        //�ݰ� �� �ܵ��� �ִ� ��Ȳ         


				  //���� ���Ե� �ݾ��� �����ִ� â�� ����
				cout << "���Ե� �ݾ� : " << M->input->_total << endl << endl;

				//����ڰ� ������ �ݾ��� ������ ����
				Money *user_input_money = new Money();
				//����ڰ� ������ �ݾ����� edit��.
				user_input_money->edit_money(U->input_money());

				//�б�. ���� ����ڰ� ������ �� ������ ����� �ݰ� ���뷮�� �ʰ��� ��� ������ �� �״�� �����ְ� �ŷ� ����.
				//����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (user_input_money->_100 < 0 || user_input_money->_500 < 0 || user_input_money->_1000 < 0) {
					cout << endl;
					cout << "�ʰ��� ��" << endl;
					cout << "������ �ܵ� �ⱸ" << endl;
					cout << "100�� : " << abs(user_input_money->_100) << "�� ";
					cout << "500�� : " << abs(user_input_money->_500) << "�� " << endl;

					cout << endl << "���� ���Ա�(���� �ܵ� �ⱸ)" << endl;
					cout << "1000�� : " << abs(user_input_money->_1000) << "�� " << endl << endl;
					//input�� ���� �״�� change�� �ű��(������° �ŷ��� ��� input�� ���� change�� �����ֱ⶧��)
					M->change = M->input;

					//change�� ���� user_input_money�� ���� ������(�� ��쿡 Money�� �Ķ���� �� ������ �ݵ�� �����Ƿ�) �߰��Ѵ�.
					//M->change->edit_money(new Money(abs(user_input_money->_100), abs(user_input_money->_500), abs(user_input_money->_1000)));
					//���� �����ش�. 
					M->return_change();
					break;
				}
				//����ڰ� ������ ���� �ݰ��� ���뷮�� �ʰ����� ���� ���
				else {
					//����ڰ� ������ �ݾ��� Machine�� ���� �߰��Ѵ�.
					M->get_money(user_input_money);

					cout << "���Ե� �ݾ� : " << M->input->_total << endl << endl;
					int slot_num = 0;

					//������� ���� ������ ������ �̶����� �ð��� ���. 
					//U->select_slot�� false�� ��ȯ�� ��� �ð��ʰ��� �˸��� ���� �������� �ٲ� �����ش�.
					if (!U->select_slot(&slot_num)) {
						cout << endl;
						cout << "!" << TIME_LIMIT << "�� �ʰ�!" << endl << endl;
						//input�� �״�� change�� �ٲ�
						M->change = M->input;
						//������ ����� ��
						M->optimal_change(M->change->_total);
						//�����ش�.
						M->return_change();
						break;
					}

					//�б� 1. ��ȯ ������ �ƴѰ�?
					if (slot_num != 11 && slot_num != 0) {
						//�б� 2. ��ϵ� �����ΰ�?
						if (M->_drink[slot_num - 1]._isRegist) {
							//�б� 3. ��� �ִ°�?
							if (M->_drink[slot_num - 1]._stock > 0) {
								//�б� 4. ���Աݾ��� ����Ѱ�?
								if (M->_drink[slot_num - 1]._price <= M->input->_total) {
									//�ܵ����
									int return_to_user = M->calculate(slot_num);
									//cout << return_to_user << endl;

									//��賻���� ������ ���� �ܵ� ��ȯ ��� ���
									//�б� 5. & ���� ������ �� �ִ°�?
									if (M->optimal_change(return_to_user)) {
										//���Ǳ� ���� ��� ���̱�
										M->_drink[slot_num - 1].set_stock(M->_drink[slot_num - 1]._stock - 1);
										cout << M->_drink[slot_num - 1]._name << " ��(��) ���� �ŷ��Ǿ����ϴ�." << endl;
										cout << "���� �ⱸ" << endl;
										cout << "���Ḧ ��������!*^^*" << endl << endl;
										//���� �ŷ��� ��� �̾��.
									}
									else {
										cout << "!��� �� �ܵ� ����!" << endl << endl;
										//���� �״�� �����ְ� �ŷ� ����
										M->change = M->input;
										M->return_change();
										break;
									}
								}
								else {
									cout << "!�ݾ� ����!" << endl << endl;
									M->change = M->input;
								}
							}
							else {
								cout << "!��� ����!" << endl << endl;
								M->change = M->input;
							}
						}
						else {
							cout << "!�̵�� ����!" << endl << endl;
							M->change = M->input;
						}
					}
					//�б� 1���� ��ȯ ������ ���
					else {
						if (slot_num == 11) {
							cout << "!��ȯ ����!" << endl << endl;
							//���� ������ �� �ŷ� ����
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
		//����
		case 3:
		{
			cout << "����Ǿ����ϴ�." << endl << endl;
		}
		break;
		default:
		{
			cout << "1,2,3�� �Է� ����." << endl << endl;
			continue;
		}
		break;
		}
	}

	//������� ���� �� ����
	file_write_function();

	return 0;
}