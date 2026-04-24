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

int qtde_interacoes = 10;
int qtde_threads = 20;
int saldo = 10000;

using chrono::system_clock;
duration<int, ratio<60 * 60 * 24>> um_dia(1);

system_clock::time_point hoje = system_clock::now();

int contador_exibicao = 1;

void Banco()
{
    this_thread::sleep_for(chrono::seconds(5));

    {
        unique_lock<mutex> BancoLock(BancoMutex);

        for (int j = 0; j < qtde_interacoes; j++) {
            random_device rd;
            mt19937 gen(rd());

            uniform_int_distribution<int> dist(10000, 100000);

            int acao = dist(gen) / 100.0f;

            time_t tt;

            tt = system_clock::to_time_t(hoje);

            tm data_local;
            localtime_s(&data_local, &tt);

            fstream MyFile;
            MyFile.open("Banco.txt", ios::app);
            if (MyFile.is_open())
            {
                MyFile << "Acesso " << contador_exibicao << "° :" << endl;

                MyFile << "Data: " << put_time(&data_local, "%d/%m/%Y") << endl;

                if (j % 2 == 0)
                {
                    MyFile << "Usuário depositou R$ " << acao << " em sua conta bancária" << endl;
                    saldo = saldo + acao;
                }
                else
                {
                    MyFile << "Usuário sacou R$ " << acao << " de sua conta bancária" << endl;
                    saldo = saldo - acao;
                }

                MyFile << "Saldo Atual: R$ " << saldo << endl;
                MyFile << "================================================================" << endl;

                MyFile.close();
            }

            hoje = hoje + um_dia;

            contador_exibicao++;
        }

        BancoLock.unlock();
    }
}

int main()
{

    fstream MyFile;
    MyFile.open("Banco.txt", ios::out);
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
        pool.emplace_back(Banco);
    }

    for (auto& t : pool)
    {
        t.join();
    }

    return 0;
}
