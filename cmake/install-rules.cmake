install(
    TARGETS odyssey_exe
    RUNTIME COMPONENT odyssey_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
