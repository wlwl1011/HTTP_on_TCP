CPP		= g++
OUTPUT	= http_server
FLAGS	= -Wno-deprecated -g
LIBRARY	= -lpthread

SRC	 = main.o
SRC	+= ServerSocket.o
SRC	+= Session.o
SRC	+= HTTP.o
SRC	+= MyProcessor.o

all: $(OUTPUT)
clean:
	@rm $(SRC) $(OUTPUT)
cleanup: $(SRC)
	@rm *.o
$(OUTPUT): $(SRC)
	@echo [LD] $(OUTPUT)
	@$(CPP)	$(FLAGS) $(LIBRARY) $(SRC) -o $(OUTPUT)
	@make cleanup

main.o:	$($@:.o=.cpp) ServerSocket.h Session.h HTTP.h MyProcessor.h Makefile
ServerSocket.o: $($@:.o=.cpp) $($@:.o=.h) Socket.h Makefile
Session.o: $($@:.o=.cpp) $($@:.o=.h) HTTP.h Makefile
HTTP.o: $($@:.o=.cpp) $($@:.o=.h) ServerSocket.h Semaphore.h Makefile
MyProcessor.o: $($@:.o=.cpp) $($@:.o=.h) HTTP.h Makefile

.cpp.o: $< Makefile
	@echo [CPP] $@
	@$(CPP) $(FLAGS) -c $< -o $@
