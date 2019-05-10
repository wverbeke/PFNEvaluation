#include "PFNReader.h"


#include <iostream>
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"


int main(){
    Double_t _JetPt;
    Double_t _JetEta;
    Double_t _JetConstituentPt[50];
    Double_t _JetConstituentEta[50];
    Double_t _JetConstituentPhi[50];
    Int_t _JetConstituentPdgId[50];
    Int_t _JetConstituentCharge[50];
    Double_t _JetConstituentdxySig[50];
    Double_t _JetConstituentdzSig[50];
    Int_t _JetConstituentsNumberOfHits[50];
    Int_t _JetConstituentsNumberOfPixelHits[50];

    TBranch* b__JetPt;
    TBranch* b__JetEta;
    TBranch* b__JetConstituentPt;
    TBranch* b__JetConstituentEta;
    TBranch* b__JetConstituentPhi;
    TBranch* b__JetConstituentPdgId;
    TBranch* b__JetConstituentCharge;
    TBranch* b__JetConstituentdxySig;
    TBranch* b__JetConstituentdzSig;
    TBranch* b__JetConstituentsNumberOfHits;
    TBranch* b__JetConstituentsNumberOfPixelHits;

    TFile* inputFile = TFile::Open("mergedFile_randomized.root");
    TTree* inputTree = (TTree*) inputFile->Get("HNLtagger_tree");
    
    inputTree->SetBranchAddress("_JetPt", &_JetPt, &b__JetPt);
    inputTree->SetBranchAddress("_JetEta", &_JetEta, &b__JetEta);
    inputTree->SetBranchAddress("_JetConstituentPt", _JetConstituentPt, &b__JetConstituentPt);
    inputTree->SetBranchAddress("_JetConstituentEta", _JetConstituentEta, &b__JetConstituentEta);
    inputTree->SetBranchAddress("_JetConstituentPhi", _JetConstituentPhi, &b__JetConstituentPhi);
    inputTree->SetBranchAddress("_JetConstituentPdgId", _JetConstituentPdgId, &b__JetConstituentPdgId);
    inputTree->SetBranchAddress("_JetConstituentCharge", _JetConstituentCharge, &b__JetConstituentCharge);
    inputTree->SetBranchAddress("_JetConstituentdxySig", _JetConstituentdxySig, &b__JetConstituentdxySig);
    inputTree->SetBranchAddress("_JetConstituentdzSig", _JetConstituentdzSig, &b__JetConstituentdzSig);
    inputTree->SetBranchAddress("_JetConstituentsNumberOfHits", _JetConstituentsNumberOfHits, &b__JetConstituentsNumberOfHits);
    inputTree->SetBranchAddress("_JetConstituentsNumberOfPixelHits", _JetConstituentsNumberOfPixelHits, &b__JetConstituentsNumberOfPixelHits);

    PFNReader pfn( "jetTagger.h5", {50,9}, 2 );

    for( long unsigned entryIndex = 0; entryIndex < (long unsigned) inputTree->GetEntries(); ++entryIndex ){
        inputTree->GetEntry( entryIndex );

        std::vector< double > highlevelInput( { _JetPt, _JetEta } );
        std::vector< std::vector< double > > pfnInput;
        
        for( std::vector< std::vector< double > >::size_type p = 0; p < 50; ++p){
            pfnInput.push_back( { _JetConstituentPt[p], _JetConstituentEta[p], _JetConstituentPhi[p], _JetConstituentdxySig[p], (double) _JetConstituentsNumberOfHits[p], (double) _JetConstituentsNumberOfPixelHits[p], (double)_JetConstituentCharge[p], (double) _JetConstituentPdgId[p], _JetConstituentdzSig[p] } );
        }

        //PFN output
        pfn.predict(  pfnInput, highlevelInput );
    }
    return 0;
}
