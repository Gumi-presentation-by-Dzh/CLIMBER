TARGET = noattack_hc_climber noattack_bwl_climber noattack_twl attack_hc_climber attack_bwl_climber attack_twl
all: $(TARGET)
noattack_hc_climber: noattack_hc_climber.cpp hcmm_climber.h hcmm_climber.cpp
	g++ -o noattack_hc_climber noattack_hc_climber.cpp hcmm_climber.h hcmm_climber.cpp
noattack_bwl_climber:noattack_bwl_climber.cpp bwlmm_climber.h bwlmm_climber.cpp
	g++ -o noattack_bwl_climber noattack_bwl_climber.cpp bwlmm_climber.h bwlmm_climber.cpp
noattack_twl:noattack_twl_climber.cpp twl.h twl.cpp
	g++ -o noattack_twl noattack_twl_climber.cpp twl.h twl.cpp

attack_hc_climber: attack_hc_climber.cpp hcmm_climber.h hcmm_climber.cpp
	g++ -o attack_hc_climber attack_hc_climber.cpp hcmm_climber.h hcmm_climber.cpp
attack_bwl_climber:attack_bwl_climber.cpp bwlmm_climber.h bwlmm_climber.cpp
	g++ -o attack_bwl_climber attack_bwl_climber.cpp bwlmm_climber.h bwlmm_climber.cpp
attack_twl:attack_twl_climber.cpp twl.h twl.cpp
	g++ -o attack_twl attack_twl_climber.cpp twl.h twl.cpp
clean :
	rm -rf noattack_hc_climber noattack_bwl_climber noattack_twl attack_hc_climber attack_bwl_climber attack_twl