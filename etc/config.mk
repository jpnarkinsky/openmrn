ifndef MAKE_INC_CONFIG_MK
MAKE_INC_CONFIG_MK := 1

CORELIBS := utils executor os dcc nmranet

LINKCORELIBS = -lnmranet -ldcc -lexecutor -lutils -los

endif # MAKE_INC_CONFIG_MK
