#include "PFNReader.h"


#include <iostream>
#include <vector>

#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TH1.h"


int main(){
    Bool_t   _JetIsFromHNL;
    Double_t _JetPt;
    Double_t _JetEta;
    Double_t _JetConstituentPt[50];
    Double_t _JetConstituentEta[50];
    Double_t _JetConstituentPhi[50];
    Int_t _JetConstituentPdgId[50];
    Int_t _JetConstituentCharge[50];
    Double_t _JetConstituentdxy[50];
    Double_t _JetConstituentdz[50];
    Double_t _JetConstituentdxyErr[50];
    Double_t _JetConstituentdzErr[50];
    Int_t _JetConstituentNumberOfHits[50];
    Int_t _JetConstituentNumberOfPixelHits[50];

    TBranch* b__JetIsFromHNL;
    TBranch* b__JetPt;
    TBranch* b__JetEta;
    TBranch* b__JetConstituentPt;
    TBranch* b__JetConstituentEta;
    TBranch* b__JetConstituentPhi;
    TBranch* b__JetConstituentPdgId;
    TBranch* b__JetConstituentCharge;
    TBranch* b__JetConstituentdxy;
    TBranch* b__JetConstituentdz;
    TBranch* b__JetConstituentdxyErr;
    TBranch* b__JetConstituentdzErr;
    TBranch* b__JetConstituentNumberOfHits;
    TBranch* b__JetConstituentNumberOfPixelHits;

    TFile* inputFile = TFile::Open("~/public/2l2q_analysis/trees/HNLtagger/final/full_analyzer/mergedFile.root");
    TTree* inputTree = (TTree*) inputFile->Get("HNLtagger_tree");
    
    inputTree->SetBranchAddress("_JetIsFromHNL", &_JetIsFromHNL, &b__JetIsFromHNL);
    inputTree->SetBranchAddress("_JetPt", &_JetPt, &b__JetPt);
    inputTree->SetBranchAddress("_JetEta", &_JetEta, &b__JetEta);
    inputTree->SetBranchAddress("_JetConstituentPt", _JetConstituentPt, &b__JetConstituentPt);
    inputTree->SetBranchAddress("_JetConstituentEta", _JetConstituentEta, &b__JetConstituentEta);
    inputTree->SetBranchAddress("_JetConstituentPhi", _JetConstituentPhi, &b__JetConstituentPhi);
    inputTree->SetBranchAddress("_JetConstituentPdgId", _JetConstituentPdgId, &b__JetConstituentPdgId);
    inputTree->SetBranchAddress("_JetConstituentCharge", _JetConstituentCharge, &b__JetConstituentCharge);
    inputTree->SetBranchAddress("_JetConstituentdxy", _JetConstituentdxy, &b__JetConstituentdxy);
    inputTree->SetBranchAddress("_JetConstituentdz", _JetConstituentdz, &b__JetConstituentdz);
    inputTree->SetBranchAddress("_JetConstituentdxyErr", _JetConstituentdxyErr, &b__JetConstituentdxyErr);
    inputTree->SetBranchAddress("_JetConstituentdzErr", _JetConstituentdzErr, &b__JetConstituentdzErr);
    inputTree->SetBranchAddress("_JetConstituentNumberOfHits", _JetConstituentNumberOfHits, &b__JetConstituentNumberOfHits);
    inputTree->SetBranchAddress("_JetConstituentNumberOfPixelHits", _JetConstituentNumberOfPixelHits, &b__JetConstituentNumberOfPixelHits);

    PFNReader pfn( "/user/bvermass/heavyNeutrino/Dileptonprompt/CMSSW_10_2_14/src/deepLearning/jetTagger_reliso.h5", {50,11}, 2 );

    TH1F *Model_output_histo_sig = new TH1F("Model_output_histo_sig", ";Model output;Events", 20, 0, 1);
    TH1F *Model_output_histo_bkg = new TH1F("Model_output_histo_bkg", ";Model output;Events", 20, 0, 1);

    long unsigned nentries = inputTree->GetEntries();
    std::cout << "Number of entries: " << nentries << std::endl;
    for( long unsigned entryIndex = 0; entryIndex < nentries; ++entryIndex ){
        inputTree->GetEntry( entryIndex );
        if(entryIndex%(nentries/20) == 0) std::cout << "entry " << entryIndex << " of " << nentries << std::endl;

        std::vector< double > highlevelInput( { _JetPt, _JetEta } );
        std::vector< std::vector< double > > pfnInput;
        
        for( std::vector< std::vector< double > >::size_type p = 0; p < 50; ++p){
            pfnInput.push_back( { _JetConstituentPt[p], _JetConstituentEta[p], _JetConstituentPhi[p], _JetConstituentdxy[p], _JetConstituentdz[p], _JetConstituentdxyErr[p], _JetConstituentdzErr[p], (double) _JetConstituentNumberOfHits[p], (double) _JetConstituentNumberOfPixelHits[p], (double)_JetConstituentCharge[p], (double) _JetConstituentPdgId[p] } );
        }

        //PFN output
        double prediction = pfn.predict(  pfnInput, highlevelInput );
        if(entryIndex%(nentries/20) == 0) std::cout << "prediction: " << prediction << std::endl;
        if(_JetIsFromHNL) Model_output_histo_sig->Fill(prediction);
        else Model_output_histo_bkg->Fill(prediction);
    }

    TString outputfilename = "modeloutputhisto.root";
    std::cout << "output to: " << outputfilename << std::endl;
    TFile *output = new TFile(outputfilename, "recreate");
    Model_output_histo_sig->Write();
    Model_output_histo_bkg->Write();
    output->Close();
    return 0;
}
