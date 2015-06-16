#include "CSDistYieldComp.hh"

CSDistYieldComp::CSDistYieldComp()
{
    //ctor
}

CSDistYieldComp::~CSDistYieldComp()
{
    //dtor
}

void CSDistYieldComp::ExtractMCNPData(stringstream data, int count&)
{
    int intTemp;
    double temp;

    stream >> mtNum; count++;
    stream >> numRegs; count++;
    regEndPos = new int[numRegs];
    intScheme = new int[numRegs];

    for(int i=0; i<numRegs; i++, count++)
    {
        stream >> intTemp;
        regEndPos[i]=intTemp;
    }

    for(int i=0; i<numRegs; i++, count++)
    {
        stream >> intTemp;
        intScheme[i]=intTemp;
    }

    stream >> numIncEner; count++;
    enerVec = new double[numIncEner];
    yieldVec = new double[numIncEner];

    for(int i=0; i<numIncEner; i++, count++)
    {
        stream >> temp;
        enerVec[i]=temp;
    }
    for(int i=0; i<numIncEner; i++, count++)
    {
        stream >> intTemp;
        yieldVec[i]=intTemp;
    }
}

//set up for Capture data
void CSDistYieldComp::WriteG4NDLCSData(stringstream stream )
{
    double csNum;
    stream << std::setw(14) << std::right << enerVec.size() << std::setw(14) << std::right << numRegs << endl;

    int i,csIndex;
    for(i=0; i<numRegs; i++)
    {
        stream << regEndPos[i] << intScheme[i] << endl;
    }
    stream << '\n';
    stream.fill(' ');
    stream.precision(6);
    stream.setf(std::ios::scientific);

    for(int j=0; j<enerVec.size(); j++)
    {
        stream << std::setw(14) << std::right << enerVec[j]*1000000;
        for(i=csEnerStart; i<cSSize+csEnerStart; i++)
        {
            // assume average incoming neutron energy is 1eV
            if(enerCSVec[i]>enerVec[j])
            {
                i--;
                break;
            }
        }
        csIndex=i-csEnerStart;
        if(i<0)
            i=0;
        if(csIndex<0)
            csIndex=0;
        csNum = (enerVec[i]-enerCSVec[i])*(csVec[csIndex+1]-csVec[csIndex)/(enerCSVec[i+1]-enerCSVec[i])+csVec[csIndex];
        stream << std::setw(14) << std::right << (yieldVec[j]*csNum);
    }
}

void CSDistYieldComp::WriteG4NDLYieldData(stringstream stream )
{
    stream << std::setw(14) << std::right << enerVec.size() << std::setw(14) << std::right << numRegs << endl;

    for(int i=0; i<numRegs; i++)
    {
        stream << regEndPos[i] << intScheme[i] << endl;
    }

    for(int i=0; i<enerVec.size(); i++)
    {
        stream << std::setw(14) << std::right << enerVec[i]*1000000;
        stream << std::setw(14) << std::right << yieldVec[i];
    }
}

string CSDistYieldComp::IdentifyYourSelf()
{
    return "CSDistYieldComp";
}

double CSDistYieldComp::Interpolate(double x)
{
    int i, low, reg=0, csIndex;
    double csNum1, csNum2;
    for(i=0; i<CSVecSize; i++)
    {
        while(i>regEndPos[reg])
            reg++;
        if(enerVec[i]>x)
        {
            i--;
            break;
        }
    }
    if(i<0)
        i=0;

    for(low=csEnerStart; low<cSSize+csEnerStart; low++)
    {
        // assume average incoming neutron energy is 1eV
        if(enerCSVec[low]>enerVec[i])
        {
            low--;
            break;
        }
    }
    csIndex=low-csEnerStart;
    if(low<0)
        low=0;
    if(csIndex<0)
        csIndex=0;
    csNum1 = (enerVec[i]-enerCSVec[low])*(csVec[csIndex+1]-csVec[csIndex)/(enerCSVec[low+1]-enerCSVec[low])+csVec[csIndex];

    for(low=csEnerStart; low<cSSize+csEnerStart; low++)
    {
        // assume average incoming neutron energy is 1eV
        if(enerCSVec[low]>enerVec[i+1])
        {
            low--;
            break;
        }
    }
    csIndex=low-csEnerStart;
    if(low<0)
        low=0;
    if(csIndex<0)
        csIndex=0;
    csNum2 = (enerVec[i+1]-enerCSVec[low])*(csVec[csIndex+1]-csVec[csIndex)/(enerCSVec[low+1]-enerCSVec[low])+csVec[csIndex];

    return Interpolate(intScheme[reg], x, enerVec[i], enerVec[i+1], yieldVec[i]*csNum1, yieldVec[i+1]*csNum2);
}