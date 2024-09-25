#include "main.h"
#include "DE.cpp"
const int LimStackSize = 4096;
const int LimSize = 1024;
const int NCECRuns = 1;
const int MaxFuncs = 9;
const int NIndsGlobal = 112;
const unsigned PerfGlobal = NCECRuns*MaxFuncs*1 + 4 + 1; //MaxFuncs*NCECRuns*1=nrows + 4 + NInds*2 //+ NIndsGlobal*2
double ResultsBuffer[PerfGlobal];
double AllResults[NIndsGlobal][PerfGlobal];
int GlobalMode = 0;
string GlobalLog = "";

//AlgParams
int GlobalPopSizeMult = 5;
int GlobalIntRandMult = 64;

class interpreter
{
public:
    bool namequoteflag = 0;

    int bracketCounter = 0;

    int boolSize = 0;
    int minBoolSize = LimStackSize;
    int multBoolSize = 1;

    int intSize = 0;
    int minIntSize = LimStackSize;
    int multIntSize = 1;

    int floatSize = 0;
    int minFloatSize = LimStackSize;
    int multFloatSize = 1;

    int NISize = 0;
    int minNISize = LimStackSize;
    int multNISize = 1;

    int NESize = 0;
    int minNESize = LimStackSize;
    int multNESize = 1;

    int InpSize = 0;
    int minInpSize = LimStackSize;
    int multInpSize = 1;

    int nameSize = 0;
    int minNameSize = LimStackSize;
    int multNameSize = 1;

    int vectorSize = 0 ;
    int minVectorSize = LimStackSize;
    int multVectorSize = 1;

    int minRandomInt = 0;
    //int maxRandomInt = 64;
    int maxRandomInt = GlobalIntRandMult;

    //double minRandomFloat = -100;
    //double maxRandomFloat = 100;

    double minRandomFloat = 0;
    double maxRandomFloat = 1;

    double meanRandomFloat = 0;
    double stdRandomFloat = 1;

    int NLoopLimit = 64;
    int StackSizeLimit = LimSize;
    int NNOOPS = 0;
    int DIM = 2;

    bool verbose = false;

    char buffer[100];

    string prog;
    string inputs;
    string charset;

    unordered_map<string, int> inst_map;
    unordered_map<int, string> inst_map_inv;

    unordered_map<string, vector<atomInst> > names_map;

    bool* boolStack;
    bool* tempBoolStack;

    int* intStack;
    int* tempIntStack;

    double* floatStack;
    double* tempFloatStack;

    int* instNIStack;
    int* instNIConstIStack;
    double* instNIConstFStack;
    string* instNIConstSStack;
    vector<double>* instNIConstVStack;
    int* tempinstNIStack;
    int* tempinstNIConstIStack;
    double* tempinstNIConstFStack;
    string* tempinstNIConstSStack;
    vector<double>* tempinstNIConstVStack;

    int* instNEStack;
    int* instNEConstIStack;
    double* instNEConstFStack;
    string* instNEConstSStack;
    vector<double>* instNEConstVStack;
    int* tempinstNEStack;
    int* tempinstNEConstIStack;
    double* tempinstNEConstFStack;
    string* tempinstNEConstSStack;
    vector<double>* tempinstNEConstVStack;

    int* instInpStack;
    int* instInpConstIStack;
    double* instInpConstFStack;
    string* instInpConstSStack;
    vector<double>* instInpConstVStack;
    int* tempinstInpStack;
    int* tempinstInpConstIStack;
    double* tempinstInpConstFStack;
    string* tempinstInpConstSStack;
    vector<double>* tempinstInpConstVStack;

    string* nameStack;
    string* tempNameStack;

    vector<double>* vectorStack;
    vector<double>* tempVectorStack;

    int* tempInst;
    int* tempInstI;
    double* tempInstF;
    string* tempInstS;
    vector<double>* tempInstV;

    interpreter();
    ~interpreter();
    void loadProgram(const string newprog);
    void setProgram(const vector<atomInst> inProg);
    void setProgram(atomInst* inProg, int newSize);
    void loadInputs(const string newinputs);
    void instStep();
    void reset(int newNESize);
    void updateFloatSize();
    void updateIntSize();
    void updateBoolSize();
    void updateNameSize();
    void updateNESize();
    void updateNISize();
    void updateInpSize();
    void updateVectorSize();
    string printStacks();
    string printProgram();
};
interpreter::interpreter()
{
    DIM = GNVars;
    boolSize = 0;
    boolStack = new bool[minBoolSize];

    intSize = 0;
    intStack = new int[minIntSize];

    floatSize = 0;
    floatStack = new double[minFloatSize];

    NISize = 0;
    instNIStack = new int[minNISize];
    instNIConstIStack = new int[minNISize];
    instNIConstFStack = new double[minNISize];
    instNIConstSStack = new string[minNISize];
    instNIConstVStack = new vector<double>[minNISize];
    for(int i=0;i!=minNISize;i++)
        instNIConstVStack[i].resize(DIM);

    NESize = 0;
    instNEStack = new int[minNESize];
    instNEConstIStack = new int[minNESize];
    instNEConstFStack = new double[minNESize];
    instNEConstSStack = new string[minNESize];
    instNEConstVStack = new vector<double>[minNESize];
    for(int i=0;i!=minNESize;i++)
        instNEConstVStack[i].resize(DIM);

    InpSize = 0;
    instInpStack = new int[minInpSize];
    instInpConstIStack = new int[minInpSize];
    instInpConstFStack = new double[minInpSize];
    instInpConstSStack = new string[minInpSize];
    instInpConstVStack = new vector<double>[minInpSize];
    for(int i=0;i!=minInpSize;i++)
        instInpConstVStack[i].resize(DIM);

    nameSize = 0;
    nameStack = new string[minNameSize];

    vectorSize = 0;
    vectorStack = new vector<double>[minVectorSize];
    for(int i=0;i!=minVectorSize;i++)
        vectorStack[i].resize(DIM);

    tempInst = new int[LimStackSize];
    tempInstI = new int[LimStackSize];
    tempInstF = new double[LimStackSize];
    tempInstS = new string[LimStackSize];
    tempInstV = new vector<double>[LimStackSize];

    charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    ///float 1000
    ///int 2000
    ///name 7000
    ///vector 11000

    inst_map["false"] = 3000;
    inst_map["true"] = 3001;
    inst_map["bool.and"] = 3002;
    inst_map["bool.or"] = 3003;
    inst_map["bool.xor"] = 3004;
    inst_map["bool.not"] = 3005;
    inst_map["bool.fromint"] = 3006;
    inst_map["bool.fromfloat"] = 3007;
    inst_map["bool.rand"] = 3008;
    inst_map["bool.pop"] = 3009;
    inst_map["bool.flush"] = 3010;
    inst_map["bool.dup"] = 3011;
    inst_map["bool.rot"] = 3012;
    inst_map["bool.shove"] = 3013;
    inst_map["bool.swap"] = 3014;
    inst_map["bool.yank"] = 3015;
    inst_map["bool.yankdup"] = 3016;
    inst_map["bool.stackdepth"] = 3017;

    inst_map["int.+"] = 4000;
    inst_map["int.-"] = 4001;
    inst_map["int.*"] = 4002;
    inst_map["int./"] = 4003;
    inst_map["int.%"] = 4004;
    inst_map["int.="] = 4005;
    inst_map["int.>"] = 4006;
    inst_map["int.<"] = 4007;
    inst_map["int.ln"] = 4008;
    inst_map["int.log"] = 4009;
    inst_map["int.pow"] = 4010;
    inst_map["int.min"] = 4011;
    inst_map["int.max"] = 4012;
    inst_map["int.abs"] = 4013;
    inst_map["int.neg"] = 4014;
    inst_map["int.rand"] = 4015;
    inst_map["int.frombool"] = 4016;
    inst_map["int.fromfloat"] = 4017;
    inst_map["int.pop"] = 4018;
    inst_map["int.flush"] = 4019;
    inst_map["int.dup"] = 4020;
    inst_map["int.rot"] = 4021;
    inst_map["int.shove"] = 4022;
    inst_map["int.swap"] = 4023;
    inst_map["int.yank"] = 4024;
    inst_map["int.yankdup"] = 4025;
    inst_map["int.stackdepth"] = 4026;

    inst_map["float.+"] = 5000;
    inst_map["float.-"] = 5001;
    inst_map["float.*"] = 5002;
    inst_map["float./"] = 5003;
    inst_map["float.%"] = 5004;
    inst_map["float.="] = 5005;
    inst_map["float.>"] = 5006;
    inst_map["float.<"] = 5007;
    inst_map["float.ln"] = 5008;
    inst_map["float.log"] = 5009;
    inst_map["float.pow"] = 5010;
    inst_map["float.min"] = 5011;
    inst_map["float.max"] = 5012;
    inst_map["float.abs"] = 5013;
    inst_map["float.neg"] = 5014;
    inst_map["float.rand"] = 5015;
    inst_map["float.frombool"] = 5016;
    inst_map["float.fromint"] = 5017;
    inst_map["float.pop"] = 5018;
    inst_map["float.flush"] = 5019;
    inst_map["float.dup"] = 5020;
    inst_map["float.rot"] = 5021;
    inst_map["float.shove"] = 5022;
    inst_map["float.swap"] = 5023;
    inst_map["float.yank"] = 5024;
    inst_map["float.yankdup"] = 5025;
    inst_map["float.stackdepth"] = 5026;
    inst_map["float.sin"] = 5027;
    inst_map["float.cos"] = 5028;
    inst_map["float.tan"] = 5029;
    inst_map["float.exp"] = 5030;
    inst_map["float.normrand"] = 5031;
    inst_map["float.setmean"] = 5032;
    inst_map["float.setstd"] = 5033;
    inst_map["float.setrandmin"] = 5034;
    inst_map["float.setrandmax"] = 5035;
    inst_map["float.rand01"] = 5036;

    inst_map["input.index"] = 6000;
    inst_map["input.inall"] = 6001;
    inst_map["input.inallrev"] = 6002;
    inst_map["input.stackdepth"] = 6003;

    inst_map["name.="] = 7001;
    inst_map["name.dup"] = 7002;
    inst_map["name.flush"] = 7003;
    inst_map["name.pop"] = 7004;
    inst_map["name.quote"] = 7005;
    inst_map["name.rand"] = 7006;
    inst_map["name.randboundname"] = 7007;
    inst_map["name.rot"] = 7008;
    inst_map["name.shove"] = 7009;
    inst_map["name.stackdepth"] = 7010;
    inst_map["name.swap"] = 7011;
    inst_map["name.yank"] = 7012;
    inst_map["name.yankdup"] = 7013;

    inst_map["exec.="] = 8000;
    inst_map["exec.define"] = 8001;
    inst_map["exec.do*count"] = 8002;
    inst_map["exec.do*range"] = 8003;
    inst_map["exec.do*times"] = 8004;
    inst_map["exec.dup"] = 8005;
    inst_map["exec.flush"] = 8006;
    inst_map["exec.pop"] = 8007;
    inst_map["exec.rot"] = 8008;
    inst_map["exec.shove"] = 8009;
    inst_map["exec.stackdepth"] = 8010;
    inst_map["exec.swap"] = 8011;
    inst_map["exec.yank"] = 8012;
    inst_map["exec.yankdup"] = 8013;
    inst_map["exec.k"] = 8014;
    inst_map["exec.s"] = 8015;
    inst_map["exec.y"] = 8016;
    inst_map["exec.if"] = 8017;

    /*inst_map["code.="] = 9000;
    inst_map["code.append"] = 9001;
    inst_map["code.atom"] = 9002;
    inst_map["code.car"] = 9003;
    inst_map["code.cdr"] = 9004;
    inst_map["code.cons"] = 9005;
    inst_map["code.container"] = 9006;
    inst_map["code.contains"] = 9007;
    inst_map["code.define"] = 9008;
    inst_map["code.definition"] = 9009;
    inst_map["code.discrepancy"] = 9010;
    inst_map["code.do"] = 9011;
    inst_map["code.do*"] = 9012;
    inst_map["code.do*count"] = 9013;
    inst_map["code.do*range"] = 9014;
    inst_map["code.do*times"] = 9015;
    inst_map["code.dup"] = 9016;
    inst_map["code.extract"] = 9017;
    inst_map["code.flush"] = 9018;
    inst_map["code.frombool"] = 9019;
    inst_map["code.fromfloat"] = 9020;
    inst_map["code.fromint"] = 9021;
    inst_map["code.fromname"] = 9022;
    inst_map["code.if"] = 9023;
    inst_map["code.insert"] = 9024;
    inst_map["code.instructions"] = 9025;
    inst_map["code.length"] = 9026;
    inst_map["code.list"] = 9027;
    inst_map["code.member"] = 9028;
    inst_map["code.noop"] = 9029;
    inst_map["code.nth"] = 9030;
    inst_map["code.nthcdr"] = 9031;
    inst_map["code.null"] = 9032;
    inst_map["code.pop"] = 9033;
    inst_map["code.position"] = 9034;
    inst_map["code.quote"] = 9035;
    inst_map["code.rand"] = 9036;
    inst_map["code.shove"] = 9037;
    inst_map["code.size"] = 9038;
    inst_map["code.stackdepth"] = 9039;
    inst_map["code.subst"] = 9040;
    inst_map["code.swap"] = 9041;
    inst_map["code.yank"] = 9042;
    inst_map["code.yankdup"] = 9043;*/

    inst_map["("] = 10000;
    inst_map[")"] = 10001;

    inst_map["vector.+"] = 12000;
    inst_map["vector.-"] = 12001;
    inst_map["vector.*"] = 12002;
    inst_map["vector./"] = 12003;
    inst_map["vector.%"] = 12004;
    inst_map["vector.="] = 12005;
    inst_map["vector.>"] = 12006;
    inst_map["vector.<"] = 12007;
    inst_map["vector.ln"] = 12008;
    inst_map["vector.log"] = 12009;
    inst_map["vector.pow"] = 12010;
    inst_map["vector.min"] = 12011;
    inst_map["vector.max"] = 12012;
    inst_map["vector.abs"] = 12013;
    inst_map["vector.neg"] = 12014;
    inst_map["vector.rand"] = 12015;
    inst_map["vector.frombool"] = 12016;
    inst_map["vector.fromint"] = 12017;
    inst_map["vector.pop"] = 12018;
    inst_map["vector.flush"] = 12019;
    inst_map["vector.dup"] = 12020;
    inst_map["vector.rot"] = 12021;
    inst_map["vector.shove"] = 12022;
    inst_map["vector.swap"] = 12023;
    inst_map["vector.yank"] = 12024;
    inst_map["vector.yankdup"] = 12025;
    inst_map["vector.stackdepth"] = 12026;
    inst_map["vector.sin"] = 12027;
    inst_map["vector.cos"] = 12028;
    inst_map["vector.tan"] = 12029;
    inst_map["vector.exp"] = 12030;
    inst_map["vector.dot"] = 12031;
    inst_map["vector.dimget"] = 12032;
    inst_map["vector.dim+"] = 12033;
    inst_map["vector.dim*"] = 12034;
    inst_map["vector.fromfloat"] = 12035;
    inst_map["vector.sum"] = 12036;
    inst_map["vector.dist"] = 12037;
    inst_map["vector.evalfunc"] = 12038;
    inst_map["vector.scale"] = 12039;
    inst_map["vector.between"] = 12040;
    inst_map["vector.rand01"] = 12041;
    inst_map["vector.normrand"] = 12042;

    inst_map_inv = inverse_map(inst_map);
}
void interpreter::reset(int newNESize)
{
    /*cout<<"vectorStack delete "<<DIM<<endl;
    cout<<vectorSize<<endl;
    for(int i=0;i!=vectorSize;i++)
    {
        cout<<i<<"\t"<<vectorStack[i].size()<<endl;
    }
    try
    {
        delete[] vectorStack;
    }
    catch(...)
    {
        vectorSize = 0;
        cout<<"error free vector"<<endl;
        throw;
    }
    cout<<"vectorStack delete end "<<DIM<<endl;*/

    namequoteflag = 0;
    DIM = GNVars;

    bracketCounter = 0;

    minBoolSize = LimStackSize;
    //multBoolSize = 1;

    minIntSize = LimStackSize;
    //multIntSize = 1;

    minFloatSize = LimStackSize;
    //multFloatSize = 1;

    minNISize = LimStackSize;
    //multNISize = 1;

    minNESize = LimStackSize;
    //multNESize = 1;

    minInpSize = LimStackSize;
    //multInpSize = 1;

    minNameSize = LimStackSize;
    //multNameSize = 1;

    minRandomInt = 0;
    maxRandomInt = 64;

    //minRandomFloat = -100;
    //maxRandomFloat = 100;

    minRandomFloat = 0;
    maxRandomFloat = 1;

    meanRandomFloat = 0;
    stdRandomFloat = 1;

    minVectorSize = LimStackSize;
    //multVectorSize = 1;

    NNOOPS = 0;

    while(minNESize*multNESize < newNESize)
    {
        //cout<<"allocate?"<<endl;
        multNESize*=2;
        multNISize*=2;
        updateNISize();
        updateNESize();
    }

    /*delete boolStack;
    delete intStack;
    delete floatStack;
    delete instNIStack;
    delete instNIConstIStack;
    delete instNIConstFStack;
    delete[] instNIConstSStack;
    delete[] instNIConstVStack;
    delete instNEStack;
    delete instNEConstIStack;
    delete instNEConstFStack;
    delete[] instNEConstSStack;
    delete[] instNEConstVStack;
    delete instInpStack;
    delete instInpConstIStack;
    delete instInpConstFStack;
    delete[] instInpConstSStack;
    delete[] instInpConstVStack;
    delete[] nameStack;
    delete[] vectorStack;*/

    names_map.clear();

    boolSize = 0;
    intSize = 0;
    floatSize = 0;
    NISize = 0;
    NESize = 0;
    InpSize = 0;
    nameSize = 0;
    vectorSize = 0;

    /*boolStack = new bool[minBoolSize];

    intStack = new int[minIntSize];

    floatStack = new double[minFloatSize];

    instNIStack = new int[minNISize*multNISize];
    instNIConstIStack = new int[minNISize*multNISize];
    instNIConstFStack = new double[minNISize*multNISize];
    instNIConstSStack = new string[minNISize*multNISize];
    instNIConstVStack = new vector<double>[minNISize*multNISize];
    for(int i=0;i!=minNISize*multNISize;i++)
        instNIConstVStack[i].resize(DIM);

    instNEStack = new int[minNESize*multNESize];
    instNEConstIStack = new int[minNESize*multNESize];
    instNEConstFStack = new double[minNESize*multNESize];
    instNEConstSStack = new string[minNESize*multNESize];
    instNEConstVStack = new vector<double>[minNESize*multNESize];
    for(int i=0;i!=minNESize*multNESize;i++)
        instNEConstVStack[i].resize(DIM);

    instInpStack = new int[minInpSize];
    instInpConstIStack = new int[minInpSize];
    instInpConstFStack = new double[minInpSize];
    instInpConstSStack = new string[minInpSize];
    instInpConstVStack = new vector<double>[minInpSize];
    for(int i=0;i!=minInpSize;i++)
        instInpConstVStack[i].resize(DIM);

    nameStack = new string[minNameSize];

    vectorStack = new vector<double>[minVectorSize];
    for(int i=0;i!=minVectorSize;i++)
        vectorStack[i].resize(DIM);*/
}
interpreter::~interpreter()
{
    delete boolStack;
    delete intStack;
    delete floatStack;
    delete instNIStack;
    delete instNIConstIStack;
    delete instNIConstFStack;
    delete[] instNIConstSStack;
    delete[] instNIConstVStack;
    delete instNEStack;
    delete instNEConstIStack;
    delete instNEConstFStack;
    delete[] instNEConstSStack;
    delete[] instNEConstVStack;
    delete instInpStack;
    delete instInpConstIStack;
    delete instInpConstFStack;
    delete[] instInpConstSStack;
    delete[] instInpConstVStack;
    delete[] nameStack;
    delete[] vectorStack;
    delete tempInst;
    delete tempInstI;
    delete tempInstF;
    delete[] tempInstS;
    delete[] tempInstV;
}
void interpreter::loadProgram(const string newprog)
{
    prog = newprog;
    int lenprog = 0;
    int lenvec = 0;
    NISize = 0;
    NESize = 0;
    replace_all(prog,"("," ( ");
    replace_all(prog,")"," ) ");
    replace_all(prog,"integer","int");
    replace_all(prog,"boolean","bool");
    replace_all(prog,"\n\n","\n");
    replace_all(prog,"\n","");
    replace_all(prog,"  "," ");
    trim(prog);
    int lastDIM = -1;
    //regex pattern("[+-]?[0-9]+([.][0-9]+)?([eE][+-]?[0-9]+)?");
    vector<string> instructions = explode(prog, ' ', lenprog);
    for(int i=0;i!=lenprog;i++)
    {
        string tempinst = instructions[lenprog-i-1];
        replace_all(tempinst," ","");
        if(inst_map.contains(tempinst))
        {
            instNIStack[NISize] = inst_map[tempinst];
            instNEStack[NESize] = inst_map[tempinst];
            if(verbose)
                cout<<instNIStack[NISize]<<"\t";
            if(verbose)
                cout<<instNEStack[NISize]<<"\t";
        }
        else if(isNumeric(tempinst))
        {
            instNIConstIStack[i] = stoi(tempinst);
            instNIStack[NISize] = 2000;
            if(verbose)
                cout<<instNIStack[NISize]<<"\t";
            if(verbose)
                cout<<instNIConstIStack[NISize]<<"\t";

            instNEConstIStack[i] = stoi(tempinst);
            instNEStack[NESize] = 2000;
            if(verbose)
                cout<<instNEStack[NESize]<<"\t";
            if(verbose)
                cout<<instNEConstIStack[NESize]<<"\t";
        }
        else if(tempinst.find('<') != std::string::npos)
        {
            if(tempinst.find('>') != std::string::npos)
            {
                replace_all(tempinst,"<","");
                replace_all(tempinst,">","");
                vector<string> components = explode(tempinst, ',', lenvec);
                if(lastDIM == -1)
                {
                    lastDIM = components.size();
                    if(lastDIM != DIM)
                    {
                        DIM = lastDIM;
                    }
                }
                else if(DIM != lastDIM)
                {
                    cout<<"Vector size mismatch!"<<endl;
                    getchar();
                }
                for(int j=0;j!=minNISize*multNISize;j++)
                {
                    instNIConstVStack[j].resize(DIM);
                }
                instNIStack[NISize] = 11000;
                if(verbose)
                    cout<<instNIStack[NISize]<<"\t";
                if(verbose)
                    cout<<"<";
                for(int j=0;j!=DIM;j++)
                {
                    instNIConstVStack[NISize][j] = stod(components[j]);
                    if(verbose)
                        cout<<instNIConstVStack[NISize][j]<<",";
                }
                if(verbose)
                    cout<<"> ";
                for(int j=0;j!=minNESize*multNESize;j++)
                {
                    instNEConstVStack[j].resize(DIM);
                }
                instNEStack[NESize] = 11000;
                if(verbose)
                    cout<<instNEStack[NESize]<<"\t";
                if(verbose)
                    cout<<"<";
                for(int j=0;j!=DIM;j++)
                {
                    instNEConstVStack[NESize][j] = stod(components[j]);
                    if(verbose)
                        cout<<instNEConstVStack[NESize][j]<<",";
                }
                if(verbose)
                    cout<<"> ";
            }
            else
            {
                cout<<" > not found"<<endl;
            }
        }
        else if(tempinst.find('.') != std::string::npos) //if(regex_match(tempinst, pattern))
        {
            instNIConstFStack[i] = stod(tempinst);
            instNIStack[NISize] = 1000;
            if(verbose)
                cout<<instNIStack[NISize]<<"\t";
            if(verbose)
                cout<<instNIConstFStack[NISize]<<"\t";

            instNEConstFStack[i] = stod(tempinst);
            instNEStack[NISize] = 1000;
            if(verbose)
                cout<<instNEStack[NESize]<<"\t";
            if(verbose)
                cout<<instNEConstFStack[NESize]<<"\t";
        }
        else
        {
            instNIConstSStack[i] = tempinst;
            instNIStack[NISize] = 7000;
            if(verbose)
                cout<<instNIStack[NISize]<<"\t";
            if(verbose)
                cout<<instNIConstSStack[NISize]<<"\t";

            instNEConstSStack[i] = tempinst;
            instNEStack[NESize] = 7000;
            if(verbose)
                cout<<instNEStack[NESize]<<"\t";
            if(verbose)
                cout<<instNEConstSStack[NESize]<<"\t";
        }
        NISize++;
        NESize++;
        if(NISize >= minNISize*multNISize-1)
        {
            updateNISize();
            updateNESize();
        }
        if(verbose)
            cout<<endl;
    }
}
void interpreter::setProgram(const vector<atomInst> inProg)
{
    int lenprog = inProg.size();
    NISize = lenprog;
    NESize = lenprog;
    while(NISize > minNISize*multNISize-1)
    {
        updateNISize();
        updateNESize();
    }
    int j = 0;
    int lastDIM = -1;
    for(int i=lenprog-1;i>=0;i--)
    {
        if(inProg[j].inst > 2000 && inProg[j].inst != 7000  && inProg[j].inst != 11000)
        {
            instNIStack[i] = inProg[j].inst;
            instNEStack[i] = inProg[j].inst;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNEStack[i]<<"\t";
        }
        else if(inProg[j].inst == 2000)
        {
            instNIConstIStack[i] = inProg[j].i;
            instNIStack[i] = 2000;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNIConstIStack[i]<<"\t";

            instNEConstIStack[i] = inProg[j].i;
            instNEStack[i] = 2000;
            if(verbose)
                cout<<instNEStack[i]<<"\t";
            if(verbose)
                cout<<instNEConstIStack[i]<<"\t";
        }
        else if(inProg[j].inst == 1000)
        {
            instNIConstFStack[i] = inProg[j].f;
            instNIStack[i] = 1000;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNIConstFStack[i]<<"\t";

            instNEConstFStack[i] = inProg[j].f;
            instNEStack[i] = 1000;
            if(verbose)
                cout<<instNEStack[i]<<"\t";
            if(verbose)
                cout<<instNEConstFStack[i]<<"\t";
        }
        else if(inProg[j].inst == 11000)
        {
            bool goodsize = false;
            if(lastDIM == -1)
            {
                lastDIM = inProg[j].v.size();
                if(lastDIM != DIM)
                {
                    //DIM = lastDIM;
                    //cout<<"Vector size mismatch!"<<endl;
                    //getchar();
                }
                else
                {
                    goodsize = true;
                }
            }
            else if(DIM != lastDIM)
            {
                //cout<<"Vector size mismatch!"<<endl;
                //getchar();
            }
            if(goodsize)
            {
                for(int k=0;k!=DIM;k++)
                {
                    instNIConstVStack[k].resize(DIM);
                }
                instNIConstVStack[i] = inProg[j].v;
                instNIStack[i] = 11000;
                if(verbose)
                    cout<<instNIStack[i]<<"\t";
                if(verbose)
                {
                    cout<<"<";
                    for(int k=0;k!=DIM;k++)
                    {
                        cout<<instNIConstVStack[NISize][k]<<",";
                    }
                    cout<<"> ";
                }
                for(int k=0;k!=DIM;k++)
                {
                    instNEConstVStack[k].resize(DIM);
                }
                instNEConstVStack[i] = inProg[j].v;
                instNEStack[i] = 11000;
                if(verbose)
                    cout<<instNEStack[i]<<"\t";
                if(verbose)
                {
                    cout<<"<";
                    for(int k=0;k!=DIM;k++)
                    {
                        cout<<instNEConstVStack[NESize][k]<<",";
                    }
                    cout<<"> ";
                }
            }
        }
        else
        {
            instNIConstSStack[i] = inProg[j].n;
            instNIStack[i] = 7000;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNIConstSStack[i]<<"\t";

            instNEConstSStack[i] = inProg[j].n;
            instNEStack[i] = 7000;
            if(verbose)
                cout<<instNEStack[i]<<"\t";
            if(verbose)
                cout<<instNEConstSStack[i]<<"\t";
        }
        if(verbose)
                cout<<endl;
        j++;
    }
}
void interpreter::setProgram(atomInst* inProg, int newSize)
{
    int lenprog = newSize;
    NISize = lenprog;
    NESize = lenprog;
    while(NISize > minNISize*multNISize-1)
    {
        updateNISize();
        updateNESize();
    }
    int j = 0;
    int lastDIM = -1;
    for(int i=lenprog-1;i>=0;i--)
    {
        if(inProg[j].inst > 2000 && inProg[j].inst != 7000 && inProg[j].inst != 11000)
        {
            instNIStack[i] = inProg[j].inst;
            instNEStack[i] = inProg[j].inst;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNEStack[i]<<"\t";
        }
        else if(inProg[j].inst == 2000)
        {
            instNIConstIStack[i] = inProg[j].i;
            instNIStack[i] = 2000;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNIConstIStack[i]<<"\t";

            instNEConstIStack[i] = inProg[j].i;
            instNEStack[i] = 2000;
            if(verbose)
                cout<<instNEStack[i]<<"\t";
            if(verbose)
                cout<<instNEConstIStack[i]<<"\t";
        }
        else if(inProg[j].inst == 1000)
        {
            instNIConstFStack[i] = inProg[j].f;
            instNIStack[i] = 1000;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNIConstFStack[i]<<"\t";

            instNEConstFStack[i] = inProg[j].f;
            instNEStack[i] = 1000;
            if(verbose)
                cout<<instNEStack[i]<<"\t";
            if(verbose)
                cout<<instNEConstFStack[i]<<"\t";
        }
        else if(inProg[j].inst == 11000)
        {
            bool goodsize = false;
            if(lastDIM == -1)
            {
                lastDIM = inProg[j].v.size();
                if(lastDIM != DIM)
                {
                    //DIM = lastDIM;
                    //cout<<"Vector size mismatch!"<<endl;
                }
                else
                {
                    goodsize = true;
                }
            }
            else if(DIM != lastDIM)
            {
                //cout<<"Vector size mismatch!"<<endl;
                //getchar();
            }
            if(goodsize)
            {
                instNIConstVStack[i] = inProg[j].v;
                instNIStack[i] = 11000;
                if(verbose)
                    cout<<instNIStack[i]<<"\t";
                if(verbose)
                {
                    cout<<"<";
                    for(int k=0;k!=DIM;k++)
                    {
                        cout<<instNIConstVStack[i][k]<<",";
                    }
                    cout<<"> ";
                }
                for(int k=0;k!=DIM;k++)
                {
                    instNEConstVStack[k].resize(DIM);
                }
                instNEConstVStack[i] = inProg[j].v;
                instNEStack[i] = 11000;
                if(verbose)
                    cout<<instNEStack[i]<<"\t";
                if(verbose)
                {
                    cout<<"<";
                    for(int k=0;k!=DIM;k++)
                    {
                        cout<<instNEConstVStack[i][k]<<",";
                    }
                    cout<<"> ";
                }
            }
        }
        else
        {
            instNIConstSStack[i] = inProg[j].n;
            instNIStack[i] = 7000;
            if(verbose)
                cout<<instNIStack[i]<<"\t";
            if(verbose)
                cout<<instNIConstSStack[i]<<"\t";

            instNEConstSStack[i] = inProg[j].n;
            instNEStack[i] = 7000;
            if(verbose)
                cout<<instNEStack[i]<<"\t";
            if(verbose)
                cout<<instNEConstSStack[i]<<"\t";
        }
        if(verbose)
                cout<<endl;
        j++;
    }
}
void interpreter::loadInputs(const string newinputs)
{
    inputs = newinputs;
    int leninp = 0;
    int lenvec = 0;
    InpSize = 0;
    replace_all(inputs,"\n\n","\n");
    replace_all(inputs,"\n","");
    replace_all(inputs,"  "," ");
    trim(inputs);
    int lastDIM = -1;
    regex pattern("[+-]?[0-9]+([.][0-9]+)?([eE][+-]?[0-9]+)?");
    vector<string> instructions = explode(inputs, ' ', leninp);
    for(int i=0;i!=leninp;i++)
    {
        string tempinst = instructions[leninp-i-1];
        replace_all(tempinst," ","");
        if(inst_map.contains(tempinst))
        {
            int INum = inst_map[tempinst];
            if(INum > 3001)
            {
                if(verbose)
                cout<<"INSTRUCTIONS ARE NOT ALLOWED IN INPUT"<<endl;
                continue;
            }
            else
            {
                instInpStack[InpSize] = INum;
                if(verbose)
                cout<<instInpStack[InpSize]<<"\t";
            }
        }
        else if(isNumeric(tempinst))
        {
            instInpConstIStack[i] = stoi(tempinst);
            instInpStack[InpSize] = 2000;
            if(verbose)
                cout<<instInpStack[InpSize]<<"\t";
            if(verbose)
                cout<<instInpConstIStack[InpSize]<<"\t";
        }
        else if(tempinst.find('<') != std::string::npos)
        {
            if(tempinst.find('>') != std::string::npos)
            {
                replace_all(tempinst,"<","");
                replace_all(tempinst,">","");
                vector<string> components = explode(tempinst, ',', lenvec);
                bool goodsize = false;
                if(lastDIM == -1)
                {
                    lastDIM = components.size();
                    if(lastDIM != DIM)
                    {
                        //DIM = lastDIM;
                        //cout<<"Vector size mismatch!"<<endl;
                    }
                    else
                    {
                        goodsize = true;
                    }
                }
                else if(DIM != lastDIM)
                {
                    //cout<<"Vector size mismatch!"<<endl;
                    //getchar();
                }
                if(goodsize)
                {
                    for(int j=0;j!=minInpSize*multIntSize;j++)
                    {
                        instInpConstVStack[j].resize(DIM);
                    }
                    instInpStack[InpSize] = 11000;
                    if(verbose)
                        cout<<instInpStack[InpSize]<<"\t";
                    if(verbose)
                        cout<<"<";
                    for(int k=0;k!=DIM;k++)
                    {
                        instInpConstVStack[InpSize][k] = stod(components[k]);
                        if(verbose)
                            cout<<instInpConstVStack[InpSize][k]<<",";
                    }
                    if(verbose)
                        cout<<"> ";
                }
            }
            else
            {
                cout<<" > not found"<<endl;
            }
        }
        else if(tempinst.find('.') != string::npos) //if(regex_match(tempinst, pattern))
        {
            instInpConstFStack[i] = stod(tempinst);
            instInpStack[InpSize] = 1000;
            if(verbose)
                cout<<instInpStack[InpSize]<<"\t";
            if(verbose)
                cout<<instInpConstFStack[InpSize]<<"\t";
        }
        else
        {
            instInpConstSStack[i] = tempinst;
            instInpStack[InpSize] = 7000;
            if(verbose)
                cout<<instInpStack[InpSize]<<"\t";
            if(verbose)
                cout<<instInpConstSStack[InpSize]<<"\t";
        }
        InpSize++;
        updateInpSize();
        if(verbose)
                cout<<endl;
    }
}
string interpreter::printProgram()
{
    string res = "";
    for(int i=0;i!=NESize;i++)
    {
        if(instNEStack[NESize-i-1] == 1000)
        {
            if(fabs(instNEConstFStack[NESize-i-1]) < 100000 && fabs(instNEConstFStack[NESize-i-1]) > 1e-5)
                sprintf(buffer," %.6f ",instNEConstFStack[NESize-i-1]);
            else
                sprintf(buffer," %.6e ",instNEConstFStack[NESize-i-1]);
        }
        else if(instNEStack[NESize-i-1] == 2000)
            sprintf(buffer," %d ",instNEConstIStack[NESize-i-1]);
        else if(instNEStack[NESize-i-1] == 7000)
            sprintf(buffer," %s ",instNEConstSStack[NESize-i-1].c_str());
        else if(instNEStack[NESize-i-1] == 11000)
        {
            sprintf(buffer,"<"); //cout<<"<";
            res += buffer;
            for(int j=0;j!=DIM;j++)
            {
                if(fabs(instNEConstVStack[NESize-i-1][j]) < 100000 && fabs(instNEConstVStack[NESize-i-1][j]) > 1e-5)
                    sprintf(buffer,"%.6f",instNEConstVStack[NESize-i-1][j]);
                else
                    sprintf(buffer,"%.6e",instNEConstVStack[NESize-i-1][j]);
                res += buffer;
                if(j != DIM-1)
                {
                    sprintf(buffer,",");
                    res += buffer;
                }
            }
            sprintf(buffer,"> ");
            res += buffer;//cout<<"> ";
        }
        else if(inst_map_inv.contains(instNEStack[NESize-i-1]))
            sprintf(buffer," %s ",inst_map_inv[instNEStack[NESize-i-1]].c_str());
        else
            if(verbose)
                cout<<"ERROR INV MAP NOT FOUND"<<endl;
        if(instNEStack[NESize-i-1] != 11000)
            res += buffer;
    }
    return res;
}
string interpreter::printStacks()
{
    string res = "Bool stack: ";
    for(int i=0;i!=boolSize;i++)
    {
        if(boolStack[boolSize-i-1] == false)
            sprintf(buffer," false ");
        else if(boolStack[boolSize-i-1] == true)
            sprintf(buffer," true ");
        else
            if(verbose)
                cout<<"WRONG BOOL"<<endl;
        res += buffer;
    }
    res += "\nInt stack: ";
    for(int i=0;i!=intSize;i++)
    {
        sprintf(buffer," %d ",intStack[intSize-i-1]);
        res += buffer;
    }
    res += "\nFloat stack: ";
    for(int i=0;i!=floatSize;i++)
    {
        if(fabs(floatStack[floatSize-i-1]) < 100000 && fabs(floatStack[floatSize-i-1]) > 1e-5)
            sprintf(buffer," %.6f ",floatStack[floatSize-i-1]);
        else
            sprintf(buffer," %.6e ",floatStack[floatSize-i-1]);
        res += buffer;
    }
    res += "\nExec stack: ";
    for(int i=0;i!=NESize;i++)
    {
        if(instNEStack[NESize-i-1] == 1000)
        {
            if(fabs(instNEConstFStack[NESize-i-1]) < 100000 && fabs(instNEConstFStack[NESize-i-1]) > 1e-5)
                sprintf(buffer," %.6f ",instNEConstFStack[NESize-i-1]);
            else
                sprintf(buffer," %.6e ",instNEConstFStack[NESize-i-1]);
        }
        else if(instNEStack[NESize-i-1] == 2000)
            sprintf(buffer," %d ",instNEConstIStack[NESize-i-1]);
        else if(instNEStack[NESize-i-1] == 7000)
            sprintf(buffer," %s ",instNEConstSStack[NESize-i-1].c_str());
        else if(instNEStack[NESize-i-1] == 11000)
        {
            sprintf(buffer,"<"); //cout<<"<";
            res += buffer;
            for(int j=0;j!=DIM;j++)
            {
                if(fabs(instNEConstVStack[NESize-i-1][j]) < 100000 && fabs(instNEConstVStack[NESize-i-1][j]) > 1e-5)
                    sprintf(buffer,"%.6f",instNEConstVStack[NESize-i-1][j]);
                else
                    sprintf(buffer,"%.6e",instNEConstVStack[NESize-i-1][j]);
                res += buffer;
                if(j != DIM-1)
                {
                    sprintf(buffer,",");
                    res += buffer;
                }
            }
            sprintf(buffer,"> ");
            res += buffer;//cout<<"> ";
        }
        else if(inst_map_inv.contains(instNEStack[NESize-i-1]))
            sprintf(buffer," %s ",inst_map_inv[instNEStack[NESize-i-1]].c_str());
        else
            if(verbose)
                cout<<"ERROR INV MAP NOT FOUND"<<endl;
        if(instNEStack[NESize-i-1] != 11000)
            res += buffer;
    }
    /*res += "\nCode stack: ";
    for(int i=0;i!=NISize;i++)
    {
        if(instNIStack[NISize-i-1] == 1000)
        {
            if(fabs(instNIConstFStack[NISize-i-1]) < 100000 && fabs(instNIConstFStack[NISize-i-1]) > 1e-5)
                sprintf(buffer," %.6f ",instNIConstFStack[NISize-i-1]);
            else
                sprintf(buffer," %.6e ",instNIConstFStack[NISize-i-1]);
        }
        else if(instNIStack[NISize-i-1] == 2000)
            sprintf(buffer," %d ",instNIConstIStack[NISize-i-1]);
        else if(instNIStack[NISize-i-1] == 7000)
            sprintf(buffer," %s ",instNIConstSStack[NISize-i-1].c_str());
        else if(instNIStack[NISize-i-1] == 11000)
        {
            sprintf(buffer,"<"); //cout<<"<";
            res += buffer;
            for(int j=0;j!=DIM;j++)
            {
                if(fabs(instNIConstVStack[NISize-i-1][j]) < 100000 && fabs(instNIConstVStack[NISize-i-1][j]) > 1e-5)
                    sprintf(buffer,"%.6f",instNIConstVStack[NISize-i-1][j]);
                else
                    sprintf(buffer,"%.6e",instNIConstVStack[NISize-i-1][j]);
                res += buffer;
                if(j != DIM-1)
                {
                    sprintf(buffer,",");
                    res += buffer;
                }
            }
            sprintf(buffer,"> ");
            res += buffer;//cout<<"> ";
        }
        else if(inst_map_inv.contains(instNIStack[NISize-i-1]))
            sprintf(buffer," %s ",inst_map_inv[instNIStack[NISize-i-1]].c_str());
        else
            if(verbose)
                cout<<"ERROR INV MAP NOT FOUND"<<endl;
        if(instNIStack[NISize-i-1] != 11000)
            res += buffer;
    }*/
    res += "\nInput stack: ";
    for(int i=0;i!=InpSize;i++)
    {
        if(instInpStack[InpSize-i-1] == 1000)
        {
            if(fabs(instInpConstFStack[InpSize-i-1]) < 100000 && fabs(instInpConstFStack[InpSize-i-1]) > 1e-5)
                sprintf(buffer," %.6f ",instInpConstFStack[InpSize-i-1]);
            else
                sprintf(buffer," %.6e ",instInpConstFStack[InpSize-i-1]);
        }
        else if(instInpStack[InpSize-i-1] == 2000)
            sprintf(buffer," %d ",instInpConstIStack[InpSize-i-1]);
        else if(instInpStack[InpSize-i-1] == 7000)
            sprintf(buffer," %s ",instInpConstSStack[InpSize-i-1].c_str());
        else if(instInpStack[InpSize-i-1] == 11000)
        {
            sprintf(buffer,"<"); //cout<<"<";
            res += buffer;
            for(int j=0;j!=DIM;j++)
            {
                if(fabs(instInpConstVStack[InpSize-i-1][j]) < 100000 && fabs(instInpConstVStack[InpSize-i-1][j]) > 1e-5)
                    sprintf(buffer,"%.6f",instInpConstVStack[InpSize-i-1][j]);
                else
                    sprintf(buffer,"%.6e",instInpConstVStack[InpSize-i-1][j]);
                res += buffer;
                if(j != DIM-1)
                {
                    sprintf(buffer,",");
                    res += buffer;
                }
            }
            sprintf(buffer,"> ");
            res += buffer;//cout<<"> ";
        }
        else if(inst_map_inv.contains(instInpStack[InpSize-i-1]))
            sprintf(buffer," %s ",inst_map_inv[instInpStack[InpSize-i-1]].c_str());
        else
            if(verbose)
                cout<<"ERROR INV MAP NOT FOUND"<<endl;
        if(instInpStack[InpSize-i-1] != 11000)
            res += buffer;
    }
    res += "\nName stack: ";
    for(int i=0;i!=nameSize;i++)
    {
        sprintf(buffer," %s ",nameStack[nameSize-i-1].c_str());
        res += buffer;
    }
    res += "\nVector stack: ";
    for(int i=0;i!=vectorSize;i++)
    {
        sprintf(buffer,"<"); //cout<<"<";
        res += buffer;
        for(int j=0;j!=DIM;j++)
        {
            if(fabs(vectorStack[vectorSize-i-1][j]) < 100000 && fabs(vectorStack[vectorSize-i-1][j]) > 1e-5)
                sprintf(buffer,"%.6f",vectorStack[vectorSize-i-1][j]);
            else
                sprintf(buffer,"%.6e",vectorStack[vectorSize-i-1][j]);
            res += buffer;
            if(j != DIM-1)
            {
                sprintf(buffer,",");
                res += buffer;
            }
        }
        sprintf(buffer,"> ");
        res += buffer;//cout<<"> ";
    }
    res+="\n";
    return res;
}
void interpreter::updateFloatSize()
{
    if(floatSize == minFloatSize*multFloatSize-1)
    {
        multFloatSize = multFloatSize*2;
        tempFloatStack = new double[minFloatSize*multFloatSize];
        for(int j=0;j!=floatSize;j++)
            tempFloatStack[j] = floatStack[j];
        delete floatStack;
        floatStack = tempFloatStack;
        cout<<"Update float size "<<floatSize<<endl;
    }
}
void interpreter::updateIntSize()
{
    if(intSize == minIntSize*multIntSize-1)
    {
        multIntSize = multIntSize*2;
        tempIntStack = new int[minIntSize*multIntSize];
        for(int j=0;j!=intSize;j++)
            tempIntStack[j] = intStack[j];
        delete intStack;
        intStack = tempIntStack;
        cout<<"Update int size "<<intSize<<endl;
    }
}
void interpreter::updateBoolSize()
{
    if(boolSize == minBoolSize*multBoolSize-1)
    {
        multBoolSize = multBoolSize*2;
        tempBoolStack = new bool[minBoolSize*multBoolSize];
        for(int j=0;j!=boolSize;j++)
            tempBoolStack[j] = boolStack[j];
        delete boolStack;
        boolStack = tempBoolStack;
        cout<<"Update bool size "<<boolSize<<endl;
    }
}
void interpreter::updateNameSize()
{
    if(nameSize == minNameSize*multNameSize-1)
    {
        multNameSize = multNameSize*2;
        tempNameStack = new string[minNameSize*multNameSize];
        for(int j=0;j!=nameSize;j++)
            tempNameStack[j] = nameStack[j];
        delete[] nameStack;
        nameStack = tempNameStack;
        cout<<"Update name size "<<nameSize<<endl;
    }
}
void interpreter::updateNESize()
{
    if(NESize >= minNESize*multNESize-1)
    {
        multNESize = multNESize*2;
        tempinstNEStack = new int[minNESize*multNESize];
        tempinstNEConstIStack = new int[minNESize*multNESize];
        tempinstNEConstFStack = new double[minNESize*multNESize];
        tempinstNEConstSStack = new string[minNESize*multNESize];
        tempinstNEConstVStack = new vector<double>[minNESize*multNESize];
        for(int k=0;k!=NESize;k++)
        {
            tempinstNEStack[k] = instNEStack[k];
            tempinstNEConstIStack[k] = instNEConstIStack[k];
            tempinstNEConstFStack[k] = instNEConstFStack[k];
            tempinstNEConstSStack[k] = instNEConstSStack[k];
            tempinstNEConstVStack[k] = instNEConstVStack[k];
        }
        for(int k=NESize;k!=minNESize*multNESize;k++)
            tempinstNEConstVStack[k].resize(DIM);
        delete instNEStack;
        delete instNEConstIStack;
        delete instNEConstFStack;
        delete[] instNEConstSStack;
        delete[] instNEConstVStack;
        instNEStack = tempinstNEStack;
        instNEConstIStack = tempinstNEConstIStack;
        instNEConstFStack = tempinstNEConstFStack;
        instNEConstSStack = tempinstNEConstSStack;
        instNEConstVStack = tempinstNEConstVStack;
        cout<<"Update NE size "<<NESize<<endl;
    }
}
void interpreter::updateNISize()
{
    if(NISize >= minNISize*multNISize-1)
    {
        multNISize = multNISize*2;
        tempinstNIStack = new int[minNISize*multNISize];
        tempinstNIConstIStack = new int[minNISize*multNISize];
        tempinstNIConstFStack = new double[minNISize*multNISize];
        tempinstNIConstSStack = new string[minNISize*multNISize];
        tempinstNIConstVStack = new vector<double>[minNISize*multNISize];
        for(int k2=0;k2!=NISize;k2++)
        {
            tempinstNIStack[k2] = instNIStack[k2];
            tempinstNIConstIStack[k2] = instNIConstIStack[k2];
            tempinstNIConstFStack[k2] = instNIConstFStack[k2];
            tempinstNIConstSStack[k2] = instNIConstSStack[k2];
            tempinstNIConstVStack[k2] = instNIConstVStack[k2];
        }
        for(int k2=NISize;k2!=minNISize*multNISize;k2++)
            tempinstNIConstVStack[k2].resize(DIM);
        delete instNIStack;
        delete instNIConstIStack;
        delete instNIConstFStack;
        delete[] instNIConstSStack;
        delete[] instNIConstVStack;
        instNIStack = tempinstNIStack;
        instNIConstIStack = tempinstNIConstIStack;
        instNIConstFStack = tempinstNIConstFStack;
        instNIConstSStack = tempinstNIConstSStack;
        instNIConstVStack = tempinstNIConstVStack;
        cout<<"Update NI size "<<NISize<<endl;
    }
}
void interpreter::updateInpSize()
{
    if(InpSize == minInpSize*multInpSize-1)
    {
        multInpSize = multInpSize*2;
        tempinstInpStack = new int[minInpSize*multInpSize];
        tempinstInpConstIStack = new int[minInpSize*multInpSize];
        tempinstInpConstFStack = new double[minInpSize*multInpSize];
        tempinstInpConstSStack = new string[minInpSize*multInpSize];
        tempinstInpConstVStack = new vector<double>[minInpSize*multInpSize];
        for(int j=0;j!=InpSize;j++)
        {
            tempinstInpStack[j] = instInpStack[j];
            tempinstInpConstIStack[j] = instInpConstIStack[j];
            tempinstInpConstFStack[j] = instInpConstFStack[j];
            tempinstInpConstSStack[j] = instInpConstSStack[j];
            tempinstInpConstVStack[j] = instInpConstVStack[j];
        }
        delete instInpStack;
        delete instInpConstIStack;
        delete instInpConstFStack;
        delete[] instInpConstSStack;
        delete[] instInpConstVStack;
        instInpStack = tempinstInpStack;
        instInpConstIStack = tempinstInpConstIStack;
        instInpConstFStack = tempinstInpConstFStack;
        instInpConstSStack = tempinstInpConstSStack;
        instInpConstVStack = tempinstInpConstVStack;
        cout<<"Update input size "<<InpSize<<endl;
    }
}
void interpreter::updateVectorSize()
{
    if(vectorSize == minVectorSize*multVectorSize-1)
    {
        multVectorSize = multVectorSize*2;
        tempVectorStack = new vector<double>[minVectorSize*multVectorSize];
        for(int j=0;j!=vectorSize;j++)
        {
            tempVectorStack[j] = vectorStack[j];
            if(tempVectorStack[j].size() == 0)
                tempVectorStack[j].resize(DIM);
        }
        for(int j=vectorSize;j!=minVectorSize*multVectorSize;j++)
            tempVectorStack[j].resize(DIM);
        delete[] vectorStack;
        vectorStack = tempVectorStack;
        cout<<"Update vector size "<<vectorSize<<endl;
    }
}
void interpreter::instStep()
{
    int idx = NESize - 1;
    if(idx < 0)
    {
        if(verbose)
                cout<<"EMPTY EXEC"<<endl;
        return;
    }
    int instr = instNEStack[idx];
    switch(instr)
    {
        default:
            {
                if(verbose)
                cout<<"ERROR UNKNOWN INSTRUCTION "<< instr <<endl;
                NNOOPS++;
            } break;
        case 1000:
            {
                floatStack[floatSize] = instNEConstFStack[idx];
                floatSize++;
                updateFloatSize();
            } break;
        case 2000:
            {
                intStack[intSize] = instNEConstIStack[idx];
                intSize++;
                updateIntSize();
            } break;
        case 3000:
            {
                if(verbose)
                cout<<"BOOL FALSE"<<endl;
                boolStack[boolSize] = false;
                boolSize++;
                updateBoolSize();
            } break;
        case 3001:
            {
                if(verbose)
                cout<<"BOOL TRUE"<<endl;
                boolStack[boolSize] = true;
                boolSize++;
                updateBoolSize();
            } break;
        case 3002:
            {
                if(boolSize > 1)
                {
                    if(verbose)
                cout<<"BOOL AND"<<endl;
                    boolStack[boolSize-2] = boolStack[boolSize-2] & boolStack[boolSize-1];
                    boolSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3003:
            {
                if(boolSize > 1)
                {
                    if(verbose)
                cout<<"BOOL OR"<<endl;
                    boolStack[boolSize-2] = boolStack[boolSize-2] | boolStack[boolSize-1];
                    boolSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3004:
            {
                if(boolSize > 1)
                {
                    if(verbose)
                cout<<"BOOL XOR"<<endl;
                    boolStack[boolSize-2] = boolStack[boolSize-2] ^ boolStack[boolSize-1];
                    boolSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3005:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                cout<<"BOOL NOT"<<endl;
                    boolStack[boolSize-1] = !boolStack[boolSize-1];
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3006:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"BOOL FROM INT"<<endl;
                    boolStack[boolSize] = (intStack[intSize-1] != 0);
                    boolSize++;
                    updateBoolSize();
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3007:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"BOOL FROM FLOAT"<<endl;
                    boolStack[boolSize] = !((floatStack[floatSize-1] < 1) & (floatStack[floatSize-1] >= 0));
                    boolSize++;
                    updateBoolSize();
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3008:
            {
                if(verbose)
                cout<<"BOOL RAND"<<endl;
                boolStack[boolSize] = bool(IntRandom2(2));
                boolSize++;
                updateBoolSize();
            } break;
        case 3009:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                        cout<<"BOOL POP"<<endl;
                    boolSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3010:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                cout<<"BOOL FLUSH"<<endl;
                    boolSize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3011:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                cout<<"BOOL DUP"<<endl;
                    boolStack[boolSize] = boolStack[boolSize-1];
                    boolSize++;
                    updateBoolSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3012:
            {
                if(boolSize > 2)
                {
                    if(verbose)
                cout<<"BOOL ROT"<<endl;
                    bool tmp = boolStack[boolSize-3];
                    boolStack[boolSize-3] = boolStack[boolSize-2];
                    boolStack[boolSize-2] = boolStack[boolSize-1];
                    boolStack[boolSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3013:
            {
                if(boolSize > 0 && intSize > 0)
                {
                    if(verbose)
                cout<<"BOOL SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 0)
                        index_shove = 0;
                    if(index_shove > boolSize-1)
                        index_shove = boolSize-1;
                    bool tmp = boolStack[boolSize-1];
                    index_shove = boolSize-1-index_shove;
                    for(int j=boolSize-1;j>index_shove;j--)
                        boolStack[j] = boolStack[j-1];
                    boolStack[index_shove] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3014:
            {
                if(boolSize > 1)
                {
                    if(verbose)
                cout<<"BOOL SWAP"<<endl;
                    bool tmp = boolStack[boolSize-1];
                    boolStack[boolSize-1] = boolStack[boolSize-2];
                    boolStack[boolSize-2] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3015:
            {
                if(boolSize > 0 && intSize > 0)
                {
                    if(verbose)
                cout<<"BOOL YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > boolSize-1)
                        index_yank = boolSize-1;
                    index_yank = boolSize-1-index_yank;
                    bool tmp = boolStack[index_yank];
                    for(int j=index_yank;j<boolSize-1;j++)
                        boolStack[j] = boolStack[j+1];
                    boolStack[boolSize-1] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3016:
            {
                if(boolSize > 0 && intSize > 0)
                {
                    if(verbose)
                cout<<"BOOL YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > boolSize-1)
                        index_yank = boolSize-1;
                    index_yank = boolSize-1-index_yank;
                    bool tmp = boolStack[index_yank];
                    boolStack[boolSize] = tmp;
                    boolSize++;
                    updateBoolSize();
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 3017:
            {
                if(verbose)
                cout<<"BOOL STACKDEPTH"<<endl;
                intStack[intSize] = boolSize;
                intSize++;
                updateIntSize();
            } break;
        case 4000:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT +"<<endl;
                    intStack[intSize-2] = intStack[intSize-2] + intStack[intSize-1];
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] > 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-2]))
                        intStack[intSize-2] = 0;
                    intSize--;
                }
            } break;
        case 4001:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT -"<<endl;
                    intStack[intSize-2] = intStack[intSize-2] - intStack[intSize-1];
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] > 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-2]))
                        intStack[intSize-2] = 0;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4002:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT *"<<endl;
                    intStack[intSize-2] = intStack[intSize-2] * intStack[intSize-1];
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] > 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-2]))
                        intStack[intSize-2] = 0;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4003:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT /"<<endl;
                    if(abs(intStack[intSize-1]) == 0 && intStack[intSize-2] >= 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    else if(abs(intStack[intSize-1]) == 0 && intStack[intSize-2] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    else if(intStack[intSize-2] == std::numeric_limits<int>::min() && intStack[intSize-1] == -1)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    else
                        intStack[intSize-2] = intStack[intSize-2] / intStack[intSize-1];
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] > 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-2]))
                        intStack[intSize-2] = 0;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4004:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT %"<<endl;
                    if(intStack[intSize-2] == std::numeric_limits<int>::min() && intStack[intSize-1] == -1)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    else if(intStack[intSize-1] != 0)
                        intStack[intSize-2] = intStack[intSize-2] % intStack[intSize-1];
                    else
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4005:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT ="<<endl;
                    boolStack[boolSize] = intStack[intSize-2] == intStack[intSize-1];
                    boolSize++;
                    updateBoolSize();
                    intSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4006:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT >"<<endl;
                    boolStack[boolSize] = intStack[intSize-2] > intStack[intSize-1];
                    boolSize++;
                    updateBoolSize();
                    intSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4007:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT <"<<endl;
                    boolStack[boolSize] = intStack[intSize-2] < intStack[intSize-1];
                    boolSize++;
                    updateBoolSize();
                    intSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4008:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"INT LN"<<endl;
                    intStack[intSize-1] = log(intStack[intSize-1]);
                    if(isinf(intStack[intSize-1]) && intStack[intSize-1] > 0)
                        intStack[intSize-1] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-1]) && intStack[intSize-1] < 0)
                        intStack[intSize-1] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-1]))
                        intStack[intSize-1] = 0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4009:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT LOG"<<endl;
                    intStack[intSize-2] = log(intStack[intSize-1]) / log(intStack[intSize-2]);
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] > 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-2]) && intStack[intSize-2] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-2]))
                        intStack[intSize-2] = 0;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4010:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT POW"<<endl;
                    intStack[intSize-2] = pow(intStack[intSize-1], intStack[intSize-2]);
                    if(isinf(intStack[intSize-2]) && intStack[intSize-1] > 0)
                        intStack[intSize-2] = std::numeric_limits<int>::max();
                    if(isinf(intStack[intSize-2]) && intStack[intSize-1] < 0)
                        intStack[intSize-2] = std::numeric_limits<int>::min();
                    if(isnan(intStack[intSize-2]))
                        intStack[intSize-2] = 0;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4011:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT MIN"<<endl;
                    intStack[intSize-2] = min(intStack[intSize-1], intStack[intSize-2]);
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4012:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT MAX"<<endl;
                    intStack[intSize-2] = max(intStack[intSize-1], intStack[intSize-2]);
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4013:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"INT ABS"<<endl;
                    intStack[intSize-1] = abs(intStack[intSize-1]);
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4014:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"INT NEG"<<endl;
                    intStack[intSize-1] = -intStack[intSize-1];
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4015:
            {
                if(verbose)
                cout<<"INT RAND"<<endl;
                intStack[intSize] = IntRandom2(maxRandomInt-minRandomInt)+minRandomInt;
                intSize++;
                updateIntSize();
            } break;
        case 4016:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                cout<<"INT FROMBOOL"<<endl;
                    intStack[intSize] = int(boolStack[boolSize-1]);
                    boolSize--;
                    intSize++;
                    updateIntSize();
                }
            } break;
        case 4017:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"INT FROMFLOAT"<<endl;
                    intStack[intSize] = int(floatStack[floatSize-1]);
                    floatSize--;
                    intSize++;
                    updateIntSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4018:
            {
                if(intSize > 0)
                {
                    if(verbose)
                        cout<<"INT POP"<<endl;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4019:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"INT FLUSH"<<endl;
                    intSize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4020:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"INT DUP"<<endl;
                    intStack[intSize] = intStack[intSize-1];
                    intSize++;
                    updateIntSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4021:
            {
                if(intSize > 2)
                {
                    if(verbose)
                cout<<"INT ROT"<<endl;
                    int tmp = intStack[intSize-3];
                    intStack[intSize-3] = intStack[intSize-2];
                    intStack[intSize-2] = intStack[intSize-1];
                    intStack[intSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4022:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 0)
                        index_shove = 0;
                    if(index_shove > intSize-2)
                        index_shove = intSize-2;
                    intSize--;
                    int tmp = intStack[intSize-1];
                    index_shove = intSize-1-index_shove;
                    for(int j=intSize-1;j>index_shove;j--)
                        intStack[j] = intStack[j-1];
                    intStack[index_shove] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4023:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT SWAP"<<endl;
                    bool tmp = intStack[intSize-1]; ///BUG!!!
                    intStack[intSize-1] = intStack[intSize-2];
                    intStack[intSize-2] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4024:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > intSize-2)
                        index_yank = intSize-2;
                    intSize--;
                    index_yank = intSize-1-index_yank;
                    int tmp = intStack[index_yank];
                    for(int j=index_yank;j<intSize-1;j++)
                        intStack[j] = intStack[j+1];
                    intStack[intSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4025:
            {
                if(intSize > 1)
                {
                    if(verbose)
                cout<<"INT YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > intSize-2)
                        index_yank = intSize-2;
                    index_yank = intSize-2-index_yank;
                    int tmp = intStack[index_yank];
                    intStack[intSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 4026:
            {
                if(verbose)
                cout<<"INT STACKDEPTH"<<endl;
                intStack[intSize] = intSize;
                intSize++;
                updateIntSize();
            } break;
        //float
        case 5000:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT +"<<endl;
                    floatStack[floatSize-2] = floatStack[floatSize-2] + floatStack[floatSize-1];
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = -std::numeric_limits<double>::max();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5001:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT -"<<endl;
                    floatStack[floatSize-2] = floatStack[floatSize-2] - floatStack[floatSize-1];
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = -std::numeric_limits<double>::max();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5002:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT *"<<endl;
                    floatStack[floatSize-2] = floatStack[floatSize-2] * floatStack[floatSize-1];
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = -std::numeric_limits<double>::max();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5003:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT /"<<endl;
                    if(fabs(floatStack[floatSize-1]) == 0 && floatStack[floatSize-2] >= 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    else if(fabs(floatStack[floatSize-1]) == 0 && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = -std::numeric_limits<double>::max();
                    else
                        floatStack[floatSize-2] = floatStack[floatSize-2] / floatStack[floatSize-1];
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5004:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT %"<<endl;
                    floatStack[floatSize-2] = remainder(floatStack[floatSize-2], floatStack[floatSize-1]);
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5005:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT ="<<endl;
                    boolStack[boolSize] = floatStack[floatSize-2] == floatStack[floatSize-1];
                    boolSize++;
                    updateBoolSize();
                    floatSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5006:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT >"<<endl;
                    boolStack[boolSize] = floatStack[floatSize-2] > floatStack[floatSize-1];
                    boolSize++;
                    updateBoolSize();
                    floatSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5007:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT <"<<endl;
                    boolStack[boolSize] = floatStack[floatSize-2] < floatStack[floatSize-1];
                    boolSize++;
                    updateBoolSize();
                    floatSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5008:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT LN"<<endl;
                    floatStack[floatSize-1] = log(floatStack[floatSize-1]);
                    if(isinf(floatStack[floatSize-1]) && floatStack[floatSize-1] > 0)
                        floatStack[floatSize-1] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-1]) && floatStack[floatSize-1] < 0)
                        floatStack[floatSize-1] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-1]))
                        floatStack[floatSize-1] = 0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5009:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT LOG"<<endl;
                    floatStack[floatSize-2] = log(floatStack[floatSize-1]) / log(floatStack[floatSize-2]);
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-2] < 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5010:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT POW"<<endl;
                    floatStack[floatSize-2] = pow(floatStack[floatSize-1], floatStack[floatSize-2]);
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-1] > 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-2]) && floatStack[floatSize-1] < 0)
                        floatStack[floatSize-2] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-2]))
                        floatStack[floatSize-2] = 0;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5011:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT MIN"<<endl;
                    floatStack[floatSize-2] = min(floatStack[floatSize-1], floatStack[floatSize-2]);
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5012:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT MAX"<<endl;
                    floatStack[floatSize-2] = max(floatStack[floatSize-1], floatStack[floatSize-2]);
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5013:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT ABS"<<endl;
                    floatStack[floatSize-1] = abs(floatStack[floatSize-1]);
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5014:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT NEG"<<endl;
                    floatStack[floatSize-1] = -floatStack[floatSize-1];
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5015:
            {
                if(verbose)
                    cout<<"FLOAT RAND"<<endl;
                floatStack[floatSize] = Random2(minRandomFloat,maxRandomFloat);
                floatSize++;
                updateFloatSize();
            } break;
        case 5016:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT FROMBOOL"<<endl;
                    floatStack[floatSize] = double(boolStack[boolSize-1]);
                    boolSize--;
                    floatSize++;
                    updateFloatSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5017:
            {
                if(intSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT FROMINT"<<endl;
                    floatStack[floatSize] = double(intStack[intSize-1]);
                    intSize--;
                    floatSize++;
                    updateFloatSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5018:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                        cout<<"FLOAT POP"<<endl;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5019:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT FLUSH"<<endl;
                    floatSize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5020:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT DUP"<<endl;
                    floatStack[floatSize] = floatStack[floatSize-1];
                    floatSize++;
                    updateFloatSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5021:
            {
                if(floatSize > 2)
                {
                    if(verbose)
                cout<<"FLOAT ROT"<<endl;
                    double tmp = floatStack[floatSize-3];
                    floatStack[floatSize-3] = floatStack[floatSize-2];
                    floatStack[floatSize-2] = floatStack[floatSize-1];
                    floatStack[floatSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5022:
            {
                if(floatSize > 0 && intSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 0)
                        index_shove = 0;
                    if(index_shove > floatSize-1)
                        index_shove = floatSize-1;
                    double tmp = floatStack[floatSize-1];
                    index_shove = floatSize-1-index_shove;
                    for(int j=floatSize-1;j>index_shove;j--)
                        floatStack[j] = floatStack[j-1];
                    floatStack[index_shove] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5023:
            {
                if(floatSize > 1)
                {
                    if(verbose)
                cout<<"FLOAT SWAP"<<endl;
                    double tmp = floatStack[floatSize-1];
                    floatStack[floatSize-1] = floatStack[floatSize-2];
                    floatStack[floatSize-2] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5024:
            {
                if(floatSize > 0 && intSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > floatSize-1)
                        index_yank = floatSize-1;
                    index_yank = floatSize-1-index_yank;
                    double tmp = floatStack[index_yank];
                    for(int j=index_yank;j<floatSize-1;j++)
                        floatStack[j] = floatStack[j+1];
                    floatStack[floatSize-1] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5025:
            {
                if(floatSize > 0 && intSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > floatSize-1)
                        index_yank = floatSize-1;
                    index_yank = floatSize-1-index_yank;
                    double tmp = floatStack[index_yank];
                    floatStack[floatSize] = tmp;
                    floatSize++;
                    updateFloatSize();
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5026:
            {
                if(verbose)
                cout<<"FLOAT STACKDEPTH"<<endl;
                intStack[intSize] = floatSize;
                intSize++;
                updateIntSize();
            } break;
        case 5027:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT SIN"<<endl;
                    floatStack[floatSize-1] = sin(floatStack[floatSize-1]);
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5028:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT COS"<<endl;
                    floatStack[floatSize-1] = cos(floatStack[floatSize-1]);
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5029:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT TAN"<<endl;
                    floatStack[floatSize-1] = tan(floatStack[floatSize-1]);
                    if(isinf(floatStack[floatSize-1]) && floatStack[floatSize-1] > 0)
                        floatStack[floatSize-1] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-1]) && floatStack[floatSize-1] < 0)
                        floatStack[floatSize-1] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-1]))
                        floatStack[floatSize-1] = 0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5030:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                cout<<"FLOAT EXP"<<endl;
                    floatStack[floatSize-1] = exp(floatStack[floatSize-1]);
                    if(isinf(floatStack[floatSize-1]) && floatStack[floatSize-1] > 0)
                        floatStack[floatSize-1] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize-1]) && floatStack[floatSize-1] < 0)
                        floatStack[floatSize-1] = std::numeric_limits<double>::min();
                    if(isnan(floatStack[floatSize-1]))
                        floatStack[floatSize-1] = 0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 5031:
            {
                if(verbose)
                    cout<<"FLOAT NORMRAND"<<endl;
                floatStack[floatSize] = NormRand2(meanRandomFloat,stdRandomFloat);
                floatSize++;
                updateFloatSize();
            } break;
        case 5032:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                        cout<<"FLOAT SETMEAN"<<endl;
                    meanRandomFloat = floatStack[floatSize-1];
                    floatSize--;
                }
            } break;
        case 5033:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                        cout<<"FLOAT SETSTD"<<endl;
                    stdRandomFloat = floatStack[floatSize-1];
                    floatSize--;
                }
            } break;
        case 5034:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                        cout<<"FLOAT SETMIN"<<endl;
                    minRandomFloat = floatStack[floatSize-1];
                    floatSize--;
                }
            } break;
        case 5035:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                        cout<<"FLOAT SETMAX"<<endl;
                    maxRandomFloat = floatStack[floatSize-1];
                    floatSize--;
                }
            } break;
        case 5036:
            {
                if(verbose)
                    cout<<"FLOAT RAND01"<<endl;
                floatStack[floatSize] = Random2(0,1);
                floatSize++;
                updateFloatSize();
            } break;
        case 6000:
            {
                if(InpSize > 0 && intSize > 0)
                {
                    if(verbose)
                        cout<<"INPUT ININDEX"<<endl;
                    int idx2 = intStack[intSize-1];
                    if(idx2 > InpSize-1)
                        idx2 = InpSize-1;
                    if(idx2 < 0)
                        idx2 = 0;
                    int instr2 = instInpStack[idx2];
                    switch(instr2)
                    {
                    default:
                        {
                            if(verbose)
                                cout<<"ERROR UNKNOWN INPUT "<< instr2 <<endl;
                        } break;
                    case 1000:
                        {
                            floatStack[floatSize] = instInpConstFStack[idx2];
                            floatSize++;
                            updateFloatSize();
                        } break;
                    case 2000:
                        {
                            intStack[intSize] = instInpConstIStack[idx2];
                            intSize++;
                            updateIntSize();
                        } break;
                    case 3000:
                        {
                            if(verbose)
                                cout<<"BOOL TRUE"<<endl;
                            boolStack[boolSize] = false;
                            boolSize++;
                            updateBoolSize();
                        } break;
                    case 3001:
                        {
                            if(verbose)
                                cout<<"BOOL FALSE"<<endl;
                            boolStack[boolSize] = true;
                            boolSize++;
                            updateBoolSize();
                        } break;
                    case 11000:
                        {
                            if(verbose)
                                cout<<"INPUT VECTOR"<<endl;
                            if(int(instInpConstVStack[idx2].size()) == DIM)
                            {
                                vectorStack[vectorSize].resize(DIM);
                                for(int d=0;d!=DIM;d++)
                                {
                                    vectorStack[vectorSize][d] = instInpConstVStack[idx2][d];
                                }
                                vectorSize++;
                                updateVectorSize();
                            }
                            else
                            {
                                NNOOPS++;
                            }
                        } break;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 6001:
            {
                if(InpSize > 0)
                {
                    if(verbose)
                        cout<<"INPUT ALL"<<endl;
                    while(InpSize > 0)
                    {
                        int idx2 = InpSize-1;
                        int instr2 = instInpStack[idx2];
                        switch(instr2)
                        {
                        default:
                            {
                                if(verbose)
                                    cout<<"ERROR UNKNOWN INPUT "<< instr2 <<endl;
                            } break;
                        case 1000:
                            {
                                floatStack[floatSize] = instInpConstFStack[idx2];
                                floatSize++;
                                updateFloatSize();
                            } break;
                        case 2000:
                            {
                                intStack[intSize] = instInpConstIStack[idx2];
                                intSize++;
                                updateIntSize();
                            } break;
                        case 3000:
                            {
                                if(verbose)
                                    cout<<"BOOL TRUE"<<endl;
                                boolStack[boolSize] = false;
                                boolSize++;
                                updateBoolSize();
                            } break;
                        case 3001:
                            {
                                if(verbose)
                                    cout<<"BOOL FALSE"<<endl;
                                boolStack[boolSize] = true;
                                boolSize++;
                                updateBoolSize();
                            } break;
                        case 11000:
                            {
                                if(verbose)
                                    cout<<"VECTOR INPUT"<<endl;
                                if(int(instInpConstVStack[idx2].size()) == DIM)
                                {
                                    vectorStack[vectorSize].resize(DIM);
                                    for(int d=0;d!=DIM;d++)
                                    {
                                        vectorStack[vectorSize][d] = instInpConstVStack[idx2][d];
                                    }
                                    vectorSize++;
                                    updateVectorSize();
                                }
                                else
                                {
                                    NNOOPS++;
                                }
                            } break;
                        }
                        InpSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 6002:
            {
                if(InpSize > 0)
                {
                    if(verbose)
                        cout<<"INPUT ALL REVERSE"<<endl;
                    for(int idx2=0;idx2!=InpSize;idx2++)
                    {
                        int instr2 = instInpStack[idx2];
                        switch(instr2)
                        {
                        default:
                            {
                                if(verbose)
                                    cout<<"ERROR UNKNOWN INPUT "<< instr2 <<endl;
                            } break;
                        case 1000:
                            {
                                floatStack[floatSize] = instInpConstFStack[idx2];
                                floatSize++;
                                updateFloatSize();
                            } break;
                        case 2000:
                            {
                                intStack[intSize] = instInpConstIStack[idx2];
                                intSize++;
                                updateIntSize();
                            } break;
                        case 3000:
                            {
                                if(verbose)
                                    cout<<"BOOL TRUE"<<endl;
                                boolStack[boolSize] = false;
                                boolSize++;
                                updateBoolSize();
                            } break;
                        case 3001:
                            {
                                if(verbose)
                                    cout<<"BOOL FALSE"<<endl;
                                boolStack[boolSize] = true;
                                boolSize++;
                                updateBoolSize();
                            } break;
                        case 11000:
                            {
                                if(verbose)
                                    cout<<"VECTOR INPUT"<<endl;
                                if(int(instInpConstVStack[idx2].size()) == DIM)
                                {
                                    vectorStack[vectorSize].resize(DIM);
                                    for(int d=0;d!=DIM;d++)
                                    {
                                        vectorStack[vectorSize][d] = instInpConstVStack[idx2][d];
                                    }
                                    vectorSize++;
                                    updateVectorSize();
                                }
                                else
                                {
                                    NNOOPS++;
                                }
                            } break;
                        }
                    }
                    InpSize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 6003:
            {
                if(verbose)
                    cout<<"INPUT STACKDEPTH"<<endl;
                intStack[intSize] = InpSize;
                intSize++;
                updateIntSize();
            } break;
        case 7000:
            {
                if(namequoteflag || !names_map.contains(instNEConstSStack[NESize-1]))
                {
                    if(verbose)
                        cout<<"NAME ADD NAME"<<endl;
                    if(verbose)
                        cout<<"CONTAINS "<<names_map.contains(instNEConstSStack[NESize-1])<<endl;
                    string newname = instNEConstSStack[NESize-1];
                    nameStack[nameSize] = newname;
                    nameSize++;
                    updateNameSize();
                    namequoteflag = false;
                }
                else
                {
                    if(verbose)
                        cout<<"NAME GET INSTRUCTION"<<endl;
                    vector<atomInst> subProg = names_map[instNEConstSStack[NESize-1]];
                    int Len = subProg.size();
                    for(int j=0;j!=Len;j++)
                    {
                        instNEStack[NESize-1] = subProg[j].inst;
                        instNEConstIStack[NESize-1] = subProg[j].i;
                        instNEConstFStack[NESize-1] = subProg[j].f;
                        instNEConstSStack[NESize-1] = subProg[j].n;
                        instNEConstVStack[NESize-1] = subProg[j].v;
                        NESize++;
                        updateNESize();
                    }
                }
            } break;
        case 7001:
            {
                if(nameSize > 1)
                {
                    if(verbose)
                cout<<"NAME ="<<endl;
                    boolStack[boolSize] = !(nameStack[nameSize-1].compare(nameStack[nameSize-2]));
                    boolSize++;
                    updateBoolSize();
                    nameSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7002:
            {
                if(nameSize > 0)
                {
                    if(verbose)
                        cout<<"NAME DUP"<<endl;
                    nameStack[nameSize] = nameStack[nameSize-1];
                    nameSize++;
                    updateNameSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7003:
            {
                if(nameSize > 0)
                {
                    if(verbose)
                        cout<<"NAME FLUSH"<<endl;
                    nameSize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7004:
            {
                if(nameSize > 0)
                {
                    if(verbose)
                        cout<<"NAME POP"<<endl;
                    nameSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7005:
            {
                if(verbose)
                    cout<<"NAME QUOTE"<<endl;
                namequoteflag = true;
            } break;
        case 7006:
            {
                if(verbose)
                    cout<<"NAME RAND"<<endl;
                string randstr;
                int randlen;
                int maxlen = 1;
                bool hassame = false;
                do
                {
                    randlen = IntRandom2(maxlen)+2;
                    randstr.resize(randlen);
                    for (int i = 0; i < randlen; i++)
                        randstr[i] = charset[IntRandom2(52)];
                    hassame = names_map.contains(randstr);
                    maxlen += IntRandom2(2);
                }
                while(hassame);
                nameStack[nameSize] = randstr;
                nameSize++;
                updateNameSize();
            } break;
        case 7007:
            {
                if(names_map.size() > 0)
                {
                    if(verbose)
                        cout<<"NAME RANDBOUNDNAME"<<endl;
                    vector<string> keys;
                    for(unordered_map<string,vector<atomInst> >::iterator it = names_map.begin(); it != names_map.end(); ++it)
                    {
                        keys.push_back(it->first);
                        if(verbose)
                            cout << "Key: " << it->first << endl;
                    }
                    string randstr = keys[IntRandom2(names_map.size())];
                    if(verbose)
                        cout<<randstr<<endl;
                    nameStack[nameSize] = randstr;
                    nameSize++;
                    updateNameSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7008:
            {
                if(nameSize > 2)
                {
                    if(verbose)
                        cout<<"NAME ROT"<<endl;
                    string tmp = nameStack[nameSize-3];
                    nameStack[nameSize-3] = nameStack[nameSize-2];
                    nameStack[nameSize-2] = nameStack[nameSize-1];
                    nameStack[nameSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7009:
            {
                if(nameSize > 0 && intSize > 0)
                {
                    if(verbose)
                        cout<<"NAME SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 0)
                        index_shove = 0;
                    if(index_shove > nameSize-1)
                        index_shove = nameSize-1;
                    string tmp = nameStack[nameSize-1];
                    index_shove = nameSize-1-index_shove;
                    for(int j=nameSize-1;j>index_shove;j--)
                        nameStack[j] = nameStack[j-1];
                    nameStack[index_shove] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7010:
            {
                if(verbose)
                    cout<<"NAME STACKDEPTH"<<endl;
                intStack[intSize] = nameSize;
                intSize++;
                updateIntSize();
            } break;
        case 7011:
            {
                if(nameSize > 1)
                {
                    if(verbose)
                        cout<<"NAME SWAP"<<endl;
                    string tmp = nameStack[nameSize-1];
                    nameStack[nameSize-1] = nameStack[nameSize-2];
                    nameStack[nameSize-2] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7012:
            {
                if(nameSize > 0 && intSize > 0)
                {
                    if(verbose)
                        cout<<"NAME YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > nameSize-1)
                        index_yank = nameSize-1;
                    index_yank = nameSize-1-index_yank;
                    string tmp = nameStack[index_yank];
                    for(int j=index_yank;j<nameSize-1;j++)
                        nameStack[j] = nameStack[j+1];
                    nameStack[nameSize-1] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 7013:
            {
                if(nameSize > 0 && intSize > 0)
                {
                    if(verbose)
                        cout<<"NAME YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > nameSize-1)
                        index_yank = nameSize-1;
                    index_yank = nameSize-1-index_yank;
                    string tmp = nameStack[index_yank];
                    nameStack[nameSize] = tmp;
                    nameSize++;
                    updateNameSize();
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8000:
            {
                if(NESize > 1)
                {
                    if(verbose)
                        cout<<"EXEC ="<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNEStack[j] == 10000);
                        bracketCounter-=(instNEStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    int start2 = end1;
                    int end2 = start2-1;
                    int range2 = 1;
                    if(end1 >= 0)
                    {
                        if(instNEStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        range2 = start2 - end2;
                        int neq = 0;
                        bool eq2 = false;
                        if(range1 == range2)
                        {
                            for(int j=0;j<range1;j++)
                            {
                                eq2 = (instNEStack[end1+j+1] == instNEStack[end2+j+1] && instNEStack[end1+j+1] > 2000 && instNEStack[end1+j+1] != 7000
                                       && instNEStack[end1+j+1] != 11000) ||
                                          (instNEStack[end1+j+1] == 1000 && instNEConstFStack[end1+j+1] == instNEConstFStack[end2+j+1]) ||
                                          (instNEStack[end1+j+1] == 2000 && instNEConstIStack[end1+j+1] == instNEConstIStack[end2+j+1]) ||
                                          (instNEStack[end1+j+1] == 7000 && instNEConstSStack[end1+j+1] == instNEConstSStack[end2+j+1]);
                                int neq2 = 0;
                                if(instNEStack[end1+j+1] == 11000)
                                {
                                    for(int d=0;d!=DIM;d++)
                                    {
                                        if(instNEConstVStack[end1+j+1][d] == instNEConstVStack[end2+j+1][d])
                                            neq2++;
                                    }
                                    eq2 = eq2 || (neq2 == DIM);
                                }
                                neq += eq2;
                            }
                        }
                        boolStack[boolSize] = (neq == range1);
                        boolSize++;
                        updateBoolSize();
                    }
                    //NESize-=range1;
                    //NESize-=range2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8001:
            {
                if(nameSize > 0 && NESize > 1)
                {
                    if(verbose)
                cout<<"EXEC DEFINE"<<endl;
                    vector<atomInst> subProg;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNEStack[j] == 10000);
                        bracketCounter-=(instNEStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1-end1;
                    int k = end1+1;
                    for(int j=0;j!=range1;j++)
                    {
                        atomInst b;
                        b.inst = instNEStack[k];
                        b.i = instNEConstIStack[k];
                        b.f = instNEConstFStack[k];
                        b.n = instNEConstSStack[k];
                        b.v = instNEConstVStack[k];
                        subProg.push_back(b);
                        k++;
                    }
                    if(verbose)
                cout<<printSubProg(subProg)<<endl;
                    names_map[nameStack[nameSize-1]] = subProg;
                    NESize-=range1;
                    nameSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8002:
            {
                if(NESize > 0 && intSize > 0)///TODO
                {
                    if(verbose)
                        cout<<"EXEC DO*COUNT"<<endl;
                    if(intStack[intSize-1] > 0)
                    {
                        int NLoop = intStack[intSize-1];
                        NLoop = min(NLoopLimit,NLoop);
                        NLoop = max(0,NLoop);
                        int start1 = NESize-2;
                        int end1 = NESize-3;
                        if(instNEStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            int range1 = start1 - end1;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNEStack[j];
                                tempInstI[j-end1-1] = instNEConstIStack[j];
                                tempInstF[j-end1-1] = instNEConstFStack[j];
                                tempInstS[j-end1-1] = instNEConstSStack[j];
                                tempInstV[j-end1-1] = instNEConstVStack[j];
                            }
                            NESize-=range1;
                            for(int L=0;L!=NLoop;L++)
                            {
                                for(int j=0;j!=range1;j++)
                                {
                                    instNEStack[NESize-1] = tempInst[j];
                                    instNEConstIStack[NESize-1] = tempInstI[j];
                                    instNEConstFStack[NESize-1] = tempInstF[j];
                                    instNEConstSStack[NESize-1] = tempInstS[j];
                                    instNEConstVStack[NESize-1] = tempInstV[j];
                                    NESize++;
                                    updateNESize();
                                    if(NESize >= StackSizeLimit)
                                        break;
                                }
                                instNEStack[NESize-1] = 2000;
                                instNEConstIStack[NESize-1] = NLoop-L-1;
                                NESize++;
                                updateNESize();
                                if(NESize >= StackSizeLimit)
                                    break;
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8003:
            {
                if(NESize > 0 && intSize > 1)///TODO
                {
                    if(verbose)
                        cout<<"EXEC DO*RANGE"<<endl;
                    if(intStack[intSize-1] != intStack[intSize-2])
                    {
                        int NLoop = abs(intStack[intSize-1] - intStack[intSize-2]);
                        NLoop = min(NLoopLimit,NLoop);
                        NLoop = max(0,NLoop);
                        int LStart = intStack[intSize-2];
                        int LStop = intStack[intSize-1];
                        int dir = 2*(LStop - LStart > 0)-1;
                        int start1 = NESize-2;
                        int end1 = NESize-3;
                        if(instNEStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            int range1 = start1 - end1;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNEStack[j];
                                tempInstI[j-end1-1] = instNEConstIStack[j];
                                tempInstF[j-end1-1] = instNEConstFStack[j];
                                tempInstS[j-end1-1] = instNEConstSStack[j];
                                tempInstV[j-end1-1] = instNEConstVStack[j];
                            }
                            NESize-=range1;
                            for(int L=0;L!=NLoop;L++)
                            {
                                for(int j=0;j!=range1;j++)
                                {
                                    instNEStack[NESize-1] = tempInst[j];
                                    instNEConstIStack[NESize-1] = tempInstI[j];
                                    instNEConstFStack[NESize-1] = tempInstF[j];
                                    instNEConstSStack[NESize-1] = tempInstS[j];
                                    instNEConstVStack[NESize-1] = tempInstV[j];
                                    NESize++;
                                    updateNESize();
                                    if(NESize >= StackSizeLimit)
                                        break;
                                }
                                instNEStack[NESize-1] = 2000;
                                instNEConstIStack[NESize-1] = LStart + dir*(L+1);
                                NESize++;
                                updateNESize();
                                if(NESize >= StackSizeLimit)
                                    break;
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        intSize-=2;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8004:
            {
                if(NESize > 0 && intSize > 0)///TODO
                {
                    if(verbose)
                        cout<<"EXEC DO*TIMES"<<endl;
                    if(intStack[intSize-1] > 0)
                    {
                        int NLoop = intStack[intSize-1];
                        NLoop = min(NLoopLimit,NLoop);
                        NLoop = max(0,NLoop);
                        int start1 = NESize-2;
                        int end1 = NESize-3;
                        if(instNEStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            int range1 = start1 - end1;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNEStack[j];
                                tempInstI[j-end1-1] = instNEConstIStack[j];
                                tempInstF[j-end1-1] = instNEConstFStack[j];
                                tempInstS[j-end1-1] = instNEConstSStack[j];
                                tempInstV[j-end1-1] = instNEConstVStack[j];
                            }
                            NESize-=range1;
                            for(int L=0;L!=NLoop;L++)
                            {
                                for(int j=0;j!=range1;j++)
                                {
                                    instNEStack[NESize-1] = tempInst[j];
                                    instNEConstIStack[NESize-1] = tempInstI[j];
                                    instNEConstFStack[NESize-1] = tempInstF[j];
                                    instNEConstSStack[NESize-1] = tempInstS[j];
                                    instNEConstVStack[NESize-1] = tempInstV[j];
                                    NESize++;
                                    updateNESize();
                                    if(NESize >= StackSizeLimit)
                                        break;
                                }
                                if(NESize >= StackSizeLimit)
                                    break;
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8005:
            {
                if(NESize > 1)
                {
                    if(instNEStack[NESize-2] == 10000 && NESize > 2)
                    {
                        if(verbose)
                            cout<<"EXEC DUP SUBPROG"<<endl;
                        bracketCounter=0;
                        int pairIndex = -1;
                        for(int j=NESize-2;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                pairIndex = j;
                                break;
                            }
                        }
                        if(pairIndex != -1)
                        {
                            int tmpIndex = NESize;
                            NESize -= 1;
                            for(int j=pairIndex;j<tmpIndex;j++)
                            {
                                instNEStack[NESize] = instNEStack[j];
                                instNEConstFStack[NESize] = instNEConstFStack[j];
                                instNEConstIStack[NESize] = instNEConstIStack[j];
                                instNEConstSStack[NESize] = instNEConstSStack[j];
                                instNEConstVStack[NESize] = instNEConstVStack[j];
                                NESize++;
                                updateNESize();
                            }
                        }
                        else
                            if(verbose)
                                cout<<"NO MATCHING CLOSE BRACKET"<<endl;
                    }
                    else
                    {
                        if(verbose)
                            cout<<"EXEC DUP"<<endl;
                        instNEStack[NESize-1] = instNEStack[NESize-2];
                        instNEConstFStack[NESize-1] = instNEConstFStack[NESize-2];
                        instNEConstIStack[NESize-1] = instNEConstIStack[NESize-2];
                        instNEConstSStack[NESize-1] = instNEConstSStack[NESize-2];
                        instNEConstVStack[NESize-1] = instNEConstVStack[NESize-2];
                        if(instNEStack[NESize-1] != 8005)
                            NESize++;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8006:
            {
                if(NESize > 0)
                {
                    if(verbose)
                        cout<<"EXEC FLUSH"<<endl;
                    NESize=1;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8007:
            {
                if(NESize > 1)
                {
                    if(verbose)
                        cout<<"EXEC POP"<<endl;
                    if(instNEStack[NESize-2] == 10000)
                    {
                        int start1 = NESize-2;
                        int end1 = NESize-3;
                        if(instNEStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            NESize -= (start1-end1);
                        }
                    }
                    else
                    {
                        NESize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8008:
            {
                if(NESize > 3)
                {
                    if(verbose)
                        cout<<"EXEC ROT"<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    int start2;
                    int end2;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 > 0)
                    {
                        if(instNEStack[end1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=end1;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        else
                            end1--;
                        if(end1 >= 0)
                        {
                            start2 = end1;
                            end2 = start2-1;
                            if(instNEStack[start2] == 10000)
                            {
                                bracketCounter = 0;
                                for(int j=start2;j>=0;j--)
                                {
                                    bracketCounter+=(instNEStack[j] == 10000);
                                    bracketCounter-=(instNEStack[j] == 10001);
                                    if(bracketCounter == 0)
                                    {
                                        end2 = j-1;
                                        break;
                                    }
                                }
                            }
                            int range1 = start1 - end1;
                            int range2 = start2 - end2;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNEStack[j];
                                tempInstI[j-end1-1] = instNEConstIStack[j];
                                tempInstF[j-end1-1] = instNEConstFStack[j];
                                tempInstS[j-end1-1] = instNEConstSStack[j];
                                tempInstV[j-end1-1] = instNEConstVStack[j];
                            }
                            for(int j=start1;j!=start1-range2;j--)
                            {
                                instNEStack[j] = instNEStack[j-range1];
                                instNEConstIStack[j] = instNEConstIStack[j-range1];
                                instNEConstFStack[j] = instNEConstFStack[j-range1];
                                instNEConstSStack[j] = instNEConstSStack[j-range1];
                                instNEConstVStack[j] = instNEConstVStack[j-range1];
                            }
                            for(int j=start1-range2;j!=end2;j--)
                            {
                                instNEStack[j] = tempInst[j-end2-1];
                                instNEConstIStack[j] = tempInstI[j-end2-1];
                                instNEConstFStack[j] = tempInstF[j-end2-1];
                                instNEConstSStack[j] = tempInstS[j-end2-1];
                                instNEConstVStack[j] = tempInstV[j-end2-1];
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        else
                        {
                            if(verbose)
                                cout<<"EXEC ROT NOT ENOUGH 2"<<endl;
                        }
                    }
                    else
                    {
                        if(verbose)
                            cout<<"EXEC ROT NOT ENOUGH 1"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8009:
            {
                if(NESize > 1 && intSize > 0)
                {
                    if(verbose)
                        cout<<"EXEC SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 1)
                        index_shove = 1;
                    if(index_shove > NESize-1)
                        index_shove = NESize-1;
                    index_shove = NESize-2-index_shove;

                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNEStack[j];
                            tempInstI[j-end1-1] = instNEConstIStack[j];
                            tempInstF[j-end1-1] = instNEConstFStack[j];
                            tempInstS[j-end1-1] = instNEConstSStack[j];
                            tempInstV[j-end1-1] = instNEConstVStack[j];
                        }
                        if(index_shove > end1 && index_shove <= start1)
                        {
                            //insert
                            if(verbose)
                                cout<<"insert"<<endl;
                            int nadd = range1 - (start1 - index_shove);
                            for(int j=0;j<range1;j++)
                            {
                                instNEStack[NESize-1] = tempInst[j];
                                instNEConstIStack[NESize-1] = tempInstI[j];
                                instNEConstFStack[NESize-1] = tempInstF[j];
                                instNEConstSStack[NESize-1] = tempInstS[j];
                                instNEConstVStack[NESize-1] = tempInstV[j];
                                NESize++;
                                updateNESize();
                            }
                            int k = start1 + nadd;
                            for(int j=0;j!=range1;j++)
                            {
                                instNEStack[k] = tempInst[range1-1-j];
                                instNEConstIStack[k] = tempInstI[range1-1-j];
                                instNEConstFStack[k] = tempInstF[range1-1-j];
                                instNEConstSStack[k] = tempInstS[range1-1-j];
                                instNEConstVStack[k] = tempInstV[range1-1-j];
                                k--;
                            }
                            if(verbose)
                                cout<<"end insert"<<endl;
                        }
                        else
                        {
                            //swap
                            int k = NESize - 2;
                            for(int j=end1;j!=index_shove;j--)
                            {
                                instNEStack[k] = instNEStack[j];
                                instNEConstIStack[k] = instNEConstIStack[j];
                                instNEConstFStack[k] = instNEConstFStack[j];
                                instNEConstSStack[k] = instNEConstSStack[j];
                                instNEConstVStack[k] = instNEConstVStack[j];
                                k--;
                            }
                            for(int j=range1-1;j>=0;j--)
                            {
                                instNEStack[k] = tempInst[j];
                                instNEConstIStack[k] = tempInstI[j];
                                instNEConstFStack[k] = tempInstF[j];
                                instNEConstSStack[k] = tempInstS[j];
                                instNEConstVStack[k] = tempInstV[j];
                                k--;
                            }
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8010:
            {
                if(verbose)
                    cout<<"EXEC STACKDEPTH"<<endl;
                intStack[intSize] = NESize;
                intSize++;
                updateIntSize();
            } break;
        case 8011:
            {
                if(NESize > 2)
                {
                    if(verbose)
                        cout<<"EXEC SWAP"<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    int start2;
                    int end2;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        start2 = end1;
                        end2 = start2-1;
                        if(instNEStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        int range1 = start1 - end1;
                        int range2 = start2 - end2;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNEStack[j];
                            tempInstI[j-end1-1] = instNEConstIStack[j];
                            tempInstF[j-end1-1] = instNEConstFStack[j];
                            tempInstS[j-end1-1] = instNEConstSStack[j];
                            tempInstV[j-end1-1] = instNEConstVStack[j];
                        }
                        for(int j=start1;j!=start1-range2;j--)
                        {
                            instNEStack[j] = instNEStack[j-range1];
                            instNEConstIStack[j] = instNEConstIStack[j-range1];
                            instNEConstFStack[j] = instNEConstFStack[j-range1];
                            instNEConstSStack[j] = instNEConstSStack[j-range1];
                            instNEConstVStack[j] = instNEConstVStack[j-range1];
                        }
                        for(int j=start1-range2;j!=end2;j--)
                        {
                            instNEStack[j] = tempInst[j-end2-1];
                            instNEConstIStack[j] = tempInstI[j-end2-1];
                            instNEConstFStack[j] = tempInstF[j-end2-1];
                            instNEConstSStack[j] = tempInstS[j-end2-1];
                            instNEConstVStack[j] = tempInstV[j-end2-1];
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                    }
                    else
                    {
                        if(verbose)
                            cout<<"EXEC SWAP NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8012:
            {
                if(NESize > 1 && intSize > 0)
                {
                    if(verbose)
                        cout<<"EXEC YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 1)
                        index_yank = 1;
                    if(index_yank > NESize-1)
                        index_yank = NESize-1;
                    index_yank = NESize-1-index_yank;

                    int start1 = index_yank;
                    int end1 = index_yank-1;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= -1)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                        {
                            tempInstV[j].resize(DIM);
                        }*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNEStack[j];
                            tempInstI[j-end1-1] = instNEConstIStack[j];
                            tempInstF[j-end1-1] = instNEConstFStack[j];
                            tempInstS[j-end1-1] = instNEConstSStack[j];
                            tempInstV[j-end1-1] = instNEConstVStack[j];
                        }
                        int k = end1+1;
                        for(int j=0;k!=NESize-range1-1;j++)
                        {
                            instNEStack[k] = instNEStack[k+range1];
                            instNEConstIStack[k] = instNEConstIStack[k+range1];
                            instNEConstFStack[k] = instNEConstFStack[k+range1];
                            instNEConstSStack[k] = instNEConstSStack[k+range1];
                            instNEConstVStack[k] = instNEConstVStack[k+range1];
                            k++;
                        }
                        k = NESize - 2;
                        for(int j=0;j!=range1;j++)
                        {
                            instNEStack[k] = tempInst[range1-1-j];
                            instNEConstIStack[k] = tempInstI[range1-1-j];
                            instNEConstFStack[k] = tempInstF[range1-1-j];
                            instNEConstSStack[k] = tempInstS[range1-1-j];
                            instNEConstVStack[k] = tempInstV[range1-1-j];
                            k--;
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        intSize--;
                    }
                    else
                    {
                        if(verbose)
                cout<<"EXEC YANK NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8013:
            {
                if(NESize > 1 && intSize > 0)
                {
                    if(verbose)
                        cout<<"EXEC YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 1)
                        index_yank = 1;
                    if(index_yank > NESize-1)
                        index_yank = NESize-1;
                    index_yank = NESize-1-index_yank;

                    int start1 = index_yank;
                    int end1 = index_yank-1;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= -1)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                        {
                            tempInstV[j].resize(DIM);
                        }*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNEStack[j];
                            tempInstI[j-end1-1] = instNEConstIStack[j];
                            tempInstF[j-end1-1] = instNEConstFStack[j];
                            tempInstS[j-end1-1] = instNEConstSStack[j];
                            tempInstV[j-end1-1] = instNEConstVStack[j];
                        }
                        int k = NESize-1;
                        for(int j=0;j!=range1;j++)
                        {
                            instNEStack[k] = tempInst[j];
                            instNEConstIStack[k] = tempInstI[j];
                            instNEConstFStack[k] = tempInstF[j];
                            instNEConstSStack[k] = tempInstS[j];
                            instNEConstVStack[k] = tempInstV[j];
                            k++;
                            NESize++;
                            updateNESize();
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8014:
            {
                if(NESize > 2)
                {
                    if(verbose)
                        cout<<"EXEC K COMBINATOR"<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    int start2;
                    int end2;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        start2 = end1;
                        end2 = start2-1;
                        if(instNEStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        int range1 = start1 - end1;
                        int range2 = start2 - end2;
                        int k = end2+1;
                        for(int j=0;j!=range1;j++)
                        {
                            instNEStack[k] = instNEStack[k+range2];
                            instNEConstIStack[k] = instNEConstIStack[k+range2];
                            instNEConstFStack[k] = instNEConstFStack[k+range2];
                            instNEConstSStack[k] = instNEConstSStack[k+range2];
                            instNEConstVStack[k] = instNEConstVStack[k+range2];
                            k++;
                        }
                        NESize -= range2;
                    }
                    else
                    {
                        if(verbose)
                cout<<"EXEC K NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8015:
            {
                if(NESize > 3)
                {
                    if(verbose)
                        cout<<"EXEC S COMBINATOR"<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    int start2;
                    int end2;
                    int start3;
                    int end3;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        start2 = end1;
                        end2 = start2-1;
                        if(instNEStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }

                        if(end2 >= 1)
                        {
                            start3 = end2;
                            end3 = start3-1;
                            if(instNEStack[start3] == 10000)
                            {
                                bracketCounter = 0;
                                for(int j=start3;j>=0;j--)
                                {
                                    bracketCounter+=(instNEStack[j] == 10000);
                                    bracketCounter-=(instNEStack[j] == 10001);
                                    if(bracketCounter == 0)
                                    {
                                        end3 = j-1;
                                        break;
                                    }
                                }
                            }
                            int range1 = start1 - end1;
                            int range2 = start2 - end2;
                            int range3 = start3 - end3;

                            /*int* tempInst = new int[range1+range2+range3];
                            int* tempInstI = new int[range1+range2+range3];
                            double* tempInstF = new double[range1+range2+range3];
                            string* tempInstS = new string[range1+range2+range3];
                            vector<double>* tempInstV = new vector<double>[range1+range2+range3];*/
                            /*vector<int> tempInst(range1+range2+range3);
                            vector<int> tempInstI(range1+range2+range3);
                            vector<double> tempInstF(range1+range2+range3);
                            vector<string> tempInstS(range1+range2+range3);
                            vector<vector<double> > tempInstV(range1+range2+range3);
                            for(int j=0;j!=range1+range2+range3;j++)
                                tempInstV[j].resize(DIM);*/
                            int k = end3+1;
                            for(int j=0;j!=range1+range2+range3;j++)
                            {
                                tempInst[j] = instNEStack[k];
                                tempInstI[j] = instNEConstIStack[k];
                                tempInstF[j] = instNEConstFStack[k];
                                tempInstS[j] = instNEConstSStack[k];
                                tempInstV[j] = instNEConstVStack[k];
                                k++;
                            }

                            k = end3+1;
                            instNEStack[k] = 10001;
                            for(int j=0;j!=range2+range3;j++)
                            {
                                k++;
                                instNEStack[k] = tempInst[j];
                                instNEConstIStack[k] = tempInstI[j];
                                instNEConstFStack[k] = tempInstF[j];
                                instNEConstSStack[k] = tempInstS[j];
                                instNEConstVStack[k] = tempInstV[j];
                            }
                            k++;
                            instNEStack[k] = 10000;

                            if(k >= NESize-1)
                            {
                                int nadd = k-NESize+2;
                                for(int L=0;L!=nadd;L++)
                                {
                                    NESize++;
                                    updateNESize();
                                }
                            }
                            for(int j=0;j!=range3;j++)
                            {
                                k++;
                                instNEStack[k] = tempInst[j];
                                instNEConstIStack[k] = tempInstI[j];
                                instNEConstFStack[k] = tempInstF[j];
                                instNEConstSStack[k] = tempInstS[j];
                                instNEConstVStack[k] = tempInstV[j];
                                if(k >= NESize-1)
                                {
                                    NESize++;
                                    updateNESize();
                                }
                            }

                            for(int j=range2+range3;j!=range1+range2+range3;j++)
                            {
                                k++;
                                instNEStack[k] = tempInst[j];
                                instNEConstIStack[k] = tempInstI[j];
                                instNEConstFStack[k] = tempInstF[j];
                                instNEConstSStack[k] = tempInstS[j];
                                instNEConstVStack[k] = tempInstV[j];
                                if(k >= NESize-1)
                                {
                                    NESize++;
                                    updateNESize();
                                }
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        else
                        {
                            if(verbose)
                                cout<<"EXEC S NOT ENOUGH 2"<<endl;
                        }
                    }
                    else
                    {
                        if(verbose)
                            cout<<"EXEC S NOT ENOUGH 1"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8016:
            {
                if(NESize > 1)
                {
                    if(verbose)
                        cout<<"EXEC Y COMBINATOR"<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= -1)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNEStack[j];
                            tempInstI[j-end1-1] = instNEConstIStack[j];
                            tempInstF[j-end1-1] = instNEConstFStack[j];
                            tempInstS[j-end1-1] = instNEConstSStack[j];
                            tempInstV[j-end1-1] = instNEConstVStack[j];
                        }
                        int k = end1+1;
                        instNEStack[k] = 10001;
                        k++;
                        for(int j=0;j!=range1;j++)
                        {
                            instNEStack[k] = tempInst[j];
                            instNEConstIStack[k] = tempInstI[j];
                            instNEConstFStack[k] = tempInstF[j];
                            instNEConstSStack[k] = tempInstS[j];
                            instNEConstVStack[k] = tempInstV[j];
                            k++;
                        }
                        instNEStack[k] = 8016;
                        k++;
                        NESize++;
                        updateNESize();
                        instNEStack[k] = 10000;
                        k++;
                        NESize++;
                        updateNESize();
                        for(int j=0;j!=range1;j++)
                        {
                            instNEStack[NESize] = tempInst[j];
                            instNEConstIStack[NESize] = tempInstI[j];
                            instNEConstFStack[NESize] = tempInstF[j];
                            instNEConstSStack[NESize] = tempInstS[j];
                            instNEConstVStack[NESize] = tempInstV[j];
                            NESize++;
                            updateNESize();
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        NESize++;
                    }
                    else
                    {
                        if(verbose)
                            cout<<"EXEC Y NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 8017:
            {
                if(NESize > 2 && boolSize > 0)
                {
                    if(verbose)
                        cout<<"EXEC IF"<<endl;
                    int start1 = NESize-2;
                    int end1 = NESize-3;
                    int start2;
                    int end2;
                    if(instNEStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNEStack[j] == 10000);
                            bracketCounter-=(instNEStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        start2 = end1;
                        end2 = start2-1;
                        if(instNEStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNEStack[j] == 10000);
                                bracketCounter-=(instNEStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        int range1 = start1 - end1;
                        int range2 = start2 - end2;
                        if(boolStack[boolSize-1] == true)
                        {
                            int k = end2+1;
                            for(int j=0;j!=range1;j++)
                            {
                                instNEStack[k] = instNEStack[k+range2];
                                instNEConstIStack[k] = instNEConstIStack[k+range2];
                                instNEConstFStack[k] = instNEConstFStack[k+range2];
                                instNEConstSStack[k] = instNEConstSStack[k+range2];
                                instNEConstVStack[k] = instNEConstVStack[k+range2];
                                k++;
                            }
                            NESize -= range2;
                        }
                        else
                        {
                            NESize -= range1;
                        }
                        boolSize--;
                    }
                    else
                    {
                        if(verbose)
                            cout<<"EXEC K NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9000:
            {
                if(NISize > 1)
                {
                    if(verbose)
                        cout<<"CODE ="<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    int start2 = end1;
                    int end2 = start2-1;
                    int range2 = 1;
                    if(end1 >= 0)
                    {
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        range2 = start2 - end2;
                        int neq = 0;
                        bool eq2 = false;
                        if(range1 == range2)
                        {
                            for(int j=0;j<range1;j++)
                            {
                                eq2 = (instNIStack[end1+j+1] == instNIStack[end2+j+1] && instNIStack[end1+j+1] > 2000 && instNIStack[end1+j+1] != 7000
                                       && instNIStack[end1+j+1] != 11000) ||
                                          (instNIStack[end1+j+1] == 1000 && instNIConstFStack[end1+j+1] == instNIConstFStack[end2+j+1]) ||
                                          (instNIStack[end1+j+1] == 2000 && instNIConstIStack[end1+j+1] == instNIConstIStack[end2+j+1]) ||
                                          (instNIStack[end1+j+1] == 7000 && instNIConstSStack[end1+j+1] == instNIConstSStack[end2+j+1]);
                                int neq2 = 0;
                                if(instNIStack[end1+j+1] == 11000)
                                {
                                    for(int d=0;d!=DIM;d++)
                                    {
                                        if(instNIConstVStack[end1+j+1][d] == instNIConstVStack[end2+j+1][d])
                                            neq2++;
                                    }
                                    eq2 = eq2 || (neq2 == DIM);
                                }
                                neq += eq2;
                            }
                        }
                        boolStack[boolSize] = (neq == range1);
                        boolSize++;
                        updateBoolSize();
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9001:
            {
                if(NISize > 1) ///TODO
                {
                    if(verbose)
                        cout<<"CODE APPEND"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    int start2 = end1;
                    int end2 = start2-1;
                    int range2 = 1;
                    if(end1 >= 0)
                    {
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        range2 = start2 - end2;
                        int newsize = range1 + range2 + 2;
                        /*int* tempInst = new int[newsize];
                        int* tempInstI = new int[newsize];
                        double* tempInstF = new double[newsize];
                        string* tempInstS = new string[newsize];
                        vector<double>* tempInstV = new vector<double>[newsize];*/
                        /*vector<int> tempInst(newsize);
                        vector<int> tempInstI(newsize);
                        vector<double> tempInstF(newsize);
                        vector<string> tempInstS(newsize);
                        vector<vector<double> > tempInstV(newsize);
                        for(int j=0;j!=newsize;j++)
                            tempInstV[j].resize(DIM);*/
                        int k = 0;
                        if(range2 == 1)
                        {
                            tempInst[k] = 10001;
                            k++;
                        }
                        for(int j=0;j!=range2;j++)
                        {
                            tempInst[k] = instNIStack[end2+1+j];
                            tempInstI[k] = instNIConstIStack[end2+1+j];
                            tempInstF[k] = instNIConstFStack[end2+1+j];
                            tempInstS[k] = instNIConstSStack[end2+1+j];
                            tempInstV[k] = instNIConstVStack[end2+1+j];
                            k++;
                        }
                        k-=(range2 != 1);
                        int shift = !(range1 == 1);
                        for(int j=0;j!=range1-shift;j++)
                        {
                            tempInst[k] = instNIStack[end1+j+shift+1];
                            tempInstI[k] = instNIConstIStack[end1+j+shift+1];
                            tempInstF[k] = instNIConstFStack[end1+j+shift+1];
                            tempInstS[k] = instNIConstSStack[end1+j+shift+1];
                            tempInstV[k] = instNIConstVStack[end1+j+shift+1];
                            k++;
                        }
                        if(!shift)
                        {
                            tempInst[k] = 10000;
                            k++;
                        }
                        newsize = k;
                        if(end2+1+newsize < NISize)
                            NISize = end2+1+newsize;
                        k = end2+1;
                        for(int j=0;j!=newsize;j++)
                        {
                            instNIStack[k] = tempInst[j];
                            instNIConstIStack[k] = tempInstI[j];
                            instNIConstFStack[k] = tempInstF[j];
                            instNIConstSStack[k] = tempInstS[j];
                            instNIConstVStack[k] = tempInstV[j];
                            k++;
                            if(k > NISize-1)
                            {
                                NISize++;
                                updateNISize();
                            }
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9002:
            {
                if(NISize > 1)
                {
                    if(verbose)
                        cout<<"CODE ATOM"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    boolStack[boolSize] = (bracketCounter == 0) && (start1-end1 == 1);
                    boolSize++;
                    updateBoolSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9003:
            {
                if(NISize > 1)///TODO
                {
                    if(verbose)
                        cout<<"CODE CAR"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    if((bracketCounter == 0) && (start1-end1 > 1)) //NOT ATOM
                    {
                        int range1 = start1 - end1;
                        int start2 = start1-1;
                        int end2 = start1-2;
                        int range2 = 1;
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=end1;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        range2 = start2 - end2;
                        for(int j=0;j!=range2;j++)
                        {
                            instNIStack[end1+1+j] = instNIStack[end2+1+j];
                            instNIConstIStack[end1+1+j] = instNIConstIStack[end2+1+j];
                            instNIConstFStack[end1+1+j] = instNIConstFStack[end2+1+j];
                            instNIConstSStack[end1+1+j] = instNIConstSStack[end2+1+j];
                            instNIConstVStack[end1+1+j] = instNIConstVStack[end2+1+j];
                        }
                        NISize += (range2-range1-1);
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9004:
            {
                if(NISize > 1)///TODO
                {
                    if(verbose)
                        cout<<"CODE CDR"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    if((bracketCounter == 0) && (start1-end1 > 1)) //NOT ATOM
                    {
                        int start2 = start1-1;
                        int end2 = start1-2;
                        int range2 = 1;
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=end1;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        range2 = start2 - end2;
                        int range3 = end2 - end1-1;
                        if(range3 > 0)
                        {
                            for(int j=0;j!=range3;j++)
                            {
                                instNIStack[end1+1+j] = instNIStack[end1+2+j];
                                instNIConstIStack[end1+1+j] = instNIConstIStack[end1+2+j];
                                instNIConstFStack[end1+1+j] = instNIConstFStack[end1+2+j];
                                instNIConstSStack[end1+1+j] = instNIConstSStack[end1+2+j];
                                instNIConstVStack[end1+1+j] = instNIConstVStack[end1+2+j];
                            }
                            NISize += (-range2-3);
                        }
                        else
                        {
                            instNIStack[end1+1] = 10001;
                            instNIStack[end1+2] = 10000;
                            NISize -= (range1-1);
                        }
                    }
                    else if((bracketCounter == 0) && (start1-end1 == 1)) //ATOM
                    {
                        instNIStack[end1+1] = 10001;
                        instNIStack[end1+2] = 10000;
                        NISize -= (range1-1);
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9005:
            {
                if(NISize > 2 && false)///TODO
                {
                    if(verbose)
                cout<<"CODE CONS"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    if((bracketCounter == 0) && (start1-end1 > 1)) //NOT ATOM
                    {
                        int start2 = end1;
                        int end2 = end1-1;
                        int range2 = 1;
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end2 > -2)
                        {
                            range2 = start2 - end2;
                            /*int* tempInst = new int[range2];
                            int* tempInstI = new int[range2];
                            double* tempInstF = new double[range2];
                            string* tempInstS = new string[range2];
                            vector<double>* tempInstV = new vector<double>[range2];*/
                            /*vector<int> tempInst(range2);
                            vector<int> tempInstI(range2);
                            vector<double> tempInstF(range2);
                            vector<string> tempInstS(range2);
                            vector<vector<double> > tempInstV(range2);
                            for(int j=0;j!=range2;j++)
                                tempInstV[j].resize(DIM);*/
                            int k = 0;
                            for(int j=0;j!=range2;j++)
                            {
                                tempInst[k] = instNIStack[end2+1+j];
                                tempInstI[k] = instNIConstIStack[end2+1+j];
                                tempInstF[k] = instNIConstFStack[end2+1+j];
                                tempInstS[k] = instNIConstSStack[end2+1+j];
                                tempInstV[k] = instNIConstVStack[end2+1+j];
                                k++;
                            }
                            for(int j=0;j!=range1-1;j++)
                            {
                                instNIStack[end1-range2+1+j] = instNIStack[end1+1+j];
                                instNIConstIStack[end1-range2+1+j] = instNIConstIStack[end1+1+j];
                                instNIConstFStack[end1-range2+1+j] = instNIConstFStack[end1+1+j];
                                instNIConstSStack[end1-range2+1+j] = instNIConstSStack[end1+1+j];
                                instNIConstVStack[end1-range2+1+j] = instNIConstVStack[end1+1+j];
                            }
                            k = start1 - range2;
                            for(int j=0;j!=range2;j++)
                            {
                                instNIStack[k] = tempInst[j];
                                instNIConstIStack[k] = tempInstI[j];
                                instNIConstFStack[k] = tempInstF[j];
                                instNIConstSStack[k] = tempInstS[j];
                                instNIConstVStack[k] = tempInstV[j];
                                k++;
                            }
                            NISize--;
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                    }
                    else if((bracketCounter == 0) && (start1-end1 == 1)) //ATOM
                    {
                        int start2 = end1;
                        int end2 = end1-1;
                        int range2 = 1;
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        range2 = start2 - end2;
                        /*int* tempInst = new int[range2];
                        int* tempInstI = new int[range2];
                        double* tempInstF = new double[range2];
                        string* tempInstS = new string[range2];
                        vector<double>* tempInstV = new vector<double>[range2];*/
                        /*vector<int> tempInst(range2);
                        vector<int> tempInstI(range2);
                        vector<double> tempInstF(range2);
                        vector<string> tempInstS(range2);
                        vector<vector<double> > tempInstV(range2);
                        for(int j=0;j!=range2;j++)
                            tempInstV[j].resize(DIM);*/
                        int k = 0;
                        for(int j=0;j!=range2;j++)
                        {
                            tempInst[k] = instNIStack[end2+1+j];
                            tempInstI[k] = instNIConstIStack[end2+1+j];
                            tempInstF[k] = instNIConstFStack[end2+1+j];
                            tempInstS[k] = instNIConstSStack[end2+1+j];
                            tempInstV[k] = instNIConstVStack[end2+1+j];
                            k++;
                        }
                        instNIStack[end2+1] = 10001;
                        k = end2+2;
                        for(int j=0;j!=range1;j++)
                        {
                            instNIStack[k] = instNIStack[end1+1+j];
                            instNIConstIStack[k] = instNIConstIStack[end1+1+j];
                            instNIConstFStack[k] = instNIConstFStack[end1+1+j];
                            instNIConstSStack[k] = instNIConstSStack[end1+1+j];
                            instNIConstVStack[k] = instNIConstVStack[end1+1+j];
                            k++;
                        }
                        for(int j=0;j!=range2;j++)
                        {
                            instNIStack[k] = tempInst[j];
                            instNIConstIStack[k] = tempInstI[j];
                            instNIConstFStack[k] = tempInstF[j];
                            instNIConstSStack[k] = tempInstS[j];
                            instNIConstVStack[k] = tempInstV[j];
                            k++;
                            if(k == NISize)
                            {
                                NISize++;
                                updateNISize();
                            }
                        }
                        instNIStack[k] = 10000;
                        k++;
                        if(k > NISize)
                        {
                            NISize++;
                            updateNISize();
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9006:
            {
                if(verbose)
                cout<<"CODE CONTAINER TODO"<<endl;
                NNOOPS++;
            } break;
        case 9007:
            {
                if(NISize > 2)
                {
                    if(verbose)
                        cout<<"CODE CONTAINS"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    int start2 = end1;
                    int end2 = end1-1;
                    int range2 = 1;
                    if(instNIStack[start2] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start2;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end2 = j-1;
                                break;
                            }
                        }
                    }
                    range2 = start2 - end2;
                    bool foundeq = false;
                    if(range2 >= range1) // may contain
                    {
                        for(int j=start2;j!=0;j--)
                        {
                            int neq = 0;
                            bool eq2 = false;
                            for(int k=0;k<range1;k++)
                            {
                                /*eq2 = (instNIStack[j-k] == instNIStack[start1-k] && instNIStack[j-k] > 2000 && instNIStack[j-k] != 7000) ||
                                          (instNIStack[j-k] == 1000 && instNIConstFStack[j-k] == instNIConstFStack[start1-k]) ||
                                          (instNIStack[j-k] == 2000 && instNIConstIStack[j-k] == instNIConstIStack[start1-k]) ||
                                          (instNIStack[j-k] == 7000 && instNIConstSStack[j-k] == instNIConstSStack[start1-k]);
                                neq += eq2;*/

                                eq2 = (instNIStack[j-k] == instNIStack[start1-k] && instNIStack[j-k] > 2000 && instNIStack[j-k] != 7000
                                       && instNIStack[j-k] != 11000) ||
                                          (instNIStack[j-k] == 1000 && instNIConstFStack[j-k] == instNIConstFStack[start1-k]) ||
                                          (instNIStack[j-k] == 2000 && instNIConstIStack[j-k] == instNIConstIStack[start1-k]) ||
                                          (instNIStack[j-k] == 7000 && instNIConstSStack[j-k] == instNIConstSStack[start1-k]);
                                int neq2 = 0;
                                if(instNIStack[j-k] == 11000)
                                {
                                    for(int d=0;d!=DIM;d++)
                                    {
                                        if(instNIConstVStack[j-k][d] == instNIConstVStack[start1-k][d])
                                            neq2++;
                                    }
                                    eq2 = eq2 || (neq2 == DIM);
                                }
                                neq += eq2;

                                if(j-k < 0)
                                    break;
                            }
                            if(neq == range1)
                            {
                                foundeq = true;
                                break;
                            }
                        }
                    }
                    boolStack[boolSize] = foundeq;
                    boolSize++;
                    updateBoolSize();
                    NISize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9008:
            {
                if(nameSize > 0 && NISize > 1)
                {
                    if(verbose)
                        cout<<"CODE DEFINE"<<endl;
                    vector<atomInst> subProg;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1-end1;
                    int k = end1+1;
                    for(int j=0;j!=range1;j++)
                    {
                        atomInst b;
                        b.inst = instNIStack[k];
                        b.i = instNIConstIStack[k];
                        b.f = instNIConstFStack[k];
                        b.n = instNIConstSStack[k];
                        b.v = instNIConstVStack[k];
                        subProg.push_back(b);
                        k++;
                    }
                    if(verbose)
                cout<<printSubProg(subProg)<<endl;
                    names_map[nameStack[nameSize-1]] = subProg;
                    NISize-=range1;
                    nameSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9009:
            {
                if(nameSize > 0 && false)///TODO
                {
                    if(names_map.contains(nameStack[nameSize-1]))
                    {
                        if(verbose)
                            cout<<"CODE DEFINITION"<<endl;
                        vector<atomInst> subProg = names_map[nameStack[nameSize-1]];
                        int Len = subProg.size();
                        for(int j=0;j!=Len;j++)
                        {
                            instNIStack[NISize] = subProg[j].inst;
                            instNIConstIStack[NISize] = subProg[j].i;
                            instNIConstFStack[NISize] = subProg[j].f;
                            instNIConstSStack[NISize] = subProg[j].n;
                            instNIConstVStack[NISize] = subProg[j].v;
                            NISize++;
                            updateNISize();
                        }
                        nameSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9010:
            {
                if(verbose)
                    cout<<"CODE DISCREPANCY TODO"<<endl;
                NNOOPS++;
            } break;
        case 9011:
            {
                if(NISize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE DO"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    instNEStack[NESize-1] = 9033;
                    NESize++;
                    updateNESize();
                    for(int j=0;j!=range1;j++)
                    {
                        instNEStack[NESize-1] = instNIStack[end1+1+j];
                        instNEConstFStack[NESize-1] = instNIConstFStack[end1+1+j];
                        instNEConstIStack[NESize-1] = instNIConstIStack[end1+1+j];
                        instNEConstSStack[NESize-1] = instNIConstSStack[end1+1+j];
                        instNEConstVStack[NESize-1] = instNIConstVStack[end1+1+j];
                        NESize++;
                        updateNESize();
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9012:
            {
                if(NISize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE DO*"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    for(int j=0;j!=range1;j++)
                    {
                        instNEStack[NESize-1] = instNIStack[end1+1+j];
                        instNEConstFStack[NESize-1] = instNIConstFStack[end1+1+j];
                        instNEConstIStack[NESize-1] = instNIConstIStack[end1+1+j];
                        instNEConstSStack[NESize-1] = instNIConstSStack[end1+1+j];
                        instNEConstVStack[NESize-1] = instNIConstVStack[end1+1+j];
                        NESize++;
                        updateNESize();
                    }
                    instNEStack[NESize-1] = 9033;
                    NESize++;
                    updateNESize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9013:
            {
                if(NISize > 0 && intSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE DO*COUNT"<<endl;
                    if(intStack[intSize-1] > 0)
                    {
                        int NLoop = intStack[intSize-1];
                        NLoop = min(NLoopLimit,NLoop);
                        NLoop = max(0,NLoop);
                        int start1 = NISize-1;
                        int end1 = NISize-2;
                        if(instNIStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            int range1 = start1 - end1;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNIStack[j];
                                tempInstI[j-end1-1] = instNIConstIStack[j];
                                tempInstF[j-end1-1] = instNIConstFStack[j];
                                tempInstS[j-end1-1] = instNIConstSStack[j];
                                tempInstV[j-end1-1] = instNIConstVStack[j];
                            }
                            NISize-=range1;
                            for(int L=0;L!=NLoop;L++)
                            {
                                for(int j=0;j!=range1;j++)
                                {
                                    instNEStack[NESize-1] = tempInst[j];
                                    instNEConstIStack[NESize-1] = tempInstI[j];
                                    instNEConstFStack[NESize-1] = tempInstF[j];
                                    instNEConstSStack[NESize-1] = tempInstS[j];
                                    instNEConstVStack[NESize-1] = tempInstV[j];
                                    NESize++;
                                    updateNESize();
                                    if(NESize >= StackSizeLimit)
                                        break;
                                }
                                instNEStack[NESize-1] = 2000;
                                instNEConstIStack[NESize-1] = NLoop-L-1;
                                NESize++;
                                updateNESize();
                                if(NESize >= StackSizeLimit)
                                    break;
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9014:
            {
                if(NISize > 0 && intSize > 1 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE DO*RANGE"<<endl;
                    if(intStack[intSize-1] != intStack[intSize-2])
                    {
                        int NLoop = abs(intStack[intSize-1] - intStack[intSize-2]);
                        NLoop = min(NLoopLimit,NLoop);
                        NLoop = max(0,NLoop);
                        int LStart = intStack[intSize-2];
                        int LStop = intStack[intSize-1];
                        int dir = 2*(LStop - LStart > 0)-1;
                        int start1 = NISize-1;
                        int end1 = NISize-2;
                        if(instNIStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            int range1 = start1 - end1;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNIStack[j];
                                tempInstI[j-end1-1] = instNIConstIStack[j];
                                tempInstF[j-end1-1] = instNIConstFStack[j];
                                tempInstS[j-end1-1] = instNIConstSStack[j];
                                tempInstV[j-end1-1] = instNIConstVStack[j];
                            }
                            NISize-=range1;
                            for(int L=0;L!=NLoop;L++)
                            {
                                for(int j=0;j!=range1;j++)
                                {
                                    instNEStack[NESize-1] = tempInst[j];
                                    instNEConstIStack[NESize-1] = tempInstI[j];
                                    instNEConstFStack[NESize-1] = tempInstF[j];
                                    instNEConstSStack[NESize-1] = tempInstS[j];
                                    instNEConstVStack[NESize-1] = tempInstV[j];
                                    NESize++;
                                    updateNESize();
                                    if(NESize >= StackSizeLimit)
                                        break;
                                }
                                instNEStack[NESize-1] = 2000;
                                instNEConstIStack[NESize-1] = LStart + dir*(L+1);
                                NESize++;
                                updateNESize();
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        intSize-=2;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9015:
            {
                if(NISize > 0 && intSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE DO*TIMES"<<endl;
                    if(intStack[intSize-1] > 0)
                    {
                        int NLoop = intStack[intSize-1];
                        NLoop = min(NLoopLimit,NLoop);
                        NLoop = max(0,NLoop);
                        int start1 = NISize-1;
                        int end1 = NISize-2;
                        if(instNIStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            int range1 = start1 - end1;
                            /*int* tempInst = new int[range1];
                            int* tempInstI = new int[range1];
                            double* tempInstF = new double[range1];
                            string* tempInstS = new string[range1];
                            vector<double>* tempInstV = new vector<double>[range1];*/
                            /*vector<int> tempInst(range1);
                            vector<int> tempInstI(range1);
                            vector<double> tempInstF(range1);
                            vector<string> tempInstS(range1);
                            vector<vector<double> > tempInstV(range1);
                            for(int j=0;j!=range1;j++)
                                tempInstV[j].resize(DIM);*/
                            for(int j=start1;j!=end1;j--)
                            {
                                tempInst[j-end1-1] = instNIStack[j];
                                tempInstI[j-end1-1] = instNIConstIStack[j];
                                tempInstF[j-end1-1] = instNIConstFStack[j];
                                tempInstS[j-end1-1] = instNIConstSStack[j];
                                tempInstV[j-end1-1] = instNIConstVStack[j];
                            }
                            NISize-=range1;
                            for(int L=0;L!=NLoop;L++)
                            {
                                for(int j=0;j!=range1;j++)
                                {
                                    instNEStack[NESize-1] = tempInst[j];
                                    instNEConstIStack[NESize-1] = tempInstI[j];
                                    instNEConstFStack[NESize-1] = tempInstF[j];
                                    instNEConstSStack[NESize-1] = tempInstS[j];
                                    instNEConstVStack[NESize-1] = tempInstV[j];
                                    NESize++;
                                    updateNESize();
                                    if(NESize >= StackSizeLimit)
                                        break;
                                }
                            }
                            /*delete tempInst;
                            delete tempInstI;
                            delete tempInstF;
                            delete[] tempInstS;
                            delete[] tempInstV;*/
                        }
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9016:
            {
                if(NISize > 1 && false)///TODO
                {
                    if(instNIStack[NISize-1] == 10000 && NISize > 1)
                    {
                        if(verbose)
                            cout<<"CODE DUP SUBPROG"<<endl;
                        bracketCounter=0;
                        int pairIndex = -1;
                        for(int j=NISize-2;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                pairIndex = j;
                                break;
                            }
                        }
                        if(pairIndex != -1)
                        {
                            int tmpIndex = NISize;
                            NISize -= 1;
                            for(int j=pairIndex;j<tmpIndex;j++)
                            {
                                instNIStack[NISize] = instNIStack[j];
                                instNIConstFStack[NISize] = instNIConstFStack[j];
                                instNIConstIStack[NISize] = instNIConstIStack[j];
                                instNIConstSStack[NISize] = instNIConstSStack[j];
                                instNIConstVStack[NISize] = instNIConstVStack[j];
                                NISize++;
                                updateNISize();
                            }
                        }
                        else
                            if(verbose)
                                cout<<"NO MATCHING CLOSE BRACKET"<<endl;
                    }
                    else
                    {
                        if(verbose)
                            cout<<"CODE DUP"<<endl;
                        instNIStack[NISize-1] = instNIStack[NISize-2];
                        instNIConstFStack[NISize-1] = instNIConstFStack[NISize-2];
                        instNIConstIStack[NISize-1] = instNIConstIStack[NISize-2];
                        instNIConstSStack[NISize-1] = instNIConstSStack[NISize-2];
                        instNIConstVStack[NISize-1] = instNIConstVStack[NISize-2];
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9017:
            {
                if(verbose)
                    cout<<"CODE EXTRACT TODO"<<endl;
                NNOOPS++;
            } break;
        case 9018:
            {
                if(NISize > 0)
                {
                    if(verbose)
                        cout<<"CODE FLUSH"<<endl;
                    NISize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9019:
            {
                if(boolSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE FROM BOOL"<<endl;
                    instNIStack[NISize] = 3000 + (boolStack[boolSize-1] == true);
                    boolSize--;
                    NISize++;
                    updateNISize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9020:
            {
                if(floatSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE FROM FLOAT"<<endl;
                    instNIStack[NISize] = 1000;
                    instNIConstFStack[NISize] = floatStack[floatSize-1];
                    floatSize--;
                    NISize++;
                    updateNISize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9021:
            {
                if(intSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE FROM INT"<<endl;
                    instNIStack[NISize] = 2000;
                    instNIConstIStack[NISize] = intStack[intSize-1];
                    intSize--;
                    NISize++;
                    updateNISize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9022:
            {
                if(nameSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE FROM NAME"<<endl;
                    instNIStack[NISize] = 7000;
                    instNIConstSStack[NISize] = nameStack[nameSize-1];
                    nameSize--;
                    NISize++;
                    updateNISize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9023:
            {
                if(NISize > 2 && boolSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE IF"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    int start2;
                    int end2;
                    if(instNIStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        start2 = end1;
                        end2 = start2-1;
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        int range1 = start1 - end1;
                        int range2 = start2 - end2;
                        if(boolStack[boolSize-1] == true)
                        {
                            int k = end2+1;
                            for(int j=0;j!=range1;j++)
                            {
                                instNIStack[k] = instNIStack[k+range2];
                                instNIConstIStack[k] = instNIConstIStack[k+range2];
                                instNIConstFStack[k] = instNIConstFStack[k+range2];
                                instNIConstSStack[k] = instNIConstSStack[k+range2];
                                instNIConstVStack[k] = instNIConstVStack[k+range2];
                                k++;
                            }
                            NISize -= range2;
                        }
                        else
                        {
                            NISize -= range1;
                        }
                        boolSize--;
                    }
                    else
                    {
                        if(verbose)
                            cout<<"CODE K NOT ENOUGH"<<endl;
                        NNOOPS++;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9024:
            {
                if(verbose)
                    cout<<"CODE INSERT TODO"<<endl;
                NNOOPS++;
            } break;
        case 9025:
            {
                if(verbose)
                    cout<<"CODE INSTRUCTIONS TODO"<<endl;
                NNOOPS++;
            } break;
        case 9026:
            {
                if(verbose)
                    cout<<"CODE LENGTH TODO"<<endl;
                NNOOPS++;
            } break;
        case 9027:
            {
                if(verbose)
                    cout<<"CODE LIST TODO"<<endl;
                NNOOPS++;
            } break;
        case 9028:
            {
                if(verbose)
                    cout<<"CODE MEMBER TODO"<<endl;
                NNOOPS++;
            } break;
        case 9029:
            {
                if(verbose)
                    cout<<"CODE NOOP"<<endl;
                NNOOPS++;
            } break;
        case 9030:
            {
                if(verbose)
                    cout<<"CODE NTH TODO"<<endl;
                NNOOPS++;
            } break;
        case 9031:
            {
                if(verbose)
                    cout<<"CODE NTHCDR TODO"<<endl;
                NNOOPS++;
            } break;
        case 9032:
            {
                if(NISize > 0)
                {
                    if(verbose)
                        cout<<"CODE NULL"<<endl;
                    if(NISize == 1)
                    {
                        boolStack[boolSize] = false;
                        boolSize++;
                        updateBoolSize();
                    }
                    else
                    {
                        int start1 = NISize-1;
                        int end1 = NISize-2;
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                        int range1 = start1 - end1;
                        if(bracketCounter == 0 && range1 == 2)
                        {
                            boolStack[boolSize] = true;
                        }
                        else
                        {
                            boolStack[boolSize] = false;
                        }
                        boolSize++;
                        updateBoolSize();
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9033:
            {
                if(NISize > 0)
                {
                    if(verbose)
                        cout<<"CODE POP"<<endl;
                    if(instNIStack[NISize-1] == 10000)
                    {
                        int start1 = NISize-1;
                        int end1 = NISize-2;
                        if(instNIStack[start1] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start1;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end1 = j-1;
                                    break;
                                }
                            }
                        }
                        if(end1 >= -1)
                        {
                            NISize -= (start1-end1);
                        }
                    }
                    else
                    {
                        NISize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9034:
            {
                if(NISize > 2)
                {
                    if(verbose)
                        cout<<"CODE POSITION"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNIStack[j] == 10000);
                        bracketCounter-=(instNIStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    int start2 = end1;
                    int end2 = end1-1;
                    int range2 = 1;
                    if(instNIStack[start2] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start2;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end2 = j-1;
                                break;
                            }
                        }
                    }
                    range2 = start2 - end2;
                    //bool foundeq = false;
                    int pos = -1;
                    if(range2 >= range1) // may contain
                    {
                        for(int j=start2;j!=0;j--)
                        {
                            int neq = 0;
                            bool eq2 = false;
                            for(int k=0;k<range1;k++)
                            {
                                /*eq2 = (instNIStack[j-k] == instNIStack[start1-k] && instNIStack[j-k] > 2000 && instNIStack[j-k] != 7000) ||
                                          (instNIStack[j-k] == 1000 && instNIConstFStack[j-k] == instNIConstFStack[start1-k]) ||
                                          (instNIStack[j-k] == 2000 && instNIConstIStack[j-k] == instNIConstIStack[start1-k]) ||
                                          (instNIStack[j-k] == 7000 && instNIConstSStack[j-k] == instNIConstSStack[start1-k]);
                                neq += eq2;*/

                                eq2 = (instNIStack[j-k] == instNIStack[start1-k] && instNIStack[j-k] > 2000 && instNIStack[j-k] != 7000
                                       && instNIStack[j-k] != 11000) ||
                                          (instNIStack[j-k] == 1000 && instNIConstFStack[j-k] == instNIConstFStack[start1-k]) ||
                                          (instNIStack[j-k] == 2000 && instNIConstIStack[j-k] == instNIConstIStack[start1-k]) ||
                                          (instNIStack[j-k] == 7000 && instNIConstSStack[j-k] == instNIConstSStack[start1-k]);
                                int neq2 = 0;
                                if(instNIStack[j-k] == 11000)
                                {
                                    for(int d=0;d!=DIM;d++)
                                    {
                                        if(instNIConstVStack[j-k][d] == instNIConstVStack[start1-k][d])
                                            neq2++;
                                    }
                                    eq2 = eq2 || (neq2 == DIM);
                                }
                                neq += eq2;

                                if(j-k < 0)
                                    break;
                            }
                            if(neq == range1)
                            {
                                //foundeq = true;
                                pos = NISize-j;
                                break;
                            }
                        }
                    }
                    intStack[intSize] = pos;
                    intSize++;
                    updateIntSize();
                    NISize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9035:
            {
                if(NESize > 0 && false) ///TODO
                {
                    if(verbose)
                        cout<<"CODE QUOTE"<<endl;
                    int start1 = NESize-1;
                    int end1 = NESize-2;
                    bracketCounter = 0;
                    for(int j=start1;j>=0;j--)
                    {
                        bracketCounter+=(instNEStack[j] == 10000);
                        bracketCounter-=(instNEStack[j] == 10001);
                        if(bracketCounter == 0)
                        {
                            end1 = j-1;
                            break;
                        }
                    }
                    int range1 = start1 - end1;
                    for(int j=0;j!=range1;j++)
                    {
                        instNIStack[NISize] = instNEStack[end1+1+j];
                        instNIConstIStack[NISize] = instNEConstIStack[end1+1+j];
                        instNIConstFStack[NISize] = instNEConstFStack[end1+1+j];
                        instNIConstSStack[NISize] = instNEConstSStack[end1+1+j];
                        instNIConstVStack[NISize] = instNEConstVStack[end1+1+j];
                        NISize++;
                        updateNISize();
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9036:
            {
                if(verbose)
                    cout<<"CODE RAND TODO"<<endl;
                NNOOPS++;
            } break;
        case 9037:
            {
                if(NISize > 1 && intSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 1)
                        index_shove = 1;
                    if(index_shove > NISize-1)
                        index_shove = NISize-1;
                    index_shove = NISize-2-index_shove;

                    int start1 = NISize-2;
                    int end1 = NISize-3;
                    if(instNIStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNIStack[j];
                            tempInstI[j-end1-1] = instNIConstIStack[j];
                            tempInstF[j-end1-1] = instNIConstFStack[j];
                            tempInstS[j-end1-1] = instNIConstSStack[j];
                            tempInstV[j-end1-1] = instNIConstVStack[j];
                        }

                        if(index_shove > end1 && index_shove <= start1)
                        {
                            //insert
                            if(verbose)
                                cout<<"insert"<<endl;
                            int nadd = range1 - (start1 - index_shove);
                            for(int j=0;j<range1;j++)
                            {
                                instNIStack[NISize-1] = tempInst[j];
                                instNIConstIStack[NISize-1] = tempInstI[j];
                                instNIConstFStack[NISize-1] = tempInstF[j];
                                instNIConstSStack[NISize-1] = tempInstS[j];
                                instNIConstVStack[NISize-1] = tempInstV[j];
                                NISize++;
                                updateNISize();
                            }
                            int k = start1 + nadd;
                            for(int j=0;j!=range1;j++)
                            {
                                instNIStack[k] = tempInst[range1-1-j];
                                instNIConstIStack[k] = tempInstI[range1-1-j];
                                instNIConstFStack[k] = tempInstF[range1-1-j];
                                instNIConstSStack[k] = tempInstS[range1-1-j];
                                instNIConstVStack[k] = tempInstV[range1-1-j];
                                k--;
                            }
                            if(verbose)
                                cout<<"end insert"<<endl;
                        }
                        else
                        {
                            //swap
                            int k = NISize - 2;
                            for(int j=end1;j!=index_shove;j--)
                            {
                                instNIStack[k] = instNIStack[j];
                                instNIConstIStack[k] = instNIConstIStack[j];
                                instNIConstFStack[k] = instNIConstFStack[j];
                                instNIConstSStack[k] = instNIConstSStack[j];
                                instNIConstVStack[k] = instNIConstVStack[j];
                                k--;
                            }
                            for(int j=range1-1;j>=0;j--)
                            {
                                instNIStack[k] = tempInst[j];
                                instNIConstIStack[k] = tempInstI[j];
                                instNIConstFStack[k] = tempInstF[j];
                                instNIConstSStack[k] = tempInstS[j];
                                instNIConstVStack[k] = tempInstV[j];
                                k--;
                            }
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;;
        case 9038:
            {
                if(verbose)
                    cout<<"CODE SIZE"<<endl;
                int start1 = NISize-1;
                int end1 = NISize-1-(NISize > 0);
                bracketCounter = 0;
                for(int j=start1;j>=0;j--)
                {
                    bracketCounter+=(instNIStack[j] == 10000);
                    bracketCounter-=(instNIStack[j] == 10001);
                    if(bracketCounter == 0)
                    {
                        end1 = j-1;
                        break;
                    }
                }
                intStack[intSize] = start1 - end1;
                intSize++;
                updateIntSize();
            } break;
        case 9039:
            {
                if(verbose)
                    cout<<"CODE STACKDEPTH"<<endl;
                intStack[intSize] = NISize;
                intSize++;
                updateIntSize();
            } break;
        case 9040:
            {
                if(verbose)
                    cout<<"CODE SUBST TODO"<<endl;
                NNOOPS++;
            } break;
        case 9041:
            {
                if(NISize > 1 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE SWAP"<<endl;
                    int start1 = NISize-1;
                    int end1 = NISize-2;
                    int start2;
                    int end2;
                    if(instNIStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= 0)
                    {
                        start2 = end1;
                        end2 = start2-1;
                        if(instNIStack[start2] == 10000)
                        {
                            bracketCounter = 0;
                            for(int j=start2;j>=0;j--)
                            {
                                bracketCounter+=(instNIStack[j] == 10000);
                                bracketCounter-=(instNIStack[j] == 10001);
                                if(bracketCounter == 0)
                                {
                                    end2 = j-1;
                                    break;
                                }
                            }
                        }
                        int range1 = start1 - end1;
                        int range2 = start2 - end2;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNIStack[j];
                            tempInstI[j-end1-1] = instNIConstIStack[j];
                            tempInstF[j-end1-1] = instNIConstFStack[j];
                            tempInstS[j-end1-1] = instNIConstSStack[j];
                            tempInstV[j-end1-1] = instNIConstVStack[j];
                        }
                        for(int j=start1;j!=start1-range2;j--)
                        {
                            instNIStack[j] = instNIStack[j-range1];
                            instNIConstIStack[j] = instNIConstIStack[j-range1];
                            instNIConstFStack[j] = instNIConstFStack[j-range1];
                            instNIConstSStack[j] = instNIConstSStack[j-range1];
                            instNIConstVStack[j] = instNIConstVStack[j-range1];
                        }
                        for(int j=start1-range2;j!=end2;j--)
                        {
                            instNIStack[j] = tempInst[j-end2-1];
                            instNIConstIStack[j] = tempInstI[j-end2-1];
                            instNIConstFStack[j] = tempInstF[j-end2-1];
                            instNIConstSStack[j] = tempInstS[j-end2-1];
                            instNIConstVStack[j] = tempInstV[j-end2-1];
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                    }
                    else
                    {
                        if(verbose)
                            cout<<"CODE SWAP NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9042:
            {
                if(NISize > 1 && intSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 1)
                        index_yank = 1;
                    if(index_yank > NISize-1)
                        index_yank = NISize-1;
                    index_yank = NISize-1-index_yank;

                    int start1 = index_yank;
                    int end1 = index_yank-1;
                    if(instNIStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= -1)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNIStack[j];
                            tempInstI[j-end1-1] = instNIConstIStack[j];
                            tempInstF[j-end1-1] = instNIConstFStack[j];
                            tempInstS[j-end1-1] = instNIConstSStack[j];
                            tempInstV[j-end1-1] = instNIConstVStack[j];
                        }
                        int k = end1+1;
                        //for(int j=0;k!=NISize-2;j++)
                        for(int j=0;j!=NISize-start1-1;j++)
                        {
                            instNIStack[k] = instNIStack[k+range1];
                            instNIConstIStack[k] = instNIConstIStack[k+range1];
                            instNIConstFStack[k] = instNIConstFStack[k+range1];
                            instNIConstSStack[k] = instNIConstSStack[k+range1];
                            instNIConstVStack[k] = instNIConstVStack[k+range1];
                            k++;
                        }
                        k = NISize - 2;
                        for(int j=0;j!=range1;j++)
                        {
                            instNIStack[k] = tempInst[range1-1-j];
                            instNIConstIStack[k] = tempInstI[range1-1-j];
                            instNIConstFStack[k] = tempInstF[range1-1-j];
                            instNIConstSStack[k] = tempInstS[range1-1-j];
                            instNIConstVStack[k] = tempInstV[range1-1-j];
                            k--;
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        intSize--;
                    }
                    else
                    {
                        if(verbose)
                            cout<<"CODE YANK NOT ENOUGH"<<endl;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 9043:
            {
                if(NISize > 1 && intSize > 0 && false)///TODO
                {
                    if(verbose)
                        cout<<"CODE YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 1)
                        index_yank = 1;
                    if(index_yank > NISize-1)
                        index_yank = NISize-1;
                    index_yank = NISize-1-index_yank;

                    int start1 = index_yank;
                    int end1 = index_yank-1;
                    if(instNIStack[start1] == 10000)
                    {
                        bracketCounter = 0;
                        for(int j=start1;j>=0;j--)
                        {
                            bracketCounter+=(instNIStack[j] == 10000);
                            bracketCounter-=(instNIStack[j] == 10001);
                            if(bracketCounter == 0)
                            {
                                end1 = j-1;
                                break;
                            }
                        }
                    }
                    if(end1 >= -1)
                    {
                        int range1 = start1 - end1;
                        /*int* tempInst = new int[range1];
                        int* tempInstI = new int[range1];
                        double* tempInstF = new double[range1];
                        string* tempInstS = new string[range1];
                        vector<double>* tempInstV = new vector<double>[range1];*/
                        /*vector<int> tempInst(range1);
                        vector<int> tempInstI(range1);
                        vector<double> tempInstF(range1);
                        vector<string> tempInstS(range1);
                        vector<vector<double> > tempInstV(range1);
                        for(int j=0;j!=range1;j++)
                            tempInstV[j].resize(DIM);*/
                        for(int j=start1;j!=end1;j--)
                        {
                            tempInst[j-end1-1] = instNIStack[j];
                            tempInstI[j-end1-1] = instNIConstIStack[j];
                            tempInstF[j-end1-1] = instNIConstFStack[j];
                            tempInstS[j-end1-1] = instNIConstSStack[j];
                            tempInstV[j-end1-1] = instNIConstVStack[j];
                        }
                        int k = NISize-1;
                        for(int j=0;j!=range1;j++)
                        {
                            instNIStack[k] = tempInst[j];
                            instNIConstIStack[k] = tempInstI[j];
                            instNIConstFStack[k] = tempInstF[j];
                            instNIConstSStack[k] = tempInstS[j];
                            instNIConstVStack[k] = tempInstV[j];
                            k++;
                            NISize++;
                            updateNISize();
                        }
                        /*delete tempInst;
                        delete tempInstI;
                        delete tempInstF;
                        delete[] tempInstS;
                        delete[] tempInstV;*/
                        intSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 10000:
            {
                if(verbose)
                    cout<<"BRACKET OPEN"<<endl;
                bracketCounter=0;
                int pairIndex = -1;
                for(int j=NESize-1;j>=0;j--)
                {
                    bracketCounter+=(instNEStack[j] == 10000);
                    bracketCounter-=(instNEStack[j] == 10001);
                    if(bracketCounter == 0)
                    {
                        pairIndex = j;
                        break;
                    }
                }
                if(pairIndex != -1)
                {
                    for(int j=pairIndex;j<NESize-2;j++)
                    {
                        instNEStack[j] = instNEStack[j+1];
                        instNEConstFStack[j] = instNEConstFStack[j+1];
                        instNEConstIStack[j] = instNEConstIStack[j+1];
                        instNEConstSStack[j] = instNEConstSStack[j+1];
                        instNEConstVStack[j] = instNEConstVStack[j+1];
                    }
                    NESize--;
                }
                else
                {
                    if(verbose)
                        cout<<"BRACKET NOT CLOSED"<<endl;
                }
            } break;
        case 10001:
            {
                if(verbose)
                    cout<<"BRACKET CLOSE UNPAIRED"<<endl;
            } break;
        case 11000:
            {
                if(int(instNEConstVStack[idx].size()) == DIM)
                {
                    vectorStack[vectorSize].resize(DIM);
                    vectorStack[vectorSize] = instNEConstVStack[idx];
                    vectorSize++;
                    updateVectorSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12000:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR +"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = vectorStack[vectorSize-2][d] + vectorStack[vectorSize-1][d];
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12001:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR -"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = vectorStack[vectorSize-2][d] - vectorStack[vectorSize-1][d];
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12002:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR *"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = vectorStack[vectorSize-2][d] * vectorStack[vectorSize-1][d];
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12003:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR /"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        if(fabs(vectorStack[vectorSize-1][d]) == 0 && vectorStack[vectorSize-2][d] >= 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        else if(fabs(vectorStack[vectorSize-1][d]) == 0 && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        else
                            vectorStack[vectorSize-2][d] = vectorStack[vectorSize-2][d] / vectorStack[vectorSize-1][d];
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12004:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR %"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = remainder(vectorStack[vectorSize-2][d], vectorStack[vectorSize-1][d]);
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12005:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR ="<<endl;
                    int neq = 0;
                    for(int d=0;d!=DIM;d++)
                    {
                        if(vectorStack[vectorSize-2][d] == vectorStack[vectorSize-1][d])
                            neq++;
                    }
                    boolStack[boolSize] = (neq == DIM);
                    boolSize++;
                    updateBoolSize();
                    vectorSize-=2;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12006:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR >"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = vectorStack[vectorSize-2][d] > vectorStack[vectorSize-1][d];
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12007:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR <"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = vectorStack[vectorSize-2][d] < vectorStack[vectorSize-1][d];
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12008:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR LN"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = log(vectorStack[vectorSize-1][d]);
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] > 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] < 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-1][d]))
                            vectorStack[vectorSize-1][d] = 0;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12009:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR LOG"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = log(vectorStack[vectorSize-2][d]) / log(vectorStack[vectorSize-1][d]);
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12010:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR POW"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = pow(vectorStack[vectorSize-1][d], vectorStack[vectorSize-2][d]);
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = -std::numeric_limits<double>::max();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12011:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR MIN"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = min(vectorStack[vectorSize-2][d], vectorStack[vectorSize-1][d]);
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12012:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR MAX"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = max(vectorStack[vectorSize-2][d], vectorStack[vectorSize-1][d]);
                    }
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12013:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR ABS"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = abs(vectorStack[vectorSize-1][d]);
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12014:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR NEG"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = -vectorStack[vectorSize-1][d];
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12015:
            {
                if(verbose)
                    cout<<"VECTOR RAND"<<endl;
                vectorStack[vectorSize].resize(DIM);
                for(int d=0;d!=DIM;d++)
                {
                    vectorStack[vectorSize][d] = Random2(minRandomFloat,maxRandomFloat);
                }
                vectorSize++;
                updateVectorSize();
            } break;
        case 12016:
            {
                if(boolSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR FROMBOOL"<<endl;
                    vectorStack[vectorSize].resize(DIM);
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize][d] = double(boolStack[boolSize-1]);
                    }
                    boolSize--;
                    vectorSize++;
                    updateVectorSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12017:
            {
                if(intSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR FROMINT"<<endl;
                    vectorStack[vectorSize].resize(DIM);
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize][d] = double(intStack[intSize-1]);
                    }
                    intSize--;
                    vectorSize++;
                    updateVectorSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12018:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR POP"<<endl;
                    vectorSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12019:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR FLUSH"<<endl;
                    vectorSize=0;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12020:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR DUP"<<endl;
                    vectorStack[vectorSize].resize(DIM);
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize][d] = vectorStack[vectorSize-1][d];
                    }
                    vectorSize++;
                    updateVectorSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12021:
            {
                if(vectorSize > 2)
                {
                    if(verbose)
                        cout<<"VECTOR ROT"<<endl;
                    vector<double> tmp = vectorStack[vectorSize-3];
                    vectorStack[vectorSize-3] = vectorStack[vectorSize-2];
                    vectorStack[vectorSize-2] = vectorStack[vectorSize-1];
                    vectorStack[vectorSize-1] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12022:
            {
                if(vectorSize > 1 && intSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR SHOVE"<<endl;
                    int index_shove = intStack[intSize-1];
                    if(index_shove < 0)
                        index_shove = 0;
                    if(index_shove > vectorSize-1)
                        index_shove = vectorSize-1;
                    vector<double> tmp = vectorStack[vectorSize-1];
                    index_shove = vectorSize-1-index_shove;
                    for(int j=vectorSize-1;j>index_shove;j--)
                        vectorStack[j] = vectorStack[j-1];
                    vectorStack[index_shove] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12023:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR SWAP"<<endl;
                    vector<double> tmp = vectorStack[vectorSize-1];
                    vectorStack[vectorSize-1] = vectorStack[vectorSize-2];
                    vectorStack[vectorSize-2] = tmp;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12024:
            {
                if(vectorSize > 1 && intSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR YANK"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > vectorSize-1)
                        index_yank = vectorSize-1;
                    index_yank = vectorSize-1-index_yank;
                    vector<double> tmp = vectorStack[index_yank];
                    for(int j=index_yank;j<vectorSize-1;j++)
                        vectorStack[j] = vectorStack[j+1];
                    vectorStack[vectorSize-1] = tmp;
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12025:
            {
                if(vectorSize > 0 && intSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR YANKDUP"<<endl;
                    int index_yank = intStack[intSize-1];
                    if(index_yank < 0)
                        index_yank = 0;
                    if(index_yank > vectorSize-1)
                        index_yank = vectorSize-1;
                    index_yank = vectorSize-1-index_yank;
                    vector<double> tmp = vectorStack[index_yank];
                    vectorStack[vectorSize] = tmp;
                    vectorSize++;
                    updateVectorSize();
                    intSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12026:
            {
                if(verbose)
                    cout<<"VECTOR STACKDEPTH"<<endl;
                intStack[intSize] = vectorSize;
                intSize++;
                updateIntSize();
            } break;
        case 12027:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR SIN"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = sin(vectorStack[vectorSize-1][d]);
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12028:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR COS"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = cos(vectorStack[vectorSize-1][d]);
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12029:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR TAN"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = tan(vectorStack[vectorSize-1][d]);
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] > 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] < 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-1][d]))
                            vectorStack[vectorSize-1][d] = 0;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12030:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR EXP"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = exp(vectorStack[vectorSize-1][d]);
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] > 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] < 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-1][d]))
                            vectorStack[vectorSize-1][d] = 0;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12031:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR DOT"<<endl;
                    double res = 0;
                    for(int d=0;d!=DIM;d++)
                    {
                        res += vectorStack[vectorSize-2][d] * vectorStack[vectorSize-1][d];
                    }
                    vectorSize--;
                    floatStack[floatSize] = res;
                    if(isinf(floatStack[floatSize]) && floatStack[floatSize] > 0)
                        floatStack[floatSize] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize]) && floatStack[floatSize] < 0)
                        floatStack[floatSize] = -std::numeric_limits<double>::max();
                    if(isnan(floatStack[floatSize]))
                        floatStack[floatSize] = 0;
                    floatSize++;
                    updateFloatSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12032:
            {
                if(vectorSize > 0 && intSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR DIM GET"<<endl;
                    int index = intStack[intSize-1]%DIM;
                    if(isnan(index))
                    {
                        NNOOPS++;
                    }
                    else
                    {
                        floatStack[floatSize] = vectorStack[vectorSize-1][index];
                        vectorSize--;
                        intSize--;
                        floatSize++;
                        updateFloatSize();
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12033:
            {
                if(vectorSize > 0 && intSize > 0 && floatSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR DIM +"<<endl;
                    int index = intStack[intSize-1];
                    if(index > DIM-1)
                        index = DIM-1;
                    if(index < 0)
                        index = 0;
                    if(isnan(index))
                    {
                        NNOOPS++;
                    }
                    else
                    {
                        vectorStack[vectorSize-1][index] = vectorStack[vectorSize-1][index] + floatStack[floatSize-1];
                        if(isinf(vectorStack[vectorSize-1][index]) && vectorStack[vectorSize-1][index] > 0)
                            vectorStack[vectorSize-1][index] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-1][index]) && vectorStack[vectorSize-1][index] < 0)
                            vectorStack[vectorSize-1][index] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-1][index]))
                            vectorStack[vectorSize-1][index] = 0;
                        intSize--;
                        floatSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12034:
            {
                if(vectorSize > 0 && intSize > 0 && floatSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR DIM *"<<endl;
                    int index = intStack[intSize-1];
                    if(index > DIM-1)
                        index = DIM-1;
                    if(index < 0)
                        index = 0;
                    if(isnan(index))
                    {
                        NNOOPS++;
                    }
                    else
                    {
                        vectorStack[vectorSize-1][index] = vectorStack[vectorSize-1][index] * floatStack[floatSize-1];
                        if(isinf(vectorStack[vectorSize-1][index]) && vectorStack[vectorSize-1][index] > 0)
                            vectorStack[vectorSize-1][index] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-1][index]) && vectorStack[vectorSize-1][index] < 0)
                            vectorStack[vectorSize-1][index] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-1][index]))
                            vectorStack[vectorSize-1][index] = 0;
                        intSize--;
                        floatSize--;
                    }
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12035:
            {
                if(floatSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR FROM FLOAT"<<endl;
                    vectorStack[vectorSize].resize(DIM);
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize][d] = floatStack[floatSize-1];
                    }
                    floatSize--;
                    vectorSize++;
                    updateVectorSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12036:
            {
                if(vectorSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR SUM"<<endl;
                    double sum = 0;
                    for(int d=0;d!=DIM;d++)
                    {
                        sum += vectorStack[vectorSize-1][d];
                    }
                    floatStack[floatSize] = sum;
                    if(isinf(floatStack[floatSize]) && floatStack[floatSize] > 0)
                        floatStack[floatSize] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize]) && floatStack[floatSize] < 0)
                        floatStack[floatSize] = -std::numeric_limits<double>::max();
                    if(isnan(floatStack[floatSize]))
                        floatStack[floatSize] = 0;
                    vectorSize--;
                    floatSize++;
                    updateFloatSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12037:
            {
                if(vectorSize > 1)
                {
                    if(verbose)
                        cout<<"VECTOR DIST"<<endl;
                    double sum = 0;
                    for(int d=0;d!=DIM;d++)
                    {
                        sum += (vectorStack[vectorSize-1][d] - vectorStack[vectorSize-2][d])*(vectorStack[vectorSize-1][d] - vectorStack[vectorSize-2][d]);
                    }
                    floatStack[floatSize] = sqrt(sum);
                    if(isinf(floatStack[floatSize]) && floatStack[floatSize] > 0)
                        floatStack[floatSize] = std::numeric_limits<double>::max();
                    if(isinf(floatStack[floatSize]) && floatStack[floatSize] < 0)
                        floatStack[floatSize] = -std::numeric_limits<double>::max();
                    if(isnan(floatStack[floatSize]))
                        floatStack[floatSize] = 0;
                    vectorSize-=2;
                    floatSize++;
                    updateFloatSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12038:
            {
                if(vectorSize > 0 && false)
                {
                    if(verbose)
                        cout<<"VECTOR EVAL FUNC"<<endl;
                    double nWithin = 0;
                    vector<double> tmp;
                    vector<double> tmp2;
                    tmp.resize(DIM);
                    tmp2.resize(DIM);
                    for(int d=0;d!=DIM;d++)
                    {
                        nWithin += (vectorStack[vectorSize-1][d] > CEC_Left[d]) && (vectorStack[vectorSize-1][d] < CEC_Right[d]);
                        tmp[d] = (vectorStack[vectorSize-1][d] > CEC_Left[d]) && (vectorStack[vectorSize-1][d] < CEC_Right[d]);
                        tmp2[d] = (vectorStack[vectorSize-1][d] - CEC_Shifts[d])/CEC_Streches[d];
                    }
                    if(nWithin < DIM)
                    {
                        floatStack[floatSize] = std::numeric_limits<double>::max();
                        //intStack[intSize] = nWithin;
                        //boolStack[boolSize] = false;
                    }
                    else
                    {
                        //floatStack[floatSize] = CEC17(vectorStack[vectorSize-1]);
                        floatStack[floatSize] = CEC17(tmp2) - func_num*100;
                        //floatStack[floatSize] = Random(0,32768);
                        //intStack[intSize] = DIM;
                        //boolStack[boolSize] = true;
                    }
                    /*for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = tmp[d];
                    }*/
                    vectorSize--;
                    floatSize++;
                    //intSize++;
                    //boolSize++;
                    updateFloatSize();
                    updateIntSize();
                    updateBoolSize();
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12039:
            {
                if(vectorSize > 0 && floatSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR SCALE"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-1][d] = vectorStack[vectorSize-1][d] * floatStack[floatSize-1];
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] > 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-1][d]) && vectorStack[vectorSize-1][d] < 0)
                            vectorStack[vectorSize-1][d] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-1][d]))
                            vectorStack[vectorSize-1][d] = 0;
                    }
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12040:
            {
                if(vectorSize > 1 && floatSize > 0)
                {
                    if(verbose)
                        cout<<"VECTOR BETWEEN"<<endl;
                    for(int d=0;d!=DIM;d++)
                    {
                        vectorStack[vectorSize-2][d] = vectorStack[vectorSize-1][d] + floatStack[floatSize-1]*(vectorStack[vectorSize-2][d] - vectorStack[vectorSize-1][d]);
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] > 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::max();
                        if(isinf(vectorStack[vectorSize-2][d]) && vectorStack[vectorSize-2][d] < 0)
                            vectorStack[vectorSize-2][d] = std::numeric_limits<double>::min();
                        if(isnan(vectorStack[vectorSize-2][d]))
                            vectorStack[vectorSize-2][d] = 0;
                    }
                    vectorSize--;
                    floatSize--;
                }
                else
                {
                    NNOOPS++;
                }
            } break;
        case 12041:
            {
                if(verbose)
                    cout<<"VECTOR RAND01"<<endl;
                vectorStack[vectorSize].resize(DIM);
                for(int d=0;d!=DIM;d++)
                {
                    vectorStack[vectorSize][d] = Random2(0,1);
                }
                vectorSize++;
                updateVectorSize();
            } break;
        case 12042:
            {
                if(verbose)
                    cout<<"VECTOR NORMAL RAND"<<endl;
                vectorStack[vectorSize].resize(DIM);
                for(int d=0;d!=DIM;d++)
                {
                    vectorStack[vectorSize][d] = NormRand2(0,1);
                }
                vectorSize++;
                updateVectorSize();
            } break;
    }
    NESize--;

    NESize = min(NESize,StackSizeLimit);
    NISize = min(NISize,StackSizeLimit);
    intSize = min(intSize,StackSizeLimit);
    floatSize = min(floatSize,StackSizeLimit);
    boolSize = min(boolSize,StackSizeLimit);
    nameSize = min(nameSize,StackSizeLimit);
    InpSize = min(InpSize,StackSizeLimit);
    vectorSize = min(vectorSize,StackSizeLimit);
}

class PushGP
{
public:
    int NInds;
    int MinInitSize;
    int MaxInitSize;
    int NActiveInst;
    int NTypes;

    int* IndLength;
    int* ActiveInst;
    int* NTypeInst;
    int* Indexes;
    int* Rands;
    int* BracketPosOpen;
    int* BracketPosClose;
    int* IndActive;
    int* OrderCases;

    double* Fitness;
    double* Ranks;
    double* RanksCopy;
    double** Perf;
    double* tempSort;
    double* TempRanks;
    double* TempVals;
    double* MedAbsDist;
    double* SortPerf;

    double** data;
    //int nrows = MaxFuncs*NCECRuns*2;
    int nrows = MaxFuncs*NCECRuns*1;
    int PerfSize;
    int counterlimit = 300;
    int MaxLength = LimSize;
    int TourSize = 2;

    bool outputfinal;
    int FuncNum_;

    atomInst** Popul;
    interpreter* Inters;
    atomInst* tempInd;

    PushGP(int newNInds);
    ~PushGP();
    void FitCalc(int index);
    void FitCalcCEC(int index);
    void FitCalcPopBasedCEC(int index);
    void FitCalcPopBasedCEC2(int index);
    void FitCalcPopBasedCEC_LPSR(int index);
    void FitCalcClassifReg(int index);
    void FitCalcDE(int index);
    void SelectTour2(int index);
    void SelectTour(int index);
    void SelectLexicase(int index, int SizeMultiplier);
    void PrepareLexicase(int SizeMultiplier);
    void Crossover(int index);
    void CrossoverAdd(int index);
    void CrossoverULT(int index);
    void Mutate(int index);
    void MutateAdd(int index);
    void MutateElWise(int index);
    void RepairBracket(int index);
    void Replace(int index);
    void CalculateRanks(int SizeMultiplier, double SimilarityWeight, double LengthWeight, double NOOPSWeight, double T1Weight);
    void ReplaceSort();
    void ReplaceNew();
    void SimilarityCalc(int SizeMultiplier);
};
PushGP::PushGP(int newNInds)
{
    /*ifstream fin("banana.dat");
    data = new double*[nrows];
    for(int i=0;i!=nrows;i++)
    {
        data[i] = new double[3];
        fin>>data[i][0];
        fin>>data[i][1];
        fin>>data[i][2];
    }*/
    NInds = newNInds;

    PerfSize = nrows + 4 + NInds*2; // + complexity + nnoop + time + median
    outputfinal = false;

    MinInitSize = 1;
    MaxInitSize = 64;

    MedAbsDist = new double[PerfSize];
    SortPerf = new double[NInds*2];
    IndActive = new int[NInds*2];
    OrderCases = new int[PerfSize];

    IndLength = new int[NInds*2];
    Indexes = new int[NInds*2];
    Popul = new atomInst*[NInds*2];
    Inters = new interpreter[1];
    Fitness = new double[NInds*2];
    Ranks = new double[NInds*2];
    RanksCopy = new double[NInds*2];
    Perf = new double*[NInds*2];
    tempSort = new double[PerfSize-4];
    TempRanks = new double[NInds*2];
    TempVals = new double[NInds*2];
    Rands = new int[TourSize];
    BracketPosOpen = new int[MaxLength];
    BracketPosClose = new int[MaxLength];

    vector<int> keys;
    for(unordered_map<int,string>::iterator it = Inters[0].inst_map_inv.begin(); it != Inters[0].inst_map_inv.end(); ++it)
    {
        keys.push_back(it->first);
        //if(verbose)
          //  cout << "Key: " << it->first << endl;
    }
    NActiveInst = keys.size()+4;
    ActiveInst = new int[NActiveInst];
    ActiveInst[0] = 1000;
    ActiveInst[1] = 2000;
    ActiveInst[2] = 7000;
    ActiveInst[3] = 11000;
    for(int i=0;i!=NActiveInst-4;i++)
    {
        ActiveInst[4+i] = keys[i];
    }
    NTypes = 12;
    NTypeInst = new int[NTypes];
    for(int i=0;i!=NTypes;i++)
    {
        NTypeInst[i] = 0;
        for(int j=0;j!=NActiveInst;j++)
        {
            if(ActiveInst[j] >= 1000*(i+1) && ActiveInst[j] < (1000*(i+2)))
            {
                NTypeInst[i]++;
            }
        }
    }
    tempInd = new atomInst[MaxLength];

    for(int i=0;i!=NInds*2;i++)
    {
        Perf[i] = new double[PerfSize];
        IndLength[i] = IntRandom(MaxInitSize-MinInitSize)+MinInitSize;
        //Popul[i] = new atomInst[IndLength[i]];
        Popul[i] = new atomInst[MaxLength];
        for(int j=0;j!=IndLength[i];j++)
        {
            int InstType;// = IntRandom(NTypes)+1;
            int InstIndex;// = IntRandom(NTypeInst[InstType-1]) + 1000*InstType;
            InstIndex = IntRandom(NActiveInst);
            //InstIndex = 1;
            int last = 0;
            for(int k=0;k!=NTypes;k++)
            {
                if(InstIndex < last + NTypeInst[k])
                {
                    InstType = k+1;
                    InstIndex = InstIndex - last + 1000*InstType;
                    break;
                }
                else
                    last += NTypeInst[k];
            }

            Popul[i][j].inst = InstIndex;
            if(InstIndex == 1000)
            {
                Popul[i][j].f = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
            }
            if(InstIndex == 2000)
            {
                Popul[i][j].i = Random(Inters[0].minRandomInt,Inters[0].maxRandomInt);
            }
            if(InstIndex == 7000)
            {
                string randstr;
                int randlen;
                randlen = IntRandom(2)+2;
                randstr.resize(randlen);
                for (int k = 0; k < randlen; k++)
                    randstr[k] = Inters[0].charset[IntRandom(52)];
                Popul[i][j].n = randstr;
            }
            if(InstIndex == 11000)
            {
                vector<double> newvec;
                int DIM = Inters[0].DIM;
                newvec.resize(DIM);
                for (int k = 0; k < DIM; k++)
                    newvec[k] = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
                Popul[i][j].v = newvec;
            }
        }
    }
}
PushGP::~PushGP()
{
    delete IndLength;
    for(int i=0;i!=NInds*2;i++)
    {
        delete[] Popul[i];
        delete Perf[i];
    }
    delete[] Popul;
    delete[] Inters;
    delete Perf;
    delete Ranks;
    delete RanksCopy;
    delete Indexes;
    delete ActiveInst;
    delete NTypeInst;
    delete Fitness;
    delete[] tempInd;
    delete tempSort;
    delete TempRanks;
    delete TempVals;
    delete Rands;
    delete BracketPosOpen;
    delete BracketPosClose;
    delete SortPerf;
    delete MedAbsDist;
    delete OrderCases;
    delete IndActive;
    /*for(int i=0;i!=nrows;i++)
    {
        delete data[i];
    }
    delete data;*/
}
void PushGP::FitCalcCEC(int index)
{
    //cout<<"index "<<index<<endl;
    Inters[0].verbose = false;
    //unsigned t0=clock(),t1;
    int totalNNOPS = 0;
    //outputfinal = false;
    if(outputfinal)
    {
        Inters[0].verbose = true;
        Inters[0].reset(IndLength[index]);
        Inters[0].setProgram(Popul[index],IndLength[index]);
        ofstream fout("result_program.txt");
        fout<<Inters[0].printProgram();
    }
    //outputfinal = true;
    int perf_counter = 0;
    //double vm, rss;
    //double vm2, rss2;
    int TotalNFEUsed = 0;
    double totalStackSize = 0;

    int LimRuns = 1;
    int LimFunc = 1;
    for(int i=0;i!=NInds*2;i++)
    {
        if(Perf[i][4] < 1e300)
        {
            LimRuns = NCECRuns;
            LimFunc = MaxFuncs;
            //cout<<"ALL"<<endl;
            break;
        }
    }
    int MaxNFEUsed = NCECRuns*LimRuns*LimFunc;

    for(int run=0;run!=LimRuns;run++)
    {
        for(func_num=1;func_num!=1+LimFunc;func_num++)
        {
            //cout<<run<<" "<<func_num<<" reset"<<endl;
            //process_mem_usage(vm, rss); cout<<"MEM 1\t"<<vm<<endl;
            Inters[0].reset(IndLength[index]);
            //process_mem_usage(vm, rss); //cout<<"MEM 2\t"<<vm<<endl;
            //cout<<run<<" "<<func_num<<" set"<<endl;
            Inters[0].setProgram(Popul[index],IndLength[index]);
            //cout<<run<<" "<<func_num<<" start"<<endl;

            Inters[0].instInpStack[Inters[0].InpSize] = 2000;
            Inters[0].instInpConstIStack[Inters[0].InpSize] = GNVars;
            Inters[0].InpSize++;
            Inters[0].updateInpSize();
            //cout<<run<<" "<<func_num<<" stage1"<<endl;

            Inters[0].instInpStack[Inters[0].InpSize] = 11000;
            Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
            for(int i=0;i!=GNVars;i++)
            {
                Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Left[i];
            }
            Inters[0].InpSize++;
            Inters[0].updateInpSize();
            //cout<<run<<" "<<func_num<<" stage2"<<endl;

            Inters[0].instInpStack[Inters[0].InpSize] = 11000;
            Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
            for(int i=0;i!=GNVars;i++)
            {
                Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Right[i];
            }
            Inters[0].InpSize++;
            Inters[0].updateInpSize();
            //cout<<run<<" "<<func_num<<" stage3"<<endl;

            NFE = 0;
            fbest = numeric_limits<double>::max();
            int counter = 0;
            while(Inters[0].NESize > 0 && NFE < NFEmax)
            {
                //cout<<"counter: "<<counter<<endl;
                if(outputfinal)
                {
                    cout<<Inters[0].printStacks()<<endl;
                    getchar();
                }
                Inters[0].instStep();
                counter++;
                if(
                   (counter > counterlimit && NFE == 0)
                   || counter > NFEmax*200
                   /*|| Inters[0].boolSize > LimStackSize
                   || Inters[0].intSize > LimStackSize
                   || Inters[0].floatSize > LimStackSize
                   || Inters[0].vectorSize > LimStackSize
                   || Inters[0].nameSize > LimStackSize
                   || Inters[0].NESize > LimStackSize
                   || Inters[0].NISize > LimStackSize*/
                   )
                    break;
                /*if(outputfinal)
                    getchar();*/
                /*for(int i=0;i!=Inters[0].vectorSize;i++)
                {
                    if(int(Inters[0].vectorStack[i].size()) != GNVars)
                    {
                        cout<<"vector size mismatch"<<endl;
                        Inters[0].vectorStack[i].resize(GNVars);
                    }
                }*/
            }
            fbest = (NFE >= NFEmax)*fbest + (NFE < NFEmax)*numeric_limits<double>::max();
            TotalNFEUsed += NFE;
            totalStackSize += double(Inters[0].boolSize);
            totalStackSize += double(Inters[0].intSize);
            totalStackSize += double(Inters[0].floatSize);
            totalStackSize += double(Inters[0].vectorSize);
            totalStackSize += double(Inters[0].nameSize);
            totalStackSize += double(Inters[0].NESize);
            totalStackSize += double(Inters[0].NISize);

            //cout<<run<<" "<<func_num<<" stage4"<<endl;
            //cout<<"END1"<<endl;
            if(outputfinal)
            {
                cout<<Inters[0].printStacks()<<endl;
            }
            Perf[index][4+perf_counter] = fbest - func_num*100;
            //cout<<run<<" "<<func_num<<" stage5"<<endl;
            perf_counter++;
            totalNNOPS += Inters[0].NNOOPS;
            //Inters[0].reset(IndLength[index]);
            //process_mem_usage(vm2, rss2); //cout<<"MEM 3\t"<<vm<<endl;
        }
    }
    //if(vm != vm2)
    {
        //Inters[0].reset(IndLength[index]);
        //Inters[0].setProgram(Popul[index],IndLength[index]);
        //cout<<Inters[0].printStacks()<<endl;
        //cout<<endl;
    }
    //cout<<"END2"<<endl;
    //t1=clock()-t0;
    Perf[index][0] = IndLength[index];//abs(IndLength[index]-MaxInitSize);
    Perf[index][1] = totalNNOPS;
    Perf[index][2] = totalStackSize;//double(t1);

    if(IndLength[index] > 128)
    {
        //  Perf[index][0] = IndLength[index] - 256;
        Perf[index][0] = IndLength[index];//abs(IndLength[index]-MaxInitSize);
        Perf[index][1] = totalNNOPS;
        Perf[index][2] = totalStackSize;//double(t1);
    }

    if(TotalNFEUsed < MaxNFEUsed)
    {
        for(int i=4;i!=nrows+4;i++)
            Perf[index][i] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
        //Perf[index][1] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
        //Perf[index][2] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
    }

    double sumPerf = 0;
    for(int i=4;i!=nrows+4;i++)
    {
        tempSort[i-4] = Perf[index][i];
        sumPerf += Perf[index][i];
    }
    //cout<<"END3"<<endl;
    qSort1(tempSort,0,nrows-1);
    Fitness[index] = tempSort[((nrows)>>1)];//sumPerf;//
    Perf[index][3] = Fitness[index];
    //cout<<"END4"<<endl;
    //delete tempSort;
    //cout<<"END5"<<endl;
}

void PushGP::FitCalcPopBasedCEC(int index)
{
    /*unsigned long seed1 = globalseed;
    unsigned long seed2 = globalseed;
    unsigned long seed3 = globalseed;
    unsigned long seed4 = globalseed;
    generator_uni_i2.seed(seed1);
    generator_uni_r2.seed(seed2);
    generator_norm2.seed(seed3);
    generator_cauchy2.seed(seed4);*/

    Inters[0].verbose = false;
    int totalNNOPS = 0;
    //outputfinal = true;
    if(outputfinal)
    {
        Inters[0].verbose = true;
        Inters[0].reset(IndLength[index]);
        Inters[0].setProgram(Popul[index],IndLength[index]);
        ofstream fout("result_program.txt");
        fout<<Inters[0].printProgram();
    }
    int perf_counter = 0;
    int TotalNFEUsed = 0;
    int LimRuns = NCECRuns;
    int LimFunc = MaxFuncs;
    /*for(int i=0;i!=NInds*2;i++)
    {
        if(Perf[i][4] < 1e300)
        {
            LimRuns = NCECRuns;
            LimFunc = MaxFuncs;
            break;
        }
    }*/
    sprintf(path_cec,"%s","input_data_17");
    int MaxNFEUsed = NFEmax*LimRuns*LimFunc;
    for(int run=0;run!=LimRuns;run++)
    {
        int func_index = 1;
        ofstream foutTraj("Trajectories.txt");
        for(func_index=1;func_index!=1+LimFunc;func_index++)
        {
            func_num = func_index;
            if(func_index > 1)
                func_num = func_index + 1;

            //cout<<func_num<<endl;
            //////////////////////

            double* xopt = new double[GNVars];
            fopt = GetOptimum(func_num,xopt);
            delete xopt;

            //////////////////////

            int popsize = GlobalPopSizeMult*GNVars;//(IntRandom2(6)+5)*GNVars; //5*GNVars; //
            //maxRandomInt = popsize;
            double** Pop = new double*[popsize];
            double* Fit = new double[popsize];
            for(int i=0;i!=popsize;i++)
            {
                Pop[i] = new double[GNVars];
                for(int j=0;j!=GNVars;j++)
                {
                    Pop[i][j] = Random2(CEC_Left[j],CEC_Right[j]);
                }
            }
            NFE = 0;

            vector<double> tmp2;
            tmp2.resize(GNVars);
            fbest = numeric_limits<double>::max();
            int counter2 = 0;
            int counter2limit = 300;
            for(int i=0;i!=popsize;i++)
            {
                for(int d=0;d!=GNVars;d++)
                {
                    tmp2[d] = (Pop[i][d] - CEC_Shifts[d])/CEC_Streches[d];
                    foutTraj<<tmp2[d]<<"\t";
                }
                Fit[i] = CEC17(tmp2);
                foutTraj<<Fit[i]<<"\n";
                TotalNFEUsed++;
                fbest = min(fbest,Fit[i]);
            }
            while(NFE < NFEmax)
            {
                for(int i=0;i!=popsize;i++)
                {
                    Inters[0].reset(IndLength[index]);
                    Inters[0].setProgram(Popul[index],IndLength[index]);
                    for(int k=0;k!=popsize;k++)
                    {
                        Inters[0].floatStack[Inters[0].floatSize] = Fit[k];
                        Inters[0].floatSize++;
                        Inters[0].updateFloatSize();

                        Inters[0].vectorStack[Inters[0].vectorSize].resize(GNVars);
                        for(int j=0;j!=GNVars;j++)
                        {
                            Inters[0].vectorStack[Inters[0].vectorSize][j] = Pop[k][j];
                        }
                        Inters[0].vectorSize++;
                        Inters[0].updateVectorSize();
                    }
                    Inters[0].intStack[Inters[0].intSize] = GNVars;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].intStack[Inters[0].intSize] = popsize;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].intStack[Inters[0].intSize] = i;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].instInpStack[Inters[0].InpSize] = 11000;
                    Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
                    for(int i=0;i!=GNVars;i++)
                    {
                        Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Left[i];
                    }
                    Inters[0].InpSize++;
                    Inters[0].updateInpSize();

                    Inters[0].instInpStack[Inters[0].InpSize] = 11000;
                    Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
                    for(int i=0;i!=GNVars;i++)
                    {
                        Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Right[i];
                    }
                    Inters[0].InpSize++;
                    Inters[0].updateInpSize();


                    char buffer[100];
                    int counter = 0;
                    if(outputfinal)
                    {
                        GlobalLog = "";
                        cout<<Inters[0].printStacks()<<endl;
                        sprintf(buffer,"Iteration %d \n",counter);
                        GlobalLog+=buffer;
                        GlobalLog+=Inters[0].printStacks()+"\n";
                        getchar();
                    }
                    while(Inters[0].NESize > 0)
                    {
                        Inters[0].instStep();
                        counter++;
                        if(counter > counterlimit)
                        {
                            break;
                        }
                        if(outputfinal)
                        {
                            cout<<Inters[0].printStacks()<<endl;
                            sprintf(buffer,"Iteration %d \n",counter);
                            GlobalLog+=buffer;
                            GlobalLog+=Inters[0].printStacks()+"\n";
                            getchar();
                        }
                    }
                    double tempfit = 0;
                    if(Inters[0].vectorSize > 0 && counter < counterlimit)
                    {
                        int nbad = 0;
                        int nbad2 = 0;
                        for(int j=0;j!=GNVars;j++)
                        {
                            if(Inters[0].vectorStack[Inters[0].vectorSize-1][j] > CEC_Right[j] || Inters[0].vectorStack[Inters[0].vectorSize-1][j] < CEC_Left[j])
                            {
                                nbad++;
                            }
                            if(Inters[0].vectorStack[Inters[0].vectorSize-1][j] == Pop[popsize-1][j])
                            {
                                nbad2++;
                            }
                        }
                        if(nbad > 0)
                        {
                            tempfit = numeric_limits<double>::max();
                        }
                        else if(nbad2 == GNVars && GlobalMode == 0)
                        {
                            //cout<<"nbad2 "<<endl;
                            tempfit = numeric_limits<double>::max();
                        }
                        else
                        {
                            //cout<<"calc"<<endl;
                            for(int d=0;d!=GNVars;d++)
                            {
                                tmp2[d] = (Inters[0].vectorStack[Inters[0].vectorSize-1][d] - CEC_Shifts[d])/CEC_Streches[d];
                                //foutTraj<<tmp2[d]<<"\t";
                            }
                            tempfit = CEC17(tmp2);
                            //foutTraj<<tempfit<<"\n";
                            TotalNFEUsed++;
                            if(tempfit < Fit[i])
                            {
                                Fit[i] = tempfit;
                                for(int j=0;j!=GNVars;j++)
                                {
                                    Pop[i][j] = Inters[0].vectorStack[Inters[0].vectorSize-1][j];
                                }
                            }
                            for(int j=0;j!=GNVars;j++)
                            {
                                foutTraj<<Pop[i][j]<<"\t";
                            }
                            foutTraj<<Fit[i]<<"\n";
                            fbest = min(fbest,tempfit);
                        }
                        /*for(int k=0;k!=popsize;k++)
                        {
                            if(tempfit < Fit[k])
                            {
                                fbest = min(fbest,tempfit);
                                Fit[k] = tempfit;
                                for(int j=0;j!=GNVars;j++)
                                {
                                    Pop[k][j] = Inters[0].vectorStack[Inters[0].vectorSize-1][j];
                                }
                                break;
                            }
                        }*/
                    }
                    counter2++;
                    if(NFE >= NFEmax)
                        break;

                    if(outputfinal)
                    {
                        ofstream foutGL("GlobalLog.txt");
                        foutGL<<GlobalLog;
                    }
                }
                if(counter2 - TotalNFEUsed > counter2limit)
                    break;
                if(counter2 > counter2limit && NFE < counter2limit)
                    break;
            }
            Perf[index][4+perf_counter] = fbest - func_num*100;
            perf_counter++;
            totalNNOPS += Inters[0].NNOOPS;

            for(int i=0;i!=popsize;i++)
            {
                delete Pop[i];
            }
            delete Pop;
            delete Fit;
        }
    }
    Perf[index][0] = IndLength[index];
    Perf[index][1] = totalNNOPS;
    Perf[index][2] = 0;
    if(TotalNFEUsed < MaxNFEUsed)
    {
        for(int i=4;i!=nrows+4;i++)
            Perf[index][i] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
    }
    double sumPerf = 0;
    for(int i=4;i!=nrows+4;i++)
    {
        tempSort[i-4] = Perf[index][i];
        sumPerf += Perf[index][i];
    }
    qSort1(tempSort,0,nrows-1);
    Fitness[index] = tempSort[((nrows)>>1)];
    Perf[index][3] = Fitness[index];
    for(int i=0;i!=nrows+4;i++)
    {
        if(isnan(Perf[index][i]))
            Perf[index][i] = numeric_limits<double>::max();
        if(isinf(Perf[index][i]))
            Perf[index][i] = numeric_limits<double>::max();
    }

    //cout<<index<<"\t"<<"\t"<<IntRandom2(10)<<endl;
}

void PushGP::FitCalcPopBasedCEC2(int index)
{
    /*unsigned long seed1 = globalseed;
    unsigned long seed2 = globalseed;
    unsigned long seed3 = globalseed;
    unsigned long seed4 = globalseed;
    generator_uni_i2.seed(seed1);
    generator_uni_r2.seed(seed2);
    generator_norm2.seed(seed3);
    generator_cauchy2.seed(seed4);*/

    Inters[0].verbose = false;
    int totalNNOPS = 0;
    //outputfinal = true;
    if(outputfinal)
    {
        Inters[0].verbose = true;
        Inters[0].reset(IndLength[index]);
        Inters[0].setProgram(Popul[index],IndLength[index]);
        ofstream fout("result_program.txt");
        fout<<Inters[0].printProgram();
    }
    int perf_counter = 0;
    int TotalNFEUsed = 0;
    int LimRuns = NCECRuns;
    int LimFunc = MaxFuncs;
    /*for(int i=0;i!=NInds*2;i++)
    {
        if(Perf[i][4] < 1e300)
        {
            LimRuns = NCECRuns;
            LimFunc = MaxFuncs;
            break;
        }
    }*/
    int MaxNFEUsed = NFEmax*LimRuns*LimFunc;
    for(int run=0;run!=LimRuns;run++)
    {
        for(func_num=1;func_num!=1+LimFunc;func_num++)
        {
            int popsize = (IntRandom2(6)+5)*GNVars; //5*GNVars; //
            double** Pop = new double*[popsize];
            double* Fit = new double[popsize];
            for(int i=0;i!=popsize;i++)
            {
                Pop[i] = new double[GNVars];
                for(int j=0;j!=GNVars;j++)
                {
                    Pop[i][j] = Random2(CEC_Left[j],CEC_Right[j]);
                }
            }
            NFE = 0;

            vector<double> tmp2;
            tmp2.resize(GNVars);
            fbest = numeric_limits<double>::max();
            int counter2 = 0;
            int counter2limit = 300;
            for(int i=0;i!=popsize;i++)
            {
                for(int d=0;d!=GNVars;d++)
                {
                    tmp2[d] = (Pop[i][d] - CEC_Shifts[d])/CEC_Streches[d];
                }
                Fit[i] = CEC17(tmp2);
                TotalNFEUsed++;
                fbest = min(fbest,Fit[i]);
            }
            while(NFE < NFEmax)
            {
                for(int i=0;i!=popsize;i++)
                {
                    Inters[0].reset(IndLength[index]);
                    Inters[0].setProgram(Popul[index],IndLength[index]);
                    for(int k=0;k!=popsize;k++)
                    {
                        Inters[0].floatStack[Inters[0].floatSize] = Fit[k];
                        Inters[0].floatSize++;
                        Inters[0].updateFloatSize();

                        Inters[0].vectorStack[Inters[0].vectorSize].resize(GNVars);
                        for(int j=0;j!=GNVars;j++)
                        {
                            Inters[0].vectorStack[Inters[0].vectorSize][j] = Pop[k][j];
                        }
                        Inters[0].vectorSize++;
                        Inters[0].updateVectorSize();
                    }
                    Inters[0].intStack[Inters[0].intSize] = GNVars;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].intStack[Inters[0].intSize] = popsize;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].intStack[Inters[0].intSize] = i;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].instInpStack[Inters[0].InpSize] = 11000;
                    Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
                    for(int i=0;i!=GNVars;i++)
                    {
                        Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Left[i];
                    }
                    Inters[0].InpSize++;
                    Inters[0].updateInpSize();

                    Inters[0].instInpStack[Inters[0].InpSize] = 11000;
                    Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
                    for(int i=0;i!=GNVars;i++)
                    {
                        Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Right[i];
                    }
                    Inters[0].InpSize++;
                    Inters[0].updateInpSize();

                    int counter = 0;
                    while(Inters[0].NESize > 0)
                    {
                        Inters[0].instStep();
                        counter++;
                        if(counter > counterlimit)
                        {
                            break;
                        }
                        if(outputfinal)
                        {
                            cout<<Inters[0].printStacks()<<endl;
                            getchar();
                        }
                    }
                    double tempfit = 0;
                    if(Inters[0].vectorSize > 0 && counter < counterlimit)
                    {
                        int nbad = 0;
                        int nbad2 = 0;
                        for(int j=0;j!=GNVars;j++)
                        {
                            if(Inters[0].vectorStack[Inters[0].vectorSize-1][j] > CEC_Right[j] || Inters[0].vectorStack[Inters[0].vectorSize-1][j] < CEC_Left[j])
                            {
                                nbad++;
                            }
                            if(Inters[0].vectorStack[Inters[0].vectorSize-1][j] == Pop[popsize-1][j])
                            {
                                nbad2++;
                            }
                        }
                        if(nbad > 0)
                        {
                            tempfit = numeric_limits<double>::max();
                        }
                        else if(nbad2 == GNVars)
                        {
                            //cout<<"nbad2 "<<endl;
                            tempfit = numeric_limits<double>::max();
                        }
                        else
                        {
                            //cout<<"calc"<<endl;
                            for(int d=0;d!=GNVars;d++)
                            {
                                tmp2[d] = (Inters[0].vectorStack[Inters[0].vectorSize-1][d] - CEC_Shifts[d])/CEC_Streches[d];
                            }
                            tempfit = CEC17(tmp2);
                            TotalNFEUsed++;
                            if(tempfit < Fit[i])
                            {
                                Fit[i] = tempfit;
                                for(int j=0;j!=GNVars;j++)
                                {
                                    Pop[i][j] = Inters[0].vectorStack[Inters[0].vectorSize-1][j];
                                }
                            }
                            fbest = min(fbest,tempfit);
                        }
                        /*for(int k=0;k!=popsize;k++)
                        {
                            if(tempfit < Fit[k])
                            {
                                fbest = min(fbest,tempfit);
                                Fit[k] = tempfit;
                                for(int j=0;j!=GNVars;j++)
                                {
                                    Pop[k][j] = Inters[0].vectorStack[Inters[0].vectorSize-1][j];
                                }
                                break;
                            }
                        }*/
                    }
                    counter2++;
                    if(NFE >= NFEmax)
                        break;
                }
                if(counter2 - TotalNFEUsed > counter2limit)
                    break;
                if(counter2 > counter2limit && NFE < counter2limit)
                    break;
            }
            Perf[index][4+perf_counter] = fbest - func_num*100;
            perf_counter++;
            totalNNOPS += Inters[0].NNOOPS;

            for(int i=0;i!=popsize;i++)
            {
                delete Pop[i];
            }
            delete Pop;
            delete Fit;
        }
    }
    Perf[index][0] = IndLength[index];
    Perf[index][1] = totalNNOPS;
    Perf[index][2] = 0;
    if(TotalNFEUsed < MaxNFEUsed)
    {
        for(int i=4;i!=nrows+4;i++)
            Perf[index][i] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
    }
    double sumPerf = 0;
    for(int i=4;i!=nrows+4;i++)
    {
        tempSort[i-4] = Perf[index][i];
        sumPerf += Perf[index][i];
    }
    qSort1(tempSort,0,nrows-1);
    Fitness[index] = tempSort[((nrows)>>1)];
    Perf[index][3] = Fitness[index];

    //cout<<index<<"\t"<<"\t"<<IntRandom2(10)<<endl;
}

void PushGP::FitCalcPopBasedCEC_LPSR(int index)
{
    /*unsigned long seed1 = globalseed;
    unsigned long seed2 = globalseed;
    unsigned long seed3 = globalseed;
    unsigned long seed4 = globalseed;
    generator_uni_i2.seed(seed1);
    generator_uni_r2.seed(seed2);
    generator_norm2.seed(seed3);
    generator_cauchy2.seed(seed4);*/

    Inters[0].verbose = false;
    int totalNNOPS = 0;
    //outputfinal = true;
    if(outputfinal)
    {
        Inters[0].verbose = true;
        Inters[0].reset(IndLength[index]);
        Inters[0].setProgram(Popul[index],IndLength[index]);
        ofstream fout("result_program.txt");
        fout<<Inters[0].printProgram();
    }
    int perf_counter = 0;
    int TotalNFEUsed = 0;
    int LimRuns = NCECRuns;
    int LimFunc = MaxFuncs;
    /*for(int i=0;i!=NInds*2;i++)
    {
        if(Perf[i][4] < 1e300)
        {
            LimRuns = NCECRuns;
            LimFunc = MaxFuncs;
            break;
        }
    }*/
    int MaxNFEUsed = NFEmax*LimRuns*LimFunc;
    for(int run=0;run!=LimRuns;run++)
    {
        for(func_num=1;func_num!=1+LimFunc;func_num++)
        {
            int popsize = GlobalPopSizeMult*GNVars;//(IntRandom2(6)+5)*GNVars; //5*GNVars; //
            //maxRandomInt = popsize;
            double** Pop = new double*[popsize];
            double* Fit = new double[popsize];
            for(int i=0;i!=popsize;i++)
            {
                Pop[i] = new double[GNVars];
                for(int j=0;j!=GNVars;j++)
                {
                    Pop[i][j] = Random2(CEC_Left[j],CEC_Right[j]);
                }
            }
            NFE = 0;

            vector<double> tmp2;
            tmp2.resize(GNVars);
            fbest = numeric_limits<double>::max();
            int counter2 = 0;
            int counter2limit = 300;
            for(int i=0;i!=popsize;i++)
            {
                for(int d=0;d!=GNVars;d++)
                {
                    tmp2[d] = (Pop[i][d] - CEC_Shifts[d])/CEC_Streches[d];
                }
                Fit[i] = CEC17(tmp2);
                TotalNFEUsed++;
                fbest = min(fbest,Fit[i]);
            }
            int MaxPopSize = popsize;
            int MinPopSize = 4;
            while(NFE < NFEmax)
            {
                for(int i=0;i!=popsize;i++)
                {
                    //GlobalIntRandMult = popsize;
                    Inters[0].reset(IndLength[index]);
                    Inters[0].setProgram(Popul[index],IndLength[index]);
                    for(int k=0;k!=popsize;k++)
                    {
                        Inters[0].floatStack[Inters[0].floatSize] = Fit[k];
                        Inters[0].floatSize++;
                        Inters[0].updateFloatSize();

                        Inters[0].vectorStack[Inters[0].vectorSize].resize(GNVars);
                        for(int j=0;j!=GNVars;j++)
                        {
                            Inters[0].vectorStack[Inters[0].vectorSize][j] = Pop[k][j];
                        }
                        Inters[0].vectorSize++;
                        Inters[0].updateVectorSize();
                    }
                    Inters[0].intStack[Inters[0].intSize] = GNVars;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].intStack[Inters[0].intSize] = popsize;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].intStack[Inters[0].intSize] = i;
                    Inters[0].intSize++;
                    Inters[0].updateIntSize();
                    Inters[0].instInpStack[Inters[0].InpSize] = 11000;
                    Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
                    for(int i=0;i!=GNVars;i++)
                    {
                        Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Left[i];
                    }
                    Inters[0].InpSize++;
                    Inters[0].updateInpSize();

                    Inters[0].instInpStack[Inters[0].InpSize] = 11000;
                    Inters[0].instInpConstVStack[Inters[0].InpSize].resize(GNVars);
                    for(int i=0;i!=GNVars;i++)
                    {
                        Inters[0].instInpConstVStack[Inters[0].InpSize][i] = CEC_Right[i];
                    }
                    Inters[0].InpSize++;
                    Inters[0].updateInpSize();

                    int counter = 0;
                    while(Inters[0].NESize > 0)
                    {
                        Inters[0].instStep();
                        counter++;
                        if(counter > counterlimit)
                        {
                            break;
                        }
                        if(outputfinal)
                        {
                            cout<<Inters[0].printStacks()<<endl;
                            getchar();
                        }
                    }
                    double tempfit = 0;
                    if(Inters[0].vectorSize > 0 && counter < counterlimit)
                    {
                        int nbad = 0;
                        int nbad2 = 0;
                        for(int j=0;j!=GNVars;j++)
                        {
                            if(Inters[0].vectorStack[Inters[0].vectorSize-1][j] > CEC_Right[j] || Inters[0].vectorStack[Inters[0].vectorSize-1][j] < CEC_Left[j])
                            {
                                nbad++;
                            }
                            if(Inters[0].vectorStack[Inters[0].vectorSize-1][j] == Pop[popsize-1][j])
                            {
                                nbad2++;
                            }
                        }
                        if(nbad > 0)
                        {
                            tempfit = numeric_limits<double>::max();
                        }
                        else if(nbad2 == GNVars && GlobalMode == 0)
                        {
                            //cout<<"nbad2 "<<endl;
                            tempfit = numeric_limits<double>::max();
                        }
                        else
                        {
                            //cout<<"calc"<<endl;
                            for(int d=0;d!=GNVars;d++)
                            {
                                tmp2[d] = (Inters[0].vectorStack[Inters[0].vectorSize-1][d] - CEC_Shifts[d])/CEC_Streches[d];
                            }
                            tempfit = CEC17(tmp2);
                            TotalNFEUsed++;
                            if(tempfit < Fit[i])
                            {
                                Fit[i] = tempfit;
                                for(int j=0;j!=GNVars;j++)
                                {
                                    Pop[i][j] = Inters[0].vectorStack[Inters[0].vectorSize-1][j];
                                }
                            }
                            fbest = min(fbest,tempfit);
                        }
                        /*for(int k=0;k!=popsize;k++)
                        {
                            if(tempfit < Fit[k])
                            {
                                fbest = min(fbest,tempfit);
                                Fit[k] = tempfit;
                                for(int j=0;j!=GNVars;j++)
                                {
                                    Pop[k][j] = Inters[0].vectorStack[Inters[0].vectorSize-1][j];
                                }
                                break;
                            }
                        }*/
                    }
                    counter2++;
                    if(NFE >= NFEmax)
                        break;
                }
                if(counter2 - TotalNFEUsed > counter2limit)
                    break;
                if(counter2 > counter2limit && NFE < counter2limit)
                    break;

                int newpopsize = int(double(MinPopSize-MaxPopSize)/double(NFEmax)*NFE + MaxPopSize);

                int PointsToRemove = popsize - newpopsize;
                for(int L=0;L!=PointsToRemove;L++)
                {
                    double WorstFit = Fit[0];
                    int WorstNum = 0;
                    for(int i=1;i!=popsize;i++)
                    {
                        if(Fit[i] > WorstFit)
                        {
                            WorstFit = Fit[i];
                            WorstNum = i;
                        }
                    }
                    for(int i=WorstNum;i!=popsize-1;i++)
                    {
                        for(int j=0;j!=GNVars;j++)
                            Pop[i][j] = Pop[i+1][j];
                        Fit[i] = Fit[i+1];
                    }
                }
                popsize = newpopsize;
            }
            Perf[index][4+perf_counter] = fbest - func_num*100;
            perf_counter++;
            totalNNOPS += Inters[0].NNOOPS;

            for(int i=0;i!=MaxPopSize;i++)
            {
                delete Pop[i];
            }
            delete Pop;
            delete Fit;
        }
    }
    Perf[index][0] = IndLength[index];
    Perf[index][1] = totalNNOPS;
    Perf[index][2] = 0;
    if(TotalNFEUsed < MaxNFEUsed)
    {
        for(int i=4;i!=nrows+4;i++)
            Perf[index][i] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
    }
    double sumPerf = 0;
    for(int i=4;i!=nrows+4;i++)
    {
        tempSort[i-4] = Perf[index][i];
        sumPerf += Perf[index][i];
    }
    qSort1(tempSort,0,nrows-1);
    Fitness[index] = tempSort[((nrows)>>1)];
    Perf[index][3] = Fitness[index];
    for(int i=0;i!=nrows+4;i++)
    {
        if(isnan(Perf[index][i]))
            Perf[index][i] = numeric_limits<double>::max();
        if(isinf(Perf[index][i]))
            Perf[index][i] = numeric_limits<double>::max();
    }
    //cout<<index<<"\t"<<"\t"<<IntRandom2(10)<<endl;
}

void PushGP::FitCalcClassifReg(int index)
{
    //cout<<"index "<<index<<endl;
    Inters[0].verbose = false;
    //unsigned t0=clock(),t1;
    int totalNNOPS = 0;
    if(outputfinal)
    {
        Inters[0].verbose = true;
        Inters[0].reset(IndLength[index]);
        Inters[0].setProgram(Popul[index],IndLength[index]);
        ofstream fout("result_program.txt");
        fout<<Inters[0].printProgram();
    }
    int perf_counter = 0;

    int SSize = 100;
    int NVars = 1;
    int NOuts = 1;
    double** DataX = new double*[SSize];
    double** DataY = new double*[SSize];
    for(int i=0;i!=SSize;i++)
    {
        DataX[i] = new double[NVars];
        DataY[i] = new double[NOuts];
        for(int j=0;j!=NVars;j++)
        {
            DataX[i][j] = 0.1*i;
        }
        for(int j=0;j!=NOuts;j++)
        {
            DataY[i][j] = DataX[i][j]*DataX[i][j] + DataX[i][j];
        }
    }
    int TotalNFEUsed = 0;
    int MaxNFEUsed = NCECRuns*SSize;
    for(int run=0;run!=NCECRuns;run++)
    {
        for(int point=0;point!=SSize;point++)
        {
            Inters[0].reset(IndLength[index]);
            Inters[0].setProgram(Popul[index],IndLength[index]);
            Inters[0].instInpStack[Inters[0].InpSize] = 1000;
            Inters[0].instInpConstFStack[Inters[0].InpSize] = DataX[point][0];
            Inters[0].InpSize++;
            Inters[0].updateInpSize();
            int counter = 0;
            while(Inters[0].NESize > 0)
            {
                if(outputfinal)
                {
                    cout<<Inters[0].printStacks()<<endl;
                    getchar();
                }
                Inters[0].instStep();
                counter++;
                if(counter > counterlimit)
                    break;
            }
            double res = 0;
            if(Inters[0].floatSize > 0)
            {
                res = Inters[0].floatStack[0];
                TotalNFEUsed += 1;
                Perf[index][4+perf_counter] = (res - DataY[point][0])*(res - DataY[point][0]);
            }
            else
            {
                Perf[index][4+perf_counter] = std::numeric_limits<double>::max();
            }
            if(outputfinal)
            {
                cout<<Inters[0].printStacks()<<endl;
            }
            perf_counter++;
            totalNNOPS += Inters[0].NNOOPS;
        }
    }
    //t1=clock()-t0;
    Perf[index][0] = 0;
    Perf[index][1] = 0;
    Perf[index][2] = 0;
    if(IndLength[index] > 0)
    {
        Perf[index][0] = IndLength[index];
        Perf[index][1] = totalNNOPS;
        Perf[index][2] = 0;//double(t1);
    }
    if(TotalNFEUsed < MaxNFEUsed)
    {
        Perf[index][0] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
        Perf[index][1] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
        Perf[index][2] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
    }

    double sumPerf = 0;
    for(int i=4;i!=nrows+4;i++)
    {
        tempSort[i-4] = Perf[index][i];
        sumPerf += Perf[index][i];
    }
    //qSort1(tempSort,0,PerfSize-5);
    Fitness[index] = sumPerf;//tempSort[((PerfSize-4)>>1)];
    Perf[index][3] = Fitness[index];
    for(int i=0;i!=SSize;i++)
    {
        delete DataX[i];
        delete DataY[i];
    }
    delete DataX;
    delete DataY;
}

void PushGP::FitCalcDE(int index)
{
    Inters[0].verbose = false;
    //unsigned t0=clock(),t1;
    int totalNNOPS = 0;
    if(outputfinal)
    {
        Inters[0].verbose = true;
        Inters[0].reset(IndLength[index]);
        Inters[0].setProgram(Popul[index],IndLength[index]);
        ofstream fout("result_program.txt");
        fout<<Inters[0].printProgram();
    }
    int perf_counter = 0;
    //int TotalNFEUsed = 0;
    double totalStackSize = 0;
    //int MaxNFEUsed = NCECRuns*MaxFuncs;

    for(int run=0;run!=NCECRuns;run++)
    {
        int func_index = 0;
        for(func_num=1;func_num!=2+MaxFuncs;func_num++)
        //for(func_num=FuncNum_;func_num!=FuncNum_+1;func_num++)
        {
            if(func_num == 2)
                continue;
            MaxFEval_DE = NFEmax;
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
            double* xopt = new double[GNVars];
            fopt = GetOptimum(func_num,xopt);
            delete xopt;
            Optimizer OptZ;
            OptZ.Initialize(CurPRS.PopSizeLoop*GNVars,
                            GNVars,
                            func_num,
                            func_index,
                            CurPRS.MemorySizePar,
                            CurPRS.initF,
                            CurPRS.initCr,
                            CurPRS.pBestStart,
                            CurPRS.pBestEnd,
                            MaxFEval_DE);
            while(NFEval_DE < MaxFEval_DE)
            {
                //OptZ.MainCycle();
                double minfit = OptZ.FitMassFront[0];
                double maxfit = OptZ.FitMassFront[0];
                for(int i=0;i!=OptZ.NIndsFront;i++)
                {
                    OptZ.FitMassCopy2[i] = OptZ.FitMassFront[i];
                    OptZ.Indexes2[i] = i;
                    maxfit = max(maxfit,OptZ.FitMassFront[i]);
                    minfit = min(minfit,OptZ.FitMassFront[i]);
                }
                if(minfit != maxfit)
                    qSort2int(OptZ.FitMassCopy2,OptZ.Indexes2,0,OptZ.NIndsFront-1);
                for(OptZ.IndIter=0;OptZ.IndIter<OptZ.NIndsFront;OptZ.IndIter++)
                {
                    OptZ.MemoryCurrentIndex = IntRandom(OptZ.MemorySize);
                    if(OptZ.classic)
                    {
                        do
                            OptZ.F = CauchyRand(OptZ.MemoryF[OptZ.MemoryCurrentIndex],0.1);
                        while(OptZ.F < 0.0);
                        OptZ.F = min(OptZ.F,1.0);
                    }
                    else
                    {
                        //Inters[0].minRandomFloat = 0;
                        //Inters[0].minRandomFloat = 1;
                        Inters[0].reset(IndLength[index]);
                        Inters[0].setProgram(Popul[index],IndLength[index]);

                        Inters[0].instInpStack[Inters[0].InpSize] = 1000;
                        Inters[0].instInpConstFStack[Inters[0].InpSize] = OptZ.SuccessRate;
                        Inters[0].InpSize++;
                        Inters[0].updateInpSize();

                        Inters[0].instInpStack[Inters[0].InpSize] = 1000;
                        Inters[0].instInpConstFStack[Inters[0].InpSize] = OptZ.meanF;
                        Inters[0].InpSize++;
                        Inters[0].updateInpSize();

                        Inters[0].instInpStack[Inters[0].InpSize] = 1000;
                        Inters[0].instInpConstFStack[Inters[0].InpSize] = double(NFEval_DE) / double(MaxFEval_DE);
                        Inters[0].InpSize++;
                        Inters[0].updateInpSize();

                        int counter = 0;
                        while(Inters[0].NESize > 0 && counter < counterlimit)
                        {
                            /*if(outputfinal)
                            {
                                cout<<Inters[0].printStacks()<<endl;
                                getchar();
                            }*/
                            Inters[0].instStep();
                            counter++;
                        }
                        if(Inters[0].floatSize > 0)
                        {
                            OptZ.NewF = Inters[0].floatStack[0];
                            //TotalNFEUsed += 1;
                        }
                        else
                        {
                            OptZ.NewF = 0;
                        }
                        totalStackSize += double(Inters[0].boolSize);
                        totalStackSize += double(Inters[0].intSize);
                        totalStackSize += double(Inters[0].floatSize);
                        totalStackSize += double(Inters[0].vectorSize);
                        totalStackSize += double(Inters[0].nameSize);
                        totalStackSize += double(Inters[0].NESize);
                        totalStackSize += double(Inters[0].NISize);
                        if(outputfinal)
                        {
                            cout<<Inters[0].printStacks()<<endl;
                        }
                        //OptZ.F = CauchyRand(OptZ.NewF,0.1);
                        OptZ.F = OptZ.NewF;
                    }
                    OptZ.FGenerated[OptZ.IndIter] = OptZ.F;
                    if(OptZ.MemoryCr[OptZ.MemoryCurrentIndex] < 0)
                        OptZ.Cr = 0;
                    else
                        OptZ.Cr = NormRand(OptZ.MemoryCr[OptZ.MemoryCurrentIndex],0.1);
                    OptZ.Cr = min(max(OptZ.Cr,0.0),1.0);
                    OptZ.CrGeneratedTemp[OptZ.IndIter] = OptZ.Cr;
                    OptZ.CrGenerated[OptZ.IndIter] = OptZ.Cr;
                }
                if(SortCrossRates == 1)
                {
                    qSort1(OptZ.CrGeneratedTemp,0,OptZ.NIndsFront-1);
                    for(OptZ.IndIter=0;OptZ.IndIter<OptZ.NIndsFront;OptZ.IndIter++)
                        OptZ.CrGenerated[OptZ.Indexes2[OptZ.IndIter]] = OptZ.CrGeneratedTemp[OptZ.IndIter];
                }
                int prand;
                int psizeval;
                OptZ.psize = OptZ.psizeParamStart + ((double)NFEval_DE/(double)MaxFEval_DE) * (OptZ.psizeParamEnd-OptZ.psizeParamStart);
                psizeval = OptZ.NIndsFront*OptZ.psize;
                if(psizeval <= 1)
                    psizeval = 2;
                for(OptZ.IndIter=0;OptZ.IndIter<OptZ.NIndsFront;OptZ.IndIter++)
                {
                    OptZ.TheChosenOne = OptZ.IndIter;
                    do
                        prand = OptZ.Indexes2[IntRandom(psizeval)];
                    while(prand == OptZ.TheChosenOne);

                    int Rand1 = 0;
                    int Rand2 = 0;
                    do
                    {
                        Rand1 = IntRandom(OptZ.NIndsFront);
                    }
                    while(Rand1 == prand);
                    do
                    {
                        Rand2 = IntRandom(OptZ.NIndsFront);
                    }
                    while(Rand2 == prand || Rand2 == Rand1);
                    OptZ.F = OptZ.FGenerated[OptZ.IndIter];
                    for(int j=0;j!=OptZ.NVars;j++)
                        OptZ.Donor[j] = OptZ.PopulFront[OptZ.TheChosenOne][j] + OptZ.F*(OptZ.PopulFront[prand][j] - OptZ.PopulFront[OptZ.TheChosenOne][j])
                                                                              + OptZ.F*(OptZ.PopulFront[Rand1][j] - OptZ.PopulFront[Rand2][j]);
                    FindLimits(OptZ.Donor,OptZ.NVars,OptZ.Left,OptZ.Right);
                    int WillCrossover = IntRandom(OptZ.NVars);
                    OptZ.Cr = OptZ.CrGenerated[OptZ.TheChosenOne];
                    for(int j=0;j!=OptZ.NVars;j++)
                    {
                        if(Random(0,1) < OptZ.Cr || WillCrossover == j)
                            OptZ.Trial[j] = OptZ.Donor[j];
                        else
                            OptZ.Trial[j] = OptZ.PopulFront[OptZ.TheChosenOne][j];
                    }
                    double TempFit = cec_22_(OptZ.Trial,OptZ.func_num);
                    if(TempFit <= OptZ.FitMassFront[OptZ.TheChosenOne])
                    {
                        OptZ.PFIndex = OptZ.IndIter;
                        for(int j=0;j!=OptZ.NVars;j++)
                        {
                            OptZ.PopulFront[OptZ.PFIndex][j] = OptZ.Trial[j];
                        }
                        if(TempFit < globalbest)
                            globalbest = TempFit;
                        OptZ.SaveSuccessCrF(OptZ.Cr,OptZ.F,fabs(OptZ.FitMassFront[OptZ.TheChosenOne]-TempFit));
                        OptZ.FitMassFront[OptZ.PFIndex] = TempFit;
                    }
                    SaveBestValues(OptZ.func_index);
                }
                OptZ.SuccessRate = double(OptZ.SuccessFilled)/double(OptZ.NIndsFront);
                OptZ.newNIndsFront = int(double(OptZ.NIndsFrontMin-OptZ.NIndsFrontMax)/double(MaxFEval_DE)*NFEval_DE + OptZ.NIndsFrontMax);
                if(OptZ.newNIndsFront < OptZ.NIndsFrontMin)
                    OptZ.newNIndsFront = OptZ.NIndsFrontMin;
                OptZ.RemoveWorst(OptZ.NIndsFront,OptZ.newNIndsFront);
                OptZ.NIndsFront = OptZ.newNIndsFront;
                OptZ.UpdateMemoryCrF();
                OptZ.SuccessFilled = 0;
                OptZ.Generation++;
            }
            OptZ.Clean();
            //for(int j=0;j!=ResTsize2;j++)
            for(int j=0;j!=ResTsize2;j++)
            {
                Perf[index][4+perf_counter] = ResultsArray[func_index][j];
                if(isnan(Perf[index][4+perf_counter]) || isinf(Perf[index][4+perf_counter]))
                    Perf[index][4+perf_counter] = std::numeric_limits<double>::max();
                perf_counter++;
            }
            /*for(int j=0;j!=2;j++)
            {
                Perf[index][4+perf_counter] = ResultsArray[func_index][ResTsize2 - 2 + j];
                if(isnan(Perf[index][4+perf_counter]) || isinf(Perf[index][4+perf_counter]))
                    Perf[index][4+perf_counter] = std::numeric_limits<double>::max();
                perf_counter++;
            }*/
            totalNNOPS += Inters[0].NNOOPS;
            func_index++;
        }
    }
    //t1=clock()-t0;
    Perf[index][0] = IndLength[index];//abs(IndLength[index]-MaxInitSize);
    Perf[index][1] = totalNNOPS;
    Perf[index][2] = totalStackSize;//double(t1);

    /*if(IndLength[index] > 128)
    {
        //  Perf[index][0] = IndLength[index] - 256;
        Perf[index][0] = IndLength[index];//abs(IndLength[index]-MaxInitSize);
        Perf[index][1] = totalNNOPS;
        Perf[index][2] = totalStackSize;//double(t1);
    }*/

    /*if(TotalNFEUsed < MaxNFEUsed)
    {
        for(int i=4;i!=nrows+4;i++)
            Perf[index][i] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
        //Perf[index][1] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
        //Perf[index][2] = double(MaxNFEUsed-TotalNFEUsed)/double(MaxNFEUsed)*numeric_limits<double>::max();
    }*/

    double sumPerf = 0;
    for(int i=4;i!=nrows+4;i++)
    {
        tempSort[i-4] = Perf[index][i];
        sumPerf += Perf[index][i];
    }
    //cout<<"END3"<<endl;
    qSort1(tempSort,0,nrows-1);
    Fitness[index] = tempSort[((nrows)>>1)];//sumPerf;//
    Perf[index][3] = Fitness[index];
    //cout<<"END4"<<endl;
    //delete tempSort;
    //cout<<"END5"<<endl;
}

void PushGP::SelectTour(int index)
{
    int bestindex = 0;
    double bestrank = 0;
    for(int i=0;i!=TourSize;i++)
    {
        Rands[i] = IntRandom(NInds);
        if(Ranks[Rands[i]] < bestrank || i == 0)
        {
            bestindex = i;
            bestrank = Rands[i];
        }
    }
    IndLength[index+NInds] = IndLength[bestindex];
    for(int i=0;i!=IndLength[index+NInds];i++)
    {
        Popul[index+NInds][i] = Popul[bestindex][i];
    }
}
void PushGP::PrepareLexicase(int SizeMultiplier)
{
    for(int c=0;c!=PerfSize;c++)
    {
        for(int i=0;i!=NInds*SizeMultiplier;i++)
        {
            SortPerf[i] = Perf[i][c];
        }
        qSort1(SortPerf,0,NInds*SizeMultiplier-1);
        MedAbsDist[c] = SortPerf[(NInds*SizeMultiplier) >> 1];
        OrderCases[c] = c;
    }
}
void PushGP::SelectLexicase(int index, int SizeMultiplier)
{
    for(int i=0;i!=PerfSize*5;i++)
    {
        swap(OrderCases[IntRandom(PerfSize)],OrderCases[IntRandom(PerfSize)]);
    }
    for(int i=0;i!=NInds*SizeMultiplier;i++)
    {
        IndActive[i] = 1;
    }
    int CaseIndex = 0;
    int NActive = NInds*SizeMultiplier;
    while(NActive > 1 && CaseIndex < PerfSize)
    {
        int CurCase = OrderCases[CaseIndex];
        double tempBest = Perf[0][CurCase];
        for(int i=0;i!=NInds*SizeMultiplier;i++)
        {
            if(Perf[i][CurCase] < tempBest)
                tempBest = Perf[i][CurCase];
        }
        double Med = MedAbsDist[CurCase];
        for(int i=0;i!=NInds*SizeMultiplier;i++)
        {
            if(fabs(tempBest - Perf[i][CurCase]) > Med)
            {
                NActive -= IndActive[i];
                IndActive[i] = 0;
            }
            if(NActive == 1)
                break;
        }
        CaseIndex++;
    }
    int Selected = IntRandom(NActive);
    for(int i=0;i!=NInds*SizeMultiplier;i++)
    {
        if(Selected == 0 && IndActive[i] == 1)
        {
            Selected = i;
            break;
        }
        Selected -= IndActive[i];
    }
    IndLength[index+NInds] = IndLength[Selected];
    for(int i=0;i!=IndLength[index+NInds];i++)
    {
        Popul[index+NInds][i] = Popul[Selected][i];
    }
}
void PushGP::SelectTour2(int index)
{
    int rand1 = IntRandom(NInds);
    int rand2 = IntRandom(NInds);
    //if(Fitness[rand1] < Fitness[rand2])
    if(Ranks[rand2] < Ranks[rand1])
        rand1 = rand2;

    //delete[] Popul[index+NInds];
    IndLength[index+NInds] = IndLength[rand1];
    //Popul[index+NInds] = new atomInst[IndLength[index+NInds]];
    for(int i=0;i!=IndLength[index+NInds];i++)
    {
        Popul[index+NInds][i] = Popul[rand1][i];
    }
}
void PushGP::Crossover(int index)
{
    int point1;
    int point2;
    int point3;
    int point4;
    int CurLen2;
    int NewSize;

    int iter=0;
    do
    {
        int CurLen1 = IndLength[index];
        CurLen2 = IndLength[index+NInds];

        point1 = IntRandom(CurLen1-1);
        point2 = IntRandom(CurLen1-1)+1;
        if(point2 < point1)
            swap(point1,point2);

        point3 = IntRandom(CurLen2-1);
        point4 = IntRandom(CurLen2-1)+1;
        if(point4 < point3)
            swap(point3,point4);

        NewSize = CurLen2 - (point4 - point3) + (point2 - point1);
        iter++;
        if(iter > 128)
            break;
    }
    while(NewSize >= MaxLength || NewSize < 1);

    if(NewSize > 0)
    {
        //cout<<NewSize<<endl;
        //atomInst* tempInd = new atomInst[NewSize];
        for(int i=0;i!=point3;i++)
        {
            tempInd[i] = Popul[index+NInds][i];
            //cout<<i<<endl;
        }
        for(int i=0;i!=(point2-point1);i++)
        {
            tempInd[point3+i] = Popul[index][i+point1];
            //cout<<point3+i<<"\t"<<i+point1<<endl;
        }
        for(int i=0;i!=CurLen2-point4;i++)
        {
            tempInd[point3+(point2-point1)+i] = Popul[index+NInds][point4+i];
            //cout<<point3+(point2-point1)+i<<"\t"<<point4+i<<endl;
        }
        //delete[] Popul[index+NInds];
        for(int i=0;i!=NewSize;i++)
        {
            Popul[index+NInds][i] = tempInd[i];
        }
        //Popul[index+NInds] = tempInd;
        IndLength[index+NInds] = NewSize;
    }
}
void PushGP::CrossoverAdd(int index)
{
    int point1;
    int point2;
    int point3;
    int point4;
    int CurLen2;
    int NewSize;

    int iter=0;
    do
    {
        int CurLen1 = IndLength[index];
        CurLen2 = IndLength[index+NInds];

        point1 = IntRandom(CurLen1-1);
        point2 = IntRandom(CurLen1-1)+1;
        if(point2 < point1)
            swap(point1,point2);

        point3 = IntRandom(CurLen2-1);
        //point4 = IntRandom(CurLen2-1)+1;
        //if(point4 < point3)
            //swap(point3,point4);
        point4 = point3;

        NewSize = CurLen2 - (point4 - point3) + (point2 - point1);
        iter++;
        if(iter > 128 && NewSize > 0)
            return;
    }
    while(NewSize >= MaxLength || NewSize < 1);

    if(NewSize > 0)
    {
        //cout<<NewSize<<endl;
        //atomInst* tempInd = new atomInst[NewSize];
        for(int i=0;i!=point3;i++)
        {
            tempInd[i] = Popul[index+NInds][i];
            //cout<<i<<endl;
        }
        for(int i=0;i!=(point2-point1);i++)
        {
            tempInd[point3+i] = Popul[index][i+point1];
            //cout<<point3+i<<"\t"<<i+point1<<endl;
        }
        for(int i=0;i!=CurLen2-point4;i++)
        {
            tempInd[point3+(point2-point1)+i] = Popul[index+NInds][point4+i];
            //cout<<point3+(point2-point1)+i<<"\t"<<point4+i<<endl;
        }
        //delete[] Popul[index+NInds];
        for(int i=0;i!=NewSize;i++)
        {
            Popul[index+NInds][i] = tempInd[i];
        }
        //Popul[index+NInds] = tempInd;
        IndLength[index+NInds] = NewSize;
    }
}
void PushGP::CrossoverULT(int index)
{
    int CurLen1 = IndLength[index];
    int CurLen2 = IndLength[index+NInds];
    int NewSize = min(CurLen1,CurLen2);
    int FinalSize = NewSize;
    double prob = Random(0.0,0.1);

    for(int i=0;i!=NewSize;i++)
    {
        if(Random(0.0,1.0) < 0.1)
        {
            tempInd[i] = Popul[index][i];
        }
        else
        {
            tempInd[i] = Popul[index+NInds][i];
        }
        if(Random(0.0,1.0) < 0.1)
        {
            if(prob < 0.1)
                prob = Random(0.9,1.0);
            else
                prob = Random(0.0,0.1);
        }
    }
    if(CurLen1 > NewSize)
    {
        for(int i=NewSize;i!=CurLen1;i++)
        {
            tempInd[i] = Popul[index][i];
            FinalSize++;
            if(Random(0.0,1.0) < 0.1)
                break;
        }
    }
    else if(CurLen2 > NewSize)
    {
        for(int i=NewSize;i!=CurLen2;i++)
        {
            tempInd[i] = Popul[index+NInds][i];
            FinalSize++;
            if(Random(0.0,1.0) < 0.1)
                break;
        }
    }
    for(int i=0;i!=FinalSize;i++)
    {
        Popul[index+NInds][i] = tempInd[i];
    }
    IndLength[index+NInds] = FinalSize;
}
void PushGP::Mutate(int index)
{
    int point1;
    int point2;
    int MutSize;
    int NewLen;
    int iter=0;
    do
    {
        int CurLen = IndLength[index+NInds];

        point1 = IntRandom(CurLen-1)+1;
        point2 = IntRandom(CurLen-1)+1;
        if(point2 < point1)
            swap(point1,point2);

        int minMutSize = 1;
        int maxMutSize = 4;

        MutSize = IntRandom(maxMutSize - minMutSize) + minMutSize;
        NewLen = CurLen + MutSize - (point2-point1);
        iter++;
        if(iter > 128)
            break;
    }
    while(NewLen >= MaxLength || NewLen < 1);

    //cout<<CurLen<<"\t"<<NewLen<<endl;

    //atomInst* tempInd = new atomInst[NewLen];

    for(int i=0;i!=point1;i++)
    {
        tempInd[i] = Popul[index+NInds][i];
    }
    for(int i=point1;i!=point1+MutSize;i++)
    {
        int InstType;// = IntRandom(NTypes)+1;
        int InstIndex;// = IntRandom(NTypeInst[InstType-1]) + 1000*InstType;
        InstIndex = IntRandom(NActiveInst);
        //InstIndex = 1;
        int last = 0;
        for(int k=0;k!=NTypes;k++)
        {
            if(InstIndex < last + NTypeInst[k])
            {
                InstType = k+1;
                InstIndex = InstIndex - last + 1000*InstType;
                break;
            }
            else
                last += NTypeInst[k];
        }
        tempInd[i].inst = InstIndex;
        if(InstIndex == 1000)
        {
            tempInd[i].f = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
        }
        if(InstIndex == 2000)
        {
            tempInd[i].i = Random(Inters[0].minRandomInt,Inters[0].maxRandomInt);
        }
        if(InstIndex == 7000)
        {
            string randstr;
            int randlen;
            randlen = IntRandom(2)+2;
            randstr.resize(randlen);
            for (int k = 0; k < randlen; k++)
                randstr[k] = Inters[0].charset[IntRandom(52)];
            tempInd[i].n = randstr;
        }
        if(InstIndex == 11000)
        {
            vector<double> newvec;
            int DIM = Inters[0].DIM;
            newvec.resize(DIM);
            for (int k = 0; k < DIM; k++)
                newvec[k] = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
            tempInd[i].v = newvec;
        }
    }
    int k = point2;
    for(int i=point1+MutSize;i!=NewLen;i++)
    {
        tempInd[i] = Popul[index+NInds][k];
        k++;
    }
    //delete[] Popul[index+NInds];
    //Popul[index+NInds] = tempInd;
    for(int i=0;i!=NewLen;i++)
    {
        Popul[index+NInds][i] = tempInd[i];
    }
    IndLength[index+NInds] = NewLen;
}
void PushGP::MutateAdd(int index)
{
    int point1;
    int point2;
    int MutSize;
    int NewLen;
    int iter=0;
    do
    {
        int CurLen = IndLength[index+NInds];

        point1 = IntRandom(CurLen-1)+1;
        //point2 = IntRandom(CurLen-1)+1;
        //if(point2 < point1)
            //swap(point1,point2);
        point2 = point1;

        int minMutSize = 1;
        int maxMutSize = 4;

        MutSize = IntRandom(maxMutSize - minMutSize) + minMutSize;
        NewLen = CurLen + MutSize - (point2-point1);
        iter++;
        if(iter > 128 && NewLen > 0)
            return;
    }
    while(NewLen >= MaxLength || NewLen < 1);

    //cout<<CurLen<<"\t"<<NewLen<<endl;

    //atomInst* tempInd = new atomInst[NewLen];

    for(int i=0;i!=point1;i++)
    {
        tempInd[i] = Popul[index+NInds][i];
    }
    for(int i=point1;i!=point1+MutSize;i++)
    {
        int InstType;// = IntRandom(NTypes)+1;
        int InstIndex;// = IntRandom(NTypeInst[InstType-1]) + 1000*InstType;
        InstIndex = IntRandom(NActiveInst);
        //InstIndex = 1;
        int last = 0;
        for(int k=0;k!=NTypes;k++)
        {
            if(InstIndex < last + NTypeInst[k])
            {
                InstType = k+1;
                InstIndex = InstIndex - last + 1000*InstType;
                break;
            }
            else
                last += NTypeInst[k];
        }
        tempInd[i].inst = InstIndex;
        if(InstIndex == 1000)
        {
            tempInd[i].f = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
        }
        if(InstIndex == 2000)
        {
            tempInd[i].i = Random(Inters[0].minRandomInt,Inters[0].maxRandomInt);
        }
        if(InstIndex == 7000)
        {
            string randstr;
            int randlen;
            randlen = IntRandom(2)+2;
            randstr.resize(randlen);
            for (int k = 0; k < randlen; k++)
                randstr[k] = Inters[0].charset[IntRandom(52)];
            tempInd[i].n = randstr;
        }
        if(InstIndex == 11000)
        {
            vector<double> newvec;
            int DIM = Inters[0].DIM;
            newvec.resize(DIM);
            for (int k = 0; k < DIM; k++)
                newvec[k] = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
            tempInd[i].v = newvec;
        }
    }
    int k = point2;
    for(int i=point1+MutSize;i!=NewLen;i++)
    {
        tempInd[i] = Popul[index+NInds][k];
        k++;
    }
    //delete[] Popul[index+NInds];
    //Popul[index+NInds] = tempInd;
    for(int i=0;i!=NewLen;i++)
    {
        Popul[index+NInds][i] = tempInd[i];
    }
    IndLength[index+NInds] = NewLen;
}
void PushGP::MutateElWise(int index)
{
    int MutSize = 0;
    int CurLen = IndLength[index+NInds];
    //atomInst* tempInd = new atomInst[NewLen];
    double prob = 1.0/double(CurLen);
    int iter1 = 0;
    do
    {
        for(int i=0;i!=CurLen;i++)
        {
            if(Random(0,1) > prob)
            {
                tempInd[MutSize] = Popul[index+NInds][i];
                MutSize++;
                //cout<<"c"<<i<<endl;
            }
            else
            {
                //cout<<"r"<<i<<endl;
                if(Random(0,1) < prob)
                {
                    //skip parent gene
                }
                else
                {
                    int InstType;// = IntRandom(NTypes)+1;
                    int InstIndex;// = IntRandom(NTypeInst[InstType-1]) + 1000*InstType;
                    InstIndex = IntRandom(NActiveInst);
                    //InstIndex = 1;
                    int last = 0;
                    for(int k=0;k!=NTypes;k++)
                    {
                        if(InstIndex < last + NTypeInst[k])
                        {
                            InstType = k+1;
                            InstIndex = InstIndex - last + 1000*InstType;
                            break;
                        }
                        else
                            last += NTypeInst[k];
                    }
                    tempInd[MutSize].inst = InstIndex;
                    if(InstIndex == 1000)
                    {
                        tempInd[MutSize].f = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
                    }
                    if(InstIndex == 2000)
                    {
                        tempInd[MutSize].i = Random(Inters[0].minRandomInt,Inters[0].maxRandomInt);
                    }
                    if(InstIndex == 7000)
                    {
                        string randstr;
                        int randlen;
                        randlen = IntRandom(2)+2;
                        randstr.resize(randlen);
                        for (int k = 0; k < randlen; k++)
                            randstr[k] = Inters[0].charset[IntRandom(52)];
                        tempInd[MutSize].n = randstr;
                    }
                    if(InstIndex == 11000)
                    {
                        vector<double> newvec;
                        int DIM = Inters[0].DIM;
                        newvec.resize(DIM);
                        for (int k = 0; k < DIM; k++)
                            newvec[k] = Random(Inters[0].minRandomFloat,Inters[0].maxRandomFloat);
                        tempInd[MutSize].v = newvec;
                    }
                    MutSize++;
                }
            }
        }
        //cout<<CurLen<<"\t"<<MutSize<<endl;
        iter1++;
        if(iter1 > 25)
        {
            return;
        }
    }
    while(MutSize == 0);
    //delete[] Popul[index+NInds];
    //Popul[index+NInds] = tempInd;
    for(int i=0;i!=MutSize;i++)
    {
        Popul[index+NInds][i] = tempInd[i];
    }
    IndLength[index+NInds] = MutSize;
}
void PushGP::RepairBracket(int index)
{
    int BracketCounter = 0;
    int IndLen = IndLength[index];
    int BracketPosN = 0;
    int BracketPosC = 0;
    for(int i=0;i!=IndLen;i++)
    {
        if(Popul[index][i].inst == 10000)
        {
            BracketPosOpen[BracketPosN] = i;
            BracketPosN++;
            BracketCounter++;
        }
        if(Popul[index][i].inst == 10001)
        {
            BracketPosClose[BracketPosC] = i;
            BracketPosC++;
            BracketCounter--;
        }
    }
    if(BracketCounter > 0)
    {
        for(int k=0;k<BracketCounter;k++)
        {
            BracketPosN--;
            //int LastPos = BracketPosOpen[BracketPosN];
            if(IndLen >= MaxLength-1)
                break;
            //add closing brackets
            int newPos = IndLen-1;//IntRandom(IndLen - LastPos) + LastPos;
            for(int i=newPos;i!=IndLen;i++)
            {
                Popul[index][i+1] = Popul[index][i];
            }
            Popul[index][newPos].inst = 10001;
            IndLen++;
        }
    }
    /*if(BracketCounter < 0)
    {
        for(int k=0;k<-BracketCounter;k++)
        {
            //BracketPosC--;
            int FirstPos = BracketPosClose[k];
            if(IndLen >= MaxLength-1)
                break;
            //add opening brackets
            int newPos = IntRandom(FirstPos);
            for(int i=newPos;i!=IndLen;i++)
            {
                Popul[index][i+1] = Popul[index][i];
            }
            Popul[index][newPos].inst = 10000;
            IndLen++;
        }
    }*/
    IndLength[index] = IndLen;
}
void PushGP::CalculateRanks(int SizeMultiplier, double SimilarityWeight, double LengthWeight, double NOOPSWeight, double T1Weight)
{
    for(int i=0;i!=NInds*SizeMultiplier;i++)
        Ranks[i] = 0;
    //double* TempRanks = new double[NInds*SizeMultiplier];
    //double* TempVals = new double[NInds*SizeMultiplier];
    for(int p=0;p!=PerfSize;p++)
    {
        for(int i=0;i!=NInds*SizeMultiplier;i++)
            TempVals[i] = Perf[i][p];
        get_fract_ranks(TempVals, TempRanks, NInds*SizeMultiplier);
        for(int i=0;i!=NInds*SizeMultiplier;i++)
        {
            if(p >= nrows + 4)
                Ranks[i] += (double)TempRanks[i] / (NInds*SizeMultiplier) * SimilarityWeight;//(double)1*SimilarityWeight;
            else if(p >= 4 && p < nrows + 4)
                Ranks[i] += (double)TempRanks[i] / (double)1;//nrows;
            else if(p == 0)
                Ranks[i] = Ranks[i] * LengthWeight;
            else if(p == 1)
                Ranks[i] = Ranks[i] * NOOPSWeight;
            else if(p == 2)
                Ranks[i] = Ranks[i] * T1Weight;
        }
    }
    //delete TempRanks;
    //delete TempVals;
}
void PushGP::Replace(int index)
{
    if(Ranks[index+NInds] < Ranks[index])
    {
        //delete[] Popul[index];
        IndLength[index] = IndLength[index+NInds];
        //Popul[index] = new atomInst[IndLength[index]];
        for(int i=0;i!=IndLength[index];i++)
            Popul[index][i] = Popul[index+NInds][i];
        Fitness[index] = Fitness[index+NInds];
        for(int i=0;i!=PerfSize;i++)
        {
            Perf[index][i] = Perf[index+NInds][i];
        }
    }
}
void PushGP::ReplaceSort()
{
    for(int i=0;i!=NInds*2;i++)
    {
        for(int j=i;j!=NInds*2;j++)
        {
            if(Ranks[i] > Ranks[j]) //replace
            {
                swap(Ranks[i],Ranks[j]);
                swap(IndLength[i],IndLength[j]);
                swap(Popul[i],Popul[j]);
                swap(Fitness[i],Fitness[j]);
                swap(Perf[i],Perf[j]);
            }
        }
        //cout<<endl;
    }
    //cout<<endl;
}
void PushGP::ReplaceNew()
{
    int indexbest = 0;
    double fitbest = Ranks[0];
    for(int i=0;i!=NInds;i++)
    {
        if(Ranks[i] < fitbest)
        {
            fitbest = Ranks[i];
            indexbest = i;
        }
    }
    for(int i=0;i!=NInds;i++)
    {
        if(i != indexbest)
        {
            swap(Ranks[i],Ranks[i+NInds]);
            swap(IndLength[i],IndLength[i+NInds]);
            swap(Popul[i],Popul[i+NInds]);
            swap(Fitness[i],Fitness[i+NInds]);
            swap(Perf[i],Perf[i+NInds]);
        }
        //cout<<endl;
    }
    //cout<<endl;
}
void PushGP::SimilarityCalc(int SizeMultiplier)
{
    for(int i=0;i!=NInds*2;i++)
    {
        for(int j=nrows+4;j!=PerfSize;j++)
            Perf[i][j] = 0;
    }
    for(int i=0;i!=NInds*SizeMultiplier;i++)
    {
        for(int j=nrows+4;j!=PerfSize;j++)
            Perf[i][j] = 0;
        for(int j=0;j!=NInds*SizeMultiplier;j++)
        {
            int LenI = IndLength[i];
            int LenJ = IndLength[j];
            int k1=0, k2=0;
            int totalEq = 0;
            while(k1 < LenI)
            {
                int tempEq = 0;
                while(Popul[i][k1].inst != Popul[j][k2].inst)
                {
                    k2++;
                    if(k2 >= LenJ)
                    {
                        k1++;
                        break;
                    }
                }
                while(k1 < LenI && k2 < LenJ && Popul[i][k1].inst == Popul[j][k2].inst)
                {
                    tempEq++;
                    k1++;
                    k2++;
                }
                k2=0;
                totalEq += tempEq;
            }
            Perf[i][nrows + 4 + j] += log(1.0+double(totalEq*10));
            if(LenI > LenJ)
                Perf[i][nrows + 4 + j] -= log(1.0+double(LenI-LenJ))*0.5;
            else
                Perf[i][nrows + 4 + j] -= log(1.0+double(LenJ-LenI));
        }
    }
}

int main()
{
    /*unsigned long seed1 = mix(clock(),time(NULL),globalseed+getpid());
    unsigned long seed2 = mix(clock(),time(NULL)+100,globalseed+getpid());
    unsigned long seed3 = mix(clock(),time(NULL)+200,globalseed+getpid());
    unsigned long seed4 = mix(clock(),time(NULL)+300,globalseed+getpid());*/
    unsigned long seed1 = globalseed;
    unsigned long seed2 = globalseed;
    unsigned long seed3 = globalseed;
    unsigned long seed4 = globalseed;
    generator_uni_i.seed(seed1);
    generator_uni_r.seed(seed2);
    generator_norm.seed(seed3);
    generator_cauchy.seed(seed4);
    //cout<<seed1<<"\t"<<seed2<<"\t"<<seed3<<"\t"<<seed4<<endl;

    int world_size,world_rank,name_len,TotalNRuns = NIndsGlobal;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &name_len);
    if(world_rank > TotalNRuns)
    {
        MPI_Finalize();
        return 0;
    }

    unsigned long seed12 = globalseed+world_rank;
    unsigned long seed22 = globalseed+world_rank;
    unsigned long seed32 = globalseed+world_rank;
    unsigned long seed42 = globalseed+world_rank;
    generator_uni_i2.seed(seed12);
    generator_uni_r2.seed(seed22);
    generator_norm2.seed(seed32);
    generator_cauchy2.seed(seed42);

//    if(world_rank != 0)
//    {
//        double val = world_rank;
//        unsigned PRSsize = sizeof(val);
//        cout<<world_rank<<" sending..."<<endl;
//        MPI_Send(&val,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD);
//    }
//    if(world_rank == 0)
//    {
//        for(int i=0;i!=TotalNRuns;i++)
//        {
//            cout<<"world_size: "<<world_size<<endl;
//            double val;
//            unsigned ResultSize = sizeof(val);
//            cout<<world_rank<<" waiting..."<<endl;
//            MPI_Recv(&val,ResultSize,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//            cout<<val<<endl;
//        }
//        //process results
//        cout<<"process results"<<endl;
//        sleep(10);
//        for(int i=0;i!=TotalNRuns;i++)
//        {
//            double val = i*2;
//            unsigned ResultSize = sizeof(val);
//            cout<<world_rank<<" sending..."<<endl;
//            MPI_Send(&val,ResultSize,MPI_BYTE,i+1,0,MPI_COMM_WORLD);
//            cout<<val<<endl;
//        }
//    }
//    if(world_rank != 0)
//    {
//        double val = world_rank;
//        unsigned PRSsize = sizeof(val);
//        cout<<world_rank<<" receiving..."<<endl;
//        MPI_Recv(&val,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//        cout<<"received: "<<val<<endl;
//    }


    /*if(world_rank != 0)
    {
        for(int i=0;i!=PerfGlobal-1;i++)
        {
            ResultsBuffer[i+1] = world_rank;
        }
        ResultsBuffer[0] = world_rank;
        unsigned PRSsize = sizeof(ResultsBuffer);
        cout<<world_rank<<" sending..."<<endl;
        MPI_Send(&ResultsBuffer,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD);
    }
    if(world_rank == 0)
    {
        for(int i=0;i!=TotalNRuns;i++)
        {
            cout<<"world_size: "<<world_size<<endl;
            unsigned ResultSize = sizeof(ResultsBuffer);
            cout<<world_rank<<" waiting..."<<endl;
            MPI_Recv(&ResultsBuffer,ResultSize,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            int rank_recv = ResultsBuffer[0];
            for(int p=0;p!=PerfGlobal;p++)
            {
                AllResults[rank_recv][p] = ResultsBuffer[p]*2;
            }
            cout<<ResultsBuffer[0]<<endl;
        }
        //process results
        cout<<"process results"<<endl;
        sleep(1);
        for(int i=0;i!=TotalNRuns;i++)
        {
            unsigned ResultSize = sizeof(AllResults);
            cout<<world_rank<<" sending..."<<endl;
            MPI_Send(&AllResults,ResultSize,MPI_BYTE,i+1,0,MPI_COMM_WORLD);
        }
    }
    if(world_rank != 0)
    {
        unsigned PRSsize = sizeof(AllResults);
        cout<<world_rank<<" receiving..."<<endl;
        MPI_Recv(&AllResults,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        cout<<"received: "<<AllResults[world_rank][1]<<endl;
    }

    cout<<world_rank<<" exiting"<<endl;
    cout<<sizeof(ResultsBuffer)<<endl;
    MPI_Finalize();
    return 0;*/


    int RunsPerProcessor = 0;
    RunsPerProcessor = ceil(double(TotalNRuns)/double(world_size));
    int* RunsStart;
    int* RunsEnd;
    RunsStart = new int[world_size];
    RunsEnd = new int[world_size];
    for(int i=0;i!=world_size;i++)
    {
        RunsStart[i] = RunsPerProcessor*i;
        RunsEnd[i] = RunsPerProcessor*(i+1);
        if(RunsStart[i] > TotalNRuns)
            RunsStart[i] = TotalNRuns;
        if(RunsEnd[i] > TotalNRuns)
            RunsEnd[i] = TotalNRuns;
    }
    char buffer[500];
    char buffer_prog2[500];
    int mode = 1;
    GlobalMode = mode;
    if(mode == 0)
    {
        //for(int RunN = RunsStart[world_rank];RunN!=RunsEnd[world_rank];RunN++)
        int RunN = 0;
        int r_file = 36;
        {
            double vm, rss;
            sprintf(buffer,"../Res_FastPushGP_24/run%d/Results_prog_R%d.txt",r_file,world_rank);
            ofstream fout_prog(buffer);
            sprintf(buffer,"../Res_FastPushGP_24/run%d/Results_perf_R%d.txt",r_file,world_rank);
            ofstream fout_perf(buffer);

            //sprintf(buffer_prog2,"../Res_FastPushGP_24/Results_prog2_R%d.txt",RunN);
            //ofstream fout_prog2(buffer_prog2);
            //sprintf(buffer,"../Res_FastPushGP_24/Results_perf2_R%d.txt",RunN);
            //ofstream fout_perf2(buffer);
            //sprintf(buffer,"../Res_FastPushGP_24/Results_prog_add_R%d.txt",world_rank);
            //ofstream fout_progadd(buffer);
            int bestindex = 0;
            int newNFEmin = 10000;
            int newNFEmax = 25000;
            int DIMmin = 10;
            int DIMmax = 20;
            int ChangeRate = 1;
            int SaveRate = 100;

            CEC_Shifts = new double[DIMmax];
            CEC_Streches = new double[DIMmax];
            CEC_Left = new double[DIMmax];
            CEC_Right = new double[DIMmax];
            tempVector = new double[DIMmax];
            GNVars = IntRandom(DIMmax-DIMmin)+DIMmin;
            //process_mem_usage(vm, rss); cout<<"MEM 1 "<<vm<<endl;
            PushGP newGP(NIndsGlobal);
            //process_mem_usage(vm, rss); cout<<"MEM 2 "<<vm<<endl;
            int maxGen = 100000;
            NFEmax = IntRandom(newNFEmax-newNFEmin)+newNFEmin;
            newGP.counterlimit = 300;
            GenerateProblem17(GNVars);
            newGP.FuncNum_ = IntRandom(30)+1;
            while(newGP.FuncNum_ == 2)
            {
                newGP.FuncNum_ = IntRandom(30)+1;
            }
            for(int g=0;g!=maxGen;g++)
            {
                seed1 = globalseed+g*1;
                seed2 = globalseed+g*1;
                seed3 = globalseed+g*1;
                seed4 = globalseed+g*1;
                generator_uni_i.seed(seed1);
                generator_uni_r.seed(seed2);
                generator_norm.seed(seed3);
                generator_cauchy.seed(seed4);

                //cout<<world_rank<<"\t"<<g<<"\t"<<IntRandom(10)<<endl;
                //cout<<"STAGE1"<<endl;
                if(g%ChangeRate == 0)
                {
                    GNVars = IntRandom(DIMmax-DIMmin)+DIMmin;
                    NFEmax = IntRandom(newNFEmax-newNFEmin)+newNFEmin;
                    //process_mem_usage(vm, rss); cout<<"MEM 3 "<<vm<<endl;
                    GenerateProblem17(GNVars);
                    /*newGP.FuncNum_ = IntRandom(30)+1;
                    while(newGP.FuncNum_ == 2)
                    {
                        newGP.FuncNum_ = IntRandom(30)+1;
                    }*/
                    for(int i=0;i!=GNVars;i++)
                    {
                        CEC_Shifts[i] = Random(-10000,10000);
                        CEC_Streches[i] = pow(10.0,Random(-1,1));
                        CEC_Left[i] = CEC_Shifts[i] - 100*CEC_Streches[i];
                        CEC_Right[i] = CEC_Shifts[i] + 100*CEC_Streches[i];
                    }
                    /*
                    GNVars = 10;
                    NFEmax = 10000;
                    //process_mem_usage(vm, rss); cout<<"MEM 3 "<<vm<<endl;
                    GenerateProblem17(GNVars);
                    newGP.FuncNum_ = IntRandom(30)+1;
                    while(newGP.FuncNum_ == 2)
                    {
                        newGP.FuncNum_ = IntRandom(30)+1;
                    }
                    for(int i=0;i!=GNVars;i++)
                    {
                        CEC_Shifts[i] = 0;
                        CEC_Streches[i] = 1;
                        CEC_Left[i] = CEC_Shifts[i] - 100*CEC_Streches[i];
                        CEC_Right[i] = CEC_Shifts[i] + 100*CEC_Streches[i];
                    }*/
                }

                /*if(g%SaveRate == 0)
                {
                    if(world_rank == 0)
                    {
                        //fout_prog2.open(buffer_prog2,ios::trunc);
                        for(int i=0;i!=TotalNRuns;i++)
                        {
                            //fout_prog2.close();
                            sprintf(buffer_prog2,"../Res_FastPushGP_24/run%d/Results_prog2_R%d.txt",r_file,i);
                            ofstream fout_prog2(buffer_prog2,ios::trunc);
                            //fout_prog2.open(buffer_prog2,ios::trunc);
                            newGP.Inters[0].reset(newGP.IndLength[i]);
                            newGP.Inters[0].setProgram(newGP.Popul[i],newGP.IndLength[i]);
                            //cout<<newGP.IndLength[i]<<endl;
                            //cout<<buffer_prog2<<endl;
                            //cout<<newGP.Inters[0].printProgram()<<endl;
                            fout_prog2<<newGP.Inters[0].printProgram()<<endl;
                            fout_prog2.close();
                            //sleep(1);
                        }
                        for(int i=0;i!=TotalNRuns;i++)
                        {
                            double val = 0;
                            MPI_Send(&val,sizeof(val),MPI_BYTE,i+1,0,MPI_COMM_WORLD);
                        }
                    }
                    else
                    {
                        double val = 0;
                        MPI_Recv(&val,sizeof(val),MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        //sleep(1);
                        for(int j=0;j!=TotalNRuns;j++)
                        {
                            //cout<<j<<endl;
                            sprintf(buffer_prog2,"../Res_FastPushGP_24/run%d/Results_prog2_R%d.txt",r_file,j);
                            string newprog;
                            newprog = readFile2(buffer_prog2);
                            int finsize = 0;
                            vector<string> programs = explode(newprog,'\n',finsize);
                            string prog = programs[finsize-1];
                            newGP.Inters[0].reset(LimSize);
                            newGP.Inters[0].loadProgram(prog);
                            newGP.IndLength[j] = newGP.Inters[0].NESize;
                            //cout<<j<<endl;
                            for(int i=0;i!=newGP.Inters[0].NESize;i++)
                            {
                                newGP.Popul[j][i].inst = newGP.Inters[0].instNEStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].i = newGP.Inters[0].instNEConstIStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].f = newGP.Inters[0].instNEConstFStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].n = newGP.Inters[0].instNEConstSStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].v = newGP.Inters[0].instNEConstVStack[newGP.IndLength[j]-1-i];
                            }
                        }
                    }
                }*/

                if(g%SaveRate == 0)
                {
                    if(world_rank != 0)
                    {
                        //fout_prog2.open(buffer_prog2,ios::trunc);
                        //for(int i=0;i!=TotalNRuns;i++)
                        {
                            //fout_prog2.close();
                            sprintf(buffer_prog2,"../Res_FastPushGP_24/run%d/Results_prog2_R%d.txt",r_file,world_rank-1);
                            ofstream fout_prog2(buffer_prog2,ios::trunc);
                            //fout_prog2.open(buffer_prog2,ios::trunc);
                            newGP.Inters[0].reset(newGP.IndLength[world_rank-1]);
                            newGP.Inters[0].setProgram(newGP.Popul[world_rank-1],newGP.IndLength[world_rank-1]);
                            //cout<<newGP.IndLength[i]<<endl;
                            //cout<<buffer_prog2<<endl;
                            //cout<<newGP.Inters[0].printProgram()<<endl;
                            fout_prog2<<newGP.Inters[0].printProgram()<<endl;
                            fout_prog2.close();
                            //sleep(1);
                        }
                        //for(int i=0;i!=TotalNRuns;i++)
                        {
                            double val = 0;
                            MPI_Send(&val,sizeof(val),MPI_BYTE,0,0,MPI_COMM_WORLD);
                        }
                    }
                    else
                    {
                        double val = 0;
                        for(int i=0;i!=TotalNRuns;i++)
                        {
                            MPI_Recv(&val,sizeof(val),MPI_BYTE,i+1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        }
                        //sleep(1);
                        for(int j=0;j!=TotalNRuns;j++)
                        {
                            //cout<<j<<endl;
                            sprintf(buffer_prog2,"../Res_FastPushGP_24/run%d/Results_prog2_R%d.txt",r_file,j);
                            string newprog;
                            newprog = readFile2(buffer_prog2);
                            int finsize = 0;
                            vector<string> programs = explode(newprog,'\n',finsize);
                            string prog = programs[finsize-1];
                            newGP.Inters[0].reset(LimSize);
                            newGP.Inters[0].loadProgram(prog);
                            newGP.IndLength[j] = newGP.Inters[0].NESize;
                            //cout<<j<<endl;
                            for(int i=0;i!=newGP.Inters[0].NESize;i++)
                            {
                                newGP.Popul[j][i].inst = newGP.Inters[0].instNEStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].i = newGP.Inters[0].instNEConstIStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].f = newGP.Inters[0].instNEConstFStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].n = newGP.Inters[0].instNEConstSStack[newGP.IndLength[j]-1-i];
                                newGP.Popul[j][i].v = newGP.Inters[0].instNEConstVStack[newGP.IndLength[j]-1-i];
                            }
                        }
                    }
                }

                seed1 = globalseed+g*1;
                seed2 = globalseed+g*1;
                seed3 = globalseed+g*1;
                seed4 = globalseed+g*1;
                generator_uni_i.seed(seed1);
                generator_uni_r.seed(seed2);
                generator_norm.seed(seed3);
                generator_cauchy.seed(seed4);
                //ini_flag = 0;
                //process_mem_usage(vm, rss); cout<<"MEM 4 "<<vm<<endl;
                //process_mem_usage(vm, rss); cout<<"MEM 5 "<<vm<<endl;

                //cout<<"Run "<<RunN<<"\tGen: "<<g<<"\tDIM: "<<GNVars<<"\tNFEmax: "<<newGP.NFEmax<<endl;
                //process_mem_usage(vm, rss); cout<<"MEM 6 "<<vm<<endl;
                //for(int t=0;t!=1000000;t++)

                //cout<<world_rank<<"\t"<<g<<"\t"<<CauchyRand(0,1)<<endl;
                //for(int i=0;i!=1;i++)
                {
                    //newGP.FitCalcPopBasedCEC(i);
                }
                /*fout_progadd<<g<<"\t"<<g<<"\t";
                for(int i=0;i!=newGP.PerfSize;i++)
                {
                    fout_progadd<<newGP.Perf[0][i]<<"\t";
                }
                fout_progadd<<endl;*/
                /*for(int i=0;i!=TotalNRuns;i++)
                {
                    newGP.Inters[0].reset(newGP.IndLength[i]);
                    newGP.Inters[0].setProgram(newGP.Popul[i],newGP.IndLength[i]);
                    fout_prog2<<g<<"\t"<<i<<"\t"<<newGP.IndLength[i]<<"\t"<<newGP.Inters[0].printProgram()<<endl;
                    fout_perf2<<g<<"\t"<<i<<"\t";
                    for(int p=0;p!=PerfGlobal-1;p++)
                    {
                        fout_perf2<<newGP.Perf[i][p];
                    }
                    fout_perf2<<endl;
                }*/
                seed12 = globalseed+g*1;
                seed22 = globalseed+g*1;
                seed32 = globalseed+g*1;
                seed42 = globalseed+g*1;
                generator_uni_i2.seed(seed12);
                generator_uni_r2.seed(seed22);
                generator_norm2.seed(seed32);
                generator_cauchy2.seed(seed42);

                if(world_rank != 0)
                {
                    newGP.FitCalcPopBasedCEC(world_rank-1);
                    for(int p=0;p!=PerfGlobal-1;p++)
                    {
                        ResultsBuffer[p+1] = newGP.Perf[world_rank-1][p];
                    }
                    ResultsBuffer[0] = world_rank;
                    unsigned PRSsize = sizeof(ResultsBuffer);
                    //cout<<world_rank<<" sending..."<<endl;
                    MPI_Send(&ResultsBuffer,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD);
                    //sleep(0.01);
                }
                if(world_rank == 0)
                {
                    //cout<<"start1"<<endl;
                    newGP.FitCalcPopBasedCEC(world_rank);
                    for(int i=0;i!=TotalNRuns;i++)
                    {
                        //cout<<"world_size: "<<world_size<<endl;
                        unsigned ResultSize = sizeof(ResultsBuffer);
                        //cout<<world_rank<<" waiting..."<<endl;
                        MPI_Recv(&ResultsBuffer,ResultSize,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        //sleep(0.01);
                        int rank_recv = ResultsBuffer[0]-1;
                        for(int p=0;p!=PerfGlobal;p++)
                        {
                            AllResults[rank_recv][p] = ResultsBuffer[p];
                        }
                        for(int p=0;p!=PerfGlobal-1;p++)
                        {
                            newGP.Perf[rank_recv][p] = ResultsBuffer[p+1];
                        }
                        newGP.Fitness[rank_recv] = newGP.Perf[rank_recv][3];
                        newGP.IndLength[rank_recv] = newGP.Perf[rank_recv][0];
                        //cout<<ResultsBuffer[0]<<endl;
                    }
                    //process results
                    //cout<<"process results"<<endl;
                    //sleep(10);
                    for(int i=0;i!=TotalNRuns;i++)
                    {
                        /*unsigned ResultSize = sizeof(AllResults);
                        //cout<<world_rank<<" sending..."<<endl;
                        MPI_Send(&AllResults,ResultSize,MPI_BYTE,i+1,0,MPI_COMM_WORLD);*/
                        for(int j=0;j!=TotalNRuns;j++)
                        {
                            unsigned ResultSize = sizeof(ResultsBuffer);
                            for(int p=0;p!=PerfGlobal;p++)
                            {
                                ResultsBuffer[p] = AllResults[j][p];
                            }
                            //cout<<"send   "<<i<<" "<<j<<endl;
                            MPI_Send(&ResultsBuffer,ResultSize,MPI_BYTE,i+1,0,MPI_COMM_WORLD);
                            //cout<<"send ok "<<i<<" "<<j<<endl;
                            //sleep(0.01);
                        }
                    }
                    //cout<<"end1"<<endl;
                }
                if(world_rank != 0)
                {
                    /*unsigned PRSsize = sizeof(AllResults);
                    //cout<<world_rank<<" receiving..."<<endl;
                    MPI_Recv(&AllResults,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);*/
                    //cout<<"received: "<<endl;//<<AllResults[world_rank-1][1]
                    for(int i=0;i!=TotalNRuns;i++)
                    {
                        unsigned PRSsize = sizeof(ResultsBuffer);
                        MPI_Recv(&ResultsBuffer,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        //sleep(0.01);
                        for(int p=0;p!=PerfGlobal-1;p++)
                        {
                            newGP.Perf[i][p] = ResultsBuffer[p+1];
                        }
                        newGP.Fitness[i] = newGP.Perf[i][3];
                        newGP.IndLength[i] = newGP.Perf[i][0];
                    }
                }

                //if(world_rank == 0)  cout<<"end12"<<endl;

                /*if(world_rank < 2)
                {
                    cout<<world_rank<<"\t"<<newGP.Fitness[0]<<endl;
                }*/

                /*{
                    for(int i=0;i!=newGP.NInds;i++)
                    {
                        newGP.FitCalcPopBasedCEC(i);
                    }
                    //process_mem_usage(vm, rss); cout<<t<<"\tMEM 14 "<<vm<<endl;
                }*/

                /*fout_progadd<<g<<"\t"<<g<<"\t";
                for(int i=0;i!=newGP.PerfSize;i++)
                {
                    fout_progadd<<newGP.Perf[0][i]<<"\t";
                }
                fout_progadd<<endl;*/

                //cout<<"STAGE2"<<endl;
                //process_mem_usage(vm, rss); cout<<"MEM 7 "<<vm<<endl;
                newGP.SimilarityCalc(1);
                //if(world_rank == 0)  cout<<"end13"<<endl;
                //newGP.CalculateRanks(1,0.1,1,1,1);
                newGP.CalculateRanks(1,0.75,0.75,0.5,1);
                //if(world_rank == 0)  cout<<"end14"<<endl;

                /*if(world_rank == 0)
                {
                    for(int i=0;i!=10;i++)
                    {
                        //cout<<newGP.Ranks[i]<<"\t"<<newGP.Fitness[i]<<endl;
                    }
                }*/

                newGP.PrepareLexicase(1);
                //if(world_rank == 0)  cout<<"end15"<<endl;
                //process_mem_usage(vm, rss); cout<<"MEM 8 "<<vm<<endl;
                for(int i=0;i!=newGP.NInds;i++)
                {
                    if(Random(0,1) < 0.5)
                        newGP.SelectTour(i);
                    else
                        newGP.SelectLexicase(i,1);
                }
                //process_mem_usage(vm, rss); cout<<"MEM 9 "<<vm<<endl;
                //cout<<"STAGE3"<<endl;
                for(int i=0;i!=newGP.NInds;i++)
                {
                    double rnd = Random(0,1);
                    if(Random(0,1) < 0.475)
                    {
                        newGP.Crossover(i);
                    }
                    else if(rnd < 0.95)
                    {
                        newGP.CrossoverULT(i);
                    }
                    else
                    {
                        newGP.CrossoverAdd(i);
                    }
                }
                //if(world_rank == 0)  cout<<"end16"<<endl;
                //process_mem_usage(vm, rss); cout<<"MEM 10 "<<vm<<endl;
                //cout<<"STAGE4"<<endl;
                for(int i=0;i!=newGP.NInds;i++)
                {
                    double rnd = Random(0,1);
                    if(rnd < 0.475)
                    {
                        newGP.Mutate(i);
                    }
                    else if(rnd < 0.95)
                    {
                        newGP.MutateElWise(i);
                    }
                    else
                    {
                        newGP.MutateAdd(i);
                    }
                }
                //if(world_rank == 0)  cout<<"end17"<<endl;
                for(int i=0;i!=newGP.NInds;i++)
                {
                    if(Random(0,1) < 0.05)
                        newGP.RepairBracket(newGP.NInds+i);
                }
                //process_mem_usage(vm, rss); cout<<"MEM 11 "<<vm<<endl;
                //cout<<"STAGE5"<<endl;
                //sleep(1);
                //if(world_rank == 0)  cout<<"end19"<<endl;

                seed12 = globalseed+g*1;
                seed22 = globalseed+g*1;
                seed32 = globalseed+g*1;
                seed42 = globalseed+g*1;
                generator_uni_i2.seed(seed12);
                generator_uni_r2.seed(seed22);
                generator_norm2.seed(seed32);
                generator_cauchy2.seed(seed42);

                if(world_rank != 0)
                {
                    newGP.FitCalcPopBasedCEC(world_rank+newGP.NInds-1);
                    for(int p=0;p!=PerfGlobal-1;p++)
                    {
                        ResultsBuffer[p+1] = newGP.Perf[world_rank+newGP.NInds-1][p];
                    }
                    ResultsBuffer[0] = world_rank;//+newGP.NInds;
                    unsigned PRSsize = sizeof(ResultsBuffer);
                    //cout<<world_rank<<" sending..."<<endl;
                    MPI_Send(&ResultsBuffer,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD);
                    //sleep(0.01);
                    //if(world_rank == 0)  cout<<"end199"<<endl;
                }
                if(world_rank == 0)
                {
                    //cout<<"start2"<<endl;
                    newGP.FitCalcPopBasedCEC(world_rank+newGP.NInds);
                    for(int i=0;i!=TotalNRuns;i++)
                    {
                        //cout<<"world_size: "<<world_size<<endl;
                        unsigned ResultSize = sizeof(ResultsBuffer);
                        //cout<<world_rank<<" waiting..."<<endl;
                        MPI_Recv(&ResultsBuffer,ResultSize,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        //sleep(0.01);
                        int rank_recv = ResultsBuffer[0]-1;//-newGP.NInds;
                        for(int p=0;p!=PerfGlobal;p++)
                        {
                            AllResults[rank_recv][p] = ResultsBuffer[p];
                        }
                        for(int p=0;p!=PerfGlobal-1;p++)
                        {
                            newGP.Perf[rank_recv+newGP.NInds][p] = ResultsBuffer[p+1];
                        }
                        newGP.Fitness[rank_recv+newGP.NInds] = newGP.Perf[rank_recv+newGP.NInds][3];
                        newGP.IndLength[rank_recv+newGP.NInds] = newGP.Perf[rank_recv+newGP.NInds][0];
                        //cout<<ResultsBuffer[0]<<endl;
                    }
                    //process results
                    //cout<<"process results"<<endl;
                    //sleep(10);
                    for(int i=0;i!=TotalNRuns;i++)
                    {
                        /*unsigned ResultSize = sizeof(AllResults);
                        //cout<<world_rank<<" sending..."<<endl;
                        MPI_Send(&AllResults,ResultSize,MPI_BYTE,i+1,0,MPI_COMM_WORLD);*/
                        for(int j=0;j!=TotalNRuns;j++)
                        {
                            unsigned ResultSize = sizeof(ResultsBuffer);
                            for(int p=0;p!=PerfGlobal;p++)
                            {
                                ResultsBuffer[p] = AllResults[j][p];
                            }
                            //cout<<"send   "<<i<<" "<<j<<endl;
                            MPI_Send(&ResultsBuffer,ResultSize,MPI_BYTE,i+1,0,MPI_COMM_WORLD);
                            //cout<<"send ok "<<i<<" "<<j<<endl;
                            //sleep(0.01);
                        }
                    }
                    //cout<<"end2"<<endl;
                }
                if(world_rank != 0)
                {
                    /*unsigned PRSsize = sizeof(AllResults);
                    //cout<<world_rank<<" receiving..."<<endl;
                    MPI_Recv(&AllResults,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);*/
                    //cout<<"received: "<<endl;//<<AllResults[world_rank-1][1]
                    for(int i=0;i!=TotalNRuns;i++)
                    {
                        unsigned PRSsize = sizeof(ResultsBuffer);
                        MPI_Recv(&ResultsBuffer,PRSsize,MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        //sleep(0.01);
                        for(int p=0;p!=PerfGlobal-1;p++)
                        {
                            newGP.Perf[i+newGP.NInds][p] = ResultsBuffer[p+1];
                        }
                        newGP.Fitness[i+newGP.NInds] = newGP.Perf[i+newGP.NInds][3];
                        newGP.IndLength[i+newGP.NInds] = newGP.Perf[i+newGP.NInds][0];
                    }
                }
                //if(world_rank == 0) cout<<"end3"<<endl;
                /*for(int i=0;i!=newGP.NInds;i++)
                {
                    newGP.FitCalcPopBasedCEC(i+newGP.NInds);
                }*/

                /*fout_progadd<<g<<"\t"<<g<<"\t";
                for(int i=0;i!=newGP.PerfSize;i++)
                {
                    fout_progadd<<newGP.Perf[newGP.NInds][i]<<"\t";
                }
                fout_progadd<<endl;*/

                //process_mem_usage(vm, rss); cout<<"MEM 12 "<<vm<<endl;
                //cout<<"STAGE6"<<endl;
                newGP.SimilarityCalc(2);
                //newGP.CalculateRanks(2,0.1,1,1,1);
                newGP.CalculateRanks(2,0.75,0.75,0.5,1);
                //if(world_rank == 0) cout<<"end4"<<endl;

                /*if(world_rank == 0)
                {
                    for(int i=0;i!=10;i++)
                    {
                        //cout<<newGP.Ranks[i]<<"\t"<<newGP.Fitness[i]<<endl;
                    }
                }*/

                //process_mem_usage(vm, rss); cout<<"MEM 13 "<<vm<<endl;
                /*for(int i=0;i!=newGP.NInds;i++)
                {
                    newGP.Replace(i);
                }*/
                //cout<<"STAGE7"<<endl;
                newGP.ReplaceSort();

                //newGP.ReplaceNew();

                //newGP.CalculateRanks(2,0.00000001,0.00000001,0.000000001,0.0000000001);
                newGP.CalculateRanks(2,0.0,0.0,0.0,0.0);

                /*if(world_rank == 0)
                {
                    for(int i=0;i!=10;i++)
                    {
                        //cout<<newGP.Ranks[i]<<"\t"<<newGP.Fitness[i]<<endl;
                    }
                }*/
                //if(world_rank == 0)
                {
                    //for(int i=0;i!=newGP.NInds;i++)
                    {
                        //cout<<newGP.Fitness[i]<<endl;
                        /*newGP.Inters[0].reset(newGP.IndLength[i]);
                        newGP.Inters[0].setProgram(newGP.Popul[i],newGP.IndLength[i]);
                        cout<<world_rank<<"\t"<<newGP.Perf[i][5]<<"\t"<<newGP.Ranks[i]<<"\t"<<newGP.Inters[0].printProgram()<<endl;*/

                        /*newGP.FitCalcPopBasedCEC(0);
                        newGP.Inters[0].reset(newGP.IndLength[i]);
                        newGP.Inters[0].setProgram(newGP.Popul[i],newGP.IndLength[i]);
                        cout<<world_rank<<"\t"<<newGP.Perf[i][5]<<"\t"<<newGP.Ranks[i]<<"\t"<<newGP.Inters[0].printProgram()<<endl;*/
                    }
                }

                /*fout_progadd<<g<<"\t"<<g<<"\t";
                for(int i=0;i!=newGP.PerfSize;i++)
                {
                    fout_progadd<<newGP.Perf[newGP.NInds][i]<<"\t";
                }
                fout_progadd<<endl;*/

                //process_mem_usage(vm, rss); cout<<"MEM 14 "<<vm<<endl;
                double avglen = 0;
                double tempmin = newGP.Ranks[0];
                bestindex = 0;
                for(int i=0;i!=newGP.NInds*2;i++)
                {
                    if(newGP.Ranks[i] < tempmin)
                    {
                        bestindex = i;
                        tempmin = newGP.Ranks[i];
                    }
                    avglen += newGP.IndLength[i];
                }
                //cout<<bestindex<<endl;
                process_mem_usage(vm, rss);// cout<<"MEM 15 "<<vm<<endl;
                //string mem = to_string(vm);

                //cout<<"STAGE8"<<endl;
                //for(int i=0;i!=newGP.NInds;i++)
                {
                    //cout<<newGP.Fitness[i]<<"\t"<<newGP.Ranks[i]<<endl;
                }
                //process_mem_usage(vm, rss); cout<<"MEM 16 "<<vm<<endl;
                /*if(g == maxGen - 1)
                {
                    newGP.outputfinal = false;
                    newGP.FitCalcPopBasedCEC(bestindex);
                }*/
                //process_mem_usage(vm, rss); cout<<"MEM 17 "<<vm<<endl;
                //newGP.Inters[0].reset(newGP.IndLength[bestindex]);
                //process_mem_usage(vm, rss); cout<<"MEM 18 "<<vm<<endl;
                //newGP.Inters[0].setProgram(newGP.Popul[bestindex],newGP.IndLength[bestindex]);
                //process_mem_usage(vm, rss); cout<<"MEM 19 "<<vm<<endl;
                //if(world_rank == 0)
                {
                    //cout<<newGP.IndLength[bestindex]<<"\t"<<newGP.Inters[0].printProgram()<<endl;
                    //fout_prog<<newGP.Inters[0].printProgram()<<endl;
                }
                //process_mem_usage(vm, rss); cout<<"MEM 20 "<<vm<<endl;

                //cout<<world_rank<<"\t"<<g<<"\t"<<IntRandom(10)<<endl;
                //newGP.FitCalcPopBasedCEC(bestindex);
                if(world_rank == 0)
                {
                    newGP.Inters[0].reset(newGP.IndLength[bestindex]);
                    newGP.Inters[0].setProgram(newGP.Popul[bestindex],newGP.IndLength[bestindex]);
                    fout_prog<<newGP.Inters[0].printProgram()<<endl; //newGP.Inters[0].NESize<<"\t"<<newGP.IndLength[bestindex]<<"\t"<<newGP.Perf[bestindex][0]<<"\t"<<newGP.Fitness[bestindex]<<"\t"<<newGP.Perf[bestindex][3]<<"\t"<<

                    //cout<<bestindex<<endl;
                    /*newGP.Inters[0].reset(newGP.IndLength[bestindex]);
                    newGP.Inters[0].setProgram(newGP.Popul[bestindex],newGP.IndLength[bestindex]);
                    cout<<bestindex<<"\t"<<newGP.Perf[bestindex][5]<<"\t"<<newGP.Ranks[bestindex]<<"\t"<<newGP.Inters[0].printProgram()<<endl;*/

                    //fout_prog<<newGP.Inters[0].printProgram()<<endl;
                    cout<<"Run "<<RunN<<"\tGen: "<<g<<"\tDIM: "<<GNVars<<"\tNFEmax: "<<NFEmax<<"\tlen "<<newGP.Perf[bestindex][0]<<"\tNOOP "<<newGP.Perf[bestindex][1]<<"\tt "
                        <<newGP.Perf[bestindex][2]<<"\tFit "<<newGP.Fitness[bestindex]<<"\tAvgL "<<avglen/newGP.NInds*0.5<<"\t"<<vm<<endl;
                    fout_perf<<g<<"\t";
                    for(int i=0;i!=newGP.PerfSize;i++)
                    {
                        fout_perf<<newGP.Perf[bestindex][i]<<"\t";
                    }
                    fout_perf<<endl;
                }
                //cout<<world_rank<<"\t"<<g<<"\t"<<IntRandom(10)<<endl;

                //if(world_rank < 2)
                {
                    /*newGP.Inters[0].reset(newGP.IndLength[0]);
                    newGP.Inters[0].setProgram(newGP.Popul[0],newGP.IndLength[0]);
                    fout_progadd<<newGP.Inters[0].printProgram()<<endl;
                    fout_progadd<<g<<"\t";
                    for(int i=0;i!=newGP.PerfSize;i++)
                    {
                        fout_progadd<<newGP.Perf[0][i]<<"\t";
                    }
                    fout_progadd<<endl;*/
                }
                //process_mem_usage(vm, rss); cout<<"MEM 21 "<<vm<<endl;
                //cout<<"STAGE9"<<endl;
                //process_mem_usage(vm, rss); cout<<"MEM 22 "<<vm<<endl;
                //cout<<"STAGE10"<<endl;
            }
            delete CEC_Shifts;
            delete CEC_Streches;
            delete CEC_Left;
            delete CEC_Right;
            delete tempVector;
        }
    }
    else
    {
        TotalNRuns = 112;
        RunsPerProcessor = ceil(double(TotalNRuns)/double(world_size));
        for(int i=0;i!=world_size;i++)
        {
            RunsStart[i] = RunsPerProcessor*i;
            RunsEnd[i] = RunsPerProcessor*(i+1);
            if(RunsStart[i] > TotalNRuns)
                RunsStart[i] = TotalNRuns;
            if(RunsEnd[i] > TotalNRuns)
                RunsEnd[i] = TotalNRuns;
        }
        unsigned long seed1 = mix(clock(),time(NULL),globalseed+getpid());
        unsigned long seed2 = mix(clock(),time(NULL)+100,globalseed+getpid());
        unsigned long seed3 = mix(clock(),time(NULL)+200,globalseed+getpid());
        unsigned long seed4 = mix(clock(),time(NULL)+300,globalseed+getpid());
        generator_uni_i2.seed(seed1);
        generator_uni_r2.seed(seed2);
        generator_norm2.seed(seed3);
        generator_cauchy2.seed(seed4);

        GNVars = 2;
        CEC_Shifts = new double[GNVars];
        CEC_Streches = new double[GNVars];
        CEC_Left = new double[GNVars];
        CEC_Right = new double[GNVars];
        tempVector = new double[GNVars];
        PushGP newGP(1);
        newGP.counterlimit = 300;
        NFEmax = 400;
        //GenerateProblem17(GNVars);
        for(int i=0;i!=GNVars;i++)
        {
            CEC_Shifts[i] = 0;
            CEC_Streches[i] = 1;
            CEC_Left[i] = CEC_Shifts[i] - 100*CEC_Streches[i];
            CEC_Right[i] = CEC_Shifts[i] + 100*CEC_Streches[i];
        }

        //char buffer[500];
        //int run = 0;
        //int run2 = 73;
        int run3 = 1;
        int r_file = 0;
        //for(int run2=run3;run2!=run3+1;run2++)
        //for(int run2 = RunsStart[world_rank];run2!=RunsEnd[world_rank];run2++)
        //for(int IR=100;IR<101;IR+=16)
        //for(int IR=64;IR<65;IR+=16)
        for(r_file=95;r_file!=96;r_file++)
        //for(int r_file = RunsStart[world_rank];r_file!=RunsEnd[world_rank];r_file++)
        {
            //for(int PS=2;PS<32;PS+=4)
            //for(int run2 = RunsStart[world_rank];run2!=RunsEnd[world_rank];run2++)
            {
                int PS = 10;// + run2*2;
                int IR = PS;
                //for(int run2=0;run2!=1;run2++)
                {
                    //GlobalIntRandMult = IR;
                    GlobalPopSizeMult = PS;
                    //cout<<"Run "<<run2<<endl;
                    //sprintf(buffer,"/home/mpiscil/cloud/Res_FastPushGP_24/run%d/Results_prog_R%d.txt",r_file,run2);
                    sprintf(buffer,"/home/mpiscil/cloud/Res_FastPushGP_24/run37_(100000k)/Results_prog2_R%d.txt",r_file);
                    //sprintf(buffer,"/home/mpiscil/cloud/Res_FastPushGP_24/run37_(100000k)/Results_prog2_R%d.txt",r_file);
                    string filename = buffer;
                    //sprintf(buffer,"/home/mpiscil/cloud/Res_FastPushGP_24/run%d/FinalTest/perf_R%d_D%d_NFE%d.txt",r_file,run2,GNVars,NFEmax);

                    //sprintf(buffer,"/home/mpiscil/cloud/Res_FastPushGP_24/run36_(100000k)/FinalTest/perf_R%d_D%d_NFE%d_IR%d_PS%d.txt",r_file,GNVars,NFEmax,GlobalIntRandMult,GlobalPopSizeMult);
                    //ofstream fout_res(buffer);

                    string newprog;
                    newprog = readFile2(filename);
                    //cout<<newprog<<endl;
                    int finsize = 0;
                    vector<string> programs = explode(newprog,'\n',finsize);
                    string prog = programs[finsize-1];
                    newGP.Inters[0].reset(LimSize);
                    newGP.Inters[0].loadProgram(prog);
                    newGP.IndLength[0] = newGP.Inters[0].NESize;
                    for(int i=0;i!=newGP.Inters[0].NESize;i++)
                    {
                        newGP.Popul[0][i].inst = newGP.Inters[0].instNEStack[newGP.IndLength[0]-1-i];
                        newGP.Popul[0][i].i = newGP.Inters[0].instNEConstIStack[newGP.IndLength[0]-1-i];
                        newGP.Popul[0][i].f = newGP.Inters[0].instNEConstFStack[newGP.IndLength[0]-1-i];
                        newGP.Popul[0][i].n = newGP.Inters[0].instNEConstSStack[newGP.IndLength[0]-1-i];
                        newGP.Popul[0][i].v = newGP.Inters[0].instNEConstVStack[newGP.IndLength[0]-1-i];
                    }
                    cout<<newGP.Inters[0].printStacks()<<endl;
                    if(run3 != -1)
                        newGP.outputfinal = true;
                    else
                        newGP.outputfinal = false;

                    newGP.FuncNum_ = IntRandom(30)+1;
                    while(newGP.FuncNum_ == 2)
                    {
                        newGP.FuncNum_ = IntRandom(30)+1;
                    }
                    newGP.FitCalcPopBasedCEC(0);
                    //newGP.FitCalcPopBasedCEC2(0);
                    //newGP.FitCalcPopBasedCEC_LPSR(0);

                    //for(int i=0;i!=newGP.PerfSize;i++)
                        //fout_res<<newGP.Perf[0][i]<<"\t";
                    //fout_res.close();
                }
            }
        }
        delete CEC_Shifts;
        delete CEC_Streches;
        delete CEC_Left;
        delete CEC_Right;
        delete tempVector;
    }
    delete RunsStart;
    delete RunsEnd;
    cout<<"Rank "<<world_rank<<"\ton\t"<<processor_name<<"\t Finished at"<<"\t"<<currentDateTime()<<"\n";
    MPI_Finalize();
	return 0;
}
