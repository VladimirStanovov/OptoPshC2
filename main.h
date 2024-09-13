#include <math.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <iomanip>
#include <random>
#include <mpi.h>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <regex>
#include "cec17_test_func.cpp"

using namespace std;
/*typedef chrono::high_resolution_clock myclock;
myclock::time_point beginning = myclock::now();
myclock::duration d1 = myclock::now() - beginning;
#ifdef __linux__
    unsigned globalseed = d1.count();
#elif _WIN32
    unsigned globalseed = unsigned(time(NULL));
#else

#endif*/
unsigned globalseed = 2041;

mt19937 generator_uni_i(0);
mt19937 generator_uni_r(0);
mt19937 generator_norm(0);
mt19937 generator_cauchy(0);
uniform_int_distribution<int> uni_int(0,1048576);
uniform_real_distribution<double> uni_real(0.0,1.0);
normal_distribution<double> norm_dist(0.0,1.0);
cauchy_distribution<double> cachy_dist(0.0,1.0);

int IntRandom(int target){if(target == 0) return 0; return uni_int(generator_uni_i)%target;}
double Random(double minimal, double maximal){return uni_real(generator_uni_r)*(maximal-minimal)+minimal;}
double NormRand(double mu, double sigma){return norm_dist(generator_norm)*sigma+mu;}
double CauchyRand(double mu, double sigma){return cachy_dist(generator_cauchy)*sigma+mu;}


mt19937 generator_uni_i2(0);
mt19937 generator_uni_r2(0);
mt19937 generator_norm2(0);
mt19937 generator_cauchy2(0);
uniform_int_distribution<int> uni_int2(0,1048576);
uniform_real_distribution<double> uni_real2(0.0,1.0);
normal_distribution<double> norm_dist2(0.0,1.0);
cauchy_distribution<double> cachy_dist2(0.0,1.0);

int IntRandom2(int target){if(target == 0) return 0; return uni_int2(generator_uni_i2)%target;}
double Random2(double minimal, double maximal){return uni_real2(generator_uni_r2)*(maximal-minimal)+minimal;}
double NormRand2(double mu, double sigma){return norm_dist2(generator_norm2)*sigma+mu;}
double CauchyRand2(double mu, double sigma){return cachy_dist2(generator_cauchy2)*sigma+mu;}

void qSort1(double* Mass, const int low, const int high)
{
    int i=low;
    int j=high;
    double x=Mass[(low+high)>>1];
    do
    {
        while(Mass[i]<x)    ++i;
        while(Mass[j]>x)    --j;
        if(i<=j)
        {
            double temp=Mass[i];
            Mass[i]=Mass[j];
            Mass[j]=temp;
            i++;    j--;
        }
    } while(i<=j);
    if(low<j)   qSort1(Mass,low,j);
    if(i<high)  qSort1(Mass,i,high);
}
void qSort2int(double* Mass, int* Mass2, const int low, const int high)
{
    int i=low;
    int j=high;
    double x=Mass[(low+high)>>1];
    do
    {
        while(Mass[i]<x)    ++i;
        while(Mass[j]>x)    --j;
        if(i<=j)
        {
            double temp=Mass[i];
            Mass[i]=Mass[j];
            Mass[j]=temp;
            int temp2=Mass2[i];
            Mass2[i]=Mass2[j];
            Mass2[j]=temp2;
            i++;    j--;
        }
    } while(i<=j);
    if(low<j)   qSort2int(Mass,Mass2,low,j);
    if(i<high)  qSort2int(Mass,Mass2,i,high);
}
const string currentDateTime() {
    time_t     now = time(NULL);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
const vector<string> explode(const string& s, const char& c, int& finsize)
{
	string buff{""};
	vector<string> v;
	for(auto n:s)
	{
		if(n != c) buff+=n;
		else if(n == c && buff != "") { v.push_back(buff); buff = ""; ++finsize;}
	}
	if(buff != "") { v.push_back(buff); ++finsize; }
	return v;
}
template<typename K, typename V>
unordered_map<V, K> inverse_map(unordered_map<K, V> &map)
{
    unordered_map<V, K> inv;
    for_each(map.begin(), map.end(),
                [&inv] (const pair<K, V> &p) {
                    inv.insert(make_pair(p.second, p.first));
                });
    return inv;
}
bool loadFile(const string & p_name, string & p_content)
{
    fstream file(p_name.c_str(), fstream::in) ;
    if(file.is_open())
    {
        getline(file, p_content, '\0') ;
        return ! file.bad() ;
    }
    return false ;
}
void replace_all(string& s, string const& toReplace, string const& replaceWith)
{
    ostringstream oss;
    size_t pos = 0;
    size_t prevPos = pos;

    while (true)
    {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == string::npos)
            break;
        oss << s.substr(prevPos, pos - prevPos);
        oss << replaceWith;
        pos += toReplace.size();
    }

    oss << s.substr(prevPos);
    s = oss.str();
}
void trim(string& s)
{
    while(true)
    {
        if(s[0] == ' ')
            s = s.substr(1);
        else
            break;
    }
    while(true)
    {
        if(s[s.length()-1] == ' ')
            s = s.substr(0,s.length()-1);
        else
            break;
    }
}
bool isNumeric(string input) {
    replace_all(input,"-","");
    for (char c : input) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}
struct atomInst
{
    int inst;
    int i;
    double f;
    string n;
    vector<double> v;
};
string printSubProg(vector<atomInst> subProg)
{
    char buffer[100];
    string res = "";
    int size1 = subProg.size();
    for(int i=0;i!=size1;i++)
    {
        sprintf(buffer,"%d\t",subProg[i].inst);
        res += buffer;
    }
    res += "\n";
    for(int i=0;i!=size1;i++)
    {
        sprintf(buffer,"%d\t",subProg[i].i);
        res += buffer;
    }
    res += "\n";
    for(int i=0;i!=size1;i++)
    {
        if(fabs(subProg[i].f) < 100000 && fabs(subProg[i].f) > 1e-5)
            sprintf(buffer,"%.6f\t",subProg[i].f);
        else
            sprintf(buffer,"%.6e\t",subProg[i].f);
        res += buffer;
    }
    res += "\n";
    for(int i=0;i!=size1;i++)
    {
        res += "'";
        res += subProg[i].n;
        res += "'";
        res += "\t";
    }
    res += "\n";
    return res;
}
void get_fract_ranks(double* Vals, double* Ranks, int n)
{
    int* indexes = new int[n];
    double* TVals = new double[n];
    double* TRanks = new double[n];
    for(int i=0;i!=n;i++)
    {
        indexes[i] = i;
        TRanks[i] = i;
        TVals[i] = Vals[i];
    }
    qSort2int(TVals,indexes,0,n-1);
    int i = 0;
    //int counter1 = 0;
    while(i<n)
    {
        int neq = 0;
        double sranks = 0;
        for(int j=i;j<n;j++)
        {
            if(TVals[i] == TVals[j] || i == j)
            {
                neq++;
                sranks += TRanks[j];
            }
            else
                break;
        }
        for(int j=i;j!=i+neq;j++)
            TRanks[j] = sranks/double(neq);
        i += neq;
        /*counter1++;
        if(counter1 > 10000)
        {
            for(int k=0;k!=n;k++)
            {
                cout<<Vals[k]<<"\t";
            }
            cout<<endl;
            cin>>counter1;
        }*/
    }
    for(int k=0;k!=n;k++)
        Ranks[indexes[k]] = TRanks[k];
    delete indexes;
    delete TVals;
    delete TRanks;
}
class Vector;

class Matrix {

public:
  // default constructor (don't allocate)
  Matrix() : m(0), n(0), data(nullptr) {}

  // constructor with memory allocation, initialized to zero
  Matrix(int m_, int n_) : Matrix() {
    m = m_;
    n = n_;
    allocate(m_,n_);
  }

  Matrix(double** in2, int m_, int n_) : Matrix() {
    m = m_;
    n = n_;
    allocate(m_,n_);
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
	(*this)(i,j) = in2[i][j];
  }

  // copy constructor
  Matrix(const Matrix& mat) : Matrix(mat.m,mat.n) {

    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
	(*this)(i,j) = mat(i,j);
  }

  // constructor from array
  template<int rows, int cols>
  Matrix(double (&a)[rows][cols]) : Matrix(rows,cols) {

    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
	(*this)(i,j) = a[i][j];
  }

  // destructor
  ~Matrix() {
    deallocate();
  }


  // access data operators
  double& operator() (int i, int j) {
    return data[i+m*j]; }
  double  operator() (int i, int j) const {
    return data[i+m*j]; }

  // operator assignment
  Matrix& operator=(const Matrix& source) {

    // self-assignment check
    if (this != &source) {
      if ( (m*n) != (source.m * source.n) ) { // storage cannot be reused
	allocate(source.m,source.n);          // re-allocate storage
      }
      // storage can be used, copy data
      std::copy(source.data, source.data + source.m*source.n, data);
    }
    return *this;
  }

  // compute minor
  void compute_minor(const Matrix& mat, int d) {

    allocate(mat.m, mat.n);

    for (int i = 0; i < d; i++)
      (*this)(i,i) = 1.0;
    for (int i = d; i < mat.m; i++)
      for (int j = d; j < mat.n; j++)
	(*this)(i,j) = mat(i,j);

  }

  // Matrix multiplication
  // c = a * b
  // c will be re-allocated here
  void mult(const Matrix& a, const Matrix& b) {

    if (a.n != b.m) {
      std::cerr << "Matrix multiplication not possible, sizes don't match !\n";
      return;
    }

    // reallocate ourself if necessary i.e. current Matrix has not valid sizes
    if (a.m != m or b.n != n)
      allocate(a.m, b.n);

    memset(data,0,m*n*sizeof(double));

    for (int i = 0; i < a.m; i++)
      for (int j = 0; j < b.n; j++)
	for (int k = 0; k < a.n; k++)
	  (*this)(i,j) += a(i,k) * b(k,j);

  }

  void transpose() {
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < i; j++) {
	double t = (*this)(i,j);
	(*this)(i,j) = (*this)(j,i);
	(*this)(j,i) = t;
      }
    }
  }

  // take c-th column of m, put in v
  void extract_column(Vector& v, int c);

  // memory allocation
  void allocate(int m_, int n_) {

    // if already allocated, memory is freed
    deallocate();

    // new sizes
    m = m_;
    n = n_;

    data = new double[m_*n_];
    memset(data,0,m_*n_*sizeof(double));

  } // allocate

  // memory free
  void deallocate() {

    if (data)
      delete[] data;

    data = nullptr;

  }

  int m, n;

private:
  double* data;

}; // struct Matrix

// column vector
class Vector {

public:
  // default constructor (don't allocate)
  Vector() : size(0), data(nullptr) {}

  // constructor with memory allocation, initialized to zero
  Vector(int size_) : Vector() {
    size = size_;
    allocate(size_);
  }

  // destructor
  ~Vector() {
    deallocate();
  }

  // access data operators
  double& operator() (int i) {
    return data[i]; }
  double  operator() (int i) const {
    return data[i]; }

  // operator assignment
  Vector& operator=(const Vector& source) {

    // self-assignment check
    if (this != &source) {
      if ( size != (source.size) ) {   // storage cannot be reused
	allocate(source.size);         // re-allocate storage
      }
      // storage can be used, copy data
      std::copy(source.data, source.data + source.size, data);
    }
    return *this;
  }

  // memory allocation
  void allocate(int size_) {

    deallocate();

    // new sizes
    size = size_;

    data = new double[size_];
    memset(data,0,size_*sizeof(double));

  } // allocate

  // memory free
  void deallocate() {

    if (data)
      delete[] data;

    data = nullptr;

  }

  //   ||x||
  double norm() {
    double sum = 0;
    for (int i = 0; i < size; i++) sum += (*this)(i) * (*this)(i);
    return sqrt(sum);
  }

  // divide data by factor
  void rescale(double factor) {
    for (int i = 0; i < size; i++) (*this)(i) /= factor;
  }

  void rescale_unit() {
    double factor = norm();
    rescale(factor);
  }

  int size;

private:
  double* data;

}; // class Vector

// c = a + b * s
void vmadd(const Vector& a, const Vector& b, double s, Vector& c)
{
  if (c.size != a.size or c.size != b.size) {
    std::cerr << "[vmadd]: vector sizes don't match\n";
    return;
  }

  for (int i = 0; i < c.size; i++)
    c(i) = a(i) + s * b(i);
}

// mat = I - 2*v*v^T
// !!! m is allocated here !!!
void compute_householder_factor(Matrix& mat, const Vector& v)
{

  int n = v.size;
  mat.allocate(n,n);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      mat(i,j) = -2 *  v(i) * v(j);
  for (int i = 0; i < n; i++)
    mat(i,i) += 1;
}

// take c-th column of a matrix, put results in Vector v
void Matrix::extract_column(Vector& v, int c) {
  if (m != v.size) {
    std::cerr << "[Matrix::extract_column]: Matrix and Vector sizes don't match\n";
    return;
  }

  for (int i = 0; i < m; i++)
    v(i) = (*this)(i,c);
}

void matrix_show(const Matrix&  m, const std::string& str="")
{
  std::cout << str << "\n";
  for(int i = 0; i < m.m; i++) {
    for (int j = 0; j < m.n; j++) {
      printf(" %8.3f", m(i,j));
    }
    printf("\n");
  }
  printf("\n");
}

// L2-norm ||A-B||^2
double matrix_compare(const Matrix& A, const Matrix& B) {
  // matrices must have same size
  if (A.m != B.m or  A.n != B.n)
    return std::numeric_limits<double>::max();

  double res=0;
  for(int i = 0; i < A.m; i++) {
    for (int j = 0; j < A.n; j++) {
      res += (A(i,j)-B(i,j)) * (A(i,j)-B(i,j));
    }
  }

  res /= A.m*A.n;
  return res;
}

void householder(Matrix& mat,
		 Matrix& R,
		 Matrix& Q)
{

  int m = mat.m;
  int n = mat.n;

  // array of factor Q1, Q2, ... Qm
  std::vector<Matrix> qv(m);

  // temp array
  Matrix z0(mat);
  Matrix z1;

  for (int k = 0; k < n && k < m - 1; k++) {

    Vector e(m), x(m);
    double a;

    // compute minor
    z1.compute_minor(z0, k);

    // extract k-th column into x
    z1.extract_column(x, k);

    a = x.norm();
    if (mat(k,k) > 0) a = -a;

    for (int i = 0; i < e.size; i++)
      e(i) = (i == k) ? 1 : 0;

    // e = x + a*e
    vmadd(x, e, a, e);

    // e = e / ||e||
    e.rescale_unit();

    // qv[k] = I - 2 *e*e^T
    compute_householder_factor(qv[k], e);

    // z = qv[k] * z1
    z0.mult(qv[k], z1);

  }

  Q = qv[0];

  // after this loop, we will obtain Q (up to a transpose operation)
  for (int i = 1; i < n && i < m - 1; i++) {

    z1.mult(qv[i], Q);
    Q = z1;

  }

  R.mult(Q, mat);
  Q.transpose();
}

const int Benchmark = 17;
double *OShift,*M,*y,*z,*x_bound;
int ini_flag=0,n_flag,func_flag,*SS;
double tempF[1];
double fbest;

double* CEC_Shifts;
double* CEC_Streches;
double* CEC_Left;
double* CEC_Right;
double* tempVector;

int NFE;
int NFEmax;
int func_num;

void GenerateProblem17(const int DIM)
{
    //cout<<"Generating CEC17 instance..."<<endl;
    //unsigned t0=clock(),t1;

    //if(Benchmark == 17)
    {
        double** tempRandMatrix = new double*[DIM];
        for(int j=0;j!=DIM;j++)
        {
            tempRandMatrix[j] = new double[DIM];
        }
        int* randshuffle = new int[DIM];

        for(int func=0;func!=30;func++)
        {
            int maxtest = 1;
            if(func > 19)
            {
                maxtest = 10;
            }
            for(int test=0;test!=maxtest;test++)
            {
                for(int j=0;j!=DIM;j++)
                {
                    for(int k=0;k!=DIM;k++)
                        tempRandMatrix[j][k] = Random(0,1);
                }
                Matrix A(tempRandMatrix,DIM,DIM);
                Matrix Q, R;
                householder(A, R, Q);
                for(int j=0;j!=DIM;j++)
                {
                    for(int k=0;k!=DIM;k++)
                        RandRotMatrix_17[func][test][j][k] = Q(j,k);
                }
                for(int j=0;j!=DIM;j++)
                {
                    randshuffle[j] = j+1;
                }
                for(int j=0;j!=DIM*10;j++)
                {
                    swap(randshuffle[IntRandom(DIM)],randshuffle[IntRandom(DIM)]);
                }
                for(int j=0;j!=DIM;j++)
                {
                    RandShuffleMatrix_17[func][test][j] = randshuffle[j];
                }
            }
            for(int test=0;test!=maxtest;test++)
            {
                for(int j=0;j!=DIM;j++)
                {
                    RandShiftMatrix_17[func][test][j] = Random(-80.0,80.0);
                }
            }
        }
        for(int j=0;j!=DIM;j++)
        {
            delete tempRandMatrix[j];
        }
        delete tempRandMatrix;
        delete randshuffle;
    }
    //t1=clock()-t0;
    //double T0 = t1/double(CLOCKS_PER_SEC);
    //cout << "Time spent: " << T0 << endl;
    ini_flag = 0;
}
double CEC17(vector<double> HostVector)
{
    for(int i=0;i!=GNVars;i++)
    {
        tempVector[i] = HostVector[i];
    }
    cec17_test_func(tempVector,tempF,GNVars,1,func_num);
    NFE++;
    if(NFE <= NFEmax)
        fbest = tempF[0];//min(fbest,tempF[0]);
    return tempF[0];
}
double CEC17(double* HostVector)
{
    for(int i=0;i!=GNVars;i++)
    {
        tempVector[i] = HostVector[i];
    }
    cec17_test_func(tempVector,tempF,GNVars,1,func_num);
    NFE++;
    if(NFE <= NFEmax)
        fbest = tempF[0];//min(fbest,tempF[0]);
    return tempF[0];
}
void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}
string readFile2(const string &fileName)
{
    ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);

    ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, ios::beg);

    vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return string(bytes.data(), fileSize);
}
