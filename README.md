# Install libs and Set project
`cd proj` \
`git clone https://github.com/emil-e/rapidcheck.git` \
`cmake CMakeLists.txt` \
`make` 

# After installation
Update src file list in ./CMakeLists.txt \
`make  # build` \
`./test   # unit test` \
`./ptest  # prop test` \
`./bench  # running time check ` \
`./main   # actual solution` 
