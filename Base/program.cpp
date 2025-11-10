#include <CD_Driver.H>
#include <CD_ItoKMCJSON.H>
#include <CD_MechanicalShaft.H>
#include <CD_ItoKMCGodunovStepper.H>
#include <CD_ItoKMCStreamerTagger.H>
#include "ParmParse.H"

using namespace ChomboDischarge;
using namespace Physics::ItoKMC;

int main(int argc, char* argv[]){

#ifdef CH_MPI
  MPI_Init(&argc, &argv);
#endif

  // Build class options from input script and command line options
  const std::string input_file = argv[1];
  ParmParse pp(argc-2, argv+2, NULL, input_file.c_str());

  // Initialize RNG
  Random::seed();

  auto compgeom    = RefCountedPtr<ComputationalGeometry> (new MechanicalShaft());
  auto amr         = RefCountedPtr<AmrMesh> (new AmrMesh());
  auto physics     = RefCountedPtr<ItoKMCPhysics> (new ItoKMCJSON());
  auto timestepper = RefCountedPtr<ItoKMCStepper<>> (new ItoKMCGodunovStepper<>(physics));
  auto tagger      = RefCountedPtr<CellTagger> (new ItoKMCStreamerTagger<ItoKMCStepper<>>(physics, timestepper, amr));

  // Get potential from input script 
  Real U0;
  {
    ParmParse pp("simulation");
    pp.get("potential", U0);
  }

  auto potentialCurve = [U0](const Real a_time) -> Real {
    return U0;
  };  
  timestepper->setVoltage(potentialCurve);

  // Set up the Driver and run it
  RefCountedPtr<Driver> engine = RefCountedPtr<Driver> (new Driver(compgeom, timestepper, amr, tagger));
  engine->setupAndRun(input_file);

#ifdef CH_MPI
  CH_TIMER_REPORT();
  MPI_Finalize();
#endif
}
