//! \file    TrackCurvature.h
//! \brief   Get charge and curvature of reconstructed tracks
//! \details Process a things object
#ifndef TRACKCURVE_HH
#define TRACKCURVE_HH
// Standard Library
// Third Party
//#include <boost/foreach.hpp>
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"

// - Bayeux
#include "bayeux/dpp/base_module.h"
#include "bayeux/mctools/simulated_data.h"
#include "bayeux/genbb_help/primary_particle.h"
#include "bayeux/genbb_help/primary_event.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/line_3d.h"
#include "bayeux/geomtools/helix_3d.h"
#include "bayeux/geomtools/geomtools.h"

// - Falaise
#include "falaise/snemo/datamodels/calibrated_data.h"
#include "falaise/snemo/datamodels/tracker_clustering_data.h"
#include "falaise/snemo/datamodels/tracker_clustering_solution.h"
#include "falaise/snemo/datamodels/particle_track_data.h"

// From this application


typedef struct TrackEventStorage{



  std::vector<double> electron_energies_;
  std::vector<int> electron_charges_;
  std::vector<int> track_charges_;
  std::vector<double> track_lengths_;
  std::vector<int> track_hit_counts_;
  std::vector<double> track_curvatures_;
  std::vector<bool> is_electron_;
  int track_count_; // How many reconstructed tracks?
  int electron_count_; // How many reconstructed electrons?

  
}trackeventstorage;


// This Project
class TrackCurvature : public dpp::base_module {
  static const uint minHitsInCluster=3;
 public:
  //! Construct module
  TrackCurvature();
  //! Destructor
  virtual ~TrackCurvature();
  //! Configure the module
  virtual void initialize(const datatools::properties& myConfig,
                          datatools::service_manager& flServices,
                          dpp::module_handle_dict_type& moduleDict);
  //! Process supplied data record
  virtual dpp::base_module::process_status process(datatools::things& workItem);
  //! Reset the module
  virtual void reset();
 private:
  TFile* hfile_;
  TTree* tree_;
  TrackEventStorage trackdetails_;

  // configurable data member
  std::string filename_output_;

  // geometry service
  const geomtools::manager* geometry_manager_; //!< The geometry manager

  
  void ResetVars();

  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(TrackCurvature);
};
#endif // TRACKCURVE_HH


