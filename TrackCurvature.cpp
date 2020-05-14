#include "TrackCurvature.h"


using namespace std;
DPP_MODULE_REGISTRATION_IMPLEMENT(TrackCurvature,"TrackCurvature");
TrackCurvature::TrackCurvature() : dpp::base_module()
{
  filename_output_="trackcurvature.root";
}

TrackCurvature::~TrackCurvature() {
  if (is_initialized()) this->reset();
}

void TrackCurvature::initialize(const datatools::properties& myConfig,
                                   datatools::service_manager& flServices,
                                   dpp::module_handle_dict_type& /*moduleDict*/){

  // Look for services
  if (flServices.has("geometry")) {
    const geomtools::geometry_service& GS = flServices.get<geomtools::geometry_service> ("geometry");
    geometry_manager_ = &GS.get_geom_manager();
    DT_THROW_IF(!geometry_manager_,
                std::runtime_error,
                "Null pointer to geometry manager return by geometry_service");
  }
  // Extract the filename_out key from the supplied config, if
  // the key exists. datatools::properties throws an exception if
  // the key isn't in the config, so catch this if thrown and don't do
  // anything
  try {
    myConfig.fetch("filename_out",this->filename_output_);
  } catch (std::logic_error& e) {
  }

  // Use the method of PTD2ROOT to create a root file with just the branches we need for the sensitivity analysis


  hfile_ = new TFile(filename_output_.c_str(),"RECREATE","Basic track curve info");
  hfile_->cd();
  tree_ = new TTree("Curvature","Curvature");
  tree_->SetDirectory(hfile_);
  
  // Reconstructed quantities
  
  // Standard cuts
  
  
  // Some basic counts
  tree_->Branch("reco.track_count",&trackdetails_.track_count_);
  tree_->Branch("reco.electron_count",&trackdetails_.electron_count_);
  tree_->Branch("reco.electron_charges",&trackdetails_.electron_charges_);
  tree_->Branch("reco.track_curvatures",&trackdetails_.track_curvatures_);
  tree_->Branch("reco.track_charges",&trackdetails_.track_charges_);
  tree_->Branch("reco.electron_energies",&trackdetails_.electron_energies_);
  tree_->Branch("reco.track_lengths", &trackdetails_.track_lengths_);
  tree_->Branch("reco.track_hit_counts",&trackdetails_.track_hit_counts_);

  this->_set_initialized(true);
}
//! [TrackCurvature::Process]
dpp::base_module::process_status
TrackCurvature::process(datatools::things& workItem) {
  

  uint electronCount=0;
  int trackCount=0;
  std::vector<snemo::datamodel::particle_track> electronCandidates;
  std::vector<double> electronEnergies;
  std::vector<int> electronCharges;
  std::vector<double> trackLengths;
  std::vector<double> trackCurvatures;
  std::vector<int> trackCharges;
  std::vector<int> trackHitCounts;

// Number of particle tracks PTD databank
  try
  {
   const snemo::datamodel::particle_track_data& trackData = workItem.get<snemo::datamodel::particle_track_data>("PTD");

   if (trackData.hasParticles ())
    {
      const snemo::datamodel::ParticleHdlCollection particles = trackData.particles();
      for (snemo::datamodel::ParticleHdlCollection::const_iterator   iParticle = particles.begin(); iParticle != particles.end(); ++iParticle) {
        const snemo::datamodel::particle_track& track = iParticle->get();
        int charge=(int)track.get_charge();

          if (charge== snemo::datamodel::particle_track::NEUTRAL)
          continue; // Not interested in gammas right now
        
          // It's a charged particle track
          trackCount++;

        // Get the track length and number of hits
        const snemo::datamodel::tracker_trajectory & the_trajectory = track.get_trajectory();
        const snemo::datamodel::tracker_cluster & the_cluster = the_trajectory.get_cluster();
        trackHitCounts.push_back(the_cluster.size());

        trackLengths.push_back( the_trajectory.get_pattern().get_shape().get_length());
        // get the charge
          if (charge== snemo::datamodel::particle_track::UNDEFINED)
          { // straight track
            trackCharges.push_back(0);
            trackCurvatures.push_back(0);
            continue;
          }
          if (charge==snemo::datamodel::particle_track::POSITIVE)
              trackCharges.push_back(1);
          else trackCharges.push_back(-1);
        }
    }
  }
  catch (std::logic_error& e) {
    std::cerr << "failed to grab PTD bank : " << e.what() << std::endl;
    return dpp::base_module::PROCESS_INVALID;
  } //end catch
// Initialise variables that might not otherwise get set
 // It does not restart the vector for each entry so we have to do that manually
  ResetVars();


  trackdetails_.track_lengths_=trackLengths;
  trackdetails_.track_hit_counts_=trackHitCounts;
  trackdetails_.electron_charges_=electronCharges;
  trackdetails_.track_charges_=trackCharges;
  trackdetails_.track_curvatures_=trackCurvatures;
  trackdetails_.electron_energies_=electronEnergies;
  trackdetails_.track_count_=trackCount;
  trackdetails_.electron_count_=electronCount;

//


  tree_->Fill();
//
//  // MUST return a status, see ref dpp::processing_status_flags_type
 return dpp::base_module::PROCESS_OK;
}

void TrackCurvature::ResetVars()
{
  // clear out all the vectors or they persist between events
    trackdetails_.track_lengths_.clear();
    trackdetails_.electron_charges_.clear();
    trackdetails_.track_charges_.clear();
    trackdetails_.track_hit_counts_.clear();
    trackdetails_.electron_energies_.clear();
    trackdetails_.track_curvatures_.clear();
    trackdetails_.track_count_=0;
    trackdetails_.electron_count_=0;
  
}

//! TrackCurvature::reset]
void TrackCurvature::reset() {
  hfile_->cd();
  tree_->Write();
  hfile_->Close(); //
  std::cout << "In reset: finished conversion, file closed " << std::endl;

  // clean up
  delete hfile_;
  filename_output_ = "trackcurvature.root";
  this->_set_initialized(false);

}

