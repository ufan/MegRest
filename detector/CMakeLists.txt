
IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  dd4hep_configure_output(INSTALL ${REST_PATH})
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
