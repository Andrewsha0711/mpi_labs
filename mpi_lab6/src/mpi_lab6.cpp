//============================================================================
// Name        : mpi_lab6.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <iostream>
#include "mpi.h"
#include <queue>
#include <string>
#include <set>
#include <complex>
#include <cmath>
#include <sstream>
#include "math.h"
using namespace std;

#define PI M_PI

struct LongInt {
    char value[1000];
    bool sign;
};

struct BigInt {
    std::string value;
    bool sign;

public:
    BigInt();
    BigInt(long x);
    BigInt(const std::string& value);
    BigInt(const BigInt& value);
    const std::string& getValue() const;
    const bool getSign() const;
    void setSign(bool value);

    const bool operator==(const BigInt& bigInt) const {
        return (value == bigInt.getValue()) && (sign == bigInt.getSign());
    }

    const bool operator!=(const BigInt& bigInt) const {
        return !(*this == bigInt);
    }

    const bool operator<(const BigInt& bigInt) const {
        std::string value2 = bigInt.getValue(); // получаем значение второго числа
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

    const bool operator>(const BigInt& bigInt) const {
        return !(*this < bigInt || *this == bigInt);
    }

    const bool operator<=(const BigInt& bigInt) const {
        return *this < bigInt || *this == bigInt;
    }

    const bool operator>=(const BigInt& bigInt) const {
        return *this > bigInt || *this == bigInt;
    }

    BigInt operator<<(size_t n) const {
        return BigInt(std::string(sign ? "-" : "") + value + std::string(n, '0'));
    }

    BigInt operator>>(size_t n) const {
        if (n >= value.length())
            return 0;

        return BigInt(std::string(sign ? "-" : "") + value.substr(0, value.length() - n));
    }

    // унарный минус
    BigInt operator-() const&& {
        return BigInt(sign ? value : std::string("-") + value);
    }

    // унарный плюс
    BigInt operator+() const&& {
        return BigInt(*this);
    }

    // сложение двух чисел
    BigInt operator+(const BigInt& bigInt) const {
        bool isAddOp = !(bigInt.getSign() ^ sign); // если знаки одинаковые, то выполняем сложение

        if (isAddOp) {
            std::string num2 = bigInt.getValue(); // запоминаем значение второго числа

            size_t len1 = value.length(); // запоминаем длину первого числа
            size_t len2 = num2.length(); // запоминаем длину второго числа
            size_t length = 1 + std::max(len1, len2);  // длина суммы равна максимуму из двух длин + 1 из-за возможного переноса разряда

            char* res = new char[length + 1]; // строковый массив для выполнения операции сложения

            res[length - 1] = res[length] = '\0';

            for (size_t i = 0; i < length - 1; i++) {
                int j = length - 1 - i;
                res[j] += ((i < len2) ? (num2[len2 - 1 - i] - '0') : 0) + ((i < len1) ? (value[len1 - 1 - i] - '0') : 0); // выполняем сложение разрядов
                res[j - 1] = res[j] / 10; // выполняем перенос в следущий разряд, если он был
                res[j] = res[j] % 10 + '0'; // оставляем только единицы от возможного переноса и превращаем символ в цифру
            }

            res[0] += '0';

            return BigInt(sign ? std::string("-") + std::string(res) : std::string(res)); // возвращаем результат, в зависимости от знака`
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
            std::string value2 = bigInt.getValue(); // запоминаем значение второго числа

            size_t len1 = value.length(); // запоминаем длину первого числа
            size_t len2 = value2.length(); // запоминаем длину второго числа
            size_t length = std::max(len1, len2); // длина результата не превысит максимума длин чисел

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

            return BigInt(isNegRes ? std::string("-") + std::string(res) : std::string(res)); // возвращаем результат, учитывая знак
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
    this->value = std::to_string(sign ? -x : x);
}

BigInt::BigInt(const std::string& value) {
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

    this->value = this->value.substr(count); // удаляем ведущие нули

    // проверяем "на цифру" каждый символ строки, кидаем исключение, если есть другие символы
    for (size_t i = 0; i < this->value.length(); i++)
        if (this->value[i] < '0' || this->value[i] > '9')
            throw string("BigInt(const string &value) - string contain incorrect characters: ") + this->value;
}

// копирование
BigInt::BigInt(const BigInt& bigInt) {
    this->value = bigInt.getValue();
    this->sign = bigInt.getSign();
}

LongInt strToChar(bool sign, string str) {
    LongInt buf = LongInt();
    strcpy(buf.value, str.c_str());
    buf.sign = sign;
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

string vectorToString(vector<int> v) {
    string res = "";
    for (int& elem : v) {
        res += to_string(elem);
    }
    return res;
}

vector<int> stringToVector(string str) {
    vector<int> res;
    for (int i = 0; i < (int)(str.length()); i++) {
        res.push_back((int)str[i] - 48);
    }
    return res;
}

typedef complex<double> base;
void fft(vector<base>& a, bool invert) {
    int n = (int)a.size();
    if (n == 1)  return;

    vector<base> a0(n / 2), a1(n / 2);
    for (int i = 0, j = 0; i < n; i += 2, ++j) {
        a0[j] = a[i];
        a1[j] = a[i + 1];
    }
    fft(a0, invert);
    fft(a1, invert);

    double ang = 2 * PI / n * (invert ? -1 : 1);
    base w((_Complex double)1), wn(cos((float)ang), sin((float)ang));
    for (int i = 0; i < n / 2; ++i) {
        a[i] = a0[i] + w * a1[i];
        a[i + n / 2] = a0[i] - w * a1[i];
        if (invert)
            a[i] /= 2, a[i + n / 2] /= 2;
        w *= wn;
    }
}

BigInt fft_multiply(const BigInt& left, const BigInt& right) {
    vector<int> a = stringToVector(left.getValue());
    vector<int> b = stringToVector(right.getValue());

    if (a.size() > 1)
        reverse(a.begin(), a.end());
    if (b.size() > 1)
        reverse(b.begin(), b.end());

    vector<int> res;

    vector<base> fa(a.begin(), a.end()), fb(b.begin(), b.end());
    size_t n = 1;
    while (n < max(a.size(), b.size()))  n <<= 1;
    n <<= 1;
    fa.resize(n), fb.resize(n);

    fft(fa, false), fft(fb, false);
    for (size_t i = 0; i < n; ++i)
        fa[i] *= fb[i];
    fft(fa, true);

    res.resize(n);
    for (size_t i = 0; i < n; ++i)
        res[i] = int(fa[i].real() + 0.5);

    int carry = 0;
    for (size_t i = 0; i < n; ++i) {
        res[i] += carry;
        carry = res[i] / 10;
        res[i] %= 10;
    }

    reverse(res.begin(), res.end());

    BigInt result = BigInt(vectorToString(res));
    result.setSign(left.getSign() ^ right.getSign());
    return result;
}

LongInt multiply(LongInt a, LongInt b) {
    string aString = a.value;
    BigInt c = BigInt(aString);
    c.setSign(a.sign);

    string bString = b.value;
    BigInt d = BigInt(bString);
    d.setSign(b.sign);

    BigInt e = fft_multiply(c, d);
    LongInt res = LongInt();
    res = strToChar(e.getSign(), e.getValue());
    return res;
}

void count(queue<LongInt> q) {
    LongInt res = strToChar(false, "1");

    for (int i = 0; i < (int)(q.size()); i++) {
        LongInt x;
        x = q.front();
        q.pop();
        res = multiply(x, res);
        q.push(x);
    }
    string digin = "+";
    if (res.sign) digin = "-";
    cout << "single process: " << "\n" << digin << res.value << endl;
}


int main(int argc, char* argv[]) {
	const int N = 3;
	const int A = 50;
    MPI_Init(&argc, &argv);

    MPI_Status status;

    int proc_num, proc_rank;
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
        long temp = rand() % (2 * (int)pow((float)10, N)) - (int)pow((float)10, N);
        BigInt a = BigInt(temp);
        LongInt buff;
        buff = strToChar(a.getSign(), a.getValue());
        q.push(buff);
    }

    const int number_of_nodes = 7;
    int indexes[number_of_nodes] = { 4, 5, 6, 7, 8, 9, 10 };
    int edges[10] = { 1, 2, 3, 4, 5, 5, 6, 6, 0, 0 };
    int reorder = false;
    MPI_Comm my_graph;
    MPI_Graph_create(MPI_COMM_WORLD, number_of_nodes, indexes, edges, reorder, &my_graph);


    LongInt master_ans;
    master_ans = strToChar(false, "1");


    double from;
    double runtime;

    if (proc_rank == 0) {
        from = MPI_Wtime();
        count(q);
        runtime = MPI_Wtime() - from;
        std::cout << "time: " << runtime << std::endl;
        from = MPI_Wtime();
    }

    int zero_layout = 0;
    set <int> first_layout{ 1, 2, 3, 4 }; // Индексы первого слоя графа
    set <int> second_layout{ 5, 6 }; // Индексы второго слоя графа

    while (q.size() > 0) {
        // 0-ой процесс
        if (proc_rank == zero_layout) {
            //cout << q.size() << endl;
            for (int i = 1; i <= 4; i++) {
                LongInt x;
                if (q.empty())
                    x = strToChar(false, "1");
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
                MPI_Send(&(x), 1, long_number, i, 0, MPI_COMM_WORLD);
                MPI_Send(&(y), 1, long_number, i, 1, MPI_COMM_WORLD);
            }
            LongInt a;
            LongInt b;
            MPI_Recv(&(a), 1, long_number, 5, 5, MPI_COMM_WORLD, &status);
            MPI_Recv(&(b), 1, long_number, 6, 6, MPI_COMM_WORLD, &status);

            LongInt iter_result = multiply(a, b);
            master_ans = multiply(master_ans, iter_result);
        }
        // Процессы с 1 по 4 (первый слой)
        else if (first_layout.find(proc_rank) != first_layout.end()) {
            LongInt x;
            LongInt y;

            LongInt c0; // результат умножения в каждом процессе

            MPI_Recv(&(x), 1, long_number, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&(y), 1, long_number, 0, 1, MPI_COMM_WORLD, &status);

            c0 = multiply(x, y);

            int neighbor;
            MPI_Graph_neighbors(my_graph, proc_rank, 1, &neighbor);
            MPI_Send(&(c0), 1, long_number, neighbor, proc_rank, MPI_COMM_WORLD);
        }
        else if (proc_rank == 5 || proc_rank == 6) {
            LongInt x;
            LongInt y;

            LongInt c0; // результат умножения в каждом процессе

            if (proc_rank == 5){
                MPI_Recv(&(x), 1, long_number, 1, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&(y), 1, long_number, 2, 2, MPI_COMM_WORLD, &status);
            }
            if (proc_rank == 6){
                MPI_Recv(&(x), 1, long_number, 3, 3, MPI_COMM_WORLD, &status);
                MPI_Recv(&(y), 1, long_number, 4, 4, MPI_COMM_WORLD, &status);
            }
            c0 = multiply(x, y);

            MPI_Send(&(c0), 1, long_number, 0, proc_rank, MPI_COMM_WORLD);
        }
    }
    if (proc_rank == 0) {
        string digin = "+";
        if (master_ans.sign) digin = "-";
        cout << "result: " << "\n" << digin << master_ans.value << endl;
        runtime = MPI_Wtime() - from;
        std::cout << "time: " << runtime << std::endl;
    }

    if (my_graph != MPI_COMM_NULL) MPI_Comm_free(&my_graph);
    MPI_Type_free(&long_number);

    MPI_Finalize();
    return 0;
}
