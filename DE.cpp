
double globalbest;
bool globalbestinit;
double fopt;
int LastFEcount;
int NFEval_DE = 0;
int MaxFEval_DE = 0;
int SortCrossRates = 0;
double global_MWLp1 = 2;
double global_MWLp2 = 2;
double global_MWLm = 1;
const int ResTsize1 = 29; // number of functions
const int ResTsize2 = 11; // number of records per function

int stepsFEval[ResTsize2-1];
double ResultsArray[ResTsize1][ResTsize2];

double GetOptimum(int _func_num, double* xopt)
{
    if(init_generator == 0)
    {
        FILE *fpt;
        char FileName[50];
        int res = 0;
        sprintf(FileName, "%s/shift_data_%d.txt",path_cec, _func_num);
        fpt = fopen(FileName,"r");
        if (fpt==NULL)
        {
            printf("\n Error: Cannot open input file for reading 1 \n");
            cout<<FileName<<endl;
        }
        for(int k=0;k<GNVars;k++)
        {
            res = fscanf(fpt,"%lf",&xopt[k]);
            if(res == 0)
                cout<<"??";
        }
        fclose(fpt);
    }
    else
    {
        for(int k=0;k<GNVars;k++)
        {
            xopt[k] = RandShiftMatrix_17[func_num-1][0][k];
        }
    }
    //CHANGE!
    cec17_test_func(xopt, tempF, GNVars, 1, func_num);
    //cec22_test_func(xopt, tempF, GNVars, 1, func_num);
    return tempF[0];
}

struct Params
{
    int TaskN;
    int Type;
    int PopSizeLoop;
    int LPSR_loop;
    int Sel1;
    int Sel2;
    int Sel3;
    int Cval1;
    int Cval2;
    int Cval3;
    int GNVars;
    int MemorySizePar;
    int initF;
    int initCr;
    int pBestStart;
    int pBestEnd;
    int MWLp1;
    int MWLp2;
    int SortCR;
    int TopUpdateType;
    int TopUpdateTypeCR;
    int TopUpdateTypeP;
    int SRType;
    int SRTypeCR;
    int SRTypeP;
};

void SaveBestValues(int func_index)
{
    double temp = globalbest - fopt;
    if(temp <= 1E-8 && ResultsArray[func_index][ResTsize2-1] == MaxFEval_DE)
    {
        ResultsArray[func_index][ResTsize2-1] = NFEval_DE;
    }
    for(int stepFEcount=LastFEcount;stepFEcount<ResTsize2-1;stepFEcount++)
    {
        if(NFEval_DE == stepsFEval[stepFEcount])
        {
            if(temp <= 1E-8)
                temp = 0;
            ResultsArray[func_index][stepFEcount] = temp;
            LastFEcount = stepFEcount;
        }
    }
}
void FindLimits(double* Ind,int CurNVars,double CurLeft, double CurRight)
{
    for (int j = 0; j<CurNVars; j++)
    {
        if (Ind[j] < CurLeft)
            Ind[j] = Random(CurLeft,CurRight);
        if (Ind[j] > CurRight)
            Ind[j] = Random(CurLeft,CurRight);
    }
}
class Optimizer
{
public:
    int MemorySize;
    int MemoryIter;
    int SuccessFilled;
    int MemoryCurrentIndex;
    int NVars;			    // размерность пространства
    int NIndsCurrent;
    int NIndsFront;
    int NIndsFrontMin;
    int NIndsFrontMax;
    int newNIndsFront;
    int newNIndsBest;
    int besti;
    int func_num;
    int func_index;
    int TheChosenOne;
    int Generation;
    int IndIter;
    int PFIndex;

    double psize;
    double psizeParamStart;
    double psizeParamEnd;
    double bestfit;
    double SuccessRate;
    double F;       /*параметры*/
    double F2;       /*параметры*/
    double Cr;
    double Right;		    // верхняя граница
    double Left;		    // нижняя граница
    double meanF;
    double meanCr;
    double NewF;

    bool classic = false;

    double* Donor;
    double* Trial;
    int* Rands;

    double** PopulFront;
    double** PopulTemp;
    double* FitMassCopy2;
    double* FitMassFront;
    int* Indexes2;
    double* tempSuccessCr;
    double* tempSuccessF;
    double* FGenerated;
    double* CrGenerated;
    double* CrGeneratedTemp;
    double* MemoryCr;
    double* MemoryF;
    double* FitDelta;
    double* Weights;

    void Initialize(int _newNInds,
                    int _newNVars,
                    int _newfunc_num,
                    int _newfunc_index,
                    int _MemorySizeParam,
                    int _newInitF,
                    int _newInitCr,
                    int _pBestStart,
                    int _pBestEnd,
                    int _MaxFEval_DE);
    void Clean();
    void MainCycle();
    void FindNSaveBest(bool init, int IndIter);
    void SaveSuccessCrF(double Cr, double F, double FitD);
    void UpdateMemoryCrF();
    double MeanWL_general(double* Vector, double* TempWeights, double g_p, double g_m);
    void RemoveWorst(int NInds, int NewNInds);
};
double cec_22_(double* HostVector,int _func_num)
{
    cec17_test_func(HostVector, tempF, GNVars, 1, _func_num);
    NFEval_DE++;
    return tempF[0];
}
void Optimizer::Initialize(int _newNInds,
                           int _newNVars,
                           int _newfunc_num,
                           int _newfunc_index,
                           int _MemorySizeParam,
                           int _newInitF,
                           int _newInitCr,
                           int _pBestStart,
                           int _pBestEnd,
                           int _MaxFEval_DE)
{
    NVars = _newNVars;
    NIndsCurrent = _newNInds;
    NIndsFront = _newNInds;
    NIndsFrontMin = 4;
    NIndsFrontMax = NIndsFront;
    Left = -100;
    Right = 100;
    besti = 0;
    Generation = 0;
    TheChosenOne = 0;
    IndIter = 0;
    psizeParamStart = 0.01*_pBestStart;
    psizeParamEnd = 0.01*_pBestEnd;
    func_num = _newfunc_num;
    func_index = _newfunc_index;
    for(int steps_k=0;steps_k!=ResTsize2-1;steps_k++)
        stepsFEval[steps_k] = int(double(_MaxFEval_DE)/double(ResTsize2-1)*(steps_k+1));
    stepsFEval[ResTsize2-2] = _MaxFEval_DE;
    ResultsArray[func_index][ResTsize2-1] = _MaxFEval_DE;
    PopulFront = new double*[NIndsFront];
    for(int i=0;i!=NIndsFront;i++)
        PopulFront[i] = new double[NVars];
    FitMassCopy2 = new double[NIndsFront];
    FitMassFront = new double[NIndsFront];
    Indexes2 = new int[NIndsFront];
    Weights = new double[NIndsFront];
    FGenerated = new double[NIndsFront];
    CrGenerated = new double[NIndsFront];
    CrGeneratedTemp = new double[NIndsFront];
	for (int i = 0; i<NIndsFront; i++)
		for (int j = 0; j<NVars; j++)
			PopulFront[i][j] = Random(Left,Right);
    Donor = new double[NVars];
    Trial = new double[NVars];
    Rands = new int[NIndsFront];

    tempSuccessCr = new double[NIndsFront];
    tempSuccessF = new double[NIndsFront];
    FitDelta = new double[NIndsFront];

    for(int i=0;i!=NIndsFront;i++)
    {
        tempSuccessCr[i] = 0;
        tempSuccessF[i] = 0;
    }
    MemorySize = _MemorySizeParam;
    MemoryIter = 0;
    SuccessFilled = 0;
    SuccessRate = 0.5;
    meanF = 0.5;
    meanCr = 0.5;
    MemoryCr = new double[MemorySize];
    MemoryF = new double[MemorySize];
    for(int i=0;i!=MemorySize;i++)
    {
        MemoryCr[i] = 0.1*_newInitCr;
        MemoryF[i] = 0.1*_newInitF;
    }
    for(IndIter=0;IndIter<NIndsFront;IndIter++)
    {
        FitMassFront[IndIter] = cec_22_(PopulFront[IndIter],func_num);
        if(!globalbestinit || FitMassFront[IndIter] < globalbest)
        {
            globalbest = FitMassFront[IndIter];
            globalbestinit = true;
        }
        SaveBestValues(func_index);
    }
}
void Optimizer::SaveSuccessCrF(double _Cr, double _F, double FitD)
{
    tempSuccessCr[SuccessFilled] = _Cr;
    tempSuccessF[SuccessFilled] = _F;
    FitDelta[SuccessFilled] = FitD;
    SuccessFilled++;
}
void Optimizer::UpdateMemoryCrF()
{
    if(SuccessFilled != 0)
    {
        double Old_F = MemoryF[MemoryIter];
        double Old_Cr = MemoryCr[MemoryIter];
        double tempmax = tempSuccessCr[0];
        for(int i=0;i!=SuccessFilled;i++)
            if(tempSuccessCr[i] > tempmax)
                tempmax = tempSuccessCr[i];
        if(MemoryCr[MemoryIter] == -1 || tempmax == 0)
            MemoryCr[MemoryIter] = -1;
        else
            MemoryCr[MemoryIter] = (MeanWL_general(tempSuccessCr,FitDelta,global_MWLp2,global_MWLm) + Old_Cr)/2.0;
        MemoryF[MemoryIter] = (MeanWL_general(tempSuccessF,FitDelta,global_MWLp1,global_MWLm) + Old_F)/2.0;
        MemoryIter++;
        if(MemoryIter >= MemorySize)
            MemoryIter = 0;
        meanF = 0;
        meanCr = 0;
        for(int i=0;i!=SuccessFilled;i++)
        {
            meanF += tempSuccessF[i];
            meanCr += tempSuccessCr[i];
        }
        meanF = meanF / double(SuccessFilled);
        meanCr = meanCr / double(SuccessFilled);
    }
}
double Optimizer::MeanWL_general(double* Vector, double* TempWeights, double g_p, double g_m)
{
    double SumWeight = 0;
    double SumSquare = 0;
    double Sum = 0;
    for(int i=0;i!=SuccessFilled;i++)
        SumWeight += TempWeights[i];
    for(int i=0;i!=SuccessFilled;i++)
        Weights[i] = TempWeights[i]/SumWeight;
    for(int i=0;i!=SuccessFilled;i++)
        SumSquare += Weights[i]*fast_pow(Vector[i],int(g_p));
    for(int i=0;i!=SuccessFilled;i++)
        Sum += Weights[i]*fast_pow(Vector[i],int(g_p-g_m));
    if(fabs(Sum) > 0.000001)
        return SumSquare/Sum;
    else
        return 0.5;
}
void Optimizer::RemoveWorst(int _NIndsFront, int _newNIndsFront)
{
    int PointsToRemove = _NIndsFront - _newNIndsFront;
    for(int L=0;L!=PointsToRemove;L++)
    {
        double WorstFit = FitMassFront[0];
        int WorstNum = 0;
        for(int i=1;i!=_NIndsFront;i++)
        {
            if(FitMassFront[i] > WorstFit)
            {
                WorstFit = FitMassFront[i];
                WorstNum = i;
            }
        }
        for(int i=WorstNum;i!=_NIndsFront-1;i++)
        {
            for(int j=0;j!=NVars;j++)
                PopulFront[i][j] = PopulFront[i+1][j];
            FitMassFront[i] = FitMassFront[i+1];
        }
    }
    for(IndIter=0;IndIter<NIndsFront;IndIter++)
    {
        FitMassFront[IndIter] = cec_22_(PopulFront[IndIter],func_num);
        if(!globalbestinit || FitMassFront[IndIter] < globalbest)
        {
            globalbest = FitMassFront[IndIter];
            globalbestinit = true;
        }
        SaveBestValues(func_index);
    }
}
void Optimizer::MainCycle()
{
    double minfit = FitMassFront[0];
    double maxfit = FitMassFront[0];
    for(int i=0;i!=NIndsFront;i++)
    {
        FitMassCopy2[i] = FitMassFront[i];
        Indexes2[i] = i;
        maxfit = max(maxfit,FitMassFront[i]);
        minfit = min(minfit,FitMassFront[i]);
    }
    if(minfit != maxfit)
        qSort2int(FitMassCopy2,Indexes2,0,NIndsFront-1);
    for(IndIter=0;IndIter<NIndsFront;IndIter++)
    {
        MemoryCurrentIndex = IntRandom(MemorySize);
        if(classic)
        {
            do
                F = CauchyRand(MemoryF[MemoryCurrentIndex],0.1);
            while(F < 0.0);
            F = min(F,1.0);
        }
        else
        {
            F = CauchyRand(NewF,0.1);
        }
        FGenerated[IndIter] = F;
        if(MemoryCr[MemoryCurrentIndex] < 0)
            Cr = 0;
        else
            Cr = NormRand(MemoryCr[MemoryCurrentIndex],0.05);
        Cr = min(max(Cr,0.0),1.0);
        CrGeneratedTemp[IndIter] = Cr;
        CrGenerated[IndIter] = Cr;
    }
    if(SortCrossRates == 1)
    {
        qSort1(CrGeneratedTemp,0,NIndsFront-1);
        for(IndIter=0;IndIter<NIndsFront;IndIter++)
            CrGenerated[Indexes2[IndIter]] = CrGeneratedTemp[IndIter];
    }
    int prand;
    int psizeval;
    psize = psizeParamStart + ((double)NFEval_DE/(double)MaxFEval_DE) * (psizeParamEnd-psizeParamStart);
    psizeval = NIndsFront*psize;
    if(psizeval <= 1)
        psizeval = 2;
    for(IndIter=0;IndIter<NIndsFront;IndIter++)
    {
        TheChosenOne = IndIter;
        do
            prand = Indexes2[IntRandom(psizeval)];
        while(prand == TheChosenOne);

        int Rand1 = 0;
        int Rand2 = 0;
        do
        {
            Rand1 = IntRandom(NIndsFront);
        }
        while(Rand1 == prand);
        do
        {
            Rand2 = IntRandom(NIndsFront);
        }
        while(Rand2 == prand || Rand2 == Rand1);
        F = FGenerated[IndIter];
        for(int j=0;j!=NVars;j++)
            Donor[j] = PopulFront[TheChosenOne][j] + F*(PopulFront[prand][j] - PopulFront[TheChosenOne][j]) + F2*(PopulFront[Rand1][j] - PopulFront[Rand2][j]);
        FindLimits(Donor,NVars,Left,Right);
        int WillCrossover = IntRandom(NVars);
        Cr = CrGenerated[TheChosenOne];
        for(int j=0;j!=NVars;j++)
        {
            if(Random(0,1) < Cr || WillCrossover == j)
                Trial[j] = Donor[j];
            else
                Trial[j] = PopulFront[TheChosenOne][j];
        }
        double TempFit = cec_22_(Trial,func_num);
        if(TempFit <= FitMassFront[TheChosenOne])
        {
            PFIndex = IndIter;
            for(int j=0;j!=NVars;j++)
            {
                PopulFront[PFIndex][j] = Trial[j];
            }
            if(TempFit < globalbest)
                globalbest = TempFit;
            SaveSuccessCrF(Cr,F,fabs(FitMassFront[TheChosenOne]-TempFit));
            FitMassFront[PFIndex] = TempFit;
        }
        SaveBestValues(func_index);
    }
    SuccessRate = double(SuccessFilled)/double(NIndsFront);
    newNIndsFront = int(double(NIndsFrontMin-NIndsFrontMax)/double(MaxFEval_DE)*NFEval_DE + NIndsFrontMax);
    if(newNIndsFront < NIndsFrontMin)
        newNIndsFront = NIndsFrontMin;
    RemoveWorst(NIndsFront,newNIndsFront);
    NIndsFront = newNIndsFront;
    UpdateMemoryCrF();
    SuccessFilled = 0;
    Generation ++;
}
void Optimizer::Clean()
{
    delete Donor;
    delete Trial;
    delete Rands;
    for(int i=0;i!=NIndsFrontMax;i++)
        delete PopulFront[i];
    delete PopulFront;
    delete FitMassCopy2;
    delete FitMassFront;
    delete Indexes2;
    delete tempSuccessCr;
    delete tempSuccessF;
    delete FitDelta;
    delete MemoryCr;
    delete MemoryF;
    delete Weights;
    delete FGenerated;
    delete CrGenerated;
    delete CrGeneratedTemp;
}


void call_DE()
{
    GNVars = 10;
    MaxFEval_DE = 100000;
    Params CurPRS;
    CurPRS.PopSizeLoop = 20;
    CurPRS.MemorySizePar = 5;
    CurPRS.initF = 3;
    CurPRS.initCr = 10;
    CurPRS.pBestStart = 30;
    CurPRS.pBestEnd = 30;
    globalbestinit = false;
    LastFEcount = 0;
    NFEval_DE = 0;
    int func_index = 1;
    int _func_num = 1;
    Optimizer OptZ;
    OptZ.Initialize(CurPRS.PopSizeLoop*GNVars,
                    GNVars,
                    _func_num,
                    func_index,
                    CurPRS.MemorySizePar,
                    CurPRS.initF,
                    CurPRS.initCr,
                    CurPRS.pBestStart,
                    CurPRS.pBestEnd,
                    MaxFEval_DE);
    while(NFEval_DE < MaxFEval_DE)
    {
        OptZ.MainCycle();
    }
    OptZ.Clean();
    for(int j=0;j!=ResTsize2;j++)
    {
        // = ResultsArray[func_index][j];
    }
}
