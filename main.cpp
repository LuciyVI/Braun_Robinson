#include <boost/range/algorithm/count.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/random/random_device.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/random.hpp>
#include <boost/format.hpp>
#include <algorithm>
#include <iostream>
#include <vector>



class NumberGenerator {
    int current;
public:
    NumberGenerator() : current(0) {}
    int next() {
        int value = current;
        current = (current + 1) % 3;
        return value;
    }
};

using namespace boost::numeric::ublas;

// using namespace std;

/*
    [1, 11, 11],
    [7, 5, 8],
    [16, 6, 2]
                   */


int main() {
NumberGenerator generator;
matrix<double> strategy_matrix(3, 3);
    strategy_matrix(0, 0) = 1;
    strategy_matrix(0, 1) = 11;
    strategy_matrix(0, 2) = 11;
    strategy_matrix(1, 0) = 7;
    strategy_matrix(1, 1) = 5;
    strategy_matrix(1, 2) = 8;
    strategy_matrix(2, 0) = 16;
    strategy_matrix(2, 1) = 6;
    strategy_matrix(2, 2) = 2;

    double eps = 0.001;

    matrix<double> strategy_matrix_obr(3, 3);
    matrix<double> strategy_matrix_inv(3, 3);

    permutation_matrix<std::size_t> pm(strategy_matrix.size1());

    lu_factorize(strategy_matrix, pm);
    strategy_matrix_obr.assign(identity_matrix<double>(strategy_matrix.size1()));
    lu_substitute(strategy_matrix, pm, strategy_matrix_obr);


    vector<double> u(3);
    u(0) = 1;
    u(1) = 1;
    u(2) = 1;
    
    vector<double> _X(3);
    _X = prod(trans(u), strategy_matrix_obr) / inner_prod(prod(trans(u), strategy_matrix_obr), u);
    std::cout<<"Analitic_X "<<_X<<std::endl;
    vector<double> _Y(3);
    _Y = prod(trans(strategy_matrix_obr), trans(u)) / inner_prod(prod(trans(u), strategy_matrix_obr), u);
    std::cout<<"Analitic_Y "<<_Y<<std::endl;
    double _V = 1 / inner_prod(prod(u, strategy_matrix_obr), trans(u));
    std::cout<<"Analitic_V "<<_V<<std::endl;
// Corrected calculation of _Y

    std::vector<int> strA;
    std::vector<int> strB;
    std::vector<double> high_cost;
    std::vector<double> low_cost;
    std::vector<double> eps_fact;

    
    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> dist(0, 2);

    int num_strA = dist(rng);
    int num_strB = dist(rng);
    strA.push_back(num_strA);
    strB.push_back(num_strB);

    std::vector<double> winA;
    std::vector<double> loseB;


    winA.push_back(strategy_matrix(0, num_strB));
    winA.push_back(strategy_matrix(1, num_strB));
    winA.push_back(strategy_matrix(2, num_strB));
    loseB.push_back(strategy_matrix(num_strA, 0));
    loseB.push_back(strategy_matrix(num_strA, 1));
    loseB.push_back(strategy_matrix(num_strA, 2));
    
    std::cout<<winA[0]<<std::endl;
    std::cout<<winA[1]<<std::endl;
    std::cout<<winA[2]<<std::endl;

    std::cout<<loseB[0]<<std::endl;
    std::cout<<loseB[1]<<std::endl;
    std::cout<<loseB[2]<<std::endl;
    
    
    high_cost.push_back(*std::max_element(winA.begin(), winA.end()));

    low_cost.push_back(*std::min_element(loseB.begin(), loseB.end()));
    std::cout<<"Operation result << "<<*std::min_element(high_cost.begin(), high_cost.end()) - *std::max_element(low_cost.begin(), low_cost.end())<<std::endl;
    eps_fact.push_back(*std::min_element(high_cost.begin(), high_cost.end()) - *std::max_element(low_cost.begin(), low_cost.end()));
    

    int k = 1;
    int iteration=0;
    std::cout<<"EPS="<<eps_fact[k-1]<<std::endl;
    std::cout << "Итерация  Выбор игрока А  Выбор игрока В  Выигрыш 1 игрока А  Выигрыш 2 игрока А  Выигрыш 3 игрока А  Проигрыш 1 игрока В  Проигрыш 2 игрока В  Проигрыш 3 игрока В  Верхняя цена игры  Нижняя цена игры\t   eps\n";
    
    while ( eps_fact[k-1] > eps) {
        iteration = generator.next();
        num_strA = std::max_element(winA.begin(), winA.end()) - winA.begin();
        num_strB = std::min_element(loseB.begin(), loseB.end()) - loseB.begin();
        
        strA.push_back(num_strA);
        strB.push_back(num_strB);
        
        winA.push_back(winA[k - 2] + strategy_matrix(0, iteration));
        winA.push_back(winA[k - 2] + strategy_matrix(1, iteration));
        winA.push_back(winA[k - 2] + strategy_matrix(2, iteration));
        
        loseB.push_back(loseB[k - 2] + strategy_matrix(iteration, 0));
        loseB.push_back(loseB[k - 2] + strategy_matrix(iteration, 1));
        loseB.push_back(loseB[k - 2] + strategy_matrix(iteration, 2));
        
        high_cost.push_back(1.0 / k * *std::max_element(winA.begin(), winA.end()));
        low_cost.push_back(1.0 / k * *std::min_element(loseB.begin(), loseB.end()));
        eps_fact.push_back(*std::min_element(high_cost.begin(), high_cost.end()) - *std::max_element(low_cost.begin(), low_cost.end()));
        std::cout << boost::format("%-16d%-16d%-16d%-19.4f%-19.4f%-25.4f%-19.4f%-19.4f%-19.4f%-23.4f%-19.4f%-27.4f\n")
                % k
                % strA[k]
                % strB[k]
                % winA[k]
                % winA[k]
                % winA[k]
                % loseB[k]
                % loseB[k]
                % loseB[k]
                % high_cost[k]
                % low_cost[k]
                % eps_fact[k];
        k++;
    }


    /* Вывод результатов */
    std::cout << "\tAnalytical Solution:\n";
    std::cout << "\t x = " << _X << "\n";
    std::cout << "\t y = " << _Y << "\n";
    std::cout << "\t v = " << _V << "\n";

    std::cout << "\t Numerical Solution using the Brown-Robinson algorithm:\n";
    std::cout << "\t x = (" << boost::count(strA, 0) / static_cast<double>(k) << ", " << boost::count(strA, 1) / static_cast<double>(k) << ", " << boost::count(strA, 2) / static_cast<double>(k) << ")\n";
    std::cout << "\t y = (" << boost::count(strB, 0) / static_cast<double>(k) << ", " << boost::count(strB, 1) / static_cast<double>(k) << ", " << boost::count(strB, 2) / static_cast<double>(k) << ")\n";
    std::cout << "\t v = " << high_cost[k - 1] << "\n";
    return 0;
}
