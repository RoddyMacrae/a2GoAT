#include "GPlotProton.h"


using namespace std;


GPlotProton::GPlotProton()  :
    protonAngleDiffPrompt("protonAngleDiffPrompt", "Proton Angle Difference Prompt", 1000, 0, 100),
    protonAngleDiffRand("protonAngleDiffRand", "Proton Angle Difference Rand", 1000, 0, 100)
{

}

GPlotProton::~GPlotProton()
{

}

void  GPlotProton::ProcessEvent()
{

}

Bool_t  GPlotProton::Process(const char* input_filename, const char* output_filename)
{
    if(!Open(input_filename))    return kFALSE;

    if(!OpenProtons())    return kFALSE;
    if(!OpenTagger())    return kFALSE;

    TraverseEntries(0, protons->GetNEntries()+1);

    TH1D    diff(protonAngleDiffPrompt);
    diff.SetNameTitle("protonAngleDiffPromptRandSubst", "Proton Angle Difference Prompt/Rand Substracted");
    diff.Add(&protonAngleDiffRand,-1);
    TH1D    diffNormed(protonAngleDiffPrompt);
    diffNormed.SetNameTitle("protonAngleDiffPromptRandSubstNorm", "Proton Angle Difference Prompt/Rand Substracted Normalised");
    diffNormed.Add(&protonAngleDiffRand,-protonAngleDiffPrompt.GetMaximum()/protonAngleDiffRand.GetMaximum());
    hist->Write(&protonAngleDiffPrompt);
    hist->Write(&protonAngleDiffRand);
    hist->Write(&diff);
    hist->Write(&diffNormed);

    return kTRUE;
}

