// work1.cpp: определяет точку входа для консольного приложения.
//a-уст, альфа- уст, примеры методов, как находится
//анализ трудоемкости - количество операций, время для интегрирования какого либо метода для заданной точности
//число жесткости похоже на число обусловленности, 
//делить длину интервала на шаг и умножать на количество вычислений правой части
//файл и график как меняется решение в зависимости от шага

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <iomanip>
#include <cmath>
#include <functional>
#include <vector>

using namespace std;
typedef double funsysarraytype(double, double*);
std::vector<std::function<double(double, double*)> > funsysarray;
double u1n, u2n, tau;
int realkol = 0;//реальное количество точек на сетке с переменным шагом

void printVector(double* Vector, int k)
{
	cout << endl;
	for (int i = 0; i < k; i++)
	{
		cout << Vector[i] /*<< " " << i+1 */ << endl;
	}

}

void printMatrix(double** extMatrix, int k, int m, bool extended)
{
	cout << endl;
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < m; j++)
		{
			char simb = ' ';
			//if ((j == k - 1) && extended) { simb = '='; }
			//else { simb = ' '; }
			//cout <<  setprecision(2) << fixed<< extMatrix[i][j] << simb;
			cout << extMatrix[i][j] << simb;
		}
		cout << endl;
	}

}

void matrix_destroyer(double** ary, int n)
{
	if (ary != nullptr)
	{
		for (int i = 0; i < n; i++) {
			delete[] ary[i];
		}
		delete[] ary;
		ary = nullptr;
	}
}

void vector_destroyer(double* vec, int n)
{
	if (vec != nullptr)
	{

		delete[] vec;
		vec = nullptr;
	}
}

double* multiplyMatrixVector(double** Matrix, double* Vector, int n)
{
	double *result;
	result = new double[n];
	double s = 0;

	for (int i = 0; i < n; i++)
	{
		s = 0;

		for (int j = 0; j < n; j++)
		{
			s += Matrix[i][j] * Vector[j];
		}
		result[i] = s;

	}
	return result;
}

double* substractVector(double* Vector1, double* Vector2, int n)
{
	double *result;
	result = new double[n];

	for (int i = 0; i < n; i++)
	{
		result[i] = Vector1[i] - Vector2[i];
	}

	return result;
}

double normVectorInfinity(double* Vector, int n)
{
	double max = 0;
	for (int i = 0; i < n; i++)
	{
		if (abs(Vector[i]) > max) max = abs(Vector[i]);
	}
	return max;
}

double* multiplyVectorNumber(double* Vector, double Number, int n)
{
	double *result;
	result = new double[n];

	for (int i = 0; i < n; i++)
	{
		result[i] = Vector[i] * Number;
	}
	return result;
}

double** RootLocalization(double(*ff) (double), double a, double b, double eps, int& npoints)
{
	//отталкиваемся от точности и выбираем шаг (точность eps = 1e-3)
	//double h = exp(log(eps) *0.5);
	double h = 0.01;
	double quantity = abs(a - b) / h;//количество промежутков на отрезке a,b
	int n = round(quantity + 1);
	//npoints = n;//для дальнейшего использования

	double **net;
	net = new double*[2];
	for (int ii = 0; ii<2; ii++)
		net[ii] = new double[n];
	double xi, yi;

	for (int i = 0; i < n; i++)
	{//создаем таблицу из аргументов и значений функции на отрезке с определенным выше шагом
		xi = a + h * i;
		yi = ff(xi);
		//cout << xi << ' ' << yi << endl;

		net[0][i] = xi;
		net[1][i] = yi;
	}
	int kol = 0;

	for (int i = 1; i < n; i++)
	{
		if ((net[1][i]>0 && net[1][i - 1]<0)
			|| (net[1][i]<0 && net[1][i - 1]>0)
			|| (net[1][i] == 0 && net[1][i - 1]>0 && net[0][i] == b)//правый конец-корень в точке в
																	//|| (net[1][i] == 0 && net[1][i - 1]>0 && net[0][i - 1] == a)
			|| (net[1][i]>0 && net[1][i - 1] == 0)//левый конец отрезка-корень
												  //|| (net[1][i] == 0 && net[1][i - 1]<0 && net[0][i - 1] == a)
			|| (net[1][i] == 0 && net[1][i - 1]<0 && net[0][i] == b)//правый конец-корень в точке в
			|| (net[1][i]<0 && net[1][i - 1] == 0)
			)
		{
			kol++;
		}
		else if (net[1][i] == 0 && net[1][i - 1] == 0)
		{
			kol++;
			kol++;
		}

	}

	npoints = kol;
	//создаем новую таблицу с концами отрезков, содержащих корни, размерность: 2 на kol
	double **rootstable;
	rootstable = new double*[2];
	for (int ii = 0; ii<2; ii++)
		rootstable[ii] = new double[kol];

	kol = 0;
	for (int i = 1; i < n; i++)
	{
		if ((net[1][i]>0 && net[1][i - 1]<0)
			|| (net[1][i]<0 && net[1][i - 1]>0)
			|| (net[1][i] == 0 && net[1][i - 1]>0 && net[0][i - 1] == a)
			|| (net[1][i]>0 && net[1][i - 1] == 0)//ноль справа
			|| (net[1][i] == 0 && net[1][i - 1]<0 && net[0][i - 1] == a)
			|| (net[1][i]<0 && net[1][i - 1] == 0)//ноль справа
			)
		{
			rootstable[0][kol] = net[0][i - 1];
			rootstable[1][kol] = net[0][i];

			kol++;
		}

		else if (net[1][i] == 0 && net[1][i - 1] == 0)
		{
			rootstable[0][kol] = net[0][i - 1];
			rootstable[1][kol] = (net[0][i] + net[0][i + 1])*0.5;
			kol++;

			rootstable[0][kol] = (net[0][i] + net[0][i + 1])*0.5;
			rootstable[1][kol] = net[0][i];


			kol++;
		}

	}

	return rootstable;
}

double NewtonMethod(double(*ff) (double), double xleft, double xright, double eps, double(*derivative) (double))
{
	double result;
	double resultprevious;
	double der;

	result = (xright + xleft)*0.5;//начальное приближение - середина отрезка
	if ((ff(xleft) - ff(xright))*1e+10 > (ff(xleft)*xright - ff(xright)*xleft))
	{
		result = (ff(xleft)*xright - ff(xright)*xleft) / (ff(xleft) - ff(xright));//начальное приближение по методу хорд 
	}

	result = 0;

	resultprevious = result + 1;

	int cnt = 0;
	while (abs(result - resultprevious) > eps)
	{
		resultprevious = result;

		if (derivative != nullptr)
		{
			result = resultprevious - ff(resultprevious) / derivative(resultprevious);
		}

		else
		{
			der = (ff(resultprevious + eps) - ff(resultprevious)) / eps;
			result = resultprevious - ff(resultprevious) / der;

		}

		if (result<xleft || result>xright)
		{
			//возвращаем на отрезок по методу хорд
			result = (ff(resultprevious)*result - ff(result)*resultprevious) / (ff(resultprevious) - ff(result));
		}

		cnt++;
	}

	cout << endl;
	cout << "Количество итераций = " << cnt << endl;
	cout << endl;

	return result;
}

double** JacobiMatrix(double* vect, double(*derivative1) (double, double), double(*derivative2) (double, double),
	double(*derivative3) (double, double), double(*derivative4) (double, double))
{
	double **MatrixJacobi;
	MatrixJacobi = new double*[2];

	for (int ii = 0; ii < 2; ii++)
	{
		MatrixJacobi[ii] = new double[2];
	}

	MatrixJacobi[0][0] = derivative1(vect[0], vect[1]);
	MatrixJacobi[0][1] = derivative2(vect[0], vect[1]);
	MatrixJacobi[1][0] = derivative3(vect[0], vect[1]);
	MatrixJacobi[1][1] = derivative4(vect[0], vect[1]);

	return MatrixJacobi;
}


double* NewtonMethodSystem(double(*ff1) (double, double), double(*ff2) (double, double), double* x0, double eps,
	double(*derivative1) (double, double), double(*derivative2) (double, double),
	double(*derivative3) (double, double), double(*derivative4) (double, double), int* cntnumb = nullptr)
{
	double det = 0;
	double **Matrix;
	Matrix = new double*[2];
	double **MatrixRev;
	MatrixRev = new double*[2];
	for (int ii = 0; ii < 2; ii++)
	{
		MatrixRev[ii] = new double[2];
		Matrix[ii] = new double[2];
	}

	double *result;
	result = new double[2];
	double *resultprevious;
	resultprevious = new double[2];
	double *boof;
	boof = new double[2];
	double *boofnew;
	boofnew = new double[2];

	result[0] = x0[0];//присваиваем вектору результата начальное приближение
	result[1] = x0[1];

	/*result[0] = 4.0;
	result[1] = -1.0;*/

	int cnt = 0;

	while (abs(result[0] - resultprevious[0]) > eps && abs(result[1] - resultprevious[1]) > eps)
	{
		resultprevious[0] = result[0];
		resultprevious[1] = result[1];

		if (derivative1 != nullptr&& derivative2 != nullptr&& derivative3 != nullptr&& derivative4 != nullptr)
		{
			Matrix = JacobiMatrix(resultprevious, derivative1, derivative2, derivative3, derivative4);
		}//формируем матрицу Якоби analit
		else
		{
			Matrix[0][0] = (ff1(resultprevious[0] + eps, resultprevious[1]) - ff1(resultprevious[0], resultprevious[1])) / eps;
			Matrix[0][1] = (ff1(resultprevious[0], resultprevious[1] + eps) - ff1(resultprevious[0], resultprevious[1])) / eps;
			Matrix[1][0] = (ff2(resultprevious[0] + eps, resultprevious[1]) - ff2(resultprevious[0], resultprevious[1])) / eps;
			Matrix[1][1] = (ff2(resultprevious[0], resultprevious[1] + eps) - ff2(resultprevious[0], resultprevious[1])) / eps;

		}//формируем матрицу Якоби num


		 /*cout << "matrix "<< endl;
		 printMatrix(Matrix,2,2,true);
		 cout << endl;*/

		 //обращаем матрицу явно
		det = Matrix[1][1] * Matrix[0][0] - Matrix[0][1] * Matrix[1][0];
		MatrixRev[0][0] = Matrix[1][1] / det;
		MatrixRev[0][1] = -Matrix[0][1] / det;
		MatrixRev[1][0] = -Matrix[1][0] / det;
		MatrixRev[1][1] = Matrix[0][0] / det;

		/*cout << "matrixrev " << endl;
		printMatrix(MatrixRev, 2, 2, true);
		cout << endl;*/

		boofnew[0] = ff1(resultprevious[0], resultprevious[1]);
		boofnew[1] = ff2(resultprevious[0], resultprevious[1]);

		/*cout << endl;
		printVector(boofnew, 2);
		cout << endl;*/

		boof = multiplyMatrixVector(MatrixRev, boofnew, 2);

		/*cout << "boof " << endl;
		printVector(boof, 2);
		cout << endl;*/

		result = substractVector(resultprevious, boof, 2);

		cnt++;
	}

/*
	if (cntnumb != nullptr) { *cntnumb = cnt; }
	else {
		cout << endl;
		cout << "Количество итераций = " << cnt << endl;
		cout << endl;
	}*/
	return result;
}

double* gaussmethod(double** extMatrix, int n)
{
	double *solution;
	solution = new double[n];
	double maxvalue = 0;
	int imax;

	for (int cnt = 0; cnt < n; cnt++)
	{
		solution[cnt] = 0;
	}
	/*double det = determinant(extMatrix, n);
	if (abs(det) < 1e-30)
	{
	cout << "Определитель равен 0. Не существует единственного решения." << endl;
	solution = nullptr;
	}
	else
	{*/

	for (int i = 0; i < n - 1; i++)//цикл по строкам, которые вычитаются из нижележащих
	{
		//выбор макс элемента из i-го столбца
		maxvalue = 0;
		for (int il = i; il < n; il++)
		{
			if (maxvalue < abs(extMatrix[il][i]))
			{
				maxvalue = abs(extMatrix[il][i]);
				imax = il;
			}
		}

		if (maxvalue < 1e-10)
		{
			cout << "Не существует единственного решения." << endl;
			return nullptr;
		}

		if (imax != i)
		{
			double* buf = extMatrix[imax];
			extMatrix[imax] = extMatrix[i];
			extMatrix[i] = buf;
		}

		//extMatrix[i][n] = extMatrix[i][n] / extMatrix[i][i];
		double aii = extMatrix[i][i];

		if (abs(aii) < 1e-10)
		{
			cout << "Не существует единственного решения. Последняя строка диагонализированной матрицы - нулевая" << endl;
			return nullptr;
		}

		for (int j = i; j <= n; j++)//цикл по элементам строками, которая вычитается из нижележащих  от i+1???
		{
			extMatrix[i][j] = extMatrix[i][j] / aii;
		}

		for (int ii = i + 1; ii < n; ii++)//вычитание из низлежащих строк i-ой строки
		{
			double a_ii_i = extMatrix[ii][i];
			for (int jj = i; jj <= n; jj++)
			{
				extMatrix[ii][jj] -= a_ii_i * extMatrix[i][jj];
			}
		}
	}
	//нормируем нижнюю строку
	double	 aii = extMatrix[n - 1][n - 1];
	if (abs(aii) < 1e-10)
	{
		cout << "Не существует единственного решения. Последняя строка диагонализированной матрицы - нулевая" << endl;
		return nullptr;
	}
	for (int j = n - 1; j <= n; j++)//цикл по элементам строками, которая вычитается из нижележащих  от i+1???
	{
		extMatrix[n - 1][j] = extMatrix[n - 1][j] / aii;
	}
	//printMatrix(extMatrix, n, n + 1, true);
	//обратный ход

	double sum = 0;
	for (int i = n - 1; i >= 0; i--)
	{
		sum = 0;
		for (int j = i + 1; j < n; j++) //суммируем все более старшие переменные  взвешенные на коэффициенты текущей строки
		{
			sum += solution[j] * extMatrix[i][j];
		}
		solution[i] = extMatrix[i][n] - sum;//вычитаем из правой части 
	}

	//printMatrix(extMatrix, n);//печать диагонализированной (для проверки)
	return solution;
}

double system1function1(double x, double y)
{
	return (x*x - y * y - 15);
}

double system1function2(double x, double y)
{
	return (x*y + 4);
}

double functiontest1(double x, double y) 
{
	return y/x+2/(x*x);
}

double functiontest2(double x, double y)
{
	return 2*x;
}

double functionsystemtest1(double t, double* y)
{
	return y[1];
}

double functionsystemtest2(double t, double* y)
{
	return -20 / 0.3*y[0];
}

double gg1(double x, double y)
{
	double *input;
	input = new double[2];
	input[0] = x;
	input[1] = y;
	return	(x - u1n) / tau - funsysarray[0](x, input);
}

double gg2(double x, double y)
{
	double *input;
	input = new double[2];
	input[0] = x;
	input[1] = y;
	return	(y - u2n) / tau - funsysarray[1](x, input);
}

double gg3(double x, double y)
{
	double *input;
	input = new double[2];
	double *yn;
	yn = new double[2];

	yn[0] = u1n;
	yn[1] = u2n;
	input[0] = x;
	input[1] = y;
	return	(x - u1n) / tau - 0.5*(funsysarray[0](x, input)+funsysarray[0](u1n, yn)) ;
}

double gg4(double x, double y)
{
	double *input;
	input = new double[2];
	double *yn;
	yn = new double[2];

	yn[0] = u1n;
	yn[1] = u2n;
	input[0] = x;
	input[1] = y;
	return	(y - u2n) / tau - 0.5*(funsysarray[1](y, input) + funsysarray[1](u2n, yn));
}

double ExactSolution(double t)
{

	return cos(sqrt(1.0 / 0.3)*t);
}

double ExactDerivate(double t)
{

	return sqrt(1.0 / 0.3)*sin(sqrt(1.0 / 0.3)*t);
}

double ExactDerivate2(double t)
{

	return 1.0 / 0.3*cos(sqrt(1.0 / 0.3)*t);
}

double** EulerMethodObvious(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	double kol = (b - a) / h+1;
	double **grid;
	grid = new double*[kol];
	for (int ii = 0; ii<kol; ii++)
		grid[ii] = new double[n+1];


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j+1] = cond[j];
	}

	for (int i = 1; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;

		for (int j = 0; j < n; j++)
		{
			grid[i][j+1] = grid[i - 1][j+1] + h * ff[j](grid[i - 1][0], grid[i - 1]+1);
		}
	}

	return grid;
}

double** EulerMethodImplicit(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];
	for (int ii = 0; ii<kol; ii++)
		grid[ii] = new double[n + 1];


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	for (int i = 1; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;
	
		//решаем на каждой итерации систему нелинейных уравнений относительно вектор столбца (u1,n+1, u2,n+1)
		//присваиваем начальные значения для метода Ньютона, равные значениям вектора u на i-1-ом шаге 
		u1n = grid[i - 1][1];
		u2n = grid[i - 1][2];
		double *resulttest = NewtonMethodSystem(gg1,gg2, grid[i - 1] + 1/*вектор u на предыдущем шаге*/, 1e-3, nullptr, nullptr, nullptr, nullptr);
		grid[i][1] = resulttest[0];
		grid[i][2] = resulttest[1];

		/*for (int j = 0; j < n; j++)
		{
			
			
			grid[i][j + 1] = grid[i - 1][j + 1] + h * ff[j](grid[i - 1][0], grid[i - 1] + 1);
		}*/
	}

	return grid;
}

double** SymmetricScheme(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];
	for (int ii = 0; ii<kol; ii++)
		grid[ii] = new double[n + 1];


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	for (int i = 1; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;

		//решаем на каждой итерации систему нелинейных уравнений относительно вектор столбца (u1,n+1, u2,n+1)
		//для симметричной схемы нужно просто изменить запись уравнений правой части
		//присваиваем начальные значения для метода Ньютона, равные значениям вектора u на i-1-ом шаге 
		u1n = grid[i - 1][1];
		u2n = grid[i - 1][2];

		double *resulttest = NewtonMethodSystem(gg3, gg4, grid[i - 1] + 1/*вектор u на предыдущем шаге*/, 1e-3, nullptr, nullptr, nullptr, nullptr);

		grid[i][1] = resulttest[0];
		grid[i][2] = resulttest[1];

	}

	return grid;
}

double** RungeKutta21(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	int m = 2;//стадийность метода
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];

	double *kn;
	kn = new double[n];

	for (int ii = 0; ii<kol; ii++)
		grid[ii] = new double[n + 1];


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	for (int i = 1; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;

		grid[i][1] = grid[i - 1][1];//yn+1=yn
		grid[i][2] = grid[i - 1][2];//yn+1=yn

		kn[0] = h*ff[0](grid[i - 1][0], grid[i - 1] + 1)+ grid[i - 1][1];
		kn[1] = h*ff[1](grid[i - 1][0], grid[i - 1] + 1)+ grid[i - 1][2];

		grid[i][1] += h * 0.5*(ff[0](grid[i - 1][0], grid[i - 1] + 1) + ff[0](grid[i][0], kn));
		grid[i][2] += h * 0.5*(ff[1](grid[i - 1][1], grid[i - 1] + 1) + ff[1](grid[i][1], kn));
		
	}

	return grid;
}

double** RungeKutta22(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	int m = 2;//стадийность метода
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];

	double *boof;
	boof = new double[n];
	double *kn1;
	kn1 = new double[n];
	double *kn2;
	kn2 = new double[n];

	for (int ii = 0; ii<kol; ii++)
		grid[ii] = new double[n + 1];


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	for (int i = 1; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;

		grid[i][1] = grid[i - 1][1];//yn+1=yn
		grid[i][2] = grid[i - 1][2];//yn+1=yn

		kn1[0] = ff[0](grid[i - 1][0], grid[i - 1] + 1);
		kn1[1] = ff[1](grid[i - 1][1], grid[i - 1] + 1);

		boof[0] = grid[i - 1][1] + 0.5*h*kn1[0];
		boof[1] = grid[i - 1][2] + 0.5*h*kn1[1];

		kn2[0] = ff[0](grid[i - 1][0] + 0.5*h, boof);
		kn2[1] = ff[1](grid[i - 1][1] + 0.5*h, boof);

		grid[i][1] = grid[i - 1][1] + h * kn2[0];
		grid[i][2] = grid[i - 1][2] + h * kn2[1];

	}

	return grid;
}

double** RungeKutta4(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];
	double *kn;
	kn = new double[n];
	double *kn1;
	kn1 = new double[n];
	double *kn2;
	kn2 = new double[n];
	double *kn3;
	kn3 = new double[n];
	double *kn4;
	kn4 = new double[n];
	double *boof;
	boof = new double[n];

	for (int ii = 0; ii<kol; ii++)
		grid[ii] = new double[n + 1];


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	for (int i = 1; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;

		//grid[i][1] = grid[i - 1][1];//yn+1=yn
		//grid[i][2] = grid[i - 1][2];//yn+1=yn

		//kn1[0] = ff[0](grid[i - 1][0], grid[i - 1] + 1);
		//kn1[1] = ff[1](grid[i - 1][1], grid[i - 1] + 1);

		//boof[0] = grid[i - 1][1] + 0.5*h*kn1[0];
		//boof[1] = grid[i - 1][2] + 0.5*h*kn1[1];

		//kn2[0] = ff[0](grid[i - 1][0] + 0.5*h, boof);
		//kn2[1] = ff[1](grid[i - 1][1] + 0.5*h, boof);

		//boof[0] = grid[i - 1][1] + 0.5*h*kn2[0];
		//boof[1] = grid[i - 1][2] + 0.5*h*kn2[1];

		//kn3[0] = ff[0](grid[i - 1][0] + 0.5*h, boof);
		//kn3[1] = ff[1](grid[i - 1][1] + 0.5*h, boof);

		//boof[0] = grid[i - 1][1] + h*kn3[0];
		//boof[1] = grid[i - 1][2] + h*kn3[1];

		//kn4[0] = ff[0](grid[i][0], boof);
		//kn4[1] = ff[1](grid[i][1], boof);

		//kn[0] = (kn1[0] + 2 * kn2[0] + 2 * kn3[0] + kn4[0]) / 6;
		//kn[1] = (kn1[1] + 2 * kn2[1] + 2 * kn3[1] + kn4[1]) / 6;

		//grid[i][1] = grid[i - 1][1] + h * kn[0];
		//grid[i][2] = grid[i - 1][2] + h * kn[1];

		for (int j = 0; j < n; j++)
		{
			grid[i][j + 1] = grid[i - 1][j + 1];//yn+1=yn
		}
			grid[i][1] = grid[i - 1][1];//yn+1=yn
			grid[i][2] = grid[i - 1][2];//yn+1=yn

			for (int j = 0; j < n; j++)
			{
				kn1[j] = ff[j](grid[i - 1][0], grid[i - 1] + 1);
			}

			for (int j = 0; j < n; j++)
			{
				boof[j] = grid[i - 1][j + 1] + 0.5*h*kn1[j];
			}

			for (int j = 0; j < n; j++)
			{
				kn2[j] = ff[j](grid[i - 1][j] + 0.5*h, boof);
			}
		
			for (int j = 0; j < n; j++)
			{
				boof[j] = grid[i - 1][j + 1] + 0.5*h*kn2[j];
			}

			for (int j = 0; j < n; j++)
			{
				kn3[j] = ff[j](grid[i - 1][j] + 0.5*h, boof);
			}

			for (int j = 0; j < n; j++)
			{
				boof[j] = grid[i - 1][j + 1] + h * kn3[j];
			}

			for (int j = 0; j < n; j++)
			{
				kn4[j] = ff[j](grid[i][j], boof);
			}

			for (int j = 0; j < n; j++)
			{
				kn[j] = (kn1[j] + 2 * kn2[j] + 2 * kn3[j] + kn4[j]) / 6.0;
			}


			for (int j = 0; j < n; j++)
			{
				grid[i][j + 1] = grid[i - 1][j + 1] + h * kn[j];
			}		

	}

	return grid;
}

double** RungeKutta4AutomaticStep(int n, double a, double b, double h, double eps, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	double exactsolution;
	double delta;
	int i = 1;
	int itercnt = 0;
	double error = 1e+6;
	double tau;
	double kol = 4*((b - a) / h + 1);
	if (kol<10000) 	{ kol = 10000; }

	double **grid;
	grid = new double*[kol];
	double **gridtau;
	gridtau = new double*[kol];

	double *kn;
	kn = new double[n];
	double *kn1;
	kn1 = new double[n];
	double *kn2;
	kn2 = new double[n];
	double *kn3;
	kn3 = new double[n];
	double *kn4;
	kn4 = new double[n];
	double *boof;
	boof = new double[n];

	double *ytau2;
	ytau2 = new double[n];
	double *ytau;
	ytau = new double[n];
	double *ytaudelta;
	ytaudelta = new double[n];

	for (int ii = 0; ii < kol; ii++)
	{
		grid[ii] = new double[n + 1];
		gridtau[ii] = new double[n + 1];
	}


	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	while( i<kol-0.1 && grid[i - 1][0] < b )//номера точек
	{
	
		if (grid[i - 1][0] > b)
		{ grid[i - 1][0] = b;	}
		else { grid[i][0] = grid[i - 1][0] + h; }

		tau = grid[i][0] - grid[i - 1][0];

		for (int j = 0; j < n; j++)
		{
			grid[i][j + 1] = grid[i - 1][j + 1];//yn+1=yn
		}
		grid[i][1] = grid[i - 1][1];//yn+1=yn
		grid[i][2] = grid[i - 1][2];//yn+1=yn

		for (int j = 0; j < n; j++)
		{
			kn1[j] = ff[j](grid[i - 1][0], grid[i - 1] + 1);
		}

		for (int j = 0; j < n; j++)
		{
			boof[j] = grid[i - 1][j + 1] + 0.5*h*kn1[j];
		}

		for (int j = 0; j < n; j++)
		{
			kn2[j] = ff[j](grid[i - 1][j] + 0.5*h, boof);
		}

		for (int j = 0; j < n; j++)
		{
			boof[j] = grid[i - 1][j + 1] + 0.5*h*kn2[j];
		}

		for (int j = 0; j < n; j++)
		{
			kn3[j] = ff[j](grid[i - 1][j] + 0.5*h, boof);
		}

		for (int j = 0; j < n; j++)
		{
			boof[j] = grid[i - 1][j + 1] + h * kn3[j];
		}

		for (int j = 0; j < n; j++)
		{
			kn4[j] = ff[j](grid[i][j], boof);
		}

		for (int j = 0; j < n; j++)
		{
			kn[j] = (kn1[j] + 2 * kn2[j] + 2 * kn3[j] + kn4[j]) / 6.0;
		}


		for (int j = 0; j < n; j++)
		{
			grid[i][j + 1] = grid[i - 1][j + 1] + h * kn[j];
		}

		ytau[0] = grid[i][1];//вектор решения в момент tn+1
		ytau[1] = grid[i][2];

		//проверка отклонения и возможное уточнение величины шага 

		error = 1e+6;

		while (error > eps)
		{
			tau = tau / 2;
			//нахождение решения на шаге номер 2 (точка tn + 2tau)
			
			gridtau = RungeKutta4(n, grid[i-1][0], grid[i - 1][0] + 2*tau, tau, grid[i - 1] + 1, ff);

			//нахождение отклонения error  + запомнить текущую точку для дальнейшего расчета и решение для нее
            

			ytau2[0] = gridtau[2][1];//вектор решения на двух шагах tau2
			ytau2[1] = gridtau[2][2];
			
			ytaudelta = substractVector(ytau2, ytau, 2);
			ytaudelta = multiplyVectorNumber(ytaudelta, 1/15.0, 2);
			error = normVectorInfinity(ytaudelta, 2);
			
			ytau[0] = gridtau[1][1];//средняя точка --- новое предполагаемое решение
			ytau[1] = gridtau[1][2];
				
		}

		grid[i][0] = grid[i - 1][0] + 2 * tau;

		grid[i][1] = ytau2[0];
		grid[i][2] = ytau2[1];

		realkol = i+1;
		i++;
	}

	exactsolution = ExactSolution(tau);
	delta = grid[i][1] - exactsolution;

	//cout << delta;


	return grid;
}

double** AdamsBashforth(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff) 
{
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];
	double **test;
	//test = new double*[2];
	double *fn;
	fn = new double[n];
	double *fn1;
	fn1 = new double[n];
	double *fn2;
	fn2 = new double[n];
	double *fn3;
	fn3 = new double[n];
	double *fn4;
	fn4 = new double[n];
	double *boof;
	boof = new double[n];

	for (int ii = 0; ii < kol; ii++)
	{
		grid[ii] = new double[n + 1];
		//test[ii] = new double[n + 1];
	}

	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	//первые 4 точки с помощью метода Рунге-Кутты 4-го порядка точности
	test = RungeKutta4(2, a, b, h, cond, funsysarray);//получили 4 значения y  в точках 1 2 3 4
	
	/*test[0][0] = 0;
	test[0][1] = -1;
	test[0][2] = 0;

	test[1][0] = test[0][0]+h;
	test[1][1] = -cos(sqrt(1.0/0.3)*test[1][0]);
	test[1][2] = sqrt(1.0 / 0.3)*sin(sqrt(1.0 / 0.3)*test[1][0]);

	test[2][0] = test[1][0]+h;
	test[2][1] = -cos(sqrt(1.0 / 0.3)*test[2][0]);
	test[2][2] = sqrt(1.0 / 0.3)*sin(sqrt(1.0 / 0.3)*test[2][0]);

	test[3][0] = test[2][0]+h;
	test[3][1] = -cos(sqrt(1.0 / 0.3)*test[3][0]);
	test[3][2] = sqrt(1.0 / 0.3)*sin(sqrt(1.0 / 0.3)*test[3][0]);*/

	/*printMatrix(test, kol, 3, false);
	cout << endl;*/

	//подставили y в аргумент f и получили 4 значения f1 f2 f3 f4
	//индекс = сдвигу назад, заполняем массив грид в обратном порядке
	fn[0] = ff[0](test[3][0], test[3] + 1);
	fn[1] = ff[1](test[3][0], test[3] + 1);

	fn1[0] = ff[0](test[2][0], test[2] + 1);
	fn1[1] = ff[1](test[2][0], test[2] + 1);

	fn2[0] = ff[0](test[1][0], test[1] + 1);
	fn2[1] = ff[1](test[1][0], test[1] + 1);

	fn3[0] = ff[0](test[0][0], test[0] + 1);
	fn3[1] = ff[1](test[0][0], test[0] + 1);

	//перенос из массива test в массив grid
	grid[0][0] = test[0][0];//время
	grid[0][1] = test[0][1];//1-ая координата
	grid[0][2] = test[0][2];//2-ая координата

	grid[1][0] = test[1][0];
	grid[1][1] = test[1][1];
	grid[1][2] = test[1][2];

	grid[2][0] = test[2][0];
	grid[2][1] = test[2][1];
	grid[2][2] = test[2][2];

	grid[3][0] = test[3][0];
	grid[3][1] = test[3][1];
	grid[3][2] = test[3][2];


	for (int i = 4; i < kol; i++)//номера точек
	{
		grid[i][0] = a + (double)i * h;//абсцисса i-ой точки

		//первые три шага с помощью метода Рунге-Кутты 4 порядка, далее по методу Адамса-Башфорта 4 порядка

		//высчитываем пятую точку по четырем предыдущим 
		grid[i][1] = grid[i - 1][1] + h * (55.0 * fn[0] - 59.0 * fn1[0] + 37.0 * fn2[0] - 9.0* fn3[0]) / 24.0;//вычислил 5-ую точку (i=4) первый прогон цикла
		grid[i][2] = grid[i - 1][2] + h * (55.0 * fn[1] - 59.0 * fn1[1] + 37.0 * fn2[1] - 9.0* fn3[1]) / 24.0;

		//готовим fn для следующей точки ( 6-ая при первой прогонке)
		//сдвигаем fn на единицу назад

		fn3[0] = fn2[0];
		fn3[1] = fn2[1];

		fn2[0] = fn1[0];
		fn2[1] = fn1[1];

		fn1[0] = fn[0];
		fn1[1] = fn[1];

		//досчитываем 4-ую точку (текущую - только что подсчитанную)
		fn[0] = ff[0](grid[i][0], grid[i] + 1);
		fn[1] = ff[1](grid[i][0], grid[i] + 1);
	}

	return grid;
}

double** ForecastCorrection(int n, double a, double b, double h, double* cond, std::vector<std::function<double(double, double*)> > ff)
{
	double kol = (b - a) / h + 1;
	double **grid;
	grid = new double*[kol];
	double **test;
	//test = new double*[2];
	double *fn;
	fn = new double[n];
	double *fn1;
	fn1 = new double[n];
	double *fn2;
	fn2 = new double[n];
	double *fn3;
	fn3 = new double[n];
	double *fn4;
	fn4 = new double[n];
	double *fn0;
	fn0 = new double[n];
	double *boof;
	boof = new double[n];

	for (int ii = 0; ii < kol; ii++)
	{
		grid[ii] = new double[n + 1];
		//test[ii] = new double[n + 1];
	}

	for (int j = 0; j < n; j++)//номера функций в вектор функции (н.у.)
	{
		grid[0][0] = a;
		grid[0][j + 1] = cond[j];
	}

	//первые 4 точки с помощью метода Рунге-Кутты 4-го порядка точности
	test = RungeKutta4(2, 0, 3 * h, h, cond, funsysarray);//получили 4 значения y  в точках 1 2 3 4

	 /*printMatrix(test, kol, 3, false);
	 cout << endl;*/

	 //подставили y в аргумент f и получили 4 значения f1 f2 f3 f4
	//индекс = сдвигу назад, заполняем массив грид в обратном порядке
	fn[0] = ff[0](test[3][0], test[3] + 1);
	fn[1] = ff[1](test[3][1], test[3] + 1);

	fn1[0] = ff[0](test[2][0], test[2] + 1);
	fn1[1] = ff[1](test[2][1], test[2] + 1);

	fn2[0] = ff[0](test[1][0], test[1] + 1);
	fn2[1] = ff[1](test[1][1], test[1] + 1);

	fn3[0] = ff[0](test[0][0], test[0] + 1);
	fn3[1] = ff[1](test[0][1], test[0] + 1);

	//перенос из массива test в массив grid
	grid[0][0] = test[0][0];//время
	grid[0][1] = test[0][1];//1-ая координата
	grid[0][2] = test[0][2];//2-ая координата

	grid[1][0] = test[1][0];
	grid[1][1] = test[1][1];
	grid[1][2] = test[1][2];

	grid[2][0] = test[2][0];
	grid[2][1] = test[2][1];
	grid[2][2] = test[2][2];

	grid[3][0] = test[3][0];
	grid[3][1] = test[3][1];
	grid[3][2] = test[3][2];


	for (int i = 4; i < kol; i++)//номера точек
	{
		grid[i][0] = a + i * h;//абсцисса i-ой точки

	//первые три шага с помощью метода Рунге-Кутты 4 порядка, далее по методу Адамса-Башфорта 4 порядка

	//высчитываем пятую точку по четырем предыдущим 
		grid[i][1] = grid[i - 1][1] + h * (55.0 * fn[0] - 59.0 * fn1[0] + 37.0 * fn2[0] - 9.0* fn3[0]) / 24.0;//вычислил 5-ую точку (i=4) первый прогон цикла
		grid[i][2] = grid[i - 1][2] + h * (55.0 * fn[1] - 59.0 * fn1[1] + 37.0 * fn2[1] - 9.0* fn3[1]) / 24.0;

		fn0[0] = ff[0](grid[i][1], grid[i] + 1);
		fn0[1] = ff[1](grid[i][1], grid[i] + 1);

		grid[i][1] = grid[i - 1][1] + h * (9.0 * fn0[0] + 19.0 * fn[0] - 5.0 * fn1[0] + fn2[0]) / 24.0;//вычислил 5-ую точку (i=4) первый прогон цикла
		grid[i][2] = grid[i - 1][2] + h * (9.0 * fn0[1] + 19.0 * fn[1] - 5.0 * fn1[1] + fn2[1]) / 24.0;


		//готовим fn для следующей точки ( 6-ая при первой прогонке)
		//сдвигаем fn на единицу назад

		fn3[0] = fn2[0];
		fn3[1] = fn2[1];

		fn2[0] = fn1[0];
		fn2[1] = fn1[1];

		fn1[0] = fn[0];
		fn1[1] = fn[1];

		//досчитываем 4-ую точку (текущую - только что подсчитанную)
		fn[0] = ff[0](grid[i][0], grid[i] + 1);
		fn[1] = ff[1](grid[i][0], grid[i] + 1);

	}

	return grid;
}

double ErrorCalculation(double** grid, double(*ff) (double), double a, double b, double h)
{
	double result = 0;
	double t = a;
	double ExactValue;
	double kol = (b - a) / h + 1;

	for (int i = 0; i < kol; i++)
	{
		ExactValue = ExactSolution(t);

		if (abs(ExactValue - grid[i][1]) > result) { result = abs(ExactValue - grid[i][1]); }

		//cout << "exact = " << ExactValue << "  " << "solution = " << grid[i][1] << endl;

        t +=  h;		
    }

	return result;
}


int main()
{
	setlocale(LC_ALL, "rus");
	//тест явного метода Эйлера
	double eps = 1e-3;
	int k = 2;//размерность
	double xi, yi;
	double error;
	double a = 0;
	double b = 3.5;
	double h = 0.0000001;
	double kol = (b - a) / h + 1;
	tau = h;

	////задача с грузиком
	funsysarray = {
		[](double t, double* u) { return  u[1]; },
		[](double t, double* u) { return  -1.0 / 0.3*u[0]; }
	};

	//система №1
	/*a = 0;
	b = 1;
	h = 0.01;
	kol = (b - a) / h + 1;

	funsysarray = {
	[](double t, double* u) { return  2*u[0]+ u[1]* u[1]-1; },
	[](double t, double* u) { return  6 * u[0] - u[1] * u[1] + 1; }
	};*/

	//система №2
	/*a = 0;
	b = 0.5;
	h = 0.01;
	kol = (b - a) / h + 1;
	funsysarray = {
	[](double t, double* u) { return  1-u[0]*u[0]-u[1]*u[1]; },
	[](double t, double* u) { return  2.0*u[0]; }
	};
*/
	//система №3
	/*k = 2;
	funsysarray = {
	[](double t, double* u) { return  10.0*(u[1]-u[0]); },
	[](double t, double* u) { return  u[0]*(28.0 - u[2]) - u[1]; },
	[](double t, double* u) { return  u[0]*u[1]-8.0/3.0*u[2]; }
	};*/

	//уравнение нелинейного осциллятора
	/*funsysarray = {
	[](double t, double* u) { return  u[1]; },
	[](double t, double* u) { return  -0.4*u[1]-u[0]+0.061*u[0]* u[0]* u[0]-0.115*cos(0.5904*t); }
	};*/

	double **test;
	test = new double*[k];

	cout << "Тест (задача с шариком на пружине):" << endl;
	//значение t=a, массив начальных значений - нулевая производная, первая и тд

    //test = EulerMethodObvious(2, a, b, h, new double[2] { 1, 0}, funsysarray);
	//test = EulerMethodImplicit(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);
	//test = SymmetricScheme(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);
	//test = RungeKutta21(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);
	//test = RungeKutta22(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);
	test = RungeKutta4(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);
	//test = AdamsBashforth(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);
	//test = ForecastCorrection(2, a, b, h, new double[2]{ 1, 0 }, funsysarray);


	//трехмерная задача
	//test = RungeKutta4(3, a, b, h, new double[3]{ 1, 2, 3 }, funsysarray);


	// для всех методов найдем оптимальную величину шага по времени, обеспечиващую различную точность 1e-3
	double erroreps = 1e-2;
	double optimaltau = 1e-5;
	//автоматический шаг, правило Рунге, запись шага в файл, вывод графиков шага и зависимости решения от шага

	//test = RungeKutta4AutomaticStep(2, a, b, 0.5, 1e-8, new double[2]{ 1, 0 }, funsysarray);
	kol = realkol;

	printMatrix(test, kol, k+1, false);
	cout << endl;

	////запись величины шагов в файл
	//ofstream fout_autostep;
	//fout_autostep.open("autostep.txt"); //связываем объект с файлом

	//for (int i = 1; i < realkol; i++)
	//{
	//	fout_autostep << test[i][0] - test[i - 1][0] << endl;
	//}
	//
	//fout_autostep.close(); // закрываем файл

	//for (int i = 1; i < realkol; i++)
	//{
	//	cout << test[i][0] - test[i - 1][0] << endl;
	//}

	ofstream fout_coordinates;
	fout_coordinates.open("testnew.txt"); //связываем объект с файлом

	for (int i = 0; i < kol; i++)
	{
	
		for (int j = 0; j < k+1; j++)
		{
			//записываем в файл абсциссы и ординаты каждой итерации (за время отвечает  int j = !0!) 
			fout_coordinates << test[i][j] << " "; //запись строки в файл 
		}
		fout_coordinates << endl;
	}
	fout_coordinates.close(); // закрываем файл


	ofstream fout_delta;
	fout_delta.open("delta.txt"); //связываем объект с файлом

	for (int i = 0; i < kol; i++)
	{
		fout_delta <<  abs(test[i][1] - ExactSolution(test[i][0])) << endl;
	}

	fout_delta.close(); // закрываем файл

	//создаем массив из точных решений с автошагами
	/*double* exactsolution;
	for (int i = 0; i < kol; i++)
	{
		delta[i] = ExactSolution(test[i][0]);
		cout << delta[i] << endl;*/
	

	

	//ищем погрешности методов
	/*error = ErrorCalculation(test, ExactSolution, a, b, h);
	cout << "Погрешность метода по максимальной норме в узлах = " << error << endl;
	cout << endl;*/


	matrix_destroyer(test, k+1);

	system("pause");
    return 0;
}

