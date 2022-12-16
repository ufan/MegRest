project(MegatDetector)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR DD4HEP_BUILD_TEST) AND TEST)
  dd4hep_enable_tests()
endif()

#==========================================================================

dd4hep_add_plugin(MegatDetector SOURCES src/*.cpp
  USES DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign
       ROOT::Core ROOT::Geom ROOT::GenVector ROOT::MathCore
  )
dd4hep_configure_scripts(MegatDetector DEFAULT_SETUP WITH_TESTS)
install(TARGETS MegatDetector LIBRARY DESTINATION lib)

#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( MegatDetector_Telescope_dump_geometry
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_MegatDetector.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${CMAKE_INSTALL_PREFIX}/geometry/compact/Telescope.xml 
  -plugin DD4hep_DetectorDump
  REGEX_PASS "/world/Telescope/module_9 NumDau\\:1 VolID\\:00000903 Place"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )

#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( MegatDetector_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_MegatDetector.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_populate 
     -input file:${CMAKE_INSTALL_PREFIX}/geometry/compact/Telescope.xml -iovs 10
  REGEX_PASS "Summary          INFO  Processed a total 190 conditions \\(S:190,L:0,C:0,M:0\\) and \\(C:190,M:0\\) alignments. Created:200"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load ALEPH TPC geometry --------------------------------------
dd4hep_add_test_reg( MegatDetector_AlephTPC_load
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_MegatDetector.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${CMAKE_INSTALL_PREFIX}/geometry/compact/AlephTPC.xml
             -destroy -no-interpreter -plugin DD4hep_GlobalAlignmentInstall
  REGEX_PASS "189 nodes/ 24 volume UID's in Detector Geometry"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
