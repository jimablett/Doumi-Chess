all:
	clang++ -o doumi uci.cpp -O3 -flto=full -ftree-vectorize -funroll-loops \
	-ffast-math -static -static-libgcc -static-libstdc++ -DNDEBUG -finline-functions -pipe -std=c++23 \
	-fno-rtti -fstrict-aliasing -fomit-frame-pointer -lm -fuse-ld=lld -MMD -MP  \
	
	
	
	

	#       -mpopcnt -msse4.1 -msse4.2 -mbmi -mfma -mavx2 -mbmi2 -mavx -march=native -mtune=native      <    avx/bmi2 enable
	
	
	#       -fprofile-instr-generate -fcoverage-mapping                                                 <   before -o
	
	#        llvm-profdata merge -output=default.profdata *.profraw                                     <  enter on command line
 
    #       -fprofile-use=default.profdata                                                              <   before -o
	
	
	

    #       -mpopcnt -mtune=znver2 -msse4.1 -mbmi -mfma -mavx2 -mavx -mbmi                              <  for Zen 2 processors (no bmi2)
    
    
    #       -march=silvermont -mtune=silvermont -msse4.1 -msse4.2 -mpopcnt                              <  for popcount builds (with sse4.1/4.2)    


    #       -msse3 -mssse3 -march=k8 -mtune=k8                                                          <  sse3 build	
   
