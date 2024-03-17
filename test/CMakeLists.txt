# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(odysseyTests LANGUAGES CXX)

# ---- Tests ----

add_executable(odyssey_test source/odyssey_test.cpp)
target_link_libraries(odyssey_test PRIVATE odyssey_lib)
target_compile_features(odyssey_test PRIVATE cxx_std_17)

add_test(NAME odyssey_test COMMAND odyssey_test)

# ---- End-of-file commands ----

add_folders(Test)
