#include <CD_Driver.H>
#include <CD_MechanicalShaft.H>
#include <CD_ItoKMCJSON.H>
#include <CD_ItoKMCSubclass.H>
#include <CD_ItoKMCStreamerTagger.H>

using namespace ChomboDischarge;
using namespace Physics::ItoKMC;

int
main(int argc, char* argv[])
{
  ChomboDischarge::initialize(argc, argv);

  Random::seed();

  auto amr         = RefCountedPtr<AmrMesh>(new AmrMesh());
  auto compgeom    = RefCountedPtr<ComputationalGeometry>(new MechanicalShaft());  
  auto physics     = RefCountedPtr<ItoKMCPhysics>(new ItoKMCJSON());
  auto timestepper = RefCountedPtr<ItoKMCStepper<>>(new ItoKMCSubclass<>(physics));
  auto tagger      = RefCountedPtr<CellTagger>(new ItoKMCStreamerTagger<ItoKMCStepper<>>(physics, timestepper, amr));
  auto engine      = RefCountedPtr<Driver>(new Driver(compgeom, timestepper, amr, tagger));

  // Set the potential based on the input script
  Real      U0;
  ParmParse pp("simulation");
  pp.get("potential", U0);

  auto potentialCurve = [U0](const Real a_time) -> Real {
    return U0;
  };

  timestepper->setVoltage(potentialCurve);

  engine->setupAndRun();

  ChomboDischarge::finalize();
}
