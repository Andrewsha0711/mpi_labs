//============================================================================
// Name        : mpi_lab5.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "mpi.h"
#include <queue>
#include <set>
#include "math.h"
using namespace std;

struct LongInt {
    char value[1000];
    bool sign;
};

struct BigInt {
    string value; // значение числа
    bool sign; // флаг отрицательности

public:
    static BigInt karatsuba_mul(const BigInt& a, const BigInt& b); // умножение методом Карацубы
    BigInt(); // конструктор по умолчанию (= 0)
    BigInt(long x); // конструктор из long
    BigInt(const string& value); // конструктор из строки
    BigInt(const BigInt& value); // копирование
    const string& getValue() const;
    const bool getSign() const;
    void setSign(bool value);

    const bool operator==(const BigInt& bigInt) const {
        return (value == bigInt.getValue()) && (sign == bigInt.getSign());
    }

    const bool operator!=(const BigInt& bigInt) const {
        return !(*this == bigInt);
    }

    const bool operator<(const BigInt& bigInt) const {
        string value2 = bigInt.getValue(); // получаем значение второго числа
        size_t len1 = value.length(); // запоминаем длину первого числа
        size_t len2 = value2.length(); // запоминаем длину второго числа

        // если знаки одинаковые, то проверяем значения
        if (sign == bigInt.getSign()) {
            // если длины не равны
            if (len1 != len2)
                return (len1 < len2) ^ sign; // меньше число с меньшей длинной для положительных и с большей длиной для отрицательных
            size_t i = 0;
            // ищем разряд, в котором значения отличаются
            while (i < len1 && value[i] == value2[i])
                i++;
            // если разряд найден, то меньше число с меньшей цифрой для положительных и с большей цифрой для отрицательных, иначе числа равны
            return (i < len1) && ((value[i] < value2[i]) ^ sign);
        }
        return sign; // знаки разные, если число отрицательное, то оно меньше, если положительное, то больше
    }

    // проверка, что число больше bigInt
    const bool operator>(const BigInt& bigInt) const {
        return !(*this < bigInt || *this == bigInt);
    }

    // проверка, что число меньше или равно bigInt
    const bool operator<=(const BigInt& bigInt) const {
        return *this < bigInt || *this == bigInt;
    }

    // проверка, что число больше или равно bigInt
    const bool operator>=(const BigInt& bigInt) const {
        return *this > bigInt || *this == bigInt;
    }

    BigInt operator<<(size_t n) const {
        return BigInt(string(sign ? "-" : "") + value + string(n, '0'));
    }

    BigInt operator>>(size_t n) const {
        if (n >= value.length())
            return 0;

        return BigInt(string(sign ? "-" : "") + value.substr(0, value.length() - n));
    }

    // унарный минус
    BigInt operator-() const&& {
        return BigInt(sign ? value : string("-") + value);
    }

    // унарный плюс
    BigInt operator+() const&& {
        return BigInt(*this);
    }

    // сложение двух чисел
    BigInt operator+(const BigInt& bigInt) const {
        bool isAddOp = !(bigInt.getSign() ^ sign); // если знаки одинаковые, то выполняем сложение

        if (isAddOp) {
            string num2 = bigInt.getValue(); // запоминаем значение второго числа
            size_t len1 = value.length(); // запоминаем длину первого числа
            size_t len2 = num2.length(); // запоминаем длину второго числа
            size_t length = 1 + max(len1, len2);  // длина суммы равна максимуму из двух длин + 1 из-за возможного переноса разряда

            char* res = new char[length + 1]; // строковый массив для выполнения операции сложения

            res[length - 1] = res[length] = '\0';

            for (size_t i = 0; i < length - 1; i++) {
                int j = length - 1 - i;
                res[j] += ((i < len2) ? (num2[len2 - 1 - i] - '0') : 0) + ((i < len1) ? (value[len1 - 1 - i] - '0') : 0); // выполняем сложение разрядов
                res[j - 1] = res[j] / 10; // выполняем перенос в следущий разряд, если он был
                res[j] = res[j] % 10 + '0'; // оставляем только единицы от возможного переноса и превращаем символ в цифру
            }
            res[0] += '0';
            return BigInt(sign ? string("-") + string(res) : string(res)); // возвращаем результат, в зависимости от знака`
        }
        else
            return sign ? (bigInt - (-BigInt(*this))) : (*this - (-BigInt(bigInt))); // одно из чисел отрицательное, а другое положительное, отправляем на вычитание, меняя знак
    }

    // вычитание двух чисел
    BigInt operator-(const BigInt& bigInt) const {
        if (*this == bigInt)
            return 0;

        // если оба числа положительные, то выполняем вычитание
        if (!sign && !bigInt.getSign()) {
            string value2 = bigInt.getValue(); // запоминаем значение второго числа

            size_t len1 = value.length(); // запоминаем длину первого числа
            size_t len2 = value2.length(); // запоминаем длину второго числа
            size_t length = max(len1, len2); // длина результата не превысит максимума длин чисел

            bool isNegRes = bigInt > *this; // определяем знак результата

            int* a = new int[length];
            int* b = new int[length];

            a[0] = b[0] = 0; // обнуляем нулевые элементы массивов

            char* res = new char[length + 1]; // строковый массив для результата
            res[length - 1] = res[length] = '\0'; // устанавливаем символ окончания строки

            int sign = (2 * isNegRes - 1); // получаем числовое значение знака результата

            for (size_t i = 0; i < length - 1; i++) {
                a[i] += (i < len1) ? (value[len1 - 1 - i] - '0') : 0; // формируем разряды
                b[i] += (i < len2) ? (value2[len2 - 1 - i] - '0') : 0; // из строк аргументов

                b[i + 1] = -isNegRes; // в зависимости от знака занимаем или не занимаем
                a[i + 1] = isNegRes - 1; // 10 у следующего разряда

                res[length - 1 - i] += 10 + sign * (b[i] - a[i]);
                res[length - 1 - i - 1] = res[length - 1 - i] / 10;
                res[length - 1 - i] = res[length - 1 - i] % 10 + '0';
            }

            // выполняем операцию с последним разрядом
            a[length - 1] += (length - 1 < len1) * (value[0] - '0');
            b[length - 1] += (length - 1 < len2) * (value2[0] - '0');

            // записываем в строку последний разряд
            res[0] += sign * (b[length - 1] - a[length - 1]) + '0';

            return BigInt(isNegRes ? string("-") + string(res) : string(res)); // возвращаем результат, учитывая знак
        }
        else // если оба числа отрицательные, то меняем местами, меняем знаки и повторяем вычитание, а если знаки разные, то отправляем на сумму
            return sign && bigInt.getSign() ? (-BigInt(bigInt) - (-BigInt(*this))) : (*this + -BigInt(bigInt));
    }
};

BigInt::BigInt() {
    this->sign = false;
    this->value = "0";
}

BigInt::BigInt(long x) {
    this->sign = x < 0;
    this->value = to_string(sign ? -x : x);
}

BigInt::BigInt(const string& value) {
    if (!value.length()) {
        this->value = "0";
        sign = false;
        return;
    }
    sign = value[0] == '-';
    this->value = value.substr(sign);

    // определяем число ведущих нулей в строке
    int count = 0;
    while (this->value[count] == '0' && this->value.length() - count > 1)
        count++;
    this->value = this->value.substr(count);

    for (size_t i = 0; i < this->value.length(); i++)
        if (this->value[i] < '0' || this->value[i] > '9')
            throw string("BigInt(const string &value) - string contain incorrect characters: ") + this->value;
}

BigInt::BigInt(const BigInt& bigInt) {
    this->value = bigInt.getValue();
    this->sign = bigInt.getSign();
}

LongInt strToChar(bool isNeg, string str) {
    LongInt buf = LongInt();
    strcpy(buf.value, str.c_str());
    buf.sign = isNeg;
    return buf;
}

const string& BigInt::getValue() const {
    return value;
}

const bool BigInt::getSign() const {
    return sign;
}

void BigInt::setSign(bool isNeg) {
    this->sign = isNeg;
}

LongInt mul(LongInt a, LongInt b) {
    BigInt c = BigInt(a.value);
    c.setSign(a.sign);
    BigInt d = BigInt(b.value);
    d.setSign(b.sign);
    BigInt e = BigInt::karatsuba_mul(c, d);
    LongInt res = LongInt();
    res = strToChar(e.getSign(), e.getValue());
    return res;
}

BigInt BigInt::karatsuba_mul(const BigInt& a, const BigInt& b) {
    string v1 = a.getValue();
    string v2 = b.getValue();

    size_t len1 = v1.length();
    size_t len2 = v2.length();
    size_t len = std::max(len1, len2);

    if (len1 + len2 < 9)
        return stol(a.getValue()) * stol(b.getValue());

    len += len % 2;
    size_t n = len / 2;

    BigInt Xr(len1 > n ? v1.substr(len1 - n, n) : v1);
    BigInt Xl(a >> n);
    BigInt Yr(len2 > n ? v2.substr(len2 - n, n) : v2);
    BigInt Yl(b >> n);

    BigInt P1 = karatsuba_mul(Xl, Yl);
    BigInt P2 = karatsuba_mul(Xr, Yr);
    BigInt P3 = karatsuba_mul(Xl + Xr, Yl + Yr);

    return (P1 << len) + ((P3 - P2 - P1) << n) + P2;
}

void count(queue<LongInt> q) {
    LongInt res = strToChar(false, "1");

    for (int i = 0; i < (int)(q.size()); i++) {
        LongInt x;
        x = q.front();
        q.pop();
        res = mul(x, res);
        q.push(x);
    }
    string digin = "+";
    if (res.sign) digin = "-";
    cout << "single process: " << "\n" << digin << res.value << endl;
}

int main(int argc, char* argv[]) {
    const int master = 0;
	const int from_master = 0;
	const int from_slave = 1;
	const int N = 3;
	const int A = 8;
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int proc_num, proc_rank;
//	int recv_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    MPI_Datatype long_number;
    MPI_Datatype type[2] = { MPI_CHAR, MPI_C_BOOL };
    int blocklen[2] = { 1000, 1 };
    MPI_Aint disp[2];

    disp[0] = 0;
    disp[1] = 1000;

    MPI_Type_create_struct(2, blocklen, disp, type, &long_number);
    MPI_Type_commit(&long_number);

    queue<LongInt> q;

    for (int i = 0; i < A; i++) {
        long temp = rand() % (2 * (int)pow((float)10, (float)N)) - (int)pow((float)10, (float)N);
        BigInt a = BigInt(temp);
        LongInt buff;
        buff = strToChar(a.getSign(), a.getValue());
        q.push(buff);
    }

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int ranks12[2] = { 1, 2 };
    int ranks34[2] = { 3, 4 };
    int ranks13[2] = { 1, 3 };

    MPI_Group group12, group34, group13;
    MPI_Group_incl(world_group, 2, ranks12, &group12);
    MPI_Group_incl(world_group, 2, ranks34, &group34);
    MPI_Group_incl(world_group, 2, ranks13, &group13);

    MPI_Comm comm12, comm34, comm13;
    MPI_Comm_create(MPI_COMM_WORLD, group12, &comm12);
    MPI_Comm_create(MPI_COMM_WORLD, group34, &comm34);
    MPI_Comm_create(MPI_COMM_WORLD, group13, &comm13);

    int proc_num12, proc_num34, proc_num13, proc_rank12, proc_rank34, proc_rank13;
    if (comm12 != MPI_COMM_NULL) {
        MPI_Comm_size(comm12, &proc_num12);
        MPI_Comm_rank(comm12, &proc_rank12);
    }

    if (comm34 != MPI_COMM_NULL) {
        MPI_Comm_size(comm34, &proc_num34);
        MPI_Comm_rank(comm34, &proc_rank34);
    }

    if (comm13 != MPI_COMM_NULL) {
        MPI_Comm_size(comm13, &proc_num13);
        MPI_Comm_rank(comm13, &proc_rank13);
    }

    //множество главных процессов
    set<int> masters;
    LongInt master_ans;
    master_ans = strToChar(false, "1");

    double from;
    double runtime;

    // Запуск в одном процессе
    if (proc_rank == master) {
        from = MPI_Wtime();
        count(q);
        runtime = MPI_Wtime() - from;
        std::cout << "time: " << runtime << std::endl;
        from = MPI_Wtime();
    }

    while (q.size() > 0) {
        LongInt c0;
        masters.clear();
        if (proc_rank == master) {
            for (int i = 0; i < proc_num - 1; i++) {
                LongInt x;
                if (q.empty()){
                	x = strToChar(false, "1");
                }
                else {
                    x = q.front();
                    q.pop();
                }
                LongInt y;
                if (q.empty())
                    y = strToChar(false, "1");
                else {
                    y = q.front();
                    q.pop();
                }
                MPI_Send(&(x), 1, long_number, i + 1, from_master, MPI_COMM_WORLD);
                MPI_Send(&(y), 1, long_number, i + 1, from_master, MPI_COMM_WORLD);
            }
        }
        else {
            LongInt x;
            LongInt y;

            c0 = strToChar(false, "1"); // промежуточный результат в каждом процессе
            MPI_Recv(&(x), 1, long_number, master, from_master, MPI_COMM_WORLD, &status);
            MPI_Recv(&(y), 1, long_number, master, from_master, MPI_COMM_WORLD, &status);

            c0 = mul(x, y);
        }

        LongInt res;
        res = strToChar(false, "1");
        masters.insert(1);
        masters.insert(3);
        if (masters.find(proc_rank) != masters.end()){
            LongInt get;

            if (proc_rank == 1) {
                MPI_Recv(&get, 1, long_number, 1, from_slave, comm12, &status);
            }
            else {
                MPI_Recv(&get, 1, long_number, 1, from_slave, comm34, &status);
            }
            res = mul(get, c0);
        }
        else {
            if (proc_rank == 2) {
                MPI_Send(&c0, 1, long_number, 0, from_slave, comm12);
            }
            else if (proc_rank == 4) {
                MPI_Send(&c0, 1, long_number, 0, from_slave, comm34);
            }
        }

        // передача результата процессу 1 из процесса 3
        masters.erase(3);
        if (masters.find(proc_rank) != masters.end()) {
            LongInt get = LongInt();
            MPI_Recv(&get, 1, long_number, 1, from_slave, comm13, &status);

            res = mul(res, get);
        }
        else {
            if (proc_rank == 3) {
                MPI_Send(&res, 1, long_number, 0, from_slave, comm13);
            }
        }
        // возвращаем значение процессу 0 из 1-го процессора
        if (proc_rank == master) {
            LongInt get = LongInt();
            MPI_Recv(&get, 1, long_number, 1, from_slave, MPI_COMM_WORLD, &status);
            master_ans = mul(master_ans, get);
        }
        else if (proc_rank == 1) {
            MPI_Send(&res, 1, long_number, master, from_slave, MPI_COMM_WORLD);
        }
    }
    if (proc_rank == master) {
        string digin = "+";
        if (master_ans.sign) digin = "-";
        cout << "result: " << "\n" << digin << master_ans.value << endl;
        runtime = MPI_Wtime() - from;
        std::cout << "time: " << runtime << std::endl;
    }

    MPI_Group_free(&group12);
    MPI_Group_free(&group34);
    MPI_Group_free(&group13);
    if (comm12 != MPI_COMM_NULL) MPI_Comm_free(&comm12);
    if (comm34 != MPI_COMM_NULL) MPI_Comm_free(&comm34);
    if (comm13 != MPI_COMM_NULL) MPI_Comm_free(&comm13);
    MPI_Type_free(&long_number);

    MPI_Finalize();
    return 0;
}
