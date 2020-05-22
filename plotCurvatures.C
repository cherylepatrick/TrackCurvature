void plotCurves(string filename, string title, string outfile)
{
  gStyle->SetOptStat(false);
  TFile *f = new TFile(filename.c_str());
  TTree *tree = (TTree*) f->Get("Curvature");
  

  // Plot the radius of curvature for positive and negative tracks
  TH1D *poscurves = new TH1D("poscurves","positive",150,0,30);
  TH1D *negcurves = new TH1D("negcurves","negative",150,0,30);
  TCanvas *c = new TCanvas("c","Radius of curvature",900,600);
  poscurves->SetLineColor(kRed);
  negcurves->SetLineColor(kBlue);
  tree->Draw("reco.track_curvatures/1000.>>poscurves","reco.track_charges== 1");
  tree->Draw("reco.track_curvatures/1000.>>negcurves","reco.track_charges== -1");
  
  string fullTitle=title+Form(" - %lld events",tree->GetEntries());
  negcurves->SetTitle (fullTitle.c_str());
  negcurves->GetXaxis()->SetTitle ("Radius of curvature (m)");
  negcurves->GetYaxis()->SetTitle ("Number of tracks");
  
  negcurves->Draw("HIST");
  poscurves->Draw("HIST SAME");
  
  TLegend *leg=new TLegend(0.6,0.7,0.8,0.8); // Add a legend
  leg->AddEntry(negcurves, "Negative","l");
  leg->AddEntry(poscurves,"Positive","l");
  leg->Draw();
  
  c->SaveAs(("plots/"+outfile+"_radii.png").c_str());
  
  // Now plot radius of curvature of negative tracks vs energy
  TH2D *radvEneg = new TH2D("radvEneg","radvEneg",150,0,3,150,0,30);
  TH2D *radvEpos = new TH2D("radvEpos","radvEpos",150,0,3,150,0,30);
  radvEpos->SetMarkerColor(kRed);
  radvEneg->SetMarkerColor(kBlue);
  radvEpos->SetMarkerStyle(kFullTriangleUp);
  radvEneg->SetMarkerStyle(kFullTriangleUp);
  tree->Draw("reco.track_curvatures/1000.:reco.electron_energies>>radvEneg" ,"reco.track_count==2 && reco.electron_count==2 && reco.track_charges== -1");
  tree->Draw("reco.track_curvatures/1000.:reco.electron_energies>>radvEpos" ,"reco.track_count==2 && reco.electron_count==2 && reco.track_charges== 1");
  radvEneg->SetTitle (fullTitle.c_str());
  
  radvEneg->GetYaxis()->SetTitle ("Radius of curvature (m)");
  radvEneg->GetXaxis()->SetTitle ("Reconstructed energy (MeV)");
  radvEneg->GetYaxis()->SetTitleOffset(0.8);
  radvEneg->Draw("");
  radvEpos->Draw("same");
  TLegend *leg2=new TLegend(0.2,0.7,0.4,0.8); // Add a legend
  leg2->AddEntry(radvEneg, "Negative","p");
  leg2->AddEntry(radvEpos,"Positive","p");
  leg2->Draw();
  c->SaveAs(("plots/"+outfile+"_rad_v_E.png").c_str());
  
  // Finally, plot the charges
  TH1D *charges = new TH1D("charges","charges",3,-1,2);
  TH1D *echarges = new TH1D("echarges","echarges",3,-1,2);
  tree->Draw("reco.track_charges>>charges");
  tree->Draw("reco.electron_charges>>echarges");
  charges->GetXaxis()->SetTitle ("Charge");
  charges->GetYaxis()->SetTitle ("Number of tracks");
  charges->SetLineColor(kOrange+2);
  echarges->SetLineColor(kGreen+2);
  charges->SetLineWidth(3);
  echarges->SetLineWidth(3);

  charges->SetTitle (fullTitle.c_str());
  charges->GetYaxis()->SetRangeUser(0,20000);
  charges->Draw("hist");
  echarges->Draw(" same HIST");
  
  string allstr = Form("All tracks (%.0f)",charges->Integral());
  string assstr = Form("Associated tracks (%.0f)",echarges->Integral());
  TLegend *leg3=new TLegend(0.6,0.7,0.9,0.9); // Add a legend
  leg3->AddEntry(charges, allstr.c_str(),"L");
  leg3->AddEntry(echarges,assstr.c_str(),"L");
  leg3->Draw();
  c->SaveAs(("plots/"+outfile+"_charges.png").c_str());
  
  // Look for wide tracks
  
  // Plot the radius of curvature for positive and negative tracks
  TH1D *poscurves2 = new TH1D("poscurves2","positive2",100,0,100000);
  TH1D *negcurves2 = new TH1D("negcurves2","negative2",100,0,100000);
  poscurves2->SetLineColor(kRed);
  negcurves2->SetLineColor(kBlue);
  tree->Draw("reco.track_curvatures/1000.>>poscurves2","reco.track_charges== 1");
  tree->Draw("reco.track_curvatures/1000.>>negcurves2","reco.track_charges== -1");
  
  negcurves2->SetTitle (fullTitle.c_str());
  negcurves2->GetXaxis()->SetTitle ("Radius of curvature (m)");
  negcurves2->GetYaxis()->SetTitle ("Number of tracks");
  
  negcurves2->Draw("HIST");
  poscurves2->Draw("HIST SAME");
  
  TLegend *leg4=new TLegend(0.6,0.7,0.8,0.8); // Add a legend
  
  string negstr = Form("Negative (%.1f)",negcurves2->GetMean());
  string posstr = Form("Positive (%.1f)",poscurves2->GetMean());
  leg4->AddEntry(negcurves2, negstr.c_str(),"l");
  leg4->AddEntry(poscurves2,posstr.c_str(),"l");
  leg4->Draw();
  c->SetLogx();
  c->SetLogy();
  c->SaveAs(("plots/"+outfile+"_log.png").c_str());
  c->SetLogx(false);
  c->SetLogy(false);
  
}

void plotCurvatures()
{
  plotCurves("se82_0nubb_mapped-reco_curve.root","Realistic field", "mapped");
  plotCurves("se82_0nubb_uniform-reco_curve.root","25G uniform field", "uniform");
  plotCurves("se82_0nubb_zero-reco_curve.root","No field", "zero");
  plotCurves("euniform.root","1MeV electrons, 25G", "e_uniform");
  plotCurves("ezero.root","1MeV electrons, no field", "e_zero");
  plotCurves("emapped.root","1MeV electrons, realistic field", "e_map");

}
