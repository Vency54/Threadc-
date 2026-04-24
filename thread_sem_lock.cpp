#include <iostream>
#include <thread>
#include <random>
#include <mutex>
#include <chrono>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>

using namespace std;
using namespace chrono;
using namespace std::chrono;

mutex BancoMutex;

int qtde_iteracoes = 500;
int qtde_threads = 20;
float saldo = 1500.00;

using chrono::system_clock;
duration<int, ratio<60 * 60 * 24>> um_dia(1);

system_clock::time_point hoje = system_clock::now();

int contador_exibicao = 1;

void Banco(int i)
{
    this_thread::sleep_for(chrono::seconds(2));

    {

        random_device rd;
        mt19937 gen(rd());

        uniform_int_distribution<> dist(100, 1000);

        float acao;

        acao = dist(gen);

        time_t tt;

        tt = system_clock::to_time_t(hoje);

        tm *data_local = localtime(&tt);

        fstream MyFile;
        MyFile.open("Banco-semLock.txt", ios::app);
        if (MyFile.is_open())
        {
            MyFile << "Acesso " << contador_exibicao << "° :" << endl;

            MyFile << "Data: " << put_time(data_local, "%d/%m/%Y") << endl;

            if (i % 2 == 0)
            {
                MyFile << "Usuário depositou R$ " << acao << " em sua conta bancária" << endl;
                saldo = saldo + acao;
            }
            else
            {
                MyFile << "Usuário sacou R$ " << acao << " de sua conta bancária" << endl;
                saldo = saldo - acao;
            }

            MyFile << "Saldo Atual: " << saldo << endl;
            MyFile << "================================================================" << endl;

            MyFile.close();
        }

        hoje = hoje + um_dia;

        contador_exibicao++;
    }
}

int main()
{

    fstream MyFile;
    MyFile.open("Banco-semLock.txt", ios::out);
    if (MyFile.is_open())
    {
        MyFile << "Extrato Bancário" << endl;
        MyFile << "================================================================" << endl;

        MyFile.close();
    }

    cout << "Gerando extrato bancário..." << endl;
    cout << endl;

    vector<thread> pool;

    for (int i = 0; i < qtde_threads; i++)
    {
        pool.emplace_back(Banco, i);
    }

    for (auto &t : pool)
    {
        t.join();
    }

    return 0;
}