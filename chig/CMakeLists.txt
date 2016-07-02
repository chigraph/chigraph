
set(CHIG_PUBLIC_FILES
	include/chig/node.h
)

set(CHIG_PRIVATE_FILES

)

add_library(chig SHARED ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})
