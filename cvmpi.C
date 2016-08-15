#include <TMVA/ParallelExecutorMpi.h>
#include <TMVA/DataLoader.h>
#include <TMVA/Tools.h>
#include <TMVA/CrossValidation.h>

#include <TMessage.h>

void cvmpi()
{
    TMVA::DataLoader *dataloader=nullptr;
    TMVA::ParallelExecutorMpi executor;
    
    //setting up the dataloader in the main process
    if(executor.IsMainProcess()){
        TFile file("dataset.root");
        dataloader=(TMVA::DataLoader *)file.Get("dataset");
        file.Close();
    }else dataloader=new TMVA::DataLoader("dataset");
    
    //if it is the main process it will be send else it will be reciev
    executor.SharedDataLoader(dataloader);
    
    TMVA::CrossValidation *cv=new TMVA::CrossValidation(dataloader);

    cv->BookMethod(TMVA::Types::kBDT, "BDT",
    "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:nCuts=20");
   
    cv->SetNumFolds(8);
  
    auto results=executor.Execute(cv);

    if(executor.IsMainProcess()){
        TMVA::MsgLogger::EnableOutput();
        TMVA::gConfig().SetSilent(kFALSE);   
        results.Print();
    }
}
